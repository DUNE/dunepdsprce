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


#include "dam/Headers.hh"
#include <cinttypes>



class DataFragmentUnpack
{
public:
   DataFragmentUnpack (uint64_t const *buf);

   bool                                                    isTpcNormal   () const;
   uint32_t                                                getN64        () const;
   pdd::fragment::Header<pdd::fragment::Type::Data> const *getHeader     () const;
   pdd::fragment::Identifier                        const *getIdentifier () const;
   pdd::fragment::Originator                        const *getOriginator () const;
   pdd::fragment::Data                              const *getData       () const;
   pdd::Trailer                                     const *getTrailer    () const;


   static pdd::fragment::Header<pdd::fragment::Type::Data> const 
                                          *getHeader     (uint64_t const *buf);
   static pdd::fragment::Identifier const *getIdentifier (uint64_t const *buf);
   static pdd::fragment::Originator const *getOriginator (uint64_t const *buf);
   static pdd::fragment::Data       const *getData       (uint64_t const *buf);
   static pdd::Trailer              const *getTrailer    (uint64_t const *buf);

   void print           () const;
   void printHeader     () const;
   void printIdentifier () const;
   void printOriginator () const;
   void printData       () const;
   void printTrailer    () const;


   static void print (pdd::Header0 const  *header);
   static void print (pdd::Trailer const *trailer);


private:
   uint64_t                  const         *m_buf;
   pdd::fragment::Originator const  *m_originator;
   pdd::fragment::Data       const        *m_data;
   pdd::Trailer              const     *m_trailer;
};
/* ====================================================================== */

#endif
