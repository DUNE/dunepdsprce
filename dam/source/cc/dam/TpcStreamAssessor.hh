// -*-Mode: C++;-*-

#ifndef PDD_TPCSTREAMASSESSOR_HH
#define PDD_TPCSTREAMASSESSOR_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcStreamAssessor,hh
 *  @brief    Proto-Dune Online/Offline Data Tpc Access Routines, to
 *            assess any errors/abnormalities in a tpc stream
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
   2018.10.11 jjr Created
  
\* ---------------------------------------------------------------------- */


#include <cstdint>
#include  <vector>


class TpcStreamUnpack;
class WibErrorRecord;
class WibExpected;

namespace pdd    { 
namespace access {
   class WibFrame;
}}

/* ---------------------------------------------------------------------- *//*!

  brief Manages a vector of WibErrorRecords
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcStreamAssessor 
{
public:
   TpcStreamAssessor  ()         { return; }
  ~TpcStreamAssessor  ();


   /* ------------------------------------------------------------------- *//*!

      \brief The complete error record
                                                                          */
   /* ------------------------------------------------------------------- */
   class Record
   {
   public:
      Record () { return; }

      /* ------------------------------------------------------------------ *//*!

         \brief Maps out the error bits
                                                                            */
      /* ------------------------------------------------------------------ */
      enum Error
      {
         ERR_M_WIB_COMMA     = (1 << 0), /*!< Comma character error         */
         ERR_M_WIB_VERSION   = (1 << 1), /*!< Inconsistent version          */
         ERR_M_WIB_ID        = (1 << 2), /*!< WIB Identification mismatch   */
         ERR_M_WIB_RSVD      = (1 << 3), /*!< Mismatched converts/bad synch */
         ERR_M_WIB_ERRORS    = (1 << 4), /*!< Wib error word non-zero       */
         ERR_M_WIB_TIMESTAMP = (1 << 5), /*!< Mismatch timestamp            */
         ERR_M_WIB_RSVD6     = (1 << 6), /*!< Reserved                      */
         ERR_M_WIB_RSVD7     = (1 << 7), /*!< Reserved                      */

         ERR_V_CD_BEG     = 8,
         ERR_M_CD_STRERR  = (1 << 0), /*!< Colddata stream 0, stream error  */
         ERR_M_CD_RSVD1   = (1 << 1), /*!< Reserved field was eliminated    */
         ERR_M_CD_CVTCNT  = (1 << 2), /*!< Colddata stream 0, convert count */
         ERR_M_CD_ERRREG  = (1 << 3), /*!< Colddata stream 0, error register*/
         ERR_M_CD_RSVD    = (1 << 4), /*!< Reserved field non-zero          */
         ERR_M_CD_HDR     = (1 << 5), /*!< Headers are not checked          */
         ERR_M_CD_RSVD6   = (1 << 6), /*!< Reserved                         */
         ERR_M_CD_RSVD7   = (1 << 6), /*!< Reserved                         */

         ERR_K_CD_CNT     = 8,        /*!< Number of colddata stream errors */


         // Cold Data Stream 0 error bits
         ERR_V_CD0_BEG    = ERR_V_CD_BEG,
         ERR_M_CD0_STRERR = ERR_M_CD_STRERR << ERR_V_CD0_BEG,
         ERR_M_CD0_ERRREG = ERR_M_CD_ERRREG << ERR_V_CD0_BEG,
         ERR_M_CD0_RSVD1  = ERR_M_CD_RSVD1  << ERR_V_CD0_BEG,
         ERR_M_CD0_CVTCNT = ERR_M_CD_CVTCNT << ERR_V_CD0_BEG,
         ERR_M_CD0_RSVD   = ERR_M_CD_RSVD   << ERR_V_CD0_BEG,
         ERR_M_CD0_HDR    = ERR_M_CD_HDR    << ERR_V_CD0_BEG,
         ERR_M_CD0_RSVD6  = ERR_M_CD_RSVD6  << ERR_V_CD0_BEG,
         ERR_M_CD0_RSVD7  = ERR_M_CD_RSVD7  << ERR_V_CD0_BEG,
         ERR_V_CD0_END    = ERR_V_CD0_BEG    +  ERR_K_CD_CNT,

         
         // Cold Data Stream 1 error bits
         ERR_V_CD1_BEG    = ERR_V_CD_BEG + ERR_K_CD_CNT,
         ERR_M_CD1_STRERR = ERR_M_CD_STRERR << ERR_V_CD1_BEG,
         ERR_M_CD1_ERRREG = ERR_M_CD_ERRREG << ERR_V_CD1_BEG,
         ERR_M_CD1_RSVD1  = ERR_M_CD_RSVD1  << ERR_V_CD1_BEG,
         ERR_M_CD1_CVTCNT = ERR_M_CD_CVTCNT << ERR_V_CD1_BEG,
         ERR_M_CD1_RSVD   = ERR_M_CD_RSVD   << ERR_V_CD1_BEG,
         ERR_M_CD1_HDR    = ERR_M_CD_HDR    << ERR_V_CD1_BEG,
         ERR_M_CD1_RSVD6  = ERR_M_CD_RSVD6  << ERR_V_CD1_BEG,
         ERR_M_CD1_RSVD7  = ERR_M_CD_RSVD7  << ERR_V_CD1_BEG,
         ERR_V_CD1_END    = ERR_V_CD1_BEG    + ERR_K_CD_CNT
      };


   public:
      typedef uint32_t Error_t;
      Error_t evaluateAndUpdate (WibExpected           *expected, 
                                 pdd::access::WibFrame const &wf);

      void               report (unsigned int             errCnt, 
                                 unsigned int             pktNum,
                                 unsigned int             frmNum) const;

      void report (unsigned int errCnt);

      struct ColdData
      {
         uint16_t  m_cvtcnt; /*!< Evaluated cold data stream convert counts */
         uint16_t  m_errreg; /*!< Evaluated cold data stream error register */
         uint16_t  m_stmerr; /*!< Evaluated cold data stream errors         */
         uint16_t    m_rsvd; /*!< Reserved word not 0                       */
         int32_t  m_dcvtcnt; /*!< Difference in the convert counts          */
         int32_t      m_pad; /*!< Pad to 64-bit boundary                    */
      } __attribute__ ((packed));


   public:
      uint16_t     m_pktNum; /*!< The packet number of the error            */
      uint16_t     m_frmNum; /*!< The frame  number of the packet           */
      uint32_t     m_smpNum; /*!< The sample number                         */
      uint32_t     m_errors; /*!< The error mask                            */
      uint32_t        m_pad; /*!< Pad to 64-bit boundary                    */

      uint8_t      m_wibcomma; /*!< The found comma character               */
      uint8_t    m_wibversion; /*!< The found version number                */
      uint16_t        m_wibid; /*!< The found WIB ID                        */
      uint32_t      m_wibrsvd; /*!< The found WIB reserved field (24-bits)  */
      uint16_t    m_wiberrors; /*!< The found WIB error    field (16-bits)  */
      uint16_t    m_wibpad[3]; /*!< Round to 64-bit boundary                */
      uint64_t m_wibtimestamp; /*!< Evaluated timestamp                     */
      int64_t m_wibdtimestamp; /*!< Difference in the timestamp             */


      ColdData      m_cd[2]; /*!< Cold data link errors/context             */
   } __attribute ((packed));
   /* --------------------------------------------------------------------- */


