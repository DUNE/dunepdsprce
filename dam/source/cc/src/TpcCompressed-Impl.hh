// -*-Mode: C++;-*-

#ifndef TPCCOMPRESSED_IMPL_HH
#define TPCCOMPRESSED_IMPL_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcCompressedHeder-Impl.hh
 *  @brief    Proto-Dune Data Tpc Compressed Header Record, implementation
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
 *  pdd
 *
 *  @author
 *  <russell@slac.stanford.edu>
 *
 *  @par Date created:
 *  <2018/07/11>
 *
 * @par Credits:
 * SLAC
 *
 * @par
 * This laysout the format and primitive access methods to the data
 * found in the header record of a TpcCompressed Data Packet, that
 * for performance reasons, are inline'd for internal use and made 
 * external for external use.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2018.07.11 jjr Created

\* ---------------------------------------------------------------------- */


#include  "dam/access/TpcCompressed.hh"
#include "dam/records/TpcCompressed.hh"


/// !!! KLUDGE !!! these should be defined in the record namespace
///                need to get consistency, for example, with TpcStream
namespace pdd    {
namespace access {

/* ---------------------------------------------------------------------- *//*!

  \namespae tcpstream_bridge
  \brief    Defines the layout of TcpStreamHeader bridge 
                                                                          */
/* ---------------------------------------------------------------------- */
namespace tpccompressedtoctrailer
{
   /* -------------------------------------------------------------------- *//*!

     \brief Size of the bit fields of the trailer word
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Size
   {
      Format    =  4,  /*!< Size of the format field                      */
      Type      =  4,  /*!< Size of the record type field                 */
      N64       = 16,  /*!< Size of the record length field               */
      RecFormat =  4,  /*!< Size of the record format field               */
      NSamples  = 12,  /*!< Size of the # of samples  - 1 field           */
      NChannels = 12,  /*!< Size of the # of channels - 1 field           */
      Reserved  = 12   /*!< Size of the reserved field                    */
   };
   /* ------------------------------------------------------------------- */


   /* ---------------------------------------------------------------- *//*!

      \brief Right justified offsets of the bit fields of the bridge
             word
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Offset
   {
      Format    =  0, /*!< Offset to the trailer format field             */
      Type      =  4, /*!< Offset to the record type field                */
      N64       =  8, /*!< Offset to the record length field              */
      RecFormat = 24, /*!< Offset to the record format field              */
      NSamples  = 28, /*!< Offset to the # of samples  - 1 field          */
      NChannels = 40, /*!< Offset to the # of channels - 1 field          */
      Reserved  = 52  /*!< Offset to the reserved field                   */
   };
   /* ------------------------------------------------------------------- */


   /* ------------------------------------------------------------------- *//*!

     \brief Right justified masks of the bit fields of the bridge word
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class Mask : uint32_t
   {
      Format    = 0x0000000f,
      Type      = 0x0000000f,
      N64       = 0x00000fff,
      RecFormat = 0x0000000f,
      NSamples  = 0x00000fff,
      NChannels = 0x00000fff,
      Reserved  = 0x00000fff
    };
   /* ------------------------------------------------------------------- */
}
/* ---------------------------------------------------------------------- */
} /* Namespace: access                                                    */
} /* Namespace: pdd                                                       */
/* ---------------------------------------------------------------------- */




