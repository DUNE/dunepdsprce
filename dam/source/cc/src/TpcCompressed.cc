// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file    TpcCompressed.cc
 *  @brief    Proto-Dune Data Tpc Compressed record and sub-records
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
 *  <2018/07/12>
 *
 * @par Credits:
 * SLAC
 *
 * Implement access methods to the the Tpc Compressed record and subrecords
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2018.07.11 jjr Created
  
\* ---------------------------------------------------------------------- */

#define   TPCCOMPRESSE_IMPL extern


#include "TpcCompressed-Impl.hh"
#include "BFU.h"
#include  <cstdio>
#include  <iostream>
#include  <iomanip>



namespace pdd    {
namespace record {

/* ---------------------------------------------------------------------- *//*!

  \class TpcCompressed
  \brief The TPC Compressed record.  

   This may include any of the following records

     -# List of WIB/Colddata headers
        this provides both the seed values for these headers and, potentially
        a list of exceptions where these headers the known sequencing and
        patterns.
    -#  Data Record
        This is the compressed data.  It presented as a bit stream coded
        in 64-bit words
    -#  The Table of Contents
        This allows randow access within the above bit stream to individual
        channels.  It an also serve as a constraint on the decoding in the
        sense that the decoding of a channel must consume a well defined
        number of bits.  
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcCompressed 
{
private:
   TpcCompressed () = delete;

public:
   uint64_t const *getRecord () const { return reinterpret_cast<uint64_t const *>(this); }

public:
   // ----------------
   // TPC Record Types
   // ---------------
   enum class RecType
   {
      Reserved   = 0,   /*!< Reserved                                     */
      Header     = 1,   /*!< The record of WIB/ColdData headers           */
      Toc        = 2    /*!< Table of Contents record                     */
   };

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
}  /* Namespace:: record                                                  */
/* ---------------------------------------------------------------------- */
}  /* Namespace:: pdd                                                     */
/* ====================================================================== */