public:
   typedef Record::Error_t  Error_t;

   /* --------------------------------------------------------------------- *//*!

      \brief Predefined filtering masks
                                                                            */
   /* --------------------------------------------------------------------- */
      enum Filters
      {
         FLT_M_ALL            = 0xffffffff,
         /*!< Report all errors                                             */
         
         FLT_M_WIB_TIMESTAMPS = Record::ERR_M_WIB_TIMESTAMP, 
         /*!< Only timestamp errors                                         */

         FLT_M_CD_CVTCNTS     = Record::ERR_M_CD0_CVTCNT 
                              | Record::ERR_M_CD1_CVTCNT,
         /*!< Only convert count errors                                     */
         
         FLT_M_TIMING         = FLT_M_WIB_TIMESTAMPS | FLT_M_CD_CVTCNTS,    
         /*!< All timing types                                              */
 
         FLT_M_CD_STRERRS     = Record::ERR_M_CD0_STRERR 
                              | Record::ERR_M_CD1_STRERR
         /*!< All CD stream error                                           */
      };
      /* ------------------------------------------------------------------ */



public:
   void    reset           ();
   Error_t assessUntrimmed (TpcStreamUnpack const *tpc);
   Error_t assessUntrimmed (TpcStreamUnpack const &tpc);

   Error_t assessTrimmed   (TpcStreamUnpack const *tpc);
   Error_t assessTrimmed   (TpcStreamUnpack const &tpc);

   void    add             (TpcStreamAssessor::Record const &&rec);

   TpcStreamAssessor::Record const *get (unsigned int idx) const;
   TpcStreamAssessor::Record const *get (unsigned int idx, Error_t filter) const;


   void    report (Error_t filter) const;
   void    report ()               const;

public:
   std::vector<Record> m_recs;
   unsigned char      m_crate;
   unsigned char       m_slot;
   unsigned char      m_fiber;
   Error_t       m_errsummary;
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Convenience routine to accept a pointer rather than a reference
  \return A bit list of the summary of all errors, 0 is no error
  
  \param[in]  tpc  The TpcStreamUnpack to assess

  \par
   This assesses all the WibFrames
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcStreamAssessor::Error_t 
       TpcStreamAssessor::assessUntrimmed (TpcStreamUnpack const *tpc)
{
   return assessUntrimmed (*tpc);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Convenience routine to accept a pointer rather than a reference
  \return A bit list of the summary of all errors, 0 is no error
  
  \param[in]  tpc  The TpcStreamUnpack to assess

  \par
   This assesses all the WibFrames
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcStreamAssessor::Error_t 
       TpcStreamAssessor::assessTrimmed (TpcStreamUnpack const *tpc)
{
   return assessTrimmed (*tpc);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the next error record starting at the specified index
  \retval Pointer to the record or NULL if none

  \param[in] idx  The index to start the search at 
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcStreamAssessor::Record const *
       TpcStreamAssessor::get (unsigned int idx) const
{
   TpcStreamAssessor::Record const *rec = get (idx, FLT_M_ALL);
   return rec;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Report all errors
                                                                          */
/* ---------------------------------------------------------------------- */
inline void TpcStreamAssessor::report () const
{
   report (FLT_M_ALL);
}
/* ---------------------------------------------------------------------- */





#endif
