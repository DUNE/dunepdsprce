// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     RceFragmentUnpack.cc
 *  @brief    Proto-Dune Fragment access
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


#include "dam/RceFragmentUnpack.hh"
#include "dam/HeaderFragmentUnpack.hh"




/* ---------------------------------------------------------------------- *//*!

  \brief  Performs a sanity check on whether \a buf is looks like a 
          legitimate RCE fragment
  \retval true  if \a buf passes the rudimentary sanity check
  \retval false if \a buf does not pass the rudimentary sanity check

  \param[in] buf  The input buffer
                                                                          */
/* ---------------------------------------------------------------------- */
bool RceFragmentUnpack::isOkay (uint64_t const *buf)
{
   auto const *header = HeaderFragmentUnpack::assign (buf);
   uint32_t       n64 = header->getN64 ();
   uint64_t   trailer = buf[n64 - 1];
   
   bool   okay =  (buf[0] == ~trailer);
   return okay;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Performs a sanity check on whether \a buf is looks like a 
          legitimate RCE fragment of \a nbytes
  \retval true  if \a buf passes the rudimentary sanity check
  \retval false if \a buf does not pass the rudimentary sanity check

  \param[in]    buf  The input buffer
  \param[in] nbytes  The expected length
                                                                          */
/* ---------------------------------------------------------------------- */
bool RceFragmentUnpack::isOkay (uint64_t const *buf,
                                size_t       nbytes)
{
   bool          okay;
   auto const *header = HeaderFragmentUnpack::assign (buf);
   
   // ----------------------------------
   // Check that header looks legitimate
   // ----------------------------------
   okay = header->isOkay (nbytes);
   if (!okay)
   {
      return false;
   }



   // ------------------------------------------------
   // Check that header is consistent with the trailer
   // ------------------------------------------------
   uint32_t      n64 = header->getN64 ();
   uint64_t  trailer = buf[n64 -1 ];
   okay              = (buf[0] == ~trailer);

   return okay;
}
/* ---------------------------------------------------------------------- */
