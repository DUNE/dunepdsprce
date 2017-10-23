// -*-Mode: C++;-*-

#ifndef DATAFRAGMENTUNPACK_HH
#define DATAFRAGMENTUNPACK_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     DataFragment.hh
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


#include "dam/access/DataFragment.hh"
#include <cinttypes>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace pdd      {
namespace fragment {
   class DataFragmentHeader;
   class Identifier;
   class Originator;
   class Data;
}
   class Trailer;
}
/* ====================================================================== */




/* ====================================================================== */
/* CLASS DEFINITION                                                       */
/* ---------------------------------------------------------------------- *//*!

  \class DataFragmentUnpack
  \brief Provides access to a Data Fragment and its records

  \par
   This is more of a convenience class than anything else. Its main value
   is in cacheing pointer to the records contained within a Data Fragment.

   Finding thes records can also be done by the caller using the static
   routines contained in this class, with no need to instantiate the 
   class.  
                                                                          */
/* ---------------------------------------------------------------------- */
class DataFragmentUnpack
{
public:
   DataFragmentUnpack (uint64_t const *buf);


   // ------------------------------------------------------------
   // Get the length of the Data Fragment and check its subtype
   // Currently only TpcNormal streams exist, but this may change
   // in the future.
   // ------------------------------------------------------------
   uint32_t                                getN64        () const;
   bool                                    isTpcNormal   () const;


   // ------------------------------------------------------------
   // Access the records within the Data Fragment
   // ------------------------------------------------------------
   pdd::record::DataFragmentHeader const *getHeader     () const;
   pdd::record::Identifier         const *getIdentifier () const;
   pdd::record::Originator         const *getOriginator () const;
   pdd::record::Data               const *getData       () const;
   pdd::Trailer                    const *getTrailer    () const;


   static pdd::record::DataFragmentHeader 
                                     const *getHeader     (uint64_t const *buf);
   static pdd::record::Identifier    const *getIdentifier (uint64_t const *buf);
   static pdd::record::Originator    const *getOriginator (uint64_t const *buf);
   static pdd::record::Data          const *getData       (uint64_t const *buf);
   static pdd::Trailer               const *getTrailer    (uint64_t const *buf);


   // ---------------------------------------------------------
   // Crude print methods, meant only to get one off the ground
   // Given the size and complexity of a Data Fragment, writing
   // one-size-fits-all print routines is not feasible.
   // ---------------------------------------------------------
   void print           () const;
   void printHeader     () const;
   void printIdentifier () const;
   void printOriginator () const;
   void printData       () const;
   void printTrailer    () const;

   static void print (pdd::Header0 const  *header);
   static void print (pdd::Trailer const *trailer);

public:
   pdd::access::DataFragment     m_df;
};
/* ====================================================================== */

#endif
