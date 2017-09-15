// -*-Mode: C++;-*-

#ifndef PD_DAM_HEADERS_HH
#define PD_DAM_HEADERS_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     Headers.hh
 *  @brief    Proto-Dune Data Header 
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
 *  <2017/08/12>
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
   2017.08.12 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/BfExtract.hh"

#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cinttypes>

namespace pdd {

/* ---------------------------------------------------------------------- *//*!

  \class  Header 0
  \brief  Generic format = 0 header
                                                                          */
/* ---------------------------------------------------------------------- */
class Header0
{
   Header0 () = delete;

public:
   Header0 (uint64_t w64)        : m_w64 (w64)  { return; }
   Header0 (uint64_t const *p64) : m_w64 (*p64) { return; }

public:
   uint64_t   retrieve () const { return             m_w64;  }
   uint32_t  getFormat () const { return  getFormat (m_w64); }
   uint32_t    getType () const { return    getType (m_w64); }
   uint32_t     getN64 () const { return     getN64 (m_w64); }
   uint32_t  getNaux64 () const { return  getNaux64 (m_w64); }
   uint32_t getSubtype () const { return getSubtype (m_w64); }
   uint32_t  getBridge () const { return  getBridge (m_w64); }


   static uint32_t getFormat  (uint64_t w64);
   static uint32_t getType    (uint64_t w64);
   static uint32_t getN64     (uint64_t w64);
   static uint32_t getNaux64  (uint64_t w64);
   static uint32_t getSubtype (uint64_t w64);
   static uint32_t getBridge  (uint64_t w64);

private:
   /* ------------------------------------------------------------------- *//*!

     \enum  class Size
     \brief Enumerates the sizes of the Header bit fields.
                                                                         */
   /* ------------------------------------------------------------------ */
   enum class Size: int
   {
      Format    =  4, /*!< Size of the format field                      */
      Type      =  4, /*!< Size of the frame type field                  */
      N64       = 24, /*!< Size of the length field                      */
      NAux64    =  4, /*!< Size of the auxillary length field            */
      SubType   =  4, /*!< Size of the record's subtype field            */
      Specific1 = 24  /*!< Size of the first type specific field         */
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
      Format    =  0, /*!< Offset to the format field                    */
      Type      =  4, /*!< Offset to the frame type field                */
      N64        =  8, /*!< Offset to the length field                    */
      NAux64    = 32, /*!< Offset to the auxillary length field          */
      SubType   = 36, /*!< Offset to the record's subtype field          */
      Bridge    = 40  /*!< Offset to the bridge word                     */
   };
   /* ------------------------------------------------------------------ */


   /* ------------------------------------------------------------------- *//*!

     \enum  class Offset
     \brief Enumerates the right justified masks of the header bit fields.
                                                                         */
   /* ------------------------------------------------------------------ */
   enum class Mask: uint32_t
   {
      Format    =  0x0000000f,
      Type      =  0x0000000f,
      N64       =  0x00ffffff,
      NAux64    =  0x0000000f,
      SubType   =  0x0000000f,
      Bridge    =  0x00ffffff
   };
   /* ------------------------------------------------------------------ */


public:
   uint64_t m_w64;
}  __attribute__ ((packed));
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \class  Header 1
  \brief  Generic format = 1 header
                                                                          */
/* ---------------------------------------------------------------------- */
class Header1
{
   Header1 () = delete;

public:
   Header1 (uint64_t w64) : m_w64 (w64) { return; }

public:
   uint64_t retrieve  () const { return              m_w64; }
   int      getFormat () const { return getFormat  (m_w64); }
   int      getType   () const { return getType    (m_w64); }
   int      getN64    () const { return getN64     (m_w64); }
   int      getBridge () const { return getBridge  (m_w64); }


   static uint32_t getFormat (uint64_t w64);
   static uint32_t getType   (uint64_t w64);
   static uint32_t getN64    (uint64_t w64);
   static uint32_t getBridge (uint64_t w64);


