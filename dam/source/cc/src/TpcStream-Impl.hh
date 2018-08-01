// -*-Mode: C++;-*-
#ifndef TPCSTREAM_IMPL_HH
#define TPCSTREAM_IMPL_HH


/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcStream-Impl.hh
 *  @brief    Access methods for the RCE Tpc Stream data records
 *            decoding a binary test file.  These are methods that are
 *            \e inline'd for local use and made external for \e external
 *            use.
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
   2018.07.13 jjr Used the reserved byte in the bridge word for the stream's
                  status value.
   2017.10.07 jjr Created.  This is for methods that, for performance 
                  reasons are inlined for internal use and made external
                  for external use. 
  
\* ---------------------------------------------------------------------- */



#include "dam/access/TpcStream.hh"
/*
#include "dam/access/TpcRanges.hh"
#include "dam/access/TpcToc.hh"
#include "dam/access/TpcPacket.hh"

#include "dam/records/TpcRanges.hh"
#include "dam/records/TpcToc.hh"
#include "dam/records/TpcPacket.hh"
*/


#include "dam/records/TpcStream.hh"

#include <cstdio>

// --------------------------------
// Default implementation to inline
// --------------------------------
#ifndef TPCSTREAM_IMPL
#define TPCSTREAM_IMPL inline
#endif


namespace pdd    {
namespace record {
/* ---------------------------------------------------------------------- *//*!

  \namespae tcpstream_bridge
  \brief    Defines the layout of TcpStreamHeader bridge 
                                                                          */
/* ---------------------------------------------------------------------- */
namespace tpcstream_bridge
{
   /* -------------------------------------------------------------------- *//*!

     \brief Size of the bit fields of the bridge word
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Size
   {
      Format    =  4,  /*!< Size of the bridge word's format field        */
      Csf       = 12,  /*!< Size of the Crate.Slot.Fiber field            */
      Left      =  8,  /*!< Size of the number of Tpc Records left        */
      Status    =  8   /*!< Size of the status field                      */
   };
   /* ------------------------------------------------------------------- */


   /* ---------------------------------------------------------------- *//*!

      \brief Right justified offsets of the bit fields of the bridge
             word
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Offset
   {
      Format   =  0, /*!< Offset to the bridge words's format field       */
      Csf      =  4, /*!< Offset to the Crate.Sloc.Fiber field            */  
      Left     = 16, /*!< Offset to the number of Tpc Records left        */
      Status   = 24  /*!< Offset to the status field                      */
   };
   /* ------------------------------------------------------------------- */


   /* ------------------------------------------------------------------- *//*!

     \brief Right justified masks of the bit fields of the bridge word
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Mask : uint32_t
   {
      Format   = 0x0000000f,
      Csf      = 0x00000fff,
      Left     = 0x000000ff,
      Status   = 0x000000ff
    };
   /* ------------------------------------------------------------------- */
}
/* ---------------------------------------------------------------------- */

/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the format of the bridge word
  \return The format of the bridge word

  \param[in] bridge The bridge word

  \par
   The format number of the bridge word is generally not of interest to
   the user.  It used internally to define the layout of the remaining
   fields. It is provided for completeness and may by useful in diagnostic
   or debuggings usages.
                                                                          */
/* ---------------------------------------------------------------------- */
TPCSTREAM_IMPL int TpcStreamHeader::Bridge::getFormat (uint32_t bridge)
{
   return PDD_EXTRACT32 (bridge, tpcstream_bridge::Mask  ::Format, 
                                 tpcstream_bridge::Offset::Format);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Returns the number of Tpc Stream records still remaining in an
         array of Tpc Stream records

  \param[in] bridge The bridge word

  \par
   The value of this field can be used in the following ways

      -# In the first element this will give the total numbers of
         TpcStream records (+1) to expect. This allows the user to
         allocate resources for this number and/or define a loop to 
         service all these records
     -#  It can be checked for consistency in each succeeding record
     -#  The last element will have a value of 0.
                                                                          */
/* ---------------------------------------------------------------------- */
TPCSTREAM_IMPL int TpcStreamHeader::Bridge::getLeft (uint32_t bridge)
{
   return PDD_EXTRACT32 (bridge, tpcstream_bridge::Mask  ::Left, 
                                 tpcstream_bridge::Offset::Left);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Returns the packed originating WIB Crate.Slot.Fiber.

  \param[in] bridge The bridge word
                                                                          */
/* ---------------------------------------------------------------------- */
TPCSTREAM_IMPL uint32_t TpcStreamHeader::Bridge::getCsf (uint32_t bridge)
{
   return PDD_EXTRACT32 (bridge, tpcstream_bridge::Mask  ::Csf, 
                                 tpcstream_bridge::Offset::Csf);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Returns the status word

  \param[in] bridge The bridge word
                                                                          */
/* ---------------------------------------------------------------------- */
TPCSTREAM_IMPL uint32_t TpcStreamHeader::Bridge::getStatus (uint32_t bridge)
{
   return PDD_EXTRACT32 (bridge, tpcstream_bridge::Mask  ::Status, 
                                 tpcstream_bridge::Offset::Status);
}
/* ---------------------------------------------------------------------- */
}
}


namespace pdd    {
namespace access {

/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the packed Crate.Slot.Fiber that provided the data
  \return The packed Crate.Slot.Fiber that provided the data
                                                                          */
/* ---------------------------------------------------------------------- */
TPCSTREAM_IMPL uint32_t TpcStream::getCsf () const 
{ 
   return getHeader ()->getCsf  ();
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number of TpcStream records left in this array. The
          last such TpcStream record will return 0.
  \return The number of TpcStream records left in this array 
                                                                          */
/* ---------------------------------------------------------------------- */
TPCSTREAM_IMPL int TpcStream::getLeft () const 
{ 
   return getHeader ()->getLeft (); 
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the TpcStream record's status
  \return The TpcStream record's status
                                                                          */
/* ---------------------------------------------------------------------- */
TPCSTREAM_IMPL uint32_t TpcStream::getStatus () const 
{ 
   return getHeader ()->getStatus (); 
}
/* ---------------------------------------------------------------------- */
}}

namespace pdd    {
namespace access {

class TpcStreamHeader : public pdd::record::TpcStreamHeader
{
   TpcStreamHeader () = delete;

};
   


} /* END: namespace access                                                */
/* ---------------------------------------------------------------------- */
} /* END: namespace pdd                                                   */
/* ====================================================================== */

#endif
