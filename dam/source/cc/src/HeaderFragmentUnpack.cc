// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     HeaderFragmentUnpack.cc
 *  @brief    Proto-Dune Header Fragment access
 *  @verbatim
 *                               Copyright 2018
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
 *  <2018/09/10>
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
   2018.09.10 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/HeaderFragmentUnpack.hh"



/* ---------------------------------------------------------------------- *//*!

  \brief  Performs a sanity check on whether \a buf is looks like a 
          legitimate RCE fragment

  \retval true  if the header passes the rudimentary sanity check
  \retval false if the header does not pass the rudimentary sanity check
                                                                          */
/* ---------------------------------------------------------------------- */
bool HeaderFragmentUnpack::isOkay () const
{
   uint32_t pattern = getBridge ();

   // --------------------------------------------------------
   // Make sure the length is consistent with what is expected
   // --------------------------------------------------------
   bool okay = (pattern == pdd::fragment::Pattern);

   return okay;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Performs a sanity check on whether the header is looks like a 
          legitimate RCE fragment header of \a nbytes.
  \retval true  if \a buf passes the rudimentary sanity check
  \retval false if \a buf does not pass the rudimentary sanity check

  \param[in] nbytes  The number of bytes in \a buf
                                                                          */
/* ---------------------------------------------------------------------- */
bool HeaderFragmentUnpack::isOkay (size_t nbytes) const
{
   uint32_t     n64 = getN64    ();
   uint32_t pattern = getBridge ();

   // --------------------------------------------------------
   // Make sure the length is consistent with what is expected
   // --------------------------------------------------------
   bool okay = (pattern                 == pdd::fragment::Pattern)
             & (n64 * sizeof (uint64_t) == nbytes);


   return okay;
}
/* ---------------------------------------------------------------------- */