   /* ------------------------------------------------------------------- *//*!

     \enum  class Size
     \brief Enumerates the sizes of the Header bit fields.
                                                                         */
   /* ------------------------------------------------------------------ */
   enum class Size: int
   {
      Format    =  4, /*!< Size of the format field                      */
      Type      =  4, /*!< Size of the record/frame type    field        */
      Length    = 24, /*!< Size of the length field                      */
      Bridge    = 32  /*!< Size of the bridge field                      */
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
      Format    =  0, /*!< Offset of the format field                    */
      Type      =  4, /*!< Offset of the frame type field                */
      N64       =  8, /*!< Offset of the length field                    */
      Bridge    = 32  /*!< Offset of the bridge field                    */
   };
   /* ------------------------------------------------------------------ */


   /* ------------------------------------------------------------------- *//*!

     \enum  class Offset
     \brief Enumerates the right justified masks of the header bit fields.
                                                                         */
   /* ------------------------------------------------------------------ */
   enum class Mask: uint32_t
   {
      Format    =  0x0000000f,
      Type      =  0x0000000f,
      N64       =  0x00ffffff,
      Bridge    =  0xffffffff
   };
   /* ------------------------------------------------------------------ */

private:
   uint64_t m_w64;
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \class  Header 2
  \brief  Generic format = 1 header
                                                                          */
/* ---------------------------------------------------------------------- */
class Header2
{
   Header2 () = delete;

public:
   Header2 (uint32_t w32) : m_w32 (w32) { return; }
   Header2 (uint64_t w64) : m_w32 (w64) { return; }

public:
   uint32_t  retrieve () const { return            m_w32;  }
   uint32_t getFormat () const { return getFormat (m_w32); }
   uint32_t   getType () const { return   getType (m_w32); }
   uint32_t    getN64 () const { return    getN64 (m_w32); }
   uint32_t getBridge () const { return getBridge (m_w32); }


   static uint32_t getFormat (uint32_t w32);
   static uint32_t getType   (uint64_t w32);
   static uint32_t getN64    (uint32_t w32);
   static uint32_t getBridge (uint32_t w32);


   /* ------------------------------------------------------------------- *//*!

     \enum  class Size
     \brief Enumerates the sizes of the Header bit fields.
                                                                         */
   /* ------------------------------------------------------------------ */
   enum class Size: int
   {
      Format    =  4, /*!< Size of the format field                      */
      Type      =  4, /*!< Size of the record/frame type    field        */
      N64       = 12, /*!< Size of the length field                      */
      Bridge    = 12 /*!< Size of the bridge word field                 */
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
      Format    =  0, /*!< Offset of the format field                    */
      Type      =  4, /*!< Offset of the frame type field                */
      N64       =  8, /*!< Offset of the length field                    */
      Bridge    = 20  /*!< Offset of the bridge word field               */
   };
   /* ------------------------------------------------------------------ */


   /* ------------------------------------------------------------------- *//*!

     \enum  class Offset
     \brief Enumerates the right justified masks of the header bit fields.
                                                                         */
   /* ------------------------------------------------------------------ */
   enum class Mask: uint32_t
   {
      Format    =  0x0000000f,
      Type      =  0x0000000f,
      N64       =  0x00000fff,
      Bridge    =  0x00000fff
   };
   /* ------------------------------------------------------------------ */


private:
   uint32_t m_w32;
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \class  Trailer
  \brief  Generic trailers.  Trailers are always the complemented value
          of its corresponding Header.  As such, they are not actually
          filled on a field by field basis.
                                                                          */
/* ---------------------------------------------------------------------- */
class Trailer
{
public:
   Trailer (uint64_t  header) = delete;

