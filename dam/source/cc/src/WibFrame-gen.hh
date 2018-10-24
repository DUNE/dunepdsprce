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
   2018.10.23 jjr Had to remove the inline from expandAdcs64x1_kernel. 
                  The gcc optimizer optimized it right out of existence.
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


/*
    65 54 44 33 22 21 1
    06 28 40 62 84 06 28 40
   +--+--+--+--+--+--+--+--+
   |55 44 33 22 31 20 11 00|
   |10 10 21 21 02 02 10 10|
   +--+--+--+--+--+--+--+--+
*/

#define D0_10(_w0) (((_w0 >>  0) & 0xff) << 0)
#define D1_10(_w0) (((_w0 >>  8) & 0xff) << 0)
#define D0_2(_w0)  (((_w0 >> 16) & 0x0f) << 8)
#define D2_0(_w0)  (((_w0 >> 20) & 0x0f) << 0)
#define D1_2(_w0)  (((_w0 >> 24) & 0x0f) << 8)
#define D3_0(_w0)  (((_w0 >> 28) & 0x0f) << 0)
#define D2_21(_w0) (((_w0 >> 32) & 0xff) << 4)
#define D3_21(_w0) (((_w0 >> 40) & 0xff) << 4)
#define D4_10(_w0) (((_w0 >> 48) & 0xff) << 0)
#define D5_10(_w0) (((_w0 >> 56) & 0xff) << 0)

/*
    65 54 44 33 22 21 1
    06 28 40 62 84 06 28 40
   +--+--+--+--+--+--+--+--+
   |B9 A8 99 88 77 66 75 64|
   |02 02 10 10 21 21 02 02|
   +--+--+--+--+--+--+--+--+

*/
#define D4_2(_w1)  (((_w1 >>  0) & 0x0f) << 8)
#define D6_0(_w1)  (((_w1 >>  4) & 0x0f) << 0)
#define D5_2(_w1)  (((_w1 >>  8) & 0x0f) << 8)
#define D7_0(_w1)  (((_w1 >> 12) & 0x0f) << 0)
#define D6_21(_w1) (((_w1 >> 16) & 0xff) << 4)
#define D7_21(_w1) (((_w1 >> 24) & 0xff) << 4)
#define D8_10(_w1) (((_w1 >> 32) & 0xff) << 0)
#define D9_10(_w1) (((_w1 >> 40) & 0xff) << 0)
#define D8_2(_w1)  (((_w1 >> 48) & 0x0f) << 8)
#define DA_0(_w1)  (((_w1 >> 52) & 0x0f) << 0)
#define D9_2(_w1)  (((_w1 >> 56) & 0x0f) << 8)
#define DB_0(_w1)  (((_w1 >> 60) & 0x0f) << 0)

/*
    65 54 44 33 22 21 1
    06 28 40 62 84 06 28 40
   +--+--+--+--+--+--+--+--+
   |FF EE FD EC DD CC BB AA|
   |21 21 02 02 10 10 21 21|
   +--+--+--+--+--+--+--+--+
*/

#define DA_21(_w2)  (((_w2 >>  0) & 0xff) << 4)
#define DB_21(_w2)  (((_w2 >>  8) & 0xff) << 4)
#define DC_10(_w2)  (((_w2 >> 16) & 0xff) << 0)
#define DD_10(_w2)  (((_w2 >> 24) & 0xff) << 0)
#define DC_2(_w2)   (((_w2 >> 32) & 0x0f) << 8)
#define DE_0(_w2)   (((_w2 >> 36) & 0x0f) << 0)
#define DD_2(_w2)   (((_w2 >> 40) & 0x0f) << 8)
#define DF_0(_w2)   (((_w2 >> 44) & 0x0f) << 0)
#define DE_21(_w2)  (((_w2 >> 48) & 0xff) << 4)
#define DF_21(_w2)  (((_w2 >> 56) & 0xff) << 4)

