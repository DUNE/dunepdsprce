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



static void processFragment (uint64_t       const *buf);
static void process        (TpcStreamUnpack const *tpc);
static void processRaw     (TpcStreamUnpack const *tpc);


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
      reader.read (buf, n64, nbytes);

      processFragment (buf);
   }


   fprintf (stderr, "Closing\n");
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
      if (df.isTpcNormal ())
      {

         printf ("Have TpcStream data type\n");

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
            process    (tpcStream);
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
   int                  nchannels = tpcStream->getNChannels ();
   printf ("TpcStream: %1d.%1d.%1d  # channels = %4d\n",
           id.getCrate (),
           id.getSlot  (),
           id.getFiber (),
           nchannels);



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
   int  adcNBytes = adcCnt    * sizeof (uint16_t);
   uint16_t *adcs = reinterpret_cast <decltype (adcs)>(malloc (adcNBytes));

   printf ("Transposing data: allocated %u bytes @ %p\n", 
           adcNBytes, (void *)adcs);

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
   TpcStream const        &stream = tpcStream->getStream ();

   // -----------------------
   // Construct the accessors
   // -----------------------
   TpcRanges        ranges (stream.getRanges ());
   TpcToc           toc    (stream.getToc    ());
   TpcPacket        pktRec (stream.getPacket ());
   uint64_t const  *pkts  = pktRec.getData   ();


   printf ("TpcStream: %1d.%1d.%1d\n",
           id.getCrate (),
           id.getSlot  (),
           id.getFiber ());

   ranges.print ();
   toc   .print ();
   

   int   npkts = toc.getNPacketDscs ();


   for (int ipkt = 0; ipkt < npkts; ++ipkt)
   {
      TpcTocPacketDsc pktDsc (toc.getPacketDsc (ipkt));
      unsigned int      o64 = pktDsc.getOffset64 ();
      unsigned int  pktType = pktDsc.getType ();

      
      if (pktDsc.isWibFrame ())
      {
         printf ("Have Wib frames\n");
      }

      uint64_t const *ptr = pkts + o64;
      printf ("Packet[%2u:%1u] = "
              " %16.16" PRIx64 " %16.16" PRIx64 " %16.16" PRIx64 "\n",
              ipkt, pktType,
              ptr[0], ptr[1], ptr[2]);


      WibFrame const *wf = reinterpret_cast<decltype(wf)>(ptr);


      for (unsigned iwf = 0; iwf < 4; ++iwf)
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

         // ----------------------------------------------------------
         // The const & are necessary to make sure this is a reference
         // ----------------------------------------------------------
         auto const &colddata = wf->getColdData ();

         auto cvt0 = colddata[0].getConvertCount ();
         auto cvt1 = colddata[1].getConvertCount ();
         
         puts   (
        "  Wf  CC Ve Cr.S.F ( Id)   Rsvd  WibErrs         TimeStamp Cvt0 Cvt1\n"
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
            
            uint16_t adcs[WibColdData::NAdcs];
            cd.expandAdcs64x1 (adcs, packedAdcs);

            for (unsigned iadc = 0; iadc < WibColdData::NAdcs; iadc += 8)
            {
               printf (
               "Chn%2x:"
               " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 ""
               " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 "\n",
               iadc,  
               adcs[iadc + 0], adcs[iadc + 1], adcs[iadc + 2], adcs[iadc + 3],
               adcs[iadc + 4], adcs[iadc + 5], adcs[iadc + 6], adcs[iadc + 7]);
            }
         }
         putchar ('\n');
      }      
   }

   return;
}
/* ---------------------------------------------------------------------- */