   uint64_t retrieve () const { return m_w64; }

public:
   uint64_t m_w64;
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
namespace fragment
{

static const uint32_t Pattern = 0x8b309e;

/* ---------------------------------------------------------------------- *//*!
      
   \enum  class Type
   \brief Enumerates the type of fragment. A fragment is the largest 
          self-contained piece of data that originates from 1 
          contributor.
                                                                          */
/* ---------------------------------------------------------------------- */
enum class Type
{
   Reserved_0    = 0, /*!< Reserved for future use                        */
   Control       = 1, /*!< Control, \e e.g. Begin/Pause/Resume/Stop etc.  */
   Data          = 2, /*!< Detector data of some variety                  */
   MonitorSync   = 3, /*!< Statistics synch'd across contributors         */
   MonitorUnSync = 4  /*!< Statistics for a single contributor            */
};
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \class Identifier
  \brief Gives the spatial (which electronics components) and temporal 
         (the timestamp/instance) of the fragment

   This class is normally used asthe auxilliary block of the fragment 
   header.
                                                                          */
/* ---------------------------------------------------------------------- */
class Identifier
{
   Identifier ()  = delete;


public:
   /* ------------------------------------------------------------------- *//*!

     \enum  class FormatType
     \brief This enumerates the identifier formats. It is really more
            for documentation than usage
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class FormatType
   {
      _0 = 0,  /*!< Only 1 source identifier                              */
      _1 = 1   /*!< Two source identifiers                                */
   };
   /* ------------------------------------------------------------------- */

public:
   uint32_t getFormat          () const { return     getFormat (m_w64);      }
   uint32_t getSrc      (int idx) const { return        getSrc (m_w64, idx); }
   uint32_t getSequence        () const { return   getSequence (m_w64);      }
   uint32_t getTimestamp       () const { return           m_timestamp;      }

   void     print              () const { print (this);                      }


   static uint32_t getFormat   (uint64_t w64);
   static uint32_t getType     (uint64_t w64);
   static uint32_t getSrc0     (uint64_t w64);
   static uint32_t getSrc1     (uint64_t w64);
   static uint32_t getSrc      (uint64_t w64, int idx);
   static uint32_t getSequence (uint64_t w64);
   static void     print       (Identifier const *identifier);


private:
   enum class Size
   {
      Format   =  4, /*!< Size of the format field                       */
      Reserved =  4, /*!< Reserved, must be 0                            */
      Src0     = 12, /*!< Channel bundle 0 source identifier (from WIB)  */
      Src1     = 12, /*!< Channel bundle 1 source identified (from WIB)  */
      Sequence = 32  /*!< Overall sequence number                        */
   };

   enum class Mask : uint32_t
   {
      Format   = 0x0000000f,
      Type     = 0x0000000f,
      Src0     = 0x00000fff,
      Src1     = 0x00000fff,
      Sequence = 0xffffffff
   };

   enum Offset
   {
      Format    = 0,
      Type      = 4,
      Src0      = 8,
      Src1      = 20,
      Sequence  = 32
   };


private:
   uint64_t       m_w64; /*!< 64-bit packed word of format,srcs, sequence */
   uint64_t m_timestamp; /*!< The identifying timestamp                   */
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief  Template class for Fragment Headers

   This class is specialized for the various types of fragment headers
                                                                          */
/* ---------------------------------------------------------------------- */
template<enum fragment::Type TYPE>
class Header : public Header0
{
   Header  () = delete;
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Specialized fragment header for Data
                                                                          */
/* ---------------------------------------------------------------------- */
template<>
class Header<fragment::Type::Data> : public Header0
{
   Header<fragment::Type::Data> () = delete;

public:
   enum class RecType 
   {
      Reserved_0   = 0,  /*!< Reserved for future use                     */
      Originator   = 1,  /*!< Originator record type                      */
      TpcNormal    = 2,  /*!< Normal  TPC data, \e i.e. no errors         */
      TpcDamaged   = 3   /*!< Damaged TPC data, \e i.e. has errors        */
   };

   Header<fragment::Type::Data>::RecType getRecType () const;

   Identifier getIdentifier () const  { return m_identifier; }
   bool       isTpc         () const  { return isTpc        (getRecType ()); }
   bool       isTpcNormal   () const  { return isTpcNormal  (getRecType ()); }
   bool       isTpcDamaged  () const  { return isTpcDamaged (getRecType ()); }


