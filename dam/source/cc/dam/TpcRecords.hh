// -*-Mode: C++;-*-

#ifndef PDD_TPCRECORDS_HH
#define PDD_TPCRECORDS_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcRecords.hh
 *  @brief    Proto-Dune Data Tpc Data Records
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
   2017.08.07 jjr Created
  
\* ---------------------------------------------------------------------- */

#include "dam/Headers.hh"
#include "dam/WibFrame.hh"
#include <cstdint> 
#include <cstdio>

namespace pdd      {
namespace fragment {

/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */



/* ---------------------------------------------------------------------- *//*!

   \brief Specializes a standard header for Tpc Data usage
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcStreamHeader : public pdd::fragment::DataHeader
{
public:
   TpcStreamHeader () = delete;


   class Bridge
   {
   public:
      explicit Bridge () { return; }

   public:
      /* ---------------------------------------------------------------- *//*!

         \brief Size of the bit fields of the bridge word
                                                                          */
      /* ---------------------------------------------------------------- */
      enum class Size
      {
         Format    =  4,  /*!< Size of the bridge word's format field     */
         Csf       = 12,  /*!< Size of the Crate.Slot.Fiber field         */
         Left      =  8,  /*!< Size of the number of Tpc Records left     */
         Reserved  =  8   /*!< Size of the reserved field                 */
      };
      /* ---------------------------------------------------------------- */


      /* ---------------------------------------------------------------- *//*!

         \brief Right justified offsets of the bit fields of the bridge
                word
                                                                          */
      /* ---------------------------------------------------------------- */
      enum class Offset
      {
         Format   =  0, /*!< Offset to the bridge words's format field    */
         Csf      =  4, /*!< Offset to the Crate.Sloc.Fiber field         */  
         Left     = 16, /*!< Offset to the number of Tpc Records left     */
         Reserved =  8  /*!< Offset to the reserved field                 */
      };
      /* ---------------------------------------------------------------- */


      /* ---------------------------------------------------------------- *//*!

         \brief Right justified masks of the bit fields of the bridge word
                                                                          */
      /* ---------------------------------------------------------------- */
      enum class Mask : uint32_t
      {
         Format   = 0x0000000f,
         Csf      = 0x00000fff,
         Left     = 0x000000ff,
         Reserved = 0x000000ff
      };
      /* ---------------------------------------------------------------- */


   public:
      static int getLeft (uint32_t bridge)
      {
         return PDD_EXTRACT32 (bridge, Mask::Left, Offset::Left);
      }

      static uint32_t getCsf (uint32_t bridge)
      {
         return PDD_EXTRACT32 (bridge, Mask::Csf, Offset::Csf);
      }

   public:
      uint32_t m_w32;  /*!< Storage for the bridge word                   */
   };
   /* ------------------------------------------------------------------- */


public:
   int getLeft () const { return Bridge::getLeft (getBridge ()); }
   int getCsf  () const { return Bridge::getCsf  (getBridge ()); }
      
   static void print (DataHeader const *dh);
   void        print () const;
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \class TpcStream
  \brief The TPC Stream record.  

   This may include any of the following records

     -# Table of Contents, 
        this provides a description and the location of each TPC data
        packet. It allows random location of both the first packet in
        the HLS streams and the packets within that stream.  Future
        versions will allow random location of the channels within the
        packets
    -#  Error Record
        This is an optional/as needed record describing any error
        conditions
    -#  The Tpc Data packets.

                                                                          */
/* ---------------------------------------------------------------------- */
class TpcStream : public TpcStreamHeader
{
   TpcStream () = delete;

public:
    /* TPC Record Types */
    enum class RecType
    {
       Reserved   = 0,   /*!< Reserved                                   */
       Toc        = 1,   /*!< Table of Contents record                   */
       Ranges     = 2,   /*!< Event range descriptor record              */
       Packets    = 3    /*!< Data packets                               */
    };

   void const *getBody () const { return this + 1; }

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

   \brief This specializes the bridge word of a standard header for use
          Ramges usage.
                                                                          */
/* ---------------------------------------------------------------------- */
class RangesHeader : public pdd::Header2
{
   RangesHeader () = delete;

public:
   void     print       () const {  print (this); }
   uint32_t getVersion  () const { return getVersion  (Header2::retrieve ()); }
   uint32_t getReserved () const { return getReserved (Header2::retrieve ()); }

