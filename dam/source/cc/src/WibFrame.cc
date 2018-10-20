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
   2017.10.05 jjr Added transposeAdcs128xN. These are methods that optimize
                  the transpose for an arbitrary number of frames.  
                  NOTE: Current implementation still limits this to
                        a multiple of 8.
   2017.08.28 jjr Split from WibFrame.hh
   2017.08.16 jjr Cloned from RCE firmware version
   ---------- --- --------------------------------
   2017.04.19 jjr Updated to the format Eric Hazen published on 2017.03.13
   2016.10.18 jjr Corrected K28.5 definition, was 0xDC -> 0xBC
   2016.06.14 jjr Created

\* ---------------------------------------------------------------------- */


#include "dam/access/WibFrame.hh"
#include <cinttypes>
#include <cstdio>


namespace pdd    {
namespace access {


static inline void expandAdcs16_init_kernel () __attribute__ ((always_inline));

// ------------------------------
// EXPANDERS:   Contigious Memory
// ------------------------------
static inline void expandAdcs16x1_kernel  (int16_t        *dst,
                                           uint64_t const *src) __attribute__ ((always_inline));

static inline void expandAdcs64x1_kernel  (int16_t        *dst, 
                                           uint64_t const *src) __attribute__ ((always_inline));

static inline void expandAdcs16x4_kernel  (int16_t        *dst, 
                                           uint64_t const *src) __attribute__ ((always_inline));


// ------------------------------
// TRANSPOSERS: Contigious Memory
// ------------------------------
static inline void transposeAdcs128xN_kernel (int16_t              *dst,
                                              int            ndstStride,
                                              WibFrame const    *frames,
                                              int               nframes) __attribute__ ((always_inline));

static inline void transposeAdcs16x8_kernel  (int16_t        *dst, 
                                              int          offset, 
                                              uint64_t const *src) __attribute__ ((always_inline));

static inline void transposeAdcs16x8_kernel  (int16_t        *dst, 
                                              int          offset, 
                                              uint64_t const *src) __attribute__ ((always_inline));

static inline void transposeAdcs16x8N_kernel (int16_t        *dst, 
                                              int              n8,
                                              int          stride, 
                                              uint64_t const *src) __attribute__ ((always_inline));

static inline void transposeAdcs16x16_kernel (int16_t        *dst, 
                                              int          offset, 
                                              uint64_t const *src) __attribute__ ((always_inline));

static inline void transposeAdcs16x32_kernel (int16_t        *dst, 
                                              int          offset, 
                                              uint64_t const *src) __attribute__ ((always_inline));



// --------------------------------------
// TRANSPOSERS: Channel-by-Channel Memory
// --------------------------------------
static inline void transposeAdcs128xN_kernel (int16_t *const       *dst,
                                              WibFrame const    *frames,
                                              int               nframes) __attribute__ ((always_inline));

static inline void transposeAdcs16x8_kernel  (int16_t  *const *dst, 
                                              int           offset, 
                                              uint64_t const  *src) __attribute__ ((always_inline));

static inline void transposeAdcs16x8N_kernel (int16_t  *const *dst, 
                                              int               n8,
                                              int           offset, 
                                              uint64_t const  *src) __attribute__ ((always_inline));

static inline void transposeAdcs16x16_kernel (int16_t  *const *dst, 
                                              int           offset, 
                                              uint64_t  const *src) __attribute__ ((always_inline));

static inline void transposeAdcs16x32_kernel (int16_t  *const *dst, 
                                              int           offset, 
                                              uint64_t  const *src) __attribute__ ((always_inline));
     


/* ---------------------------------------------------------------------- *//*!

   \brief  Expand all 128 12-bit Adcs associated withthis WibFrame to 
           16 bits.
 
   \param[out] dst A pointer to an array large enough to hold 128 16-bit 
                   adcs.
                                                                          */
/* ---------------------------------------------------------------------- */
void  WibFrame::expandAdcs128x1 (int16_t *dst) const
{
   WibColdData const (& coldData)[2] = getColdData ();
   coldData[0].expandAdcs64x1 (dst + 0*pdd::record::WibColdData::NAdcs);
   coldData[1].expandAdcs64x1 (dst + 1*pdd::record::WibColdData::NAdcs);
   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

   \brief  Expand all 128 channels 12-bit -< 16 bits.
 
   \param[out] dst A pointer to an array large enough to hold 128 16-bit 
                   adcs.
                                                                          */
/* ---------------------------------------------------------------------- */
void  WibFrame::expandAdcs128xN (int16_t           *dst, 
                                 WibFrame const *frames, 
                                 int            nframes)
{
   for (int iframe = 0; iframe < nframes; ++iframe)
   {
      frames[iframe].expandAdcs128x1 (dst);
      dst += 2*pdd::record::WibColdData::NAdcs;
   }
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief Transposes the 128 ADC channels serviced by a WibFrame for
          \a nframes time samples.

   \param[in]       dst[out]  Array of pointers to the channel-by-channel
                              destination arrays 
   \param[in] ndstStride[in]  The number of entries of each of the 128
   \param[in]     frames[in]  The array of WibFrames
   \param[in]    nframes[in]  The number frames, \e i.e. time samples
                              to transpose.  This must a multiple of 16.
                                                                          */
/* ---------------------------------------------------------------------- */
void WibFrame::transposeAdcs128xN  (int16_t              *dst,
                                    int            ndstStride,
                                    WibFrame  const   *frames,
                                    int               nframes)
{
   transposeAdcs128x32N (dst, ndstStride, frames, nframes);

   // -----------------------------
   // Pickup the remainder
   // Still must be a multiple of 8
   // -----------------------------
   int mframes = nframes & 0x1f; 
   if (mframes)
   {
      int n32  = nframes & ~0x1f;
      dst     += n32;
      frames  += n32;
      transposeAdcs128x8N (dst, ndstStride, frames, mframes);
   }


   // Get any remaining frames (this must be less than 8) 
   int rframes = mframes & 0x7;
   if (rframes)
   {
      dst    += mframes - rframes;
      frames += mframes - rframes;
      transposeAdcs128xN_kernel (dst, ndstStride, frames, rframes);
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
void WibFrame::transposeAdcs128x8N (int16_t              *dst,
                                    int            ndstStride,
                                    WibFrame const    *frames,
                                    int               nframes)
{
   //puts ("transposeAcs128x16");


   // ----------------------------------
   // Locate the cold data in this frame
   // ----------------------------------
   WibColdData const (& coldData)[2] = frames->getColdData ();

   // ------------------------------------------------
   // Locate the packed data for each cold data stream
   // ------------------------------------------------
   uint64_t const *src0 = coldData[0].locateAdcs12b ();
   uint64_t const *src1 = coldData[1].locateAdcs12b ();


   // ----------------------------------------------------------------
   // Locate where in the output data for the 2 cold data streams goes
   // ----------------------------------------------------------------
   int16_t *dst0 = dst;
   int16_t *dst1 = dst + 64 * ndstStride;
   int n8frames  = nframes/8;


   // ---------------------------------
   // Initialize the expander registers
   // ---------------------------------
   expandAdcs16_init_kernel ();

   // ------------------------------=-----
   // Loop over the frames in groups of 8
   // ------------------------------------
   for (int iframe = 0; iframe < n8frames; ++iframe)
   { 
      uint64_t const *lclsrc0 = src0;
      uint64_t const *lclsrc1 = src1;

      int16_t        *lcldst0 = dst0;
      int16_t        *lcldst1 = dst1;


      // ----------------------------------
      // Loop over the adcs in groups of 16
      // ----------------------------------
      for (int iadcs = 0; iadcs < 64; iadcs += 16)
      {
         // ----------------------------------------------------------------
         // Transpose the cold data stream 0 & 1  for 16 channels x 16 times
         // ----------------------------------------------------------------
         transposeAdcs16x8_kernel (lcldst0, ndstStride, lclsrc0);
         lcldst0 += 16*ndstStride;
         lclsrc0 +=  3;

         transposeAdcs16x8_kernel (lcldst1, ndstStride, lclsrc1);
         lcldst1 += 16*ndstStride;
         lclsrc1 +=  3;
      }

      // Advance the source and destination by the 8 time frames
      src0 += 8 * sizeof (WibFrame) / sizeof (*src0);
      src1 += 8 * sizeof (WibFrame) / sizeof (*src1);

      // Advance the destination by the same 
      dst0 += 8;
      dst1 += 8;
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
void WibFrame::transposeAdcs128x16N (int16_t              *dst,
                                     int            ndstStride,
                                     WibFrame const    *frames,
                                     int               nframes)
{
   //puts ("transposeAcs128x16");

   // ----------------------------------
   // Locate the cold data in this frame
   // ----------------------------------
   WibColdData const (& coldData)[2] = frames->getColdData ();


   // ------------------------------------------------
   // Locate the packed data for each cold data stream
   // ------------------------------------------------
   uint64_t const *src0 = coldData[0].locateAdcs12b ();
   uint64_t const *src1 = coldData[1].locateAdcs12b ();


   // ----------------------------------------------------------------
   // Locate where in the output data for the 2 cold data streams goes
   // ----------------------------------------------------------------
   int16_t *dst0 = dst;
   int16_t *dst1 = dst + 64 * ndstStride;


   int n16frames = nframes/16;


   // ---------------------------------
   // Initialize the expander registers
   // ---------------------------------
   expandAdcs16_init_kernel ();

   // ------------------------------=-----
   // Loop over the frames in groups of 16
   // ------------------------------------
   for (int iframe = 0; iframe < n16frames; ++iframe)
   { 
      uint64_t const *lclsrc0 = src0;
      uint64_t const *lclsrc1 = src1;

      int16_t        *lcldst0 = dst0;
      int16_t        *lcldst1 = dst1;


      // ----------------------------------
      // Loop over the adcs in groups of 16
      // ----------------------------------
      for (int iadcs = 0; iadcs < 64; iadcs += 16)
      {
         // ----------------------------------------------------------------
         // Transpose the cold data stream 0 & 1  for 16 channels x 16 times
         // ----------------------------------------------------------------
         transposeAdcs16x16_kernel (lcldst0, ndstStride, lclsrc0);
         lcldst0 += 16*ndstStride;
         lclsrc0 +=  3;

         transposeAdcs16x16_kernel (lcldst1, ndstStride, lclsrc1);
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
void WibFrame::transposeAdcs128x32N (int16_t              *dst,
                                     int            ndstStride,
                                     WibFrame const    *frames,
                                     int               nframes)
{
   // ----------------------------------
   // Locate the cold data in this frame
   // ----------------------------------
   WibColdData const (& coldData)[2] = frames->getColdData ();


   // ------------------------------------------------
   // Locate the packed data for each cold data stream
   // ------------------------------------------------
   uint64_t const *src0 = coldData[0].locateAdcs12b ();
   uint64_t const *src1 = coldData[1].locateAdcs12b ();


   // ----------------------------------------------------------------
   // Locate where in the output data for the 2 cold data streams goes
   // ----------------------------------------------------------------
   int16_t *dst0 = dst;
   int16_t *dst1 = dst + 64 * ndstStride;


   int n32frames = nframes/32;


   // ---------------------------------
   // Initialize the expander registers
   // ---------------------------------
   expandAdcs16_init_kernel ();

   // ------------------------------=-----
   // Loop over the frames in groups of 32
   // ------------------------------------
   for (int iframe = 0; iframe < n32frames; ++iframe)
   { 
      uint64_t const *lclsrc0 = src0;
      uint64_t const *lclsrc1 = src1;

      int16_t        *lcldst0 = dst0;
      int16_t        *lcldst1 = dst1;

      // ----------------------------------
      // Loop over the adcs in groups of 16
      // ----------------------------------
      for (int iadcs = 0; iadcs < 64; iadcs += 16)
      {
         // ----------------------------------------------------------------
         // Transpose the cold data stream 0 & 1  for 16 channels x 32 times
         // ----------------------------------------------------------------
         transposeAdcs16x32_kernel (lcldst0, ndstStride, lclsrc0);
         lcldst0 += 16*ndstStride;
         lclsrc0 +=  3;

         transposeAdcs16x32_kernel (lcldst1, ndstStride, lclsrc1);
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
          \a nframes time samples.  nframes must be a multiple of 8.

   \param[in]       dst[out]  Array of pointers to the channel-by-channel
                              destination arrays 
   \param[in]         offset  The offset into the destination arrays to
                              store the first transposed ADC.
   \param[in]     frames[in]  The array of WibFrames
   \param[in]    nframes[in]  The number frames, \e i.e. time samples
                              to transpose.  This must a multiple of 16.
                                                                          */
/* ---------------------------------------------------------------------- */
void WibFrame::transposeAdcs128xN  (int16_t  *const  dst[128],
                                    int                offset,
                                    WibFrame  const   *frames,
                                    int               nframes)
{
   transposeAdcs128x32N (dst, offset, frames, nframes);

   // -----------------------------
   // Pickup the remainder
   // Still must be a multiple of 8
   // -----------------------------
   int mframes = nframes & 0x1f; 
   if (mframes)
   {
      int n32  = nframes & ~0x1f;
      offset  += n32;
      frames  += n32;
      transposeAdcs128x8N (dst, offset, frames, mframes);
   }


   // Get any remaining frames (this must be less than 8) 
   int rframes = mframes & 0x7;
   if (rframes)
   {
      dst    += mframes - rframes;
      frames += mframes - rframes;
      transposeAdcs128xN_kernel (dst, frames, rframes);
   }


   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \brief Transposes the 128 ADC channels serviced by a WibFrame for
          \a nframes time samples.  nframes must be a multiple of 8.

   \param[in]       dst[out]  Array of pointers to the channel-by-channel
                              destination arrays 
   \param[in]         offset  The offset into the destination arrays to
                              store the first transposed ADC.
   \param[in]     frames[in]  The array of WibFrames
   \param[in]    nframes[in]  The number frames, \e i.e. time samples
                              to transpose.  This must a multiple of 16.
                                                                          */
/* ---------------------------------------------------------------------- */
void WibFrame::transposeAdcs128x8N  (int16_t  *const  dst[128],
                                     int                offset,
                                     WibFrame  const   *frames,
                                     int               nframes)
{
   // ----------------------------------
   // Locate the cold data in this frame
   // ----------------------------------
   WibColdData const (& coldData)[2] = frames->getColdData ();


   // ------------------------------------------------
   // Locate the packed data for each cold data stream
   // ------------------------------------------------
   uint64_t const *src0 = coldData[0].locateAdcs12b ();
   uint64_t const *src1 = coldData[1].locateAdcs12b ();


   // ----------------------------------------------------------------
   // Locate where in the output data for the 2 cold data streams goes
   // ----------------------------------------------------------------
   int16_t *const *dst0 = dst;
   int16_t *const *dst1 = dst + 64;
   int         n8frames = nframes/8;

   // ---------------------------------
   // Initialize the expander registers
   // ---------------------------------
   expandAdcs16_init_kernel ();


   // ------------------------------=----
   // Loop over the frames in groups of 8
   // -----------------------------------
   for (int iframe = 0; iframe < n8frames; ++iframe)
   { 
      uint64_t const *lclsrc0 = src0;
      uint64_t const *lclsrc1 = src1;

      int16_t *const *lcldst0 = dst0;
      int16_t *const *lcldst1 = dst1;


      // ----------------------------------
      // Loop over the adcs in groups of 16
      // ----------------------------------
      for (int iadcs = 0; iadcs < 64; iadcs += 16)
      {
         // ----------------------------------------------------------------
         // Transpose the cold data stream 0 & 1  for 16 channels x 16 times
         // ----------------------------------------------------------------
         transposeAdcs16x8_kernel  (lcldst0, offset, lclsrc0);
         lcldst0 += 16;
         lclsrc0 +=  3;

         transposeAdcs16x8_kernel  (lcldst1, offset, lclsrc1);
         lcldst1 += 16;
         lclsrc1 +=  3;
      }

      // Advance the source and destination by the 16 time frames
      src0   += 8 * sizeof (WibFrame) / sizeof (*src0);
      src1   += 8 * sizeof (WibFrame) / sizeof (*src1);

      // Advance the destination by the same 
      offset += 8;
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
void WibFrame::transposeAdcs128x16N (int16_t *const   dst[128],
                                     int                offset,
                                     WibFrame const    *frames,
                                     int               nframes)
{
   // ----------------------------------
   // Locate the cold data in this frame
   // ----------------------------------
   WibColdData const (& coldData)[2] = frames->getColdData ();


   // ------------------------------------------------
   // Locate the packed data for each cold data stream
   // ------------------------------------------------
   uint64_t const *src0 = coldData[0].locateAdcs12b ();
   uint64_t const *src1 = coldData[1].locateAdcs12b ();


   // ----------------------------------------------------------------
   // Locate where in the output data for the 2 cold data streams goes
   // ----------------------------------------------------------------
   int16_t *const *dst0 = dst;
   int16_t *const *dst1 = dst + 64;


   // ---------------------------------
   // Initialize the expander registers
   // ---------------------------------
   expandAdcs16_init_kernel ();


   // ------------------------------=-----
   // Loop over the frames in groups of 16
   // ------------------------------------
   int n16frames = nframes/16;
   for (int iframe = 0; iframe < n16frames; ++iframe)
   { 
      uint64_t const *lclsrc0 = src0;
      uint64_t const *lclsrc1 = src1;

      int16_t *const *lcldst0 = dst0;
      int16_t *const *lcldst1 = dst1;

      // ----------------------------------
      // Loop over the adcs in groups of 16
      // ----------------------------------
      for (int iadcs = 0; iadcs < 64; iadcs += 16)
      {
         // ----------------------------------------------------------------
         // Transpose the cold data stream 0 & 1  for 16 channels x 16 times
         // ----------------------------------------------------------------
         transposeAdcs16x16_kernel (lcldst0, offset, lclsrc0);
         lcldst0 += 16;
         lclsrc0 +=  3;

         transposeAdcs16x16_kernel (lcldst1, offset, lclsrc1);
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
void WibFrame::transposeAdcs128x32N (int16_t  *const  dst[128],
                                     int                offset,
                                     WibFrame  const   *frames,
                                     int               nframes)
{
   // ----------------------------------
   // Locate the cold data in this frame
   // ----------------------------------
   WibColdData const (& coldData)[2] = frames->getColdData ();


   // ------------------------------------------------
   // Locate the packed data for each cold data stream
   // ------------------------------------------------
   uint64_t const *src0 = coldData[0].locateAdcs12b ();
   uint64_t const *src1 = coldData[1].locateAdcs12b ();


   // ----------------------------------------------------------------
   // Locate where in the output data for the 2 cold data streams goes
   // ----------------------------------------------------------------
   int16_t *const *dst0 = dst;
   int16_t *const *dst1 = dst + 64;


   // ---------------------------------
   // Initialize the expander registers
   // ---------------------------------
   expandAdcs16_init_kernel ();

   // ------------------------------=-----
   // Loop over the frames in groups of 16
   // ------------------------------------
   int n32frames = nframes/32;
   for (int iframe = 0; iframe < n32frames; ++iframe)
   { 
      uint64_t  const *lclsrc0 = src0;
      uint64_t  const *lclsrc1 = src1;

      int16_t  *const *lcldst0 = dst0;
      int16_t  *const *lcldst1 = dst1;

      // ----------------------------------
      // Loop over the adcs in groups of 16
      // ----------------------------------
      for (int iadcs = 0; iadcs < 64; iadcs += 16)
      {
         // ----------------------------------------------------------------
         // Transpose the cold data stream 0 & 1  for 16 channels x 32 times
         // ----------------------------------------------------------------
         transposeAdcs16x32_kernel (lcldst0, offset, lclsrc0);
         lcldst0 += 16;
         lclsrc0 +=  3;

         transposeAdcs16x32_kernel (lcldst1, offset, lclsrc1);
         lcldst1 += 16;
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




#ifdef __AVX2__
#include "WibFrame-avx2.hh"
#elif defined (__AVX__)
#include "WibFrame-avx.hh"
#else
#include "WibFrame-gen.hh"
#endif

#include <string.h>

/* ---------------------------------------------------------------------- *//*!

  \brief The kernel to unpack 64 densely packet 12-bit values into 
         64 16-bit values.

  \param[in] dst  The destination address
  \param[in] src  The source address
                                                                          */
/* ---------------------------------------------------------------------- */
void WibColdData::expandAdcs64x1 (int16_t              *dst,
                                  uint64_t const (&src)[12])
{
   expandAdcs16_init_kernel ();
   expandAdcs64x1_kernel    (dst, reinterpret_cast<uint64_t const *>(&src));
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \brief Transposes the 128 ADC channels serviced by a WibFrame for
          \a nframes time samples.


   \param[in]       dst[out]  The output destination array. 
   \param[in] ndstStride[in]  The number of entries of each of the 128
                              arrays of transposed ADC values.
   \param[in]     frames[in]  The array of WibFrames
   \param[in]    nframes[in]  The number frames, \e i.e. time samples
                              to transpose. 

   This output array should be thought of as a 2d array dst[128][ndstStride].
   This allows each channel space for ndstStride contigous transposed
   ADC values.
                                                                          */
/* ---------------------------------------------------------------------- */
static void transposeAdcs128xN_kernel (int16_t              *dst,
                                       int            ndstStride,
                                       WibFrame const    *frames,
                                       int               nframes)
{
   //puts ("transposeAcs128xXN_kernel);


   // ---------------------------------
   // Initialize the expander registers
   // ---------------------------------
   expandAdcs16_init_kernel ();


   for (int iframe = 0; iframe < nframes;  iframe++)
   {
      // ----------------------------------
      // Locate the cold data in this frame
      // ----------------------------------
      WibColdData const (& coldData)[2] = frames[iframe].getColdData ();


      // ------------------------------------------------
      // Locate the packed data for each cold data stream
      // ------------------------------------------------
      uint64_t const *src0 = coldData[0].locateAdcs12b ();
      uint64_t const *src1 = coldData[1].locateAdcs12b ();


      // ------------------------------
      // Decode into a local Adc buffer
      // ------------------------------ 
      int16_t                adcBuf[128];
      expandAdcs64x1_kernel (adcBuf+ 0, src0);
      expandAdcs64x1_kernel (adcBuf+64, src1);


      // ---------------------------------------------------------------
      // Copy the decoded channel ordered Adcs into sample ordered array
      // ---------------------------------------------------------------
      for (int idx = 0; idx < 128; idx++)
      {
         int chnOffset           = idx * ndstStride;
         dst[chnOffset + iframe] = adcBuf[idx];
      }
   } 
   

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Transpose 8N time samples for 16 channels

  \param[out]   dst The destination array
  \param[ in]    n8 The number of groups of 8 channels, \e i.e. the N in
                    transpose16x8N
  \param[in] offset The number of elements in on channel's destination
                    array.
  \param[in]    src The source array
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void transposeAdcs16x8N_kernel (int16_t        *dst, 
                                              int              n8,
                                              int          stride,
                                              uint64_t const *src)
{
   for (int idx = 0; idx < n8; ++idx)
   {
      transposeAdcs16x8_kernel (dst, stride, src + idx * 8 * sizeof (WibFrame) / sizeof (*src));
      dst += 8;
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static inline void transposeAdcs16x16_kernel (int16_t        *dst, 
                                              int          stride, 
                                              uint64_t const *src)
{
   transposeAdcs16x8N_kernel (dst, 2, stride, src);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static inline void transposeAdcs16x32_kernel (int16_t        *dst, 
                                              int          stride, 
                                              uint64_t const *src)
{
   transposeAdcs16x8N_kernel (dst, 4, stride, src);
}
/* ---------------------------------------------------------------------- */
/* END: CONTIGIOUS TRANSPOSITION                                          */
/* ====================================================================== */





/* ====================================================================== */
/* BEGIN: CHANNEL-BY-CHANNEL TRANSPOSITION                                */
/* ---------------------------------------------------------------------- *//*!

   \brief Transposes the 128 ADC channels serviced by a WibFrame for
          \a nframes time samples.


   \param[in]       dst[out]  The output destination array. 
   \param[in] ndstStride[in]  The number of entries of each of the 128
                              arrays of transposed ADC values.
   \param[in]     frames[in]  The array of WibFrames
   \param[in]    nframes[in]  The number frames, \e i.e. time samples
                              to transpose. 

   This output array should be thought of as a 2d array dst[128][ndstStride].
   This allows each channel space for ndstStride contigous transposed
   ADC values.
                                                                          */
/* ---------------------------------------------------------------------- */
static void transposeAdcs128xN_kernel (int16_t *const       *dst,
                                       WibFrame const    *frames,
                                       int               nframes)
{
   //puts ("transposeAcs128xXN);


   // ---------------------------------
   // Initialize the expander registers
   // ---------------------------------
   expandAdcs16_init_kernel ();


   for (int iframe = 0; iframe < nframes;  iframe++)
   {
      // ----------------------------------
      // Locate the cold data in this frame
      // ----------------------------------
      WibColdData const (& coldData)[2] = frames[iframe].getColdData ();


      // ------------------------------------------------
      // Locate the packed data for each cold data stream
      // ------------------------------------------------
      uint64_t const *src0 = coldData[0].locateAdcs12b ();
      uint64_t const *src1 = coldData[1].locateAdcs12b ();


      // -----------------------------
      // Decode into a local Adc buffer
      // ----------------------------- 
      int16_t                adcBuf[128];
      expandAdcs64x1_kernel (adcBuf+ 0, src0);
      expandAdcs64x1_kernel (adcBuf+64, src1);


      // ---------------------------------------------------------------
      // Copy the decoded channel ordered Adcs into sample ordered array
      // ---------------------------------------------------------------
      for (int idx = 0; idx < 128; idx++)
      {
         dst[idx][iframe] = adcBuf[idx];
      }
   } 
   

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!
  \brief  Transpose 8N time samples for 16 channels

  \param[out]   dst Pointers to 16 arrays to receive the transposed data
  \param[ in]    n8 The number of groups of 8 channels, \e i.e. the N in
                    transpose16x8N
  \param[in] offset The number of elements in on channel's destination
                    array.
  \param[in]    src The source array
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void transposeAdcs16x8N_kernel (int16_t  *const *dst, 
                                              int               n8,
                                              int           offset,
                                              uint64_t const  *src)
{
   for (int idx = 0; idx < n8; ++idx)
   {
      transposeAdcs16x8_kernel (dst, offset, src);
      src    += 8 * sizeof (WibFrame) / sizeof (*src);
      offset += 8;
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static inline void transposeAdcs16x16_kernel (int16_t *const  *dst, 
                                              int           offset,
                                              uint64_t const  *src)
{
   transposeAdcs16x8N_kernel (dst, 2, offset, src);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static inline void transposeAdcs16x32_kernel (int16_t *const  *dst, 
                                              int           offset, 
                                              uint64_t const  *src)
{
   transposeAdcs16x8N_kernel (dst, 4, offset, src);
}
/* ---------------------------------------------------------------------- */
/* END: CONTIGIOUS TRANSPOSITION                                          */
/* ====================================================================== */
/*   END: IMPLEMENTATION: class WibFrame                                  */
} /* END: namespace access                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */
