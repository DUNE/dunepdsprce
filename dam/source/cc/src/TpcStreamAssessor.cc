// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcStreamAssessor,hh
 *  @brief    Proto-Dune Online/Offline Data Tpc Access Routines, to
 *            produce an error report
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
 *  @par Facility:
 *  pdd
 *
 *  @author
 *  <russell@slac.stanford.edu>
 *
 *  @par Date created:
 *  <2017/10/11>
 *
 * @par Credits:
 * SLAC
 *
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2018.10.10 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/TpcStreamAssessor.hh"
#include "dam/TpcStreamUnpack.hh"
#include "TpcTrimmedRange.hh"
#include "TpcStream-Impl.hh"
#include "TpcRanges-Impl.hh"
#include "TpcToc-Impl.hh"
#include "TpcPacket-Impl.hh"

#include <dam/access/WibFrame.hh>

#include <cstdint>
#include <stdio.h>
#include  <vector>



/* ---------------------------------------------------------------------- *//*!

   \brief Expected values for the ensuing frame based on the previous
          frame.
                                                                          */
/* ---------------------------------------------------------------------- */
class WibExpected
{
public:
   WibExpected ();

public:
   void   seed (uint8_t           version,  
                uint16_t            wibid);

   void update ( uint64_t       timestamp,
                 uint16_t const cvtcnt[2]);
      
public:
   uint64_t m_wibtimestamp; /*!< The predicted timestamp                  */
   uint16_t    m_cvtcnt[2]; /*!< The predicted convert counts, each stream*/
   uint8_t    m_wibversion; /*!< The expected version number              */
   uint16_t        m_wibid; /*!< The expected WIB crate.slot.fiber id     */
   bool            m_valid; /*!< Values are valid                         */

};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Seeds the static values
  
  \param[in]  version  The WIB version #
  \param[in]       id  The WIB identifier
                                                                          */
