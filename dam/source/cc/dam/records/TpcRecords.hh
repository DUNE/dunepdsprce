// -*-Mode: C++;-*-

#ifndef RECORDS_TPCRECORDS_HH
#define RECORDS_TPCRECORDS_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcRecords.hh
 *  @brief    Proto-Dune Data Tpc Data records definition
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
 *  <2017/08/07>
 *
 * @par Credits:
 * SLAC
 *
 * This layout the format and primitive access methods to the data
 * found in a TpcNormal and TpcDamaged records.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.10.12 jjr Moved from dam/access -> dam/records
   2017.08.07 jjr Created
  
\* ---------------------------------------------------------------------- */

#include "dam/access/Headers.hh"
#include <cstdint> 
#include <cstdio>

namespace pdd    {
namespace record {


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
class WibFrame;
/* ====================================================================== */




/* ====================================================================== */
/* CLASSS DEFINITIONS                                                     */
/* ---------------------------------------------------------------------- *//*!

   \brief This specializes the bridge word of a standard header for use
          Toc usage.
                                                                          */
/* ---------------------------------------------------------------------- */
class TocHeader : public pdd::Header2
{
private:
   TocHeader () = delete;

public:
   class Bridge;

public:
   void     print       () const { print (this); }
   uint32_t getTocFormat() const { return getTocFormat (Header2::getBridge ()); }
   uint32_t getNDscs    () const { return getNDscs     (Header2::getBridge ()); }

   static int       getNDscs (uint32_t bridge) 
   { return Bridge::getNDscs (bridge); }

   static uint32_t  getTocFormat(uint32_t bridge) 
   { return Bridge::getTocFormat (bridge);}

   static void     print       (TocHeader const *hdr);

public:
   class Bridge
   {
   public:
      explicit Bridge (uint32_t bridge) : m_bridge (bridge) { return; }

   public:
      int        getTocFormat () { return getTocFormat (m_bridge); }
      int        getNDscs     () { return getNDscs     (m_bridge); }

      static int getTocFormat (uint32_t bridge);
      static int getNDscs     (uint32_t bridge);

   private:
      uint32_t m_bridge;

   } __attribute__ ((packed));

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \class TocBody
  \brief Table of contents for the data pieces
                                                                          */
/* ---------------------------------------------------------------------- */
class TocBody
{
private:
   TocBody () = delete;

public:
   class Contributor;
   class PacketDsc;

public:
   uint32_t     const          *getW32 () const;
   PacketDsc    const   *getPacketDscs () const;

   void        print (uint32_t bridge) const;
   static void print (TocBody const *body, uint32_t bridge);

   /* ------------------------------------------------------------------- */
   class PacketDsc
   {
      PacketDsc () = delete;

   public:
      enum class Type
      {
         WibFrame   = 1, /*!< Raw WIB frames                            */
         Transposed = 2, /*!< Transposed, but not compressed            */
         Compressed = 3  /*!< Compressed data                           */
      };        

   public:
      uint32_t          getW32 () const;
      unsigned int   getFormat () const;
      Type             getType () const;
      unsigned int getOffset64 () const;
      bool          isWibFrame () const;

   private:
      uint32_t m_w32;
   } __attribute__ ((packed));
   /* ------------------------------------------------------------------- */

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief The table of contents record = header + body
                                                                          */
/* ---------------------------------------------------------------------- */
class Toc : public TocHeader
{
private:
   Toc () = delete;

public:
   TocBody const *getBody () const { return &m_body; }

   void           print () const { print (this); }    
   static  void   print (Toc const *toc);

private:
   TocBody m_body;

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \brief Header of a TPC data packet
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcPacketHeader : public Header1
{
private:
   TpcPacketHeader () = delete;

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief Body of a TPC data packet
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcPacketBody 
{
private:
   TpcPacketBody () = delete;

public:
   WibFrame const *locateWibFrames ();
   WibFrame const *locateWibFrame  (int idx);

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief A TPC data packet record
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcPacket : public TpcPacketHeader
{
private:
   TpcPacket () = delete;

public:
   TpcPacketBody const *getBody () const { return &m_body; }

private:
   TpcPacketBody m_body;

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
/* END DEFINITIONS                                                        */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief Prints the body of Table of Contents

  \param[in] version  The version of this TOC
                                                                          */
/* ---------------------------------------------------------------------- */
inline void TocBody::print (unsigned int version) const 
{ 
   print (this, version);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Returns a bare 32-bit pointer
  \return  A bare 32-bit 

  \note
   This is not a main line method. It is used primarily for debugging.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t const *TocBody::getW32 () const
{
   return reinterpret_cast<uint32_t const *>(this);
}
/* ---------------------------------------------------------------------- */

inline TocBody::PacketDsc const *TocBody::getPacketDscs () const
{
   uint32_t const            *ptr = getW32 ();
   TocBody::PacketDsc const *dscs = reinterpret_cast<decltype (dscs)>(ptr);
   return dscs;
}



inline uint32_t TocBody::PacketDsc::getW32 () const 
{ 
   return m_w32; 
}
/* ---------------------------------------------------------------------- */
}  /* Namespace:: record                                                  */
}  /* Namespace:: pdd                                                     */
/* ---------------------------------------------------------------------- */

#endif
