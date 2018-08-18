// -*-Mode: C++;-*-


/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     Identifier.cc
 *  @brief    Indentifier Record implementation
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
 *  @par Facility:
 *  proto-dune DAM 
 *
 *  @author
 *  <russell@slac.stanford.edu>
 *
 *  @par Date created:
 *  <2017/10/12>
 *
 * @par Credits:
 * SLAC
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2018.08.14 jjr Correct extraction of crate.slot.fiber in print
   2017.10.12 jjr Separated from Headers.hh
  
\* ---------------------------------------------------------------------- */


#include "dam/access/Identifier.hh"
#include "dam/records/Identifier.hh"
#include "dam/util/BfExtract.hh"

#include <cstdio>


/* ====================================================================== */
/* IMPLEMENTATION : Identifier                                            */
/* ---------------------------------------------------------------------- */
namespace pdd     {
namespace access  {


uint32_t Identifier::getFormat (pdd::record::Identifier const *id)
{
   uint64_t w64 = id->m_w64;
   return PDD_EXTRACT64 (w64,  
                         pdd::record::Identifier::Mask  ::Format, 
                         pdd::record::Identifier::Offset::Format);
}

uint32_t Identifier::getType (pdd::record::Identifier const *id)
{
   uint64_t w64 = id->m_w64;
   return PDD_EXTRACT64 (w64,
                         pdd::record::Identifier::Mask  ::Type, 
                         pdd::record::Identifier::Offset::Type);
}

uint32_t Identifier::getSrc0 (pdd::record::Identifier const *id)
{
   uint64_t w64 = id->m_w64;
   return PDD_EXTRACT64 (w64, 
                         pdd::record::Identifier::Mask  ::Src0,
                         pdd::record::Identifier::Offset::Src0);
}

uint32_t Identifier::getSrc1 (pdd::record::Identifier const *id)
{
   uint64_t w64 = id->m_w64;
   return PDD_EXTRACT64 (w64, 
                         pdd::record::Identifier::Mask  ::Src1,
                         pdd::record::Identifier::Offset::Src1);
}

uint32_t Identifier::getSrc (pdd::record::Identifier const *id, int idx)
{
   return idx ? getSrc0 (id) : getSrc1 (id);
}

uint32_t Identifier::getSequence (pdd::record::Identifier const *id)
{   
   uint64_t w64 = id->m_w64;
   return PDD_EXTRACT64 (w64, 
                         pdd::record::Identifier::Mask  ::Sequence, 
                         pdd::record::Identifier::Offset::Sequence);
}

uint64_t Identifier::getTimestamp(pdd::record::Identifier const *id)
{
   return id->m_timestamp;
}



/* ====================================================================== */
/* IMPLEMENTATION : Identifier                                            */
/* ---------------------------------------------------------------------- *//*!
   
   \brief Prints the Identifier Block

   \param[in]  identifier The identifier block to print
                                                                          */
/* ---------------------------------------------------------------------- */
void Identifier::print (Identifier const *identifier)
{
   auto            id = identifier->m_id;
   uint64_t w64       = id->m_w64;
   uint64_t timestamp = getTimestamp (id);
   printf ("Identifier: %16.16" PRIx64 " %16.16" PRIx64 "\n",
           w64, timestamp);
   
   unsigned format    = getFormat   (id);
   unsigned src0      = getSrc0     (id);
   unsigned type      = getType     (id);
   unsigned src1      = getSrc1     (id);
   unsigned sequence  = getSequence (id);

   

   unsigned s0 = (src0 >> 8) & 0x7;
   unsigned c0 = (src0 >> 3) & 0x1f;
   unsigned f0 = (src0 >> 0) & 0x7;
   

   unsigned s1 = (src1 >> 8) & 0x7;
   unsigned c1 = (src1 >> 3) & 0x1f;
   unsigned f1 = (src1 >> 0) & 0x7;

   
   printf (
      "            Format.Type = 0x%1.1x.%1.1x Srcs = 0x%1x.%1x.%1x : 0x%1x.%1x.%1x\n"
      "            Timestamp   = 0x%16.16" PRIx64 " Sequence = 0x%8.8" PRIx32 "\n",
      format, type, c0, s0, f0, c1, s1, f1,
      timestamp, sequence);

   return;
}
/* ---------------------------------------------------------------------- */
/*   END: Identifier                                                      */
/* ====================================================================== */
} /* END: namespace: access                                               */
} /* END: namespace: pdd                                                  */
/* ====================================================================== */
