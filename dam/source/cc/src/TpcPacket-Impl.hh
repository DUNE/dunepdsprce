// -*-Mode: C++;-*-

#ifndef TPCPACKET_IMPL_HH
#define TPCPACKET_IMPL_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcPacket-Impl.cc
 *  @brief    Proto-Dune Data Tpc Data Packet Record, implementation
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
 * Implements the access methods to the data packets.
 * These are methods that, for performance reasons, are inline'd for 
 * internal use and made external for external use.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.11.10 jjr Added a method to locate a pointer to the first WIB frame 
                  in each (nominally) 1024 data packet.
   2017.08.07 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/access/TpcPacket.hh"
#include "dam/records/TpcPacket.hh"
#include "dam/access/WibFrame.hh"


namespace pdd    {
namespace access {


// --------------------------------
// Default implementation to inline
// --------------------------------
#ifndef TPCPACKET_IMPL
#define TPCPACKET_IMPL inline
#endif


/* ====================================================================== */
/* BEGIN: TpcPacketHeader                                                 */
/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the format of the Tpc Packet record
  \return The format of the Tpc Packet descriptor

  \param[in] header The Tpc Packet record header

  \par
   In general the format is not of interest to the general user, but is
   provided for completeness and may be useful for diagnostic or debugging
   purposes.
                                                                          */
/* ---------------------------------------------------------------------- */
TPCPACKET_IMPL unsigned int 
TpcPacketHeader::getRecordFormat (pdd::record::TpcPacketHeader const *header)
{
   return getRecordFormat (header->retrieve ());
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the reserved field the Tpc Packet record's brodge word
  \return The reserved field the TpcPacket record's brodge word

  \param[in] header The Tpc Packet record header
                                                                          */
/* ---------------------------------------------------------------------- */
TPCPACKET_IMPL unsigned int
TpcPacketHeader::getPacketReserved (pdd::record::TpcPacketHeader const *header)
{
   return getPacketReserved (header->retrieve ());
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer TpcPacket record class bridge word
                                                                          */
/* ---------------------------------------------------------------------- */
TPCPACKET_IMPL uint32_t 
TpcPacketHeader::getBridge (pdd::record::TpcPacketHeader const *header) 
{
   return header->getBridge ();
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the format of the TpcPacket record
  \return The format of the Tpc Packet record

  \par
   In general the format is not of interest to the general user, but is
   provided for completeness and may be useful for diagnostic or debugging
   purposes.
                                                                          */
/* ---------------------------------------------------------------------- */
TPCPACKET_IMPL uint32_t TpcPacketHeader::getRecordFormat (uint64_t w64)
{
   return PDD_EXTRACT64
         (w64, 
          pdd::record::TpcPacketHeader::Bridge::Mask   ::Format, 
          static_cast<uint32_t>(Header1::Offset::Bridge) 
        + static_cast<uint32_t>(pdd::record::TpcPacketHeader::
                                        Bridge::Offset::Format));
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the 
  \return The reserved field the TpcPacket record's brodge word

  \par
   In general the format is not of interest to the general user, but is
   provided for completeness and may be useful for diagnostic or debugging
   purposes.
                                                                          */
/* ---------------------------------------------------------------------- */
TPCPACKET_IMPL uint32_t TpcPacketHeader::getPacketReserved (uint64_t w64)
{
   return PDD_EXTRACT64
         (w64, 
          pdd::record::TpcPacketHeader::Bridge::Mask   ::Reserved, 
          static_cast<uint32_t>(Header1::Offset::Bridge) 
        + static_cast<uint32_t>(pdd::record::TpcPacketHeader::
                                        Bridge::Offset::Reserved));
}
/* ---------------------------------------------------------------------- */
/* END: TpcPacketHeader                                                   */
/* ====================================================================== */




/* ====================================================================== */
/* BEGIN: TpcPacketBody                                                   */
/* ---------------------------------------------------------------------- *//*!

  \brief Construct for the TpcPacketBody access class

  \param[in] header A pointer to the Tpc Packet record body
                                                                          */
/* ---------------------------------------------------------------------- */
TPCPACKET_IMPL 
TpcPacketBody::TpcPacketBody (pdd::record::TpcPacket const *packet) :
      m_body   (TpcPacket::getBody   (packet)),
      m_bridge (TpcPacket::getHeader (packet)->getBridge ()),
      m_nbytes (packet->getNbytes   () - sizeof (TpcPacketHeader))
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Get the array of WibFrames

  \param[in] body Pointer to the Tpc Packet record body
  \param[in] type The packet type
  \param[in] o64  The 64-bit word offset.  This would usually come from
                  the Table of Contents.
                                                                          */
/* ---------------------------------------------------------------------- */
TPCPACKET_IMPL pdd::access::WibFrame const *
   TpcPacketBody::getWibFrames (pdd::record::TpcPacketBody const *body,
                                unsigned int                      type,
                                unsigned int                       o64)
{
   return reinterpret_cast<pdd::access::WibFrame const *>(body->m_w64 + o64);
}
/* ---------------------------------------------------------------------- */
/* END: TpcPacketBody                                                     */
/* ====================================================================== */





/* ====================================================================== */
/* BEGIN: TpcPacket                                                       */
/* ---------------------------------------------------------------------- *//*!

  \brief Return the TPC Packet bridge word as 32-bit immediate value.

  \param[in] packet The TPC Packet record 
                                                                          */
/* ---------------------------------------------------------------------- */  
TPCPACKET_IMPL uint32_t 
TpcPacket::getBridge (pdd::record::TpcPacket const *packet)
{
   return packet->getBridge ();
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer to the TPC Packet body

  \param[in] packet The TPC Packet record 
                                                                          */
/* ---------------------------------------------------------------------- */  
TPCPACKET_IMPL pdd::record::TpcPacketBody    const *
TpcPacket::getBody   (pdd::record::TpcPacket const *packet)
{
   return &packet->m_body;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Get the format of Tpc Packet record itself
  \return The format of Tpc  Packet record itself

  \param[in] header The Tpc Packet record header
                                                                          */
/* ---------------------------------------------------------------------- */
TPCPACKET_IMPL unsigned int
TpcPacket::getRecordFormat (pdd::record::TpcPacket const *packet)
{
   pdd::record::TpcPacketHeader const *hdr = packet;
   return       TpcPacketHeader::getRecordFormat (hdr);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Get the reserved field of Tpc Packet record
  \return The reserved field of Tpc Packet record

  \param[in] header The Tpc Packet record header
                                                                          */
/* ---------------------------------------------------------------------- */
TPCPACKET_IMPL unsigned int 
TpcPacket::getPacketReserved (pdd::record::TpcPacket const *packet)
{
   pdd::record::TpcPacketHeader const *hdr = packet;
   return       TpcPacketHeader::getPacketReserved (hdr);
}
/* ---------------------------------------------------------------------- */
/*   END: TpcPacket                                                       */
} /* END: namespace access                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */

#endif


