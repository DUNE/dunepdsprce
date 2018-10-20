// -*-Mode: C++;-*-

#ifndef RCEFRAGMENTUNPACK_HH
#define RCEFRAGMENTUNPACK_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     RceFragmentUnpack.hh
 *  @brief    Proto-Dune Generic FragmentHeader 
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


#include "dam/access/Headers.hh"


/* ====================================================================== */
/* DEFINITION : RceFragmentUnpack                                      */
/* ---------------------------------------------------------------------- *//*!

  \class RceFragmentUnpack
  \brief Convenience class used to access a generic fragment
                                                                          */
/* ---------------------------------------------------------------------- */
class RceFragmentUnpack
{
   RceFragmentUnpack () = delete;

public:
   RceFragmentUnpack (uint64_t const *buf);

public:
   bool                               isOkay ()              const;
   bool                               isOkay (size_t nbytes) const;

   static bool                        isOkay (uint64_t const *buf);
   static bool                        isOkay (uint64_t const *buf,
                                              size_t       nbytes);
private:
   uint64_t const *m_buf;  /*!< Pointer to the fragment data              */
};
/* ---------------------------------------------------------------------- */
/* DEFINITION : RceFragmentUnpack                                         */
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION : RceFragmentUnpack                                     */
/* ---------------------------------------------------------------------- *//*!

   \brief Construct a RceFragmentUnpack from a 64-bit pointer

   \param[in] buf The 64-bit pointer
                                                                          */
/* ---------------------------------------------------------------------- */
RceFragmentUnpack::RceFragmentUnpack (uint64_t const *buf) :
   m_buf (buf)
{
   return;
}
/* ---------------------------------------------------------------------- */
/* IMPLEMENTATION : RceFragmentUnpack                                     */
/* ====================================================================== */

   
#endif
