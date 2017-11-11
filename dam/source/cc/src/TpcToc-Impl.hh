// -*-Mode: C++;-*-

#ifndef TPCTOC_IMPL_HH
#define TPCTOC_IMPL_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcToc-Impl.hh
 *  @brief    Proto-Dune Data Tpc Table Of Contents Record, implementation
 *  @verbatim
 *                               Copyright 2013
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
 * @par
 * This layout the format and primitive access methods to the data
 * found in a TpcNormal and TpcDamaged records.  These are methods that,
 * for performance reasons, are inline'd for internal use and made 
 * external for external use.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.11.10 jjr Add getNWibFrames to return the number of WibFrames in
                  a packet.
   2017.08.07 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/access/TpcToc.hh"
#include "dam/records/TpcToc.hh"
#include "dam/records/WibFrame.hh"


namespace pdd    {
namespace access {


// --------------------------------
// Default implementation to inline
// --------------------------------
#ifndef TPCTOC_IMPL
#define TPCTOC_IMPL inline
#endif


/* ====================================================================== */
/* BEGIN: TpcToc                                                          */
/* ---------------------------------------------------------------------- *//*!

  \brief Return the TPC Table Of Contents bridge word as 32-bit immediate
         value.

  \param[in] toc The TPC Table Of Contents record 
                                                                          */
/* ---------------------------------------------------------------------- */  
TPCTOC_IMPL uint32_t TpcToc::getBridge   (pdd::record::TpcToc const *toc)
{
   return toc->getBridge ();
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer to the TPC Table Of Contents body

  \param[in] toc The TPC Table Of Contents record 
                                                                          */
/* ---------------------------------------------------------------------- */  
TPCTOC_IMPL pdd::record::TpcTocBody    const *
TpcToc::getBody   (pdd::record::TpcToc const *toc)
{
   return &toc->m_body;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number of packet descriptors in the TOC record 
  \return The number of packet descriptors in the TOC record 

  \param[in] header The Tpc Table of Contents record header
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL int 
TpcToc::getNPacketDscs (pdd::record::TpcToc const *toc)
{
   pdd::record::TpcTocHeader const *hdr = toc;
   return       TpcTocHeader::getNPacketDscs (hdr);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Get the format of Tpc Table of Contents itself
  \return The format of Tpc Table of Contents itself

  \param[in] header The Tpc Table of Contents record header
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL unsigned int
 TpcToc::getRecordFormat (pdd::record::TpcToc const *toc)
{
   pdd::record::TpcTocHeader const *hdr = toc;
   return       TpcTocHeader::getRecordFormat (hdr);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer to the Tpc Table Of Contents descriptors

  \param[in] toc Pointer to the Tpc Table Of Contents record
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL pdd::record::TpcTocPacketDsc  const *
TpcToc::getPacketDscs (pdd::record::TpcToc const *toc)
{
   pdd::record::TpcTocBody      const *body = TpcToc::getBody (toc);
   pdd::record::TpcTocPacketDsc const *dscs = TpcTocBody::getPacketDscs (body);
   return dscs;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer to the specified Tpc Table Of Contents
         descriptor

  \param[in] toc Pointer to the Tpc Table Of Contents record
  \param[in] idx Index to the desired descriptor
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL pdd::record::TpcTocPacketDsc  const *
TpcToc::getPacketDsc (pdd::record::TpcToc const *toc,  int idx)
{
   pdd::record::TpcTocBody const      *body = TpcToc::getBody (toc);
   pdd::record::TpcTocPacketDsc const *dscs = TpcTocBody::getPacketDsc (body, idx);
   return dscs;
}
/* ---------------------------------------------------------------------- */
/* END:   TpcToc                                                          */
/* ====================================================================== */





/* ====================================================================== */
/* BEGIN: TpcTocBody                                                      */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the body of Table of Contents

  \param[in]    toc  A pointer to the Tpc Table of Contents record
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL TpcTocBody::TpcTocBody  (pdd::record::TpcToc const *toc) :
   m_body   (TpcToc::getBody   (toc)),
   m_bridge (TpcToc::getHeader (toc)->getBridge ()),
   m_nbytes (toc->getNbytes() - sizeof (TpcTocHeader))
{ 
   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief   Returns a bare 32-bit pointer to the contents 
  \return  A bare 32-bit 

  \param[in] body A pointer to the TPC Table Of Contents record body

  \note
   This is not a main line method. It is used primarily for debugging.
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL uint32_t const *
TpcTocBody::getW32 (pdd::record::TpcTocBody const *body) 
{
   uint32_t const *w32  = reinterpret_cast<decltype (w32)>(body);
   return          w32;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the list of Tpc Table Of Contents packet
          descriptors.
  \return A pointer to the list of Tpc Table Of Contents packet
          descriptors.

  \param[in] body A pointer to the TPC Table Of Contents record body
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL pdd::record::TpcTocPacketDsc const *
TpcTocBody::getPacketDscs (pdd::record::TpcTocBody const *body)
{
   pdd::record::TpcTocPacketDsc const 
                          *dscs = reinterpret_cast<decltype (dscs)>(body);
   return dscs;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the list of Tpc Table Of Contents packet
          descriptors.
  \return A pointer to the list of Tpc Table Of Contents packet
          descriptors.

  \param[in] body A pointer to the TPC Table Of Contents record body
  \param[in]  idx The index of the descriptor you want
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL pdd::record::TpcTocPacketDsc const *
TpcTocBody::getPacketDsc (pdd::record::TpcTocBody const *body,
                          int                             idx)
{
   pdd::record::TpcTocPacketDsc const 
                          *dscs = reinterpret_cast<decltype (dscs)>(body);
   return dscs + idx;
}
/* ---------------------------------------------------------------------- */
/* END: TpcTocBody                                                        */
/* ====================================================================== */



/* ====================================================================== */
/* BEGIN: TpcTocPacketDsc                                                 */
/* ---------------------------------------------------------------------- *//*!

  \brief   Get the format of the descriptor itself
  \return  The format of the descriptor.

  \param[in] dsc The packet descriptor.

  \par
   The format defines the layout of the descriptor, not the data packet
   it is describing.  It is generally only for internal consumption,
   allowing the proper decoding of the descriptor, but is provided for
   completeness and debugging/diagnostic purposes.
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL unsigned int
TpcTocPacketDsc::getFormat (pdd::record::TpcTocPacketDsc dsc) 
{
   unsigned int format = 
      PDD_EXTRACT32 (dsc.m_w32,
                     pdd::record::TpcTocPacketDsc::Mask  ::Format,
                     pdd::record::TpcTocPacketDsc::Offset::Format);
   return format;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the type of packet. 
  \return The packet type

  \par
   The packet types are enumerated in TocBody::PacketDsc::Type and are
   such things as raw WIB frames, compressed data, etc.  This value is 
   essential since it instructs the TPC data decoders how to interpret
   the binary data within a data packet. 
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL unsigned int
TpcTocPacketDsc::getType (pdd::record::TpcTocPacketDsc dsc) 
{
   unsigned int type = 
      PDD_EXTRACT32 (dsc.m_w32,
                     pdd::record::TpcTocPacketDsc::Mask  ::Type,
                     pdd::record::TpcTocPacketDsc::Offset::Type);
   return type;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the 64-bit offset of this data packet within the data
          record.
  \return The 64-bit offset of this data packet within the data
          record.

                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL unsigned int 
TpcTocPacketDsc::getOffset64 (pdd::record::TpcTocPacketDsc dsc)
{
   unsigned int o64 =
      PDD_EXTRACT32 (dsc.m_w32, 
                     pdd::record::TpcTocPacketDsc::Mask  ::Offset64, 
                     pdd::record::TpcTocPacketDsc::Offset::Offset64);
   return o64;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Tests whether the associated data packet is a raw WIB frame
  \retval == true,  if the associated data packet is a raw WIB frame
  \revval == false, if the associated data packet is not a raw WIB frame
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL bool TpcTocPacketDsc::isWibFrame (pdd::record::TpcTocPacketDsc dsc)
{
   unsigned int type = getType (dsc);
   return (type == static_cast<decltype (type)>
                   (pdd::record::TpcTocPacketDsc::Type::WibFrame));
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief   Get the format of the descriptor itself
  \return  The format of the descriptor.

  \param[in] dsc Pointer to packet descriptor.

  \par
   The format defines the layout of the descriptor, not the data packet
   it is describing.  It is generally only for internal consumption,
   allowing the proper decoding of the descriptor, but is provided for
   completeness and debugging/diagnostic purposes.
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL unsigned int 
TpcTocPacketDsc::getFormat (pdd::record::TpcTocPacketDsc const *dsc)
{
   unsigned int format = getFormat (*dsc);
   return       format;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the type of packet. 
  \return The packet type

  \par
   The packet types are enumerated in TocBody::PacketDsc::Type and are
   such things as raw WIB frames, compressed data, etc.  This value is 
   essential since it instructs the TPC data decoders how to interpret
   the binary data within a data packet. 
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL unsigned int
TpcTocPacketDsc::getType (pdd::record::TpcTocPacketDsc const *dsc) 
{
   unsigned int type = getType (*dsc);
   return       type;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the 64-bit offset of this data packet within the data
          record.
  \return The 64-bit offset of this data packet within the data
          record.

                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL unsigned int
TpcTocPacketDsc::getOffset64 (pdd::record::TpcTocPacketDsc const *dsc)
{
   unsigned int o64 = getOffset64 (*dsc);
   return       o64;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Tests whether the associated data packet is a raw WIB frame
  \retval == true,  if the associated data packet is a raw WIB frame
  \revval == false, if the associated data packet is not a raw WIB frame
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL bool 
TpcTocPacketDsc::isWibFrame (pdd::record::TpcTocPacketDsc const *dsc)
{
   return isWibFrame (*dsc);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number of frames for this data packet
  \return The number of frames
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL unsigned int 
TpcTocPacketDsc::getNWibFrames (pdd::record::TpcTocPacketDsc const *dsc)
{
   unsigned int o64_beg = getOffset64 (dsc[0]);
   unsigned int o64_end = getOffset64 (dsc[1]);

   unsigned int nframes = (o64_end - o64_beg) 
                        / (sizeof (pdd::record::WibFrame) /sizeof (uint64_t));
                        
   return nframes;
}
/* ---------------------------------------------------------------------- */
/*   END: TpcPacketDsc                                                    */
/* ====================================================================== */






/* ====================================================================== */
/* BEGIN: TpcTocHeader                                                    */
/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the format of the Table of Contents itself
  \return The format of the Table of Contents itself

  \param[in] header The Tpc Table of Contents record header

  \par
   In general the format is not of interest to the general user, but is
   provided for completeness and may be useful for diagnostic or debugging
   purposes.
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL unsigned int 
TpcTocHeader::getRecordFormat (pdd::record::TpcTocHeader const *header)
{
   return getRecordFormat (header->retrieve ());
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number of packet descriptors in the TOC record 
  \return The number of packet descriptors in the TOC record 

  \param[in] header The Tpc Table of Contents record header
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL int 
TpcTocHeader::getNPacketDscs (pdd::record::TpcTocHeader const *header)
{
   return getNPacketDscs (header->retrieve ());
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the format of the Table of Contents record
  \return The format of the descriptor

  \par
   In general the format is not of interest to the general user, but is
   provided for completeness and may be useful for diagnostic or debugging
   purposes.
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL uint32_t TpcTocHeader::getRecordFormat (uint32_t w32)
{
   return PDD_EXTRACT32 (w32, 
                         pdd::record::TpcTocHeader::Bridge::Mask  ::Format, 
   static_cast<uint32_t>(Header2::Offset::Bridge) 
 + static_cast<uint32_t>(pdd::record::TpcTocHeader::Bridge::Offset::Format));
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number of packet descriptors format of the
          TOC record 
  \return The number of packet descriptors of the TOC record 

  \param[in] bridge The TOC record bridge word.
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL int TpcTocHeader::getNPacketDscs (uint32_t w32)
{
   return PDD_EXTRACT32 (w32, 
                         pdd::record::TpcTocHeader::Bridge::Mask  ::DscCount, 
   static_cast<uint32_t>(Header2::Offset::Bridge)
 + static_cast<uint32_t>(pdd::record::TpcTocHeader::Bridge::Offset::DscCount));
}
/* ---------------------------------------------------------------------- */
/* END:   TpcTocHeader                                                    */
/* ====================================================================== */





/* ====================================================================== */
/* BEGIN: TpcTocBridge                                                    */
/* ---------------------------------------------------------------------- *//*!

  \brief Return the format of the Table of Contents record

  \param[in] bridge The Table of Contents bridge word
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL unsigned int TpcTocBridge::getRecordFormat (uint32_t bridge)
{
   unsigned int fmt = 
            PDD_EXTRACT32 (bridge,
                           pdd::record::TpcTocHeader::Bridge::Mask::  Format,
                           pdd::record::TpcTocHeader::Bridge::Offset::Format);

   return fmt;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return the number of packet descriptors in the Table of Contents
         record

  \param[in] bridge The Table of Contents bridge word
                                                                          */
/* ---------------------------------------------------------------------- */
TPCTOC_IMPL int TpcTocBridge::getNPacketDscs (uint32_t bridge)
{
   int ndscs = PDD_EXTRACT32 (bridge,
                              pdd::record::TpcTocHeader::Bridge::Mask  ::DscCount, 
                              pdd::record::TpcTocHeader::Bridge::Offset::DscCount);
   return ndscs;
}
/* ---------------------------------------------------------------------- */
/*   END: TpcTocBridge                                                    */
} /* END: namespace access                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */

#endif


