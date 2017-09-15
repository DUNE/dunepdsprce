// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     WibFrame.cc
 *  @brief    Core file for the DUNE compresssion
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
 *  DUNE
 *
 *  @author
 *  russell@slac.stanford.edu
 *
 *  @par Date created:
 *  2017.08.28
 * *
 * @par Credits:
 * SLAC
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\

   HISTORY
   -------

   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.08.28 jjr Split from WibFrame.hh
   2017.08.16 jjr Cloned from RCE firmware version
   ---------- --- --------------------------------
   2017.04.19 jjr Updated to the format Eric Hazen published on 2017.03.13
   2016.10.18 jjr Corrected K28.5 definition, was 0xDC -> 0xBC
   2016.06.14 jjr Created

\* ---------------------------------------------------------------------- */


#include "dam/WibFrame.hh"
#include <cinttypes>
#include <cstdio>


namespace pdd      {
namespace fragment {


static inline void expandAdcs16_avx2_init ();
static inline void expandAdcs16x1_avx2_kernel  (uint16_t        *dst,
                                                uint64_t const *src);

static inline void expandAdcs64x1_avx2_kernel  (uint16_t       *dst, 
                                                uint64_t const *src);

static inline void expandAdcs128x1_avx2_kernel (uint16_t        *dst,
                                                uint64_t const *src);

static inline void expandAdcs16x4_avx2_kernel  (uint16_t       *dst, 
                                                uint64_t const *src);

static inline void expandAdcs16x8N_avx2        (uint16_t       *dst, 
                                                int              n8,
                                                uint64_t const *src);



// TRANSPOSERS: Contigious Memory
static inline void transpose16x8N_avx2_kernel  (uint16_t       *dst, 
                                                int              n8,
                                                int          stride, 
                                                uint16_t const *src);

static inline void transpose16x8_avx2          (uint16_t       *dst, 
                                                int          stride, 
                                                uint64_t const *src);

static inline void transpose16x16_avx2         (uint16_t       *dst, 
                                                int          stride, 
                                                uint64_t const *src);

static inline void transpose16x32_avx2         (uint16_t       *dst, 
                                                int          stride, 
                                                uint64_t const *src);



// TRANSPOSERS: Channel-by-Channel Memory
static inline void transpose16x8N_avx2_kernel  (uint16_t   *dst[16], 
                                                int              n8,
                                                int          offset, 
                                                uint16_t const *src);

static inline void transpose16x8_avx2          (uint16_t  *dst[128], 
                                                int          stride, 
                                                uint64_t const *src);

static inline void transpose16x16_avx2         (uint16_t   *dst[16], 
                                                int          stride, 
                                                uint64_t const *src);

static inline void transpose16x32_avx2         (uint16_t   *dst[16], 
                                                int          stride, 
                                                uint64_t const *src);





/* ---------------------------------------------------------------------- *//*!

   \brief  Expand all 128 12-bit Adcs associated withthis WibFrame to 
           16 bits.
 
   \param[out] dst A pointer to an array large enough to hold 128 16-bit 
                   adcs.
                                                                          */
/* ---------------------------------------------------------------------- */
void  WibFrame::expandAdcs128x1 (uint16_t *dst) const
{
   m_coldData[0].expandAdcs64x1 (dst + 0*ColdData::NAdcs);
   m_coldData[1].expandAdcs64x1 (dst + 1*ColdData::NAdcs);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Expands the 128 ADC channels in each cold data stream from 
         12 -> 16 bits for \a nframes.

  \param[out]     dst  The destination array.
  \param[ in]  frames  The array of source frames to expand.
  \param[ in] nframes  The number of frames to expand.

   The destination array \a dst should be viewed as \nframes of 128 
   ADC channels.
                                                                          */
/* ---------------------------------------------------------------------- */
void WibFrame::expandAdcs128xN  (uint16_t               *dst,
                                 WibFrame const      *frames,
                                 int                 nframes)
{
   uint64_t const *src0 = frames->m_coldData[0].locateAdcs12b ();
   uint64_t const *src1 = frames->m_coldData[1].locateAdcs12b ();

   expandAdcs16_avx2_init   ();

   for (int iframe = 0; iframe < nframes; ++iframe)
   {
      expandAdcs16x4_avx2_kernel (dst,    src0);
      expandAdcs16x4_avx2_kernel (dst+64, src1);

      dst += 128;
      src0 += sizeof (WibFrame) / sizeof (*src0);
      src1 += sizeof (WibFrame) / sizeof (*src1);
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief Transposes the 128 ADC channels serviced by a WibFrame for
          8 time samples.  

   \param[in]       dst[out]  The output destination array. 
   \param[in] ndstStride[in]  The number of entries of each of the 128
                              arrays of transposed ADC values.
   \param[in]      frame[in]  The WibFrame 
                              

   This output array should be thought of as a 2d array dst[128][ndstStride].
   This allows each channel space for ndstStride contigous transposed
   ADC values.

   If one is doing 16 or more, use the Transpose128x16N routine. It is
   20-30% faster than doing this is groups of 8 time samples.
                                                                          */
/* ---------------------------------------------------------------------- */                        
void WibFrame::transposeAdcs128x8 (uint16_t             *dst,
                                   int            ndstStride,
                                   WibFrame const     *frame)
{
   // ------------------------------------------------
   // Locate the packed data for each cold data stream
   // ------------------------------------------------
   uint64_t const *src0 = frame->m_coldData[0].locateAdcs12b ();
   uint64_t const *src1 = frame->m_coldData[1].locateAdcs12b ();


   // ----------------------------------------------------------------
   // Locate where in the output data for the 2 cold data streams goes
   // ----------------------------------------------------------------
   uint16_t *dst0 = dst;
   uint16_t *dst1 = dst + 64 * ndstStride;

   // ---------------------------------
   // Initialize the expander registers
   // ---------------------------------
   expandAdcs16_avx2_init ();

   // ----------------------------------
   // Loop over the adcs in groups of 16
   // ----------------------------------
   for (int iadcs = 0; iadcs < 64; iadcs += 16)
   {
      // ---------------------------------------------------------------
      // Transpose the cold data stream 0 & 1  for 16 channels x 8 times
      // ---------------------------------------------------------------
      transpose16x8_avx2 (dst0, ndstStride, src0);
      dst0 += 16*ndstStride;
      src0 +=  3;

      transpose16x8_avx2 (dst1, ndstStride, src1);
      dst1 += 16*ndstStride;
      src1 +=  3;
   }

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \brief Transposes the 128 ADC channels serviced by a WibFrame for
          \a nframes time samples.  nframes must be a multiple of 16.

   \param[in]       dst[out]  The output destination array. 
   \param[in] ndstStride[in]  The number of entries of each of the 128
                              arrays of transposed ADC values.
   \param[in]     frames[in]  The array of WibFrames
   \param[in]    nframes[in]  The number frames, \e i.e. time samples
                              to transpose.  This must a multiple of 16.

   This output array should be thought of as a 2d array dst[128][ndstStride].
   This allows each channel space for ndstStride contigous transposed
   ADC values.
                                                                          */
/* ---------------------------------------------------------------------- */                        
void WibFrame::transposeAdcs128x16N (uint16_t             *dst,
                                     int            ndstStride,
                                     WibFrame const    *frames,
                                     int               nframes)
{
   // ------------------------------------------------
   // Locate the packed data for each cold data stream
   // ------------------------------------------------
   uint64_t const *src0 = frames->m_coldData[0].locateAdcs12b ();
   uint64_t const *src1 = frames->m_coldData[1].locateAdcs12b ();


   // ----------------------------------------------------------------
   // Locate where in the output data for the 2 cold data streams goes
   // ----------------------------------------------------------------
   uint16_t *dst0 = dst;
   uint16_t *dst1 = dst + 64 * ndstStride;


   int n16frames = nframes/16;


   // ---------------------------------
   // Initialize the expander registers
   // ---------------------------------
   expandAdcs16_avx2_init ();

   // ------------------------------=-----
   // Loop over the frames in groups of 16
   // ------------------------------------
   for (int iframe = 0; iframe < n16frames; ++iframe)
   { 
      uint64_t const *lclsrc0 = src0;
      uint64_t const *lclsrc1 = src1;

      uint16_t       *lcldst0 = dst0;
      uint16_t       *lcldst1 = dst1;


      // ----------------------------------
      // Loop over the adcs in groups of 16
      // ----------------------------------
      for (int iadcs = 0; iadcs < 64; iadcs += 16)
      {
         // ----------------------------------------------------------------
         // Transpose the cold data stream 0 & 1  for 16 channels x 16 times
         // ----------------------------------------------------------------
         transpose16x16_avx2 (lcldst0, ndstStride, lclsrc0);
         lcldst0 += 16*ndstStride;
         lclsrc0 +=  3;

         transpose16x16_avx2 (lcldst1, ndstStride, lclsrc1);
         lcldst1 += 16*ndstStride;
         lclsrc1 +=  3;
      }

      // Advance the source and destination by the 8 time frames
      src0 += 16 * sizeof (WibFrame) / sizeof (*src0);
      src1 += 16 * sizeof (WibFrame) / sizeof (*src1);

      // Advance the destination by the same 
      dst0 += 16;
      dst1 += 16;
   }

   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

   \brief Transposes the 128 ADC channels serviced by a WibFrame for
          \a nframes time samples.  nframes must be a multiple of 32.

   \param[in]       dst[out]  The output destination array. 
   \param[in] ndstStride[in]  The number of entries of each of the 128
                              arrays of transposed ADC values.
   \param[in]     frames[in]  The array of WibFrames
   \param[in]    nframes[in]  The number frames, \e i.e. time samples
                              to transpose.  This must a multiple of 32.

   This output array should be thought of as a 2d array dst[128][ndstStride].
   This allows each channel space for ndstStride contigous transposed
   ADC values.
                                                                          */
/* ---------------------------------------------------------------------- */                        
void WibFrame::transposeAdcs128x32N (uint16_t             *dst,
                                     int            ndstStride,
                                     WibFrame const    *frames,
                                     int               nframes)
{
   // ------------------------------------------------
   // Locate the packed data for each cold data stream
   // ------------------------------------------------
   uint64_t const *src0 = frames->m_coldData[0].locateAdcs12b ();
   uint64_t const *src1 = frames->m_coldData[1].locateAdcs12b ();


   // ----------------------------------------------------------------
   // Locate where in the output data for the 2 cold data streams goes
   // ----------------------------------------------------------------
   uint16_t *dst0 = dst;
   uint16_t *dst1 = dst + 64 * ndstStride;


   int n32frames = nframes/32;


   // ---------------------------------
   // Initialize the expander registers
   // ---------------------------------
   expandAdcs16_avx2_init ();

   // ------------------------------=-----
   // Loop over the frames in groups of 16
   // ------------------------------------
   for (int iframe = 0; iframe < n32frames; ++iframe)
   { 
      uint64_t const *lclsrc0 = src0;
      uint64_t const *lclsrc1 = src1;

      uint16_t       *lcldst0 = dst0;
      uint16_t       *lcldst1 = dst1;

      // ----------------------------------
      // Loop over the adcs in groups of 16
      // ----------------------------------
      for (int iadcs = 0; iadcs < 64; iadcs += 16)
      {
         // ----------------------------------------------------------------
         // Transpose the cold data stream 0 & 1  for 16 channels x 32 times
         // ----------------------------------------------------------------
         transpose16x32_avx2 (lcldst0, ndstStride, lclsrc0);
         lcldst0 += 16*ndstStride;
         lclsrc0 +=  3;

         transpose16x32_avx2 (lcldst1, ndstStride, lclsrc1);
         lcldst1 += 16*ndstStride;
         lclsrc1 +=  3;
      }

      // Advance the source and destination by the 8 time frames
      src0 += 32 * sizeof (WibFrame) / sizeof (*src0);
      src1 += 32 * sizeof (WibFrame) / sizeof (*src1);

      // Advance the destination by the same 
      dst0 += 32;
      dst1 += 32;
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ====================================================================== */
/* BEGIN: CHANNEL-BY-CHANNEL TRANSPOSERS                                  */
/* ---------------------------------------------------------------------- *//*!

   \brief Transposes the 128 ADC channels serviced by a WibFrame for
          8 time samples.  

   \param[in]       dst[out]  Array of pointers to the channel-by-channel
                              destination arrays 
   \param[in]         offset  The offset into the destination arrays to
                              store the first transposed ADC.
   \param[in]      frame[in]  The WibFrame 
                                                                          */
/* ---------------------------------------------------------------------- */                        
void WibFrame::transposeAdcs128x8 (uint16_t        *dst[128],
                                   int                offset,
                                   WibFrame const     *frame)
{
   // ------------------------------------------------
   // Locate the packed data for each cold data stream
   // ------------------------------------------------
   uint64_t const *src0 = frame->m_coldData[0].locateAdcs12b ();
   uint64_t const *src1 = frame->m_coldData[1].locateAdcs12b ();


   // ---------------------------------
   // Initialize the expander registers
   // ---------------------------------
   expandAdcs16_avx2_init ();

   // ----------------------------------
   // Loop over the adcs in groups of 16
   // ----------------------------------
   for (int iadcs = 0; iadcs < 64; iadcs += 16)
   {
      // ---------------------------------------------------------------
      // Transpose the cold data stream 0 & 1  for 16 channels x 8 times
      // ---------------------------------------------------------------
      transpose16x8_avx2 (dst, offset, src0);
      dst  += 16;
      src0 +=  3;

      transpose16x8_avx2 (dst, offset, src1);
      dst  += 16;
      src1 +=  3;
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief Transposes the 128 ADC channels serviced by a WibFrame for
          \a nframes time samples.  nframes must be a multiple of 16.

   \param[in]       dst[out]  Array of pointers to the channel-by-channel
                              destination arrays 
   \param[in]         offset  The offset into the destination arrays to
                              store the first transposed ADC.
   \param[in]     frames[in]  The array of WibFrames
   \param[in]    nframes[in]  The number frames, \e i.e. time samples
                              to transpose.  This must a multiple of 16.
                                                                          */
/* ---------------------------------------------------------------------- */                        
void WibFrame::transposeAdcs128x16N (uint16_t        *dst[128],
                                     int                offset,
                                     WibFrame const    *frames,
                                     int               nframes)
{
   // ------------------------------------------------
   // Locate the packed data for each cold data stream
   // ------------------------------------------------
   uint64_t const *src0 = frames->m_coldData[0].locateAdcs12b ();
   uint64_t const *src1 = frames->m_coldData[1].locateAdcs12b ();


   // ----------------------------------------------------------------
   // Locate where in the output data for the 2 cold data streams goes
   // ----------------------------------------------------------------
   uint16_t **dst0 = dst;
   uint16_t **dst1 = dst + 16;


   // ---------------------------------
   // Initialize the expander registers
   // ---------------------------------
   expandAdcs16_avx2_init ();


   // ------------------------------=-----
   // Loop over the frames in groups of 16
   // ------------------------------------
   int n16frames = nframes/16;
   for (int iframe = 0; iframe < n16frames; ++iframe)
   { 
      uint64_t const *lclsrc0 = src0;
      uint64_t const *lclsrc1 = src1;

      uint16_t      **lcldst0 = dst0;
      uint16_t      **lcldst1 = dst1;

      // ----------------------------------
      // Loop over the adcs in groups of 16
      // ----------------------------------
      for (int iadcs = 0; iadcs < 64; iadcs += 16)
      {
         // ----------------------------------------------------------------
         // Transpose the cold data stream 0 & 1  for 16 channels x 16 times
         // ----------------------------------------------------------------
         transpose16x16_avx2 (lcldst0, offset, lclsrc0);
         lcldst0 += 16;
         lclsrc0 +=  3;

         transpose16x16_avx2 (lcldst1, offset, lclsrc1);
         lcldst1 += 16;
         lclsrc1 +=  3;
      }


      // Advance the source and destination by the 16 time frames
      src0   += 16 * sizeof (WibFrame) / sizeof (*src0);
      src1   += 16 * sizeof (WibFrame) / sizeof (*src1);
      offset += 16;

   }

   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

   \brief Transposes the 128 ADC channels serviced by a WibFrame for
          \a nframes time samples.  nframes must be a multiple of 32.

   \param[in]       dst[out]  Array of pointers to the channel-by-channel
                              destination arrays 
   \param[in]         offset  The offset into the destination arrays to
                              store the first transposed ADC.
   \param[in]     frames[in]  The array of WibFrames
   \param[in]    nframes[in]  The number frames, \e i.e. time samples
                              to transpose.  This must a multiple of 32.

   This output array should be thought of as a 2d array dst[128][ndstStride].
   This allows each channel space for ndstStride contigous transposed
   ADC values.
                                                                          */
/* ---------------------------------------------------------------------- */                        
void WibFrame::transposeAdcs128x32N (uint16_t        *dst[128],
                                     int                offset,
                                     WibFrame const    *frames,
                                     int               nframes)
{
   // ------------------------------------------------
   // Locate the packed data for each cold data stream
   // ------------------------------------------------
   uint64_t const *src0 = frames->m_coldData[0].locateAdcs12b ();
   uint64_t const *src1 = frames->m_coldData[1].locateAdcs12b ();


   // ----------------------------------------------------------------
   // Locate where in the output data for the 2 cold data streams goes
   // ----------------------------------------------------------------
   uint16_t **dst0 = dst;
   uint16_t **dst1 = dst + 32;


   // ---------------------------------
   // Initialize the expander registers
   // ---------------------------------
   expandAdcs16_avx2_init ();

   // ------------------------------=-----
   // Loop over the frames in groups of 16
   // ------------------------------------
   int n32frames = nframes/32;
   for (int iframe = 0; iframe < n32frames; ++iframe)
   { 
      uint64_t const *lclsrc0 = src0;
      uint64_t const *lclsrc1 = src1;

      uint16_t      **lcldst0 = dst0;
      uint16_t      **lcldst1 = dst1;

      // ----------------------------------
      // Loop over the adcs in groups of 16
      // ----------------------------------
      for (int iadcs = 0; iadcs < 64; iadcs += 16)
      {
         // ----------------------------------------------------------------
         // Transpose the cold data stream 0 & 1  for 16 channels x 32 times
         // ----------------------------------------------------------------
         transpose16x32_avx2 (lcldst0, offset, lclsrc0);
         lcldst0 += 32;
         lclsrc0 +=  3;

         transpose16x32_avx2 (lcldst1, offset, lclsrc1);
         lcldst1 += 32;
         lclsrc1 +=  3;
      }

      // Advance the source and destination by the 32 time frames
      src0   += 32 * sizeof (WibFrame) / sizeof (*src0);
      src1   += 32 * sizeof (WibFrame) / sizeof (*src1);
      offset += 32;
   }

   return;
}
/* ---------------------------------------------------------------------- */
/* END: CHANNEL-BY-CHANNEL TRANSPOSERS                                  */
/* ====================================================================== */




/* ---------------------------------------------------------------------- */
static void print (char const *what, uint64_t d[4]) __attribute__ ((unused));
static void print (char const *what, uint64_t d[4])
{
   printf ("%-15.15s %16.16" PRIx64 " %16.16" PRIx64 " %16.16" PRIx64 " %16.16" PRIx64 "\n", 
           what, d[3], d[2], d[1], d[0]);
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Initializes the ymm0 and ymm1 AVX2 registers to contain the 
         variable shifts needed to left justify 18-bit values within 
         their respective 32-bit lanes.  

 \note
  There are 2 patterns,
     -# ymm0 contains the variable shifts for the even words
     -# ymm1 contains the variable shifts for the  odd words
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void expandAdcs16_avx2_init ()
{

   /* Each entry indicates where the src byte comes from in the destination */
   static uint8_t const Shuffle0[16] __attribute__ ((aligned (32))) = 
   {
   //    0     1     2     3     4     5     6     7
      0x00, 0x01, 0x02, 0x80, 0x03, 0x04, 0x05, 0x80,

    //   8     9     a     b     c     d     e     f
      0x06, 0x07, 0x08, 0x80, 0x09, 0x0a, 0x0b, 0x80
   };


   ///uint64_t Dbg[4] __attribute__ ((aligned (32)));


   /* Load the variable shift patterns into ymm15 */
   asm ("vbroadcasti128  %0,%%ymm15" :: "m"(Shuffle0[0]) : "%xmm15");
   ///asm ("vmovapd     %%ymm15,%0"            : "=m"(Dbg[0])  ::);
   //print ("Remap0 Array:", Dbg);

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief The kernel to unpack 64 densely packet 12-bit values into 
         64 16-bit values.

  \param[in] dst  The destination address
  \param[in] src  The source address

  \warning
   This routine assumes that the SIMD register ymm15 is initialized
   by eapandAdcs_avx2_init to do the initial shuffle
                                                                          */
/* ---------------------------------------------------------------------- */
inline void expandAdcs16x1_avx2_kernel (uint16_t *dst, uint64_t const *src)
{
   uint8_t const *s8 =  reinterpret_cast<decltype (s8)>(src);

   /* 

        7      6      5      4       3      2      1      0
     ----   ----   ---- | ----    ----   ----   ----   ----
                   7776 | 6655 || 5444 | 3332 | 2211 | 1000  A

   */

   #ifdef   DEBUG
   uint64_t Dbg[16][4];
   #endif

   /* 
    | This describes the bit twiddling in the following instructions.
    | Only the lower 128 bits are shown, containing ADCS 0 - 7.
    | The upper 128 bits are  identical, containing ADCS 8 - F
    |
    | The goal is to divide the data into groups of 6 bytes in each of the 8 32-bit words
    | This is done using the vpshufb instruction to move these groups of 6 bytes
    | Each group of 32 begins with a byte of 0s and 2 12-bit.  
    | A nibble must be insert between these two.  
    | The brute force way works of just shifting them within a 32-bit word works best.
    |
    |
    |    7      6      5      4       3      2      1      0
    | ---- | ---- | ---- | ----    ----   ----   ----   ----
    |               7776 | 6655 || 5444 | 3332 | 2211 | 1000  xmm3  vmovupd + vinsert ymm3
    | zz77 | 7666 | zz55 | 5444 || zz33 | 3222 | zz11 | 1000  xmm3  vpshuf ymm0,ymm3,ymm3
    |
    | 666z | zzzz | 444z | zzzz || 222z | zzzz | 000z | zzzz  vpslld 20,ymm3,ymm4
    | zzzz | z777 | zzzz | z555 || zzzz | z333 | zzzz | z111  vpsrld 12,ymm3,ymm5

    | zzzz | z666 | zzzz | z444 || zzzz | z222 | zzzz | z000  vpsrld 20,ymm4,ymm4
    | z777 | zzzz | z555 | zzzz || z333 | zzzz | z111 | zzzz  vpsrld 16,ymm3,ymm5
    |
    | z777 | z666 | z555 | z444 || z333 | z222 | z111 | z000  vpor   ymm3,ymm4,ymm3
    |
   */
   //asm ("vmovupd     %0,%%xmm3"             :: "m"(s8[0])  : "%xmm3");
   asm ("vinserti128 $0,%0,%%ymm3,%%ymm3"   :: "m"(s8[0]) : "%xmm3");
   asm ("vinserti128 $1,%0,%%ymm3,%%ymm3"   :: "m"(s8[12]) : "%xmm3");


   /* ----- DEBUG ---- */
   #ifdef   DEBUG
   asm ("vmovupd     %%ymm3,%0"             : "=m"(Dbg[3][0])  :: "memory");
   print ("Initial Load:", Dbg[3]);
   asm ("vmovupd     %0,%%ymm3"             :: "m"(Dbg[3][0])  : "%xmm3");
   #endif
   /* ----- END DEBUG ---- */


   asm ("vpshufb     %%ymm15,%%ymm3,%%ymm3" ::             : "%xmm3");


   /* ----- DEBUG ---- */
   #ifdef   DEBUG
   asm ("vmovupd     %%ymm3,%0"             : "=m"(Dbg[3][0])  ::);
   print ("Shuffle0    :", Dbg[3]);
   asm ("vmovupd     %0,%%ymm3"             :: "m"(Dbg[3][0]) : "%xmm3");
   #endif
   /* ----- END DEBUG ---- */


   asm ("vpslld      $5*4,%%ymm3,%%ymm4"    ::             : "%xmm4");
   asm ("vpsrld      $3*4,%%ymm3,%%ymm3"    ::             : "%xmm3");

   /* ----- DEBUG ---- */
   #ifdef   DEBUG
   asm ("vmovupd     %%ymm4,%0"            : "=m"(Dbg[4][0])  :: "memory");
   asm ("vmovupd     %%ymm3,%0"            : "=m"(Dbg[3][0])  :: "memory");

   print ("Ymm4 << 5:", Dbg[4]);
   print ("Ymm3 >> 3:", Dbg[3]);

   asm ("vmovupd     %0,%%ymm4"            :: "m"(Dbg[4][0])  : "%xmm4");
   asm ("vmovupd     %0,%%ymm3"            :: "m"(Dbg[3][0])  : "%xmm3");
   #endif
   /* ----- END DEBUG ---- */


   asm ("vpsrld      $5*4,%%ymm4,%%ymm4"   ::             : "%xmm4");
   asm ("vpslld      $4*4,%%ymm3,%%ymm3"   ::             : "%xmm3");

   /* ----- DEBUG ---- */
   #ifdef   DEBUG
   asm ("vmovupd     %%ymm4,%0"            : "=m"(Dbg[4][0])  :: "memory");
   asm ("vmovupd     %%ymm3,%0"            : "=m"(Dbg[3][0])  :: "memory");

   print ("Ymm4 >> 5:", Dbg[4]);
   print ("Ymm3 << 4:", Dbg[3]);

   asm ("vmovupd     %0,%%ymm3"            :: "m"(Dbg[3][0])  : "%xmm3");
   asm ("vmovupd     %0,%%ymm4"            :: "m"(Dbg[4][0])  : "%xmm4");
   #endif
   /* ----- END DEBUG ---- */

   asm ("vpor        %%ymm3,%%ymm4,%%ymm3" ::             : "%xmm3");


   /* ----- DEBUG ---- */
   #ifdef   DEBUG
   asm ("vmovupd     %%ymm3,%0"            : "=m"(Dbg[3][0])  :: "memory");
   print ("Ymm3 or:", Dbg[3]);
   asm ("vmovupd     %0,%%ymm3"            :: "m"(Dbg[3][0])  : "%xmm3");
   #endif
   /* ----- END DEBUG ---- */


   /* Store the result */
   asm ("vmovupd      %%ymm3,%0"   : "=m"(dst[0])  :: "memory");

   /* ----- DEBUG ---- */
   #ifdef   DEBUG
   print ("Result:", (uint64_t *)dst);
   #endif
   /* ----- END DEBUG ---- */

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief The kernel to unpack 64 densely packet 12-bit values into 
         64 16-bit values.

  \param[in] dst  The destination address
  \param[in] src  The source address

  \warning
   This routine assumes that the SIMD register ymm15 is initialized
   by expancAdcs_avx2_init to do the initial shuffle
                                                                          */
/* ---------------------------------------------------------------------- */
inline void expandAdcs64x1_avx2_kernel (uint16_t       *dst, 
                                       uint64_t const *src)
{
   uint8_t const *s8 =  reinterpret_cast<decltype (s8)>(src);

   #ifdef   DEBUG
   uint64_t Dbg[16][4];
   #endif


   //asm ("vmovupd     %0,%%xmm0"             :: "m"(s8[0*24])    : "%xmm0" );
   asm ("vinserti128 $0,%0,%%ymm0,%%ymm0"   :: "m"(s8[0*24])    : "%xmm0" );
   asm ("vinserti128 $1,%0,%%ymm0,%%ymm0"   :: "m"(s8[0*24+12]) : "%xmm0" );
   asm ("vpshufb     %%ymm15,%%ymm0,%%ymm0" ::                  : "%xmm0" );
   asm ("vpslld      $5*4,%%ymm0,%%ymm14"   ::                  : "%xmm14");
   asm ("vpsrld      $3*4,%%ymm0,%%ymm0"    ::                  : "%xmm0" );
   asm ("vpsrld      $5*4,%%ymm14,%%ymm14"  ::                  : "%xmm14");
   asm ("vpslld      $4*4,%%ymm0,%%ymm0"    ::                  : "%xmm0" );
   asm ("vpor        %%ymm0,%%ymm14,%%ymm0" ::                  : "%xmm0" );
   asm ("vmovupd     %%ymm0,%0"             : "=m"(dst[0*16])   :: "memory");


   //asm ("vmovupd     %0,%%xmm1"             :: "m"(s8[1*24])    : "%xmm2" );
   asm ("vinserti128 $0,%0,%%ymm1,%%ymm1"   :: "m"(s8[1*24])    : "%xmm2" );
   asm ("vinserti128 $1,%0,%%ymm1,%%ymm1"   :: "m"(s8[1*24+12]) : "%xmm2" );
   asm ("vpshufb     %%ymm15,%%ymm1,%%ymm1" ::                  : "%xmm1" );
   asm ("vpslld      $5*4,%%ymm1,%%ymm14"   ::                  : "%xmm14");
   asm ("vpsrld      $3*4,%%ymm1,%%ymm1"    ::                  : "%xmm1" );
   asm ("vpsrld      $5*4,%%ymm14,%%ymm14"  ::                  : "%xmm14");
   asm ("vpslld      $4*4,%%ymm1,%%ymm1"    ::                  : "%xmm1" );
   asm ("vpor        %%ymm1,%%ymm14,%%ymm1" ::                  : "%xmm1" );
   asm ("vmovupd     %%ymm1,%0"             : "=m"(dst[1*16])   :: "memory");


        //asm ("vmovupd     %0,%%xmm2"             :: "m"(s8[2*24])    : "%xmm2" );
   asm ("vinserti128 $0,%0,%%ymm2,%%ymm2"   :: "m"(s8[2*24])    : "%xmm2" );
   asm ("vinserti128 $1,%0,%%ymm2,%%ymm2"   :: "m"(s8[2*24+12]) : "%xmm2" );
   asm ("vpshufb     %%ymm15,%%ymm2,%%ymm2" ::                  : "%xmm2" );
   asm ("vpslld      $5*4,%%ymm2,%%ymm14"   ::                  : "%xmm14");
   asm ("vpsrld      $3*4,%%ymm2,%%ymm2"    ::                  : "%xmm2" );
   asm ("vpsrld      $5*4,%%ymm14,%%ymm14"  ::                  : "%xmm14");
   asm ("vpslld      $4*4,%%ymm2,%%ymm2"    ::                  : "%xmm2" );
   asm ("vpor        %%ymm2,%%ymm14,%%ymm2" ::                  : "%xmm2" );
   asm ("vmovupd     %%ymm2,%0"             : "=m"(dst[2*16])   :: "memory");


   //asm ("vmovupd     %0,%%xmm3"             :: "m"(s8[3*24])    : "%xmm3" );
   asm ("vinserti128 $0,%0,%%ymm3,%%ymm3"   :: "m"(s8[3*24])    : "%xmm3" );
   asm ("vinserti128 $1,%0,%%ymm3,%%ymm3"   :: "m"(s8[3*24+12]) : "%xmm3" );
   asm ("vpshufb     %%ymm15,%%ymm3,%%ymm3" ::                  : "%xmm3" );
   asm ("vpslld      $5*4,%%ymm3,%%ymm14"   ::                  : "%xmm14");
   asm ("vpsrld      $3*4,%%ymm3,%%ymm3"    ::                  : "%xmm3" );
   asm ("vpsrld      $5*4,%%ymm14,%%ymm14"  ::                  : "%xmm14");
   asm ("vpslld      $4*4,%%ymm3,%%ymm3"    ::                  : "%xmm3" );
   asm ("vpor        %%ymm3,%%ymm14,%%ymm3" ::                  : "%xmm3" );
   asm ("vmovupd     %%ymm3,%0"             : "=m"(dst[3*16])   :: "memory");

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief The kernel to unpack 128 densely packet 12-bit values into 
         6128 16-bit values.

  \param[in] dst  The destination address
  \param[in] src  The source address

  \warning
   This routine assumes that the SIMD register ymm15 is initialized
   by expandAdcs_avx2_init to do the initial shuffle
                                                                          */
/* ---------------------------------------------------------------------- */
inline void expandAdcs128x1_avx2_kernel (uint16_t       *dst, 
                                        uint64_t const *src)
{
   uint8_t const *s8 =  reinterpret_cast<decltype (s8)>(src);

   #ifdef   DEBUG
   uint64_t Dbg[16][4];
   #endif


   //asm ("vmovupd     %0,%%xmm0"             :: "m"(s8[0*24])    : "%xmm0" );
   asm ("vinserti128 $0,%0,%%ymm0,%%ymm0"   :: "m"(s8[0*24])    : "%xmm0" );
   asm ("vinserti128 $1,%0,%%ymm0,%%ymm0"   :: "m"(s8[0*24+12]) : "%xmm0" );
   asm ("vpshufb     %%ymm15,%%ymm0,%%ymm0" ::                  : "%xmm0" );
   asm ("vpslld      $5*4,%%ymm0,%%ymm14"   ::                  : "%xmm14");
   asm ("vpsrld      $3*4,%%ymm0,%%ymm0"    ::                  : "%xmm0" );
   asm ("vpsrld      $5*4,%%ymm14,%%ymm14"  ::                  : "%xmm14");
   asm ("vpslld      $4*4,%%ymm0,%%ymm0"    ::                  : "%xmm0" );
   asm ("vpor        %%ymm0,%%ymm14,%%ymm0" ::                  : "%xmm0" );
   asm ("vmovupd     %%ymm0,%0"             : "=m"(dst[0*16])   :: "memory");


   //asm ("vmovupd     %0,%%xmm1"             :: "m"(s8[1*24])    : "%xmm2" );
   asm ("vinserti128 $0,%0,%%ymm1,%%ymm1"   :: "m"(s8[1*24])    : "%xmm2" );
   asm ("vinserti128 $1,%0,%%ymm1,%%ymm1"   :: "m"(s8[1*24+12]) : "%xmm2" );
   asm ("vpshufb     %%ymm15,%%ymm1,%%ymm1" ::                  : "%xmm1" );
   asm ("vpslld      $5*4,%%ymm1,%%ymm14"   ::                  : "%xmm14");
   asm ("vpsrld      $3*4,%%ymm1,%%ymm1"    ::                  : "%xmm1" );
   asm ("vpsrld      $5*4,%%ymm14,%%ymm14"  ::                  : "%xmm14");
   asm ("vpslld      $4*4,%%ymm1,%%ymm1"    ::                  : "%xmm1" );
   asm ("vpor        %%ymm1,%%ymm14,%%ymm1" ::                  : "%xmm1" );
   asm ("vmovupd     %%ymm1,%0"             : "=m"(dst[1*16])   :: "memory");


   //asm ("vmovupd     %0,%%xmm2"             :: "m"(s8[2*24])    : "%xmm2" );
   asm ("vinserti128 $0,%0,%%ymm2,%%ymm2"   :: "m"(s8[2*24])    : "%xmm2" );
   asm ("vinserti128 $1,%0,%%ymm2,%%ymm2"   :: "m"(s8[2*24+12]) : "%xmm2" );
   asm ("vpshufb     %%ymm15,%%ymm2,%%ymm2" ::                  : "%xmm2" );
   asm ("vpslld      $5*4,%%ymm2,%%ymm14"   ::                  : "%xmm14");
   asm ("vpsrld      $3*4,%%ymm2,%%ymm2"    ::                  : "%xmm2" );
   asm ("vpsrld      $5*4,%%ymm14,%%ymm14"  ::                  : "%xmm14");
   asm ("vpslld      $4*4,%%ymm2,%%ymm2"    ::                  : "%xmm2" );
   asm ("vpor        %%ymm2,%%ymm14,%%ymm2" ::                  : "%xmm2" );
   asm ("vmovupd     %%ymm2,%0"             : "=m"(dst[2*16])   :: "memory");


   //asm ("vmovupd     %0,%%xmm3"             :: "m"(s8[3*24])    : "%xmm3" );
   asm ("vinserti128 $0,%0,%%ymm3,%%ymm3"   :: "m"(s8[3*24])    : "%xmm3" );
   asm ("vinserti128 $1,%0,%%ymm3,%%ymm3"   :: "m"(s8[3*24+12]) : "%xmm3" );
   asm ("vpshufb     %%ymm15,%%ymm3,%%ymm3" ::                  : "%xmm3" );
   asm ("vpslld      $5*4,%%ymm3,%%ymm14"   ::                  : "%xmm14");
   asm ("vpsrld      $3*4,%%ymm3,%%ymm3"    ::                  : "%xmm3" );
   asm ("vpsrld      $5*4,%%ymm14,%%ymm14"  ::                  : "%xmm14");
   asm ("vpslld      $4*4,%%ymm3,%%ymm3"    ::                  : "%xmm3" );
   asm ("vpor        %%ymm3,%%ymm14,%%ymm3" ::                  : "%xmm3" );
   asm ("vmovupd     %%ymm3,%0"             : "=m"(dst[3*16])   :: "memory");


   //asm ("vmovupd     %0,%%xmm4"             :: "m"(s8[4*24])    : "%xmm4" );
   asm ("vinserti128 $0,%0,%%ymm4,%%ymm4"   :: "m"(s8[4*24])    : "%xmm4" );
   asm ("vinserti128 $1,%0,%%ymm4,%%ymm4"   :: "m"(s8[4*24+12]) : "%xmm4" );
   asm ("vpshufb     %%ymm15,%%ymm4,%%ymm4" ::                  : "%xmm4" );
   asm ("vpslld      $5*4,%%ymm4,%%ymm14"   ::                  : "%xmm14");
   asm ("vpsrld      $3*4,%%ymm4,%%ymm4"    ::                  : "%xmm4" );
   asm ("vpsrld      $5*4,%%ymm14,%%ymm14"  ::                  : "%xmm14");
   asm ("vpslld      $4*4,%%ymm4,%%ymm4"    ::                  : "%xmm4" );
   asm ("vpor        %%ymm4,%%ymm14,%%ymm4" ::                  : "%xmm4" );
   asm ("vmovupd     %%ymm4,%0"             : "=m"(dst[4*16])   :: "memory");


   //asm ("vmovupd     %0,%%xmm5"             :: "m"(s8[5*24])    : "%xmm5" );
   asm ("vinserti128 $0,%0,%%ymm5,%%ymm5"   :: "m"(s8[5*24])    : "%xmm5" );
   asm ("vinserti128 $1,%0,%%ymm5,%%ymm5"   :: "m"(s8[5*24+12]) : "%xmm5" );
   asm ("vpshufb     %%ymm15,%%ymm5,%%ymm5" ::                  : "%xmm5" );
   asm ("vpslld      $5*4,%%ymm5,%%ymm14"   ::                  : "%xmm14");
   asm ("vpsrld      $3*4,%%ymm5,%%ymm5"    ::                  : "%xmm5" );
   asm ("vpsrld      $5*4,%%ymm14,%%ymm14"  ::                  : "%xmm14");
   asm ("vpslld      $4*4,%%ymm5,%%ymm5"    ::                  : "%xmm5" );
   asm ("vpor        %%ymm5,%%ymm14,%%ymm5" ::                  : "%xmm5" );
   asm ("vmovupd     %%ymm5,%0"             : "=m"(dst[5*16])   :: "memory");


   //asm ("vmovupd     %0,%%xmm6"             :: "m"(s8[6*24])    : "%xmm6" );
   asm ("vinserti128 $0,%0,%%ymm6,%%ymm6"   :: "m"(s8[6*24])    : "%xmm6" );
   asm ("vinserti128 $1,%0,%%ymm6,%%ymm6"   :: "m"(s8[6*24+12]) : "%xmm6" );
   asm ("vpshufb     %%ymm15,%%ymm6,%%ymm6" ::                  : "%xmm6" );
   asm ("vpslld      $5*4,%%ymm6,%%ymm14"   ::                  : "%xmm14");
   asm ("vpsrld      $3*4,%%ymm6,%%ymm6"    ::                  : "%xmm6" );
   asm ("vpsrld      $5*4,%%ymm14,%%ymm14"  ::                  : "%xmm14");
   asm ("vpslld      $4*4,%%ymm6,%%ymm6"    ::                  : "%xmm6" );
   asm ("vpor        %%ymm6,%%ymm14,%%ymm6" ::                  : "%xmm6" );
   asm ("vmovupd     %%ymm6,%0"             : "=m"(dst[6*16])   :: "memory");


   //asm ("vmovupd     %0,%%xmm7"             :: "m"(s8[7*24])    : "%xmm7" );
   asm ("vinserti128 $0,%0,%%ymm7,%%ymm7"   :: "m"(s8[7*24])    : "%xmm7" );
   asm ("vinserti128 $1,%0,%%ymm7,%%ymm7"   :: "m"(s8[7*24+12]) : "%xmm7" );
   asm ("vpshufb     %%ymm15,%%ymm7,%%ymm7" ::                  : "%xmm7" );
   asm ("vpslld      $5*4,%%ymm7,%%ymm14"   ::                  : "%xmm14");
   asm ("vpsrld      $3*4,%%ymm7,%%ymm7"    ::                  : "%xmm7" );
   asm ("vpsrld      $5*4,%%ymm14,%%ymm14"  ::                  : "%xmm14");
   asm ("vpslld      $4*4,%%ymm7,%%ymm7"    ::                  : "%xmm7" );
   asm ("vpor        %%ymm7,%%ymm14,%%ymm7" ::                  : "%xmm7" );
   asm ("vmovupd     %%ymm7,%0"             : "=m"(dst[7*16])   :: "memory");


   return;
}
/* ---------------------------------------------------------------------- */


#define STRIDE sizeof (WibFrame)



/* ---------------------------------------------------------------------- *//*!

  \brief The kernel to unpack 16 densely packet 12-bit values for 4
         time samples into  64 16-bit values.

  \param[in] dst  The destination address
  \param[in] src  The source address

  \warning
   This routine assumes that the SIMD register ymm15 is initialized
   by expandhAdcs_avx2_init to do the initial shuffle
                                                                          */
/* ---------------------------------------------------------------------- */
inline void expandAdcs16x4_avx2_kernel (uint16_t       *dst, 
                                       uint64_t const *src)
{
   uint8_t const *s8 =  reinterpret_cast<decltype (s8)>(src);

   #ifdef   DEBUG
   uint64_t Dbg[16][4];
   #endif

   /*
   printf ("First %4.4x %4.4x %4.4x %4.4x\n",
           ((uint16_t const *)s8)[0*STRIDE/2],
           ((uint16_t const *)s8)[1*STRIDE/2],
           ((uint16_t const *)s8)[2*STRIDE/2],
           ((uint16_t const *)s8)[3*STRIDE/2]);
   */


   //asm ("vmovupd     %0,%%xmm0"             :: "m"(s8[0*STRIDE])    : "%xmm0" );
   asm ("vinserti128 $0,%0,%%ymm0,%%ymm0"   :: "m"(s8[0*STRIDE])    : "%xmm0" );
   asm ("vinserti128 $1,%0,%%ymm0,%%ymm0"   :: "m"(s8[0*STRIDE+12]) : "%xmm0" );
   asm ("vpshufb     %%ymm15,%%ymm0,%%ymm0" ::                      : "%xmm0" );
   asm ("vpslld      $5*4,%%ymm0,%%ymm14"   ::                      : "%xmm14");
   asm ("vpsrld      $3*4,%%ymm0,%%ymm0"    ::                      : "%xmm0" );
   asm ("vpsrld      $5*4,%%ymm14,%%ymm14"  ::                      : "%xmm14");
   asm ("vpslld      $4*4,%%ymm0,%%ymm0"    ::                      : "%xmm0" );
   asm ("vpor        %%ymm0,%%ymm14,%%ymm0" ::                      : "%xmm0" );
   asm ("vmovupd     %%ymm0,%0"             : "=m"(dst[0*16])      :: "memory");


   //asm ("vmovupd     %0,%%xmm1"             :: "m"(s8[1*STRIDE])    : "%xmm2" );
   asm ("vinserti128 $0,%0,%%ymm1,%%ymm1"   :: "m"(s8[1*STRIDE])    : "%xmm2" );
   asm ("vinserti128 $1,%0,%%ymm1,%%ymm1"   :: "m"(s8[1*STRIDE+12]) : "%xmm2" );
   asm ("vpshufb     %%ymm15,%%ymm1,%%ymm1" ::                      : "%xmm1" );
   asm ("vpslld      $5*4,%%ymm1,%%ymm14"   ::                      : "%xmm14");
   asm ("vpsrld      $3*4,%%ymm1,%%ymm1"    ::                      : "%xmm1" );
   asm ("vpsrld      $5*4,%%ymm14,%%ymm14"  ::                      : "%xmm14");
   asm ("vpslld      $4*4,%%ymm1,%%ymm1"    ::                      : "%xmm1" );
   asm ("vpor        %%ymm1,%%ymm14,%%ymm1" ::                      : "%xmm1" );
   asm ("vmovupd     %%ymm1,%0"             : "=m"(dst[1*16])      :: "memory");


   //asm ("vmovupd     %0,%%xmm2"             :: "m"(s8[2*STRIDE])    : "%xmm2" );
   asm ("vinserti128 $0,%0,%%ymm2,%%ymm2"   :: "m"(s8[2*STRIDE])    : "%xmm2" );
   asm ("vinserti128 $1,%0,%%ymm2,%%ymm2"   :: "m"(s8[2*STRIDE+12]) : "%xmm2" );
   asm ("vpshufb     %%ymm15,%%ymm2,%%ymm2" ::                      : "%xmm2" );
   asm ("vpslld      $5*4,%%ymm2,%%ymm14"   ::                      : "%xmm14");
   asm ("vpsrld      $3*4,%%ymm2,%%ymm2"    ::                      : "%xmm2" );
   asm ("vpsrld      $5*4,%%ymm14,%%ymm14"  ::                      : "%xmm14");
   asm ("vpslld      $4*4,%%ymm2,%%ymm2"    ::                      : "%xmm2" );
   asm ("vpor        %%ymm2,%%ymm14,%%ymm2" ::                      : "%xmm2" );
   asm ("vmovupd     %%ymm2,%0"             : "=m"(dst[2*16])      :: "memory");


   //asm ("vmovupd     %0,%%xmm3"             :: "m"(s8[3*STRIDE])    : "%xmm3" );
   asm ("vinserti128 $0,%0,%%ymm3,%%ymm3"   :: "m"(s8[3*STRIDE])    : "%xmm3" );
   asm ("vinserti128 $1,%0,%%ymm3,%%ymm3"   :: "m"(s8[3*STRIDE+12]) : "%xmm3" );
   asm ("vpshufb     %%ymm15,%%ymm3,%%ymm3" ::                      : "%xmm3" );
   asm ("vpslld      $5*4,%%ymm3,%%ymm14"   ::                      : "%xmm14");
   asm ("vpsrld      $3*4,%%ymm3,%%ymm3"    ::                      : "%xmm3" );
   asm ("vpsrld      $5*4,%%ymm14,%%ymm14"  ::                      : "%xmm14");
   asm ("vpslld      $4*4,%%ymm3,%%ymm3"    ::                      : "%xmm3" );
   asm ("vpor        %%ymm3,%%ymm14,%%ymm3" ::                      : "%xmm3" );
   asm ("vmovupd     %%ymm3,%0"             : "=m"(dst[3*16])      :: "memory");

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static inline void transpose16x8_avx2_kernel (uint16_t       *dst, 
                                              int          stride, 
                                              uint16_t const *src)
{
   /*  ----------------------------------------------------------------- *\ 
    |                                                                    |
    |  NOMENCLATURE                                                      |
    |  ------------                                                      |
    |  The contents are labeled to simulate the channel and time         |
    |  numbering with the channel number being the leading value and the |
    |  time the trailing value.                                          |
    |                                                                    |
    |  Thus 1f = Denotes channel 0x1 for time sample 0xf                 |
    |                                                                    |
    |  GOAL                                                              |
    |  ----                                                              |
    |  The goal is rearrange the order such that the 8 time samples of   |
    |  each channel are contigious in a 128-bit xmm register.            |
    |                                                                    |
    |  This is accomplished by using the unpck lo and hi instructions    |
    |  successively on increasing data widths within a 128 bit xmm       |
    |  register. One would rather do this on the full 256 bit ymm        |
    |  register, but AXV2 confines this instruction (as is the case with |
    |  many others) to only operate within the confines of a 128-bit     |
    |  lane.                                                             |
    |                                                                    |
    |  So the succession is                                              |
    |     16 ->  32 bit ordering                                         |
    |     32 ->  64 bit ordering                                         |
    |     64 -> 128 bit ordering                                         |
    |                                                                    |
   \*  ----------------------------------------------------------------- */



   /* ------------------------------------------------------
    |  BEGIN:  FIRST SET OF 16 -> 32 bit ordering
    |
    |   ymm0 0f 0e 0d 0c 0b 0a 09 08 07 06 05 04 03 02 01 00
    |   ymm1 1f 1f 1d 1c 1b 1a 19 18 17 16 15 14 13 12 11 10
    |
    |   ymm2 1b 0b 1a 0a 19 09 18 08 13 03 12 02 11 01 10 00 
    |   ymm3 1f 0f 1e 0e 1d 0d 1c 0c 17 07 16 06 15 05 14 04
   */
   asm ("vmovapd     %0,%%ymm0"             :: "m"(src[0*16])   : "%xmm0" );
   asm ("vmovapd     %0,%%ymm1"             :: "m"(src[1*16])   : "%xmm1" );
   asm ("vpunpcklwd  %%ymm1,%%ymm0,%%ymm2" ::: "%xmm2");
   asm ("vpunpckhwd  %%ymm1,%%ymm0,%%ymm3" ::: "%xmm3");


   /* 
    |   ymm0 2f 2e 2d 2c 2b 2a 29 28 27 26 25 24 23 22 21 20
    |   ymm1 3f 3e 3d 3c 3b 3a 39 38 37 36 35 34 33 32 31 30
    |
    |   ymm4 3b 2b 3a 2a 39 29 38 28 33 23 32 22 31 21 30 20 
    |   ymm5 3f 2f 3e 2e 3d 2d 3c 2c 37 27 36 26 35 25 34 24
   */
   asm ("vmovapd     %0,%%ymm0"             :: "m"(src[2*16])   : "%xmm0" );
   asm ("vmovapd     %0,%%ymm1"             :: "m"(src[3*16])   : "%xmm1" );
   asm ("vpunpcklwd  %%ymm1,%%ymm0,%%ymm4" ::: "%xmm4");
   asm ("vpunpckhwd  %%ymm1,%%ymm0,%%ymm5" ::: "%xmm5");
   /*
    |  END:   FIRST SET OF 16 -> 32 bit ordering
    |
    |  Active Registers
    |  ----------------
    |   ymm2 1b 0b 1a 0a 19 09 18 08 13 03 12 02 11 01 10 00 
    |   ymm3 1f 0f 1e 0e 1d 0d 1c 0c 17 07 16 06 15 05 14 04
    |   ymm4 3b 2b 3a 2a 39 29 38 28 33 23 32 22 31 21 30 20 
    |   ymm5 3f 2f 3e 2e 3d 2d 3c 2c 37 27 36 26 35 25 34 24
   \* ------------------------------------------------------ */



   /* ------------------------------------------------------
    |  BEGIN: FIRST SET OF 32 -> 64 bit ordering
    |
    |   ymm2 1b 0b 1a 0a 19 09 18 08 13 03 12 02 11 01 10 00  
    |   ymm4 3b 2b 3a 2a 39 29 38 28 33 23 32 22 31 21 30 20 
    |
    |   ymm0 39 29 19 09 38 28 18 08 31 21 11 01 30 20 10 00 
    |   ymm1 3b 2b 1b 0b 3a 2a 1a 0a 33 23 13 03 32 22 12 02
   */
   asm ("vpunpckldq  %%ymm4,%%ymm2,%%ymm0" ::: "%xmm0");
   asm ("vpunpckhdq  %%ymm4,%%ymm2,%%ymm1" ::: "%xmm1");

   /*
    |  
    |   ymm3 1f 0f 1e 0e 1d 0d 1c 0c 17 07 16 06 15 05 14 04
    |   ymm5 3f 2f 3e 2e 3d 2d 3c 2c 37 27 36 26 35 25 34 24
    |
    |   ymm2 3d 2d 1d 0d 3c 2c 1c 0c 35 25 15 05 34 24 14 04
    |   ymm3 3f 2f 1f 0f 3e 23 1e 0e 37 27 17 07 36 26 16 06
   */
   asm ("vpunpckldq  %%ymm5,%%ymm3,%%ymm2" ::: "%xmm2");
   asm ("vpunpckhdq  %%ymm5,%%ymm3,%%ymm3" ::: "%xmm3");
   /*
    |  END:   FIRST SET OF 32 -> 64 bit ordering
    |
    |  Active registers
    |  ---------------- 
    |   ymm0 39 29 19 09 38 28 18 08 31 21 11 01 30 20 10 00 
    |   ymm1 3b 2b 1b 0b 3a 2a 1a 0a 33 23 13 03 32 22 12 02
    |   ymm2 3d 2d 1d 0d 3c 2c 1c 0c 35 25 15 05 34 24 14 04
    |   ymm3 3f 2f 1f 0f 3e 23 1e 0e 37 27 17 07 36 26 16 06
   \* ------------------------------------------------------ */





   /* ------------------------------------------------------
    |  BEGIN:  SECOND SET OF 16 -> 32 bit ordering
    | 
    |   ymm4 4f 4e 4d 4c 4b 4a 49 48 47 46 45 44 43 42 41 40
    |   ymm5 5f 5f 5d 5c 5b 5a 59 58 57 56 55 54 53 52 51 50
    |
    |   ymm6 5b 4b 5a 4a 59 49 58 48 53 43 52 42 51 41 50 40 
    |   ymm7 5f 4f 5e 4e 5d 4d 5c 4c 57 47 56 46 55 45 54 44
   */
   asm ("vmovapd     %0,%%ymm4"             :: "m"(src[4*16])   : "%xmm4" );
   asm ("vmovapd     %0,%%ymm5"             :: "m"(src[5*16])   : "%xmm5" );
   asm ("vpunpcklwd  %%ymm5,%%ymm4,%%ymm6" ::: "%xmm6");
   asm ("vpunpckhwd  %%ymm5,%%ymm4,%%ymm7" ::: "%xmm7");


   /* 
    |   ymm4 6f 6e 6d 6c 6b 6a 69 68 67 66 65 64 63 62 61 60
    |   ymm5 7f 7e 7d 7c 7b 7a 79 78 77 76 75 74 73 72 71 70
    |
    |   ymm8 7b 6b 7a 6a 79 69 78 68 73 63 72 62 71 61 70 60 
    |   ymm9 7f 6f 7e 6e 7d 6d 7c 6c 77 67 76 66 75 65 74 64 
   */
   asm ("vmovapd     %0,%%ymm4"             :: "m"(src[6*16])   : "%xmm4" );
   asm ("vmovapd     %0,%%ymm5"             :: "m"(src[7*16])   : "%xmm5" );
   asm ("vpunpcklwd  %%ymm5,%%ymm4,%%ymm8" ::: "%xmm8");
   asm ("vpunpckhwd  %%ymm5,%%ymm4,%%ymm9" ::: "%xmm5");
   /*
    |  END:   SECOND SET OF 16 -> 32 bit ordering
    |
    |  Active registers
    |  ---------------- 
    |   ymm6 5b 4b 5a 4a 59 49 58 48 53 43 52 42 51 41 50 40 
    |   ymm7 5f 4f 5e 4e 5d 4d 5c 4c 57 47 56 46 55 45 54 44
    |   ymm8 7b 6b 7a 6a 79 69 78 68 73 63 72 62 71 61 70 60 
    |   ymm9 7f 6f 7e 6e 7d 6d 7c 6c 77 67 76 66 75 65 74 64 
   \* ------------------------------------------------------ */



   /* ------------------------------------------------------
    |  BEGIN SECOND SET OF 32 -> 64 bit ordering
    |
    |   ymm6 5b 4b 5a 4a 59 49 58 48 53 43 52 42 51 41 50 40 
    |   ymm8 7b 6b 7a 6a 79 69 78 68 73 63 72 62 71 61 70 60
    |
    |   ymm4 79 69 59 49 78 68 58 48 71 61 51 41 70 60 50 40
    |   ymm5 7b 6b 5b 4b 7a 6a 5a 4a 73 63 53 43 72 62 52 42
   */
   asm ("vpunpckldq  %%ymm8,%%ymm6,%%ymm4" ::: "%xmm4");
   asm ("vpunpckhdq  %%ymm8,%%ymm6,%%ymm5" ::: "%xmm5");

   /*
    |   ymm7 5f 4f 5e 4e 5d 4d 5c 4c 57 47 56 46 55 45 54 44
    |   ymm9 7f 6f 7e 6e 7d 6d 7c 6c 77 67 76 66 75 65 74 64
    |
    |   ymm6 7d 6d 5d 4d 7c 6c 5c 4c 75 65 55 45 74 64 54 44
    |   ymm7 7f 6f 6f 4f 7e 6e 5e 4e 77 67 57 47 76 66 56 46
   */
   asm ("vpunpckldq  %%ymm9,%%ymm7,%%ymm6" ::: "%xmm6");
   asm ("vpunpckhdq  %%ymm9,%%ymm7,%%ymm7" ::: "%xmm7");
   /*
    |   ymm4 79 66 59 49 78 68 58 48 71 61 51 41 70 60 50 40
    |   ymm5 7b 6b 5b 4b 7a 6a 5a 4a 73 63 53 43 72 62 52 42
    |   ymm6 7d 6d 5d 4d 7c 6c 5c 4c 75 65 55 45 74 64 54 44
    |   ymm7 7f 6f 6f 4f 7e 6e 5e 4e 77 67 57 47 76 66 56 46
    |
    |  END SECOND SET OF 32 - 64-bit ordering
   \* ------------------------------------------------------ */




   /* ------------------------------------------------------
    |  BEGIN 64 - 128 bit ordering
    |
    |  Active registers
    |  ----------------
    |   ymm0 39 29 19 09 38 28 18 08 31 21 11 01 30 20 10 00 
    |   ymm1 3b 2b 1b 0b 3a 2a 1a 0a 33 23 13 03 32 22 12 02
    |   ymm2 3d 2d 1d 0d 3c 2c 1c 0c 35 25 15 05 34 24 14 04
    |   ymm3 3f 2f 1f 0f 3e 23 1e 0e 37 27 17 07 36 26 16 06
    |
    |   ymm4 79 66 59 49 78 68 58 48 71 61 51 41 70 60 50 40
    |   ymm5 7b 6b 5b 4b 7a 6a 5a 4a 73 63 53 43 72 62 52 42
    |   ymm6 7d 6d 5d 4d 7c 6c 5c 4c 75 65 55 45 74 64 54 44
    |   ymm7 7f 6f 6f 4f 7e 6e 5e 4e 77 67 57 47 76 66 56 46
    |  ----------------
    |
    |
    |   ymm0 39 29 19 09 38 28 18 08 31 21 11 01 30 20 10 00
    |   ymm4 79 69 59 49 78 68 58 48 71 61 51 41 70 60 50 40
    |
    |   ymm8 78 68 58 48 38 28 18 08 70 60 50 40 30 20 10 00
    |   ymm9 79 69 59 49 39 29 19 09 71 61 51 41 31 21 11 01
   */
   asm ("vpunpcklqdq %%ymm4,%%ymm0,%%ymm8" ::: "%xmm8");
   asm ("vpunpckhqdq %%ymm4,%%ymm0,%%ymm9" ::: "%xmm9");

// asm ("vmovapd     %0,%%xmm8"        : "=m"(dst[0x0*stride]));
   asm ("vextracti128 $0,%%ymm8,%0"    : "=m"(dst[0x0*stride]));
   asm ("vextracti128 $1,%%ymm8,%0"    : "=m"(dst[0x8*stride]));

// asm ("vmovapd     %0,%%xmm9"        : "=m"(dst[0x1*stride]));
   asm ("vextracti128 $0,%%ymm9,%0"    : "=m"(dst[0x1*stride]));
   asm ("vextracti128 $1,%%ymm9,%0"    : "=m"(dst[0x9*stride]));




   /*
    |   ymm1 3b 2b 1b 0b 3a 2a 1a 0a 33 23 13 03 32 22 12 02
    |   ymm5 7b 6b 5b 4b 7a 6a 5a 4a 73 63 53 43 72 62 52 42
    |
    |   ymm8 7a 6a 5a 4a 3a 2a 1a 0a 72 62 52 42 32 22 12 02
    |   ymm9 7b 6b 5b 4b 3b 2b 1b 0b 73 63 53 43 33 23 13 03
   */
   asm ("vpunpcklqdq %%ymm5,%%ymm1,%%ymm8" ::: "%xmm8");
   asm ("vpunpckhqdq %%ymm5,%%ymm1,%%ymm9" ::: "%xmm9");

// asm ("vmovapd     %0,%%xmm8"        : "=m"(dst[0x2*stride]));
   asm ("vextracti128 $0,%%ymm8,%0"    : "=m"(dst[0x2*stride]));
   asm ("vextracti128 $1,%%ymm8,%0"    : "=m"(dst[0xA*stride]));

// asm ("vmovapd     %0,%%xmm9"        : "=m"(dst[0x3*stride]));
   asm ("vextracti128 $0,%%ymm9,%0"    : "=m"(dst[0x3*stride]));
   asm ("vextracti128 $1,%%ymm9,%0"    : "=m"(dst[0xB*stride]));

        
   /*
    |   ymm2 3d 2d 1d 0d 3c 2c 1c 0c 35 25 15 05 34 24 14 04
    |   ymm6 7d 6d 5d 4d 7c 6c 5c 4c 75 65 55 45 74 64 54 44
    |
    |   ymm8 7c 6c 5c 4c 3c 2c 1c 0c 74 64 54 44 34 24 14 04
    |   ymm9 7d 6d 5d 4d 3d 2d 1d 0d 75 65 55 45 35 25 15 05
    */
   asm ("vpunpcklqdq %%ymm6,%%ymm2,%%ymm8" ::: "%xmm8");
   asm ("vpunpckhqdq %%ymm6,%%ymm2,%%ymm9" ::: "%xmm9");

// asm ("vmovapd     %0,%%xmm8"        : "=m"(dst[0x4*stride]));
   asm ("vextracti128 $0,%%ymm8,%0"    : "=m"(dst[0x4*stride]));
   asm ("vextracti128 $1,%%ymm8,%0"    : "=m"(dst[0xC*stride]));

// asm ("vmovapd     %0,%%xmm9"        : "=m"(dst[0x5*stride]));
   asm ("vextracti128 $0,%%ymm9,%0"    : "=m"(dst[0x5*stride]));
   asm ("vextracti128 $1,%%ymm9,%0"    : "=m"(dst[0xD*stride]));

   
   /*
    |   ymm3 3f 2f 1f 0f 3e 23 1e 0e 37 27 17 07 36 26 16 06
    |   ymm7 7f 6f 6f 4f 7e 6e 5e 4e 77 67 57 47 76 66 56 46
    |
    |   ymm8 7e 6e 5e 4e 3e 2e 1e 0e 76 66 56 46 36 26 16 06
    |   ymm9 7f 6f 5f 4f 3f 2f 1f 0f 77 67 57 47 37 27 17 07
   */
   asm ("vpunpcklqdq %%ymm7,%%ymm3,%%ymm8" ::: "%xmm8");
   asm ("vpunpckhqdq %%ymm7,%%ymm3,%%ymm9" ::: "%xmm9");

// asm ("vmovapd     %0,%%xmm8"        : "=m"(dst[0x6*stride]));
   asm ("vextracti128 $0,%%ymm8,%0"    : "=m"(dst[0x6*stride]));
   asm ("vextracti128 $1,%%ymm8,%0"    : "=m"(dst[0xE*stride]));

// asm ("vmovapd     %0,%%xmm9"        : "=m"(dst[0x7*stride]));
   asm ("vextracti128 $0,%%ymm9,%0"    : "=m"(dst[0x7*stride]));
   asm ("vextracti128 $1,%%ymm9,%0"    : "=m"(dst[0xF*stride]));
   /*
    |  END 64 - 128 bit ordering
   \* ------------------------------------------------------ */

   /*
   printf ("Stride = %d\n", stride);
   for (int idx = 0; idx < 8; idx += 1)
   {
      
      printf ("dst[%4.4x] = "
              " %4.4" PRIx16 " %4.4" PRIx16" %4.4" PRIx16" %4.4" PRIx16 ""
              " %4.4" PRIx16 " %4.4" PRIx16" %4.4" PRIx16" %4.4" PRIx16 ""
              " %4.4" PRIx16 " %4.4" PRIx16" %4.4" PRIx16" %4.4" PRIx16 ""
              " %4.4" PRIx16 " %4.4" PRIx16" %4.4" PRIx16" %4.4" PRIx16 "\n",
              idx,
              dst[idx + 0*stride], dst[idx + 1*stride], 
              dst[idx + 2*stride], dst[idx + 3*stride], 
              dst[idx + 4*stride], dst[idx + 5*stride], 
              dst[idx + 6*stride], dst[idx + 7*stride],
              dst[idx + 8*stride], dst[idx + 9*stride],
              dst[idx +10*stride], dst[idx +11*stride],
              dst[idx +12*stride], dst[idx +13*stride],
              dst[idx +14*stride], dst[idx +15*stride]);
   }
   */

   return;

}
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief Convenience method to expand 16 channels x 4N time slots.

  \param[in] buf  The buffer to receive the ADCS
  \param[in]  n8  The number of groups of 8 timesamples to expand, 
                  \e i.e. the N in expand32Nx4_avx2.  
  \param[in] src  The source of adcs.  

                                                                          */
/* ---------------------------------------------------------------------- */
static inline void expandAdcs16x8N_avx2 (uint16_t        *buf,
                                         int               n8,
                                         uint64_t const  *src)
{
   #define TIMESAMPLE_STRIDE (4 * STRIDE / sizeof (*src))

   //printf ("Timesample_Stride = %u\n", (unsigned)TIMESAMPLE_STRIDE);

   for (int idx = 0; idx < n8; ++idx)
   {
      expandAdcs16x4_avx2_kernel (buf, src);
      buf += 64;
      src += TIMESAMPLE_STRIDE;

      expandAdcs16x4_avx2_kernel (buf, src);
      buf += 64;
      src += TIMESAMPLE_STRIDE;

   }

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Transpose 8N time samples for 16 channels

  \param[out]   dst The destination array
  \param[ in]    n8 The number of groups of 8 channels, \e i.e. the N in
                    transpose16x8N_avx2
  \param[in] offset The number of elements in on channel's destination
                    array.
  \param[in]    src The source array
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void transpose16x8N_avx2_kernel (uint16_t       *dst, 
                                               int              n8,
                                               int          stride,
                                               uint16_t const *src)
{
   for (int idx = 0; idx < n8; ++idx)
   {
      transpose16x8_avx2_kernel (dst, stride, src + idx * 128);
      dst += 8;
   }

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
static inline void transpose16x8_avx2 (uint16_t       *dst, 
                                       int          stride, 
                                       uint64_t const *src)
{
   uint16_t buf[16*8] __attribute__ ((aligned (32)));

   expandAdcs16x8N_avx2 (buf, 1, src);

   /*
   for (int idx = 0; idx < 128; ++idx)
   {
      if ((idx & 0xf) == 0) printf ("%2.2x:", idx);
      printf (" %4.4x", buf[idx]);
      if ((idx & 0xf) == 0xf) putchar ('\n');
   }
   putchar ('\n');
   */


   transpose16x8_avx2_kernel (dst, stride, buf);
}
/* ---------------------------------------------------------------------- */






/* ---------------------------------------------------------------------- */
static inline void transpose16x16_avx2 (uint16_t       *dst, 
                                        int          stride, 
                                        uint64_t const *src)
{
   #define TIMESAMPLE_STRIDE (4 * STRIDE / sizeof (*src))
   uint16_t buf[16*16]  __attribute__ ((aligned (32)));

   expandAdcs16x8N_avx2 (buf, 2, src);


   /*
   for (int idx = 0; idx < 128; ++idx)
   {
      if ((idx & 0xf) == 0) printf ("%2.2x:", idx);
      printf (" %4.4x", buf[idx]);
      if ((idx & 0xf) == 0xf) putchar ('\n');
   }
   putchar ('\n');
   */

   
   transpose16x8N_avx2_kernel (dst, 2, stride, buf);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static inline void transpose16x32_avx2 (uint16_t       *dst, 
                                        int          stride, 
                                        uint64_t const *src)
{
   #define TIMESAMPLE_STRIDE (4 * STRIDE / sizeof (*src))
   uint16_t buf[16*32]  __attribute__ ((aligned (32)));

   expandAdcs16x8N_avx2       (buf, 4, src);
   transpose16x8N_avx2_kernel (dst, 4, stride, buf);
}
/* ---------------------------------------------------------------------- */
/* END: CONTIGIOUS TRANSPOSITION                                          */
/* ====================================================================== */




/* ====================================================================== */
/* CHANNEL-BY-CHANNEL TRANSPOSITION                                       */
/* ---------------------------------------------------------------------- */
static inline void transpose16x8_avx2_kernel (uint16_t  *dst[128], 
                                              int          offset, 
                                              uint16_t const *src)
{
   /* ------------------------------------------------------------------- *\
    | For a discription of the algorithm, see the kernel for contigious   |
    | memory
   \* ------------------------------------------------------------------- */



   /* ------------------------------------------------------ *\
    | BEGIN: FIRST SET OF 16 -> 32 bit ordering
   */
   asm ("vmovapd     %0,%%ymm0"             :: "m"(src[0*16])   : "%xmm0" );
   asm ("vmovapd     %0,%%ymm1"             :: "m"(src[1*16])   : "%xmm1" );
   asm ("vpunpcklwd  %%ymm1,%%ymm0,%%ymm2" ::: "%xmm2");
   asm ("vpunpckhwd  %%ymm1,%%ymm0,%%ymm3" ::: "%xmm3");


   asm ("vmovapd     %0,%%ymm0"             :: "m"(src[2*16])   : "%xmm0" );
   asm ("vmovapd     %0,%%ymm1"             :: "m"(src[3*16])   : "%xmm1" );
   asm ("vpunpcklwd  %%ymm1,%%ymm0,%%ymm4" ::: "%xmm4");
   asm ("vpunpckhwd  %%ymm1,%%ymm0,%%ymm5" ::: "%xmm5");
   /*
    |  END:   FIRST SET OF 16 -> 32 bit ordering
   \* ------------------------------------------------------ */


   /* ------------------------------------------------------ *\
    |  BEGIN:  FIRST SET OF 32 -> 64 bit ordering
   */
   asm ("vpunpckldq  %%ymm4,%%ymm2,%%ymm0" ::: "%xmm0");
   asm ("vpunpckhdq  %%ymm4,%%ymm2,%%ymm1" ::: "%xmm1");

   asm ("vpunpckldq  %%ymm5,%%ymm3,%%ymm2" ::: "%xmm2");
   asm ("vpunpckhdq  %%ymm5,%%ymm3,%%ymm3" ::: "%xmm3");
   /*
    |  END:   FIRST SET OF 32 -> 64 bit ordering
   \* ------------------------------------------------------ */





   /* ------------------------------------------------------
    |  BEGIN:  SECOND SET OF 16 -> 32 bit ordering
   */
   asm ("vmovapd     %0,%%ymm4"             :: "m"(src[4*16])   : "%xmm4" );
   asm ("vmovapd     %0,%%ymm5"             :: "m"(src[5*16])   : "%xmm5" );
   asm ("vpunpcklwd  %%ymm5,%%ymm4,%%ymm6" ::: "%xmm6");
   asm ("vpunpckhwd  %%ymm5,%%ymm4,%%ymm7" ::: "%xmm7");


   asm ("vmovapd     %0,%%ymm4"             :: "m"(src[6*16])   : "%xmm4" );
   asm ("vmovapd     %0,%%ymm5"             :: "m"(src[7*16])   : "%xmm5" );
   asm ("vpunpcklwd  %%ymm5,%%ymm4,%%ymm8" ::: "%xmm8");
   asm ("vpunpckhwd  %%ymm5,%%ymm4,%%ymm9" ::: "%xmm5");
   /*
    |  END:   SECOND SET OF 16 -> 32 bit ordering
   \* ------------------------------------------------------ */



   /* ------------------------------------------------------
    |  BEGIN SECOND SET OF 32 -> 64 bit ordering
   */
   asm ("vpunpckldq  %%ymm8,%%ymm6,%%ymm4" ::: "%xmm4");
   asm ("vpunpckhdq  %%ymm8,%%ymm6,%%ymm5" ::: "%xmm5");

   asm ("vpunpckldq  %%ymm9,%%ymm7,%%ymm6" ::: "%xmm6");
   asm ("vpunpckhdq  %%ymm9,%%ymm7,%%ymm7" ::: "%xmm7");
   /*
    |  END SECOND SET OF 32 - 64-bit ordering
   \* ------------------------------------------------------ */




   /* ------------------------------------------------------
    |  BEGIN 64 - 128 bit ordering
   */
   asm ("vpunpcklqdq %%ymm4,%%ymm0,%%ymm8" ::: "%xmm8");
   asm ("vpunpckhqdq %%ymm4,%%ymm0,%%ymm9" ::: "%xmm9");

   //asm ("vmovapd     %0,%%xmm8"         : "=m"(dst[0x0][offset]));
   asm ("vextracti128 $0,%%ymm8,%0"    : "=m"(dst[0x0][offset]));
   asm ("vextracti128 $1,%%ymm8,%0"    : "=m"(dst[0x8][offset]));

   //asm ("vmovapd     %0,%%xmm9"         : "=m"(dst[0x1][offset]));
   asm ("vextracti128 $0,%%ymm9,%0"    : "=m"(dst[0x1][offset]));
   asm ("vextracti128 $1,%%ymm9,%0"    : "=m"(dst[0x9][offset]));


   /*
    |   ymm1 3b 2b 1b 0b 3a 2a 1a 0a 33 23 13 03 32 22 12 02
    |   ymm5 7b 6b 5b 4b 7a 6a 5a 4a 73 63 53 43 72 62 52 42
    |
    |   ymm8 7a 6a 5a 4a 3a 2a 1a 0a 72 62 52 42 32 22 12 02
    |   ymm9 7b 6b 5b 4b 3b 2b 1b 0b 73 63 53 43 33 23 13 03
   */
   asm ("vpunpcklqdq %%ymm5,%%ymm1,%%ymm8" ::: "%xmm8");
   asm ("vpunpckhqdq %%ymm5,%%ymm1,%%ymm9" ::: "%xmm9");

   //asm ("vmovapd     %0,%%xmm8"        : "=m"(dst[0x2][offset]));
   asm ("vextracti128 $0,%%ymm8,%0"    : "=m"(dst[0x2][offset]));
   asm ("vextracti128 $1,%%ymm8,%0"    : "=m"(dst[0xA][offset]));

   //asm ("vmovapd     %0,%%xmm9"        : "=m"(dst[0x3][offset]));
   asm ("vextracti128 $0,%%ymm9,%0"    : "=m"(dst[0x3][offset]));
   asm ("vextracti128 $1,%%ymm9,%0"    : "=m"(dst[0xB][offset]));

        
   /*
    |   ymm2 3d 2d 1d 0d 3c 2c 1c 0c 35 25 15 05 34 24 14 04
    |   ymm6 7d 6d 5d 4d 7c 6c 5c 4c 75 65 55 45 74 64 54 44
    |
    |   ymm8 7c 6c 5c 4c 3c 2c 1c 0c 74 64 54 44 34 24 14 04
    |   ymm9 7d 6d 5d 4d 3d 2d 1d 0d 75 65 55 45 35 25 15 05
    */
   asm ("vpunpcklqdq %%ymm6,%%ymm2,%%ymm8" ::: "%xmm8");
   asm ("vpunpckhqdq %%ymm6,%%ymm2,%%ymm9" ::: "%xmm9");

   //asm ("vmovapd     %0,%%xmm8"        : "=m"(dst[0x4][offset]));
   asm ("vextracti128 $0,%%ymm8,%0"    : "=m"(dst[0x4][offset]));
   asm ("vextracti128 $1,%%ymm8,%0"    : "=m"(dst[0xC][offset]));

   //asm ("vmovapd     %0,%%xmm9"        : "=m"(dst[0x5][offset]));
   asm ("vextracti128 $0,%%ymm9,%0"    : "=m"(dst[0x5][offset]));
   asm ("vextracti128 $1,%%ymm9,%0"    : "=m"(dst[0xD][offset]));

   
   /*
    |   ymm3 3f 2f 1f 0f 3e 23 1e 0e 37 27 17 07 36 26 16 06
    |   ymm7 7f 6f 6f 4f 7e 6e 5e 4e 77 67 57 47 76 66 56 46
    |
    |   ymm8 7e 6e 5e 4e 3e 2e 1e 0e 76 66 56 46 36 26 16 06
    |   ymm9 7f 6f 5f 4f 3f 2f 1f 0f 77 67 57 47 37 27 17 07
   */
   asm ("vpunpcklqdq %%ymm7,%%ymm3,%%ymm8" ::: "%xmm8");
   asm ("vpunpckhqdq %%ymm7,%%ymm3,%%ymm9" ::: "%xmm9");

   //asm ("vmovapd     %0,%%xmm8"        : "=m"(dst[0x6][offset]));
   asm ("vextracti128 $0,%%ymm8,%0"    : "=m"(dst[0x6][offset]));
   asm ("vextracti128 $1,%%ymm8,%0"    : "=m"(dst[0xE][offset]));

   //asm ("vmovapd     %0,%%xmm9"        : "=m"(dst[0x7][offset]));
   asm ("vextracti128 $0,%%ymm9,%0"    : "=m"(dst[0x7][offset]));
   asm ("vextracti128 $1,%%ymm9,%0"    : "=m"(dst[0xF][offset]));
   /*
    |  END 64 - 128 bit ordering
   \* ------------------------------------------------------ */

   return;

}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Transpose 8N time samples for 16 channels

  \param[out]   dst Pointers to 16 arrays to receive the transposed data
  \param[ in]    n8 The number of groups of 8 channels, \e i.e. the N in
                    transpose16x8N_avx2
  \param[in] offset The offset in the array to store the first transposed
                    ADC in
  \param[in]    src The source array
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void transpose16x8N_avx2_kernel (uint16_t   *dst[16], 
                                               int              n8,
                                               int          offset,
                                               uint16_t const *src)
{
   for (int idx = 0; idx < n8; ++idx)
   {
      transpose16x8_avx2_kernel (dst, offset, src + idx * 128);
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static inline void transpose16x8_avx2 (uint16_t    *dst[16], 
                                       int          offset,
                                       uint64_t const *src)
{
   uint16_t buf[16*8] __attribute__ ((aligned (32)));

   expandAdcs16x8N_avx2       (buf, 1,         src);
   transpose16x8N_avx2_kernel (dst, 1, offset, buf);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static inline void transpose16x16_avx2 (uint16_t   *dst[16], 
                                        int          offset,
                                        uint64_t const *src)
{
   uint16_t buf[16*16] __attribute__ ((aligned (32)));

   expandAdcs16x8N_avx2       (buf, 2,        src);
   transpose16x8N_avx2_kernel (dst, 2, offset, buf);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static inline void transpose16x32_avx2 (uint16_t   *dst[16], 
                                        int          offset, 
                                        uint64_t const *src)
{
   uint16_t buf[16*32] __attribute__ ((aligned (32)));

   expandAdcs16x8N_avx2      (buf, 4,         src);
   transpose16x8N_avx2_kernel (dst, 4, offset, buf);
}
/* ---------------------------------------------------------------------- */
/* END: CONTIGIOUS TRANSPOSITION                                          */
/* ====================================================================== */


/* ---------------------------------------------------------------------- *//*!

  \brief The kernel to unpack 64 densely packet 12-bit values into 
         64 16-bit values.

  \param[in] dst  The destination address
  \param[in] src  The source address

  \warning
   This routine assumes that the SIMD registers ymm0 and ymm1 are
   initialized to correctly left justify the 18-bit values in the even
   and odd words within a 32-bit lane. 
                                                                          */
/* ---------------------------------------------------------------------- */
void WibFrame::ColdData::expandAdcs64x1 (uint16_t             *dst,
                                         uint64_t const (&src)[12])
{
   expandAdcs16_avx2_init     ();
   expandAdcs64x1_avx2_kernel (dst, reinterpret_cast<uint64_t const *>(&src));
}
/* ---------------------------------------------------------------------- */
/*   END: IMPLEMENTATION: class WibFrame                                  */
} /* END: namespace fragment                                              */
} /* END: namespace pdd                                                   */
/* ====================================================================== */