#define D0(_w0)     (D0_2  (_w0) | D0_10 (_w0))
#define D1(_w0)     (D1_2  (_w0) | D1_10 (_w0))
#define D2(_w0)     (D2_21 (_w0) | D2_0  (_w0))
#define D3(_w0)     (D3_21 (_w0) | D3_0  (_w0))

#define D4(_w1,_w0) (D4_2  (_w1) | D4_10 (_w0))
#define D5(_w1,_w0) (D5_2  (_w1) | D5_10 (_w0))
#define D6(_w1)     (D6_21 (_w1) | D6_0  (_w1))
#define D7(_w1)     (D7_21 (_w1) | D7_0  (_w1))
#define D8(_w1)     (D8_2  (_w1) | D8_10 (_w1))
#define D9(_w1)     (D9_2  (_w1) | D9_10 (_w1))

#define DA(_w2,_w1) (DA_21 (_w2) | DA_0  (_w1))
#define DB(_w2,_w1) (DB_21 (_w2) | DB_0  (_w1))
#define DC(_w2)     (DC_2  (_w2) | DC_10 (_w2))
#define DD(_w2)     (DD_2  (_w2) | DD_10 (_w2))
#define DE(_w2)     (DE_21 (_w2) | DE_0  (_w2))
#define DF(_w2)     (DF_21 (_w2) | DF_0  (_w2))


static inline uint64_t expand0_3 (uint64_t w0)
               __attribute__ ((always_inline));

static inline uint64_t expand0_3 (uint64_t w0)
{
   uint64_t dst = (D0 (w0)     << 0 * 16)
                | (D1 (w0)     << 1 * 16)
                | (D2 (w0)     << 2 * 16)
                | (D3 (w0)     << 3 * 16);

   return dst;
}


static inline uint64_t expand4_7 (uint64_t w1, uint64_t w0)
                            __attribute__ ((always_inline));

static inline uint64_t expand4_7 (uint64_t w1, uint64_t w0)
{
   uint64_t dst = (D4 (w1, w0) << 0 * 16)
                | (D5 (w1, w0) << 1 * 16)
                | (D6 (w1)     << 2 * 16) 
                | (D7 (w1)     << 3 * 16);

   return dst;
}

static inline uint64_t expand8_B (uint64_t w2, uint64_t w1) 
                            __attribute__ ((always_inline));

static inline uint64_t expand8_B (uint64_t w2, uint64_t w1)
{
   uint64_t dst = (D8 (w1)     << 0 * 16)
                | (D9 (w1)     << 1 * 16)
                | (DA (w2, w1) << 2 * 16)
                | (DB (w2, w1) << 3 * 16);

   return dst;
}


static inline uint64_t expandC_F (uint64_t w2) 
               __attribute__ ((always_inline));

static inline uint64_t expandC_F (uint64_t w2)
{
   uint64_t dst = (DC (w2)     << 0 * 16)
                | (DD (w2)     << 1 * 16)
                | (DE (w2)     << 2 * 16)
                | (DF (w2)     << 3 * 16);

   return dst;
}
 

/* ---------------------------------------------------------------------- *//*!


  \brief The kernel to unpack 16 densely packet 12-bit values into 
         64 16-bit values.

  \param[in] dst  The destination address
  \param[in] src  The source address
                                                                          */
