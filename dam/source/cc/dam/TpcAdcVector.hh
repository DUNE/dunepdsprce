// -*-Mode: C++;-*-

#ifndef PDD_TPCADCVECTOR_HH
#define PDD_TPCADCVECTOR_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcAdcVector.hh
 *  @brief    Defines a data type for a vector of TPC ADCs
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
 *  pdd
 *
 *  @author
 *  <russell@slac.stanford.edu>
 *
 *  @par Date created:
 *  <2017/10/04>
 *
 * @par Credits:
 * SLAC
 *
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.10.04 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/util/AlignedAllocator.hh"
#include <vector>
#include <cstdint>


/* ---------------------------------------------------------------------- *//*!

  \brief Define an ADC vector in terms of a std::vector, but with an 
         custom allocator to allocate cache-line aligned memory
                                                                          */
/* ---------------------------------------------------------------------- */
typedef std::vector<uint16_t, 
                    pdd::AlignedAllocator<64, uint16_t>> TpcAdcVector;
/* ---------------------------------------------------------------------- */

#endif
