// -*-Mode: C++;-*-

#ifndef ACCESS_TPCCOMPRESSED_HH
#define ACCESS_TPCCOMPRESSED_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcCompressed.hh
 *  @brief    Proto-Dune Data Tpc Compressed Header record and sub-record
 *            access
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
 *  <2017/07/11>
 *
 * @par Credits:
 * SLAC
 *
 * This define the acccess method for the Header record found in a 
 * in a Tpc Compressed data record.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2018.07.11 jjr Created
  
\* ---------------------------------------------------------------------- */

#include "dam/access/Headers.hh"
#include <cstdint> 
#include <cstdio>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */

namespace pdd    {
namespace record {

   class    TpcCompressedHdrHeader;
   class    TpcCompressedHdrBody;
   class    TpcCompressedHdr;

   class    TpcCompressedTocTrailer;
   class    TpcCompressedToc;

   class    TpcCompressed;
}
}
/* ====================================================================== */




namespace pdd    {
namespace access {


/* ---------------------------------------------------------------------- *//*!

  \class  TpcCompressedHdrHeader
  \brief  Class to access the fields in the Tpc Compressed Hdr record 
          header.
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcCompressedHdrHeader
{
public:
   TpcCompressedHdrHeader (pdd::record::TpcCompressedHdrHeader const *header);

public:
   pdd::record::TpcCompressedHdrHeader const *getHeader () const;
   unsigned int                         getRecordFormat () const;
   unsigned int                             getNHdrWrds () const;
   unsigned int                             getNExcWrds () const;
   uint32_t                                   getStatus () const;
   void                                           print () const;

   static unsigned int 
          getRecordFormat (pdd::record::TpcCompressedHdrHeader const *header);

   static unsigned int
          getNHdrWrds     (pdd::record::TpcCompressedHdrHeader const *header);

   static unsigned int
          getNExcWrds     (pdd::record::TpcCompressedHdrHeader const *header);

   static uint32_t
          getStatus       (pdd::record::TpcCompressedHdrHeader const *header);

   static void 
          print           (pdd::record::TpcCompressedHdrHeader const *header);

public:
   pdd::record::TpcCompressedHdrHeader const *m_header;
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \class  TpcCompressedHdrBody
  \brief  Class to access the Tpc Compressed Hdr record body
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcCompressedHdrBody
{
public:
   TpcCompressedHdrBody (pdd::record::TpcCompressedHdr const     *cmp);
   TpcCompressedHdrBody (pdd::record::TpcCompressedHdrBody const *body,
                         unsigned int                           format,
                         unsigned int                         nExcWrds,
                         unsigned int                         nHdrWrds,
                         uint32_t                               nbytes);
public:
   pdd::record::TpcCompressedHdrBody const    *getBody () const;
   uint32_t                                  getFormat () const;
   uint32_t                                  getNbytes () const;
   uint64_t                          const     *getW64 () const;
   void                                          print () const;


   uint64_t        getWib0            () const;
   uint64_t        getWibBegTimestamp () const;
   uint64_t        getWibEndTimestamp () const;
   uint64_t        getColdData00      () const;
   uint64_t        getColdData01      () const;
   uint64_t        getColdData10      () const;
   uint64_t        getColdData11      () const;

   unsigned int    getNExcWrds        () const;
   unsigned int    getNHdrWrds        () const;

   uint64_t const *locateHdrs         () const;
   uint16_t const *locateExcs         () const;

public:
   static uint64_t getWib0            (pdd::record::TpcCompressedHdrBody const *body);
   static uint64_t getWibBegTimestamp (pdd::record::TpcCompressedHdrBody const *body);
   static uint64_t getWibEndTimestamp (pdd::record::TpcCompressedHdrBody const *body);
   static uint64_t getColdData00      (pdd::record::TpcCompressedHdrBody const *body);
   static uint64_t getColdData01      (pdd::record::TpcCompressedHdrBody const *body);
   static uint64_t getColdData10      (pdd::record::TpcCompressedHdrBody const *body);
   static uint64_t getColdData11      (pdd::record::TpcCompressedHdrBody const *body);
   

   static   uint16_t const  getWibExcMask  (uint16_t exception);
   static   uint16_t const  getWibExcFrame (uint16_t exception);




private:
   pdd::record::TpcCompressedHdrBody 
                                 const    *m_body; /*!< The record body   */
   unsigned int                  const   m_format; /*!< The record format */
   uint16_t                      const *m_excWrds; /*!< Exception words   */
   uint64_t                      const *m_hdrWrds; /*!< Header words      */
   unsigned int                        m_nExcWrds; /*!< # of exceptions   */
   unsigned int                        m_nHdrWrds; /*!< # of header words */
   uint32_t                              m_nbytes; /*!< The record length */
};
/* ---------------------------------------------------------------------- */
                         

/* ---------------------------------------------------------------------- *//*!

  \class TpcCompressedHdr
  \brief Access to the fields of a TPC Compressed Header records
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcCompressedHdr
{
public:
   TpcCompressedHdr (pdd::record::TpcCompressedHdr const *hdr);


public:
   pdd::record::TpcCompressedHdr       const *getRecord     () const;
   pdd::record::TpcCompressedHdrHeader const *getHeader     () const;
   pdd::record::TpcCompressedHdrBody   const *getBody       () const;
   uint32_t                                   getStatus     () const;
   uint64_t                            const *locateHdrWrds () const;
   uint16_t                            const *locateExcWrds () const;
   void                                       print         () const;

   static pdd::record::TpcCompressedHdr       const 
                         *getRecord     (pdd::record::TpcCompressedHdr const *hdr);

   static pdd::record::TpcCompressedHdrHeader const 
                         *getHeader     (pdd::record::TpcCompressedHdr const *hdr);

   static pdd::record::TpcCompressedHdrBody   const
                         *getBody       (pdd::record::TpcCompressedHdr const *hdr);

   static uint32_t        getStatus     (pdd::record::TpcCompressedHdr const *hdr);

   static uint64_t const *locateHdrWrds (pdd::record::TpcCompressedHdr const *hdr);
   static uint16_t const *locateExcWrds (pdd::record::TpcCompressedHdr const *hdr);

   static void            print     (pdd::record::TpcCompressedHdr const *hdr);


private:
   pdd::record::TpcCompressedHdr const *m_hdr;
};
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \class TpcCompressedTocTrailer
  \brief Provides access to the field within the Tpc Compressed Table
         of Contents trailer word

  \note
   This record is unusual in that, because of the serial output nature
   of compressed data, it must be located after the compressed data itself.
   As such, it must be located at the bottom of the record, so fields
   that would normally appear in a record header, appear in this trailer.
                                                                          */
/* ---------------------------------------------------------------------- */   
class TpcCompressedTocTrailer
{
public:
   TpcCompressedTocTrailer (pdd::record::TpcCompressedTocTrailer const *tlr);

public:
   pdd::record::TpcCompressedTocTrailer const *getTrailer   () const;
   uint32_t                                    getFormat    () const;
   uint32_t                                    getType      () const;
   uint32_t                                    getN64       () const;
   uint32_t                                    getNChannels () const;
   uint32_t                                    getNSamples  () const;
   uint32_t                             const *getOffsets   () const;

public:
   static uint32_t        getFormat    (pdd::record::TpcCompressedTocTrailer const *tlr);
   static uint32_t        getType      (pdd::record::TpcCompressedTocTrailer const *tlr);
   static uint32_t        getN64       (pdd::record::TpcCompressedTocTrailer const *tlr);
   static uint32_t        getNChannels (pdd::record::TpcCompressedTocTrailer const *tlr);
   static uint32_t        getNSamples  (pdd::record::TpcCompressedTocTrailer const *tlr);
   static uint32_t const *getOffsets   (pdd::record::TpcCompressedTocTrailer const *tlr);

private:
   pdd::record::TpcCompressedTocTrailer const *m_trailer;
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Provides access to the Tpc Compressed Data Table of Contents 
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcCompressedToc
{
public:
   explicit TpcCompressedToc () { return; }
   TpcCompressedToc (pdd::record::TpcCompressedTocTrailer const *trailer);

   pdd::record::TpcCompressedToc *getRecord () const;
   unsigned int                getNChannels () const;
   unsigned int                getNSamples  () const;
   uint32_t                    getOffset    (unsigned int ichannel) const;

public:
   pdd::record::TpcCompressedTocTrailer  const *m_trailer;
   uint32_t                              const *m_offsets;
   unsigned int                               m_nchannels;
   unsigned int                                m_nsamples;
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \class TpcCompressed
  \brief Access methods to the compressed data record
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcCompressed
{
public:
   explicit TpcCompressed () { return; }
   TpcCompressed (uint64_t const *w64, uint32_t n64);

public:
   void construct (uint64_t const *w64, uint32_t n64);

   pdd::record::TpcCompressedHdr const *getHdr  () const;
   uint64_t                      const *getData () const;
   pdd::record::TpcCompressedToc const *getToc  () const;
   uint32_t                             getN64  () const;


   // Decompression into pseudo 2-D ADC array
   uint32_t decompress (int16_t       *adcs, 
                        int           nadcs, 
                        int           itick, 
                        int          nticks);

   uint32_t decompress (int16_t       *adcs, 
                        int           nadcs, 
                        int          nticks);


   // Decompression into an array of channel pointers
   uint32_t decompress (int16_t  *const *adcs, 
                        int             iadcs,
                        int             itick, 
                        int            nticks);

   uint32_t decompress (int16_t  *const *adcs,
                        int              iadc,
                        int            nticks);



private:
   pdd::record::TpcCompressedHdr        const    *m_hdr;
   uint64_t                             const    *m_w64;
   pdd::record::TpcCompressedToc        const    *m_toc;
   pdd::record::TpcCompressedTocTrailer const *m_tocTlr;
   uint32_t                                       m_n64;
};
/* ---------------------------------------------------------------------- */




/* ====================================================================== */
/* IMPLEMENTATION: TpcCompresssedHdrHeader                                */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for a Tpc Compressed Data Header record header

  \param[in] header  A pointer to the Tpc Compressed Data Header record
                     header definition
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcCompressedHdrHeader::
       TpcCompressedHdrHeader (pdd::record::TpcCompressedHdrHeader const *header) :
   m_header (header)
{
   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief   Return the definition of Tpc Compressed Data Header recrod header
  \return  The definition of Tpc Compressed Data Header recrod header
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcCompressedHdrHeader const 
      *TpcCompressedHdrHeader::getHeader () const
{
   return m_header;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief  Return the format of the header record
   \return The format of the header record
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcCompressedHdrHeader::getRecordFormat () const
{
   unsigned int format = getRecordFormat (m_header);
   return format;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief  Return the number WIB/Colddata header words that are in the
           exception block
   \return The count of 'excepted' header words.
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcCompressedHdrHeader::getNHdrWrds () const
{
   unsigned int nHdrWrds = getNHdrWrds (m_header);
   return       nHdrWrds;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return the number WIB/Colddata exception frames
  \return The count of 'exception' frames

  \par
   Each expection word contains two values, a 6-bit mask indicating 
   which of the 6 WIB/Colddata header words failed to match the predicted
   pattern and the frame the exception occurred.
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcCompressedHdrHeader::getNExcWrds () const
{
   unsigned int nExcWrds = getNExcWrds (m_header);
   return       nExcWrds;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a status value that is a bit mask of error conditions
  \return A status mask, 0 is no errors
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcCompressedHdrHeader::getStatus () const
{
   uint32_t status = getStatus (m_header);
   return   status;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Print the TPC Compressed Hdr record header
                                                                          */
/* ---------------------------------------------------------------------- */
inline void TpcCompressedHdrHeader::print () const
{
   print (m_header);
}
/* ---------------------------------------------------------------------- */
/* END : TpcCompressedHdrHeader                                           */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION: TpcCompressedHdrBody                                   */
/* ---------------------------------------------------------------------- *//*!

  \brief Construct for the body of a Tpc Compressed Header record body

  \param[in]    body  A pointer to the Tpc Compressed Header record body's
                      definition
  \param[in]   format The format of the record body
  \param[in] nExcWrds The number WIB frames with exceptions
  \param[in] nHdrWrds The number of WIB/Colddata header words that failed
                      to match the predicted value.
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcCompressedHdrBody::
       TpcCompressedHdrBody (pdd::record::TpcCompressedHdrBody const *body,
                             unsigned int                           format,
                             unsigned int                         nExcWrds,
                             unsigned int                         nHdrWrds,
                             uint32_t                               nbytes) :
          m_body     (body),
          m_format   (format),
          m_nExcWrds (nExcWrds),
          m_nHdrWrds (nHdrWrds),
          m_nbytes   (nbytes)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a pointer to the Tpc Compressed Hdr record's body definition
  \return A pointer to the Tpc Compressed Hdr record's body definition
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcCompressedHdrBody const    
                   *TpcCompressedHdrBody::getBody () const
{
   return m_body;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return the record's format
  \return The record's format
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcCompressedHdrBody::getFormat () const
{
   return m_format;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number of bytes in the record body
  \return The number of bytes in the record body
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcCompressedHdrBody::getNbytes () const
{
   return m_nbytes;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief   Returns a bare 64-bit pointer to the record body
  \return  A bare 64-bit pointer to the record body

  \par
   This is mainly used for debugging purposes
                                                                         */
/* ---------------------------------------------------------------------- */
inline uint64_t const *TpcCompressedHdrBody::getW64 () const
{
   return reinterpret_cast <uint64_t const *>(m_body);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the first WIB header word, containing the comma character,
          the WIB crate.slot.fiber, etc

  \return The first WIB header word
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TpcCompressedHdrBody::getWib0 () const 
{ 
   return getWib0 (m_body); 
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the timestamp of the first WIB frame in the packet
  \return The timestamp of the first WIB frame in the packet
                                                                          */
/* ---------------------------------------------------------------------- */

inline uint64_t TpcCompressedHdrBody::getWibBegTimestamp () const
{ 
   return getWibBegTimestamp (m_body); 
}
/* ---------------------------------------------------------------------- */
   

/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the timestamp of the last WIB frame in the packet
  \return The timestamp of the last WIB frame in the packet

  \note
   To get the ending time of this packet, one must add on the clock
   period.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TpcCompressedHdrBody::getWibEndTimestamp () const
{ 
   return getWibEndTimestamp (m_body); 
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the first header word of the first Colddata link
  \return The first header word of the first Colddata link
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TpcCompressedHdrBody::getColdData00 () const 
{ 
   return getColdData00 (m_body); 
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the first header word of the first Colddata link
  \return The first header word of the first Colddata link
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TpcCompressedHdrBody::getColdData01 () const 
{ 
   return getColdData01 (m_body); 
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the first header word of the second Colddata link
  \return The first header word of the second Colddata link
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TpcCompressedHdrBody::getColdData10 () const
{
   return getColdData10 (m_body); 
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the second header word of the second Colddata link
  \return The second header word of the second Colddata link
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TpcCompressedHdrBody::getColdData11 () const 
{ 
   return getColdData11 (m_body); 
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number of WIB/Colddata frame exception words
  \return The number of WIB/Colddata frame exception words
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcCompressedHdrBody::getNExcWrds () const
{
   return m_nExcWrds;
}


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number of WIB/Colddata header exception words
  \return The number of WIB/Colddata header exception words
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcCompressedHdrBody::getNHdrWrds () const
{
   return m_nHdrWrds;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Returns a pointer to the WIB frame exception headerwords
  \return  A pointer to the WIB frame exception headerwords
                                                                          */
/* ---------------------------------------------------------------------- */ 
inline uint64_t const *TpcCompressedHdrBody::locateHdrs () const
{
   return m_hdrWrds;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Returns a pointer to the WIB frame exception words
  \return  A pointer to the WIB frame exception words
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint16_t const *TpcCompressedHdrBody::locateExcs () const
{
   return m_excWrds;
}
/* ---------------------------------------------------------------------- */
/* END : TpcCompressedHdrBody                                             */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION: pdd::access::TpcCompresedHdr                           */
/* ---------------------------------------------------------------------- *//*!

  \brief Construct for the TPC Compressed Header acesssor
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcCompressedHdr::
       TpcCompressedHdr (pdd::record::TpcCompressedHdr const *hdr) :
   m_hdr (hdr)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the TPC Compressed Header record
  \return A pointer to the TPC Compressed Header record
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcCompressedHdr const *TpcCompressedHdr::getRecord () const
{
   return m_hdr;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the TPC Compressed Header record header
  \return A pointer to the TPC Compressed Header record header
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcCompressedHdrHeader const 
      *TpcCompressedHdr::getHeader () const
{
   pdd::record::TpcCompressedHdrHeader const *header = getHeader (m_hdr);
   return header;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the TPC Compressed Header record body
  \return A pointer to the TPC Compressed Header record body
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcCompressedHdrBody   const 
      *TpcCompressedHdr::getBody () const
{
   pdd::record::TpcCompressedHdrBody const *body = getBody (m_hdr);
   return body;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the TPC Compressed summary status
  \return A pointer to the TPC Compressed summary status
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcCompressedHdr::getStatus () const
{
   uint32_t status = getStatus (m_hdr);
   return status;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief  Returns a pointer to the 'excepted' WIB/ColdData header words
   \return A pointer to the 'excepted' WIB/ColdData header words
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t const *TpcCompressedHdr::locateHdrWrds () const
{
   uint64_t const *hdrWrds = locateHdrWrds (m_hdr);
   return          hdrWrds;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief  Returns a pointer to the WIB/ColdData exception frames
   \return A pointer to the WIB/ColdData exception frames
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint16_t const *TpcCompressedHdr::locateExcWrds () const
{
   uint16_t const *excWrds = locateExcWrds (m_hdr);
   return          excWrds;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Prints the TPC Compressed record
                                                                          */
/* ---------------------------------------------------------------------- */
inline void TpcCompressedHdr::print () const
{
   print (m_hdr);
}
/* ---------------------------------------------------------------------- */
/* pdd::access::TpcCompresedHdr                                           */
/* ====================================================================== */




/* ====================================================================== */
/* IMPPLEMENTATION::TpcCompressedTocTrailer                               */
/* ---------------------------------------------------------------------- *//*!

  \brief  Constructor for the Tpc Compressed Table of contents record's 
          trailer word
                                                                          */
/* ---------------------------------------------------------------------- */
inline 
TpcCompressedTocTrailer::
TpcCompressedTocTrailer (pdd::record::TpcCompressedTocTrailer const *trailer) :
   m_trailer (trailer)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return the Tpc Compressed Table of Contents Trailer
  \return The trailer word

  \note
   The TpcCompressedToc record is read from the end. Therefore the values
   that would normally reside in a record header actually reside in the
   trailer.
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcCompressedTocTrailer const
                   *TpcCompressedTocTrailer::getTrailer () const
{
   return m_trailer;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the format of the trailer
  \return The trailer format
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcCompressedTocTrailer::getFormat () const
{
   uint32_t fmt = getFormat (m_trailer);
   return   fmt;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Returns the record type
  \return  The record type


  \note
   By definition this is TpcCompressed::Toc.  This routine is provided
   for completeness.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcCompressedTocTrailer::getType () const
{
   uint32_t type = getType (m_trailer);
   return   type;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief  Return the length, in units of 64-bit words of the record
   \return The length, in units of 64-bit words of the record
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcCompressedTocTrailer::getN64 () const
{
   uint32_t n64 = getN64 (m_trailer);
   return   n64;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number of channels contained in the compressed data
  \return The number of channels contained in the compressed data

  \note
   This will almost always be 128, but if there are dead channels it may
   be less.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcCompressedTocTrailer::getNChannels () const
{
   uint32_t nchannels = getNChannels (m_trailer);
   return   nchannels;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Returns the number of ADC time samples in the compressed data
  \return  The number of ADC time samples in the compressed data

                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t  TpcCompressedTocTrailer::getNSamples () const
{
   uint32_t nsamples = getNSamples (m_trailer);
   return   nsamples;
}
/* ---------------------------------------------------------------------- */
/* pdd::access::TpcCompressedTocTrailer                                   */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION: TpcCompressed                                          */
/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the WIB/Colddata definition record
  \return A pointer to the WIB/Colddata definition record
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcCompressedHdr const *TpcCompressed::getHdr () const
{
   return m_hdr;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the compressed data bits
  \return A pointer to the compressed data bits
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t const *TpcCompressed::getData () const
{
   return m_w64;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the Tpc Compressed Table of Contents 
  \return A pointer to the Tpc Compressed Table of Contents 
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcCompressedToc const *TpcCompressed::getToc () const
{
   return m_toc;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return the length, in units of 64-bit words of the Tpc Compressed
          data record.
  \return The length, in units of 64-bit words of the Tpc Compressed
          data record.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcCompressed::getN64 () const
{
   return m_n64;
}
/* ---------------------------------------------------------------------- */
/* pdd::accdss::TpcCompressed                                             */
/* ====================================================================== */


/* ---------------------------------------------------------------------- */
}  /* Namespace:: access                                                  */
}  /* Namespace:: pdd                                                     */
/* ---------------------------------------------------------------------- */

#endif