/* ---------------------------------------------------------------------- */
static inline void expandAdcs16x1_kernel (int16_t *dst, uint64_t const *src)
{
   uint64_t *dst64 = reinterpret_cast<uint64_t *>(dst);

   uint64_t w0 = *src++;  
   dst64[0]    = expand0_3 (w0);

   uint64_t w1 = *src++; 
   dst64[1]    = expand4_7 (w1, w0);

   uint64_t w2 = *src++; 
   dst64[2]    = expand8_B (w2, w1);
   dst64[3]    = expandC_F (w2);

/*
   for (int idx = 0; idx < 16; idx++)
   {
      printf (" %4.4" PRIx16 "", dst[idx]);
   }

   putchar ('\n');
*/

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
static void expandAdcs64x1_kernel (int16_t        *dst, 
                                   uint64_t const *src)
{
   ///puts ("In expandAdcs64x1_kernel");
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
inline void expandAdcs16x4_kernel (int16_t        *dst, 
                                   uint64_t const *src)
{
#define STRIDE64 (sizeof (WibFrame) / sizeof (*src))

   /*
   puts ("Kernel 16x4");
   printf ("First %4.4x %4.4x %4.4x %4.4x\n",
           ((int16_t const *)src)[0*STRIDE/2],
           ((int16_t const *)src)[1*STRIDE/2],
           ((int16_t const *)src)[2*STRIDE/2],
           ((int16_t const *)src)[3*STRIDE/2]);
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


static inline uint64_t transpose0 (uint64_t w0_0,
                                   uint64_t w1_0,
                                   uint64_t w2_0,
                                   uint64_t w3_0) __attribute ((always_inline));

static inline uint64_t transpose0 (uint64_t w0_0,
                                   uint64_t w1_0,
                                   uint64_t w2_0,
                                   uint64_t w3_0)
{
   uint64_t dst = (D0 (w0_0) << 0 * 16)
                | (D0 (w1_0) << 1 * 16)
                | (D0 (w2_0) << 2 * 16)
                | (D0 (w3_0) << 3 * 16);
   return dst;
}


static inline uint64_t transpose1 (uint64_t w0_0,
                                   uint64_t w1_0,
                                   uint64_t w2_0,
                                   uint64_t w3_0) __attribute ((always_inline));

static inline uint64_t transpose1 (uint64_t w0_0,
                                   uint64_t w1_0,
                                   uint64_t w2_0,
                                   uint64_t w3_0)
{
   uint64_t dst = (D1 (w0_0) << 0 * 16)
                | (D1 (w1_0) << 1 * 16)
                | (D1 (w2_0) << 2 * 16)
                | (D1 (w3_0) << 3 * 16);
   return dst;
}



static inline uint64_t transpose2 (uint64_t w0_0,
                                   uint64_t w1_0,
                                   uint64_t w2_0,
                                   uint64_t w3_0) __attribute ((always_inline));

static inline uint64_t transpose2 (uint64_t w0_0,
                                   uint64_t w1_0,
                                   uint64_t w2_0,
                                   uint64_t w3_0)
{
   uint64_t dst = (D2 (w0_0) << 0 * 16)
                | (D2 (w1_0) << 1 * 16)
                | (D2 (w2_0) << 2 * 16)
                | (D2 (w3_0) << 3 * 16);
   return dst;
}


static inline uint64_t transpose3 (uint64_t w0_0,
                                   uint64_t w1_0,
                                   uint64_t w2_0,
                                   uint64_t w3_0) __attribute ((always_inline));

static inline uint64_t transpose3 (uint64_t w0_0,
                                   uint64_t w1_0,
                                   uint64_t w2_0,
                                   uint64_t w3_0)
{
   uint64_t dst = (D3 (w0_0) << 0 * 16)
                | (D3 (w1_0) << 1 * 16)
                | (D3 (w2_0) << 2 * 16)
                | (D3 (w3_0) << 3 * 16);
   return dst;
}



static inline uint64_t transpose4 (uint64_t w0_0, uint64_t w0_1,
                                   uint64_t w1_0, uint64_t w1_1,
                                   uint64_t w2_0, uint64_t w2_1,
                                   uint64_t w3_0, uint64_t w3_1) 
                                   __attribute ((always_inline));

static inline uint64_t transpose4 (uint64_t w0_0, uint64_t w0_1,
                                   uint64_t w1_0, uint64_t w1_1,
                                   uint64_t w2_0, uint64_t w2_1,
                                   uint64_t w3_0, uint64_t w3_1) 
{
   uint64_t dst = (D4 (w0_1, w0_0) << 0 * 16)
                | (D4 (w1_1, w1_0) << 1 * 16)
                | (D4 (w2_1, w2_0) << 2 * 16)
                | (D4 (w3_1, w3_0) << 3 * 16);
   return dst;
}


static inline uint64_t transpose5 (uint64_t w0_0, uint64_t w0_1,
                                   uint64_t w1_0, uint64_t w1_1,
                                   uint64_t w2_0, uint64_t w2_1,
                                   uint64_t w3_0, uint64_t w3_1) 
                                   __attribute ((always_inline));

static inline uint64_t transpose5 (uint64_t w0_0, uint64_t w0_1,
                                   uint64_t w1_0, uint64_t w1_1,
                                   uint64_t w2_0, uint64_t w2_1,
                                   uint64_t w3_0, uint64_t w3_1) 
{
   uint64_t dst = (D5 (w0_1, w0_0) << 0 * 16)
                | (D5 (w1_1, w1_0) << 1 * 16)
                | (D5 (w2_1, w2_0) << 2 * 16)
                | (D5 (w3_1, w3_0) << 3 * 16);
   return dst;
}


static inline uint64_t transpose6 (uint64_t w0_1,
                                   uint64_t w1_1,
                                   uint64_t w2_1,
                                   uint64_t w3_1)
                    __attribute ((always_inline));

static inline uint64_t transpose6 (uint64_t w0_1,
                                   uint64_t w1_1,
                                   uint64_t w2_1,
                                   uint64_t w3_1)
{
   uint64_t dst = (D6 (w0_1) << 0 * 16)
                | (D6 (w1_1) << 1 * 16)
                | (D6 (w2_1) << 2 * 16)
                | (D6 (w3_1) << 3 * 16);
   return dst;
}


static inline uint64_t transpose7 (uint64_t w0_1,
                                   uint64_t w1_1,
                                   uint64_t w2_1,
                                   uint64_t w3_1)
                    __attribute ((always_inline));

static inline uint64_t transpose7 (uint64_t w0_1,
                                   uint64_t w1_1,
                                   uint64_t w2_1,
                                   uint64_t w3_1)
{
   uint64_t dst = (D7 (w0_1) << 0 * 16)
                | (D7 (w1_1) << 1 * 16)
                | (D7 (w2_1) << 2 * 16)
                | (D7 (w3_1) << 3 * 16);
   return dst;
}


static inline uint64_t transpose8 (uint64_t w0_1,
                                   uint64_t w1_1,
                                   uint64_t w2_1,
                                   uint64_t w3_1)
                    __attribute ((always_inline));

static inline uint64_t transpose8 (uint64_t w0_1,
                                   uint64_t w1_1,
                                   uint64_t w2_1,
                                   uint64_t w3_1)
{
   uint64_t dst = (D8 (w0_1) << 0 * 16)
                | (D8 (w1_1) << 1 * 16)
                | (D8 (w2_1) << 2 * 16)
                | (D8 (w3_1) << 3 * 16);
   return dst;
}


static inline uint64_t transpose9 (uint64_t w0_1,
                                   uint64_t w1_1,
                                   uint64_t w2_1,
                                   uint64_t w3_1)
                    __attribute ((always_inline));

static inline uint64_t transpose9 (uint64_t w0_1,
                                   uint64_t w1_1,
                                   uint64_t w2_1,
                                   uint64_t w3_1)
{
   uint64_t dst = (D9 (w0_1) << 0 * 16)
                | (D9 (w1_1) << 1 * 16)
                | (D9 (w2_1) << 2 * 16)
                | (D9 (w3_1) << 3 * 16);
   return dst;
}



static inline uint64_t transposeA (uint64_t w0_1, uint64_t w0_2,
                                   uint64_t w1_1, uint64_t w1_2,
                                   uint64_t w2_1, uint64_t w2_2,
                                   uint64_t w3_1, uint64_t w3_2) 
                                   __attribute ((always_inline));

static inline uint64_t transposeA (uint64_t w0_1, uint64_t w0_2,
                                   uint64_t w1_1, uint64_t w1_2,
                                   uint64_t w2_1, uint64_t w2_2,
                                   uint64_t w3_1, uint64_t w3_2) 
{
   uint64_t dst = (DA (w0_2, w0_1) << 0 * 16)
                | (DA (w1_2, w1_1) << 1 * 16)
                | (DA (w2_2, w2_1) << 2 * 16)
                | (DA (w3_2, w3_1) << 3 * 16);
   return dst;
}



static inline uint64_t transposeB (uint64_t w0_1, uint64_t w0_2,
                                   uint64_t w1_1, uint64_t w1_2,
                                   uint64_t w2_1, uint64_t w2_2,
                                   uint64_t w3_1, uint64_t w3_2) 
                                   __attribute ((always_inline));

static inline uint64_t transposeB (uint64_t w0_1, uint64_t w0_2,
                                   uint64_t w1_1, uint64_t w1_2,
                                   uint64_t w2_1, uint64_t w2_2,
                                   uint64_t w3_1, uint64_t w3_2) 
{
   uint64_t dst = (DB (w0_2, w0_1) << 0 * 16)
                | (DB (w1_2, w1_1) << 1 * 16)
                | (DB (w2_2, w2_1) << 2 * 16)
                | (DB (w3_2, w3_1) << 3 * 16);
   return dst;
}


static inline uint64_t transposeC (uint64_t w0_2,
                                   uint64_t w1_2,
                                   uint64_t w2_2,
                                   uint64_t w3_2)
                    __attribute ((always_inline));

static inline uint64_t transposeC (uint64_t w0_2,
                                   uint64_t w1_2,
                                   uint64_t w2_2,
                                   uint64_t w3_2)
{
   uint64_t dst = (DC (w0_2) << 0 * 16)
                | (DC (w1_2) << 1 * 16)
                | (DC (w2_2) << 2 * 16)
                | (DC (w3_2) << 3 * 16);
   return dst;
}


static inline uint64_t transposeD (uint64_t w0_2,
                                   uint64_t w1_2,
                                   uint64_t w2_2,
                                   uint64_t w3_2)
                    __attribute ((always_inline));

static inline uint64_t transposeD (uint64_t w0_2,
                                   uint64_t w1_2,
                                   uint64_t w2_2,
                                   uint64_t w3_2)
{
   uint64_t dst = (DD (w0_2) << 0 * 16)
                | (DD (w1_2) << 1 * 16)
                | (DD (w2_2) << 2 * 16)
                | (DD (w3_2) << 3 * 16);
   return dst;
}


static inline uint64_t transposeE (uint64_t w0_2,
                                   uint64_t w1_2,
                                   uint64_t w2_2,
                                   uint64_t w3_2)
                    __attribute ((always_inline));

static inline uint64_t transposeE (uint64_t w0_2,
                                   uint64_t w1_2,
                                   uint64_t w2_2,
                                   uint64_t w3_2)
{
   uint64_t dst = (DE (w0_2) << 0 * 16)
                | (DE (w1_2) << 1 * 16)
                | (DE (w2_2) << 2 * 16)
                | (DE (w3_2) << 3 * 16);
   return dst;
}


static inline uint64_t transposeF (uint64_t w0_2,
                                   uint64_t w1_2,
                                   uint64_t w2_2,
                                   uint64_t w3_2)
                    __attribute ((always_inline));

static inline uint64_t transposeF (uint64_t w0_2,
                                   uint64_t w1_2,
                                   uint64_t w2_2,
                                   uint64_t w3_2)
{
   uint64_t dst = (DF (w0_2) << 0 * 16)
                | (DF (w1_2) << 1 * 16)
                | (DF (w2_2) << 2 * 16)
                | (DF (w3_2) << 3 * 16);
   return dst;
}



/* ---------------------------------------------------------------------- *//*!

  \brief  Calclulate a 64-bit address offset 16 bit index
  \return The calculated address

  \param[in]    ptr64  The 64-bit base address
  \param[in] offset16  The 16-bit offset
                                                                          */
/* ---------------------------------------------------------------------- */
static inline uint64_t *adr64 (uint64_t *ptr64, int offset16)
{
   uint16_t *ptr16  = reinterpret_cast<uint16_t *>(ptr64) + offset16;
   ptr64 = reinterpret_cast<uint64_t *>(ptr16);
   return ptr64;
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


   // Channel = 0x0
  *dst64  = transpose0 (w0_0, w1_0, w2_0, w3_0);
   dst64  = adr64 (dst64, ndstStride);
   //printf ("t16x4[0] = %16.16" PRIx64 "\n", dst);
   
   // Channel  0x1
   dst    = transpose1 (w0_0, w1_0, w2_0, w3_0);
  *dst64  = dst;
   dst64  = adr64 (dst64, ndstStride);
   //printf ("t16x4[1] = %16.16" PRIx64 "\n", dst);

   
   // Channel  0x2
   dst    = transpose2 (w0_0, w1_0, w2_0, w3_0);
  *dst64  = dst;
   dst64  = adr64 (dst64, ndstStride);

   
   // Channel  0x3
   dst    = transpose3 (w0_0, w1_0, w2_0, w3_0);
  *dst64  = dst;
   dst64  = adr64 (dst64, ndstStride);


   uint64_t w0_1 = *src0++;  
   uint64_t w1_1 = *src1++;  
   uint64_t w2_1 = *src2++;  
   uint64_t w3_1 = *src3++;  


   // Channel 0x4
  *dst64  = transpose4 (w0_0,w0_1,  w1_0,w1_1,  w2_0,w2_1,  w3_0,w3_1);
   dst64  = adr64 (dst64, ndstStride);
   //printf ("Channel 4 = %16.16" PRIx64 "\n", dst);


   // Channel 0x5
  *dst64  = transpose5 (w0_0,w0_1,  w1_0,w1_1,  w2_0,w2_1,  w3_0,w3_1);
   dst64  = adr64 (dst64, ndstStride);


   // Channel 0x6
  *dst64  = transpose6 (w0_1, w1_1, w2_1, w3_1);
   dst64  = adr64 (dst64, ndstStride);


   // Channel 0x7
  *dst64  = transpose7 (w0_1, w1_1, w2_1, w3_1);
   dst64  = adr64 (dst64, ndstStride);

   
   // Channel 0x8
  *dst64  = transpose8 (w0_1, w1_1, w2_1, w3_1);
   dst64  = adr64 (dst64, ndstStride);

   
   // Channel 0x9
  *dst64  = transpose9 (w0_1, w1_1, w2_1, w3_1);
   dst64  = adr64 (dst64, ndstStride);
   //printf ("t16x4[9] = %16.16" PRIx64 "\n", dst);
   
   uint64_t w0_2 = *src0++;  
   uint64_t w1_2 = *src1++;  
   uint64_t w2_2 = *src2++;  
   uint64_t w3_2 = *src3++;  
      

   // Chanel 0xA
  *dst64  = transposeA (w0_1,w0_2,  w1_1,w1_2,  w2_1,w2_2,  w3_1,w3_2);
   dst64  = adr64 (dst64, ndstStride);
   

   // Channel 0xB
  *dst64  = transposeB (w0_1,w0_2,  w1_1,w1_2,  w2_1,w2_2,  w3_1,w3_2);
   dst64  = adr64 (dst64, ndstStride);
      

   // Channel 0xC
  *dst64  = transposeC (w0_2, w1_2, w2_2, w3_2);
   dst64  = adr64 (dst64, ndstStride);

   // Channel 0xD  
  *dst64  = transposeD (w0_2, w1_2, w2_2, w3_2);
   dst64  = adr64 (dst64, ndstStride);

   
   // Channel 0xE
  *dst64  = transposeE (w0_2, w1_2, w2_2, w3_2);
   dst64  = adr64 (dst64, ndstStride);


   // Channel 0xF
  *dst64  = transposeF (w0_2, w1_2, w2_2, w3_2);

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
static inline void transposeAdcs16x8_kernel (int16_t        *dst, 
                                             int      ndstStride,
                                             uint64_t const *src)
{
   uint64_t  *dst64 = (uint64_t *)dst;
   //int ndstStride64 = ndstStride * sizeof (*dst) / sizeof (*dst64);


   transposeAdcs16x4_kernel (dst64,   ndstStride, src);
   transposeAdcs16x4_kernel (dst64+1, ndstStride, 
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
   dst64[0][offset] = transpose0 (w0_0, w1_0, w2_0, w3_0);
   print (dst64, 0, offset);

   
   // Channel  1
   dst64[1][offset] = transpose1 (w0_0, w1_0, w2_0, w3_0);
   print (dst64, 1, offset);

   
   // Channel  2
   dst64[2][offset] = transpose2 (w0_0, w1_0, w2_0, w3_0);
   print (dst64, 2, offset);

   
   // Channel  3
   dst64[3][offset] = transpose3 (w0_0, w1_0, w2_0, w3_0);
   print (dst64, 3, offset);


   uint64_t w0_1 = *src0++;  
   uint64_t w1_1 = *src1++;  
   uint64_t w2_1 = *src2++;  
   uint64_t w3_1 = *src3++;  


   // Channel 4
   dst64[4][offset] = transpose4 (w0_0,w0_1,  w1_0,w1_1,  w2_0,w2_1,  w3_0,w3_1);
   print (dst64, 4, offset);
   

   // Channel 5
   dst64[5][offset] = transpose5 (w0_0,w0_1,  w1_0,w1_1,  w2_0,w2_1,  w3_0,w3_1);
   print (dst64, 5, offset);


   // Channel 6
   dst64[6][offset] = transpose6 (w0_1, w1_1, w2_1, w3_1);
   print (dst64, 6, offset);


   // Channel 7
   dst64[7][offset] = transpose7 (w0_1, w1_1, w2_1, w3_1);
   print (dst64, 7, offset);
   

   // Channel 8
   dst    = D8 (w0_1)       << 0 * 16;
   dst   |= D8 (w1_1)       << 1 * 16;
   dst   |= D8 (w2_1)       << 2 * 16;
   dst   |= D8 (w3_1)       << 3 * 16;
   dst64[8][offset] = transpose8 (w0_1, w1_1, w2_1, w3_1);
   print (dst64, 8, offset);

   
   // Channel 9
   dst    = D9 (w0_1)       << 0 * 16;
   dst   |= D9 (w1_1)       << 1 * 16;
   dst   |= D9 (w2_1)       << 2 * 16;
   dst   |= D9 (w3_1)       << 3 * 16;
   dst64[9][offset] = transpose9 (w0_1, w1_1, w2_1, w3_1);
   print (dst64, 9, offset);


   uint64_t w0_2 = *src0++;  
   uint64_t w1_2 = *src1++;  
   uint64_t w2_2 = *src2++;  
   uint64_t w3_2 = *src3++;  
      

   // Chanel A
   dst64[10][offset] = transposeA (w0_1,w0_2,  w1_1,w1_2,  w2_1,w2_2,  w3_1,w3_2);
   print (dst64, 10, offset);   


   // Channel B
   dst64[11][offset] = transposeB (w0_1,w0_2,  w1_1,w1_2,  w2_1,w2_2,  w3_1,w3_2);
   print (dst64, 11, offset);
      

   // Channel C
   dst64[12][offset] = transposeC (w0_2, w1_2, w2_2, w3_2);
   print (dst64, 12, offset);


   // Channel D
   dst64[13][offset] = transposeD (w0_2, w1_2, w2_2, w3_2);
   print (dst64, 13, offset);


   // Channel 1E
   dst64[14][offset] = transposeE (w0_2, w1_2, w2_2, w3_2);
   print (dst64, 14, offset);


   // Channel F
   dst64[15][offset] = transposeF (w0_2, w1_2, w2_2, w3_2);
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
static inline void transposeAdcs16x8_kernel (int16_t  *const *dst, 
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
