// -*-Mode: C++;-*-

#ifndef RECORDS_TPCSTREAM_HH
#define RECORDS_TPCSTREAM_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     records/TpcStream.hh
 *  @brief    Raw RCE Tpc Stream data record definitions
 *
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
   2017.10.12 jjr Created
  
\* ---------------------------------------------------------------------- */

#include "dam/access/Headers.hh"
#include "dam/records/Data.hh"
#include <cstdint>



namespace pdd    {
namespace record {

/* ====================================================================== */
/* CLASS DEFINITIONS                                                      */
/* ---------------------------------------------------------------------- *//*!

  \class TpcStreamHeader
  \brief Specializes a standard header for Tpc Data usage
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcStreamHeader : public pdd::record::DataHeader
{
public:
   TpcStreamHeader () = delete;

public:
   // ----------------------------------------------------
   // Convenience methods to get at the bridge word fields
   // ----------------------------------------------------
   int getFormat () const { return Bridge::getFormat (getBridge ()); }
   int getLeft   () const { return Bridge::getLeft   (getBridge ()); }
   int getCsf    () const { return Bridge::getCsf    (getBridge ()); }
   int getStatus () const { return Bridge::getStatus (getBridge ()); }
      

   // -----------------------------------------
   // Primitive TPC stream header print methods
   // -----------------------------------------
   static void print (DataHeader const *dh);
   void        print () const;


   /* ------------------------------------------------------------------- *//*!

     \class Bridge
     \brief Accesses the fields of the TPC stream header's ebridge word
                                                                          */
   /* ------------------------------------------------------------------- */
   class Bridge
   {
   public:
      explicit Bridge () { return; }

   public:
      int             getFormat () const { return getFormat (m_w32); }
      int             getLeft   () const { return getLeft   (m_w32); }
      int             getCsf    () const { return getCsf    (m_w32); }
      uint32_t        getStatus () const { return getStatus (m_w32); }

      static int      getFormat (uint32_t bridge);
      static int      getLeft   (uint32_t bridge);
      static uint32_t getCsf    (uint32_t bridge);
      static uint32_t getStatus (uint32_t bridge);

   private:
      uint32_t m_w32;  /*!< Storage for the bridge word                   */
   };
   /* ------------------------------------------------------------------- */

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
} /* END: namespace record                                                */
/* ---------------------------------------------------------------------- */
} /* END: namespace pdd                                                   */
/* ====================================================================== */

#endif
