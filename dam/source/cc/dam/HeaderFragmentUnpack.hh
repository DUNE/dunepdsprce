// -*-Mode: C++;-*-

#ifndef HEADERFRAGMENTUNPACK_HH
#define HEADERFRAGMENTUNPACK_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     HeaderFragmentUnpack.hh
 *  @brief    Proto-Dune Generic FragmentHeader 
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
 *  proto-dune DAM 
 *
 *  @author
 *  <russell@slac.stanford.edu>
 *
 *  @par Date created:
 *  <2017/09/14>
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
   2017.09.14 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/access/Headers.hh"


/* ====================================================================== */
/* DEFINITION : HeaderFragmentUnpack                                      */
/* ---------------------------------------------------------------------- *//*!

  \class HeaderFragmentUnpack
  \brief Convenience class used to access a generic fragment header

  \par
   Most of the methods are inherited from the \a Header0 class.
                                                                          */
/* ---------------------------------------------------------------------- */
class HeaderFragmentUnpack : public pdd::Header0
{
   HeaderFragmentUnpack () = delete;

public:
   HeaderFragmentUnpack (uint64_t const *buf);

public:
   static HeaderFragmentUnpack *assign (uint64_t *buf);
   bool                         isData () const;
};
/* ---------------------------------------------------------------------- */
/* DEFINITION : HeaderFragmentUnpack                                      */
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION : HeaderFragmentUnpack                                  */
/* ---------------------------------------------------------------------- *//*!

   \brief Construct a HeaderFragmentUnpack from a 64-bit pointer

   \param[in] buf The 64-bit pointer
                                                                          */
/* ---------------------------------------------------------------------- */
HeaderFragmentUnpack::HeaderFragmentUnpack (uint64_t const *buf) :
   pdd::Header0 (buf)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Recasts a 64-bit pointer as writeable pointer to a 
           HeaderFragmentUnpack 
  \return  A pointer to a HeaderFragmentUnpack
  
  \param[in] buf  The 64-bit to interpret as a writeable pointer to a
                  HeaderFragmentUnpack

  \warning
   The main use of this is to support test programs that need to will
   in the header. 
                                                                          */
/* ---------------------------------------------------------------------- */
HeaderFragmentUnpack *HeaderFragmentUnpack::assign (uint64_t *buf)
{
   HeaderFragmentUnpack *header = reinterpret_cast<decltype(header)>(buf);
   return header;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief Tests whether this is Data fragment header
   \retval true, if this is Data fragment header
   \retval false, if this is not Data fragment header
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool HeaderFragmentUnpack::isData () const
{
   if (getType () == static_cast<uint32_t>(pdd::fragment::Type::Data)) 
   {
      return true;
   }
   else
   {
      return false;
   }
}
/* ---------------------------------------------------------------------- */
/* IMPLEMENTATION : HeaderFragmentUnpack                                  */
/* ====================================================================== */

   
#endif