   static uint32_t getVersion  (uint32_t w32);
   static uint32_t getReserved (uint32_t w32);
   static void     print       (RangesHeader const *hdr);


private:
   /* ------------------------------------------------------------------- *//*!

     \enum  class Size
     \brief Enumerates the sizes of the Brdige bit fields.
                                                                         */
   /* ------------------------------------------------------------------ */
   enum class Size: int
   {
      Version  =  4,  /*!< Size of the version field                     */
      NDscs    =  4,  /*!< Size of the number of descriptors field       */
      Reserved =  4   /*!< Size of the reserved field                    */
   };
   /* ------------------------------------------------------------------ */



   /* ------------------------------------------------------------------- *//*!

     \enum  class Offset
     \brief Enumerates the right justified offsets of the header bit 
            fields.
                                                                         */
   /* ------------------------------------------------------------------ */
   enum class Offset: int
   {
      Version   =  0, /*!< Offset of the version field                   */
      NDscs     =  4, /*!< Offset of number of descriptors field         */
      Reserved  =  8  /*!< Offset of the reserved field                  */
   };
   /* ------------------------------------------------------------------ */


   /* ------------------------------------------------------------------- *//*!

     \enum  class Offset
     \brief Enumerates the right justified masks of the header bit fields.
                                                                         */
   /* ------------------------------------------------------------------ */
   enum class Mask: uint32_t
   {
      Version  =  0x0000000f,
      NDscs    =  0x0000000f,
      Reserved =  0x0000000f
   };
   /* ------------------------------------------------------------------ */

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \class RangesBody
  \brief Defines the timestamps and packet indices of both the untrimmed
         and trimmed (the event) windows
                                                                          */
/* ---------------------------------------------------------------------- */
class RangesBody
{
   RangesBody () = delete;

public:
   class Descriptor;
   class Window;

   static void print (RangesBody const *body,
                      unsigned int    format);

   void print (unsigned int format) const
   {
      print (this, format);
      return;
   }

   Descriptor const *getDescriptors () const;

public:
   /* ------------------------------------------------------------------- *//*!

     \class Descriptor
     \brief The range definitions for one or more contributors
   
      Typically all contributors will have the same range definitions.
      but, since contributors are feed by different WIB fibers, there
      is no guarantee of synchonization on these streams due to missing
      WIB frames.  Therefore, there may be multiple range definitions.

      To accommodate this, each range defintion includes a bit mask of
      which contributors are described.
                                                                          */
   /* ------------------------------------------------------------------- */
   class Descriptor
   {
      Descriptor () = delete;

   public:
      class Indices;
      class Timestamps;


   public:
      Indices const    *getIndices    () const { return &m_indices;    }
      Timestamps const *getTimestamps () const { return &m_timestamps; }

   public:
      /* ---------------------------------------------------------------- *//*!
 
        \struct Indices
        \brief  Defines the beginning and ending of the event in terms
                of an index into the data packets for one contributor.

        An index consists of two 16-bit values. The upper 16-bits
        gives the packet number and the lower 16-bits gives the offset 
        into the packet.
                                                                          */
      /* ---------------------------------------------------------------- */
      class Indices
      {
         Indices () = delete;
      public:
         
         uint32_t   getBegin   () const { return   m_begin; }
         uint32_t   getEnd     () const { return     m_end; }
         uint32_t   getTrigger () const { return m_trigger; }

         static int getPacket (uint32_t index) { return index >>    16; }
         static int getOffset (uint32_t index) { return index & 0xffff; }

         uint32_t   m_begin; /*!< Index of beginning of event time sample */
         uint32_t     m_end; /*!< Index to ending   of event time sample  */
         uint32_t m_trigger; /*!< Index to event triggering time sample   */         

      } __attribute__ ((packed));
      /* ---------------------------------------------------------------- */




      /* ---------------------------------------------------------------- *//*!
  
         \struct Timestamps
         \brief  Gives the beginning and ending timestamp of the data 
                 this contributor

                                                                          */
      /* ---------------------------------------------------------------- */
      class Timestamps
      {
         Timestamps () = delete;

