
// -*-Mode: C++;-*-

#ifndef ACCESS_ORIGINATOR_HH
#define ACCESS_ORIGINATOR_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     access/Originator.hh
 *  @brief    Originator accesss defintion
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


#include <cinttypes>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace pdd    {
namespace record {

   class Versions;
   class Originator;

}
}
/* ====================================================================== */




/* ====================================================================== */
/* class Originator                                                       */
/* ---------------------------------------------------------------------- */
namespace pdd    {
namespace access {
/* ---------------------------------------------------------------------- */


class Versions
{
public:
   Versions (pdd::record::Versions const *versions) :
      m_versions (versions)
   {
      return;
   }


public:
   static uint32_t getFirmware (pdd::record::Versions const *versions);
   static uint32_t getSoftware (pdd::record::Versions const *versions);


   uint32_t getSoftware () const;
   uint32_t getFirmware () const;

public:
   pdd::record::Versions const *m_versions;
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \class Originator
  \brief Information about the physical entity (RCE) producing the data
         and the software and firmware running on it
                                                                          */
/* ---------------------------------------------------------------------- */
class Originator 
{
public:
   Originator (pdd::record::Originator const *org);

public:
   pdd::record::Originator const *getRecord       () const;
   uint32_t                       getLocation     () const;
   uint64_t                       getSerialNumber () const;
   pdd::record::Versions   const *getVersions     () const;
   char const                    *getRptSwTag     () const;
   char const                    *getGroupName    () const;

   void            print           () const;


   static uint32_t
          getLocation     (pdd::record::Originator const *org);

   static uint64_t 
          getSerialNumber (pdd::record::Originator const *org);

   static pdd::record::Versions const
         *getVersions     (pdd::record::Originator const *org);

   static char             const 
          *getRptSwTag     (pdd::record::Originator const *org);

   static char             const 
          *getGroupName    (pdd::record::Originator const *org);

   static void     print           (Originator const *org);

public:
   pdd::record::Originator const *m_org;
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */






/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace pdd    {
namespace access {

inline uint32_t Versions::getSoftware () const 
{ 
   return getSoftware (m_versions);
}

inline uint32_t Versions::getFirmware () const 
{ 
return getFirmware (m_versions); 
}


inline Originator::Originator (pdd::record::Originator const *org) :
   m_org    (org)
{
   return;
}


inline uint32_t Originator::getLocation   () const 
{ 
   return getLocation (m_org); 
}

inline uint64_t Originator::getSerialNumber() const 
{ 
   return getSerialNumber (m_org); 
}

inline pdd::record::Versions const *Originator::getVersions () const 
{ 
   return getVersions (m_org); 
}

inline char const *Originator::getRptSwTag () const 
{ 
   return getRptSwTag (m_org); 
}

inline char const *Originator::getGroupName() const 
{ 
   return getGroupName    (m_org); 
}
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */

#endif
