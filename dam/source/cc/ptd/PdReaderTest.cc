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
   2018.08.30 jjr Added check for TpcEmpty
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
#include "TpcWindow.hh"
#include "dam/TpcStreamAssessor.hh"
#include "dam/RceFragmentUnpack.hh"
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
   void  report ();

   class Process 
   {
   public:
      Process () { return; }

   public:
      enum Options
      {
         OPT_M_HILVLPRC  = (1 << 0),  /*!< Process using hi level         */
         OPT_M_LOLVLPRC  = (1 << 1),  /*!< Process using hi level         */
         OPT_M_WIBHDR    = (1 << 2),  /*!< Print header information       */
         OPT_M_WIBERRORS = (1 << 3),  /*!< Print the wib error records    */
         OPT_M_WIBCHNDMP = (1 << 4),  /*!< Dump adcs as channels          */
         OPT_M_WIBTIMDMP = (1 << 5),  /*!< Dump adcs in time order        */
         OPT_M_WIBFRMDMP = (1 << 6)   /*!< Dump WIB frames                */
      };

   public:
      uint32_t m_options;
   };

public:
   char *const         *m_filenames;  /*!< Input file name                */
   enum Reader::FileType m_filetype;  /*!< The input file type            */
   int                     m_nfiles;  /*!< The number of files            */
   unsigned int          m_nprocess;  /*!< Number of records to process   */
   unsigned int             m_nskip;  /*!< Number of records to skip      */
   Process                m_process;  /*!< Process ooptions               */
   bool                     m_quiet;  /*!< Quiet mode                     */
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
   uint32_t options = 0;

   m_filetype  = Reader::FileType::Binary;
   m_nprocess  = 0xffffffff;
   m_nskip     = 0;
   m_quiet     = false;
   int c;
   while ( (c = getopt (argc, argv, "qbgn:s:d:")) != -1 )
   {
      if      (c == 'b') m_filetype = Reader::FileType::Binary;
      else if (c == 'g') m_filetype = Reader::FileType::TextGdb64;
      else if (c == 'n') m_nprocess = strtoul (optarg, NULL, 0);
      else if (c == 's') m_nskip    = strtoul (optarg, NULL, 0);
      else if (c == 'q') m_quiet    = true;
      else if (c == 'd')
      {
         if (strchr (optarg, 'e')) options   |= Process::OPT_M_WIBERRORS;
         if (strchr (optarg, 'h')) options   |= Process::OPT_M_HILVLPRC;
         if (strchr (optarg, 'w')) options   |= Process::OPT_M_WIBHDR;
         if (strchr (optarg, 'c')) options   |= Process::OPT_M_WIBCHNDMP;
         if (strchr (optarg, 't')) options   |= Process::OPT_M_WIBTIMDMP;
         if (strchr (optarg, 'f')) options   |= Process::OPT_M_WIBFRMDMP;
      }
   }
   
   if (optind < argc)
   {
      m_filenames = &argv[optind];
      m_nfiles    = argc - optind;
   }
   else
   {
      fprintf (stderr, "Error: no file specified\n");
      exit (-1);
   }

   m_process.m_options = options;

   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Reports the parameters being used
                                                                          */
/* ---------------------------------------------------------------------- */
void Prms::report ()
{
   static const char Separator[] =
"----------------------------------------------------------------------------";

   if (m_quiet) 
   {
      return;
   }
   
   putchar ('\n');
   puts (Separator);

   char const *typeName = 
        (m_filetype == Reader::FileType::  Binary)  ? "Binary"
      : (m_filetype == Reader::FileType::TextGdb64) ? "Gdb dump"
      : "Unknown";

   printf ("File      : %s (%s)\n", m_filenames[0], typeName);

   if (m_nprocess == 0xffffffff) printf ("Processing: all\n");
   else                          printf ("Processing: %u\n",
                                            m_nprocess);

   if (m_nskip   ==           0) printf ("Skipping  : none\n");
   else                          printf ("Skipping  : %u\n",
                                         m_nskip);
   puts (Separator);
   putchar ('\n');
   return;
}
/* ---------------------------------------------------------------------- */





static int  processFile      (Prms const               &prms,
                              char const           *filename,
                              enum Reader::FileType filetype);

