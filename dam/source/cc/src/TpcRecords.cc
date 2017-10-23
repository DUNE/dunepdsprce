// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcRecords.cc
 *  @brief    Proto-Dune Data Tpc Data Packet Records access
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
 *  <2017/10/16>
 *
 * @par Credits:
 * SLAC
 *
 * This layout the format and primitive access methods to the data
 * found in a TpcNormal and TpcDamaged records.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.10.09 jjr Moved some methods to TpcRecords-Inline.hh for better
                  performance while making them external here for external
                  users.
   2017.08.07 jjr Created
  
\* ---------------------------------------------------------------------- */

#define   TPCPACKET_IMPL extern


#include "TpcPacket-Impl.hh"
#include  <cstdio>


namespace pdd    {
namespace record {


/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
} /* END: namespace pdd                                                   */
/* ---------------------------------------------------------------------- */

