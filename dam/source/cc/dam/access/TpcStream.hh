// -*-Mode: C++;-*-

#ifndef ACCESS_TPCSTREAM_HH
#define ACCESS_TPCSTREAM_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     access/TpcStream.hh
 *  @brief    Access methods for the raw RCE Tpc Stream data records
 *
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
   2017.10.23 jjr Padded the data section with 4 reserved words to be used
                  for future use. 
   2017.10.12 jjr Moved the record definition -> records/TpcStream.hh
   2017.08.29 jjr Created
  
\* ---------------------------------------------------------------------- */

#include "dam/access/Headers.hh"
//#include "dam/records/Data.hh"    // !!! KLUDGE -- needs to be removed
#include <cstdint>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace pdd    {
namespace record {

   class TpcStreamHeader;
   class       TpcStream;
   class       TpcRanges;
   class          TpcToc;
   class       TpcPacket;
}}
/* ====================================================================== */



/* ====================================================================== */
/* DEFINITION: TpcStream                                                  */
/* ---------------------------------------------------------------------- */
namespace pdd    {
namespace access {


/* ---------------------------------------------------------------------- *//*!

  \brief Class to access the Tpc Stream Record and its subrecords
                                                                          */
/* ---------------------------------------------------------------------- */ 
class TpcStream 
{
public:
   explicit TpcStream () { return; }
   TpcStream (pdd::record::TpcStream const *stream);

public:
   void construct (pdd::record::TpcStream const *stream);

public:
   pdd::record::TpcStream       const *getRecord () const;
   pdd::record::TpcStreamHeader const *getHeader () const;
   pdd::record::TpcRanges       const *getRanges () const;
   pdd::record::TpcToc          const *getToc    () const;
   pdd::record::TpcPacket       const *getPacket () const;
   int                                 getLeft   () const;
   uint32_t                            getCsf    () const;          


private:
   pdd::record::TpcStream       const   *m_stream; /*!< The raw TPC stream*/
   pdd::record::TpcRanges       const   *m_ranges; /*!< Time/Packet Ranges*/
   pdd::record::TpcToc          const      *m_toc; /*!< Table of Contents */
   pdd::record::TpcPacket       const   *m_packet; /*!< The data packets  */
   void                         const  *m_rsvd[4]; /*!< Future use        */
};
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
/* ---------------------------------------------------------------------- */
} /* END: namespace pdd                                                   */
/* ====================================================================== */







/* ====================================================================== */
/* IMPLEMENTATION: TpcStream                                              */
/* ---------------------------------------------------------------------- */
namespace pdd    {
namespace access {
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcStream       const *TpcStream::getRecord () const 
{ return  m_stream; }

inline pdd::record::TpcStreamHeader const *TpcStream::getHeader () const 
{ return  reinterpret_cast<pdd::record::TpcStreamHeader const *>(m_stream); }

inline pdd::record::TpcRanges       const *TpcStream::getRanges () const 
{ return m_ranges;  }

inline pdd::record::TpcToc          const *TpcStream::getToc    () const 
{ return     m_toc; }

inline pdd::record::TpcPacket       const *TpcStream::getPacket () const
 { return  m_packet; }
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
/* ---------------------------------------------------------------------- */
} /* END: namespace pdd                                                   */
/* ====================================================================== */


#endif