static void processFragment  (Prms::Process const  &prms, 
                              uint64_t        const *buf);
static void process          (Prms::Process const  &prms, 
                              TpcStreamUnpack const *tpc);
static void processRaw       (Prms::Process const  &prms, 
                              TpcStreamUnpack const *tpc);
static void dumper           (int16_t const *adcs,
                              int          nticks,
                              uint64_t   times[3],
                              int         begChan,
                              int         endChan,
                              int         begTick,
                              int         endTick);


/* ---------------------------------------------------------------------- */
int main (int argc, char *const argv[])
{


   // -----------------------------------
   // Extract the command line parameters
   // -----------------------------------
   Prms prms (argc, argv);
   prms.report ();

   for (int ifile = 0; ifile < prms.m_nfiles; ifile++)
   {
      processFile (prms,
                   prms.m_filenames[ifile],
                   prms.m_filetype);
   }

   return 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

                                                                          */
/* ---------------------------------------------------------------------- */  
static int processFile (Prms const               &prms,
                        char const           *filename,
                        enum Reader::FileType filetype)
{
   static size_t const MaxBuf = 10 * 1024 * 1024;

   Reader &reader = ReaderCreate (filename, 
                                  filetype);

   // -----------------------------------
   // Open the file to process
   // -----------------------------------
   int  err = reader.open ();
   if (err) 
   {
      reader.report (err);
      exit (-1);
   }

#  if 1
   uint64_t *buf = reinterpret_cast<decltype (buf)>(malloc (MaxBuf));
#else
   // Force misaligned 
   uint8_t  *bufptr = reinterpret_cast<decltype (bufptr)>(malloc (MaxBuf));
   uint64_t  bufint = reinterpret_cast<decltype (bufint)>(bufptr);
   bufint |= 7;
   uint64_t *buf    = reinterpret_cast<decltype(buf)>(bufint);
#endif

   unsigned total = prms.m_nprocess;
   if (total != 0xffffffff) total += prms.m_nskip;
   for (unsigned idx = 0; idx < total; idx++)
   {
      HeaderFragmentUnpack *header = HeaderFragmentUnpack::assign (buf);
      ssize_t               nbytes = reader.read (header);

      if (nbytes <= 0)
      {
         if (nbytes == 0) 
         {
            break;
         }

         else
         {
            exit (-1);
         }
      }

      // Check that this looks like a fragment header
      bool isHeader = header->isOkay ();
      if (!isHeader)
      {
         fprintf (stderr, 
         "Error: %16.16" PRIx64 " is not a legitmate fragment header\n",
                  buf[0]);
      }



      // ----------------------------------------------------
      // Get the number of 64-bit words in this data fragment
      // and read the body of the data fragment.
      // ----------------------------------------------------
      uint64_t  n64 = header->getN64 ();
      ssize_t nread = reader.read (buf, n64, nbytes);
      if (nread <= 0)
      {
         fprintf (stderr, 
                  "Error: Incomplete or corrupted record\n");
         break;
      }

      if (idx < prms.m_nskip) continue;

      bool isOkay = RceFragmentUnpack::isOkay (buf, nread+sizeof(header));
      if (!isOkay)
      {
         fprintf (stderr,
         "Error: Fragment header is inconsistent with length and trailer\n");
      }


      processFragment (prms.m_process, buf);
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
static void processFragment (Prms::Process const   &prms, 
                             uint64_t const         *buf)
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
      if (df.isTpcNormal () || df.isTpcDamaged () || df.isTpcEmpty ())
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

            if (prms.m_options & Prms::Process::OPT_M_HILVLPRC)
            {
               printf ("\nTpcStream: %d/%d  -- using high level access methods\n", 
                       istream, nstreams);
               process    (prms, tpcStream);
            }


            printf ("\nTpcStream: %d/%d -- using low  level access methods\n",
                    istream, nstreams);
            processRaw (prms, tpcStream);
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
            processRaw (prms, tpcStream);
         }
      }
      else if (df.isTpcEmpty ())
      {
         printf ("TpcStream is empty\n");
      }

      
      df.printTrailer ();
   }
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
static void announceTpcStream (TpcStreamUnpack const *tpcStream)
{
   // ----------------------------------
   // Get the identifier for this stream 
   // This is the WIB's Crate.Slot.Fiber
   // ----------------------------------
   TpcStreamUnpack::Identifier id = tpcStream->getIdentifier ();
   int                  nchannels = tpcStream->getNChannels  ();
   uint32_t                status = tpcStream->getStatus     ();
   
   char const *recName = tpcStream->isTpcNormal  () ? "Normal"
                       : tpcStream->isTpcDamaged () ? "Damaged"
                       : "Unknown";

   TpcStreamUnpack::DataFormatType fmtType = tpcStream->getDataFormatType ();
   char const *fmtName = 
     fmtType == TpcStreamUnpack::DataFormatType::WibFrame   ? "WibFrames"
   : fmtType == TpcStreamUnpack::DataFormatType::Compressed ? "Compressed"
   : fmtType == TpcStreamUnpack::DataFormatType::Mixed      ? "Mixed"
   : "Unknown";
   
   printf ("TpcStream:%s:%s WibId:0x%2x.%1x.%1x  # channels = %4d "
           "status = %8.8" PRIx32 "\n",
           recName,
           fmtName,
           id.getCrate (),
           id.getSlot  (),
           id.getFiber (),
           nchannels,
           status);

   return;
}
/* ---------------------------------------------------------------------- */






