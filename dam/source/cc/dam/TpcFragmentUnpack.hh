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
   ---------- --- -------------------------------------------------------
   2017.10.06 jjr Changed name of the, as to-date, empty print method 
                  from void Print () -> void print () const to conform
                  to the established
                  naming convention.
   2017.08.31 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/TpcStreamUnpack.hh"
#include "dam/access/TpcFragment.hh"


#include <cstddef>
#include <cstdint>
#include <vector>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
class DataFragmentUnpack;
/* ---------------------------------------------------------------------- */
/* FORWARD REFERENCES                                                     */
/* ====================================================================== */



/* ====================================================================== */
/* CLASS: TpcFragmentUnpack                                               */
/* ---------------------------------------------------------------------- *//*!

  \class TpcFragmentUnpack
  \brief Access the TpcStreams within a Tpc Fragment

  \par
   A TpcFragment is a specific example of a generic DataFragment. One
   must verify that the type of the DataFragment from which this class
   is constructed is a TpcFragment by accessing it type within the header.
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcFragmentUnpack
{
  public:
   TpcFragmentUnpack (DataFragmentUnpack const &fragment);


   void                   print       ()            const; 
   int                    getNStreams ()            const;
   TpcStreamUnpack const *getStream   (int istream) const;

private:
   pdd::access::TpcFragment  m_tpc; /*!< Low Level access classs          */
};
/* ---------------------------------------------------------------------- */
/* CLASS: TpcFragmentUnpack                                               */
/* ====================================================================== */


#endif