      public: 
         uint64_t getBegin   () const { return   m_begin; }
         uint64_t getEnd     () const { return     m_end; }

      public:
         uint64_t m_begin;  /*!< Begining timestamp of the range          */
         uint64_t   m_end;  /*!< Ending   timestamp of the range          */
      } __attribute__ ((packed));
      /* ---------------------------------------------------------------- */

   public:
      Indices       m_indices; /*!< Indices to the event time samples     */
      Timestamps m_timestamps; /*!< Begin/end of data packet timestamps   */
   } __attribute__ ((packed));
   /* ------------------------------------------------------------------- */



   /* ------------------------------------------------------------------- *//*!

      \struct Window
      \brief  Gives the timestamps of the beginning, ending and trigger.

      \note
       This window is a property of the trigger and, therefore, is common
       for all contributors.
                                                                         */
   /* ------------------------------------------------------------------- */
   class Window
   {
      Window () = delete;

   public:
      uint64_t getBegin   () const { return   m_begin; }
      uint64_t getEnd     () const { return     m_end; }
      uint64_t getTrigger () const { return m_trigger; }

   public:
      uint64_t    m_begin; /*!< Beginning timestamp of the event window   */
      uint64_t      m_end; /*!< Begin     timestamp of the event window   */
      uint64_t  m_trigger; /*!< Triggering timestamp                      */

      
   } __attribute__ ((packed));
   /* ------------------------------------------------------------------- */

public:
   Window     const *getWindow     () const { return &m_window; }
   Descriptor const *getDescriptor () const { return    &m_dsc; }

public:
   Descriptor      m_dsc; /*!< Range descriptor                           */
   Window       m_window; /*!< Beginning and ending event timestamps      */
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
class Ranges : public RangesHeader
{
public:
   Ranges () = delete;

public:
   RangesBody const *getBody () const { return &m_body; }
   

   static void print (Ranges const *ranges)
   {
      ranges->RangesHeader::print ();

      unsigned int version = ranges->getVersion ();
      ranges->m_body.print (version);
   }

   void print () const { print (this); } 


public:
   RangesBody  m_body;   /*!< The body of the Range record                */
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \brief This specializes the bridge word of a standard header for use
          Toc usage.
                                                                          */
/* ---------------------------------------------------------------------- */
class TocHeader : public pdd::Header2
{
   TocHeader () = delete;

public:
   class Bridge;

public:
   void     print       () const {  print (this); }
   uint32_t getTocFormat() const { return getTocFormat(Header2::getBridge ()); }
   uint32_t getNDscs    () const { return getNDscs    (Header2::getBridge ()); }

   static int      getNDscs    (uint32_t bridge) { return Bridge::getNDscs (bridge); }
   static uint32_t getTocFormat(uint32_t bridge) { return Bridge::getTocFormat (bridge);}
   static void     print       (TocHeader const *hdr);


public:
   class Bridge
   {
   public:
      explicit Bridge (uint32_t bridge) : m_bridge (bridge) { return; }

      /* ---------------------------------------------------------------- *//*!

        \enum  class Size
        \brief Enumerates the sizes of the Bridge bit fields.
                                                                          */
      /* ---------------------------------------------------------------- */
      enum class Size: int
      {
         TocFormat =  4, /*!< Size of TOC record format field             */
         DscCount  =  8, /*!< Size of the count of descriptors field      */
      };
      /* ---------------------------------------------------------------- */


      /* ---------------------------------------------------------------- *//*!

        \enum  class Offset
        \brief Enumerates the right justified offsets of the Bridge
               bit fields.
                                                                          */
      /* ---------------------------------------------------------------- */
      enum class Offset: int
      {
         TocFormat =  0, /*!< Offset of the TOC record format field       */
         DscCount  =  4, /*!< Offset of the count of descriptors field    */
      };
      /* ---------------------------------------------------------------- */


      /* ---------------------------------------------------------------- *//*!

        \enum  class Mask
        \brief Enumerates the right justified masks of the Bridge
               bit fields.
                                                                         */
      /* --------------------------------------------------------------- */
      enum class Mask: uint32_t
      {
         TocFormat =  0x0000000f,  /*!< Mask of the record format field */
         DscCount  =  0x000000ff,  /*!< Mask of descriptor count  field */
      };
      /* --------------------------------------------------------------- */