   static bool isTpcNormal  (RecType recType);
   static bool isTpcDamaged (RecType recType);
   static bool isTpc        (RecType recType);

private:
   Identifier  m_identifier;
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
class Versions
{
   Versions () = delete;

public:
   static uint32_t getFirmware (uint64_t m_64) { return m_64 >>  0; }
   static uint32_t getSoftware (uint64_t m_64) { return m_64 >> 32; }

   uint32_t getSoftware () const { return getSoftware (m_w64); }
   uint32_t getFirmware () const { return getFirmware (m_w64); }

private:
   uint64_t       m_w64;
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \class OriginatorBody
  \brief The hardware, software and geographic information about this
         data's orginator
                                                                          */
/* ---------------------------------------------------------------------- */
class OriginatorBody
{
   OriginatorBody () = delete;

public:
   uint32_t        getLocation     () const { return     m_location; }
   uint64_t        getSerialNumber () const { return m_serialNumber; }
   Versions const &getVersions     () const { return     m_versions; }
   char const     *getRptSwTag     () const { return      m_strings; }
   char const     *getGroupName    () const { return &m_strings[strlen(getRptSwTag())+1];}
   void            print           () const {  print (this); }
   static void     print           (OriginatorBody const *body);

private:
   uint32_t     m_location;
   uint64_t m_serialNumber;
   Versions     m_versions;
   char   m_strings[32+32];
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \class Originator
  \brief Information about the physical entity (RCE) producing the data
         and the software and firmware running on it
                                                                          */
/* ---------------------------------------------------------------------- */
class Originator : public Header2
{
   Originator () = delete;

public:
   OriginatorBody const &body () const { return m_body; }

   void                 print () const;

public:
   OriginatorBody m_body;
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief Specializes a standard header for a generic Data Record
                                                                          */
/* ---------------------------------------------------------------------- */
class DataHeader : public Header1
{
   DataHeader () = delete;

public:
   void        print () const { print (this);  };

