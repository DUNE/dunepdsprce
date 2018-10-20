// -*-Mode: C++;-*-
#ifndef DATAFRAGMENT_IMPL_HH
#define DATAFRAGMENT_IMPL_HH


/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     DataFragment-Impl.h
 *  @brief    Access methods for the RCE data fragments. These are methods 
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


#include "dam/access/DataFragment.hh"
#include "dam/access/Identifier.hh"
#include "dam/access/Originator.hh"
#include "dam/access/Data.hh"
#include "dam/records/DataFragment.hh"

#ifndef DATAFRAGMENT_IMPL
#define DATAFRAGMENT_IMPL inline
#endif  



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2018.08.30 jjr Added isTpcEmpty   method
   2018.03.23 jjr Added isTpcDamaged method
   2017.10.07 jjr Created.  This is for methods that, for performance 
                  reasons are inlined for internal use and made external
                  for external use. 
  
\* ---------------------------------------------------------------------- */



namespace pdd    {
namespace access {
/* ---------------------------------------------------------------------- *//*!

  \brief   Return a pointer to the DataFragment header.
  \return  A pointer to the DataFragment header.
                                                                          */
/* ---------------------------------------------------------------------- */
DATAFRAGMENT_IMPL pdd::record::DataFragmentHeader
                        const *DataFragment::getHeader () const
{
   return reinterpret_cast<pdd::record::DataFragmentHeader const *>(m_buf);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Test if this DataFragment is a normal, \i.e. error free TPC
          fragment.
  \retval true,   if this DataFragment is a normal TPC fragment
  \retval false,  if this DataFragment is not a normal TPC fragment
                                                                          */
/* ---------------------------------------------------------------------- */
DATAFRAGMENT_IMPL bool DataFragment::isTpcNormal () const
{
   auto dfHeader    = getHeader ();
   bool isTpcNormal = dfHeader->isTpcNormal ();
   return isTpcNormal;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Test if this DataFragment is a damaged, \i.e. a TPC fragment 
          with an error in one or more of its streams.

  \retval true,   if this DataFragment is a damaged TPC fragment
  \retval false,  if this DataFragment is not a damaged TPC fragment
                                                                          */
/* ---------------------------------------------------------------------- */
DATAFRAGMENT_IMPL bool DataFragment::isTpcDamaged () const
{
   auto dfHeader     = getHeader ();
   bool isTpcDamaged = dfHeader->isTpcDamaged ();
   return isTpcDamaged;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Test if this DataFragment is a TpcEmpty \i.e. there are no TPC
          streams

  \retval true,   if this DataFragment is an empty TPC fragment
  \retval false,  if this DataFragment is not an empty TPC fragment
                                                                          */
/* ---------------------------------------------------------------------- */
DATAFRAGMENT_IMPL bool DataFragment::isTpcEmpty () const
{
   auto dfHeader   = getHeader ();
   auto dfData     = reinterpret_cast<void const *>(getData   ());
   auto dfTrailer  = reinterpret_cast<void const *>(getTrailer());

   bool isTpcEmpty = (dfData == dfTrailer) || dfHeader->isTpcEmpty ();
   return isTpcEmpty;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Get the length of this DataFragment, in units of 64-bit words
  \return The length of this DataFragment, in units of 64-bit words
                                                                          */
/* ---------------------------------------------------------------------- */
DATAFRAGMENT_IMPL uint32_t  DataFragment::getN64() const
{
   auto dfHeader = getHeader ();
   auto n64      = dfHeader->getN64 ();
   return n64;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the Identifier subrecord.
  \return A pointer to the Identifier subrecord.
                                                                          */
/* ---------------------------------------------------------------------- */
DATAFRAGMENT_IMPL pdd::record::Identifier const *
              DataFragment::getIdentifier () const
{
   return getIdentifier (m_buf);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the Originator subrecord.
  \return A pointer to the Originator subrecord.
                                                                          */
/* ---------------------------------------------------------------------- */
DATAFRAGMENT_IMPL pdd::record::Originator const *
              DataFragment::getOriginator () const
{
   return m_originator;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the Data subrecord.
  \return A pointer to the Data subrecord.

  \par
   The Data subrecord contains not only the actual TPC data, but since
   this data can occur in a various formats, additional subrecords to 
   help locate and access this data.
                                                                          */
/* ---------------------------------------------------------------------- */
DATAFRAGMENT_IMPL pdd::record::Data const *
                  DataFragment::getData () const
{
   return m_data;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the Trailer subrecord.
  \return A pointer to the Trailer subrecord.
                                                                          */
/* ---------------------------------------------------------------------- */
DATAFRAGMENT_IMPL pdd::Trailer const *
                   DataFragment::getTrailer () const
{
   return m_trailer;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Return a pointer to the DataFragment header.
  \return  A pointer to the DataFragment header.

  \param[in]  buf The ProtoDune data buffer.  This must have been 
                  verified as a ProtoDune data.
                                                                          */
/* ---------------------------------------------------------------------- */
DATAFRAGMENT_IMPL pdd::record::DataFragmentHeader const *
                   DataFragment::getHeader (uint64_t const *buf)
{   
   pdd::record::DataFragmentHeader const
      *hdr = reinterpret_cast<decltype (hdr)>(buf);
   return hdr;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the Identifier subrecord.
  \return A pointer to the Identifier subrecord.
                                                                          */
/* ---------------------------------------------------------------------- */
DATAFRAGMENT_IMPL pdd::record::Identifier const *
              DataFragment::getIdentifier (uint64_t const *buf)
{
   pdd::Header0 const *hdr = getHeader (buf);
   pdd::record::Identifier const *identifier 
       = reinterpret_cast<decltype (identifier)>(hdr + 1);

   return identifier;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the Originator subrecord.
  \return A pointer to the Originator subrecord.

  \param[in]  buf The ProtoDune data buffer.  This must have been 
                  verified as a ProtoDune data.
                                                                          */
/* ---------------------------------------------------------------------- */
DATAFRAGMENT_IMPL pdd::record::Originator const *
              DataFragment::getOriginator (uint64_t const *buf)
{
   pdd::Header0 const *hdr = getHeader (buf);
   pdd::record::Originator const *originator
          = reinterpret_cast<decltype (originator)>(hdr + 1 + hdr->getNaux64 ());
   return originator;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the Data subrecord.
  \return A pointer to the Data subrecord.

  \param[in]  buf The ProtoDune data buffer.  This must have been 
                  verified as a ProtoDune data.

  \par
   The Data subrecord contains not only the actual TPC data, but since
   this data can occur in a various formats, additional subrecords to 
   help locate and access this data.
                                                                          */
/* ---------------------------------------------------------------------- */
DATAFRAGMENT_IMPL pdd::record::Data const *
               DataFragment::getData (uint64_t const *buf)
{
   pdd::record::Originator const *org = getOriginator (buf);
   pdd::record::Data  const *data
      = reinterpret_cast<decltype (data)>(
         reinterpret_cast<uint64_t const *>(org) + org->getN64 ());
   return data;
}
/* ---------------------------------------------------------------------- */
 


/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the Trailer subrecord.
  \return A pointer to the Trailer subrecord.

  \param[in]  buf The ProtoDune data buffer.  This must have been 
                  verified as a ProtoDune data.
                                                                          */
/* ---------------------------------------------------------------------- */
DATAFRAGMENT_IMPL pdd::Trailer const *
                  DataFragment::getTrailer (uint64_t const *buf)
{
   uint32_t            n64 = getHeader(buf)->getN64 ();
   pdd::Trailer const *tlr = reinterpret_cast<decltype(tlr)>
                            (buf + n64 - sizeof (*tlr)/sizeof(uint64_t));
   return tlr;
}
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
/* ---------------------------------------------------------------------- */
} /* END: namespace pdd                                                   */
/* ====================================================================== */


#endif
