// -*-Mode: C++;-*-

#ifndef TPCPACKET_IMPL_HH
#define TPCPACKET_IMPL_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcPacket-Impl.cc
 *  @brief    Proto-Dune Data Tpc Data Packet Record, implementation
 *  @verbatim
 *                               Copyright 2013
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
 *  <2017/10/07>
 *
 * @par Credits:
 * SLAC
 *
 * @par
 * Implements the access methods to the data packets.
 * These are methods that, for performance reasons, are inline'd for 
 * internal use and made external for external use.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.08.07 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/access/TpcPacket.hh"
#include "dam/records/TpcPacket.hh"
#include "dam/access/WibFrame.hh"


namespace pdd    {
namespace access {


// --------------------------------
// Default implementation to inline
// --------------------------------
#ifndef TPCPACKET_IMPL
#define TPCPACKET_IMPL inline
#endif




/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */

#endif


