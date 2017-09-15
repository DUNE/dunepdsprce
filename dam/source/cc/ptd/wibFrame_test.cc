#define __STDC_FORMAT_MACROS

#include "dam/WibFrame.hh"
#include <cinttypes>
#include <cstdio>
#include <sys/time.h>
#include <malloc.h>


using namespace pdd::fragment;

static void create (                   uint16_t       *pattern, int npattern);
static void fill16 (uint64_t    *srcs, uint16_t const *pattern) __attribute__((unused));
static void fill64 (uint64_t    *srcs, uint16_t const *pattern) __attribute__((unused));
static void fill   (pdd::fragment::WibFrame  *frames, int nframes, uint16_t const *pattern);

static        void print                  (char  const       *title,
                                           struct timeval *const dif,
                                           int              ntrials) __attribute__((unused));

static inline void expandAdcs16x1_cpuA_kernel (uint16_t *dst, uint64_t const *src);
static void inline expandAdcs16x1_cpuB_kernel (uint16_t *dst, uint64_t const *src);
static void inline expandAdcs16x1_cpuC_kernel (uint16_t *dst, uint64_t const *src);

/* ---------------------------------------------------------------------- */
int main (int argc, char *const argv[])
{

   using namespace pdd::fragment;

   // ----------------------------------------------------------------------------
   // This roughly the number of samples in one packet 256 channels * 1024 samples
   // ----------------------------------------------------------------------------   
   #define NFRAMES_PER_TRIAL   2048
   #define NCHANNELS_PER_FRAME  128
   #define NTRIALS               32
   #define NPATTERNS_PER_TRIAL (NFRAMES_PER_TRIAL * NCHANNELS_PER_FRAME)

   uint16_t *pattern = (uint16_t *)memalign (32, sizeof (*pattern) * NPATTERNS_PER_TRIAL * NTRIALS);
   uint16_t  *dstBuf = (uint16_t *)memalign (32, sizeof (*dstBuf ) * NPATTERNS_PER_TRIAL * NTRIALS);
   WibFrame  *frames = (WibFrame *)memalign (32, sizeof (*frames ) * NFRAMES_PER_TRIAL   * NTRIALS);
                                             
   int           npattern = NPATTERNS_PER_TRIAL * NTRIALS;
   int            ntrials = NTRIALS;
   int  nframes_per_trial = NFRAMES_PER_TRIAL;

   create (pattern, npattern);
   for (int itrial = 0; itrial < ntrials; ++itrial)
   {
      fill (&frames[nframes_per_trial * itrial], 
            nframes_per_trial, 
            pattern + itrial * NFRAMES_PER_TRIAL * NCHANNELS_PER_FRAME);
   }


   uint64_t const (&src)[12] = frames[0].getColdData ()[0].locateAdcs12b ();
   for (int idx = 0; idx < 9; ++idx)
   {
      printf (" %2.2x: %16.16" PRIx64 "\n", idx, src[idx]);
   }



   struct timeval  dif[NTRIALS];



#if 1
   // --------------------
   // AVX2 64x1 Unpacking
   // --------------------
   {
   uint16_t       *dst = dstBuf;
   for (int itrial = 0; itrial < ntrials; ++itrial) 
   {
      struct timeval beg;
      gettimeofday (&beg, NULL);

      for (int iframe = 0; iframe < nframes_per_trial; ++iframe)
      { 
         WibFrame::ColdData const (&cd)[2] = frames[iframe].getColdData ();

         cd[0].expandAdcs64x1 (dst + 0*64);
         cd[1].expandAdcs64x1 (dst + 1*64);
         dst += 128;
      }

      struct timeval end;
      gettimeofday (&end, NULL);

      timersub (&end, &beg, dif + itrial);

   }
   print ("AVX2 64x1", dif, ntrials);
   }
#endif


#if 1
   // --------------------
   // AVX2 128x1 Unpacking
   // --------------------
   {
   uint16_t       *dst = dstBuf;
   for (int itrial = 0; itrial < ntrials; ++itrial) 
   {
      struct timeval beg;
      gettimeofday (&beg, NULL);
      for (int iframe = 0; iframe < nframes_per_trial; ++iframe)
      { 
         frames[iframe].expandAdcs128x1 (dst);
         dst += 128;
      }
      struct timeval end;
      gettimeofday (&end, NULL);

      timersub (&end, &beg, dif + itrial);

   }
   print ("AVX2 128x1", dif, ntrials);
   }
#endif




#if 1
   // ---------------
   // CPU A Unpacking
   // ---------------
   {
   uint16_t       *dst = dstBuf;
   for (int itrial = 0; itrial < ntrials; ++itrial) 
   {
      struct timeval beg;
      gettimeofday (&beg, NULL);
      for (int iframe = 0; iframe < nframes_per_trial; ++iframe)
      { 
         uint64_t const (&src0)[12] = frames[iframe].getColdData()[0].locateAdcs12b ();
         uint64_t const (&src1)[12] = frames[iframe].getColdData()[1].locateAdcs12b ();

         expandAdcs16x1_cpuA_kernel (dst + 0*16, &src0[0]);
         expandAdcs16x1_cpuA_kernel (dst + 1*16, &src0[3]);
         expandAdcs16x1_cpuA_kernel (dst + 2*16, &src0[6]);
         expandAdcs16x1_cpuA_kernel (dst + 3*16, &src0[9]);

         expandAdcs16x1_cpuA_kernel (dst + 4*16, &src1[0]);
         expandAdcs16x1_cpuA_kernel (dst + 5*16, &src1[3]);
         expandAdcs16x1_cpuA_kernel (dst + 6*16, &src1[6]);
         expandAdcs16x1_cpuA_kernel (dst + 7*16, &src1[9]);

         dst += 128;
      }
      struct timeval end;
      gettimeofday (&end, NULL);

      timersub (&end, &beg, dif + itrial);

   }
   print ("CPU A", dif, ntrials);
   }
#endif


#if 1
   // ---------------
   // CPU B Unpacking
   // ---------------
   {
   uint16_t       *dst = dstBuf;
   for (int itrial = 0; itrial < ntrials; ++itrial) 
   {
      struct timeval beg;
      gettimeofday (&beg, NULL);
      for (int iframe = 0; iframe < nframes_per_trial; ++iframe)
      { 
         uint64_t const (&src0)[12] = frames[iframe].getColdData()[0].locateAdcs12b ();
         uint64_t const (&src1)[12] = frames[iframe].getColdData()[1].locateAdcs12b ();

         expandAdcs16x1_cpuB_kernel (dst + 0*16, &src0[0]);
         expandAdcs16x1_cpuB_kernel (dst + 1*16, &src0[3]);
         expandAdcs16x1_cpuB_kernel (dst + 2*16, &src0[6]);
         expandAdcs16x1_cpuB_kernel (dst + 3*16, &src0[9]);

         expandAdcs16x1_cpuB_kernel (dst + 4*16, &src1[0]);
         expandAdcs16x1_cpuB_kernel (dst + 5*16, &src1[3]);
         expandAdcs16x1_cpuB_kernel (dst + 6*16, &src1[6]);
         expandAdcs16x1_cpuB_kernel (dst + 7*16, &src1[9]);

         dst += 128;
      }
      struct timeval end;
      gettimeofday (&end, NULL);

      timersub (&end, &beg, dif + itrial);

   }
   print ("CPU B", dif, ntrials);
   }
#endif

#if 1
   // ---------------
   // CPU C Unpacking
   // ---------------
   {
   uint16_t       *dst = dstBuf;
   for (int itrial = 0; itrial < ntrials; ++itrial) 
   {
      struct timeval beg;
      gettimeofday (&beg, NULL);
      for (int iframe = 0; iframe < nframes_per_trial; ++iframe)
      { 
         uint64_t const (&src0)[12] = frames[iframe].getColdData()[0].locateAdcs12b ();
         uint64_t const (&src1)[12] = frames[iframe].getColdData()[1].locateAdcs12b ();

         expandAdcs16x1_cpuC_kernel (dst + 0*16, &src0[0]);
         expandAdcs16x1_cpuC_kernel (dst + 1*16, &src0[3]);
         expandAdcs16x1_cpuC_kernel (dst + 2*16, &src0[6]);
         expandAdcs16x1_cpuC_kernel (dst + 3*16, &src0[9]);

         expandAdcs16x1_cpuC_kernel (dst + 4*16, &src1[0]);
         expandAdcs16x1_cpuC_kernel (dst + 5*16, &src1[3]);
         expandAdcs16x1_cpuC_kernel (dst + 6*16, &src1[6]);
         expandAdcs16x1_cpuC_kernel (dst + 7*16, &src1[9]);

         dst += 128;
      }
      struct timeval end;
      gettimeofday (&end, NULL);

      timersub (&end, &beg, dif + itrial);

   }
   print ("CPU C", dif, ntrials);
   }

#endif

#if 1
   // --------------------
   // AVX2 16x8 transpose
   // --------------------
   {
   uint16_t       *dst = dstBuf;
   for (int itrial = 0; itrial < ntrials; ++itrial) 
   {

      struct timeval beg;
      gettimeofday (&beg, NULL);

      for (int iframe = 0; iframe < nframes_per_trial; iframe += 8)
      { 
         WibFrame::transposeAdcs128x8 (dst, nframes_per_trial, frames + iframe);
         dst += 128*8;
      }
      struct timeval end;
      gettimeofday (&end, NULL);

      timersub (&end, &beg, dif + itrial);

   }
   print ("AVX2 transpose128x8", dif, ntrials);
   }
#endif

#if 1
   // --------------------
   // AVX2 16x16 transpose
   // --------------------
   {
   uint16_t       *dst = dstBuf;
   for (int itrial = 0; itrial < ntrials; ++itrial) 
   {

      struct timeval beg;
      gettimeofday (&beg, NULL);
      #if 1
      WibFrame::transposeAdcs128x16N (dst, nframes_per_trial, frames, nframes_per_trial);
      #else
      for (int iframe = 0; iframe < nframes_per_trial; iframe += 16)
      { 
         WibFrame::transposeAdcs128x16N (dst, nframes_per_trial, frames + iframe, 16);
         dst += 128*16;
      }
      #endif
      struct timeval end;
      gettimeofday (&end, NULL);

      timersub (&end, &beg, dif + itrial);

   }
   print ("AVX2 transpose128x16", dif, ntrials);
   }
#endif

#if 1
   // --------------------
   // AVX2 16x32 transpose
   // --------------------
   {
   uint16_t       *dst = dstBuf;
   for (int itrial = 0; itrial < ntrials; ++itrial) 
   {
      struct timeval beg;
      gettimeofday (&beg, NULL);

      #if 0
      WibFrame::transposeAdcs128x32N (dst, nframes_per_trial, frames, nframes_per_trial);
      #else
      for (int iframe = 0; iframe < nframes_per_trial; iframe += 32)
      { 
         WibFrame::transposeAdcs128x32N (dst, nframes_per_trial, frames, 32);
         //dst += 128*32;
      }
      #endif
      struct timeval end;
      gettimeofday (&end, NULL);

      timersub (&end, &beg, dif + itrial);

   }
   print ("AVX2 Transpose128x32", dif, ntrials);
   }
#endif

   return 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static void create (uint16_t *pattern, int npattern)
{
   for (int itime = 0; itime < npattern/128; ++itime)
   {
      uint16_t p = itime << 4;
      for (int ichan = 0; ichan < 128; ++ichan)
      {
         *pattern++ = p | (ichan & 0xf);
      }
   }
}
/* ---------------------------------------------------------------------- */




static inline uint64_t packA (uint16_t v0, 
                              uint16_t v1,
                              uint16_t v2,
                              uint16_t v3,
                              uint16_t v4,
                              uint16_t v5);


static inline uint64_t packB (uint16_t v0, 
                              uint16_t v1,
                              uint16_t v2,
                              uint16_t v3,
                              uint16_t v4,
                              uint16_t v5);


static inline uint64_t packC (uint16_t v0, 
                              uint16_t v1,
                              uint16_t v2,
                              uint16_t v3,
                              uint16_t v4,
                              uint16_t v5);

/* ---------------------------------------------------------------------- */
static void fill (WibFrame *frames, int nframes, uint16_t const *pattern)
{
   for (int iframe = 0; iframe < nframes; iframe++)
   {
      WibFrame::ColdData (&cd)[2] = frames[iframe].getColdData ();
      uint64_t *adcs0 = cd[0].locateAdcs12b ();
      uint64_t *adcs1 = cd[1].locateAdcs12b ();

      fill64 (adcs0, pattern);
      fill64 (adcs1, pattern + 64);

      pattern += 128;
   }
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static void fill16 (uint64_t *src, uint16_t const *pattern)
{

   uint16_t const *p = pattern;
   uint64_t       *s = src;

   s[0] = packA (p[ 0], p[ 1], p[ 2], p[ 3], p[ 4], p[ 5]);
   s[1] = packB (p[ 5], p[ 6], p[ 7], p[ 8], p[ 9], p[10]);
   s[2] = packC (p[10], p[11], p[12], p[13], p[14], p[15]);
   

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
static void fill64 (uint64_t *src, uint16_t const *pattern)
{

   uint16_t const *p = pattern;
   uint64_t       *s = src;

   for (int idx = 0; idx < 4; idx++)
   {
      s[0] = packA (p[ 0], p[ 1], p[ 2], p[ 3], p[ 4], p[ 5]);
      s[1] = packB (p[ 5], p[ 6], p[ 7], p[ 8], p[ 9], p[10]);
      s[2] = packC (p[10], p[11], p[12], p[13], p[14], p[15]);

      p   += 16;
      s   += 3;
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static inline uint64_t packA (uint16_t v0, 
                              uint16_t v1,
                              uint16_t v2,
                              uint16_t v3,
                              uint16_t v4,
                              uint16_t v5)
{
   uint64_t s;

   s   = v5;

   s <<= 12;
   s  |= v4;

   s <<= 12;
   s  |= v3;

   s <<= 12;
   s  |= v2;

   s <<= 12;
   s  |= v1;

   s <<= 12;
   s  |= v0;

   return s;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static inline uint64_t packB (uint16_t v0, 
                              uint16_t v1,
                              uint16_t v2,
                              uint16_t v3,
                              uint16_t v4,
                              uint16_t v5)
{
   uint64_t s;

   s   = v5;

   s <<= 12;
   s  |= v4;

   s <<= 12;
   s  |= v3;

   s <<= 12;
   s  |= v2;

   s <<= 12;
   s  |= v1;

   s <<= 8;
   s  |= (v0 >> 4);

   return s;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static inline uint64_t packC (uint16_t v0, 
                              uint16_t v1,
                              uint16_t v2,
                              uint16_t v3,
                              uint16_t v4,
                              uint16_t v5)
{
   uint64_t s;

   s   = v5;

   s <<= 12;
   s  |= v4;

   s <<= 12;
   s  |= v3;

   s <<= 12;
   s  |= v2;

   s <<= 12;
   s  |= v1;

   s <<= 4;
   s  |= (v0 >> 8);

   return s;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static void print (char const *title, struct timeval *const dif, int ntrials)
{
   puts (title);
   int ncolBeg =  printf ("Elapsed:");
   int ncols   =  ncolBeg;
   for (int itrial = 0; itrial < ntrials; ++itrial) 
   {
      if (ncols > 64) 
      {
         ncols = printf ("\n%*c", ncolBeg, ' ') - 1;
      }
      ncols += printf (" %6u.%06u", 
                       (unsigned)dif[itrial].tv_sec, (unsigned)dif[itrial].tv_usec);
   }
   putchar ('\n');

   return;
}
/* ---------------------------------------------------------------------- */


static inline uint64_t _bextr_u64(uint64_t src, uint8_t beg, uint8_t end)
{
   uint64_t result;
   uint64_t ctl = (end << 8) | (beg << 0);
   asm ("bextr %0,%2,%1" : "=r"(result) : "r"(src),"r"(ctl));
   return result;
}


/* ---------------------------------------------------------------------- */
static void inline expandAdcs16x1_cpuA_kernel (uint16_t *dst, uint64_t const *src)
{
   uint64_t w0;
   uint64_t w1;
   uint64_t w2;


   w0 = *src++;  
   dst[0] = _bextr_u64(w0, 0,  11);
   dst[1] = _bextr_u64(w0, 12, 23);
   dst[2] = _bextr_u64(w0, 24, 35);
   dst[3] = _bextr_u64(w0, 36, 47);
   dst[4] = _bextr_u64(w0, 48, 60);


   w1 = *src++; 
   dst[5] = (_bextr_u64 (w1, 0, 7) << 4) | _bextr_u64 (w0, 60, 63);
   dst[6] = _bextr_u64(w1,  8, 19);
   dst[7] = _bextr_u64(w1, 20, 31);
   dst[8] = _bextr_u64(w1, 32, 43);
   dst[9] = _bextr_u64(w1, 44, 55);


   w2 = *src++; 
   dst[10] = (_bextr_u64 (w2, 0, 4) << 8) | _bextr_u64 (w1, 56, 63);
   dst[11] = _bextr_u64(w2,  4, 15);
   dst[12] = _bextr_u64(w2, 16, 27);
   dst[13] = _bextr_u64(w2, 28, 39);
   dst[14] = _bextr_u64(w2, 40, 51);
   dst[15] = _bextr_u64(w2, 52, 63);

   return;
}



static void inline expandAdcs16x1_cpuB_kernel (uint16_t *dst, uint64_t const *src)
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
   dst[10] = ((w2 & 0xf) << 8) | (w0 >> 56);
   dst[11] = (w1 >>  4) & 0xfff;
   dst[12] = (w1 >> 16) & 0xfff;
   dst[13] = (w1 >> 28) & 0xfff;
   dst[14] = (w1 >> 40) & 0xfff;
   dst[15] = (w1 >> 52) & 0xfff;

   return;
}



static void inline expandAdcs16x1_cpuC_kernel (uint16_t *dst, uint64_t const *src)
{
   uint64_t w0;
   uint64_t w1;
   uint64_t w2;

   w0 = *src++;  dst[0] = w0 & 0xfff;     
   w0 >>= 12;    dst[1] = w0 & 0xfff;
   w0 >>= 12;    dst[2] = w0 & 0xfff;
   w0 >>= 12;    dst[3] = w0 & 0xfff;
   w0 >>= 12;    dst[4] = w0 & 0xfff;

   w1 = *src++; dst[5] = ((w1 & 0xff) << 4) | w0;
   w1 >>=  8;    dst[6] = w1 & 0xfff;
   w1 >>= 12;    dst[7] = w1 & 0xfff;
   w1 >>= 12;    dst[8] = w1 & 0xfff;
   w1 >>= 12;    dst[9] = w1 & 0xfff;

   w2 = *src++; dst[10] = ((w2 & 0xf) << 8) | w1;
   w2 >>=  4;    dst[11] = w2 & 0xfff;
   w2 >>= 12;    dst[12] = w2 & 0xfff;
   w2 >>= 12;    dst[13] = w2 & 0xfff;
   w2 >>= 12;    dst[14] = w2 & 0xfff;
   w2 >>= 12;    dst[15] = w2 & 0xfff;

   return;
}
/* ---------------------------------------------------------------------- */

   
