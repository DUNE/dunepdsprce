// -*-Mode: C++;-*-

#ifndef TPCRANGES_IMPL_HH
#define TPCRANGES_IMPL_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcRanges-Impl.cc
 *  @brief    Proto-Dune Data Tpc Range Records, implementation
 *  @verbatim
 *                               Copyright 2013
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
 *  <2017/10/07>
 *
 * @par Credits:
 * SLAC
 *
 * @par
 * Implements the method giving access to the TpcRange records.
 * These are methods that, for performance reasons, are inline'd for 
 * internal use and made external for external use.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.08.07 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/access/TpcRanges.hh"
#include "dam/records/TpcRanges.hh"


namespace pdd    {
namespace access {


// --------------------------------
// Default implementation to inline
// --------------------------------
#ifndef TPCRANGES_IMPL
#define TPCRANGES_IMPL inline
#endif

/* ====================================================================== */
/* IMPLEMENTATION: TpcRangesBridge                                        */
/* ---------------------------------------------------------------------- *//*!

  \brief Return the format of the Tpc Ranges Record
                                                                          */
/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL unsigned int 
TpcRangesBridge::getRecordFormat (uint32_t bridge)
{
   using namespace pdd::record;
   unsigned int fmt = 
            PDD_EXTRACT32 (bridge,
                           pdd::record::TpcRangesHeader::Bridge::Mask  ::Format,
                           pdd::record::TpcRangesHeader::Bridge::Offset::Format);

   return fmt;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return the reserved field of Tpc Ranges bridge word

  \note
   This is provided for completeness and diagnostic/debugging purposes.
                                                                          */
/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL uint32_t TpcRangesBridge::getRangesReserved (uint32_t bridge)
{
   uint32_t rsvd = 
            PDD_EXTRACT32 (bridge,
                           pdd::record::TpcRangesHeader::Bridge::Mask  ::Reserved,
                           pdd::record::TpcRangesHeader::Bridge::Offset::Reserved);
   return rsvd;
}
/* ---------------------------------------------------------------------- */
/* END: TpcRangesBridge                                                   */
/* ====================================================================== */




/* ====================================================================== */
/* BEGIN: TpcRangesHeader                                                 */
/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL uint32_t 
TpcRangesHeader::getRecordFormat (pdd::record::TpcRangesHeader const *header)
{ 
   return getRecordFormat (header->retrieve ());
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL uint32_t 
TpcRangesHeader::getRangesReserved (pdd::record::TpcRangesHeader const *header) 
{ 
   return getRangesReserved (header->retrieve ());
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL uint32_t TpcRangesHeader::getRecordFormat (uint32_t w32)
{
   return PDD_EXTRACT32 (
         w32,   
         pdd::record::TpcRangesHeader::Bridge::Mask  ::Format, 
          static_cast<uint32_t>(Header2::      Offset::Bridge) 
        + static_cast<uint32_t>(pdd::record::
                      TpcRangesHeader::Bridge::Offset::Format));
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL uint32_t TpcRangesHeader::getRangesReserved (uint32_t w32)
{
   using namespace pdd::record;
   return PDD_EXTRACT32 (
           w32,  
           pdd::record::TpcRangesHeader::Bridge::Mask  ::Reserved, 
          static_cast<uint32_t>(Header2::        Offset::Bridge)
           + static_cast<uint32_t>(pdd::record::
                        TpcRangesHeader::Bridge::Offset::Reserved));
}
/* ---------------------------------------------------------------------- */
/* END: TpcRangesHeader                                                   */
/* ====================================================================== */





/* ====================================================================== */
/* BEGIN: TpcRangesBody                                                   */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the body of Tpc Ranges record body

  \param[in] ranges  A pointer to the Tpc Range record
                                                                          */
/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL 
TpcRangesBody::TpcRangesBody   (pdd::record::TpcRanges const *ranges) :
   m_body   (TpcRanges::      getBody   (ranges)),
   m_bridge (TpcRanges::getHeader (ranges)->getBridge     ()),
   m_nbytes (ranges->getNbytes () - sizeof (TpcRangesHeader))
{ 
   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL pdd::record::TpcRangesIndices const *
TpcRangesBody::getIndices (pdd::record::TpcRangesBody const *body,
                           uint32_t                        bridge)
{ 
   return &body->m_dsc.m_indices; 
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL pdd::record::TpcRangesTimestamps const *
TpcRangesBody::getTimestamps (pdd::record::TpcRangesBody const *body,
                              uint32_t                        bridge)
{ 
   return &body->m_dsc.m_timestamps; 
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL pdd::record::TpcRangesWindow const *
TpcRangesBody::getWindow (pdd::record::TpcRangesBody const *body,
                          uint32_t                        bridge)
{ 
   return &body->m_window; 
}
/* ---------------------------------------------------------------------- */
/* END: TpcRangesBody                                                     */
/* ====================================================================== */





/* ====================================================================== */
/* BEGIN: TpcRangesIndices                                                */
/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL uint32_t 
TpcRangesIndices::getBegin(pdd::record::TpcRangesIndices const *indices,
                           uint32_t                              bridge)

{ 
   return indices->m_begin; 
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL uint32_t 
TpcRangesIndices::getEnd (pdd::record::TpcRangesIndices const *indices,
                          uint32_t                              bridge)
{ 
   return indices->m_end; 
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL uint64_t 
TpcRangesIndices::getTrigger (pdd::record::TpcRangesIndices const *indices,
                              uint32_t                              bridge)
{ 
   return indices->m_trigger; 
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL int TpcRangesIndices::getPacket (uint32_t index)
{ 
   return index >> 16; 
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL int TpcRangesIndices::getOffset (uint32_t index) 
{ 
   return index & 0xffff;
}
/* ---------------------------------------------------------------------- */
/* END: TpcRangesIndics                                                   */
/* ====================================================================== */






/* ====================================================================== */
/* BEGIN: TpcRangesTimestamps                                             */
/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL uint64_t 
TpcRangesTimestamps::getBegin (pdd::record::TpcRangesTimestamps const *timestamps,
                               uint32_t                                    bridge)
{ 
   return timestamps->m_begin;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL uint64_t 
TpcRangesTimestamps::getEnd (pdd::record::TpcRangesTimestamps const *timestamps,
                             unsigned int                                format)
{ 
   return timestamps->m_end;
}
/* ---------------------------------------------------------------------- */
/* BEGIN: TpcRangesTimestamps                                             */
/* ====================================================================== */





/* ====================================================================== */
/* END:  TpcRangesWindow                                                  */
/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL uint64_t 
TpcRangesWindow::getBegin (pdd::record::TpcRangesWindow const *window,
                           uint32_t                            bridge)
{ 
   return window->m_begin; 
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL uint64_t
TpcRangesWindow::getEnd (pdd::record::TpcRangesWindow const *window,
                         unsigned int                        format)
{                          
   return window-> m_end;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL uint64_t 
TpcRangesWindow::getTrigger (pdd::record::TpcRangesWindow const *window,
                             uint32_t                            bridge)
{ 
   return window->m_trigger; 
}
/* ---------------------------------------------------------------------- */
/* END: TpcRangesWindow                                                   */
/* ====================================================================== */





/* ====================================================================== */
/* BEGIN: TpcRanges                                                       */
/* ---------------------------------------------------------------------- *//*!

  \brief Return the TPC Ranges bridge word as 32-bit immediate value.

  \param[in] ranges The TPC Ranges record 
                                                                          */
/* ---------------------------------------------------------------------- */  
TPCRANGES_IMPL uint32_t 
TpcRanges::getBridge (pdd::record::TpcRanges const *ranges)
{
   return ranges->getBridge ();
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer to the TpcRanges record header

  \param[in] ranges A pointer to the TpcRanges record
                                                                          */
/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL pdd::record::TpcRangesBody const *
TpcRanges::getBody (pdd::record::TpcRanges const *ranges) 
{
   return &ranges->m_body;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return the TpcRanges record format

  \param[in] ranges A pointer to the TpcRanges record
                                                                          */
/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL unsigned int 
TpcRanges::getRecordFormat  (pdd::record::TpcRanges const *ranges)
{
   uint32_t w32 = ranges->retrieve ();
   unsigned int fmt =
            PDD_EXTRACT32 
            (w32,
             pdd::record::TpcRanges::Bridge::Mask::Format,
             static_cast<uint32_t>(Header2::Offset::Bridge) 
           + static_cast<uint32_t>(pdd::record::TpcRanges::Bridge::Offset::Format));
   return fmt;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return the TpcRanges record bridge word's reserved field

  \param[in] ranges A pointer to the TpcRanges record

  \note
   This is provided for completeness and diagnostic/debugging reasons.
                                                                          */
/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL uint32_t 
TpcRanges::getRangesReserved(pdd::record::TpcRanges const *ranges)
{
   uint32_t w32 = ranges->retrieve ();
   unsigned int rsvd =
            PDD_EXTRACT32
            (w32,
             pdd::record::TpcRanges::Bridge::Mask::Reserved,
             static_cast<uint32_t>(Header2::Offset::Bridge) 
           + static_cast<uint32_t>(pdd::record::TpcRanges::Bridge::Offset::Reserved));
   return rsvd;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the TpcRanges Window class

  \param[in] ranges A pointer to the TpcRanges record
                                                                          */
/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL pdd::record::TpcRangesWindow const *
TpcRanges::getWindow (pdd::record::TpcRanges const *ranges)
{
   return &ranges->m_body.m_window;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer the TpcRanges packet indices class

  \param[in] ranges A pointer to the TpcRanges record
                                                                          */
/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL pdd::record::TpcRangesIndices const *
TpcRanges::getIndices (pdd::record::TpcRanges const *ranges)
{
   return &ranges->m_body.m_dsc.m_indices;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer the TpcRanges packet timestamps class

  \param[in] ranges A pointer to the TpcRanges record
                                                                          */
/* ---------------------------------------------------------------------- */
TPCRANGES_IMPL pdd::record::TpcRangesTimestamps const *
TpcRanges::getTimestamps (pdd::record::TpcRanges const *ranges)
{
   return &ranges->m_body.m_dsc.m_timestamps;
}
/* ---------------------------------------------------------------------- */
/*   END: TpcRanges                                                       */
} /* END: namespace access                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */


#endif
