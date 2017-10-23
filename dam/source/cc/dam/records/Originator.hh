// -*-Mode: C++;-*-

#ifndef RECORDS_ORIGINATOR_HH
#define RECORDS_ORIGINATOR_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     records/Originator.hh
 *  @brief    Originator record defintion
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


#include <dam/access/Headers.hh>
#include <cinttypes>



/* ====================================================================== */
/* class Originator                                                       */
/* ---------------------------------------------------------------------- */
namespace pdd     {
namespace record  {

/* ---------------------------------------------------------------------- */
class Versions
{
public:
   explicit Versions () { return; }

public:
   uint64_t       m_w64;   /*! A packed word containing the versions      */
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \class OriginatorBody
  \brief The hardware, software and geographic information about this
         data's orginator
                                                                          */
/* ---------------------------------------------------------------------- */
class OriginatorBody
{
public:
   explicit OriginatorBody () { return; }

public:
   uint32_t     m_location;  /*!< Packed location information             */
   uint64_t m_serialNumber;  /*!< The serial number                       */
   Versions     m_versions;  /*!< The version numbers                     */
   char   m_strings[32+32];  /*!< The strings                             */
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \class Originator
  \brief Information about the physical entity (RCE) producing the data
         and the software and firmware running on it
                                                                          */
/* ---------------------------------------------------------------------- */
class Originator : public Header2
{
public:
   explicit Originator () { return; }

public:
   OriginatorBody m_body;    /*!< The body of the Originator Record       */
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
} /* END: namespace record                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */

#endif
