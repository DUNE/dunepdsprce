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
   2017.10.12 jjr Separated from Headers.hh
  
\* ---------------------------------------------------------------------- */


#include "dam/access/Originator.hh"
#include "dam/records/Originator.hh"

#include <cstdio>


/* ====================================================================== */
/* IMPLEMENTATION : Originator                                            */
/* ---------------------------------------------------------------------- */
namespace pdd     {
namespace access  {

uint32_t Versions::getFirmware (pdd::record::Versions const *versions)
{ 
   return versions->m_w64 >>  0; 
}


uint32_t Versions::getSoftware (pdd::record::Versions const *versions) 
{
   return versions->m_w64 >> 32; 
}


pdd::record::Originator const *Originator::getRecord () const 
{ 
return m_org;
}


uint32_t Originator::getLocation (pdd::record::Originator const *org)
{ 
   return  org->m_body.m_location; 
}


uint64_t Originator::getSerialNumber (pdd::record::Originator const *org)
{ 
   return org->m_body.m_serialNumber;
}

   pdd::record::Versions const *Originator::getVersions (pdd::record::Originator const *org)
{ 
   return &org->m_body.m_versions;
}


char const *Originator::getRptSwTag (pdd::record::Originator const *org)
{ 
   return  org->m_body.m_strings;
}


char const *Originator::getGroupName (pdd::record::Originator const *org)
{
   return &org->m_body.m_strings[strlen(getRptSwTag(org))+1];
}


/* ---------------------------------------------------------------------- */
void Originator::print () const
{
   printf ("Originator: %8.8" PRIx32 "\n", this->getRecord()->retrieve());
   uint32_t        location = getLocation     ();
   Versions const  versions  (getVersions     ());
   uint64_t    serialNumber = getSerialNumber ();
   char const     *rptSwTag = getRptSwTag     ();
   char const    *groupName = getGroupName    ();

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
}
/* ---------------------------------------------------------------------- */
/* Originator                                                             */
/* ====================================================================== */
}
}
