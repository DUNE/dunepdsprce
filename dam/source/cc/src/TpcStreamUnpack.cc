 // -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcStreamUnpack.cc
 *  @brief    Access methods for the TPC data record
 *  @verbatim
 *                               Copyright 2017
 *                                    by
 *
 *                       The Board of Trustees of the
 *                    Leland Stanford Junior University.
 *                           All rights reserved.
 *
 *  @endverbatim
 *
\* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *\

   HISTORY
   -------

   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2018.10.20 jjr Replaced getTimestamps(Untrimmed) with getRange(Untrimmed)
                  This method adds the number of ticks in the trimmed/
                  untrimmed ranges in addition to their timestamps

   2018.09.11 jjr Add 
                   isTpcNormal  ()
                   isTpcDamaged ()
                   getDataFormatType ()

                   Modified all getMultiChannel methods to return false
                   if the stream is not tpcNormal.  This is a stop gap
                   method until can do the filling in of missed channels
                   properly.

                   Modified getNTicks to return the number of ticks as
                   calculated by the event window, not the indices. If
                   packets are missing, the calculation using the indices
                   will fail.

   2018.08.30 jjr Added getTimestamps(Untrimmed) to get the begin, trigger
                  and end timestamps of this TpcStream

   2018.07.26 jjr Changed ADC type to int16_t

   2018.07.23 jjr Added code for decompression
                  Corrected the calculation of the number of frames to
                  decode in all but 2-D ADCs extraction.  It was just 
                  wrong.

   2017.10.05 jjr Corrected error in getTrimmed which resulted in 1024
                  frames than it should have.

                  Eliminated printf debug statements.

   2017.10.04 jjr Changed the vector signatures from std::vector<uint16_t>
                  to TpcAdcVector.

                  Eliminated Identifier::getChannelndex.

                  Eliminated the nticks parameter from
                  getMultiChannelData(uint16_tk *adcs) const


   2017.08.29 jjr Created

\* ---------------------------------------------------------------------- */


#include "dam/TpcStreamUnpack.hh"
#include "dam/access/TpcCompressed.hh"
#include "dam/records/TpcCompressed.hh"
#include "dam/access/WibFrame.hh"
#include "TpcTrimmedRange.hh"
#include "TpcStream-Impl.hh"
#include "TpcRanges-Impl.hh"
#include "TpcToc-Impl.hh"
#include "TpcPacket-Impl.hh"
#include "TpcCompressed-Impl.hh"

#include <string>
#include <iostream>

static inline void getTrimmed (pdd::access::TpcStream const *tpc,
                               int                          *beg,
                               int                       *nticks);



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the total number of channels serviced by this fragment
  \return  The total number of channels servived by this fragment
                                                                          */
