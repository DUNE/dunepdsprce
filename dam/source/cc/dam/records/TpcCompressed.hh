// -*-Mode: C++;-*-

#ifndef RECORDS_TPC_COMPRESSED_HH
#define RECORDS_TPC_COMPRESSED_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     records/TpcCompressed.hh
 *  @brief    Definition of the Tpc Compressed data record and sub-records
 *  @verbatim
 *                               Copyright 2018
 *                                    by
 *
 *                       The Board of Trustees of the
 *                    Leland Stanford Junior University.
 *                           All rights reserved.
 *
 *  @endverbatim
 *
 *  @par Facility:
 *  DUNE
 *
 *  @author
 *  russell@slac.stanford.edu
 *
 *  @par Date created:
 *  2018.07.11
 * *
 * @par Credits:
 * SLAC
 *
 * @par
 * The TpcCompressedHdr record contains the data that is common to all
 * channels in a WIB frame.  This consists of the following pieces
 *   -# 7 words giving the 6 header from the initial WIB frame + the
 *        timestamp from the last WIB frame
 *   -# a counted list of exceptions.  Each exception contains 2 fields
 *      a 6-bit field indicating which of the 6 WIB header words failed
 *      to match its prediction and the frame number where the mismatch
 *      occurred. The most common example of this is likely the timestamp
 *      and the 2 sequence words in the cold data linnks.  This will occur 
 *      when a frame is dropped.
 *   -# A counted list of heder words.  These should be peeled off 
 *      one-by-one for each set exception bit.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\

   HISTORY
   -------

   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.07.11 jjr Created

\* ---------------------------------------------------------------------- */


#include "dam/access/Headers.hh"


namespace pdd    {
namespace record {


/* ====================================================================== *//*!

  \class TpcCompressedHeader
  \brief The Tpc Compressed Data record header

  \par
   This specializes the generic record header for the Tpc Compressed Data
   record header
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcCompressedHeader : public pdd::Header1
{
public:
   TpcCompressedHeader () = delete;

public:
};
/* ---------------------------------------------------------------------- */
/* END: TpcCompressedHeader                                               */
/* ====================================================================== */




/* ====================================================================== */
/* BEGIN: TpcCompressedHdr                                                */
/* ---------------------------------------------------------------------- *//*!

  \brief Layout of the compressed packet record header for the Tpc 
         WIB/Colddata header words.
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcCompressedHdrHeader : public pdd::Header3
{
private:
   TpcCompressedHdrHeader () = delete;

public:
   class Bridge
   {
   private:
      Bridge () = delete;

   public:
        public:
      /* ---------------------------------------------------------------- *//*!

         \enum  class Size
         \brief Enumerates the sizes of the Bridge bit fields.
                                                                          */
      /* ---------------------------------------------------------------- */
      enum class Size: int
      {
         ExcCount =  8, /*!< Size of the exception word count             */
         Status   = 32  /*!< Size of the status word                      */
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
         ExcCount =  0, /*!< Offset of the exception word count           */
         Status   =  8  /*!< Offset of the status word                    */
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
         ExcCount = 0x0000000f,  /*!< Mask of the exception word count    */
         Status   = 0x000000ff   /*!< Mask of the status word             */
      };
      /* ---------------------------------------------------------------- */
   };

      
};
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

   \brief  The Compressed Header words record body
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcCompressedHdrBody
{
private:
   TpcCompressedHdrBody () = delete;


public:
   uint64_t const m_w64[1];

};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief TpcCompressedHdr
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcCompressedHdr : public TpcCompressedHdrHeader
{
private:
   TpcCompressedHdr () = delete;

public:
   TpcCompressedHdrBody m_body;
};
/* ---------------------------------------------------------------------- */
/* TpcCompressedHdr                                                       */
/* ====================================================================== */



/* ====================================================================== */
class TpcCompressedTocTrailer
{
public:
   TpcCompressedTocTrailer () = delete;

public:
   uint64_t m_w64;
};
/* ---------------------------------------------------------------------- */
/* TpcCompresssedTocTrailer                                               */
/* ====================================================================== */



/* ---------------------------------------------------------------------- */
}  /* Namespace:: record                                                  */
}  /* Namespace:: pdd                                                     */
/* ---------------------------------------------------------------------- */

#endif
