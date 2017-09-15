// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     Headers.cc
 *  @brief    Proto-Dune Data Header 
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
 *  <2017/09/01>
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
   2017.09.01 jjr Created - separated from Headers.hh
  
\* ---------------------------------------------------------------------- */


#include <dam/Headers.hh>
#include <cinttypes>
#include <cstdio>

namespace pdd      {
namespace fragment {

/* ====================================================================== */
/* IMPLEMENTATION : Identifier                                            */
/* ---------------------------------------------------------------------- *//*!
   
   \brief Prints the Identifier Block

   \param[in]  identifier The identifier block to print
                                                                          */
/* ---------------------------------------------------------------------- */
void Identifier::print (Identifier const *identifier)
{
   uint64_t       w64 = identifier->m_w64;
   uint64_t timestamp = identifier->m_timestamp;
   printf ("Identifier: %16.16" PRIx64 " %16.16" PRIx64 "\n",
           w64, timestamp);
   
   unsigned format   = getFormat   (w64);
   unsigned src0     = getSrc0     (w64);
   unsigned type     = getType     (w64);
   unsigned src1     = getSrc1     (w64);
   unsigned sequence = getSequence (w64);
   
   unsigned c0 = (src0 >> 6) & 0x1f;
   unsigned s0 = (src0 >> 3) & 0x7;
   unsigned f0 = (src0 >> 0) & 0x7;
   
   unsigned c1 = (src1 >> 6) & 0x1f;
   unsigned s1 = (src1 >> 3) & 0x7;
   unsigned f1 = (src1 >> 0) & 0x7;

   
   printf (
      "            Format.Type = %1.1x.%1.1x Srcs = %1x.%1x.%1x : %1x.%1x.%1x\n"
      "            Timestamp   = %16.16" PRIx64 " Sequence = %8.8" PRIx32 "\n",
      format, type, c0, s0, f0, c1, s1, f1,
      timestamp, sequence);

   return;
}
/* ---------------------------------------------------------------------- */
/* END: Identifier                                                        */
/* ====================================================================== */



/* ====================================================================== */
/* OriginatorBody                                                         */
/* ---------------------------------------------------------------------- */
void OriginatorBody::print (OriginatorBody const *body)
{
   uint32_t        location = body->getLocation     ();
   Versions const &versions = body->getVersions     ();
   uint64_t    serialNumber = body->getSerialNumber ();
   char const     *rptSwTag = body->getRptSwTag     ();
   char const    *groupName = body->getGroupName    ();

   uint32_t        software = versions.getSoftware  ();
   uint32_t        firmware = versions.getFirmware  ();
   

   unsigned          slot = (location >> 16) & 0xff;
   unsigned           bay = (location >>  8) & 0xff;
   unsigned       element = (location >>  0) & 0xff;
      
   uint8_t          major = (software >> 24) & 0xff;
   uint8_t          minor = (software >> 16) & 0xff;
   uint8_t          patch = (software >>  8) & 0xff;
   uint8_t        release = (software >>  0) & 0xff;

   printf ("            Software    ="
           " %2.2" PRIx8 ".%2.2" PRIx8 ".%2.2" PRIx8 ".%2.2" PRIx8 ""
           " Firmware     = %8.8" PRIx32 "\n"
           "            RptTag      = %s\n"
           "            Serial #    = %16.16" PRIx64 "\n"
           "            Location    = %s/%u/%u/%u\n",
           major, minor, patch, release,
           firmware,
           rptSwTag,
           serialNumber,
           groupName, slot, bay, element);
   
   return;
}
/* ---------------------------------------------------------------------- */
/* OriginatorBody                                                         */
/* ====================================================================== */



/* ====================================================================== */
/* Originator                                                             */
/* ---------------------------------------------------------------------- */
void Originator::print () const
{
   printf ("Originator: %8.8" PRIx32 "\n", this->retrieve());
   m_body.print ();
}
/* ---------------------------------------------------------------------- */
/* Originator                                                             */
/* ====================================================================== */


/* ====================================================================== */
/* DataHeader                                                             */
/* ---------------------------------------------------------------------- */
void DataHeader::print (DataHeader const *dh)
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
/* DataHeader                                                             */
/* ====================================================================== */
} /* END: namespace: fragment                                             */
} /* END: namespace: pdd                                                  */
/* ====================================================================== */
