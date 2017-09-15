// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcRecords.cc
 *  @brief    Proto-Dune Data Tpc Data Records
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
 * This layout the format and primitive access methods to the data
 * found in a TpcNormal and TpcDamaged records.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.08.07 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/TpcRecords.hh"
#include  <cstdio>


namespace pdd      {
namespace fragment {



/* ---------------------------------------------------------------------- */
void TpcStreamHeader::print () const 
{ 
   print (this); 
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
void TpcStreamHeader::print (DataHeader const *dh)
{
   uint64_t         hdr = dh->retrieve ();
   unsigned int  format = DataHeader::getFormat (hdr);
   unsigned int    type = DataHeader::getType   (hdr);
   unsigned int     n64 = DataHeader::getN64    (hdr);
   uint32_t      bridge = DataHeader::getBridge (hdr);
   
   printf ("%-10.10s: Type.Format = %1.1x.%1.1x Length = %6.6x Bridge = %8.8" 
           PRIx32 "\n",
           "DataRecord",
           type,
           format,
           n64,
           bridge);
   
   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
uint32_t RangesHeader::getVersion (uint32_t w32)
{
   return PDD_EXTRACT32 (w32, Mask::Version, 
                         static_cast<uint32_t>(Header2::Offset::Bridge) 
                       + static_cast<uint32_t>(Offset::Version));
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
uint32_t RangesHeader::getReserved (uint32_t w32)
{
   return PDD_EXTRACT32 (w32, Mask::Reserved, 
                         static_cast<uint32_t>(Header2::Offset::Bridge)
                       + static_cast<uint32_t>(Offset::Reserved));
/* ---------------------------------------------------------------------- */
}



/* ---------------------------------------------------------------------- */
void RangesHeader::print (RangesHeader const *hdr)
{
   uint32_t w32     = hdr->retrieve ();
   unsigned fmt     = hdr->getFormat  (w32);
   unsigned typ     = hdr->getType    (w32);
   unsigned size    = hdr->getN64     (w32);
   unsigned ver     = hdr->getVersion (w32);


   printf ("Ranges      :"
   "Format.Type.Version = %1.1x.%1.1x.%1.1x size %4.4x %8.8"
   "" PRIx32 "\n", fmt, typ, ver, size,  w32);
   return;
}
/* ---------------------------------------------------------------------- */


void RangesBody::print (RangesBody const *body, unsigned int format)
{
   Window                 const     *window = body->getWindow ();
   Descriptor             const        *dsc = body->getDescriptor ();
   Descriptor::Indices    const    *indices =  dsc->getIndices    ();
   Descriptor::Timestamps const *timestamps =  dsc->getTimestamps ();

   printf ("    Window  beg: %16.16" PRIx64 " end: %16.16" PRIx64 ""
           " trg: %16.16" PRIx64 "\n", 
           window->getBegin   (),
           window->getEnd     (),
           window->getTrigger ());

   printf ("  Untrimmed beg: %16.16" PRIx64 " end: %16.16" PRIx64 "\n",
           timestamps->getBegin (), 
           timestamps->getEnd   ());

   printf ("    Indices beg: %16.8" PRIx32 " end: %16.8" PRIx32 ""
                      " trg: %16.8" PRIx32 "\n", 
           indices->getBegin   (),
           indices->getEnd     (), 
           indices->getTrigger ());

   return;
}
/* ---------------------------------------------------------------------- */


#if DONT

/* ---------------------------------------------------------------------- */
uint32_t TocHeader::getTocFormat (uint32_t w32)
{
   return PDD_EXTRACT32 (w32, Mask::TocFormat, 
                         static_cast<uint32_t>(Header2::Offset::Bridge) 
                       + static_cast<uint32_t>(Offset::TocFormat));
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
uint32_t TocHeader::getReserved (uint32_t w32)
{
   return PDD_EXTRACT32 (w32, Mask::NCtbs, 
                         static_cast<uint32_t>(Header2::Offset::Bridge)
                       + static_cast<uint32_t>(Offset::Reserved));
}
/* ---------------------------------------------------------------------- */

#endif

 
/* ---------------------------------------------------------------------- */
void TocHeader::print (TocHeader const *hdr)
{
   uint32_t w32     = hdr->retrieve ();
   unsigned fmt     = hdr->getFormat    (w32);
   unsigned typ     = hdr->getType      (w32);
   unsigned size    = hdr->getN64       (w32);
   unsigned tocFmt  = hdr->getTocFormat (w32);


   //uint32_t const *pkt_ptr = ctb_ptr + nctbs;

   printf ("Toc        :"
   "Format.Type.Version = %1.1x.%1.1x.%1.1x size %4.4x %8.8"
   "" PRIx32 "\n",
           fmt,
           typ,
           tocFmt,
           size,
           w32);
   return;
}


void TocBody::print (TocBody const *body, uint32_t bridge)
{
   int               ndscs = TocHeader::Bridge::getNDscs (bridge);
   PacketDsc   const *dscs = body->getPacketDscs ();


   PacketDsc           dsc = *dscs;
   unsigned            o64 =   dsc.getOffset64 ();


   // --------------------------------------------
   // Iterate over the packets in this contributor
   // --------------------------------------------
   for (int idsc = 0; idsc < ndscs; idsc++)
   {
      unsigned        format = dsc.getFormat   ();
      PacketDsc::Type   type = dsc.getType     ();
      unsigned      offset64 = o64;

      // -------------------------------------------------------------
      // Get the length via the difference of this offset and the next
      // -------------------------------------------------------------
      dsc               = *dscs++;
      o64               = dsc.getOffset64 ();
      unsigned int  n64 = o64 - offset64;

      printf ("           %2d. %1.x.%1.1x %6.6x %6.6x %8.8" PRIx32 "\n", 
              idsc, 
              format,
              static_cast<unsigned int>(type),
              offset64,
              n64,
              dsc.getW32 ());
   }

   return;
}
/* ---------------------------------------------------------------------- */
} /* END: namespace fragment                                              */
} /* END: namespace pdd                                                   */
/* ---------------------------------------------------------------------- */

