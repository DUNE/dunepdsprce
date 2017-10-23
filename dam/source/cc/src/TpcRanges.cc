// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcRanges.cc
 *  @brief    Proto-Dune Data Tpc Ranges Records
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
 *  <2017/10/16>
 *
 * @par Credits:
 * SLAC
 *
 * Implements the method giving access to the TpcRange records. Range
 * records gives the beginning and ending points of both the trimmed
 * and untrimmed data in terms of both timestamps and location within
 * the data.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.10.09 jjr Moved some methods to TpcRecords-Inline.hh for better
                  performance while making them external here for external
                  users.
   2017.08.07 jjr Created
  
\* ---------------------------------------------------------------------- */

#define   TPCRANGES_IMPL extern


#include "TpcRanges-Impl.hh"
#include  <cstdio>


namespace pdd    {
namespace access {



/* ---------------------------------------------------------------------- *//*!

  \brief  Print the Tpc Ranges record header

  \param[in] hdr Pointer to the Tpc Ranges record header
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcRangesHeader::print (pdd::record::TpcRangesHeader const *hdr)
{
   uint32_t w32     = hdr->retrieve   ();
   unsigned hdrFmt  = hdr->getFormat  (w32);
   unsigned typ     = hdr->getType    (w32);
   unsigned size    = hdr->getN64     (w32);
   unsigned recFmt  = getRecordFormat (w32);


   printf ("Ranges      :"
   "Format.Type.Version = %1.1x.%1.1x.%1.1x size %4.4x %8.8"
   "" PRIx32 "\n", hdrFmt, typ, recFmt, size,  w32);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Prints the body of a Tpc Ranges record. The \a format value 
         gives the specific layout

  \param[in] format  The format of this record
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcRangesBody::print (unsigned int format) const 
{ 
   print (m_body, format);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Prints the body of a Tpc Ranges record. The \a format value 
         gives the specific layout

  \param[in]   body  Pointer to the Tpc Ranges record
  \param[in] bridge  The bridge word
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcRangesBody::print (pdd::record::TpcRangesBody const *body, 
                           uint32_t                        bridge)
{
   TpcRangesWindow     const     window (getWindow     (body, bridge), bridge);
   TpcRangesIndices    const    indices (getIndices    (body, bridge), bridge);
   TpcRangesTimestamps const timestamps (getTimestamps (body, bridge), bridge);

   printf ("    Window  beg: %16.16" PRIx64 " end: %16.16" PRIx64 ""
           " trg: %16.16" PRIx64 "\n", 
           window.getBegin   (),
           window.getEnd     (),
           window.getTrigger ());

   printf ("  Untrimmed beg: %16.16" PRIx64 " end: %16.16" PRIx64 "\n",
           timestamps.getBegin (), 
           timestamps.getEnd   ());

   printf ("    Indices beg: %16.8" PRIx32 " end: %16.8" PRIx32 ""
                      " trg: %16.8" PRIx32 "\n", 
           indices.getBegin   (),
           indices.getEnd     (), 
           indices.getTrigger ());

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Prints the specified Tpc Ranges record

  \param[in] ranges The Tpc Ranges record to print
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcRanges::print (pdd::record::TpcRanges const *ranges)
{
   pdd::record::TpcRangesHeader const *hdr = getHeader(ranges);
   pdd::record::TpcRangesBody   const *bdy = getBody  (ranges);

   pdd::access::TpcRangesHeader::print (hdr);


   uint32_t bridge = hdr->getBridge ();

   TpcRangesBody::print (bdy, bridge);
}
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
} /* END: namespace pdd                                                   */
/* ---------------------------------------------------------------------- */