   public:
      int getTocFormat () { return getTocFormat (m_bridge); }
      int getNDscs     () { return getNDscs     (m_bridge); }

      static int getTocFormat (uint32_t bridge)
      {
         int fmt = PDD_EXTRACT32 (bridge, Mask::TocFormat, Offset::TocFormat);
         return fmt;
      }

      static int getNDscs (uint32_t bridge)
      {
         int fmt = PDD_EXTRACT32 (bridge, Mask::DscCount, Offset::DscCount);
         return fmt;
      }

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
   TocBody () = delete;

public:
   class Contributor;
   class PacketDsc;

public:
   static void print (TocBody const  *body, 
                      uint32_t      bridge);


   void print      (uint32_t bridge) const;

   uint32_t     const          *getW32 () const;
   PacketDsc    const   *getPacketDscs () const;


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
      /* ---------------------------------------------------------------- *//*!

        \enum  class Size
        \brief Enumerates the sizes of the PacketDsc bit fields.
                                                                          */
      /* ---------------------------------------------------------------- */
      enum class Size: int
      {
         Format    =  4, /*!< Size of the format field                    */
         Type      =  4, /*!< Size fo the type field                      */  
         Offset64  = 24  /*!< Size of the offset field, in 64 bit units   */
      };
      /* ---------------------------------------------------------------- */


      /* ---------------------------------------------------------------- *//*!

        \enum  class Offset
        \brief Enumerates the right justified offsets of the PacketDsc bit 
               fields.
                                                                          */
      /* ---------------------------------------------------------------- */
      enum class Offset: int
      {
         Format    =  0, /*!< Offset of the format field                  */
         Type      =  4, /*!< Offset of the frame type field              */
         Offset64  =  8  /*!< Offset of the offset index field, 64-bits   */
      };
      /* ---------------------------------------------------------------- */


      /* ---------------------------------------------------------------- *//*!

        \enum  class Mask
        \brief Enumerates the right justified masks of the PacketDsc bit 
               fields.
                                                                         */
      /* --------------------------------------------------------------- */
      enum class Mask: uint32_t
      {
         Format    =  0x0000000f,
         Type      =  0x0000000f,
         Offset64  =  0x00ffffff
      };
      /* --------------------------------------------------------------- */

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
   Toc () = delete;

public:
   TocBody const *getBody () const { return &m_body; }

   static void print (Toc const *toc)
   {
      toc->TocHeader::print ();

      uint32_t bridge  = toc->getBridge ();
      toc->m_body.print (bridge);
   }


   void print () const { print (this); } 
   

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
   TpcPacketHeader () = delete;
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief Body of a TPC data packet
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcPacketBody 
{
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
   TpcPacket () = delete;

public:
   TpcPacketBody const *getBody () const { return &m_body; }

private:
   TpcPacketBody m_body;
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Prints the body of Table of Contents

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

inline unsigned int TocBody::PacketDsc::getFormat () const
{
   unsigned int format =
      PDD_EXTRACT32 (m_w32, Mask::Format, Offset::Format);
   return format;
}

inline TocBody::PacketDsc::Type TocBody::PacketDsc::getType () const
{
   unsigned int type =
      PDD_EXTRACT32 (m_w32, Mask::Type, Offset::Type);
   return static_cast<Type>(type);
}

inline unsigned int TocBody::PacketDsc::getOffset64 () const
{
   unsigned int o64 =
      PDD_EXTRACT32 (m_w32, Mask::Offset64, Offset::Offset64);
   return o64;
}

inline bool TocBody::PacketDsc::isWibFrame () const 
{
   return (getType () == Type::WibFrame);
}


inline WibFrame const *TpcPacketBody::locateWibFrames ()
{
   return reinterpret_cast<WibFrame const *>(this);
}

inline WibFrame const *TpcPacketBody::locateWibFrame (int idx)
{
   return &locateWibFrames()[idx];
}

/* ---------------------------------------------------------------------- */
}  /* Namespace:: fragment                                                */
}  /* Namespace:: pdd                                                     */
/* ---------------------------------------------------------------------- */




#endif
