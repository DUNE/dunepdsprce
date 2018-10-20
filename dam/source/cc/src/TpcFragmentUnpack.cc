// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcFragmentUnpack.cc
 *  @brief    Access methods for the TPC data record
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
   2016.10.06 jjr Changed method void Print () -> void print () const
   2017.08.29 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/TpcFragmentUnpack.hh"
#include "dam/DataFragmentUnpack.hh"
#include "TpcStream-Impl.hh"
#include <cstddef>



/* ---------------------------------------------------------------------- *//*!

  \brief Construct the Tpc Fragment Accessor

  \param[in] df Generic DataFragment
                                                                          */
/* ---------------------------------------------------------------------- */
TpcFragmentUnpack::TpcFragmentUnpack (DataFragmentUnpack const &df) :
   m_tpc (df.m_df)
{
   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number of Tpc streams
  \return The number of Tpc streams
                                                                          */
/* ---------------------------------------------------------------------- */
int TpcFragmentUnpack::getNStreams () const 
{
   return m_tpc.getNStreams ();
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the specfied TpcStream or NULL if non-existent
  \return The specfied TpcStream or NULL if non-existent
                                                                          */
/* ---------------------------------------------------------------------- */
TpcStreamUnpack const *TpcFragmentUnpack::getStream (int istream) const 
{
   return  istream < m_tpc.getNStreams ()
         ? reinterpret_cast<TpcStreamUnpack const *>(m_tpc.getStream (istream))
         : reinterpret_cast<TpcStreamUnpack const *>(0);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Routine to print at least some part of a Tpc Fragment.

  \par
   Given the size and complexity of a TpcFragment, it is not feasible
   to provide a one-size-fits-all, so this routine is more of an outline
   of its contents.
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcFragmentUnpack::print () const
{
   puts ("TpcFrament print not ready yet");
   return;
}
/* ---------------------------------------------------------------------- */





      
