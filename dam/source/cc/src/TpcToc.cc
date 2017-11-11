// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcTpc.cc
 *  @brief    Proto-Dune Data Tpc Table Of Contents Records
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
 *  <2017/08/07>
 *
 * @par Credits:
 * SLAC
 *
 * Implement access methods to the the Tpc Table of Contents record.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.10.16 jjr Separated from TpcRecords.cc
   2017.08.07 jjr Created
  
\* ---------------------------------------------------------------------- */

#define   TPCTOC_IMPL extern


#include "TpcToc-Impl.hh"
#include  <cstdio>


namespace pdd    {
namespace access {


/* ---------------------------------------------------------------------- *//*!

  \brief Print the specified TOC record header

  \param[in] hdr  Pointer to the TOC record header to print
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcTocHeader::print (pdd::record::TpcTocHeader const *hdr)
{
   uint32_t w32     = hdr->retrieve       ();
   unsigned hdrFmt  = hdr->getFormat      (w32);
   unsigned typ     = hdr->getType        (w32);
   unsigned size    = hdr->getN64         (w32);
   unsigned tocFmt  = pdd::access::TpcTocHeader::getRecordFormat (w32);
   unsigned ndscs   = pdd::access::TpcTocHeader::getNPacketDscs  (w32);


   printf ("Toc        :"
   "Format.Type.TocFmt = %1.1x.%1.1x.%1.1x ndscs = %2.2x size %4.4x %8.8"
   "" PRIx32 "\n",
           hdrFmt,
           typ,
           tocFmt,
           ndscs,
           size,
           w32);
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Prints the specified TPC Table of Contents record

  \param[in]  toc The TPC Table Of Contents record to print
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcToc::print (pdd::record::TpcToc const *toc)
{
   pdd::record::TpcTocHeader const *hdr = getHeader (toc);
   TpcTocHeader::print (hdr);

   uint32_t bridge  = hdr->getBridge (); 
   TpcTocBody::print (getBody (toc), bridge);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Prints the body of TPC Table of Contents record

  \param[in]   body  Pointer to the TPC table of contents record to print
  \param[in] bridge  The record's bridge word. This contains information
                     necessary to interpret the record.
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcTocBody::print (pdd::record::TpcTocBody const *body, uint32_t bridge)
{
   int                                ndscs = TpcTocBridge::getNPacketDscs (bridge);
   pdd::record::TpcTocPacketDsc const *dscs = TpcTocBody  ::getPacketDscs  (body);


   pdd::record::TpcTocPacketDsc dsc = *dscs++;
   unsigned                     o64 = TpcTocPacketDsc::getOffset64 (dsc);


   // Header
   puts ("            Idx. F.T  Off64  Len64   RawDsc"); 

   // --------------------------------------------
   // Iterate over the packets in this contributor
   // --------------------------------------------
   for (int idsc = 0; ; idsc++)
   {
      unsigned       format = TpcTocPacketDsc::getFormat   (dsc);
      unsigned int     type = TpcTocPacketDsc::getType     (dsc);
      unsigned     offset64 = o64;

      // -------------------------------------------------------------
      // Get the length via the difference of this offset and the next
      // -------------------------------------------------------------
      dsc               = *dscs++;
      o64               = TpcTocPacketDsc::getOffset64 (dsc);

      // -----------------------------------------------
      // Check for and format the terminating descriptor
      // -----------------------------------------------
      if (idsc == ndscs)
      {
         printf ("             %2d. %1.1x.%1.1x %6.6x Terminator\n", 
                 idsc, 
                 format,
                 type,
                 offset64);
         break;
      }


      unsigned int  n64 = o64 - offset64;
      printf ("             %2d. %1.1x.%1.1x %6.6x %6.6x %8.8" PRIx32 "\n", 
              idsc, 
              format,
              type,
              offset64,
              n64,
              static_cast<uint32_t>(dsc.m_w32));
   }

   return;
}
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
} /* END: namespace pdd                                                   */
/* ---------------------------------------------------------------------- */