   static void print (DataHeader const *dh);
};
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \brief Specializes a standard header for a generic Data Record
                                                                          */
/* ---------------------------------------------------------------------- */
class Data : public DataHeader
{
   Data () = delete;

public:
};
/* ---------------------------------------------------------------------- */
} /* END: namespace fragment                                              */
} /* END: namespace pdd                                                   */
/* ====================================================================== */




namespace pdd {
/* ====================================================================== */
/* IMPLEMENTATION : Header0                                               */
/* ---------------------------------------------------------------------- */
inline uint32_t Header0::getFormat (uint64_t w64) 
{ 
   return PDD_EXTRACT64 (w64,  Mask::Format, Offset::Format);
}

inline uint32_t Header0::getType   (uint64_t w64) 
{
   return PDD_EXTRACT64 (w64,  Mask::Type,  Offset::Type);
}

inline uint32_t Header0::getN64    (uint64_t w64) 
{
   return PDD_EXTRACT64 (w64, Mask::N64, Offset::N64);
}

inline uint32_t Header0::getNaux64 (uint64_t w64) 
{ 
   return PDD_EXTRACT64 (w64,   Mask::NAux64,   Offset::NAux64);
}

inline uint32_t Header0::getSubtype (uint64_t w64)
{ 
   return PDD_EXTRACT64 (w64, Mask::SubType, Offset::SubType);
}

inline uint32_t Header0::getBridge (uint64_t w64)
{ 
   return PDD_EXTRACT64 (w64, Mask::Bridge, Offset::Bridge);
}
/* ---------------------------------------------------------------------- */
/* IMPLEMENTATION : Header0                                               */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION : Header1                                               */
/* ---------------------------------------------------------------------- */
inline uint32_t Header1::getFormat (uint64_t w64)
{
   return PDD_EXTRACT64 (w64, Mask::Format, Offset::Format);
}


inline uint32_t Header1::getType (uint64_t w64)
{
   return PDD_EXTRACT64 (w64, Mask::Type, Offset::Type);
}

inline uint32_t Header1::getN64 (uint64_t w64)
{
   return PDD_EXTRACT64 (w64, Mask::N64, Offset::N64);
}

inline uint32_t Header1::getBridge (uint64_t w64)
{
   uint32_t bridge = PDD_EXTRACT64 (w64, Mask::Bridge, Offset::Bridge);
   return bridge;
}
/* ---------------------------------------------------------------------- */
/* END : Header1                                                          */
/* ====================================================================== */



/* ====================================================================== */
/* IMPLEMENTATION : Header2                                               */
/* ---------------------------------------------------------------------- */
inline uint32_t Header2::getFormat (uint32_t w32)
{
   return PDD_EXTRACT32 (w32, Mask::Format, Offset::Format);
}

inline uint32_t Header2::getType (uint64_t w32)
{
   return PDD_EXTRACT32 (w32, Mask::Type, Offset::Type);
}

inline uint32_t Header2::getN64 (uint32_t w32)
{
   return PDD_EXTRACT32 (w32, Mask::N64, Offset::N64);
}

inline uint32_t Header2::getBridge (uint32_t w32)
{
   return PDD_EXTRACT32 (w32, Mask::Bridge, Offset::Bridge);
}
/* ---------------------------------------------------------------------- */
/* END : Header1                                                          */
/* ====================================================================== */


namespace fragment {




/* ====================================================================== */
/* IMPLEMENTATION : Identifier                                            */
/* ---------------------------------------------------------------------- */
inline uint32_t Identifier::getFormat (uint64_t w64) 
{
   return PDD_EXTRACT64 (w64,  Mask::Format, Offset::Format);
}

inline uint32_t Identifier::getType (uint64_t w64) 
{
   return PDD_EXTRACT64 (w64,  Mask::Type, Offset::Type);
}

inline uint32_t Identifier::getSrc0 (uint64_t w64)
{
   return PDD_EXTRACT64 (w64, Mask::Src0, Offset::Src0);
}

inline uint32_t Identifier::getSrc1 (uint64_t w64)
{
   return PDD_EXTRACT64 (w64, Mask::Src1, Offset::Src1);
}

inline uint32_t Identifier::getSrc (uint64_t w64, int idx)
{
   return idx ? getSrc0 (w64) : getSrc1 (w64);
}

inline uint32_t Identifier::getSequence (uint64_t w64)
{
   return PDD_EXTRACT64 (w64, Mask::Sequence, Offset::Sequence);
}
/* ---------------------------------------------------------------------- */
/* END: Identifier                                                        */
/* ====================================================================== */


/* ====================================================================== */
/* IMPLEMENTATION: Header<fragment::Type::Data>                           */
/* ---------------------------------------------------------------------- */
inline Header<fragment::Type::Data>::RecType 
       Header<fragment::Type::Data>::getRecType () const
{
   return static_cast<Header<fragment::Type::Data>::RecType>
                            (Header0::getSubtype ());
}

inline int x () { return 0; }

inline bool         Header<fragment::Type::Data>::
       isTpcNormal (Header<fragment::Type::Data>::RecType recType)
{
   return (recType == Header<fragment::Type::Data>::RecType::TpcNormal);
}

inline bool          Header<fragment::Type::Data>::
       isTpcDamaged (Header<fragment::Type::Data>::RecType recType)
{
   return (recType == Header<fragment::Type::Data>::RecType::TpcDamaged);
}

inline bool   Header<fragment::Type::Data>::
       isTpc (Header<fragment::Type::Data>::RecType recType)
{
   static const uint32_t TpcTypes = 
      (1 << (static_cast<int>(Header<fragment::Type::Data>::RecType::TpcNormal))) |
      (1 << (static_cast<int>(Header<fragment::Type::Data>::RecType::TpcNormal)));

   int rtm = 1 << static_cast<uint32_t>(recType);

   return (rtm & TpcTypes) != 0;
}
/* ---------------------------------------------------------------------- */
/* END: Header<fragment::Type::Data>                                      */
/* ====================================================================== */
} /* END: namespace: fragment                                             */
} /* END: namespace: pdd                                                  */
/* ====================================================================== */

#endif
