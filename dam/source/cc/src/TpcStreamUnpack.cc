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
   2017.08.29 jjr Created
  
\* ---------------------------------------------------------------------- */

#include "dam/TpcStreamUnpack.hh"
#include "dam/TpcRecords.hh"

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
   pdd::fragment::Toc const *toc = m_rawStream.getToc ();
   int                     ndscs = toc->getNDscs      ();

   return 1024*ndscs;
}
/* ---------------------------------------------------------------------- */   



/* ---------------------------------------------------------------------- */
static pdd::fragment::RangesBody::Descriptor::Indices const 
         *locateIndices (pdd::access::TpcStream const *tpc) 
{
   using namespace pdd::fragment;
   Ranges                          const *rng = tpc->getRanges     ();
   RangesBody                      const *bdy = rng->getBody       ();
   RangesBody::Descriptor          const *dsc = bdy->getDescriptor ();
   RangesBody::Descriptor::Indices const *ind = dsc->getIndices    ();

   return ind;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Get the number of trimmed time samples
  \return The number of trimmed time samples of contributor \e ictb

                                                                          */
/* ---------------------------------------------------------------------- */
size_t TpcStreamUnpack::getNTicks () const
{
   auto indices = locateIndices (&m_rawStream);

   uint32_t beg = indices->getBegin ();
   uint32_t end = indices->getEnd   ();

   int   begPkt = indices->getPacket (beg);
   int   begOff = indices->getOffset (beg);

   int   endPkt = indices->getPacket (end);
   int   endOff = indices->getOffset (end);

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
   pdd::fragment::TpcStream  const *rawStream = m_rawStream.getRecord ();
   uint32_t csf = rawStream->getCsf ();

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

  \brief  Extracts the WIB's channel number from the identifier
  \return The WIB's fiber number
                                                                          */
/* ---------------------------------------------------------------------- */
uint32_t TpcStreamUnpack::Identifier::getChannelIndex () const
{
   uint32_t channel = m_w32 & 0xffff;
   return   channel;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
void transpose (uint16_t                                *adcs[128], 
                int                                       npktDscs,
                pdd::fragment::TocBody::PacketDsc   const *pktDscs,
                pdd::fragment::TpcPacketBody const           *pkts,
                int                                         iticks,
                int                                         nticks) 
{
   using namespace pdd::fragment;

   // ---------------------------------------------------------
   // !!! WARNING !!!
   // ---------------
   // This routine cheats, assumining that all the data packets
   // follow consecutively, not using the packet descriptors to 
   // locate the data packets
   // ---------------------------------------------------------
   int      o64 = pktDscs[0].getOffset64 ();

   uint64_t const    *ptr = reinterpret_cast<decltype(ptr)>(pkts) + o64;
   WibFrame const *frames = reinterpret_cast<decltype(frames)>(ptr) + iticks;
   WibFrame::transposeAdcs128x32N (adcs, 0, frames, nticks);

   return;
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
static bool getMultiChannelDataBase (uint16_t                    *adcs,
                                     pdd::access::TpcStream const *tpc,
                                     int                         itick,
                                     int                        nticks)
{
   using namespace pdd::fragment;
   Toc               const      *toc = tpc->getToc            ();
   TocBody           const  *tocBody = toc->getBody           ();
   TpcPacket          const  *pktRec = tpc->getPacket         ();
   int                         npkts = toc->getNDscs          ();
   TpcPacketBody      const    *pkts = pktRec->getBody        ();
   TocBody::PacketDsc const *pktDscs = tocBody->getPacketDscs ();


   
   // ---------------------------------------------------------
   // Loop over all the 128 channel contributors.
   // This loop cheats in that it knows the frames from all the
   // contributors are stored contigously.
   // ---------------------------------------------------------
   int  nframes = 1024 * npkts;
   auto  pktDsc = *pktDscs;
   int      o64 = pktDsc.getOffset64 ();
      
   uint64_t const *ptr    = reinterpret_cast<decltype(ptr)   >(pkts) + o64;
   WibFrame const *frames = reinterpret_cast<decltype(frames)>(ptr) + itick;

   int mticks = (nticks < 0) ? nframes : nticks;


   printf ("Transposing nticks = %d mticks = %d itick = %d\n", nticks, mticks, itick);

   // -----------------------------------------------
   // Limit the number of frames to what is available
   // -----------------------------------------------
   int maxFrame = mticks + itick;
   int over     = maxFrame - nframes;
   nframes      = over > 0 ? mticks - over : mticks;

   printf ("Transposing nframes = %d\n", nframes);

   WibFrame::transposeAdcs128x32N (adcs, nticks, frames, nframes);
   adcs += 128 * nticks;


   return true;
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
static bool getMultiChannelDataBase (uint16_t                   **adcs,
                                     pdd::access::TpcStream const *tpc,
                                     int                        iticks,
                                     int                        nticks)

{
   using namespace pdd::fragment;

   Toc           const            *toc = tpc->getToc            ();
   TpcPacket     const         *pktRec = tpc->getPacket         ();
   int                        npktDscs = toc->getNDscs          ();
   TpcPacketBody const           *pkts = pktRec->getBody        ();
   TocBody const              *tocBody = toc->getBody           ();
   TocBody::PacketDsc   const *pktDscs = tocBody->getPacketDscs ();


   int nframes = 1024 * npktDscs;
   int  mticks = (nticks < 0) ? nframes : nticks;

   // -------------------------------------------
   // Do not decode more than nticks time samples
   // -------------------------------------------
   int maxFrame = mticks + iticks;
   int over     = maxFrame - nframes;
   if (over) nframes -= over;

   transpose (adcs, npktDscs, pktDscs, pkts, iticks, nframes);


   return true;
}
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- */
static bool getMultiChannelDataBase (std::vector<std::vector<uint16_t>> &adcs, 
                                     pdd::access::TpcStream const        *tpc,
                                     int                               iticks,
                                     int                               nticks)
{
   using namespace pdd::fragment;
   Toc     const                  *toc = tpc->getToc            ();
   TocBody const              *tocBody = toc->getBody           ();
   TpcPacket     const         *pktRec = tpc->getPacket         ();
   TpcPacketBody const           *pkts = pktRec->getBody        ();
   int                        npktDscs = toc->getNDscs          ();
   TocBody::PacketDsc   const *pktDscs = tocBody->getPacketDscs ();
   int ichan  = 0;


   uint16_t    *pAdcs[128];

   int nframes = 1024 * npktDscs;
   int  mticks = (nticks < 0) ? nframes : nticks;

   int maxFrame = mticks + iticks;
   int over     = maxFrame - nframes;
   if (over) nframes -= over;
   

   for (; ichan < 128; ++ichan)
   {
      adcs [ichan].resize (nframes);
      pAdcs[ichan] = adcs[ichan].data ();
   }


   // -----------------------------------------------
   // Limit the number of frames to what is available
   // -----------------------------------------------
   transpose (pAdcs, npktDscs, pktDscs, pkts, iticks, nframes);

   return true;
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
bool TpcStreamUnpack::getMultiChannelDataUntrimmed (uint16_t  *adcs,
                                                      int    nticks) const
{
   bool ok = getMultiChannelDataBase (adcs, &m_rawStream, 0, nticks);
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
bool TpcStreamUnpack::getMultiChannelDataUntrimmed(uint16_t **adcs,
                                                     int    nticks) const
{
   bool ok = getMultiChannelDataBase (adcs, &m_rawStream, 0, nticks);
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
     getMultiChannelDataUntrimmed (std::vector<std::vector<uint16_t>> &adcs) const
                                               
{
   bool ok = getMultiChannelDataBase (adcs, &m_rawStream, 0, -1);
   return ok;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
static inline void getTrimmed (pdd::access::TpcStream const *tpc, 
                               int                          *beg,
                               int                       *nticks)
{
   auto    indices = locateIndices (tpc);

   uint32_t begIdx = indices->getBegin ();
   uint32_t endIdx = indices->getEnd   ();

   int      begPkt = indices->getPacket (begIdx);
   int      begOff = indices->getOffset (begIdx);

   int      endPkt = indices->getPacket (endIdx);
   int      endOff = indices->getOffset (endIdx);

   *nticks = 1024 * (endPkt - begPkt + 1) - begOff + endOff;
   *beg    = 1024 * begPkt + begOff;

   return;
}
/* ---------------------------------------------------------------------- */

   


// method to unpack all channels in a fragment
bool TpcStreamUnpack::getMultiChannelData(uint16_t *adcs, int nticks) const
{
   int beg;
   int mticks;

   getTrimmed (&m_rawStream, &beg, &mticks);
   bool ok = getMultiChannelDataBase (adcs, &m_rawStream, beg, nticks);
   return ok;
}

bool TpcStreamUnpack::getMultiChannelData(uint16_t **adcs) const
{
   int beg;
   int nticks;
   getTrimmed (&m_rawStream, &beg, &nticks);

   bool ok = getMultiChannelDataBase (adcs, &m_rawStream, beg, nticks);
   return ok;
}

bool TpcStreamUnpack::getMultiChannelData(std::vector<std::vector<uint16_t>> &adcs) const
{
   int beg;
   int nticks;
   getTrimmed (&m_rawStream, &beg, &nticks);

   bool ok = getMultiChannelDataBase (adcs, &m_rawStream, beg, nticks);
   return ok;
}




/* ---------------------------------------------------------------------- */
static pdd::fragment::WibFrame const 
*locateWibFrames (pdd::access::TpcStream const &tpc) __attribute__ ((unused));


static pdd::fragment::WibFrame const 
           *locateWibFrames (pdd::access::TpcStream const &tpc)

{
   using namespace pdd::fragment;

   Toc const                      *toc = tpc.getToc             ();
   TpcPacket          const    *pktRec = tpc.getPacket          ();
   TpcPacketBody      const      *pkts = pktRec->getBody        ();
   TocBody            const   *tocBody = toc->getBody           ();
   TocBody::PacketDsc const   *pktDscs = tocBody->getPacketDscs ();


   // --------------------------------------------------------------
   // !!! WARNING !!!
   // ---------------
   // This not only assumes, but demands that the WibFrames are all
   // consecutive. 
   // -------------------------------------------------------------
   int    o64 = pktDscs[0].getOffset64 ();


   uint64_t const *ptr   = reinterpret_cast<decltype(ptr)>(pkts) + o64;
   pdd::fragment::WibFrame const *frame = reinterpret_cast<decltype(frame)>(ptr);

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
   using namespace pdd::fragment;
   Ranges                             const *ranges = m_rawStream.getRanges ();
   RangesBody                         const   *body = ranges->getBody       ();
   RangesBody::Descriptor             const    *dsc = body->getDescriptor   ();
   RangesBody::Descriptor::Timestamps const     *ts = dsc->getTimestamps    ();
   
   timestamp_t begin = ts->getBegin ();
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
   using namespace pdd::fragment;

   Ranges             const *ranges = m_rawStream.getRanges ();
   RangesBody         const   *body = ranges->getBody ();
   RangesBody::Window const *window = body->getWindow ();
   timestamp_t                begin = window->getBegin ();
   return      begin;
}
/* ---------------------------------------------------------------------- */
