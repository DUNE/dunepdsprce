// -*-Mode: C++;-*-

#ifndef RECORDS_TPCTOC_HH
#define RECORDS_TPCTOC_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcToc.hh
 *  @brief    Proto-Dune Data Tpc Table Of Contents records definition
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
 *  <2017/10/15>
 *
 * @par Credits:
 * SLAC
 *
 * This defines the format of the Table of Contents records found in a
 * Tpc record.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.10.16 jjr Moved from dam/access -> dam/records
   2017.08.07 jjr Created
  
\* ---------------------------------------------------------------------- */

#include "dam/access/Headers.hh"
#include <cstdint> 
#include <cstdio>


namespace pdd    {
namespace record {

/* ====================================================================== */
/* CLASSS DEFINITIONS                                                     */
/* ---------------------------------------------------------------------- *//*!

   \brief This specializes the bridge word of a standard header for use
          Toc usage.
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcTocHeader : public pdd::Header2
{
private:
   TpcTocHeader () = delete;

public:
   class Bridge
   {
   private:
      Bridge () = delete;

   public:
      /* ---------------------------------------------------------------- *//*!

         \enum  class Size
         \brief Enumerates the sizes of the Bridge bit fields.
                                                                          */
      /* ---------------------------------------------------------------- */
      enum class Size: int
      {
         Format   =  4, /*!< Size of TOC record format field              */
         DscCount =  8  /*!< Size of the count of descriptors field       */
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
         Format   =  0, /*!< Offset of the TOC record format field        */
         DscCount =  4  /*!< Offset of the count of descriptors field     */
      };
      /* ---------------------------------------------------------------- */


      /* ---------------------------------------------------------------- *//*!

        \enum  class Mask
        \brief Enumerates the right justified masks of the Bridge
               bit fields.
                                                                          */
      /* ---------------------------------------------------------------- */
      enum class Mask: uint32_t
      {
         Format   = 0x0000000f,  /*!< Mask of the record format field     */
         DscCount = 0x000000ff   /*!< Mask of descriptor count  field     */
      };
      /* ---------------------------------------------------------------- */
   };
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \class TpcTocPacketDsc
  \brief The format of a TpcTocPacket descriptor, essentially one 
         table of contents entry.
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcTocPacketDsc
{
   TpcTocPacketDsc () = delete;

public:
   enum class Type
   {
      WibFrame   = 1, /*!< Raw WIB frames                                 */
      Transposed = 2, /*!< Transposed, but not compressed                 */
      Compressed = 3  /*!< Compressed data                                */
   };        


   /* ------------------------------------------------------------------- *//*!

     \enum  class Size
     \brief Enumerates the sizes of the PacketDsc bit fields.
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Size: int
   {
      Format    =  4, /*!< Size of the format field                       */
      Type      =  4, /*!< Size fo the type field                         */  
      Offset64  = 24  /*!< Size of the offset field, in 64 bit units      */
   };
   /* ------------------------------------------------------------------- */


   /* ------------------------------------------------------------------- *//*!

     \enum  class Offset
     \brief Enumerates the right justified offsets of the PacketDsc bit 
            fields.
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Offset: int
   {
      Format    =  0, /*!< Offset of the format field                     */
      Type      =  4, /*!< Offset of the frame type field                 */
      Offset64  =  8  /*!< Offset of the offset index field, 64-bits      */
   };
   /* ------------------------------------------------------------------- */


   /* ------------------------------------------------------------------- *//*!

     \enum  class Mask
     \brief Enumerates the right justified masks of the PacketDsc bit 
            fields.
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Mask: uint32_t
   {
      Format    =  0x0000000f,
      Type      =  0x0000000f,
      Offset64  =  0x00ffffff
   };
   /* ------------------------------------------------------------------- */

public:
      uint32_t m_w32;
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \class TpcTocBody
  \brief Table of contents for the data pieces
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcTocBody
{
private:
   TpcTocBody () = delete;

public:
   TpcTocPacketDsc const  m_dscs[1]; /*!< The packet descriptors          */
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief The table of contents record = header + body
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcToc : public TpcTocHeader
{
private:
   TpcToc () = delete;

public:
   TpcTocBody m_body;

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
/* END DEFINITIONS                                                        */
/* ====================================================================== */

/* ---------------------------------------------------------------------- */
}  /* Namespace:: record                                                  */
}  /* Namespace:: pdd                                                     */
/* ---------------------------------------------------------------------- */

#endif