/* ---------------------------------------------------------------------- */
size_t TpcStreamUnpack::getNChannels() const
{
   size_t nchannels = 128;
   return nchannels;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Get the number of untrimmed time samples
  \retval >= 0 The number of trimmed time samples of contributor \e ictb
  \retval <  0 Error

  \param[in] ictb  Which contributor
                                                                          */
/* ---------------------------------------------------------------------- */
size_t TpcStreamUnpack::getNTicksUntrimmed () const
{
   using namespace pdd;
   record::TpcToc       const *toc = m_stream.getToc ();
   record::TpcTocHeader const *hdr = access::TpcToc      ::getHeader      (toc);
   int                       ndscs = access::TpcTocHeader::getNPacketDscs (hdr);

   return 1024*ndscs;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Get the actual number of trimmed time samples
  \return The number of trimmed time samples of contributor \e ictb

  \par
  This is usually the same as getNTicksWindow, but will differ when the
  stream is damaged.  Taking the difference getNTicksWindow - getNTicks
  will generally be 0 or a positive number. If non-zero, the number will
  be the number of frames that were dropped.

                                                                          */
/* ---------------------------------------------------------------------- */
size_t TpcStreamUnpack::getNTicks () const
{
   int    beg;
   int nticks;
   getTrimmed (&m_stream, &beg, &nticks);

   return nticks;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Get the number of time samples in the event window
  \return The number of time samples of contributor \e ictb in the 
          event window

 \par
  This is usually the same as getNTicks, but will differ when the stream
  is damaged.
                                                                          */
/* ---------------------------------------------------------------------- */
size_t TpcStreamUnpack::getNTicksWindow () const
{
   using namespace pdd;
   using namespace pdd::access;

   pdd::access::TpcStream const &stream = getStream ();
   pdd::record::TpcRanges const *ranges = stream.getRanges ();
   unsigned int                  bridge = TpcRanges::getBridge (ranges);
   pdd::access::TpcRangesWindow  window  (TpcRanges::getWindow (ranges),
                                                                bridge);

   uint32_t beg = window.getBegin ();
   uint32_t end = window.getEnd   ();

   size_t nticks = (end - beg) / 25;

   return nticks;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a compact form of the electronics identifier for a
          particular channel
  \retval Identifier::Error if the channel number is not valid
  \retval The packed identifier

   The fields of this can be accessed via \e getter's in the Identifier
   class.
                                                                          */
/* ---------------------------------------------------------------------- */
TpcStreamUnpack::Identifier TpcStreamUnpack::getIdentifier  () const
{
   pdd::record::TpcStreamHeader  const *hdr = m_stream.getHeader ();
   uint32_t csf = hdr->getCsf ();

   return TpcStreamUnpack::Identifier (csf);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Extracts the WIB's crate number from the identifier
  \return The WIB's crate number
                                                                          */
/* ---------------------------------------------------------------------- */
uint32_t TpcStreamUnpack::Identifier::getCrate () const
{
   uint32_t crate = (m_w32 >> 6) & 0x1f;
   return   crate;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Extracts the WIB's slot number from the identifier
  \return The WIB's slot number
                                                                          */
/* ---------------------------------------------------------------------- */
uint32_t TpcStreamUnpack::Identifier::getSlot () const
{
   uint32_t slot = (m_w32 >> 3) & 0x07;
   return   slot;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Extracts the WIB's fiber number from the identifier
  \return The WIB's fiber number
                                                                          */
/* ---------------------------------------------------------------------- */
uint32_t TpcStreamUnpack::Identifier::getFiber () const
{
   uint32_t fiber = (m_w32 >> 0) & 0x07;
   return   fiber;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Extracts the stream's status
  \retur  The stream's status
                                                                          */
/* ---------------------------------------------------------------------- */
uint32_t TpcStreamUnpack::getStatus () const
{
   pdd::record::TpcStreamHeader  const *hdr = m_stream.getHeader ();
   uint32_t                          status = hdr->getStatus ();
   if (status == 0)
   {
      // Check for missing frames
      using namespace pdd::access;

      pdd::record::TpcRanges const *ranges = m_stream.getRanges ();
      unsigned int                  bridge = TpcRanges::getBridge  (ranges);
      pdd::access::TpcRangesIndices indices (TpcRanges::getIndices (ranges),
                                                                     bridge);
      int32_t beg = indices.getBegin ();
      if (beg == -1)
      {
         status = 1;
      }
   }

   return status;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Tests if this a TpcNormal stream
                                                                          */
/* ---------------------------------------------------------------------- */
bool TpcStreamUnpack::isTpcNormal () const
{
   pdd::record::TpcStreamHeader const *hdr = m_stream.getHeader ();
   bool tpcNormal = hdr->isTpcNormal ();

   // --------------------------------------------------------------
   // Because of a failure to correctly mark some streams as damaged
   // perform this additional check,
   // --------------------------------------------------------------
   if (tpcNormal && getStatus () != 0) 
   {
      tpcNormal = false;
   }

   return tpcNormal;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Tests if this a TpcDamaged stream
                                                                          */
/* ---------------------------------------------------------------------- */
bool TpcStreamUnpack::isTpcDamaged () const
{
   pdd::record::TpcStreamHeader const *hdr = m_stream.getHeader ();

   // --------------------------------------------------------------
   // Because of a failure to correctly mark some streams as damaged
   // perform this additional check,
   // --------------------------------------------------------------
   bool tpcDamaged = hdr->isTpcDamaged ();

   if (!tpcDamaged && getStatus () != 0) 
   {
      tpcDamaged = true;
   }

   return tpcDamaged;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the stored data format
  \return One of the enumeration DataFormatType
                                                                          */
/* ---------------------------------------------------------------------- */
TpcStreamUnpack::DataFormatType TpcStreamUnpack::getDataFormatType () const
{
   using namespace pdd;
   using namespace pdd::access;

   record::TpcToc          const     *toc = m_stream.getToc           ();
   int                           npktDscs = TpcToc::getNPacketDscs (toc);
   record::TpcTocPacketDsc const *pktDscs = TpcToc::getPacketDscs  (toc);


   uint8_t typeMask = 0;
   for (int idx = 0; idx < npktDscs; idx++)
   {
      typeMask |= (TpcTocPacketDsc::isCompressed (pktDscs[idx])) ? 2 : 1;
   }

   if (typeMask == 1) return DataFormatType::WibFrame;
   if (typeMask == 2) return DataFormatType::Compressed;
   if (typeMask == 3) return DataFormatType::Mixed;
   else               return DataFormatType::Unknown;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Limit the number ADC tick samples to decode to what is available
  \return The minimum of the request and what is available

  \param[in]   nticks  The requested number 
  \param[in]    itick  The first requested sample 
  \param[in]  pktDscs  The array of packet descriptors
  \param[in] npktDscs  The number of packet descriptors
                                                                          */
/* ---------------------------------------------------------------------- */
static inline int limit (int                                  nticks, 
                         int                                   itick, 
                         pdd::record::TpcTocPacketDsc const *pktDscs,
                         int                                npktDscs)
{
   int nframes = 1024 * npktDscs;
   int  mticks = (nticks < 0) ? nframes : nticks;
   
   int maxFrame = mticks + itick;
   int over     = maxFrame - nframes;
   nframes      = over > 0 ? mticks - over : mticks;
   return nframes;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
void transpose (int16_t                        *const  adcs[128],
                int                                     npktDscs,
                pdd::record::TpcTocPacketDsc const      *pktDscs,
                pdd::record::TpcPacketBody   const         *pkts,
                int                                       iticks,
                int                                       nticks)
{
   using namespace pdd::access;

   // ---------------------------------------------------------
   // !!! WARNING !!!
   // ---------------
   // This routine cheats, assumining that all the data packets
   // follow consecutively, not using the packet descriptors to
   // locate the data packets
   // ---------------------------------------------------------
   int                o64 = TpcTocPacketDsc::getOffset64 (pktDscs);
   uint64_t const    *ptr = reinterpret_cast<decltype(ptr)>(pkts) + o64;
   pdd::access::WibFrame const *frames = reinterpret_cast<decltype(frames)>(ptr) 
                                       + iticks;
   pdd::access::WibFrame::transposeAdcs128xN (adcs, 0, frames, nticks);

   return;
}
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief   Extracts the ADCs in the specified range
  \retval  == true is successful
  \retval  == false is failure

  \param[out]  adcs  An array of essentially NChannels x NTicks where
                     nChannels comes from getNChannels and nTicks indicates
                     how many ticks to allocate to each array
  \param[in] nadcs   The number of elements to allocate in each each
                     channel array.  It can be
                       -# larger than the number of frames. This effectively
                          leaves some room at the end of each channel.
                          It allows one to recall this method and append
                          more ticks.
                       -# smaller than the number of frames. This will
                          limit the number of transposed frames to this
                          value.

  \param[in]    pkts  The array of TPC packets
  \param[in] pktDscs  The array of packet descriptors
  \param[in]   npkts  The number of packets and, by implication, the number
                      of packet descriptors
  \param[in]   itick  The tick number of the first sample number to be
                      extracted. Typically this represents the first ADC
                      value in the event window
  \param[in]  nticks  The number of adcs to extract.  Typically this
                      represents the number of ADCs in the event window
                                                                          */
/* ---------------------------------------------------------------------- */
inline static bool extractAdcs (int16_t                               *adcs,
                                int                                   nadcs,
                                pdd::record::TpcPacketBody   const    *pkts,
                                pdd::record::TpcTocPacketDsc const *pktDscs,
                                int                                   npkts,
                                int                                   itick,
                                int                                  nticks)
{
   using namespace pdd;
   std::string myname = "extractAdcs: ";
   bool dbg = false;

   if ( dbg ) {
     std::cout << myname << "===== nadcs=" << nadcs << ", nticks=" << nticks
               << ", itick=" << itick << ", npkts=" << npkts
               << ", output @ " << adcs << std::endl;
   }
   if ( nticks <= 0 ) return false;      // dla jan 2020  (trj mar 2020)

   if (access::TpcTocPacketDsc::isWibFrame (pktDscs))
   {
      if ( dbg ) std::cout << myname << "== Data format is WibFrame" << std::endl;
      int              o64    = access::TpcTocPacketDsc::getOffset64 (pktDscs);
      uint64_t const  *p64    = access::TpcPacketBody  ::getData (pkts) + o64;

      pdd::access::WibFrame const *frames = reinterpret_cast<decltype(frames)>(p64)
                                          + itick;
      int                         nframes = nticks;

      access::WibFrame::transposeAdcs128xN (adcs, nadcs, frames, nframes);
   }
   else if (access::TpcTocPacketDsc::isCompressed (pktDscs))
   {
      if ( dbg ) std::cout << myname << "== Data format is Compressed" << std::endl;
      pdd::record::TpcTocPacketDsc const *pktDsc = pktDscs;

      unsigned int unticks = nticks;
      for (int ipkt = 0; ipkt < npkts; pktDsc++, ipkt++)
      {
         if ( dbg ) std::cout << myname << "Packet " << ipkt << ": Output @ " << adcs << std::endl;
         int              o64 = access::TpcTocPacketDsc::getOffset64 (pktDsc);
         uint64_t const  *p64 = access::TpcPacketBody  ::getData (pkts) + o64;
         uint64_t         n64 = access::TpcTocPacketDsc::getLen64 (pktDsc);

         access::TpcCompressed cmp (p64, n64);

         unsigned int nsamples;
         if ( itick ) {
            nsamples = cmp.decompress (adcs, nadcs, itick, nticks);
         } else {
            nsamples = cmp.decompress (adcs, nadcs, nticks);
         }

         if ( dbg ) std::cout << myname << "  Nsamples: " << nsamples << std::endl;

         // DLA jan2020: Exit if decompress returns too many ticks.
         // See https://cdcvs.fnal.gov/redmine/issues/23811.
         if ( nsamples > unticks ) {
           std::cout << myname << "WARNING: Too many samples decoded." << std::endl;
           //break;
           return false;
         }
         
         nticks  -= nsamples;

         // DLA jan2020: I don't know why this is done but I carry it over from the old code.
         if ( itick && nticks > 0 ) {
           if ( dbg ) std::cout << myname << "  Setting itick = 0." << std::endl;
           itick = 0;
         }

         if (nticks <= 0) break;
         adcs     += nsamples;
      }
   }

   return true;
}
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief   Extracts the ADCs in the specified range
  \retval  == true is successful
  \retval  == false is failure

  \param[out]  adcs  An array of essentially NChannels x NTicks where
                     nChannels comes from getNChannels and nTicks indicates
                     how many ticks to allocate to each array
  \param[in] nadcs   The number of elements to allocate in each each
                     channel array.  It can be
                       -# larger than the number of frames. This effectively
                          leaves some room at the end of each channel.
                          It allows one to recall this method and append
                          more ticks.
                       -# smaller than the number of frames. This will
                          limit the number of transposed frames to this
                          value.

  \param[in]    pkts  The array of TPC packets
  \param[in] pktDscs  The array of packet descriptors
  \param[in]   npkts  The number of packets and, by implication, the number
                      of packet descriptors
  \param[in]   itick  The tick number of the first sample number to be
                      extracted. Typically this represents the first ADC
                      value in the event window
  \param[in]  nticks  The number of adcs to extract.  Typically this
                      represents the number of ADCs in the event window
                                                                          */
/* ---------------------------------------------------------------------- */
inline static bool extractAdcs (int16_t        *const                 *adcs,
                                pdd::record::TpcPacketBody   const    *pkts,
                                pdd::record::TpcTocPacketDsc const *pktDscs,
                                int                                   npkts,
                                int                                   itick,
                                int                                  nticks)
{
   using namespace pdd;

   if (access::TpcTocPacketDsc::isWibFrame (pktDscs))
   {
      transpose (adcs, npkts, pktDscs, pkts, itick, nticks);
   }
   else if (pdd::access::TpcTocPacketDsc::isCompressed (pktDscs))
   {
      record::TpcTocPacketDsc const *pktDsc = pktDscs;
      int                              iadc = 0;

      for (int ipkt = 0; ipkt < npkts; pktDsc++, ipkt++)
      {
         int              o64 = access::TpcTocPacketDsc::getOffset64 (pktDsc);
         uint64_t const  *p64 = access::TpcPacketBody  ::getData  (pkts) + o64;
         uint64_t         n64 = access::TpcTocPacketDsc::getLen64 (pktDsc);

         access::TpcCompressed cmp (p64, n64);

         int nsamples;
         if (itick)
         {
            nsamples = cmp.decompress (adcs, iadc, itick, nticks);
            nticks  -= nsamples;
            if (nticks > 0) itick = 0;
         }
         else
         {
            nsamples = cmp.decompress (adcs, iadc, nticks);
            nticks  -= nsamples;
         }

         if (nticks <= 0) break;
         iadc  += nsamples;
      }
   }

   return true;
}
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief  Extracts all the adc data in the specified range
  \retval true, if successful
  \retval false, if not successful

  \param[in]   adcs  An array of essentially NChannels x NTicks where
                     nChannels comes from getNChannels and nTicks indicates
                     how many ticks to allocate to each array
  \param[in]    tpc  Access to the Tpc stream
  \param[in]  itick  The beginning time sample tick
  \param[in]  nadcs  The number of elements to allocate in each each
                     channel array.  It can be
                       -# larger than the number of frames. This effectively
                          leaves some room at the end of each channel.
                          It allows one to recall this method and append
                          more ticks.
                       -# smaller than the number of frames. This will
                          limit the number of transposed frames to this
                          value.
                                                                          */
/* ---------------------------------------------------------------------- */
static bool getMultiChannelDataBase (int16_t                     *adcs,
                                     pdd::access::TpcStream const *tpc,
                                     int                         itick,
                                     int                        nticks)
{
   using namespace pdd;
   using namespace pdd::access;


   record::TpcToc          const     *toc = tpc->getToc               ();
   record::TpcPacket       const  *pktRec = tpc->getPacket            ();

   int                           npktDscs = TpcToc   ::getNPacketDscs    (toc);
   record::TpcTocPacketDsc const *pktDscs = TpcToc   ::getPacketDscs     (toc);
   record::TpcPacketBody   const    *pkts = TpcPacket::getBody        (pktRec);



   // ---------------------------------------------------------
   // Loop over all the 128 channel contributors.
   // This loop cheats in that it knows the frames from all the
   // contributors are stored contigously.
   // ---------------------------------------------------------

   int nframes = limit (nticks, itick, pktDscs, npktDscs);
   bool   okay = extractAdcs (adcs, nticks, pkts, pktDscs, npktDscs, itick, nframes);
   return okay;

}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Extracts and transposes the data
  \retval true, if successful
  \retval false, if not successful

  \param[in]   adcs  An array of pointers each pointing to array that is
                     at least nticks in size.
  \param[in] nticks  The number of elements to allocate in each each
                     channel array.  It can be
                       -# larger than the number of frames. This effectively
                          leaves some room at the end of each channel.
                          It allows one to recall this method and append
                          more ticks.
                       -# smaller than the number of frames. This will
                          limit the number of transposed frames to this
                          value.
                                                                          */
/* ---------------------------------------------------------------------- */
static bool getMultiChannelDataBase (int16_t               *const *adcs,
                                     pdd::access::TpcStream const  *tpc,
                                     int                          itick,
                                     int                         nticks)
{
   using namespace pdd;
   using namespace pdd::access;

   record::TpcToc          const     *toc = tpc->getToc               ();
   record::TpcPacket       const  *pktRec = tpc->getPacket            ();

   int                           npktDscs = TpcToc   ::getNPacketDscs    (toc);
   record::TpcTocPacketDsc const *pktDscs = TpcToc   ::getPacketDscs     (toc);
   record::TpcPacketBody   const    *pkts = TpcPacket::getBody        (pktRec);


   int nframes = limit       (nticks, itick, pktDscs, npktDscs);
   bool   okay = extractAdcs (adcs,    pkts, pktDscs, npktDscs, itick, nframes);

   return okay;
}
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- */
static bool getMultiChannelDataBase (std::vector<TpcAdcVector>      &adcs,
                                     pdd::access::TpcStream const    *tpc,
                                     int                            itick,
                                     int                           nticks)
{
   using namespace pdd;
   using namespace pdd::access;

   record::TpcToc          const     *toc = tpc->getToc   ();
   record::TpcPacket       const  *pktRec = tpc->getPacket();

   int                           npktDscs = TpcToc   ::getNPacketDscs (toc);
   record::TpcTocPacketDsc const *pktDscs = TpcToc   ::getPacketDscs  (toc);
   record::TpcPacketBody   const    *pkts = TpcPacket::getBody     (pktRec);


   int16_t *pAdcs[128];

   // -----------------------------------------------
   // Limit the number of frames to what is available
   // -----------------------------------------------
   int nframes = limit (nticks, itick, pktDscs, npktDscs);
   int  nchans = adcs.capacity ();


   // ------------------------------------------------------
   // Extract an array of pointers to the channel ADC arrays
   // ------------------------------------------------------
   for (int ichan = 0; ichan < nchans; ++ichan)
   {
      // Ensure each vector can handle the request frames
      adcs [ichan].reserve (nframes);
      pAdcs[ichan] = adcs[ichan].data ();
   }


   bool    okay = extractAdcs (pAdcs, pkts, pktDscs, npktDscs, itick, nframes);
   return  okay;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Extracts all the untrimmed data
  \retval true, if successful
  \retval false, if not successful

  \param[in]   adcs  An array of essentially NChannels x NTicks where
                     nChannels comes from getNChannels and nTicks indicates
                     how many ticks to allocate to each array
  \param[in] nticks  The number of elements to allocate in each each
                     channel array.  It can be
                       -# larger than the number of frames. This effectively
                          leaves some room at the end of each channel.
                          It allows one to recall this method and append
                          more ticks.
                       -# smaller than the number of frames. This will
                          limit the number of transposed frames to this
                          value.
                                                                          */
/* ---------------------------------------------------------------------- */
bool TpcStreamUnpack::getMultiChannelDataUntrimmed (int16_t  *adcs,
                                                    int     nticks) const
{
   // -----------------------------------
   // Only process normal streams for now
   // -----------------------------------
   if (!isTpcNormal ()) return false;


   bool ok = getMultiChannelDataBase (adcs, &m_stream, 0, nticks);
   return ok;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Extracts all the untrimmed data
  \retval true, if successful
  \retval false, if not successful

  \param[in]   adcs  An array of pointers each pointing to array that is
                     at least nticks in size.
  \param[in] nticks  The number of elements to allocate in each each
                     channel array.  It can be
                       -# larger than the number of frames. This effectively
                          leaves some room at the end of each channel.
                          It allows one to recall this method and append
                          more ticks.
                       -# smaller than the number of frames. This will
                          limit the number of transposed frames to this
                          value.
                                                                          */
/* ---------------------------------------------------------------------- */
bool TpcStreamUnpack::getMultiChannelDataUntrimmed(int16_t **adcs,
                                                   int     nticks) const
{
   // -----------------------------------
   // Only process normal streams for now
   // -----------------------------------
   //// if (!isTpcNormal ()) return false;


   bool ok = getMultiChannelDataBase (adcs, &m_stream, 0, nticks);
   return ok;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Extracts all the untrimmed data
  \retval true, if successful
  \retval false, if not successful

  \param[in]   adcs  An array of essentially NChannels x NTicks where
                     nChannels comes from getNChannels and nTicks indicates
                     how many ticks to allocate to each array
  \param[in] nticks  The number of elements to allocate in each each
                     channel array.  It can be
                       -# larger than the number of frames. This effectively
                          leaves some room at the end of each channel.
                          It allows one to recall this method and append
                          more ticks.
                       -# smaller than the number of frames. This will
                          limit the number of transposed frames to this
                          value.
                                                                          */
/* ---------------------------------------------------------------------- */
bool TpcStreamUnpack::
     getMultiChannelDataUntrimmed (std::vector<TpcAdcVector> &adcs) const

{
   // -----------------------------------
   // Only process normal streams for now
   // -----------------------------------
   //// if (!isTpcNormal ()) return false;


   bool ok = getMultiChannelDataBase (adcs, &m_stream, 0, -1);
   return ok;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
static inline void getTrimmed (pdd::access::TpcStream const *tpc,
                               int                          *beg,
                               int                       *nticks)
{
   using namespace pdd;
   using namespace pdd::access;
   
   TpcTrimmedRange trimmed (*tpc);
   *beg         = trimmed.m_beg.m_wibOff;
   *nticks      = trimmed.m_nticks;

   return;
}
/* ---------------------------------------------------------------------- */




// method to unpack all channels in a fragment
bool TpcStreamUnpack::getMultiChannelData (int16_t *adcs) const
{
   // -----------------------------------
   // Only process normal streams for now
   // -----------------------------------
   /// if (!isTpcNormal ()) return false;

   int    beg;
   int nticks;

   getTrimmed (&m_stream, &beg, &nticks);
   bool ok = getMultiChannelDataBase (adcs, &m_stream, beg, nticks);
   return ok;
}

bool TpcStreamUnpack::getMultiChannelData (int16_t **adcs) const
{
   // -----------------------------------
   // Only process normal streams for now
   // -----------------------------------
   /// if (!isTpcNormal ()) return false;

   int    beg;
   int nticks;

   getTrimmed (&m_stream, &beg, &nticks);
   bool ok = getMultiChannelDataBase (adcs, &m_stream, beg, nticks);
   return ok;
}

bool TpcStreamUnpack::getMultiChannelData(std::vector<TpcAdcVector> &adcs) const
{
   // -----------------------------------
   // Only process normal streams for now
   // -----------------------------------
   //// if (!isTpcNormal ()) return false;

   int    beg;
   int nticks;

   getTrimmed (&m_stream, &beg, &nticks);
   bool ok = getMultiChannelDataBase (adcs, &m_stream, beg, nticks);
   return ok;
}




/* ---------------------------------------------------------------------- */
static pdd::record::WibFrame const
*locateWibFrames (pdd::access::TpcStream const &tpc) __attribute__ ((unused));


static pdd::record::WibFrame const
           *locateWibFrames (pdd::access::TpcStream const &tpc)

{
   using namespace pdd;
   using namespace pdd::access;

   record::TpcToc          const     *toc = tpc.getToc                ();
   record::TpcPacket       const  *pktRec = tpc.getPacket             ();
   record::TpcPacketBody   const    *pkts = TpcPacket::getBody        (pktRec);
   record::TpcTocBody      const *tocBody = TpcToc::getBody           (toc);
   record::TpcTocPacketDsc const *pktDscs = TpcTocBody::getPacketDscs (tocBody);


   // --------------------------------------------------------------
   // !!! WARNING !!!
   // ---------------
   // This not only assumes, but demands that the WibFrames are all
   // consecutive.
   // -------------------------------------------------------------
   int                           o64 = TpcTocPacketDsc::getOffset64 (pktDscs);
   uint64_t const               *ptr = reinterpret_cast<decltype(ptr)>(pkts) + o64;
   pdd::record::WibFrame const *frame = reinterpret_cast<decltype(frame)>(ptr);

   return frame;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return the timestamp of the beginning of the untrimmed event

  \return The timestamp of the beginning of the untrimmed event.
                                                                          */
/* ---------------------------------------------------------------------- */
TpcStreamUnpack::timestamp_t TpcStreamUnpack::getTimeStampUntrimmed () const
{
   using namespace pdd;
   using namespace pdd::access;

   record::TpcRanges           const *ranges = m_stream.getRanges ();
   unsigned int                       bridge = TpcRanges::getBridge     (ranges);
   record::TpcRangesTimestamps const     *ts = TpcRanges::getTimestamps (ranges);

   timestamp_t begin = TpcRangesTimestamps::getBegin (ts, bridge);
   return      begin;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the timestamps of the beginning of the untrimmed data
          along with the number of ticks/samples in the untrimmed dat.

  \retval == 0, no errors
  \retval != 0, error

  \param[out]   nticks  The number of ticks/samples in the untrimmed data
                        Due to missing/dropped or otherwise not perfectly
                        time sequential WIB frame data, this may not
                        correspond to the calculated number between 
                        \a begin and \a end.
  \param[out]    begin  The beginning time stamp
  \param[out]     end   The trigger   time stamp
                                                                          */
/* ---------------------------------------------------------------------- */
uint32_t TpcStreamUnpack::getRangeUntrimmed (size_t       *nticks,
                                             timestamp_t   *begin,
                                             timestamp_t     *end) const
{
   using namespace pdd;
   using namespace pdd::access;

   record::TpcRanges           const *ranges = m_stream.getRanges ();
   unsigned int                       bridge = TpcRanges::getBridge     (ranges);
   record::TpcRangesTimestamps const     *ts = TpcRanges::getTimestamps (ranges);

   *begin  = TpcRangesTimestamps::getBegin   (ts, bridge);
   *end    = TpcRangesTimestamps::getEnd     (ts, bridge);
   *nticks = getNTicksUntrimmed ();

   return 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return the timestamp of the beginning of the trimmed event

  \return The timestamp of the beginning of the untrimmed event.

                                                                          */
/* ---------------------------------------------------------------------- */
TpcStreamUnpack::timestamp_t TpcStreamUnpack::getTimeStamp () const
{
   using namespace pdd;
   using namespace pdd::access;

   TpcTrimmedRange trimmed (m_stream);

   timestamp_t begin = trimmed.m_beg.m_wibTs;
   return begin;
}
/* ---------------------------------------------------------------------- */






/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the timestamps of the beginning of the untrimmed data
          along with the number of ticks/samples in the untrimmed dat.

  \retval == 0, no errors
  \retval != 0, error

  \param[out]   nticks  The number of ticks/samples in the untrimmed data
                        Due to missing/dropped or otherwise not perfectly
                        time sequential WIB frame data, this may not
                        correspond to the calculated number between 
                        \a begin and \a end.
  \param[out]    begin  The beginning time stamp
  \param[out]     end   The trigger   time stamp
                                                                          */
/* ---------------------------------------------------------------------- */
uint32_t TpcStreamUnpack::getRange (size_t      *nticks,
                                    timestamp_t  *begin,
                                    timestamp_t    *end) const
{
   using namespace pdd;
   using namespace pdd::access;

   TpcTrimmedRange trimmed (m_stream);

   *begin  = trimmed.m_beg.m_wibTs;
   *end    = trimmed.m_end.m_wibTs;
   *nticks = trimmed.m_nticks;


   return 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return the timestamp of the beginning of the trimmed event for
          the specified channel.

  \retval == 0, no errors
  \retval != 0, errors

  \param[out]   nticks  The predicted number of ticks within the window
  \param[out]    begin  The beginning time stamp
  \param[out]  trigger  The trigger   time stamp
  \param[out]     end   The trigger   time stamp

  \par
   The number of ticks/samples is strictly a calculation based on the 
   time span from \a end to \a begin. The actual number of frames, which
   is given by getRangeUntrimmed, may differ because of dropped/missing
   or otherwise not perfectly time sequential WIB frames.
                                                                          */
/* ---------------------------------------------------------------------- */
uint32_t TpcStreamUnpack::getRangeWindow (size_t       *nticks,
                                          timestamp_t   *begin,
                                          timestamp_t *trigger,
                                          timestamp_t     *end) const
{
   using namespace pdd;
   using namespace pdd::access;

   record::TpcRanges           const *ranges = m_stream.getRanges   ();
   unsigned int                       bridge = TpcRanges::getBridge (ranges);
   record::TpcRangesWindow     const *window = TpcRanges::getWindow (ranges);

   *begin   = TpcRangesWindow::getBegin   (window, bridge);
   *trigger = TpcRangesWindow::getTrigger (window, bridge);
   *end     = TpcRangesWindow::getEnd     (window, bridge);
   *nticks  = (end - begin) / 25;

   return 0;
}
/* ---------------------------------------------------------------------- */


