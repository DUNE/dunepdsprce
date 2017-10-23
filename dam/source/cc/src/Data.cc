// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     Data.cc
 *  @brief    Proto-Dune Data access implementation
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
   2017.10.12 jjr Created - separated from Headers.hh
  
\* ---------------------------------------------------------------------- */


#include <dam/access/Data.hh>
#include <dam/access/Headers.hh>
#include <dam/records/Data.hh>

#include <cinttypes>
#include <cstdio>

namespace pdd      {
namespace access   {


/* ====================================================================== */
/* Data                                                                   */
/* ---------------------------------------------------------------------- */
void Data::print (Data const *data)
{
   pdd::record::Data const *dr = data->getRecord ();
   unsigned int  format = dr->getFormat ();
   unsigned int    type = dr->getType   ();
   unsigned int     n64 = dr->getN64    ();
   uint32_t      bridge = dr->getBridge ();
   
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
} /* END: namespace: access                                               */
} /* END: namespace: pdd                                                  */
/* ====================================================================== */
