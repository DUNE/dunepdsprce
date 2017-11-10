// -*-Mode: C++;-*-

#ifndef ACCESS_TPCPACKET_HH
#define ACCESS_TPCPACKET_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcPacket.hh
 *  @brief    Proto-Dune Data Packet record access
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
 *  <2017/10/17>
 *
 * @par Credits:
 * SLAC
 *
 * This implements the access method for TpcPackets.
 *
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.11.10 jjr Added a method to locate a pointer to the first WIB frame 
                  in each (nominally) 1024 data packet.

                  Removed the locateWibFrame(s) methods. These were 
                  misguided and could not really work correctly in a 
                  formal sense.

   2017.10.16 jjr Moved from dam/access -> dam/records
   2017.08.07 jjr Created
  
\* ---------------------------------------------------------------------- */

#include "dam/access/Headers.hh"
#include <cstdint> 


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace pdd    {
namespace record {

   class TpcPacketHeader;
   class TpcPacketBody;
   class TpcPacket;
   class WibFrame;
}

namespace access {

   class WibFrame;
}

}
/* ====================================================================== */



namespace pdd    {
namespace access {

/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */




/* ====================================================================== */
/* CLASSS DEFINITIONS                                                     */
/* ---------------------------------------------------------------------- *//*!

  \brief Access to the TPC data packet bridge word
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcPacketBridge
{
public:
   TpcPacketBridge (uint32_t bridge);

public:
   uint32_t     getBridge () const;

   unsigned int getRecordFormat   () const;
   unsigned int getPacketReserved () const;

   static unsigned int getRecordFormat   (uint32_t bridge);
   static unsigned int getPacketReserved (uint32_t bridge);

public:
   uint32_t m_w32;  /*!< The bridge word                                  */
};
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \brief Header of a TPC data packet
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcPacketHeader 
{
public:
   TpcPacketHeader (pdd::record::TpcPacketHeader const *header);

public:
   pdd::record::TpcPacketHeader const *getHeader         () const;
   uint32_t                            getBridge         () const;
   unsigned int                        getRecordFormat   () const;
   unsigned int                        getPacketReserved () const;
   void                                print             () const;

   static unsigned int 
          getRecordFormat   (pdd::record::TpcPacketHeader const *header);

   static unsigned int 
          getPacketReserved (pdd::record::TpcPacketHeader const *header);

   static uint32_t     
          getBridge         (pdd::record::TpcPacketHeader const *header);

   static unsigned int 
          getRecordFormat   (uint64_t w64);

   static unsigned int 
          getPacketReserved (uint64_t w64);


private:
   pdd::record::TpcPacketHeader const *m_header;

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief Body of a TPC data packet
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcPacketBody 
{
public:
   TpcPacketBody (pdd::record::TpcPacket     const *packet);
   TpcPacketBody (pdd::record::TpcPacketBody const   *body,
                  uint32_t                          bridge,
                  uint32_t                             n64);

public:
   pdd::record::TpcPacketBody const          *getBody () const;
   uint32_t                                 getBridge () const;
   unsigned int                             getFormat () const;
   uint32_t                                 getNbytes () const;

   uint64_t                   const          *getData () const;

   pdd::access::WibFrame        const  
           *getWibFrames (unsigned int type,
                          unsigned int o64) const;

   static pdd::access::WibFrame const  
           *getWibFrames (pdd::record::TpcPacketBody const *body,
                          unsigned int                      type,
                          unsigned int                      o64);

   static unsigned int getPacketReserved ();

private:
   pdd::record::TpcPacketBody const *m_body;
   uint32_t                        m_bridge;
   uint32_t                        m_nbytes;

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief A TPC data packet record
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcPacket
{
public:
   TpcPacket (pdd::record::TpcPacket const *packet);

public:
   // ------------------------------------------------------------------
   // Access to the basic record elements
   // -----------------------------------
   pdd::record::TpcPacket       const *getRecord () const;
   pdd::record::TpcPacketHeader const *getHeader () const;
   uint32_t                            getBridge () const;
   pdd::record::TpcPacketBody   const *getBody   () const;
   void                                print     () const;

   static pdd::record::TpcPacketHeader const 
                            *getHeader (pdd::record::TpcPacket const *packet);

   static uint32_t           getBridge (pdd::record::TpcPacket const *packet);

   static pdd::record::TpcPacketBody   const 
                             *getBody  (pdd::record::TpcPacket const *packet);

   static void                  print  (pdd::record::TpcPacket const *packet);
   // ------------------------------------------------------------------



   // ------------------------------------------------------------------


   // ------------------------------------------------------------------
   // TpcPacketBridge methods
   // -----------------------
   unsigned int getRecordFormat   () const;
   unsigned int getPacketReserved () const;

   static unsigned int getRecordFormat   (pdd::record::TpcPacket const *packet);
   static unsigned int getPacketReserved (pdd::record::TpcPacket const *packet);
   // ------------------------------------------------------------------



   // ------------------------------------------------------------------
   // Access to TpcPacketBody  elements
   // ---------------------------------
   uint64_t                     const *getData   () const;
   pdd::access::WibFrame        const *getWibFrames    (unsigned int type,
                                                        unsigned int  o64) const;

   static pdd::access::WibFrame const * 
                    getWibFrames (pdd::record::TpcPacket const *packet,
                                  unsigned int                    type,
                                  unsigned int                     o64);

   // ------------------------------------------------------------------

private:
   pdd::record::TpcPacket const *m_packet;

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
}  /* Namespace:: access                                                  */
}  /* Namespace:: pdd                                                     */
/* ---------------------------------------------------------------------- */
/* END DEFINITIONS                                                        */
/* ====================================================================== */




namespace pdd    {
namespace access {

/* ====================================================================== */
/* IMPLEMENTATION: TpcPacketBridge                                        */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the bridge word

  \param[in] bridge The Tpc Packet bridge word
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcPacketBridge::TpcPacketBridge (uint32_t bridge) :
   m_w32 (bridge)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Get the bridge word
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcPacketBridge::getBridge () const
{
   return m_w32;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

   \brief Return the format of the packet data
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcPacketBridge::getRecordFormat () const
{
   return getRecordFormat (m_w32);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

   \brief Return the format of the reserved field

   \note
    This is usually not of interest, but is provided for completeness
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcPacketBridge::getPacketReserved  () const
{
   return getPacketReserved (m_w32);
}
/* ---------------------------------------------------------------------- */
/* END: TpcPacketBridge                                                   */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION: TpcPacketHeader                                        */
/* ---------------------------------------------------------------------- *//*!

  \brief Construct for the TpcPacketHeader access class

  \param[in] header A pointer to the Tpc Packet record header
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcPacketHeader::
       TpcPacketHeader (pdd::record::TpcPacketHeader const *header) :
   m_header (header)
{
   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer TpcPacket record header class
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcPacketHeader const *TpcPacketHeader::getHeader () const
{
   return m_header;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return the TpcPacket bridge word
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcPacketHeader::getBridge () const
{
   return getBridge (m_header);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Get the format of the Tpc Packet record
  \return The format of the Tpc Packet record
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcPacketHeader::getRecordFormat () const
{ 
   return getRecordFormat (m_header);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Get the number of Tpc Packet record
  \return The reserved field of the Tpc Packet bridge word
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcPacketHeader::getPacketReserved () const 
{ 
   return getPacketReserved (m_header);
}
/* ---------------------------------------------------------------------- */
/* END: TpcPacketHeader                                                   */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION: TpcPacketBody                                          */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the body of Tpc Packet

  \param[in]   body  A pointer to the Tpc Packet record body
  \param[in] bridge  The bridge word of the Tpc Packet record
  \param[in] nbytes  The number of bytes in the record body
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcPacketBody::
       TpcPacketBody  (pdd::record::TpcPacketBody const *body,
                       uint32_t                        bridge,
                       uint32_t                        nbytes) :
   m_body   (  body),
   m_bridge (bridge),
   m_nbytes (nbytes)
{ 
   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return the bridge word of TpcPacketBody record class
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcPacketBody::getBridge () const
{
   return m_bridge;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer TpcPacketBody record class
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcPacketBody const *TpcPacketBody::getBody () const
{
   return m_body;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return the format of TpcPacketBody record class
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcPacketBody::getFormat () const
{
   return TpcPacketBridge::getRecordFormat (m_bridge);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return the number of 32-bit words in the TpcPacketBody record
                                                                          */
/* ---------------------------------------------------------------------- */
inline  uint32_t TpcPacketBody::getNbytes () const
{
   return m_nbytes;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a 64-bit pointer to the data

  \par
   This is mainly used for debugging and diagnostic purposes
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t const *TpcPacketBody::getData () const
{
   return reinterpret_cast<uint64_t const *>(m_body);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer to the first Wib Frame at the specified offset

  \param[in] type The packet type
  \param[in]  o64 The 64-bit word offset.  This would usually come from
                  the Table of Contents.
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::access::WibFrame const *
TpcPacketBody::getWibFrames (unsigned int type,
                             unsigned int o64) const
{
   return TpcPacketBody::getWibFrames (m_body, type, o64);
}
/* ---------------------------------------------------------------------- */
/* END: TpcPacketBody                                                     */
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION: TpcPacket                                              */
/* ---------------------------------------------------------------------- *//*!

  \brief Construct for the TpcPacket access class

  \param[in] header A pointer to the Tpc Packet record body
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcPacket::
       TpcPacket (pdd::record::TpcPacket const *packet) :
   m_packet (packet)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer TpcPacket record class
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcPacket const *TpcPacket::getRecord () const
{
   return m_packet;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer TpcPacket record class header
                                                                          */
/* ---------------------------------------------------------------------- */
inline  pdd::record::TpcPacketHeader const *TpcPacket::getHeader () const
{
   return getHeader (m_packet);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the Tpc Packet record header
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcPacketHeader const *
               TpcPacket::getHeader (pdd::record::TpcPacket const *packet)
{
   return reinterpret_cast<pdd::record::TpcPacketHeader const *>(packet);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer TpcPacket record class header
                                                                          */
/* ---------------------------------------------------------------------- */
inline  uint32_t TpcPacket::getBridge () const
{
   return getBridge (m_packet);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer TpcPacket record class body
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcPacketBody   const *TpcPacket::getBody () const
{
   return getBody (m_packet);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a 64-bit pointer to the data

  \par
   This is mainly used for debugging and diagnostic purposes
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t const *TpcPacket::getData () const
{
   return reinterpret_cast<uint64_t const *>(m_packet);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer to the first Wib Frame

  \param[in]  o64 The 64-bit word offset.  This would usually come from
                  the Table of Contents.
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::access::WibFrame const 
     *TpcPacket::getWibFrames (unsigned int type, 
                               unsigned int o64) const
{
   return getWibFrames (m_packet, type, o64);
}
/* ---------------------------------------------------------------------- */
/* END: TpcPacket                                                         */
}  /* Namespace:: access                                                  */
}  /* Namespace:: pdd                                                     */
/* ---------------------------------------------------------------------- */
/* END IMPLEMENTATION                                                     */
/* ====================================================================== */



#endif
