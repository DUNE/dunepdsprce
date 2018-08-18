// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     PdReaderTest.cc
 *  @brief    Tests the PROTO-DUNE RCEs data access methods by reading and
 *            decoding a binary test file.
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
   2017.12.19 jjr Removed error message that checked to timestamp 
                  mismatches on the first WIB frame. There is no prediction
                  on the first frame, so there was an extraneous message.
   2017.11.10 jjr Check for bad record read. 
   2017.11.10 jjr Corrected an error causing the pointer to the ADCs in
                  the TpcPacket record body to be incorrect.  This was
                  because of a rather informal method of calculating the
                  pointer. This was replaced by a more formal method that
                  was added to the TpcPacket class.

                  There was also some tweaking of the output format in
                  an attempt to make it more useful and easier to read.

   2017.10.31 jjr Added documentation. Name -> PdReaderTest.  The 
                  previous name, reader was too generic.
   2017.10.05 jjr Fixed error in data read.  Previously was passing the
                  the length of the data as bytes; the method expected
                  the length in terms of 64-bit words.
   2017.10.04 jjr Eliminated the number of adcs parameter from the
                  getMultiChannel call.
   2017.07.27 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "Reader.hh"
#include "dam/HeaderFragmentUnpack.hh"
#include "dam/DataFragmentUnpack.hh"
#include "dam/TpcFragmentUnpack.hh"
#include "dam/TpcStreamUnpack.hh"
#include "dam/access/WibFrame.hh"


#include <cstdlib>
#include <cstring>
#include <cinttypes>
#include <cstdio>


/* ---------------------------------------------------------------------- *//*!

  \class  Prms
  \brief  The configuration parameters
                                                                          */
/* ---------------------------------------------------------------------- */
class Prms
{
public:
   Prms (int argc, char *const argv[]);

public:
   char const *m_ofilename;   /*!< Output file name                       */
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Constructor to extract the readers command line parameters

