// -*-Mode: C++;-*-

#ifndef PDD_WIB_FRAME_GEN_HH
#define PDD_WIB_FRAME_GEN_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     WibFrame-gen.hh
 *  @brief    WibFrame ADC expansion and unpacking - AVX2 version
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
 *  2017.09.20
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
   2017.09.20 jjr Separated from WibFrame.cc

\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief One time initialization for expansion.  This is a noop for
         the generic implementation
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void expandAdcs16_init_kernel ()
{
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!


  \brief The kernel to unpack 16 densely packet 12-bit values into 
         64 16-bit values.

  \param[in] dst  The destination address
  \param[in] src  The source address
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void expandAdcs16x1_kernel (uint16_t *dst, uint64_t const *src)
{
   uint64_t w0;
   uint64_t w1;
   uint64_t w2;

   w0 = *src++;  
   dst[0] = (w0 >>  0) & 0xfff;     
   dst[1] = (w0 >> 12) & 0xfff;
   dst[2] = (w0 >> 24) & 0xfff;
   dst[3] = (w0 >> 36) & 0xfff;
   dst[4] = (w0 >> 48) & 0xfff;


   w1 = *src++; 
   dst[5] = ((w1 & 0xff) << 4) | (w0 >> 60);
   dst[6] = (w1 >>  8) & 0xfff;
   dst[7] = (w1 >> 20) & 0xfff;
   dst[8] = (w1 >> 32) & 0xfff;
   dst[9] = (w1 >> 44) & 0xfff;

   w2 = *src++; 
   dst[10] = ((w2 & 0xf) << 8) | (w1 >> 56);
   dst[11] = (w2 >>  4) & 0xfff;
   dst[12] = (w2 >> 16) & 0xfff;
   dst[13] = (w2 >> 28) & 0xfff;
   dst[14] = (w2 >> 40) & 0xfff;
   dst[15] = (w2 >> 52) & 0xfff;

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief The kernel to unpack 64 densely packet 12-bit values into 
         64 16-bit values.

  \param[in] dst  The destination address
  \param[in] src  The source address
                                                                          */
/* ---------------------------------------------------------------------- */
inline void expandAdcs64x1_kernel (uint16_t       *dst, 
                                   uint64_t const *src)
{
   expandAdcs16x1_kernel (dst+0*16, src+0*3);
   expandAdcs16x1_kernel (dst+1*16, src+1*3);
   expandAdcs16x1_kernel (dst+2*16, src+2*3);
   expandAdcs16x1_kernel (dst+3*16, src+3*3);

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief The kernel to unpack 16 densely packet 12-bit values for 4
         time samples into  64 16-bit values.

  \param[in] dst  The destination address
  \param[in] src  The source address

  \warning
   This routine assumes that the SIMD register ymm15 is initialized
   by expandhAdcs_init to do the initial shuffle
                                                                          */
/* ---------------------------------------------------------------------- */
inline void expandAdcs16x4_kernel (uint16_t       *dst, 
                                   uint64_t const *src)
{
#define STRIDE64 (sizeof (WibFrame) / sizeof (*src))

   /*
   puts ("Kernel 16x4");
   printf ("First %4.4x %4.4x %4.4x %4.4x\n",
           ((uint16_t const *)src)[0*STRIDE/2],
           ((uint16_t const *)src)[1*STRIDE/2],
           ((uint16_t const *)src)[2*STRIDE/2],
           ((uint16_t const *)src)[3*STRIDE/2]);
   */

   expandAdcs16x1_kernel (dst+0*16, src+0*STRIDE64);
   expandAdcs16x1_kernel (dst+1*16, src+1*STRIDE64);
   expandAdcs16x1_kernel (dst+2*16, src+2*STRIDE64);
   expandAdcs16x1_kernel (dst+3*16, src+3*STRIDE64);

/*
   for (int idx = 0; idx < 64; ++idx)
   {
      if ((idx & 0xf) == 0) printf ("%2.2x:", idx);
      printf (" %4.4x", dst[idx]);
      if ((idx & 0xf) == 0xf) putchar ('\n');
   }
   putchar ('\n');
*/

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Transponse 16 channels x 4 timeslices

  \param[out]        dst  The destination address
  \param[ in] ndstStride  The number of timesamples in one channel
  \param[ in]       src0  Pointer to the ADCs for the first timeslice

                                                                          */
/* ---------------------------------------------------------------------- */
static void transposeAdcs16x4_kernel (uint64_t      *dst64, 
                                      int       ndstStride, 
                                      uint64_t const *src0)
{
   uint64_t const *src1 = src0 + sizeof (WibFrame) / sizeof (*src1);
   uint64_t const *src2 = src1 + sizeof (WibFrame) / sizeof (*src2);
   uint64_t const *src3 = src2 + sizeof (WibFrame) / sizeof (*src3);

   uint64_t dst;

   // Get the set of adcs channels 0-3 for the first 4 timeslices
   uint64_t w0_0 = *src0++;
   uint64_t w1_0 = *src1++;
   uint64_t w2_0 = *src2++;
   uint64_t w3_0 = *src3++;

   // Channel = 0
   dst    = ((w0_0 >> 0 * 12) & 0xfff) << 0 * 16;
   dst   |= ((w1_0 >> 0 * 12) & 0xfff) << 1 * 16;
   dst   |= ((w2_0 >> 0 * 12) & 0xfff) << 2 * 16;
   dst   |= ((w3_0 >> 0 * 12) & 0xfff) << 3 * 16;
  *dst64  = dst;
   dst64 += ndstStride;
   //printf ("t16x4[0] = %16.16" PRIx64 "\n", dst);
   
   // Channel  1
   dst    = ((w0_0 >> 1 * 12) & 0xfff) << 0 * 16;
   dst   |= ((w1_0 >> 1 * 12) & 0xfff) << 1 * 16;
   dst   |= ((w2_0 >> 1 * 12) & 0xfff) << 2 * 16;
   dst   |= ((w3_0 >> 1 * 12) & 0xfff) << 3 * 16;
  *dst64  = dst;
   dst64 += ndstStride;
   //printf ("t16x4[1] = %16.16" PRIx64 "\n", dst);

   
   // Channel  2
   dst    = ((w0_0 >> 2 * 12) & 0xfff) << 0 * 16;
   dst   |= ((w1_0 >> 2 * 12) & 0xfff) << 1 * 16;
   dst   |= ((w2_0 >> 2 * 12) & 0xfff) << 2 * 16;
   dst   |= ((w3_0 >> 2 * 12) & 0xfff) << 3 * 16;
  *dst64  = dst;
   dst64 += ndstStride;

   
   // Channel  3
   dst    = ((w0_0 >> 3 * 12) & 0xfff) << 0 * 16;
   dst   |= ((w1_0 >> 3 * 12) & 0xfff) << 1 * 16;
   dst   |= ((w2_0 >> 3 * 12) & 0xfff) << 2 * 16;
   dst   |= ((w3_0 >> 3 * 12) & 0xfff) << 3 * 16;
  *dst64  = dst;
   dst64 += ndstStride;


   // Channel 4
   dst    = ((w0_0 >> 4 * 12) & 0xfff) << 0 * 16;
   dst   |= ((w1_0 >> 4 * 12) & 0xfff) << 1 * 16;
   dst   |= ((w2_0 >> 4 * 12) & 0xfff) << 2 * 16;
   dst   |= ((w3_0 >> 4 * 12) & 0xfff) << 3 * 16;
  *dst64  = dst;
   dst64 += ndstStride;
   //printf ("Channel 4 = %16.16" PRIx64 "\n", dst);
   
   uint64_t w0_1 = *src0++;  
   uint64_t w1_1 = *src1++;  
   uint64_t w2_1 = *src2++;  
   uint64_t w3_1 = *src3++;  



   // Channel 5
   dst    = (((w0_1 & 0xff) << 4) | (w0_0 >> 5*12)) << 0 * 16;
   dst   |= (((w1_1 & 0xff) << 4) | (w1_0 >> 5*12)) << 1 * 16;
   dst   |= (((w2_1 & 0xff) << 4) | (w2_0 >> 5*12)) << 2 * 16;
   dst   |= (((w3_1 & 0xff) << 4) | (w3_0 >> 5*12)) << 3 * 16;
  *dst64  = dst;
   dst64 += ndstStride;


   // Channel 6
   dst    = ((w0_1 >> (0*12 + 8)) & 0xfff)        << 0 * 16;
   dst   |= ((w1_1 >> (0*12 + 8)) & 0xfff)        << 1 * 16;
   dst   |= ((w2_1 >> (0*12 + 8)) & 0xfff)        << 2 * 16;
   dst   |= ((w3_1 >> (0*12 + 8)) & 0xfff)        << 3 * 16;
  *dst64  = dst;
   dst64 += ndstStride;


   // Channel 7
   dst    = ((w0_1 >> (1*12 + 8)) & 0xfff)        << 0 * 16;
   dst   |= ((w1_1 >> (1*12 + 8)) & 0xfff)        << 1 * 16;
   dst   |= ((w2_1 >> (1*12 + 8)) & 0xfff)        << 2 * 16;
   dst   |= ((w3_1 >> (1*12 + 8)) & 0xfff)        << 3 * 16;
  *dst64  = dst;
   dst64 += ndstStride;

   
   // Channel 8
   dst    = ((w0_1 >> (2*12 + 8)) & 0xfff) << 0 * 16;
   dst   |= ((w1_1 >> (2*12 + 8)) & 0xfff) << 1 * 16;
   dst   |= ((w2_1 >> (2*12 + 8)) & 0xfff) << 2 * 16;
   dst   |= ((w3_1 >> (2*12 + 8)) & 0xfff) << 3 * 16;
  *dst64  = dst;
   dst64 += ndstStride;

   
   // Channel 9
   dst    = ((w0_1 >> (3*12 + 8)) & 0xfff) << 0 * 16;
   dst   |= ((w1_1 >> (3*12 + 8)) & 0xfff) << 1 * 16;
   dst   |= ((w2_1 >> (3*12 + 8)) & 0xfff) << 2 * 16;
   dst   |= ((w3_1 >> (3*12 + 8)) & 0xfff) << 3 * 16;
  *dst64  = dst;
   dst64 += ndstStride;
   //printf ("t16x4[9] = %16.16" PRIx64 "\n", dst);
   
   uint64_t w0_2 = *src0++;  
   uint64_t w1_2 = *src1++;  
   uint64_t w2_2 = *src2++;  
   uint64_t w3_2 = *src3++;  
      

   // Chanel 10
   dst    = (((w0_1 >> (4*12 + 8)) & 0xff)  | (w0_2 & 0xf) << 8) << 0 * 16;
   dst   |= (((w1_1 >> (4*12 + 8)) & 0xff)  | (w1_2 & 0xf) << 8) << 1 * 16;
   dst   |= (((w2_1 >> (4*12 + 8)) & 0xff)  | (w2_2 & 0xf) << 8) << 2 * 16;
   dst   |= (((w3_1 >> (4*12 + 8)) & 0xff)  | (w3_2 & 0xf) << 8) << 3 * 16;
  *dst64  = dst;
   dst64 += ndstStride;
   

   // Channel 11
   dst    = ((w0_2 >> (0*12 + 4)) & 0xfff) << 0 * 16;
   dst   |= ((w1_2 >> (0*12 + 4)) & 0xfff) << 1 * 16;
   dst   |= ((w2_2 >> (0*12 + 4)) & 0xfff) << 2 * 16;
   dst   |= ((w3_2 >> (0*12 + 4)) & 0xfff) << 3 * 16;
  *dst64  = dst;
   dst64 += ndstStride;
      

   // Channel 12
   dst    = ((w0_2 >> (1*12 + 4)) & 0xfff) << 0 * 16;
   dst   |= ((w1_2 >> (1*12 + 4)) & 0xfff) << 1 * 16;
   dst   |= ((w2_2 >> (1*12 + 4)) & 0xfff) << 2 * 16;
   dst   |= ((w3_2 >> (1*12 + 4)) & 0xfff) << 3 * 16;
  *dst64  = dst;
   dst64 += ndstStride;


   // Channel 13   
   dst    = ((w0_2 >> (2*12 + 4)) & 0xfff) << 0 * 16;
   dst   |= ((w1_2 >> (2*12 + 4)) & 0xfff) << 1 * 16;
   dst   |= ((w2_2 >> (2*12 + 4)) & 0xfff) << 2 * 16;
   dst   |= ((w3_2 >> (2*12 + 4)) & 0xfff) << 3 * 16;
  *dst64  = dst;
   dst64 += ndstStride;

   
   // Channel 14
   dst    = ((w0_2 >> (3*12 + 4)) & 0xfff) << 0 * 16;
   dst   |= ((w1_2 >> (3*12 + 4)) & 0xfff) << 1 * 16;
   dst   |= ((w2_2 >> (3*12 + 4)) & 0xfff) << 2 * 16;
   dst   |= ((w3_2 >> (3*12 + 4)) & 0xfff) << 3 * 16;
  *dst64  = dst;
   dst64 += ndstStride;


   // Channel 15
   dst    = ((w0_2 >> (4*12 + 4))        ) << 0 * 16;
   dst   |= ((w1_2 >> (4*12 + 4))        ) << 1 * 16;
   dst   |= ((w2_2 >> (4*12 + 4))        ) << 2 * 16;
   dst   |= ((w3_2 >> (4*12 + 4))        ) << 3 * 16;
  *dst64  = dst;
   dst64 += ndstStride;

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Transpose 128 channels by 8 timeslices into channels with 
          separate array address

  \param[out]        dst  An array of 128 pointers to receiving to receive
                          the data for each channel
  \param[ in] ndstStride  The number of timesamples in one channel
  \param[ in]        src  The data source
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void transposeAdcs16x8_kernel (uint16_t       *dst, 
                                             int      ndstStride,
                                             uint64_t const *src)
{
   uint64_t  *dst64 = (uint64_t *)dst;
   int ndstStride64 = ndstStride * sizeof (*dst) / sizeof (*dst64);


   transposeAdcs16x4_kernel (dst64,   ndstStride64, src);
   transposeAdcs16x4_kernel (dst64+1, ndstStride64, 
                    src+4*sizeof (WibFrame) / sizeof (*src));

   return;
}
/* ---------------------------------------------------------------------- */


#if PRINT16x4_KERNEL
static void print (uint64_t *const *dst, int chn, int off)
{
   printf ("dst64[%1x][%4.4x] @%p = %16.16" PRIx64 "\n", 
           chn, off, (void *)&dst[chn][off], dst[chn][off]);
}
#else
#define print(_dst, _chn, _off)
#endif


/* ---------------------------------------------------------------------- *//*!

  \brief Transponse 16 channels x 4 timeslices

  \param[out]        dst  The destination address
  \param[ in] ndstStride  The number of timesamples in one channel
  \param[ in]       src0  Pointer to the ADCs for the first timeslice

                                                                          */
/* ---------------------------------------------------------------------- */
static void transposeAdcs16x4_kernel (uint64_t *const *dst64, 
                                      int             offset, 
                                      uint64_t   const *src0)
{
   uint64_t const *src1 = src0 + sizeof (WibFrame) / sizeof (*src1);
   uint64_t const *src2 = src1 + sizeof (WibFrame) / sizeof (*src2);
   uint64_t const *src3 = src2 + sizeof (WibFrame) / sizeof (*src3);

   uint64_t dst;


   // Get the set of adcs channels 0-3 for the first 4 timeslices
   uint64_t w0_0 = *src0++;
   uint64_t w1_0 = *src1++;
   uint64_t w2_0 = *src2++;
   uint64_t w3_0 = *src3++;


   // Channel = 0
   dst    = ((w0_0 >> 0 * 12) & 0xfff) << 0 * 16;
   dst   |= ((w1_0 >> 0 * 12) & 0xfff) << 1 * 16;
   dst   |= ((w2_0 >> 0 * 12) & 0xfff) << 2 * 16;
   dst   |= ((w3_0 >> 0 * 12) & 0xfff) << 3 * 16;
   dst64[0][offset] = dst;
   print (dst64, 0, offset);


   
   // Channel  1
   dst    = ((w0_0 >> 1 * 12) & 0xfff) << 0 * 16;
   dst   |= ((w1_0 >> 1 * 12) & 0xfff) << 1 * 16;
   dst   |= ((w2_0 >> 1 * 12) & 0xfff) << 2 * 16;
   dst   |= ((w3_0 >> 1 * 12) & 0xfff) << 3 * 16;
   dst64[1][offset] = dst;
   print (dst64, 1, offset);

   
   // Channel  2
   dst    = ((w0_0 >> 2 * 12) & 0xfff) << 0 * 16;
   dst   |= ((w1_0 >> 2 * 12) & 0xfff) << 1 * 16;
   dst   |= ((w2_0 >> 2 * 12) & 0xfff) << 2 * 16;
   dst   |= ((w3_0 >> 2 * 12) & 0xfff) << 3 * 16;
   dst64[2][offset] = dst;
   print (dst64, 2, offset);

   
   // Channel  3
   dst    = ((w0_0 >> 3 * 12) & 0xfff) << 0 * 16;
   dst   |= ((w1_0 >> 3 * 12) & 0xfff) << 1 * 16;
   dst   |= ((w2_0 >> 3 * 12) & 0xfff) << 2 * 16;
   dst   |= ((w3_0 >> 3 * 12) & 0xfff) << 3 * 16;
   dst64[3][offset] = dst;
   print (dst64, 3, offset);

   // Channel 4
   dst    = ((w0_0 >> 4 * 12) & 0xfff) << 0 * 16;
   dst   |= ((w1_0 >> 4 * 12) & 0xfff) << 1 * 16;
   dst   |= ((w2_0 >> 4 * 12) & 0xfff) << 2 * 16;
   dst   |= ((w3_0 >> 4 * 12) & 0xfff) << 3 * 16;
   dst64[4][offset] = dst;
   print (dst64, 4, offset);
   

   uint64_t w0_1 = *src0++;  
   uint64_t w1_1 = *src1++;  
   uint64_t w2_1 = *src2++;  
   uint64_t w3_1 = *src3++;  

   // Channel 5
   dst    = (((w0_1 & 0xff) << 4) | (w0_0 >> 5*12)) << 0 * 16;
   dst   |= (((w1_1 & 0xff) << 4) | (w1_0 >> 5*12)) << 1 * 16;
   dst   |= (((w2_1 & 0xff) << 4) | (w2_0 >> 5*12)) << 2 * 16;
   dst   |= (((w3_1 & 0xff) << 4) | (w3_0 >> 5*12)) << 3 * 16;
   dst64[5][offset] = dst;
   print (dst64, 5, offset);

   // Channel 6
   dst    = ((w0_1 >> (0*12 + 8)) & 0xfff)        << 0 * 16;
   dst   |= ((w1_1 >> (0*12 + 8)) & 0xfff)        << 1 * 16;
   dst   |= ((w2_1 >> (0*12 + 8)) & 0xfff)        << 2 * 16;
   dst   |= ((w3_1 >> (0*12 + 8)) & 0xfff)        << 3 * 16;
   dst64[6][offset] = dst;
   print (dst64, 6, offset);

   // Channel 7
   dst    = ((w0_1 >> (1*12 + 8)) & 0xfff)        << 0 * 16;
   dst   |= ((w1_1 >> (1*12 + 8)) & 0xfff)        << 1 * 16;
   dst   |= ((w2_1 >> (1*12 + 8)) & 0xfff)        << 2 * 16;
   dst   |= ((w3_1 >> (1*12 + 8)) & 0xfff)        << 3 * 16;
   dst64[7][offset] = dst;
   print (dst64, 7, offset);
   
   // Channel 8
   dst    = ((w0_1 >> (2*12 + 8)) & 0xfff) << 0 * 16;
   dst   |= ((w1_1 >> (2*12 + 8)) & 0xfff) << 1 * 16;
   dst   |= ((w2_1 >> (2*12 + 8)) & 0xfff) << 2 * 16;
   dst   |= ((w3_1 >> (2*12 + 8)) & 0xfff) << 3 * 16;
   dst64[8][offset] = dst;
   print (dst64, 8, offset);

   
   // Channel 9
   dst    = ((w0_1 >> (3*12 + 8)) & 0xfff) << 0 * 16;
   dst   |= ((w1_1 >> (3*12 + 8)) & 0xfff) << 1 * 16;
   dst   |= ((w2_1 >> (3*12 + 8)) & 0xfff) << 2 * 16;
   dst   |= ((w3_1 >> (3*12 + 8)) & 0xfff) << 3 * 16;
   dst64[9][offset] = dst;
   print (dst64, 9, offset);
   
   uint64_t w0_2 = *src0++;  
   uint64_t w1_2 = *src1++;  
   uint64_t w2_2 = *src2++;  
   uint64_t w3_2 = *src3++;  
      

   // Chanel 10
   dst    = (((w0_1 >> (4*12 + 8)) & 0xff)  | (w0_2 & 0xf) << 8) << 0 * 16;
   dst   |= (((w1_1 >> (4*12 + 8)) & 0xff)  | (w1_2 & 0xf) << 8) << 1 * 16;
   dst   |= (((w2_1 >> (4*12 + 8)) & 0xff)  | (w2_2 & 0xf) << 8) << 2 * 16;
   dst   |= (((w3_1 >> (4*12 + 8)) & 0xff)  | (w3_2 & 0xf) << 8) << 3 * 16;
   dst64[10][offset] = dst;
   print (dst64, 10, offset);   

   // Channel 11
   dst    = ((w0_2 >> (0*12 + 4)) & 0xfff) << 0 * 16;
   dst   |= ((w1_2 >> (0*12 + 4)) & 0xfff) << 1 * 16;
   dst   |= ((w2_2 >> (0*12 + 4)) & 0xfff) << 2 * 16;
   dst   |= ((w3_2 >> (0*12 + 4)) & 0xfff) << 3 * 16;
   dst64[11][offset] = dst;
   print (dst64, 11, offset);
      

   // Channel 12
   dst    = ((w0_2 >> (1*12 + 4)) & 0xfff) << 0 * 16;
   dst   |= ((w1_2 >> (1*12 + 4)) & 0xfff) << 1 * 16;
   dst   |= ((w2_2 >> (1*12 + 4)) & 0xfff) << 2 * 16;
   dst   |= ((w3_2 >> (1*12 + 4)) & 0xfff) << 3 * 16;
   dst64[12][offset] = dst;
   print (dst64, 12, offset);

   // Channel 13   
   dst    = ((w0_2 >> (2*12 + 4)) & 0xfff) << 0 * 16;
   dst   |= ((w1_2 >> (2*12 + 4)) & 0xfff) << 1 * 16;
   dst   |= ((w2_2 >> (2*12 + 4)) & 0xfff) << 2 * 16;
   dst   |= ((w3_2 >> (2*12 + 4)) & 0xfff) << 3 * 16;
   dst64[13][offset] = dst;
   print (dst64, 13, offset);

   // Channel 14
   dst    = ((w0_2 >> (3*12 + 4)) & 0xfff) << 0 * 16;
   dst   |= ((w1_2 >> (3*12 + 4)) & 0xfff) << 1 * 16;
   dst   |= ((w2_2 >> (3*12 + 4)) & 0xfff) << 2 * 16;
   dst   |= ((w3_2 >> (3*12 + 4)) & 0xfff) << 3 * 16;
   dst64[14][offset] = dst;
   print (dst64, 14, offset);


   // Channel 15
   dst    = ((w0_2 >> (4*12 + 4))        ) << 0 * 16;
   dst   |= ((w1_2 >> (4*12 + 4))        ) << 1 * 16;
   dst   |= ((w2_2 >> (4*12 + 4))        ) << 2 * 16;
   dst   |= ((w3_2 >> (4*12 + 4))        ) << 3 * 16;
   dst64[15][offset] = dst;
   print (dst64, 15, offset);

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief  Transpose 16 channels by 8 timeslices into channels with 
          separate array address

  \param[out]     dst  An array of 16 pointers to receiving to receive
                       the data for each channel
  \param[ in]  offset  The offset into each channel array to start
                       storing the first timeslice
  \param[ in]     src  The data source
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void transposeAdcs16x8_kernel (uint16_t *const *dst, 
                                             int           offset, 
                                             uint64_t const  *src)
{
   uint64_t *const *dst64 = (uint64_t *const *)dst;

   transposeAdcs16x4_kernel (dst64, offset/4+0, src);
   transposeAdcs16x4_kernel (dst64, offset/4+1, 
          src+4*sizeof (WibFrame) / sizeof (*src));

   return;
}
/* ---------------------------------------------------------------------- */

#endif
