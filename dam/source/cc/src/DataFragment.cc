// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     DataFragment.cc
 *  @brief    Access methods for the RCE data fragments
 *            decoding a binary test file.
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
   2017.10.07 jjr Created
  
\* ---------------------------------------------------------------------- */


#define   DATAFRAGMENT_IMPL extern

#include "DataFragment-Impl.hh"

#include <cinttypes>
#include <cstdio>

namespace pdd    {
namespace access {
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor to populate pointers to the various subrecords of a 
         data fragment

  \param[in]  buf The ProtoDune data buffer.  This must have been 
                  verified as a ProtoDune data.
                                                                          */
/* ---------------------------------------------------------------------- */
DataFragment::DataFragment (uint64_t const *buf) :
   m_buf        (buf),
   m_originator (getOriginator (buf)),
   m_data       (getData       (buf)),
   m_trailer    (getTrailer    (buf))
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Prints all or portions of the Data Fragment's subrecords
                                                                          */
/* ---------------------------------------------------------------------- */
void DataFragment::print () const
{
   printHeader      ();
   printIdentifier  ();
   printOriginator  ();
   printData        ();
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Print the Data Fragment Header
                                                                          */
/* ---------------------------------------------------------------------- */
void DataFragment::printHeader () const
{
   pdd::Header0 const *hdr = getHeader ();
   print (hdr);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Print's a generic Fragment Header interpretted as a 
          Data Fragment Header.

  \param[in] header The generic Fragment Header.
                                                                          */
/* ---------------------------------------------------------------------- */
void DataFragment::print (pdd::Header0 const *header)
{
   printf ("Header: %16.16" PRIx64 "\n", header->retrieve());
      
   unsigned int  format = header->getFormat  ();
   unsigned int    type = header->getType    ();
   unsigned int     n64 = header->getN64     ();
   unsigned int  naux64 = header->getNaux64  ();
   unsigned int subtype = header->getSubtype ();
   unsigned int  bridge = header->getBridge  ();

   printf ("Header      Type.Format = %1.1x.%1.1x "
           "N64:%6.6x:%1.1x SubType=%1.1x Bridge=%6.6x\n",
           type,format,
           n64, naux64,
           subtype, bridge);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Print the Identifier record
                                                                          */
/* ---------------------------------------------------------------------- */
void DataFragment::printIdentifier () const
{
   pdd::access::Identifier const id (getIdentifier ());
   id.print ();
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Print the Originator record
                                                                          */
/* ---------------------------------------------------------------------- */
void DataFragment::printOriginator () const
{
   pdd::access::Originator const org (getOriginator ());
   org.print ();
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Print some portion of Data record

  \par
   Since the Data record is potentially very large, only the meta-data
   and some limited piece of the actual TPC data is printed.  There 
   really is no way to provide a generic print routine that will satisfy
   the needs in all or even most circumstances, so this is meant more as
   a first level, get off-the-ground method.
                                                                          */
/* ---------------------------------------------------------------------- */
void DataFragment::printData () const
{
   pdd::access::Data const data (getData ());
   data.print ();
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Print the Trailer record
                                                                          */
/* ---------------------------------------------------------------------- */
void DataFragment::printTrailer () const
{
   pdd::Trailer const *tlr = getTrailer ();
   print (tlr);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Print the Trailer record

  \param[in] trailer Pointer to the trailer record.  
                                                                          */
/* ---------------------------------------------------------------------- */
void DataFragment::print (pdd::Trailer const *trailer)
{
   printf ("Trailer = %16.16" PRIx64 "\n", trailer->retrieve ());
   return;
}
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
/* ---------------------------------------------------------------------- */
} /* END: namespace pdd                                                   */
/* ====================================================================== */