namespace pdd    {
namespace access {

static inline void announce (int ichan, int next, int position) 
                                        __attribute__((unused));

static inline void hist_integrate (uint16_t *table, uint16_t *bins, int nbins)
                                                     __attribute__ ((unused));

/* ---------------------------------------------------------------------- *//*!

  \brief Internal debugging

  \param[in]    ichan  The channel number
  \param[in]     next  The next predicted decoding bit position
  \param[in] position  The actual bit position
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void announce (int ichan, int next, int position)
{
   printf (
   "Decompress[%3.3x]: positions predicted: got %8.8" PRIx32 " : %8.8" PRIx32 "\n",
   ichan, next, position);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Constructs the accessor for a TpcCompressed Data record

  \param[in] w64  The start of the record
  \param[in] n64  The length, in units of 64-bit words, of the record

   The TpcCompressed data record consists of a number of subrecords. 
   Currently they most appear in a prescribed order
                                                                          */
/* ---------------------------------------------------------------------- */
TpcCompressed::TpcCompressed (uint64_t const *w64,  uint32_t n64)
{
   construct (w64, n64);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief This is the delegated constructor for a TPC Compressed data record.

  \param[in] w64  The start of the record
  \param[in] n64  The length, in units of 64-bit words, of the record
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcCompressed::construct (uint64_t const *w64,  uint32_t n64)
{
   m_hdr = reinterpret_cast<decltype(m_hdr)>(w64);

   uint32_t           nhdr = m_hdr->getN64 ();
   uint64_t const  *tocTlr = w64 + n64 - 1;
   uint32_t         tocLen = (*tocTlr >> 8) & 0xffff;

   m_w64    = w64 + nhdr;
   m_toc    = reinterpret_cast<decltype(m_toc)   >(tocTlr - tocLen + 1);
   m_tocTlr = reinterpret_cast<decltype(m_tocTlr)>(tocTlr);
   m_n64    = n64;

   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Print the specified TOC record header

  \param[in] hdr  Pointer to the TOC record header to print
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcCompressedHdrHeader::print (pdd::record::TpcCompressedHdrHeader const *hdr)
{
   uint64_t w64     = hdr->retrieve        ();
   unsigned hdrFmt  = hdr->getFormat       (w64);
   unsigned typ     = hdr->getType         (w64);
   unsigned size    = hdr->getN64          (w64);
   unsigned cmpFmt  = pdd::access::TpcCompressedHdrHeader::getRecordFormat (hdr);

   printf ("Compressed Hdr:"
           "Format.Type.TocFmt = %1.1x.%1.1x.%1.1x size = %4.4x %16.16" PRIx64 "\n",
           hdrFmt,
           typ,
           cmpFmt,
           size,
           w64);


   return;
}
/* ---------------------------------------------------------------------- */


static int  chan_decode (int16_t       *adcs,
                         uint64_t const *buf, 
                         int            nbuf,
                         int        position,
                         int         begTick,
                         int         endTick,
                         int        nsamples,
                         bool        printit);

static int table_decode (uint16_t      *bins,
                         int         *nrbins,
                         int          *first,
                         int        *novrflw,
                         int        nsamples,
                         BFU            &bfu,
                         uint64_t const *buf,
                         bool        printit);

static int  adcs_decode (int16_t         *adcs,
                         BFU               bfu,
                         uint64_t const   *buf,
                         uint16_t const *table,
                         int           begTick,
                         int            nTicks,
                         int16_t           adc,
                         int            ovrpos,
                         int             nsyms,
                         int           novrflw,
                         bool          printit);

static int16_t    restore (uint16_t       sym);
static void print_decoded (uint16_t       sym, 
                           int            idy);


///static int    Value = 0;
///static int BegValue = 0;


/* ---------------------------------------------------------------------- *//*!

   \brief  Decompress into a pseudo 2-D array of ADCs
   \retval == true if successful
   \retval == false if not

   \param[out]   adcs The array to hold the decompressed ADCs
   \param [in]  nadcs The number of elements to reserve for each channel,
                      This is essentially the stride.
   \param[in]  nticks The maximum number of ADCs to decode.
                                                                          */
/* ---------------------------------------------------------------------- */
uint32_t TpcCompressed::decompress (int16_t       *adcs, 
                                    int           nadcs,
                                    int          nticks)
{

   int           nchannels = TpcCompressedTocTrailer::getNChannels (m_tocTlr);
   int            nsamples = TpcCompressedTocTrailer::getNSamples  (m_tocTlr);
   uint32_t const *offsets = TpcCompressedTocTrailer::getOffsets   (m_tocTlr);
   unsigned int        n64 = m_n64;
   uint64_t const     *buf = reinterpret_cast<decltype(buf)>(m_hdr);
   uint32_t           next = (m_w64 - buf) * 64;
   int             endTick = nticks;
   bool            printit = false;

   for (int ichan = 0; ichan < nchannels; ichan++)
   {
      uint32_t position = offsets[ichan]; 

      ///Value = BegValue;
      if (printit) announce (ichan, next, position);

      next  = chan_decode (adcs, buf, n64, position, 0, endTick, nsamples, printit);
      adcs += nadcs;
   }

   ///BegValue = Value;
   int over     = nsamples - nticks;
   if (over >= 0) nsamples -= over;
   return nsamples;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief  Decompress into a pseudo 2-D array of ADCs
   \retval == true if successful
   \retval == false if not

   \param[out]   adcs The array to hold the decompressed ADCs
   \param [in]  nadcs The number of elements to reserve for each channel,
                      This is essentially the stride.
   \param[in] begTick The index of the first decoded ADC to store
   \param[in]  nticks The maximum number of ADCs to decode.
                                                                          */
/* ---------------------------------------------------------------------- */
uint32_t TpcCompressed::decompress (int16_t       *adcs, 
                                    int           nadcs,
                                    int         begTick,
                                    int          nticks)
{

   int           nchannels = TpcCompressedTocTrailer::getNChannels (m_tocTlr);
   int            nsamples = TpcCompressedTocTrailer::getNSamples  (m_tocTlr);
   uint32_t const *offsets = TpcCompressedTocTrailer::getOffsets   (m_tocTlr);
   unsigned int        n64 = m_n64;
   uint64_t const     *buf = reinterpret_cast<decltype(buf)>(m_hdr);
   uint32_t           next = (m_w64 - buf) * 64;
   int             endTick = begTick + nticks;
   bool            printit = false;

   for (int ichan = 0; ichan < nchannels; ichan++)
   {
      uint32_t position = offsets[ichan]; 

      if (printit) announce (ichan, next, position);

      ///Value = BegValue;
      next  = chan_decode (adcs, buf, n64, position, begTick, endTick, nsamples, printit);
      adcs += nadcs;
   }

   ///BegValue = Value;
   nsamples    -= begTick;
   int over     = nsamples - nticks;
   if (over >= 0) nsamples -= over;
   return nsamples;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!


   \brief  Decompress into an array channel specific pointers to each array ADCs
   \retval == true if successful
   \retval == false if not

   \param[out]   adcs The array pointers to each channels ADC array
   \param [in]   iadc The index to store the first adc
   \param[in]  nticks The maximum number of ADCs to decode.
                                                                          */
/* ---------------------------------------------------------------------- */
uint32_t TpcCompressed::decompress (int16_t  *const *adcs, 
                                    int              iadc,
                                    int            nticks)
{

   int           nchannels = TpcCompressedTocTrailer::getNChannels (m_tocTlr);
   int            nsamples = TpcCompressedTocTrailer::getNSamples  (m_tocTlr);
   uint32_t const *offsets = TpcCompressedTocTrailer::getOffsets   (m_tocTlr);
   unsigned int        n64 = m_n64;
   uint64_t const     *buf = reinterpret_cast<decltype(buf)>(m_hdr);
   uint32_t           next = (m_w64 - buf) * 64;
   int             endTick = nticks;
   bool            printit = false;

   for (int ichan = 0; ichan < nchannels; ichan++)
   {
      uint32_t position = offsets[ichan]; 

      ///Value = BegValue;
      if (printit) announce (ichan, next, position);

      next  = chan_decode (adcs[ichan]+iadc, buf, n64, position, 
                           0,       endTick, nsamples, printit);
   }

   ///BegValue = Value;
   int over     = nsamples - nticks;
   if (over >= 0) nsamples -= over;
   return nsamples;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief  Decompress into an array channel specific pointers to each array ADCs
   \retval == true if successful
   \retval == false if not

   \param[out]   adcs The array pointers to each channels ADC array
   \param [in]   iadc The index to store the first adc
   \param[in] begTick The index of the first decoded ADC to store
   \param[in]  nticks The maximum number of ADCs to decode.
                                                                          */
/* ---------------------------------------------------------------------- */
uint32_t TpcCompressed::decompress (int16_t  *const *adcs, 
                                    int              iadc,
                                    int           begTick,
                                    int            nticks)
{

   int           nchannels = TpcCompressedTocTrailer::getNChannels (m_tocTlr);
   int            nsamples = TpcCompressedTocTrailer::getNSamples  (m_tocTlr);
   uint32_t const *offsets = TpcCompressedTocTrailer::getOffsets   (m_tocTlr);
   unsigned int        n64 = m_n64;
   uint64_t const     *buf = reinterpret_cast<decltype(buf)>(m_hdr);
   uint32_t           next = (m_w64 - buf) * 64;
   int             endTick = begTick + nticks;
   bool            printit = false;

   for (int ichan = 0; ichan < nchannels; ichan++)
   {
      uint32_t position = offsets[ichan]; 

      if (printit) announce (ichan, next, position);

      ///Value = BegValue;
      next  = chan_decode (adcs[ichan] + iadc, buf, n64, position, 
                           begTick,   endTick, nsamples, printit);
   }

   ///BegValue = Value;
   nsamples    -= begTick;
   int over     = nsamples - nticks;
   if (over >= 0) nsamples -= over;
   return nsamples;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
static int chan_decode (int16_t       *adcs,
                        uint64_t const *buf, 
                        int            nbuf,
                        int        position,
                        int         begTick,
                        int         endTick,
                        int        nsamples,
                        bool        printit)
{
   BFU bfu;
   _bfu_put (bfu, buf[position>>6], position);


   int          nbins;
   int            adc;
   int        novrflw;
   uint16_t table[128+2];
   int ovrpos = table_decode (table, &nbins, &adc, &novrflw, 
                              nsamples, bfu,  buf,  printit);
   position   =  adcs_decode (adcs, bfu, buf, table, begTick, endTick, adc,
                              ovrpos, nsamples, novrflw, printit);

   return position;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static inline int table_decode (uint16_t     *table,
                                int         *nrbins,
                                int          *first,
                                int        *novrflw,
                                int        nsamples,
                                BFU            &bfu,
                                uint64_t const *buf,
                                bool        printit)
{
   int format __attribute__ ((unused));
   int left      = nsamples - 1;
   int position = _bfu_get_pos  (bfu);
       format   = _bfu_extractR (bfu, buf, position,  4);
   int nbins    = _bfu_extractR (bfu, buf, position,  8) + 1;
   int mbits    = _bfu_extractR (bfu, buf, position,  4);
   int nbits    = mbits;


   // Return decoding context
  *first        = _bfu_extractR (bfu, buf, position, 12);
  *novrflw      = _bfu_extractR (bfu, buf, position,  4);
  *nrbins       = nbins;

  table[0]       = nbins;
  uint16_t total = 0;
  table[1]       = 0; 

   // Extract the histogram bins
   for (int ibin = 0; ibin < nbins; ibin++)
   {
      // Extract the bits
      int cnts   = left ? _bfu_extractR (bfu, buf, position, nbits) : 0;

      total        +=   cnts;
      table[ibin+2] =  total;

      if (printit)
      {
         if ( (ibin & 0xf) == 0x0) 
         {
            std::cout << "Hist[" << std::hex << std::setw(2) << ibin << "] ";
         }

         std::cout <<  ' ' << std::hex << std::setw(3) << cnts
                   << ":"  << std::hex << std::setw(1) << nbits;
         if ( (ibin & 0xf) == 0xf) std::cout << std::endl;
      }

      left -= cnts;
      ///if (left == 0) break;

      nbits = 32 - __builtin_clz (left);
      if (nbits > mbits) nbits = mbits;
   }

   return position;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static inline void hist_integrate (uint16_t *table, uint16_t *bins, int nbins)
{
   uint16_t total  = 0;
   table[0]        = nbins;

   for (int idx = 0; idx < nbins; idx++)
   {
      table[idx+1] = total;
      total       += bins[idx];
   }

   table[nbins + 1] = total;

   return;
}
/* ---------------------------------------------------------------------- */



#include "AP-Decode.h"



/* ---------------------------------------------------------------------- */
static int inline adcs_decode (int16_t         *adcs,
                               BFU               bfu,
                               uint64_t const   *buf,
                               uint16_t const *table,
                               int           begTick,
                               int           endTick,
                               int16_t           adc,
                               int            ovrpos,
                               int          nsymbols,
                               int           novrflw,
                               bool          printit)
{
   APD_dtx dtx;

   int nbins  = table[0];  // Total number of bins
   int novr   = table[2];  // Number of overflows


   // Compute the number of bits in the overflow array
   int nobits   = novrflw *   novr;
   int sympos   = ovrpos +   nobits;
   int sym      = adc;

   APD_start (&dtx, buf, sympos);

   for (int idy = 0; ; idy++)
   {

      if ( (idy >= begTick) && (idy < endTick) ) 
      {
         *adcs++ = adc; /// Value++;
      }
      if (printit)
      {
         print_decoded (sym, idy);
      }

      if (idy == nsymbols - 1)
      {
         break;
      }

      sym = APD_decode (&dtx, table);
      if (sym == 0)
      {
         // Have overflow
         int ovr = novrflw ? _bfu_extractR (bfu, buf, ovrpos, novrflw) : 0;
         sym     =  nbins + ovr;
      }

      adc  += restore (sym); 
   }


   // Number of bits consumed by the decoding
   sympos = APD_finish (&dtx);

   // Total number of decoded symbol bits
   int totbits = sympos + ovrpos;
   return totbits;
}
/* ---------------------------------------------------------------------- */



static inline int16_t restore (uint16_t sym)
{
   if (sym & 1)  return -(sym >> 1);
   else          return  (sym >> 1);
}

static void print_decoded (uint16_t sym, int idy)
{
   static int16_t Adcs[16];

   int idz = idy & 0xf;
   if (idz == 0) { std::cout << "Sym[" << std::setfill (' ') << std::hex << std::setw (4) << idy << "] "; }
   if (idy == 0) { Adcs[0]   = sym; }
   else          { Adcs[idz] = Adcs[(idz-1) & 0xf] + restore (sym); }


   std::cout << std::hex << std::setw (3) << sym;

   if (idz == 0xf)
   {
      std::cout << "  ";
      for (int i = 0; i < 16; i++)  std::cout << std::setfill (' ') << std::hex << std::setw (4) << Adcs[i];
      std::cout << std::endl;
   }
   ///std::cout << "Sym[" << idy << "] = " << syms[idy] << " : " << dsyms[idy] <<  std::hex << std::setw(5) << std::endl);

   return;
}




#if 0
static int   header_decode (uint64_t *headers, uint32_t *status, uint64_t const *buf, int nbuf)
{
   // -------------------------------------------------------------------------------
   // Compose the record header word
   //    status(32) | exception count(8) | length (16) | RecType(4) | HeaderFormat(4)
   // -------------------------------------------------------------------------------
   uint64_t recHdr = buf[0];
   int      hdrFmt = (recHdr >>  0) & 0xf;
   int      recTyp = (recHdr >>  4) & 0xf;
   int      recLen = (recHdr >>  8) & 0xffff;
   int      excCnt = (recHdr >> 24) & 0xff;
           *status = (recHdr >> 32);

   uint16_t const *excBuf = reinterpret_cast<decltype(excBuf)>(buf + 1);
   uint64_t const *hdrBuf = reinterpret_cast<decltype(hdrBuf)>(excBuf) + excCnt;
   uint64_t        lastts = buf[recLen - 1];

   int nhdrs   = 6;
   uint64_t prv[6];
   headers[0] = prv[0] = *hdrBuf++;
   headers[1] = prv[1] = *hdrBuf++;
   lastts     =          *hdrBuf++;
   headers[2] = prv[2] = *hdrBuf++;
   headers[3] = prv[3] = *hdrBuf++;
   headers[4] = prv[4] = *hdrBuf++;
   headers[5] = prv[5] = *hdrBuf++;

   std::cout 
   << "Header Record:"
   <<               "Format   = " << std::setw ( 8) << std::hex <<  hdrFmt    << std::endl
   << "              RecType  = " << std::setw ( 8) << std::hex <<  recTyp    << std::endl
   << "              RecLen   = " << std::setw ( 8) << std::hex <<  recLen    << std::endl
   << "              ExcCnt   = " << std::setw ( 8) << std::hex <<  excCnt    << std::endl
   << "              Status   = " << std::setw ( 8) << std::hex << *status    << std::endl
   << "              Wib0     = " << std::setw (16) << std::hex <<  headers[0]<< std::endl
   << "              Wib01    = " << std::setw (16) << std::hex <<  headers[1]<< std::endl
   << "              Cd00     = " << std::setw (16) << std::hex <<  headers[2]<< std::endl
   << "              Cd01     = " << std::setw (16) << std::hex <<  headers[3]<< std::endl
   << "              Cd10     = " << std::setw (16) << std::hex <<  headers[4]<< std::endl
   << "              Cd11     = " << std::setw (16) << std::hex <<  headers[5]<< std::endl
   << "              LastTs   = " << std::setw (16) << std::hex <<  lastts    << std::endl;


   // Decode the exception frames
   for (int idx = 0; idx <= 4*excCnt; idx++)
   {
      uint16_t exception = *excBuf++;

      if (exception == 0) break;

      uint16_t     frame = exception & 0xffff;
      uint16_t      mask = exception >> 10;
      uint64_t       hdr;

      // Header word 0, all static fields
      if (mask &  0x1) hdr = *hdrBuf++;
      else             hdr =  prv[0];
      headers[nhdrs++]     =  prv[0] = hdr;


      // Header word 1, Timestamp, increment by 25 counts
      if (mask &  0x2) hdr = *hdrBuf++;
      else             hdr =  prv[1] + 25;
      headers[nhdrs++]     =  prv[1] = hdr;


      // Header word 2, Cold data convert count, increment by 1
      if (mask &  0x4) hdr = *hdrBuf++;
      else             hdr =  prv[2] + (1LL << 48);
      headers[nhdrs++]     =  prv[2] = hdr;


      // Header word 3, Cold data static fields
      if (mask &  0x8) hdr = *hdrBuf++;
      else             hdr = prv[3];
      headers[nhdrs++]     = prv[3] = hdr;


      // Header word 4, Cold data convert count, increment by 1
      if (mask & 0x10) hdr = *hdrBuf++;
      else             hdr = prv[4] + (1LL << 48);
      headers[nhdrs++]     = prv[4] = hdr;


      // Header word 4, Cold data static fields
      if (mask & 0x20) hdr = *hdrBuf++;
      else             hdr = prv[5];
      headers[nhdrs++]     = prv[5] = hdr;
   }

   return recLen;
}
/* ---------------------------------------------------------------------- */
#endif




#if 0
/* ---------------------------------------------------------------------- *//*!

  \brief  Prints the specified TPC Table of Contents record

  \param[in]  toc The TPC Table Of Contents record to print
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcCompressedHdr::print (pdd::record::TpcCompressedHdr const *hdr)
{
   pdd::record::TpcCompressedHdrHeader const *header = getHeader (hdr);
   TpcCompressedHdrHeader::print (header);

   TpcCompressedHdrBody::print (getBody (hdr));
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Prints the body of TPC Table of Contents record

  \param[in]   body  Pointer to the TPC table of contents record to print
  \param[in] bridge  The record's bridge word. This contains information
                     necessary to interpret the record.
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcCompressedHdrBody::print (pdd::record::TpcCompressedHdrBody const *body)
{
   return;
}
#endif
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
} /* END: namespace pdd                                                   */
/* ---------------------------------------------------------------------- */

