// -*-Mode: C++;-*-
#ifndef TPCTRIMMEDRANGEL_HH
#define TPCTRIMMEDRANGEL_HH


/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcTrimmedRange.hh
 *  @brief    Defines the relevant samples for the trimmed data.
 *
 *  @verbatim
 *                               Copyright 2018
 *                                    by
 *
 *                       The Board of Trustees of the
 *                    Leland Stanford Junior University.
 *                           All rights reserved.
 *
 *  @endverbatim
 *
 *  @author
 *  <russell@slac.stanford.edu>
 *
 *  @par Date created:
 *  <2018/10/19>
 *
 * @par Credits:
 * SLAC
 *
 * @par
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2018.10.22 jjr Corrected getting the correct number of frames for the
                  trimmed compressed data.  It was incorrectly using 
                  getNWibFrames, which cleary does not work for compressed
                  data.
   2018.10.18 jjr Created
\* ---------------------------------------------------------------------- */


#include "TpcStream-Impl.hh"
#include "TpcCompressed-Impl.hh"
#include <string>
#include <iostream>

namespace pdd    {
namespace access {


/* ---------------------------------------------------------------------- *//*!

  \brief The beginning and ending samples for the trimmed data
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcTrimmedRange
{
public:
   TpcTrimmedRange (pdd::access::TpcStream const &tpc);


public:
   /* ------------------------------------------------------------------- *//*!

      \brief Class to contain the informaion needed to locate the WIB 
             frame containing a target timestamp
                                                                          */
   /* ------------------------------------------------------------------- */
   class Location
   {
   public:
      Location () { return; }

   public:
     unsigned int   locate (uint64_t     targetTimestamp,
                            uint64_t      firstTimestamp,
                            uint32_t              pktIdx,
                            WibFrame const           *wf,
                            int            nframesPerPkt,
                            int               nTotFrames,
                            bool                   begin);

         
   public:
      uint64_t    m_wibTs; /*!< The WIB frame timestamp                   */
      uint64_t    m_tgtTs; /*!< The target timestamp                      */
      uint16_t   m_wibOff; /*!< The offset of the frame containing or
                                nearest to the target time                */
      uint16_t   m_pktNum; /*!< The beginning packet number               */
      uint16_t   m_pktOff; /*!< The beginning offset                      */
      uint16_t     m_rsvd; /*!< Reserved for future use                   */
   };
   /* ------------------------------------------------------------------- */

public:
   Location        m_beg; /*!< Beginning frame location                   */
   Location        m_end; /*!< Ending    frame location                   */
   uint32_t     m_nticks; /*!< Number of ticks in the trimmed data        */

};
/* ---------------------------------------------------------------------- */




/* ====================================================================== */
#define TPCTRIMMEDRANGE_DEBUG 0
//-------------------------------------------------------------------------
//
// Debugging dump
// --------------
//
#if TPCTRIMMEDRANGE_DEBUG

    #include <cstdio>

    static void printA (char const  *title, 
                        uint64_t  targetTs,
                        uint64_t     wibTs,
                        int         wibOff);

    static void printB (char const                    *which, 
                        TpcTrimmedRange::Location const &loc);

    static void printC (uint32_t                      nticks);

#else