/* ---------------------------------------------------------------------- */
void  WibExpected::seed (uint8_t           version,  
                         uint16_t               id)
{
   m_wibversion = version;
   m_wibid      = id;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Updates the expected values 

  \param[in]  timestamp  Timestamp seed
  \param[in]     cvtcnt  Cold data stream convert count seeds
                                                                          */
/* ---------------------------------------------------------------------- */
void WibExpected::update (uint64_t      timestamp,
                          uint16_t const cvtcnt[2])

{
   m_wibtimestamp = timestamp + 25;
   m_cvtcnt[0]    = cvtcnt[0] +  1;
   m_cvtcnt[1]    = cvtcnt[1]  + 1;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief  TpcStreamAssessor destructor
                                                                          */
/* ---------------------------------------------------------------------- */
TpcStreamAssessor::~TpcStreamAssessor ()
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Assesses the TpcStream for any errors or abnormalities in the
          untrimmed data

   \param[in] tpcStream  The TPC stream to assess
                                                                          */
/* ---------------------------------------------------------------------- */
TpcStreamAssessor::Error_t 
TpcStreamAssessor::assessUntrimmed (TpcStreamUnpack const &tpcStream)
{
   using namespace pdd;
   using namespace pdd::access;

   TpcStreamUnpack::Identifier id = tpcStream.getIdentifier ();
   m_crate = id.getCrate ();
   m_slot  = id.getSlot  ();
   m_fiber = id.getFiber ();


   TpcStream const &stream = tpcStream.getStream     ();
   TpcToc           toc    (stream.getToc    ());
   TpcPacket        pktRec (stream.getPacket ());
   TpcPacketBody    pktBdy (pktRec.getRecord ());

   int   npkts = toc.getNPacketDscs ();

   WibExpected        expected;
   uint32_t     errSummary = 0;

   unsigned int smpNum = 0;
   for (int pktNum = 0; pktNum < npkts; ++pktNum)
   {
      TpcTocPacketDsc pktDsc (toc.getPacketDsc (pktNum));
      unsigned int    pktOff = pktDsc.getOffset64 ();
      unsigned int   pktType = pktDsc.getType ();
      WibFrame const     *wf = pktBdy.getWibFrames (pktType, pktOff);

      
      if (pktDsc.isWibFrame ())
      {
         unsigned int nframes = pktDsc.getNWibFrames ();

         for (unsigned int iwf = 0; iwf < nframes; ++iwf, ++wf)
         {
            TpcStreamAssessor::Record      rec;
            TpcStreamAssessor::Record::Error_t 
                               errs = rec.evaluateAndUpdate (&expected,
                                                             *wf);
            if (errs)
            {
               errSummary  |= errs;
               rec.m_smpNum = smpNum + iwf;
               rec.m_pktNum = pktNum;
               rec.m_frmNum = iwf;
               add (std::move (rec));
            }
         }
         smpNum += nframes;
      }
   }

   m_errsummary = errSummary;

   return errSummary;
}
/* ---------------------------------------------------------------------- */
   


/* ---------------------------------------------------------------------- *//*!

  \brief  Assesses the TpcStream for any errors or abnormalities in the
          untrimmed data

   \param[in] tpcStream  The TPC stream to assess
                                                                          */
/* ---------------------------------------------------------------------- */
TpcStreamAssessor::Error_t 
TpcStreamAssessor::assessTrimmed (TpcStreamUnpack const &tpcStream)
{
   using namespace pdd;
   using namespace pdd::access;

   TpcStreamUnpack::Identifier id = tpcStream.getIdentifier ();
   m_crate = id.getCrate ();
   m_slot  = id.getSlot  ();
   m_fiber = id.getFiber ();


   TpcStream const &stream = tpcStream.getStream ();
   TpcTrimmedRange trimmed (stream);


   TpcToc           toc    (stream.getToc    ());
   TpcPacket        pktRec (stream.getPacket ());
   TpcPacketBody    pktBdy (pktRec.getRecord ());

   int   npkts = toc.getNPacketDscs ();

   WibExpected        expected;
   uint32_t     errSummary = 0;


   unsigned int smpNum = 0;
   for (int pktNum = 0; pktNum < npkts; ++pktNum)
   {
      TpcTocPacketDsc pktDsc (toc.getPacketDsc (pktNum));
      unsigned int    pktOff = pktDsc.getOffset64 ();
      unsigned int   pktType = pktDsc.getType ();
      WibFrame const     *wf = pktBdy.getWibFrames (pktType, pktOff);

      
      if (pktDsc.isWibFrame ())
      {
         unsigned int nframes = pktDsc.getNWibFrames ();

         for (unsigned int iwf = 0; iwf < nframes; ++iwf, ++wf)
         {
            TpcStreamAssessor::Record      rec;
            TpcStreamAssessor::Record::Error_t 
                               errs = rec.evaluateAndUpdate (&expected,
                                                             *wf);
            if (errs)
            {
               errSummary  |= errs;
               rec.m_smpNum = smpNum + iwf;
               rec.m_pktNum = pktNum;
               rec.m_frmNum = iwf;
               add (std::move (rec));
            }
         }
         smpNum += nframes;
      }
   }

   m_errsummary = errSummary;

   return errSummary;
}
/* ---------------------------------------------------------------------- */

   


/* ---------------------------------------------------------------------- *//*!

  \brief Adds an error record

  \param[in] rec  The record to add
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcStreamAssessor::add (TpcStreamAssessor::Record const &&rec)
{
   m_recs.push_back (rec);
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Resets the TpcStreamAssessor to have no records and resets the
         state so that it needs to be seeded

  \param[in] rec  The record to add
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcStreamAssessor::reset ()
{
   m_recs.clear ();
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Return the next record statisfying the filter
  \return  A pointer to the found record or 0 if none

  \param[in]   idx   The index to start the search at
  \param[in] filter  The mask of errors to filter the search on
                                                                          */
/* ---------------------------------------------------------------------- */
TpcStreamAssessor::Record const *
TpcStreamAssessor::get (unsigned int idx, Error_t filter) const
{
   auto size = m_recs.size ();
   for (; idx < size; ++idx)
   {
      if (m_recs[idx].m_errors & filter)
      {
         return &m_recs[idx];
      }
   }
    
   return 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Report errors that match the filter

  \param[in] filter The mask of errors to filter the report on
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcStreamAssessor::report (Error_t filter) const
{
   unsigned int errCnt = m_recs.size ();

   static const char Separator[] = 
           "+---------------------+----------------------------------+--------------------------+--------------------------+";

   // ----------------------------
   //  Check if anything to report
   // ----------------------------
   if (errCnt == 0)
   {
      printf ("WibId = %2x.%1x.%1x -- No Errors\n", m_crate, m_slot, m_fiber);
      return;
   }
   puts   (Separator);
   printf ("|   WIB Id = %2x.%1x.%1x   |           WIB HEADER  INFO       |     COLD DATA STEAM 0    |    COLD DATA STEAM 1     |\n", 
           m_crate, m_slot, m_fiber);
   puts   (Separator);


   printf ("|Err# Pkt.Frame Sample|       Timestamp:Delta Rsvd WibErr|CvtCnt:Delta StrErr ErrReg|CvtCnt:Delta StrErr ErrReg|\n");
   puts   ("+---- ---.----- ------+----------------:----- ---- ------+------:----- ---.-- ------+------:----- ------ ------+");


   unsigned int errNum = 0;
   for (auto rec = m_recs.begin (); rec < m_recs.end (); ++errNum, ++rec)
   {
      uint32_t       errs = rec->m_errors;
      if ((errs & filter) == 0)  continue;

      unsigned int sample = rec->m_smpNum;

      printf ("|%4u %3u.%5u %6u|%16" PRIx64 ":", 
              errNum, rec->m_pktNum, rec->m_frmNum, sample, rec->m_wibtimestamp);

      uint32_t bm = TpcStreamAssessor::Record::ERR_M_WIB_TIMESTAMP;

      if (errs & bm)
      {
         printf ("%5" PRIx64, rec->m_wibdtimestamp);
         errs &= ~bm;
      }
      else
      {
         printf ("    .");
      }


      bm = TpcStreamAssessor::Record::ERR_M_WIB_RSVD;
      if (errs & bm)
      {
         printf (" %4" PRIx16, rec->m_wibrsvd);
         errs &= ~bm;
      }
      else
      {
         printf ("    .");
      }

      bm = TpcStreamAssessor::Record::ERR_M_WIB_ERRORS;
      if (errs & bm)
      {
         printf ("%7.4" PRIx16 "|", rec->m_wiberrors);
         errs &= ~bm;
      }
      else
      {
         printf ("       |");
      }

      int shift = TpcStreamAssessor::Record::ERR_V_CD_BEG;
      for (int icd = 0; icd < 2; shift += TpcStreamAssessor::Record::ERR_K_CD_CNT, ++icd)
      {
         uint32_t bm = TpcStreamAssessor::Record::ERR_M_CD_CVTCNT << shift;
         if (errs & bm)
         {
            printf ("%6" PRIx16 ":%5" PRIx16, 
                    rec->m_cd[icd].m_cvtcnt, 
                    rec->m_cd[icd].m_dcvtcnt&0x1ffff);
            errs &= ~bm;
         }
         else
         {
            printf ("      :    .");
         }


         bm = TpcStreamAssessor::Record::ERR_M_CD_STRERR << shift;
         if (errs & bm)
         {
            uint16_t stmerr = rec->m_cd[icd].m_stmerr;
            if (stmerr & 0xff00) printf ("  %2.2" PRIx16, stmerr >>   8);
            else                 printf ("    ");
            if (stmerr & 0x00ff) printf (":%2.2" PRIx16, stmerr & 0xff);
            else                 printf (": .");
            errs &= ~bm;
         }
         else
         {
            printf ("    : .");
         }

         bm = TpcStreamAssessor::Record::ERR_M_CD_ERRREG << shift;
         if (errs & bm)
         {
            printf (" %6" PRIx16, rec->m_cd[icd].m_errreg);
            errs &= ~bm;
         }
         else
         {
            printf ("       |");
         }

      }

      putchar ('\n');
   }

   puts   (Separator);


   return;
}
/* ---------------------------------------------------------------------- */
      



/* ---------------------------------------------------------------------- *//*!

  \brief Constructor of the ensuing frame predictions

                                                                          */
/* ---------------------------------------------------------------------- */
WibExpected::WibExpected () :
   m_wibtimestamp (0)
{
   m_cvtcnt[0] = 0;
   m_cvtcnt[1] = 0;
   m_valid     = false;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Evaluates the current values against the predictions and updates
          the predictions
  \return A bit mask of the errors

  \param[in]   expected  The expected values
  \param[in]         wf  The WIB frame to evaluate

                                                                          */
/* ---------------------------------------------------------------------- */
TpcStreamAssessor::Record::Error_t 
TpcStreamAssessor::Record::evaluateAndUpdate (WibExpected            *expected,
                                               pdd::access::WibFrame  const &wf)
{
   Error_t errs = 0;
   int    shift = ERR_V_CD_BEG;


   // --------------------------------
   // Get the cold data stream context
   //
   // The const & are necessary to make sure this is a reference
   // ----------------------------------------------------------
   auto const &colddata = wf.getColdData ();      
   for (unsigned icd = 0;  icd < pdd::access::WibFrame::NColdData; ++icd)
   {
      auto const   &cd = colddata[icd];
         
      auto hdr0           = cd.getHeader0      ();
      m_cd[icd].m_stmerr  = cd.getStreamErrs   (hdr0);
      m_cd[icd].m_cvtcnt  = cd.getConvertCount (hdr0);

      auto hdr1     = cd.getHeader1      ();
      m_cd[icd].m_errreg  = cd.getErrRegister  (hdr1);

      // --------------------------------------------------
      // These are absolute checks, so need for seed values
      // --------------------------------------------------
      if (m_cd[icd].m_stmerr != 0) errs |= (ERR_M_CD_STRERR  << shift);
      if (m_cd[icd].m_errreg != 0) errs |= (ERR_M_CD_ERRREG  << shift);
      if (m_cd[icd].m_rsvd   != 0) errs |= (ERR_M_CD_RSVD    << shift);

      shift += ERR_K_CD_CNT;
   }


   // -------------------------
   // Get the WIB frame context
   // -------------------------
   m_wibcomma     = wf.getCommaChar   ();
   m_wiberrors    = wf.getWibErrors   ();
   m_wibrsvd      = wf.getReserved    ();
   m_wibtimestamp = wf.getTimestamp   ();
   m_wibid        = wf.getId          ();
   m_wibversion   = wf.getVersion     ();


   // --------------------------------------------------
   // This is an absolute check, so need for seed values
   // --------------------------------------------------
   if (m_wibcomma  != 0xbc) errs |=  ERR_M_WIB_COMMA;
   if (m_wiberrors !=    0) errs |=  ERR_M_WIB_ERRORS;
   if (m_wibrsvd   !=    0) errs |=  ERR_M_WIB_RSVD;



   // -----------------------------------------------------
   // Can only check these values if have valid predictions
   // -----------------------------------------------------
   uint16_t cvtcnt[2] = { m_cd[0].m_cvtcnt, m_cd[1].m_cvtcnt };

   if (expected->m_valid)
   {
      if ((m_wibversion                      != expected->m_wibversion  )) errs |= ERR_M_WIB_VERSION;
      if ((m_wibid                           != expected->m_wibid       )) errs |= ERR_M_WIB_ID;
      if ((m_wibdtimestamp   = m_wibtimestamp - expected->m_wibtimestamp)) errs |= ERR_M_WIB_TIMESTAMP;
      if ((m_cd[0].m_dcvtcnt = cvtcnt[0]      - expected->m_cvtcnt[0]   )) errs |= ERR_M_CD0_CVTCNT;
      if ((m_cd[1].m_dcvtcnt = cvtcnt[1]      - expected->m_cvtcnt[1]   )) errs |= ERR_M_CD1_CVTCNT;
   }
   else
   {
      expected->seed (m_wibversion, m_wibid);
      expected->m_valid = true;
   }


   // ----------------------------------------------
   // Seed the new predictions based on these values
   // ----------------------------------------------
   expected->update (m_wibtimestamp, cvtcnt);

   m_errors = errs;

   return errs;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief Reports any errors

   \param[in]  errCnt Error count
   \param[in]  pktNum The 1024 packet number
   \param[in]  frmNum The frame number within the packet
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcStreamAssessor::Record::report (unsigned int errCnt, 
                                        unsigned int pktNum,
                                        unsigned int frmNum) const
{
   /// !!! KLUDGE
   return;
   printf ("Error %2d.%3d @ %4d"
           " %16.16" PRIx64 " != %16.16" PRIx64 ""
           " %4.4" PRIx32" : %5.5" PRIx32 ""
           " %4.4" PRIx32" : %5.5" PRIx32 "\n",
           errCnt,    pktNum, frmNum, 
           m_wibtimestamp, m_wibdtimestamp,
           m_cd[0].m_cvtcnt, m_cd[0].m_dcvtcnt,
           m_cd[1].m_cvtcnt, m_cd[1].m_dcvtcnt);

   return;
}
/* ---------------------------------------------------------------------- */
