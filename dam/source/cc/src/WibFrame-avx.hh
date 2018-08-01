// -*-Mode: C++;-*-

#ifndef PDD_WIB_FRAME_AVX_HH
#define PDD_WIB_FRAME_AVX_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     WibFrame-avx.hh
 *  @brief    WibFrame ADC expansion and unpacking - AVX version
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
   2017.09.20 jjr Separated from WibFrame-avx2.hh

\* ---------------------------------------------------------------------- */


#ifdef __AVX2__
#error __AVX2__ should not be defined
#endif

// -------------------------------------------------
// !!! KLUDGE !!! 
// --------------
// Use the generic stuff till the AVX stuff is ready
// -------------------------------------------------
#include "WibFrame-gen.hh"

#if 0

/* ---------------------------------------------------------------------- */
static void print (char const *what, uint64_t d[2]) __attribute__ ((unused));
static void print (char const *what, uint64_t d[2])
{
   printf ("%-15.15s %16.16" PRIx64 " %16.16" PRIx64 "\n",
           what, d[1], d[0]);
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Initializes the xmm15 with the necessary shuffle pattern
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void expandAdcs16_init ()
{

   /* Each entry indicates where the src byte comes from in the destination */
   static uint8_t const Shuffle0[16] __attribute__ ((aligned (32))) = 
   {
   //    0     1     2     3     4     5     6     7
      0x00, 0x01, 0x02, 0x80, 0x03, 0x04, 0x05, 0x80,

    //   8     9     a     b     c     d     e     f
      0x06, 0x07, 0x08, 0x80, 0x09, 0x0a, 0x0b, 0x80
   };


   ///uint64_t Dbg[2] __attribute__ ((aligned (32)));


   /* Load the variable shift patterns into ymm15 */
   asm ("movapd %0,%%xmm15"          :: "m"(Shuffle0[0]) : "%xmm15");
   ///asm ("movapd     %%xmm15,%0"    : "=m"(Dbg[0])  ::);
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
   by eapandAdcs_init to do the initial shuffle
                                                                          */
/* ---------------------------------------------------------------------- */
inline void expandAdcs16x1_kernel (int16_t *dst, uint64_t const *src)
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
   asm ("movupd     %0,%%xmm3"           :: "m"(s8[0])  : "%xmm3");
   asm ("movupd     %0,%%xmm5"           :: "m"(s8[12]) : "%xmm5");


   /* ----- DEBUG ---- */
   #ifdef   DEBUG
   asm ("movupd     %%xmm3,%0"             : "=m"(Dbg[3][0])  :: "memory");
   print ("Initial Load:", Dbg[3]);
   asm ("movupd     %0,%%xmm3"             :: "m"(Dbg[3][0])  : "%xmm3");
   #endif
   /* ----- END DEBUG ---- */


   asm ("vpshufb     %%xmm15,%%xmm3,%%xmm3" ::             : "%xmm3");
   asm ("vpshufb     %%xmm15,%%xmm5,%%xmm5" ::             : "%xmm5");


   /* ----- DEBUG ---- */
   #ifdef   DEBUG
   asm ("movupd     %%xmm3,%0"              : "=m"(Dbg[3][0])  ::);
   print ("Shuffle0    :", Dbg[3]);
   asm ("movupd     %0,%%xymm3"             :: "m"(Dbg[3][0]) : "%xmm3");
   #endif
   /* ----- END DEBUG ---- */


   asm ("vpslld      $5*4,%%xmm3,%%xmm4"    ::             : "%xmm4");
   asm ("vpsrld      $3*4,%%xmm3,%%xmm3"    ::             : "%xmm3");

   asm ("vpslld      $5*4,%%xmm5,%%xmm6"    ::             : "%xmm6");
   asm ("vpsrld      $3*4,%%xmm5,%%xmm5"    ::             : "%xmm5");


   /* ----- DEBUG ---- */
   #ifdef   DEBUG
   asm ("movupd     %%xmm4,%0"            : "=m"(Dbg[4][0])  :: "memory");
   asm ("movupd     %%xmm3,%0"            : "=m"(Dbg[3][0])  :: "memory");

   print ("Xmm4 << 5:", Dbg[4]);
   print ("Xmm3 >> 3:", Dbg[3]);

   asm ("movupd     %0,%%xmm4"            :: "m"(Dbg[4][0])  : "%xmm4");
   asm ("movupd     %0,%%xmm3"            :: "m"(Dbg[3][0])  : "%xmm3");
   #endif
   /* ----- END DEBUG ---- */


   asm ("vpsrld      $5*4,%%xmm4,%%xmm4"   ::             : "%xmm4");
   asm ("vpslld      $4*4,%%xmm3,%%xmm3"   ::             : "%xmm3");

   asm ("vpsrld      $5*4,%%xmm6,%%xmm6"   ::             : "%xmm6");
   asm ("vpslld      $4*4,%%xmm5,%%xmm5"   ::             : "%xmm5");


   /* ----- DEBUG ---- */
   #ifdef   DEBUG
   asm ("movupd     %%xmm4,%0"            : "=m"(Dbg[4][0])  :: "memory");
   asm ("movupd     %%xmm3,%0"            : "=m"(Dbg[3][0])  :: "memory");

   print ("Ymm4 >> 5:", Dbg[4]);
   print ("Ymm3 << 4:", Dbg[3]);

   asm ("movupd     %0,%%xmm3"            :: "m"(Dbg[3][0])  : "%xmm3");
   asm ("movupd     %0,%%xmm4"            :: "m"(Dbg[4][0])  : "%xmm4");
   #endif
   /* ----- END DEBUG ---- */

   asm ("vpor        %%xmm3,%%xmm4,%%xmm3" ::             : "%xmm3");
   asm ("vpor        %%xmm5,%%xmm6,%%xmm5" ::             : "%xmm5");


   /* ----- DEBUG ---- */
   #ifdef   DEBUG
   asm ("movupd     %%xmm3,%0"            : "=m"(Dbg[3][0])  :: "memory");
   print ("Ymm3 or:", Dbg[3]);
   asm ("movupd     %0,%%xmm3"            :: "m"(Dbg[3][0])  : "%xmm3");
   #endif
   /* ----- END DEBUG ---- */


   /* Store the result */
   asm ("movupd      %%xmm3,%0"   : "=m"(dst[0])  :: "memory");
   asm ("movupd      %%xmm5,%0"   : "=m"(dst[8])  :: "memory");

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
   by expancAdcs_init to do the initial shuffle
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

  \brief The kernel to unpack 128 densely packet 12-bit values into 
         6128 16-bit values.

  \param[in] dst  The destination address
  \param[in] src  The source address

  \warning
   This routine assumes that the SIMD register xmm15 is initialized
   by expandAdcs_init to do the initial shuffle
                                                                          */
/* ---------------------------------------------------------------------- */
inline void expandAdcs128x1_kernel (int16_t        *dst, 
                                    uint64_t const *src)
{
#if 1
   expandAdcs16x1_kernel (dst+0*16, src+0*3);
   expandAdcs16x1_kernel (dst+1*16, src+1*3);
   expandAdcs16x1_kernel (dst+2*16, src+2*3);
   expandAdcs16x1_kernel (dst+3*16, src+3*3);

   expandAdcs16x1_kernel (dst+4*16, src+4*3);
   expandAdcs16x1_kernel (dst+5*16, src+5*3);
   expandAdcs16x1_kernel (dst+6*16, src+6*3);
   expandAdcs16x1_kernel (dst+7*16, src+7*3);
#endif
   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief The kernel to unpack 16 densely packet 12-bit values for 4
         time samples into  64 16-bit values.

  \param[in] dst  The destination address
  \param[in] src  The source address

  \warning
   This routine assumes that the SIMD register xmm15 is initialized
   by expandhAdcs_init to do the initial shuffle
                                                                          */
/* ---------------------------------------------------------------------- */
inline void expandAdcs16x4_kernel (int16_t        *dst, 
                                   uint64_t const *src)
{
#define STRIDE64 ((STRIDE)/sizeof (uint64_t))

   /*
   printf ("First %4.4x %4.4x %4.4x %4.4x\n",
           ((int16_t const *)s8)[0*STRIDE/2],
           ((int16_t const *)s8)[1*STRIDE/2],
           ((int16_t const *)s8)[2*STRIDE/2],
           ((int16_t const *)s8)[3*STRIDE/2]);
   */
#if 1
   expandAdcs16x1_kernel (dst+0*16, src+0*STRIDE64);
   expandAdcs16x1_kernel (dst+1*16, src+1*STRIDE64);
   expandAdcs16x1_kernel (dst+2*16, src+2*STRIDE64);
   expandAdcs16x1_kernel (dst+3*16, src+3*STRIDE64);
#endif

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
static inline void transpose16x8_kernel (int16_t        *dst, 
                                         int          stride, 
                                         int16_t  const *src)
{
#if 1
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
#endif
   return;

}
/* ---------------------------------------------------------------------- */



static inline void transpose16x8_kernel (int16_t  *dst[128], 
                                         int         offset, 
                                         int16_t const *src)
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
#endif
#endif
