// -*-Mode: C++;-*-

#ifndef TPCSTREAM_HH
#define TPCSTREAM_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcStream.hh
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
   2017.08.29 jjr Created
  
\* ---------------------------------------------------------------------- */

#include <cstdint>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace pdd      {
namespace fragment {
   class      Data;
   class TpcStream;
   class    Ranges;
   class       Toc;
   class TpcPacket;
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
   TpcStream (pdd::fragment::TpcStream const *stream);

public:
   void construct (pdd::fragment::TpcStream const *stream);

public:
   pdd::fragment::TpcStream const *getRecord () const;
   pdd::fragment::Ranges    const *getRanges () const;
   pdd::fragment::Toc       const *getToc    () const;
   pdd::fragment::TpcPacket const *getPacket () const;
   int                             getLeft   () const;
   uint32_t                        getCsf    () const;          

private:
   pdd::fragment::TpcStream const  *m_record;
   pdd::fragment::Ranges    const  *m_ranges;
   pdd::fragment::Toc       const     *m_toc;
   pdd::fragment::TpcPacket const  *m_packet;
};
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
/* ---------------------------------------------------------------------- */
} /* END: namespace pdd                                                   */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION: TpcNormal                                              */
/* ---------------------------------------------------------------------- */
namespace pdd    {
namespace access {
/* ---------------------------------------------------------------------- */
inline pdd::fragment::TpcStream  const *TpcStream::getRecord () const 
{ return  m_record; }

inline pdd::fragment::Ranges     const *TpcStream::getRanges () const 
{ return m_ranges;  }

inline pdd::fragment::Toc        const *TpcStream::getToc    () const 
{ return     m_toc; }

inline pdd::fragment::TpcPacket  const *TpcStream::getPacket () const
 { return  m_packet; }
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
/* ---------------------------------------------------------------------- */
} /* END: namespace pdd                                                   */
/* ====================================================================== */


#endif
