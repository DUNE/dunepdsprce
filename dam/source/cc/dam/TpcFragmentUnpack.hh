// -*-Mode: C++;-*-

#ifndef PDD_TPCFRAGMENTUNPACK_HH
#define PDD_TPCFRAGMENTUNPACK_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcFragmentUnpack.hh
 *  @brief    Proto-Dune Online/Offline Data Tpc Access Routines
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
 *  <2017/08/31>
 *
 * @par Credits:
 * SLAC
 *
 * This layout the format and primitive access methods to the data
 * found in a TpcStream.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.08.31 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/TpcStreamUnpack.hh"

#include <cstddef>
#include <cstdint>
#include <vector>


namespace pdd  {
namespace access 
{
   class DataFragment;
}
}

class TpcFragmentUnpack
{
  public:
   TpcFragmentUnpack (DataFragmentUnpack const &fragment);

   typedef uint64_t timestamp_t;

   void Print(); 

   int                    getNStreams ()            const;
   TpcStreamUnpack const *getStream   (int istream) const;


private:
   DataFragmentUnpack  const &m_df;
   int                  m_nstreams;
   TpcStreamUnpack m_tpcStreams[2];
};
/* ---------------------------------------------------------------------- */
#endif
