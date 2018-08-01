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
#include "TpcStream-Impl.hh"
#include "TpcRanges-Impl.hh"
#include "TpcToc-Impl.hh"
#include "TpcPacket-Impl.hh"
#include "TpcCompressed-Impl.hh"

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

  \brief  Get the number of trimmed time samples
  \return The number of trimmed time samples of contributor \e ictb

                                                                          */
/* ---------------------------------------------------------------------- */
size_t TpcStreamUnpack::getNTicks () const
{
   using namespace pdd;
   using namespace pdd::access;

   pdd::access::TpcStream const &stream = getStream ();
   pdd::record::TpcRanges const *ranges = stream.getRanges ();
   unsigned int                  bridge = TpcRanges::getBridge  (ranges);
   pdd::access::TpcRangesIndices indices (TpcRanges::getIndices (ranges),
                                                                 bridge);

   uint32_t beg = indices.getBegin ();
   uint32_t end = indices.getEnd   ();

   int   begPkt = indices.getPacket (beg);
   int   begOff = indices.getOffset (beg);

   int   endPkt = indices.getPacket (end);
   int   endOff = indices.getOffset (end);

   size_t nticks = (endPkt - begPkt) * 1024
                 -  begOff + endOff;

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

   return status;
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

   if (access::TpcTocPacketDsc::isWibFrame (pktDscs))
   {
      int              o64    = access::TpcTocPacketDsc::getOffset64 (pktDscs);
      uint64_t const  *p64    = access::TpcPacketBody  ::getData (pkts) + o64;

      pdd::access::WibFrame const *frames = reinterpret_cast<decltype(frames)>(p64)
                                          + itick;
      int                         nframes = nticks;

      access::WibFrame::transposeAdcs128xN (adcs, nadcs, frames, nframes);
   }
   else if (access::TpcTocPacketDsc::isCompressed (pktDscs))
   {
      pdd::record::TpcTocPacketDsc const *pktDsc = pktDscs;

      for (int ipkt = 0; ipkt < npkts; pktDsc++, ipkt++)
      {
         int              o64 = access::TpcTocPacketDsc::getOffset64 (pktDsc);
         uint64_t const  *p64 = access::TpcPacketBody  ::getData (pkts) + o64;
         uint64_t         n64 = access::TpcTocPacketDsc::getLen64 (pktDsc);

         access::TpcCompressed cmp (p64, n64);

         int nsamples;
         if (itick)
         {
            nsamples = cmp.decompress (adcs, nadcs, itick, nticks);
            nticks  -= nsamples;
            if (nticks > 0) itick = 0;
         }
         else
         {
            nsamples = cmp.decompress (adcs, nadcs, nticks);
            nticks  -= nsamples;
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

   pdd::record::TpcRanges const *ranges = tpc->getRanges ();
   unsigned int                  bridge = TpcRanges::getBridge  (ranges);
   pdd::access::TpcRangesIndices indices (TpcRanges::getIndices (ranges),
                                                                 bridge);

   uint32_t begIdx = indices.getBegin ();
   uint32_t endIdx = indices.getEnd   ();

   int      begPkt = indices.getPacket (begIdx);
   int      begOff = indices.getOffset (begIdx);

   int      endPkt = indices.getPacket (endIdx);
   int      endOff = indices.getOffset (endIdx);


   // -----------------------------------------------------------
   // 2017.10.05 -- jjr
   // -----------------
   // Corrected the nticks calculation; it previously incorrectly
   // was (endPkt - begPkt + 1).
   // -----------------------------------------------------------
   *nticks = 1024 * (endPkt - begPkt) - begOff + endOff;
   *beg    = 1024 *  begPkt + begOff;

   return;
}
/* ---------------------------------------------------------------------- */




// method to unpack all channels in a fragment
bool TpcStreamUnpack::getMultiChannelData (int16_t *adcs) const
{
   int    beg;
   int nticks;

   getTrimmed (&m_stream, &beg, &nticks);
   bool ok = getMultiChannelDataBase (adcs, &m_stream, beg, nticks);
   return ok;
}

bool TpcStreamUnpack::getMultiChannelData (int16_t **adcs) const
{
   int    beg;
   int nticks;

   getTrimmed (&m_stream, &beg, &nticks);
   bool ok = getMultiChannelDataBase (adcs, &m_stream, beg, nticks);
   return ok;
}

bool TpcStreamUnpack::getMultiChannelData(std::vector<TpcAdcVector> &adcs) const
{
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

  \brief  Return the timestamp of the beginning of the untrimmed event for
          the specified channel.

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

  \brief  Return the timestamp of the beginning of the trimmed event for
          the specified channel.

  \return The timestamp of the beginning of the untrimmed event.

  \param[in] ichannel The target ichannel
                                                                          */
/* ---------------------------------------------------------------------- */
TpcStreamUnpack::timestamp_t TpcStreamUnpack::getTimeStamp () const
{
   using namespace pdd;
   using namespace pdd::access;

   record::TpcRanges       const *ranges = m_stream.getRanges        ();
   unsigned int                   bridge = TpcRanges::getBridge      (ranges);
   record::TpcRangesWindow const *window = TpcRanges::getWindow      (ranges);
   timestamp_t                     begin = TpcRangesWindow::getBegin (window,
                                                                      bridge);

   return      begin;
}
/* ---------------------------------------------------------------------- */