/* ---------------------------------------------------------------------- *//*!

  \brief  Exercise the standard interface to the TPC Stream data

  \param[in]  tpcStream  The target TPC stream.
                                                                          */
/* ---------------------------------------------------------------------- */
static void process (Prms::Process const        &prms, 
                     TpcStreamUnpack const *tpcStream)
{
   TpcWindow::Window<TpcWindow::Type::Untrimmed> untrimmed (tpcStream);
   TpcWindow::Window<TpcWindow::Type::Trimmed>     trimmed (tpcStream);

   // ----------------------------------------------------------
   // For both the trimmed and untrimmed data, get the number of
   // the number of timesamples and their starting time.
   // ----------------------------------------------------------
   if (prms.m_options & Prms::Process::OPT_M_WIBHDR)
   {
      announceTpcStream (tpcStream);
      untrimmed.print ();
      trimmed  .print ();
   }

   /*
     TpcStreamUnpack::DamageReport report ();

     

   if (stream->isTpcDamaged ())
   {
      stream->getTpcDamageReport (&report);
   }
   */

   // -------------------------------------------------------
   // Get the data accessed as adcs[nchannels][trimmedNTicks]
   // For raw WIB Frames, this involves both expanding the
   // packed 12-bit ADCs to 16-bits and transposing the time
   // and channel order.
   // -------------------------------------------------------
   int  nchannels = tpcStream->getNChannels ();
   int     nticks = tpcStream->getNTicks    ();
   int     adcCnt = nchannels * nticks;
   int  adcNBytes = adcCnt    * sizeof (int16_t);
   int16_t  *adcs = reinterpret_cast <decltype (adcs)>(malloc (adcNBytes));
   bool      okay = tpcStream->getMultiChannelData (adcs);
   int      nerrs = 0;
   int16_t    min = 0x7ff;
   int16_t    max = -1;

   for (int idx = 0; idx < adcCnt;  idx++)
   {
      int16_t val = adcs[idx];
      //if (val == 0)
      //{
      //   printf ("Small adcs[%3d:%4d,%5d] = %4.4" PRIx16 "\n", 
      //           idx/nticks, idx%nticks, idx, val);
      //}
      if (val < min) min = val;
      if (val > max) max = val;
      int tst = val & ~0xfff;
      if (tst)
      {
         nerrs += 1;
         printf ("Error: Adcs[%3d:%4d,%6d] = %4.4x\n", 
                 idx/nticks, idx%nticks, idx, adcs[idx]);
      }
   }

   announceTpcStream (tpcStream);
   printf ("Number of errors: %d out of %d channels of %d adcs total of %d"
           " min = %4" PRIx16 " max = %4" PRIx16 "\n", 
           nerrs, nchannels, nticks, adcCnt, min, max);

   if (!okay) 
   {
      free (adcs);
      return;
   }


   int begChan = 0;
   int endChan = 4;
   int begTick = 0;
   int endTick = 128;
   dumper (adcs,
           trimmed.m_nticks,
           trimmed.m_times,
           begChan,
           endChan,
           begTick,
           endTick);


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




/* ---------------------------------------------------------------------- *//*!

  \brief Dumps a specified range of iticks/channels
                                                                          */
/* ---------------------------------------------------------------------- */
static void dumper (int16_t const *adcs,
                    int          nticks,
                    uint64_t   times[3],
                    int         begChan,
                    int         endChan,
                    int         begTick,
                    int         endTick)
{
   int adcsPerChannel = nticks;
   TpcStreamUnpack::timestamp_t    ts = times[0];
   TpcStreamUnpack::timestamp_t tsTrg = times[1];
   int                        trgTick = (tsTrg - ts) / 25;

   for (int ichan = begChan; ichan < endChan; ++ichan)
   {
      printf (" %2.2x.%16.16" PRIx64 "\n", ichan, ts);

      for (int itick = begTick; itick < endTick; itick++)
      {

         if ( (itick & 0xf) ==    0) printf (" %4x", itick);
         if (itick != trgTick)       printf (" %3.3" PRIx16, adcs[itick]);
         else                        printf ("*%3.3" PRIx16, adcs[itick]); 
         if  ( (itick & 0xf) == 0xf) putchar ('\n');
      }

      if (nticks & 0xf) putchar ('\n');
   

      adcs += adcsPerChannel;
   }

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
       processWibFrames  (Prms::Process const &prms,
                          pdd::access::TpcPacketBody const &pktBdy,
                          unsigned int                     pktType,
                          unsigned int                      pktOff,
                          unsigned int                      pktLen,
                          int                               pktNum,
                          int                           nWibFrames);

static unsigned int
       processCompressed (Prms::Process const                &prms,
                          pdd::access::TpcPacketBody const &pktBdy,
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
static void processRaw (Prms::Process const        &prms, 
                        TpcStreamUnpack const *tpcStream)
{
   using namespace pdd;
   using namespace pdd::access;


   // -----------------------
   // Construct the accessors
   // -----------------------
   TpcStream const &stream = tpcStream->getStream     ();
   TpcRanges        ranges (stream.getRanges ());
   TpcToc           toc    (stream.getToc    ());
   TpcPacket        pktRec (stream.getPacket ());
   TpcPacketBody    pktBdy (pktRec.getRecord ());
   uint64_t const  *pkts  = pktBdy.getData    ();

   if (prms.m_options & Prms::Process::OPT_M_WIBHDR)
   {
      announceTpcStream (tpcStream);
      ranges.print ();
      toc   .print ();
   }
   

   if (prms.m_options & Prms::Process::OPT_M_WIBERRORS)
   {
      TpcStreamAssessor assessment;
      assessment.assessTrimmed (tpcStream);
      assessment.report ();
      ///assessment.report (TpcStreamAssessor::FLT_M_TIMING);
      assessment.reset  ();
   }


   int   npkts = toc.getNPacketDscs ();

   uint64_t predicted  = 0;
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

         /*
         printf ("Packet[%2u:%1u(WibFrames ).%4d] = "
                 " %16.16" PRIx64 " %16.16" PRIx64 " %16.16" PRIx64 "\n",
                 pktNum, pktType, nWibFrames,
                 ptr[0], ptr[1], ptr[2]);
         */

         processWibFrames (prms,
                           pktBdy, 
                           pktType,
                           pktOff, 
                           pktLen,
                           pktNum,
                           nWibFrames);
      }
      else if (pktDsc.isCompressed ())
      {
         printf ("Packet[%2u:%1u(Compressed).%4d] = "
                 " %16.16" PRIx64 " %16.16" PRIx64 " %16.16" PRIx64 "\n",
                 pktNum, pktType, pktLen,
                 ptr[0], ptr[1], ptr[2]);

         processCompressed (prms,
                            pktBdy, 
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



static void wibFramesPrint  (pdd::access::WibFrame const *wf,
                             int                       wfBeg,
                             int                       wfEnd);


/*
static int  wibFrameAnalyze (WibErrorRecords     &errRecords, 
                             WibExpected           *expected,
                             unsigned int             pktNum,
                             pdd::access::WibFrame const *wf, 
                             int                       wfBeg,
                             int                       wfEnd);
*/

static void     wibChnPrint (pdd::access::WibFrame const *wf, 
                             int                       wfBeg,
                             int                       wfEnd,
                             int                      chnBeg,
                             int                      chnEnd);



/* ---------------------------------------------------------------------- *//*!

   \brief  Process a packet of WIB frames
   \return Number of errors encountered

   \param[in]      pktBdy The packet of WIB frames to process
   \param[in]     pktType The packet type (usually WibFrame), but if not
                          output will be transcoded to WibFrames
   \param[in]      pktOff The 64-bit offset of start of the WIB frames
   \param[in]      pktLen The length of the packet in units of 64-bit words
   \param[in]  nWibFrames The number of WIB frames
   \param[in] predictions The predicted timestamp, convert counts, etc
                                                                          */
/* ---------------------------------------------------------------------- */
static unsigned int 
       processWibFrames (Prms::Process const                &prms, 
                         pdd::access::TpcPacketBody const &pktBdy,
                         unsigned int                     pktType,
                         unsigned int                      pktOff,
                         unsigned int                      pktLen,
                         int                               pktNum,
                         int                           nWibFrames)
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


   int wfBeg = 0;
   int wfEnd = nWibFrames;
   
   if (prms.m_options & Prms::Process::OPT_M_WIBFRMDMP)
   {
      wibFramesPrint (wf, wfBeg, wfEnd);
   }





   if (prms.m_options & Prms::Process::OPT_M_WIBCHNDMP)
   {
      int chnBeg = 0;
      int chnEnd = 0;
      wibChnPrint (wf, wfBeg, wfEnd, chnBeg, chnEnd);
   }

   return errCnt;
}

static void wibFramesPrint (pdd::access::WibFrame const *wf, 
                            int                       wfBeg,
                            int                       wfEnd)
{
   using namespace pdd;
   using namespace pdd::access;


   for (int iwf = wfBeg; iwf < wfEnd; ++wf, ++iwf)
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

      puts   ("Wf #  CC Ve Cr.S.F ( Id)   Rsvd  WibErrs         TimeStamp Cvt0 Cvt1\n"
              "---- -- -- ------------- ------- -------- ---------------- ---- ----");
   
      printf ("%4u: %2.2x %2.2x %2.2x.%1.1x.%1.1x (%3.3x), %6.6x %8.8x %16.16" 
              PRIx64 " %4.4x %4.4x\n",
              iwf,
              commaChar, version,
              crate, slot, fiber, id,
              reserved,
              wiberrors,
              timestamp,
              cvt0, cvt1);
      
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
         auto streamErr1  = cd.getStreamErr1   (hdr0);
         auto streamErr2  = cd.getStreamErr2   (hdr0);
         auto checkSums   = cd.getCheckSums    (hdr0);
         auto cvtCnt      = cd.getConvertCount (hdr0);
      

         auto hdr1        = cd.getHeader1      ();
         auto errRegister = cd.getErrRegister  (hdr1);
         auto reserved1   = cd.getReserved1    (hdr1);
         auto hdrs        = cd.getHdrs         (hdr1);
      
         auto const &packedAdcs = cd.locateAdcs12b ();
         

         int16_t adcs[WibColdData::NAdcs];
         cd.expandAdcs64x1 (adcs, packedAdcs);
      
         puts   ("   iCd E2 E1  ChkSums  Cvt ErRg Rsvd      Hdrs\n"
                 "   --- -- -- -------- ---- ---- ----  --------");
         printf (
            "     %1x %2.2x %2.2x %8.8x %4.4x %4.4x %4.4x  %8.8" PRIx32 "\n",
            icd, 
            streamErr2,  streamErr1, checkSums, cvtCnt,
            errRegister,  reserved1, hdrs);
         
      
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
}
/* ---------------------------------------------------------------------- */





static void   wibChnPrint (pdd::access::WibFrame const *wf, 
                           int                       wfBeg,
                           int                       wfEnd,
                           int                      chnBeg,
                           int                      chnEnd)
{
   printf ("Channel Dump\n");
   return;
}


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
       processCompressed (Prms::Process const                &prms, 
                          pdd::access::TpcPacketBody const &pktBdy,
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
