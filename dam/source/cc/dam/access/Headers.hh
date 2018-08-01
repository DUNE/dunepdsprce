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
   2018.07.11 jjr Added definition of Header3
   2017.08.12 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/util/BfExtract.hh"

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
public:
   explicit Header0 () { return; }

public:
   Header0 (uint64_t w64)        : m_w64 (w64)  { return; }
   Header0 (uint64_t const *p64) : m_w64 (*p64) { return; }

public:
   uint64_t   retrieve () const { return             m_w64;  }
   uint32_t  getFormat () const { return  getFormat (m_w64); }
   uint32_t    getType () const { return    getType (m_w64); }
   uint32_t     getN64 () const { return     getN64 (m_w64); }
   uint32_t  getNbytes () const { return  getNbytes (m_w64); }
   uint32_t  getNaux64 () const { return  getNaux64 (m_w64); }
   uint32_t getSubtype () const { return getSubtype (m_w64); }
   uint32_t  getBridge () const { return  getBridge (m_w64); }


   static uint32_t getFormat  (uint64_t w64);
   static uint32_t getType    (uint64_t w64);
   static uint32_t getN64     (uint64_t w64);
   static uint32_t getNbytes  (uint64_t w64);
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
      Bridge    = 24  /*!< Size of the bridge word                       */
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
      N64       =  8, /*!< Offset to the length field                    */
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
public:
   explicit Header1 () { return; }

public:
   Header1 (uint64_t w64) : m_w64 (w64) { return; }

public:
   uint64_t retrieve  () const { return              m_w64; }
   int      getFormat () const { return getFormat  (m_w64); }
   int      getType   () const { return getType    (m_w64); }
   int      getN64    () const { return getN64     (m_w64); }
   int      getNbytes () const { return getNbytes  (m_w64); }
   int      getBridge () const { return getBridge  (m_w64); }


   static uint32_t getFormat (uint64_t w64);
   static uint32_t getType   (uint64_t w64);
   static uint32_t getN64    (uint64_t w64);
   static uint32_t getNbytes (uint64_t w64);
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
  \brief  Generic format = 2 header
                                                                          */
/* ---------------------------------------------------------------------- */
class Header2
{
public:
   explicit Header2 () { return; }

public:
   Header2 (uint32_t w32) : m_w32 (w32) { return; }
   Header2 (uint64_t w64) : m_w32 (w64) { return; }

public:
   uint32_t  retrieve () const { return            m_w32;  }
   uint32_t getFormat () const { return getFormat (m_w32); }
   uint32_t   getType () const { return   getType (m_w32); }
   uint32_t    getN64 () const { return    getN64 (m_w32); }
   uint32_t getNbytes () const { return getNbytes (m_w32); }
   uint32_t getBridge () const { return getBridge (m_w32); }


   static uint32_t getFormat (uint32_t w32);
   static uint32_t getType   (uint64_t w32);
   static uint32_t getN64    (uint32_t w32);
   static uint32_t getNbytes (uint32_t w32);
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
      Bridge    = 12  /*!< Size of the bridge word field                 */
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

  \class  Header 3
  \brief  Generic format = 2 header
                                                                          */
/* ---------------------------------------------------------------------- */
class Header3
{
public:
   explicit Header3 () { return; }

public:
   Header3 (uint64_t w64) : m_w64 (w64) { return; }

public:
   uint64_t  retrieve () const { return            m_w64;  }
   uint32_t getFormat () const { return getFormat (m_w64); }
   uint32_t   getType () const { return   getType (m_w64); }
   uint32_t    getN64 () const { return    getN64 (m_w64); }
   uint32_t getNbytes () const { return getNbytes (m_w64); }
   uint64_t getBridge () const { return getBridge (m_w64); }


   static uint32_t getFormat (uint64_t w64);
   static uint32_t getType   (uint64_t w64);
   static uint32_t getN64    (uint32_t w64);
   static uint32_t getNbytes (uint32_t w64);
   static uint64_t getBridge (uint64_t w64);



   /* ------------------------------------------------------------------- *//*!

     \enum  class Size
     \brief Enumerates the sizes of the Header bit fields.
                                                                         */
   /* ------------------------------------------------------------------ */
   enum class Size: int
   {
      Format    =  4, /*!< Size of the format field                      */
      Type      =  4, /*!< Size of the record/frame type    field        */
      N64       = 16, /*!< Size of the length field                      */
      Bridge    = 40  /*!< Size of the bridge word field                 */
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
      Bridge    =  8  /*!< Offset of the bridge word field               */
   };
   /* ------------------------------------------------------------------ */


   /* ------------------------------------------------------------------- *//*!

     \enum  class Offset
     \brief Enumerates the right justified masks of the header bit fields.
                                                                         */
   /* ------------------------------------------------------------------ */
   enum class Mask: uint64_t
   {
      Format    =      0x0000000f,
      Type      =      0x0000000f,
      N64       =      0x00000fff,
      Bridge    =  0xffffffffffLL
   };
   /* ------------------------------------------------------------------ */


private:
   uint64_t m_w64;
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
} /* END: namespace pdd                                                   */
/* ====================================================================== */



/* ====================================================================== */
namespace pdd      {
namespace fragment {

/* ---------------------------------------------------------------------- */
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
} /* END: namespace fragment                                              */
} /* END: namespace pdd                                                   */
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
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

inline uint32_t Header0::getNbytes (uint64_t w64) 
{
   return PDD_EXTRACT64 (w64, Mask::N64, Offset::N64) * sizeof (uint64_t);
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

inline uint32_t Header1::getNbytes (uint64_t w64)
{
   return PDD_EXTRACT64 (w64, Mask::N64, Offset::N64) * sizeof (uint64_t);
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

inline uint32_t Header2::getNbytes (uint32_t w32)
{
   return PDD_EXTRACT32 (w32, Mask::N64, Offset::N64) * sizeof (uint64_t);
}


inline uint32_t Header2::getBridge (uint32_t w32)
{
   return PDD_EXTRACT32 (w32, Mask::Bridge, Offset::Bridge);
}
/* ---------------------------------------------------------------------- */
/* END : Header2                                                          */
/* ====================================================================== */



/* ====================================================================== */
/* IMPLEMENTATION : Header3                                               */
/* ---------------------------------------------------------------------- */
inline uint32_t Header3::getFormat (uint64_t w64)
{
   return PDD_EXTRACT64 (w64, Mask::Format, Offset::Format);
}

inline uint32_t Header3::getType (uint64_t w64)
{
   return PDD_EXTRACT64 (w64, Mask::Type, Offset::Type);
}

inline uint32_t Header3::getN64 (uint32_t w64)
{
   return PDD_EXTRACT64 (w64, Mask::N64, Offset::N64);
}

inline uint32_t Header3::getNbytes (uint32_t w64)
{
   return PDD_EXTRACT64 (w64, Mask::N64, Offset::N64) * sizeof (uint64_t);
}

inline uint64_t Header3::getBridge (uint64_t w64)
{
   return PDD_EXTRACT64 (w64, Mask::Bridge, Offset::Bridge);
}
/* ---------------------------------------------------------------------- */
/* END : Header3                                                          */
/* ====================================================================== */


/* ====================================================================== */
} /* END: namespace: pdd                                                  */
/* ====================================================================== */

#endif