  \param[in] argc The count  of the command line parameters
  \param[in] argv The vector of the command line parameters
                                                                          */
/* ---------------------------------------------------------------------- */
Prms::Prms (int argc, char *const argv[])
{
   m_ofilename = argv[1];
}
/* ---------------------------------------------------------------------- */



static void processFragment  (uint64_t        const *buf);
static void process          (TpcStreamUnpack const *tpc);

static void processRaw       (TpcStreamUnpack const *tpc);

/* ---------------------------------------------------------------------- */
int main (int argc, char *const argv[])
{
   static size_t const MaxBuf = 10 * 1024 * 1024;

   // -----------------------------------
   // Extract the command line parameters
   // -----------------------------------
   Prms     prms (argc, argv);
   Reader reader (prms.m_ofilename);


   // -----------------------------------
   // Open the file to process
   // -----------------------------------
   int  err = reader.open ();
   if (err) 
   {
      reader.report (err);
      exit (-1);
   }

   uint64_t *buf = reinterpret_cast<decltype (buf)>(malloc (MaxBuf));

   while (1)
   {
      HeaderFragmentUnpack *header = HeaderFragmentUnpack::assign (buf);
      ssize_t               nbytes = reader.read (header);

      if (nbytes <= 0)
      {
         if (nbytes == 0) 
         {
            reader.close ();
            break;
         }

         else
         {
            exit (-1);
         }
      }

      // ----------------------------------------------------
      // Get the number of 64-bit words in this data fragment
      // and read the body of the data fragment.
      // ----------------------------------------------------
      uint64_t n64 = header->getN64 ();
      ssize_t nread = reader.read (buf, n64, nbytes);
      if (nread <= 0)
      {
         printf ("Error: Incomplete or corrupted record\n");
         break;
      }

      processFragment (buf);
   }


   printf ("Closing\n");
   reader.close ();
               
   return 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Process an RCE data fragment

  \param[in] buf The data fragment to process
                                                                          */
/* ---------------------------------------------------------------------- */
static void processFragment (uint64_t const *buf)
{
   // -----------------------------------------------
   // Interpret this as a generic RCE Fragment Header
   // -----------------------------------------------
   HeaderFragmentUnpack const header (buf);


   // ----------------------------
   // Is this an RCE Data Fragment
   // ----------------------------
   if (header.isData ())
   {
      // --------------------------------------------
      // Turn the buffer into a generic data fragment
      // --------------------------------------------
      DataFragmentUnpack df (buf);
      df.print ();


      // ----------------------------------------------
      // Is this record an error-free TPC data fragment
      // ---------------------------------------------
      if (df.isTpcNormal () || df.isTpcDamaged ())
      {
         char const *tpcType = df.isTpcNormal  () ? "TpcNormal"
                             : df.isTpcDamaged () ? "TpcDamaged"
                             : "TpcUnknown";
         
         printf ("Have TpcStream data type: %s\n", tpcType);

         // ------------------------------------------------
         // Having verified that this is a TPC data fragment
         // can now convert it an analyze it as such
         // ------------------------------------------------
         TpcFragmentUnpack tpcFragment (df);

         df.printHeader  ();
         df.printTrailer ();
         // --------------------------------------------
         // Get the number and loop over the TPC streams
         // --------------------------------------------
         int nstreams = tpcFragment.getNStreams ();

         for (int istream = 0; istream < nstreams; ++istream)
         {
            TpcStreamUnpack const *tpcStream = tpcFragment.getStream (istream);

            printf ("\nTpcStream: %d/%d  -- using high level access methods\n", 
                    istream, nstreams);
            process    (tpcStream);


            printf ("\nTpcStream: %d/%d -- using low  level access methods\n",
                    istream, nstreams);
            processRaw (tpcStream);
         }
      }
      else if (df.isTpcDamaged ())
      {
         printf ("TpcStream is damaged\n");

         // ------------------------------------------------
         // Having verified that this is a TPC data fragment
         // can now convert it an analyze it as such
         // ------------------------------------------------
         TpcFragmentUnpack tpcFragment (df);

         df.printHeader  ();
         df.printTrailer ();
         // --------------------------------------------
         // Get the number and loop over the TPC streams
         // --------------------------------------------
         int nstreams = tpcFragment.getNStreams ();

         for (int istream = 0; istream < nstreams; ++istream)
         {
            TpcStreamUnpack const *tpcStream = tpcFragment.getStream (istream);

            // Only proccess with low level methods
            printf ("\nTpcStream: %d/%d -- using low  level access methods\n",
                    istream, nstreams);
            processRaw (tpcStream);
         }


      }
      
      df.printTrailer ();
   }
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Exercise the standard interface to the TPC Stream data

  \param[in]  tpcStream  The target TPC stream.
                                                                          */
/* ---------------------------------------------------------------------- */
static void process (TpcStreamUnpack const *tpcStream)
{
   // ----------------------------------
   // Get the identifier for this stream 
   // This is the WIB's Crate.Slot.Fiber
   // ----------------------------------
   TpcStreamUnpack::Identifier id = tpcStream->getIdentifier ();
   int                  nchannels = tpcStream->getNChannels  ();
   uint32_t                status = tpcStream->getStatus     ();
   printf ("TpcStream: 0x%2x.%1x.%1x  # channels = %4d status = %8.8" PRIx32 "\n",
           id.getCrate (),
           id.getSlot  (),
           id.getFiber (),
           nchannels,
           status);



   // ----------------------------------------------------------
   // For both the trimmed and untrimmed data, get the number of
   // the number of timesamples and their starting time.
   // ----------------------------------------------------------
   int       trimmedNticks = tpcStream->getNTicks             ();
   int     untrimmedNticks = tpcStream->getNTicksUntrimmed    ();
   auto untrimmedTimestamp = tpcStream->getTimeStampUntrimmed ();
   auto   trimmedTimestamp = tpcStream->getTimeStamp          ();
   printf (" Untrimmed: %6u  %8.8" PRIx64 "\n"
           "   trimmed: %6u  %8.8" PRIx64 "\n",
           untrimmedNticks, untrimmedTimestamp,
             trimmedNticks,   trimmedTimestamp);



   // -------------------------------------------------------
   // Get the data accessed as adcs[nchannels][trimmedNTicks]
   // For raw WIB Frames, this involves both expanding the
   // packed 12-bit ADCs to 16-bits and transposing the time
   // and channel order.
   // -------------------------------------------------------
   int     adcCnt = nchannels * trimmedNticks;
   int  adcNBytes = adcCnt    * sizeof (int16_t);
   int16_t  *adcs = reinterpret_cast <decltype (adcs)>(malloc (adcNBytes));

   //printf ("Transposing data: allocated %u bytes @ %p\n", 
   //        adcNBytes, (void *)adcs);

   tpcStream->getMultiChannelData (adcs);


   #if 0
   // ----------------------------------
   // Just dumping a portion of the data
   // ----------------------------------
   int adcsPerChannel = trimmedNticks;
   for (int ichan = 0; ichan < nchannels; ++ichan)
   {
      for (int itick = 0; itick < 32; itick += 8)
      {
         printf (
            " %2.2x.%4.4x: %p"
            " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 ""
            " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 "\n",
            ichan, itick, (void *)(adcs+itick),
            adcs[itick + 0], adcs[itick + 1], adcs[itick + 2], adcs[itick + 3],
            adcs[itick + 4], adcs[itick + 5], adcs[itick + 6], adcs[itick + 7]);
      }

      adcs += adcsPerChannel;
   }
   #endif


   // ===================================================================
   //
   // Still need to check the indirect memory and vector versions 
   // of this unpacking
   //
   // ===================================================================


   free (adcs);
   return;
}
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *\
 |                                                                        |
 |  The stuff below is not part of the high level user interface.  While  |
 |  it is public, these methods below are meant for lower level access.   |
 |  As such, they take a bit more expertise.                              |
 |                                                                        |
\* ---------------------------------------------------------------------- */
#include "dam/access/Headers.hh"
#include "dam/access/TpcStream.hh"
#include "dam/access/TpcRanges.hh"
#include "dam/access/TpcToc.hh"
#include "dam/access/TpcPacket.hh"

static unsigned int
        processWibFrames (pdd::access::TpcPacketBody const &pktBdy,
                          unsigned int                     pktType,
                          unsigned int                      pktOff,
                          unsigned int                      pktLen,
                          int                               pktNum,
                          int                           nWibFrames,
                          uint64_t                      *predicted);

static unsigned int
       processCompressed (pdd::access::TpcPacketBody const &pktBdy,
                          unsigned int                     pktType,
                          unsigned int                      pktOff,
                          unsigned int                      pktLen,
                          int                               pktNum,
                          uint64_t                      *predicted);


/* ---------------------------------------------------------------------- *//*!

  \brief Exercises the low level routines that are the backbone of the
         TpcStreamUnpack interface.

  \param[in]  tpcStream  The target TPC stream.
                                                                          */
/* ---------------------------------------------------------------------- */
static void processRaw (TpcStreamUnpack const *tpcStream)
{
   using namespace pdd;
   using namespace pdd::access;

   TpcStreamUnpack::Identifier id = tpcStream->getIdentifier ();
   TpcStream const        &stream = tpcStream->getStream     ();
   int                  nchannels = tpcStream->getNChannels  ();
   uint32_t                status = tpcStream->getStatus     ();

   printf ("TpcStream: %1d.%1d.%1d  # channels = %4d status = %8.8" PRIx32 "\n",
           id.getCrate (),
           id.getSlot  (),
           id.getFiber (),
           nchannels,
           status);


   // -----------------------
   // Construct the accessors
   // -----------------------
   TpcRanges        ranges (stream.getRanges ());
   TpcToc           toc    (stream.getToc    ());
   TpcPacket        pktRec (stream.getPacket ());
   TpcPacketBody    pktBdy (pktRec.getRecord ());
   uint64_t const  *pkts  = pktBdy.getData    ();

   ranges.print ();
   toc   .print ();
   

   int   npkts = toc.getNPacketDscs ();

   uint64_t  predicted = 0;
   unsigned int errCnt = 0;
   for (int pktNum = 0; pktNum < npkts; ++pktNum)
   {
      TpcTocPacketDsc pktDsc (toc.getPacketDsc (pktNum));
      unsigned int    pktOff = pktDsc.getOffset64 ();
      unsigned int   pktType = pktDsc.getType ();
      unsigned int    pktLen = pktDsc.getLen64 ();
      uint64_t const    *ptr = pkts + pktOff;
      
      if (pktDsc.isWibFrame ())
      {
         unsigned nWibFrames = pktDsc.getNWibFrames ();

         printf ("Packet[%2u:%1u(WibFrames ).%4d] = "
                 " %16.16" PRIx64 " %16.16" PRIx64 " %16.16" PRIx64 "\n",
                 pktNum, pktType, nWibFrames,
                 ptr[0], ptr[1], ptr[2]);

         errCnt += processWibFrames (pktBdy, 
                                     pktType,
                                     pktOff, 
                                     pktLen,
                                     pktNum,
                                     nWibFrames,
                                     &predicted);

         if (errCnt)
         {
            printf ("Error %u\n", errCnt);
         }
      }
      else if (pktDsc.isCompressed ())
      {
         printf ("Packet[%2u:%1u(Compressed).%4d] = "
                 " %16.16" PRIx64 " %16.16" PRIx64 " %16.16" PRIx64 "\n",
                 pktNum, pktType, pktLen,
                 ptr[0], ptr[1], ptr[2]);

         errCnt += processCompressed (pktBdy, 
                                      pktType,
                                      pktOff, 
                                      pktLen,
                                      pktNum,
                                      &predicted);
      }
   }

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \brief  Process a packet of WIB frames
   \return Number of errors encountered

   \param[in]     pktBdy The packet of WIB frames to process
   \param[in]    pktType The packet type (usually WibFrame), but if not
                         output will be transcoded to WibFrames
   \param[in]     pktOff The 64-bit offset of start of the WIB frames
   \param[in]     pktLen The length of the packet in units of 64-bit words
   \param[in] nWibFrames The number of WIB frames
   \param[in]  predicted The predicted timestamp
                                                                          */
/* ---------------------------------------------------------------------- */
static unsigned int 
       processWibFrames (pdd::access::TpcPacketBody const &pktBdy,
                         unsigned int                     pktType,
                         unsigned int                      pktOff,
                         unsigned int                      pktLen,
                         int                               pktNum,
                         int                           nWibFrames,
                         uint64_t                      *predicted)
{
   using namespace pdd;
   using namespace pdd::access;

   unsigned int errCnt = 0;

   // -----------------------------------------------------------------
   // Locate the WibFrames
   // This is really cheating, since the raw wib frame is exposed to
   // the user.  All others accessors hid the underlying implemenation.
   // -----------------------------------------------------------------
   WibFrame const *wf = pktBdy.getWibFrames (pktType, pktOff);

   uint64_t expTimestamp = *predicted;

   for (int iwf = 0; iwf < nWibFrames; ++iwf)
   {
      auto hdr       = wf->getHeader ();
      auto commaChar = wf->getCommaChar (hdr);
      auto version   = wf->getVersion   (hdr);
      auto id        = wf->getId        (hdr);
      auto fiber     = wf->getFiber     (hdr);
      auto crate     = wf->getCrate     (hdr);
      auto slot      = wf->getSlot      (hdr);
      auto reserved  = wf->getReserved  (hdr);
      auto wiberrors = wf->getWibErrors (hdr);
      auto timestamp = wf->getTimestamp ();

      if (timestamp != expTimestamp && (expTimestamp != 0))
      {
         errCnt += 1;
         printf ("Error %2d.%3d @ %4d %16.16" PRIx64 " != %16.16" PRIx64 "\n", 
                 errCnt, pktNum, iwf, timestamp, expTimestamp);
      }

      expTimestamp = timestamp + 25;

      // ----------------------------------------------------------
      // The const & are necessary to make sure this is a reference
      // ----------------------------------------------------------
      auto const &colddata = wf->getColdData ();
      
      auto cvt0 = colddata[0].getConvertCount ();
      auto cvt1 = colddata[1].getConvertCount ();
      
      puts   (
         "Wf #  CC Ve Cr.S.F ( Id)   Rsvd  WibErrs         TimeStamp Cvt0 Cvt1\n"
         "---- -- -- ------------- ------- -------- ---------------- ---- ----");
         
      printf (
         "%4u: %2.2x %2.2x %2.2x.%1.1x.%1.1x (%3.3x), %6.6x %8.8x %16.16" 
         PRIx64 " %4.4x %4.4x\n",
         iwf,
         commaChar, version,
         crate, slot, fiber, id,
         reserved,
         wiberrors,
         timestamp,
         cvt0, cvt1);
      
      ++wf;


      // ------------------------------------------------------
      // Would rather use sizeof's on colddata, but, while gdb
      // seems to get it right 
      //    i.e sizeof (*colddata) / sizeof (colddata) = 2
      // gcc gets 1
      // ------------------------------------------------------
      for (unsigned icd = 0;  icd < WibFrame::NColdData; ++icd)
      {
         auto const   &cd = colddata[icd];
         
         auto hdr0        = cd.getHeader0      ();
         auto streamErrs  = cd.getStreamErrs   (hdr0);
         auto reserved0   = cd.getReserved0    (hdr0);
         auto checkSums   = cd.getCheckSums    (hdr0);
         auto cvtCnt      = cd.getConvertCount (hdr0);
         
         auto hdr1        = cd.getHeader1      ();
         auto errRegister = cd.getErrRegister  (hdr1);
         auto reserved1   = cd.getReserved1    (hdr1);
         auto hdrs        = cd.getHdrs         (hdr1);
         
         auto const &packedAdcs = cd.locateAdcs12b ();
         
         puts   ("   iCd SE Rv  ChkSums  Cvt ErRg Rsvd      Hdrs\n"
                 "   --- -- -- -------- ---- ---- ----  --------");
            
         printf (
            "     %1x %2.2x %2.2x %8.8x %4.4x %4.4x %4.4x  %8.8" PRIx32 "\n",
            icd, 
            streamErrs,  reserved0, checkSums, cvtCnt,
            errRegister, reserved1, hdrs);
         
         int16_t adcs[WibColdData::NAdcs];
         cd.expandAdcs64x1 (adcs, packedAdcs);

         for (unsigned iadc = 0; iadc < WibColdData::NAdcs; iadc += 16)
         {
            printf (
               "Chn%2x:"
               " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 ""
               " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 ""
               " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 ""
               " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 "\n",
               iadc,  
               adcs[iadc + 0], adcs[iadc + 1], adcs[iadc + 2], adcs[iadc + 3],
               adcs[iadc + 4], adcs[iadc + 5], adcs[iadc + 6], adcs[iadc + 7],
               adcs[iadc + 8], adcs[iadc + 9], adcs[iadc +10], adcs[iadc +11],
               adcs[iadc +12], adcs[iadc +13], adcs[iadc +14], adcs[iadc +15]);
         }
      }

      putchar ('\n');
   }      

   *predicted = expTimestamp;

   return errCnt;
}
/* ---------------------------------------------------------------------- */


#include "dam/access/TpcCompressed.hh"

/* ---------------------------------------------------------------------- *//*!

   \brief  Process a packet of WIB frames
   \return Number of errors encountered

   \param[in]     pktBdy The packet of WIB frames to process
   \param[in]    pktType The packet type (usually WibFrame), but if not
                         output will be transcoded to WibFrames
   \param[in]     pktOff The 64-bit offset of start of the WIB frames
   \param[in]     pktLen The length of the packet in units of 64-bit words
   \param[in]  predicted The predicted timestamp
                                                                          */
/* ---------------------------------------------------------------------- */
static unsigned int
       processCompressed (pdd::access::TpcPacketBody const &pktBdy,
                          unsigned int                     pktType,
                          unsigned int                      pktOff,
                          unsigned int                      pktLen,
                          int                               pktNum,
                          uint64_t                      *predicted)
{
   using namespace pdd;
   using namespace pdd::access;

   pdd::record::TpcCompressedHdrHeader const
      *rhdr = reinterpret_cast
      <pdd::record::TpcCompressedHdrHeader const *>(pktBdy.getData() + pktOff);

   TpcCompressedHdrHeader const header(rhdr);

   header.print (rhdr);

   return 0;
}
