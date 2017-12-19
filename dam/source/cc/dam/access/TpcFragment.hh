// -*-Mode: C++;-*-

#ifndef PDD_TPCFRAGMENT_HH
#define PDD_TPCFRAGMENT_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcFragment.hh
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
 *  <2017/10/07>
 *
 * @par Credits:
 * SLAC
 *
 * This layout the format and primitive access methods to the data
 * found in a TpcFragment.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- -------------------------------------------------------
   2017.12.19 jjr Removed the need for specifying a class member as unused
                  by somewhat artificially using it.
   2017.11.02 jjr Modified the unused in __attribute__ ((unused) to be
                  CLASS_MEMBER_UNUSED.  clang flags unused class members
                  as errors, but standard gcc does not and furthermore
                  does not accept __attribute__ ((unused)) on class
                  members.  The symbol CLASS_MEMBER_UNUSED is 
                  perferentially defined on the compile command to be 
                  either 'unused' or a blank string.
   2017.10.27 jjr Modified for gcc on the MAC,
                  The compiler complained that the field m_df was unused,
                  even though it does appear to be used in the constructor,
                  I marked with __attribute__ ((unused)), but shouldn't
		  have had to.
   2017.10.07 jjr Created from TpcFragmentUnpack.hh
  
\* ---------------------------------------------------------------------- */


#include <dam/access/TpcStream.hh>
#include <cstdint>

/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace pdd      {
namespace access   {
class DataFragment;
}
}
/* ---------------------------------------------------------------------- */
/* FORWARD REFERENCES                                                     */
/* ====================================================================== */




/* ====================================================================== */
/* CLASS DEFINITIONS                                                      */
/* ---------------------------------------------------------------------- */
namespace pdd      {
namespace access   {
/* ---------------------------------------------------------------------- */
/* CLASS: TpcFragment                                                     */
/* ---------------------------------------------------------------------- *//*!

  \class TpcFragment
  \brief Access the records within a Tpc Fragment

  \par
   A TpcFragment is a specific example of a generic DataFragment. One
   must verify that the type of the DataFragment from which this class
   is constructed is a TpcFragment by checking its type within the header.
   Once verified, the DataFragment can be used to construct the 
   TpcFragment.
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcFragment
{
public:  
   static const int MaxTpcStreams = 2;

public:
   TpcFragment (pdd::access::DataFragment const &fragment);

public:
   void                             print           ()            const; 
   int                              getNStreams     ()            const;
   pdd::access::TpcStream    const *getStream       (int istream) const;
   pdd::access::DataFragment const &getDataFragment ()            const;

private:
  pdd::access::DataFragment  const              &m_df;
   int                                     m_nstreams;
   pdd::access::TpcStream m_tpcStreams[MaxTpcStreams];
};
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
/* ---------------------------------------------------------------------- */
} /* END: namespace pdd                                                   */
/* ====================================================================== */






/* ====================================================================== */
/* IMPLEMENTATION: TpcFragment                                            */
/* ---------------------------------------------------------------------- */
namespace pdd    {
namespace access {
/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a constant reference to the parent DataFragment
  \return A constant reference to the parent DataFragment
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::access::DataFragment const &TpcFragment::getDataFragment () const
{
  return m_df;
}
/* ---------------------------------------------------------------------- */
/* CLASS: TpcFragment                                                     */
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
/* ---------------------------------------------------------------------- */
} /* END: namespace pdd                                                   */
/* ====================================================================== */

#endif 
