// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     DataFragment.CC
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
   2017.08.29 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/DataFragmentUnpack.hh"
#include <cinttypes>
#include <cstdio>

DataFragmentUnpack::DataFragmentUnpack (uint64_t const *buf) :
   m_buf        (buf),
   m_originator (getOriginator (buf)),
   m_data       (getData       (buf)),
   m_trailer    (getTrailer    (buf))
{
   return;
}


pdd::fragment::Header<pdd::fragment::Type::Data> const *DataFragmentUnpack::getHeader () const
{
   return 
      reinterpret_cast<pdd::fragment::Header<pdd::fragment::Type::Data> const *>
      (m_buf);
}

bool DataFragmentUnpack::isTpcNormal () const
{
   auto dfHeader    = getHeader ();
   bool isTpcNormal = dfHeader->isTpcNormal ();
   return isTpcNormal;
}

uint32_t  DataFragmentUnpack::getN64() const
{
   auto dfHeader = getHeader ();
   auto n64      = dfHeader->getN64 ();
   return n64;
}



pdd::fragment::Identifier const *DataFragmentUnpack::getIdentifier () const
{
   return getIdentifier (m_buf);
}

pdd::fragment::Originator const *DataFragmentUnpack::getOriginator () const
{
   return m_originator;
}

pdd::fragment::Data const *DataFragmentUnpack::getData () const
{
   return m_data;
}

pdd::Trailer const *DataFragmentUnpack::getTrailer () const
{
   return m_trailer;
}


pdd::fragment::Header<pdd::fragment::Type::Data> const 
              *DataFragmentUnpack::getHeader (uint64_t const *buf)
{   
   pdd::fragment::Header<pdd::fragment::Type::Data> const
      *hdr = reinterpret_cast<decltype (hdr)>(buf);
   return hdr;
}

pdd::fragment::Identifier const *DataFragmentUnpack::getIdentifier (uint64_t const *buf)
{
   pdd::Header0 const *hdr = getHeader (buf);
   pdd::fragment::Identifier const *identifier 
       = reinterpret_cast<decltype (identifier)>(hdr + 1);
   return identifier;
}


pdd::fragment::Originator const *DataFragmentUnpack::getOriginator (uint64_t const *buf)
{
   pdd::Header0 const *hdr = getHeader (buf);
   pdd::fragment::Originator const *originator
          = reinterpret_cast<decltype (originator)>(hdr + 1 + hdr->getNaux64 ());
   return originator;
}


pdd::fragment::Data const *DataFragmentUnpack::getData (uint64_t const *buf)
{
   pdd::fragment::Originator const *org = getOriginator (buf);
   pdd::fragment::Data  const *data
      = reinterpret_cast<decltype (data)>(
         reinterpret_cast<uint64_t const *>(org) + org->getN64 ());
   return data;
}
 
                            
pdd::Trailer const *DataFragmentUnpack::getTrailer (uint64_t const *buf)
{
   uint32_t            n64 = getHeader(buf)->getN64 ();
   pdd::Trailer const *tlr = reinterpret_cast<decltype(tlr)>
                            (buf + n64 - sizeof (*tlr)/sizeof(uint64_t));
   return tlr;
}


void DataFragmentUnpack::print () const
{
   printHeader      ();
   printIdentifier  ();
   printOriginator  ();
   printData        ();
   return;
}



void DataFragmentUnpack::printHeader () const
{
   pdd::Header0 const *hdr = getHeader ();
   print (hdr);
   return;
}

void DataFragmentUnpack::print (pdd::Header0 const *header)
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

void DataFragmentUnpack::printIdentifier () const
{
   pdd::fragment::Identifier const *id = getIdentifier ();
   id->print ();
   return;
}

void DataFragmentUnpack::printOriginator () const
{
   pdd::fragment::Originator const *org = getOriginator ();
   org->print ();
   return;
}

void DataFragmentUnpack::printData () const
{
   pdd::fragment::Data const *dat = getData ();
   dat->print ();
}      

void DataFragmentUnpack::printTrailer () const
{
   pdd::Trailer const *tlr = getTrailer ();
   print (tlr);
   return;
}

void DataFragmentUnpack::print (pdd::Trailer const *trailer)
{
   printf ("Trailer = %16.16" PRIx64 "\n", trailer->retrieve ());
   return;
}
/* ====================================================================== */