   #define printA(_title, _targetTs, _wibTs, _wibOff)
   #define printB(_which, _loc)
   #define printC(_nticks)

#endif
/* ====================================================================== */




/* ---------------------------------------------------------------------- *//*!

   \brief Return the starting, trigger and ending information 

   \param[in] stream

   \par
    This routine is not meant to be general purpose.  It should only be
    used for WibFrame data.  It will not work on Compressed data.  Also
    if the frame is not damaged, the the usual getTrimmed gives the same
    information faster.
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcTrimmedRange::TpcTrimmedRange (pdd::access::TpcStream const &stream)
{
   using namespace pdd;
   using namespace pdd::access;
   const std::string myname = "TpcTrimmedRange::ctor: ";

   // -------------------------------------
   // Extract the timing window information
   // -------------------------------------
   record::TpcRanges const           *ranges = stream.getRanges     ();

   if (!ranges)
     {
       std::cout << "Null ranges pointer in TpcTrimmedRange::TpcTrimmedRange.  Skipping data." << std::endl;
       m_nticks = 0;
       return;
     }

   unsigned int                       bridge = TpcRanges::getBridge (ranges);


   record::TpcRangesWindow     const *window = TpcRanges::getWindow (ranges);
   auto begTs = TpcRangesWindow::getBegin   (window, bridge);
   auto endTs = TpcRangesWindow::getEnd     (window, bridge);


   // ---------------------------------------
   // Fetch the timestamp of the first sample
   // ---------------------------------------
   record::TpcRangesTimestamps const 
                                 *timestamps = TpcRanges::getTimestamps(ranges);
   auto firstTs = TpcRangesTimestamps::getBegin (timestamps, bridge);


   // ------------------------------------------------------------
   // Find the best guesses at begin, trigger and end frames
   // These will be corrected if all WIB frames are time sequence.
   // These will be wrong if there are missing or duplicate frames.
   // ------------------------------------------------------------
   pdd::access::TpcRangesIndices indices (TpcRanges::getIndices (ranges),
                                                                 bridge);
   uint32_t begIdx = indices.getBegin   ();
   uint32_t endIdx = indices.getEnd     ();


   // ---------------------------
   // Locate the WibFrame packets
   // ---------------------------
   TpcToc           toc    (stream.getToc     ());
   TpcPacket        pktRec (stream.getPacket  ());
   if (!pktRec.getRecord())
     {
       std::cout << "dunepdsprce: bad header in TpcTrimmedRange::TpcTrimmedRange" << std::endl;
       m_nticks = 0;
       return;
     }

   TpcPacketBody    pktBdy (pktRec.getRecord  ());
   TpcTocPacketDsc  pktDsc (toc.getPacketDsc (0));
   int             npkts =  toc.getNPacketDscs ();

   unsigned int          pktType = pktDsc.getType      ();
   unsigned int         pktOff64 = pktDsc.getOffset64  ();
   WibFrame const            *wf = pktBdy.getWibFrames (pktType, pktOff64);

   int nframes;
   if (wf)
   {
      nframes = pktDsc.getNWibFrames ();
   }
   else
   {
      // --------------------------------------------------
      // This cheats by assuming all channels contain the
      // same number of samples.  While in practice, this
      // so far has been true, the underlying format allows
      // different channels to have different samples sizes.
      // --------------------------------------------------
      uint32_t      n64 = pktDsc.getLen64    ();
      TpcCompressed cmp (pktBdy.getData(), n64);
      auto const    tlr = cmp.getTocTrailer  ();
      nframes           = TpcCompressedTocTrailer::getNSamples   (tlr);
   }


   int       nTotFrames = npkts * nframes;

   // --------------------------------------------------------
   // Confirm or find the offset of the first and last samples
   // of the trimmed data
   // --------------------------------------------------------
   bool failbeg = m_beg.locate (begTs, firstTs, begIdx, wf, nframes, nTotFrames,  true);
   bool failend = m_end.locate (endTs, firstTs, endIdx, wf, nframes, nTotFrames, false);

   if ( failbeg || failend ) {
     // -------------------------------------------------------------
     // Jan 2020 (dla): If either search fails, then zero n_ticks so
     // caller will know this is corrupt data.
     // -------------------------------------------------------------
     std::cout << myname << "WARNING: Skipping corrupt data: ";
     if ( failbeg && failend ) std::cout << "first and last samples";
     else if ( failbeg )       std::cout << "first sample";
     else                      std::cout << "last sample";
     std::cout << " not found." << std::endl;
     m_nticks = 0;
   } else {
     // -------------------------------------------------------------
     // Compute the number of frames within the trimmed data
     // This will typically be, for example 6000 for a 3 msec window.
     // But if there are dropped frames, this will be less.
     // -------------------------------------------------------------
     m_nticks = m_end.m_wibOff - m_beg.m_wibOff;
   }

   printB  ("Begin",   m_beg);
   printB  ("End",     m_end);
   printC  (m_nticks);


   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Locates the \a timestamp in the data
  \retval == 0, successful
  \retval <  0, failure

  \param[in]      timestamp  The timestamp to locate
  \param[in] firstTimestamp  The timestamp of the first WIB frame in
                             the untrimmed data.
  \param[in]         pktIdx  Contains the predicted packet number 
                             and offset of target data
  \param[in]             wf  Pointer to the first WIB frame in the
                             untrimmed data. This will be NULL if
                             the data is not in WIB frame format
  \param[in]  nframesPerPkt  The number of frames (typically 1024) 
                             in a packet
  \param[in]     nTotframes  The total number of WIB frames in the
                             untrimmed data.
  \param[in]          begin  Flag indicating looking for the beginning

   If the prediction is greater than the number of untrimmed WIB frames,
   a new prediction based on the timestamp of the first frame in the
   untrimmed data is bad.  If this prediction is itself outside the
   range, an error condition is returned.

   If the data is in WIB frame format, the predicted frame is located
   and its timestamp is verified to contain the target \a timestamp. If
   not, the prediction serves as the initial guess and the WIB frames
   are scanned for the target \a timestamp.

   If the data is not in WIB frame format, the only alternative at this
   time is to believe the prediction.  This can be wrong if the WIB frames
   in the untrimmed data are not perfectly sequential in time.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcTrimmedRange::Location::
                locate (uint64_t                           timestamp, 
                        uint64_t                      firstTimestamp,
                        uint32_t                              pktIdx,
                        WibFrame const                           *wf,
                        int                            nframesPerPkt,
                        int                               nTotFrames,
                        bool                                   begin)
{
   std::string myname = "TpcTrimmedRange::Location::locate: ";
   int dbg = 1;
   auto pktNum = pdd::access::TpcRangesIndices::getPacket (pktIdx);
   auto pktOff = pdd::access::TpcRangesIndices::getOffset (pktIdx);
   int   wfOff = nframesPerPkt * pktNum + pktOff;

   m_tgtTs = timestamp;

   // ------------------------------------------------
   // Make sure that the initial guess is legitimate.
   // Failure should be very rare, only happening when
   // the calculation, which depends on the data not
   // having certain pathologies, on the RCE fails.
   // ------------------------------------------------
   if (wfOff >= nTotFrames)
   {
      // ------------
      // Try guessing
      // ------------
      //  
      int wfOffNew = (timestamp - firstTimestamp) / 25;

      // ---------------------------------------------
      // Hopeless if this occurs before the timestamp
      // of the first WIB frame in the untrimmed data.
      // This likely indicates a complete corruption.
      // ---------------------------------------------
      if ( wfOffNew < 0 || wfOffNew >= nTotFrames ) {
         if ( dbg >= 2 ) {
           std::cout << myname << "WARNING: Skipping invalid frame index: " << wfOff << " --> "
                     << wfOffNew << " >= " << nTotFrames << std::endl;
         }
         m_wibOff = 0;
         m_pktNum = 0;
         m_pktOff = 0;
         m_wibTs  = 0;
         return -1;
      }

      // Jan 2020 (dla): Write a warning so we can check if this ever happens.
      if ( dbg >= 1 ) {
        std::cout << myname << "WARNING: Guess fixed invalid frame index: " << wfOff << " --> "
                  << wfOffNew << " / " << nTotFrames << std::endl;
      }
      wfOff = wfOffNew;

      pktNum = wfOff / nframesPerPkt;
      pktOff = wfOff % nframesPerPkt;
   }


   // ----------------------------------------------
   // Can only verify location if data is WIB frames
   // ----------------------------------------------
   if (!wf)
   {
      // ------------------------------------------
      // Not WIB frame format, no choice but to 
      // go with the prediction.  In the case of
      // dropped frames, this will be incorrect.
      //
      // Note: The returned timestamp is rounded
      //        down to the nearst 25 tick boundary
      //        commensurate with the WIB frame 
      //        quantization.
      // -------------------------------------------
      m_wibOff = wfOff;
      m_pktNum = pktNum;
      m_pktOff = pktOff;
      m_wibTs  = firstTimestamp + wfOff * 25;
      return 0;
   }

   // ----------------------------------------
   // Get the timestamp at the predicted point
   // ----------------------------------------
   auto  wibTs = wf[wfOff].getTimestamp ();


   // --------------------------------------------------
   // The timestamp must be between wibTs and wibTs + 25
   // --------------------------------------------------
   int64_t delta = timestamp - wibTs;
   if (delta >= 0 && delta < 25)
   {
      // --------------------
      // Found where expected
      // --------------------
      printA ("-> found at predicted  spot:", timestamp, wibTs, wfOff);
      m_wibOff  = wfOff;
      m_pktNum  = pktNum;
      m_pktOff  = pktIdx;
      m_wibTs   = wibTs;
      return 0;         
   }  
   else
   {
      // ------------------------------------------
      // Need to search for the correct wib frame.
      //
      // Because the guess is calculated from the
      // low end and because missing frames are the
      // most likely reason the guess is wrong, 
      // the guess almost always is too far, so
      // searching backward is almost always the
      // case. It is hard to think of a scenario
      // (duplicate/bogus packets, where a forward
      // search would be done.
      // ------------------------------------------

      if (delta > 0)
      {
         printA ("-> searching forwards  from:", timestamp, wibTs, wfOff);

         for (int idx = wfOff + 1;  idx < nTotFrames; idx++)
         {
            uint64_t wibTs = wf[idx].getTimestamp ();
            int64_t delta = timestamp - wibTs;

            if (delta < 25)
            {
               // -----------------------------------------------------
               // Don't go any further forward.
               // WIB frame end time has now past the target time
               //
               // Check if have went too far forward and searching for
               // the ending of the event window.
               // This can happen if the target WIB frame is missing
               // -----------------------------------------------------
               if (!begin && delta < 0)
               {
                  // -----------------------------------------------------
                  // Have went past the end point, backoff to the previous
                  // This will happen if the target WIB frame is missing
                  // -----------------------------------------------------
                  idx  -= 1;
                  wibTs = wf[idx].getTimestamp ();
               }

               m_wibOff = idx;
               m_pktNum = idx / nframesPerPkt;
               m_pktOff = idx % nframesPerPkt;
               m_wibTs  = wibTs;
               return 0;
            }
         }
      }
      else
      {
         printA ("-> searching backwards from:", timestamp, wibTs, wfOff);

         for (int idx = wfOff - 1;  idx >= 0; --idx)
         {
            wibTs = wf[idx].getTimestamp ();
            int64_t delta = timestamp - wibTs;
            if (delta >= 0)
            {
               // -------------------------------------------------
               // Don't go any further back
               // WIB timestamp now occurs before the target time
               //
               // Check if have went too far back and searching for
               // the beginning of the event window.
               // This can happen if the target WIB frame is missing
               // --------------------------------------------------
               if (begin && delta >= 25)
               {
                  // ---------------------------------------------------
                  // Have went past the beign point, backoff to previous
                  // which was within the e
                  // ---------------------------------------------------
                  idx += 1;
                  wibTs = wf[idx].getTimestamp ();
               }

               m_wibOff = idx;
               m_pktNum = idx / nframesPerPkt;
               m_pktOff = idx % nframesPerPkt;
               m_wibTs  = wibTs;
               return 0;
            }
         }
      }
   }


   // ------------------------------
   // Not found, set error condition
   // ------------------------------
   m_wibOff = 0;
   m_pktNum = 0;
   m_pktOff = 0;
   m_wibTs  = 0;

   return -1;
}
/* ---------------------------------------------------------------------- */



/* ====================================================================== */
#if TPCTRIMMEDRANGE_DEBUG
/* ---------------------------------------------------------------------- *//*!

  \brief Prints the parameters used in verifying of locating the target
         timestamp sample within the trimmed data sample

  \param[in]    title  An identifying title
  \param[in] targetTs  The target timestamp
  \param[in]    wibTs  The timestamp of WIB frame at the predict sample
  \param[in]   wibOff  The predicted offset into the untrimmed data
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void printA (char const  *title, 
                           uint64_t  targetTs,
                           uint64_t     wibTs,
                           int         wibOff)
{
   printf ("Locate : %16.16" PRIx64 " : %16.16" PRIx64 "%s %6u\n", 
           targetTs, wibTs, title, wibOff);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Print the location of the target timestamp within the trimmed
         data

  \param[in]    which  String giving the name of what is being located,
                       Something like "Begin", "End".
  \param[in]      loc  The location to print

                                                                          */
/* ---------------------------------------------------------------------- */
static void inline printB (char const                    *which, 
                           TpcTrimmedRange::Location const &loc)
{
   printf ("%7s  Displacement:%6" PRIu16
           " Pkt.Off = %4" PRIu16 ".%6" PRIu16 
           " [%15.15" PRIx64 " <= %15.15" PRIx64 " < %15.15" PRIx64 "]\n",

           which,
           loc.m_wibOff, 
           loc.m_pktNum,
           loc.m_pktOff,
           loc.m_wibTs,
           loc.m_tgtTs,
           loc.m_wibTs + 25);


   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Print the number of ticks in the trimmed data stream

  \param[in] nticks  The number of ticks in the trimmed data stream
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void printC (uint32_t nticks)
{
   printf ("NTicks %#6x:%5d\n", nticks, nticks);
   return;
}
/* ---------------------------------------------------------------------- */
#endif
/* ====================================================================== */
#undef TPCTRIMMEDRANGE_DEBUG
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
/* ---------------------------------------------------------------------- */
} /* END: namespace pdd                                                   */
/* ====================================================================== */

#endif
