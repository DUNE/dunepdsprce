// -*-Mode: C++;-*-
#ifndef TPCFRAGMENT_IMPL_HH
#define TPCFRAGMENT_IMPL_HH


/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcFragment-Impl.hh
 *  @brief    Access methods for the RCE Tpc fragment. These are methods 
 *            that are \e inline'd for local use and made external for 
 *            \e external use.
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
 * These are methods that, for performance reasons, are inline'd for 
 * internal use and made external for external use.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.10.07 jjr Created.  This is for methods that, for performance 
                  reasons are inlined for internal use and made external
                  for external use. 
  
\* ---------------------------------------------------------------------- */


#include "dam/access/TpcFragment.hh"
#include <cstdio>

#ifndef TPCFRAGMENT_IMPL
#define TPCFRAGMENT_IMPL inline
#endif

namespace pdd    {
namespace access {

//   class TpcStream;

/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number of Tpc streams
  \return The number of Tpc streams
                                                                          */
/* ---------------------------------------------------------------------- */
TPCFRAGMENT_IMPL int TpcFragment::getNStreams () const 
{
   return m_nstreams;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the specfied TpcStream or NULL if non-existent
  \return The specfied TpcStream or NULL if non-existent
                                                                          */
/* ---------------------------------------------------------------------- */
TPCFRAGMENT_IMPL TpcStream const *TpcFragment::getStream (int istream) const 
{
   return  istream < m_nstreams
         ? &m_tpcStreams[istream] 
         : reinterpret_cast<TpcStream const *>(0);
}
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
/* ---------------------------------------------------------------------- */
} /* END: namespace pdd                                                   */
/* ====================================================================== */


#endif