namespace pdd    {
namespace access {


// --------------------------------
// Default implementation to inline
// --------------------------------
#ifndef TPCCOMPRESSED_IMPL
#define TPCCOMPRESSED_IMPL inline
#endif


   


/* ====================================================================== */
/* IMPLEMENTATION: pdd::access::TpcCompresedHdrHeader                     */
/* ---------------------------------------------------------------------- *//*!
  \brief  Return the record's format
  \return The record's format

  \param[in] header  Pointer to the Tpc Compressed Hdr record header
                     definition
                                                                          */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL unsigned int TpcCompressedHdrHeader::
    getRecordFormat (pdd::record::TpcCompressedHdrHeader const *header)
{
   ///!!! KLUDGE till properly define the record header
   return 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number 'excepted' header words
  \return The number 'excepted' header words
                                                                          */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL unsigned int TpcCompressedHdrHeader::
   getNHdrWrds (pdd::record::TpcCompressedHdrHeader const *header)
{
   /// !!! KLUDGE !!! -- need proper record header definition
   unsigned int      n64 = header->getN64 ();
   unsigned int nExcWrds = getNExcWrds (header);
   unsigned int nHdrWrds = n64 - nExcWrds - 8;
   return nHdrWrds;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number 'excepted' WIB frames
  \return The number 'excepted' WIB frames words
                                                                          */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL unsigned int TpcCompressedHdrHeader::
   getNExcWrds (pdd::record::TpcCompressedHdrHeader const *header)
{
   uint64_t bridge = header->getBridge ();
   unsigned int nExcWrds = 
     PDD_EXTRACT64 (bridge,
                    pdd::record::TpcCompressedHdrHeader::Bridge::Mask  ::ExcCount,
                    pdd::record::TpcCompressedHdrHeader::Bridge::Offset::ExcCount);
   return nExcWrds;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the firmware read status
  \return The firmware read status
                                                                          */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL unsigned int TpcCompressedHdrHeader::
   getStatus (pdd::record::TpcCompressedHdrHeader const *header)
{
   uint64_t bridge = header->getBridge ();
   uint32_t status =      PDD_EXTRACT64 (bridge,
                    pdd::record::TpcCompressedHdr::Bridge::Mask  ::Status,
                    pdd::record::TpcCompressedHdr::Bridge::Offset::Status);
   return status;
}
/* ---------------------------------------------------------------------- */
/* END: pdd::access::TpcCompresedHdrHeader                                */
/* ====================================================================== */



/* ====================================================================== */
/* IMPLEMENTATION: pdd::access::TpcCompresedHdrBody                       */
/* ---------------------------------------------------------------------- *//*!

  \brief  Constructor for the Tpc Compressed Hdr body accessor

  \param[in]  rec Pointer to  Tpc Compressed Hdr Record 
                                                                          */
/* ---------------------------------------------------------------------- */  
TPCCOMPRESSED_IMPL TpcCompressedHdrBody::
TpcCompressedHdrBody (pdd::record::TpcCompressedHdr const *rec) :
   m_body     (&rec->m_body),
   m_format   (pdd::access::TpcCompressedHdrHeader::getRecordFormat (rec)),
   m_excWrds  (pdd::access::TpcCompressedHdr      ::locateExcWrds   (rec)),
   m_hdrWrds  (pdd::access::TpcCompressedHdr      ::locateHdrWrds   (rec)),
   m_nExcWrds (pdd::access::TpcCompressedHdrHeader::getNExcWrds     (rec)),
   m_nHdrWrds (pdd::access::TpcCompressedHdrHeader::getNHdrWrds     (rec)),
   m_nbytes   (rec->getNbytes ())
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Return the WIB frame's first header word.  This has the comma
         character, WIB Crate.Slot.Fiber etc.
                                                                          */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL uint64_t TpcCompressedHdrBody::
getWib0 (pdd::record::TpcCompressedHdrBody const *body)
{
   return body->m_w64[0];
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return the packet's beginning timestamp
  \return The packet's beginning timestamp
                                                                          */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL uint64_t TpcCompressedHdrBody::
getWibBegTimestamp (pdd::record::TpcCompressedHdrBody const *body)
{
   return body->m_w64[1];
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return the packet's ending timestamp
  \return The packet's beginning timestamp

  \note 
   To get the ending time one must add on the clock period
                                                                          */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL uint64_t TpcCompressedHdrBody::
getWibEndTimestamp (pdd::record::TpcCompressedHdrBody const *body)
{
   return body->m_w64[2];
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return the first ColdData stream's first header word
  \return The first ColdData stream's first header word
                                                                          */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL uint64_t TpcCompressedHdrBody::
getColdData00      (pdd::record::TpcCompressedHdrBody const *body)
{
   return body->m_w64[3];
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return the first ColdData stream's second header word
  \return The first ColdData stream's second header word
                                                                          */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL uint64_t TpcCompressedHdrBody::
getColdData01      (pdd::record::TpcCompressedHdrBody const *body)
{
   return body->m_w64[4];
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return the second ColdData stream's first header word
  \return The second ColdData stream's first header word
                                                                          */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL uint64_t TpcCompressedHdrBody::
getColdData10      (pdd::record::TpcCompressedHdrBody const *body)
{
   return body->m_w64[5];
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return the second ColdData stream's second header word
  \return The second ColdData stream's second header word
                                                                          */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL uint64_t TpcCompressedHdrBody::
getColdData11      (pdd::record::TpcCompressedHdrBody const *body)
{
   return body->m_w64[6];
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Extracts the exception mask from the exception word
  \return The extracted exception mask

  \par
   This is a 6-bit mask indicating which of the WIB/ColdData header words
   did not match their prediction.
                                                                          */
/* ---------------------------------------------------------------------- */   
TPCCOMPRESSED_IMPL uint16_t const  TpcCompressedHdrBody::
getWibExcMask  (uint16_t exception)
{
   return (exception >> 10) & 0x3f;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Extracts the exception frame number
  \return The extracted exception frame number

  \par
   This is a 10-bit value indicating the frame containing the excepting
   header word(s).
                                                                          */
/* ---------------------------------------------------------------------- */   
TPCCOMPRESSED_IMPL uint16_t const  TpcCompressedHdrBody::
getWibExcFrame (uint16_t exception)
{
   return (exception >>  0) & 0x3ff;
}
/* ---------------------------------------------------------------------- */
/* END: pdd::access::TpcCompresedHdrHeader                                */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION: pdd::access::TpcCompresedHdr                           */
/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the TPC Compressed Hdr record header
  \return The TPC Compressed Hdr record header

  \param[in] hdr  The TPC Compressed Header record
                                                                          */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL pdd::record::TpcCompressedHdrHeader const *
TpcCompressedHdr::getHeader (pdd::record::TpcCompressedHdr const *hdr)
{
   return hdr;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the TPC Compressed Hdr record body
  \return The TPC Compressed Hdr record header body

  \param[in] hdr  The TPC Compressed Header record
                                                                          */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL pdd::record::TpcCompressedHdrBody const *
TpcCompressedHdr::getBody (pdd::record::TpcCompressedHdr const *hdr)
{
   return &hdr->m_body;
}
/* ---------------------------------------------------------------------- */




TPCCOMPRESSED_IMPL uint16_t const *
TpcCompressedHdr::locateExcWrds (pdd::record::TpcCompressedHdr const *rec)
{
   uint16_t const *excWrds = 
           reinterpret_cast<decltype(excWrds)>(rec->m_body.m_w64 + 8);
   return excWrds;
}


TPCCOMPRESSED_IMPL uint64_t const *
TpcCompressedHdr::locateHdrWrds  (pdd::record::TpcCompressedHdr const *rec)
{
   unsigned int nExcWrds = TpcCompressedHdrHeader::getNExcWrds (rec);
   uint64_t const *hdrWrds = rec->m_body.m_w64 + nExcWrds + 7;
   return hdrWrds;
}
/* ---------------------------------------------------------------------- */
/* TpcCompressedHdr                                                       */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION::TpcCompressedTocTrailer                                */
/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the format of the trailer
  \return The trailer format

   \param[in] tlr  The record's trailer word
                                                                          */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL uint32_t
TpcCompressedTocTrailer::getFormat (pdd::record::TpcCompressedTocTrailer const *tlr)
{
   using namespace tpccompressedtoctrailer;

   uint32_t fmt = PDD_EXTRACT64 (tlr->m_w64,
                                 Mask  ::Format,
                                 Offset::Format);
   return   fmt;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Returns the record type
  \return  The record type

   \param[in] tlr  The record's trailer word

  \note
   By definition this is TocCompressed::Toc.  This routine is provided
   for completeness.
                                                                          */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL uint32_t 
TpcCompressedTocTrailer::getType (pdd::record::TpcCompressedTocTrailer const *tlr)
{
   using namespace tpccompressedtoctrailer;

   uint32_t type = PDD_EXTRACT64 (tlr->m_w64,
                                  Mask  ::Type,
                                  Offset::Type);
   return   type;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief  Return the length, in units of 64-bit words of the record
   \return The length, in units of 64-bit words of the record

   \param[in] tlr  The record's trailer word
                                                                          */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL uint32_t 
TpcCompressedTocTrailer::getN64 (pdd::record::TpcCompressedTocTrailer const *tlr) 
{
   using namespace tpccompressedtoctrailer;

   uint32_t n64 = PDD_EXTRACT64 (tlr->m_w64,
                                 Mask  ::N64,
                                 Offset::N64);
   return   n64;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number of channels contained in the compressed data
  \return The number of channels contained in the compressed data

  \param[in] tlr  The record's trailer word

  \note
   This will almost always be 128, but if there are dead channels it may
   be less.
                                                                          */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL uint32_t 
TpcCompressedTocTrailer::getNChannels (pdd::record::TpcCompressedTocTrailer const *tlr)
{
   using namespace tpccompressedtoctrailer;

   uint32_t nchannels = PDD_EXTRACT64 (tlr->m_w64, 
                                       Mask  ::NChannels, 
                                       Offset::NChannels) + 1;
   return   nchannels;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Returns the number of ADC time samples in the compressed data
  \return  The number of ADC time samples in the compressed data

  \param[in] tlr  The record's trailer word
                                                                          */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL uint32_t
TpcCompressedTocTrailer::getNSamples (pdd::record::TpcCompressedTocTrailer const *tlr)
{
   using namespace tpccompressedtoctrailer;

   uint32_t nsamples = PDD_EXTRACT64 (tlr->m_w64, 
                                      Mask  ::NSamples,
                                      Offset::NSamples) + 1;
   return   nsamples;
}


TPCCOMPRESSED_IMPL uint32_t const *
TpcCompressedTocTrailer::getOffsets (pdd::record::TpcCompressedTocTrailer const *tlr)
{
   uint32_t            n64 = getN64 (tlr);
   uint64_t const     *w64 = reinterpret_cast<decltype(w64)>(tlr);
   uint32_t const *offsets = reinterpret_cast<decltype(offsets)>(w64 - n64 + 1);

   return offsets;
}
/* ---------------------------------------------------------------------- */
/* TpcCompressedTocTrailer                                                */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION:TpcCompressedToc                                        */
/* ---------------------------------------------------------------------- */
TPCCOMPRESSED_IMPL
TpcCompressedToc::TpcCompressedToc (pdd::record::TpcCompressedTocTrailer const *trailer) 
{
   m_trailer = trailer;
   pdd::access::TpcCompressedTocTrailer tlr (trailer);

   m_nchannels = tlr.getNChannels ();
   m_nsamples  = tlr.getNSamples  ();
   m_offsets   = tlr.getOffsets   ();

   return;
}
/* ---------------------------------------------------------------------- */
}  /* Namespace:: access                                                  */
}  /* Namespace:: pdd                                                     */
/* ---------------------------------------------------------------------- */



#endif
