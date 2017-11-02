/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     PdWibFrame_test.cc
 *  @brief    Tests integrity and performance of the PROTO-DUNE RCEs data 
 *            access methods to a raw WIB frame by generating fake frames
 *            with a known pattern
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
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.10.31 jjr Added documentation. Name -> PdWibFrameTest.  The 
                  previous name, wibFrame_test was to generic
   2017.07.27 jjr Created
  
\* ---------------------------------------------------------------------- */



#define __STDC_FORMAT_MACROS

#include "dam/access/WibFrame.hh"
#include "dam/TpcAdcVector.hh"
#include <cinttypes>
#include <cstdio>
#include <sys/time.h>
#include <stdlib.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iostream>



using namespace pdd::access;


static void create_tc     (uint16_t               *patterns,
                           int                    npatterns);

static void create_random (uint16_t               *patterns,
                           int                    npatterns);


static void fill16        (uint64_t                   *srcs,
                           uint16_t const         *patterns) __attribute__((unused));

static void fill64        (uint64_t                   *srcs,
                           uint16_t const         *patterns) __attribute__((unused));

static void fill         (WibFrame                 *frames,
                          int                      nframes,
                          uint16_t const         *patterns);


static void test_integrity         (uint16_t                 *dstBuf, 
                                    WibFrame const           *frames,
                                    int            nframes_per_trial,
                                    int                      ntrials,
                                    uint16_t const         *patterns,
                                    int          npatterns_per_trial,
                                    int                    npatterns) __attribute__((unused));

static void test_performance       (uint16_t                 *dstBuf, 
                                    WibFrame         const   *frames,
                                    int            nframes_per_trial,
                                    int                      ntrials,
                                    uint16_t         const *patterns,
                                    int          npatterns_per_trial,
                                    int                    npatterns) __attribute__((unused));


static void test_integrityPtrArray (uint16_t       *const  *dstPtrs, 
                                    WibFrame        const   *frames,
                                    int           nframes_per_trial,
                                    int                     ntrials,
                                    uint16_t        const *patterns,
                                    int         npatterns_per_trial,
                                    int                   npatterns) __attribute__((unused));

static void test_performancePtrArray (uint16_t     *const  *dstPtrs, 
                                      WibFrame      const   *frames,
                                      int          frames_per_trial,
                                      int                   ntrials,
                                      uint16_t      const *patterns,
                                      int       npatterns_per_trial,
                                      int                 npatterns) __attribute__((unused));


static void test_integrityVector    (std::vector<TpcAdcVector> 
                                                                   *dstVecs,
                                     WibFrame               const   *frames,
                                    int                   nframes_per_trial,
                                    int                             ntrials,
                                    uint16_t                const *patterns,
                                    int                 npatterns_per_trial,
                                    int                           npatterns) __attribute__((unused));

static void test_performanceVector  (std::vector<TpcAdcVector> 
                                                                   *dstVecs,
                                     WibFrame               const   *frames,
                                     int                  nframes_per_trial,
                                     int                            ntrials,
                                     uint16_t               const *patterns,
                                     int                npatterns_per_trial,
                                     int                          npatterns) __attribute__((unused));


static int check_expansion   (uint16_t const   *results, 
                              uint16_t const  *patterns,
                              int             npatterns) __attribute__((unused));

static int check_transpose   (uint16_t const   *results,
                              uint16_t const  *patterns,
                              int             nchannels,
                              int                stride) __attribute__((unused));

static int check_transpose  (uint16_t *const  *results,
                             uint16_t  const  *patterns,
                             int              nchannels,
                             int                nframes) __attribute__((unused));

   

static void print  (char            const     *title,
                    int                        nerrs,
                    struct timeval  const       *dif,
                    int                      ntrials,
                    int                    npatterns) __attribute__((unused));

typedef void  (*CreateMethod)(uint16_t *patterns, int npatterns);

struct TestPattern
{
   char const    *name;
   CreateMethod create;
};


const TestPattern TestPatternSuite[2] =
{
   { "Time:Channel", create_tc     },
   { "Random",       create_random }
};


/* ---------------------------------------------------------------------- */
static void *memAlign (int alignment, unsigned int size)
{
  void *ptr;
  posix_memalign (&ptr, alignment, size);
  return ptr;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
int main (int argc, char *const argv[])
{
   using namespace pdd::access;

   // -----------------------------------------------------------------------
   // This is the number of samples in one packet 256 channels * 1024 samples
   // -----------------------------------------------------------------------
   #define NFRAMES_PER_TRIAL   2048
   #define NCHANNELS_PER_FRAME  128
   #define NTRIALS               25
   #define NPATTERNS_PER_TRIAL (NCHANNELS_PER_FRAME * NFRAMES_PER_TRIAL)
   #define NPATTERNS           (NPATTERNS_PER_TRIAL * NTRIALS)
   #define NFRAMES             (NFRAMES_PER_TRIAL   * NTRIALS)

   uint16_t  *patterns = (uint16_t *)memAlign (64, sizeof (*patterns) * NPATTERNS);
   uint16_t    *dstBuf = (uint16_t *)memAlign (64, sizeof (*dstBuf  ) * NPATTERNS);
   WibFrame    *frames = (WibFrame *)memAlign (64, sizeof (*frames  ) *   NFRAMES);
   uint16_t  *dstPtrs[NCHANNELS_PER_FRAME * NTRIALS];
      
   int            npatterns = NPATTERNS;
   int              ntrials = NTRIALS;
   int    nframes_per_trial = NFRAMES_PER_TRIAL;
   int  npatterns_per_trial = NPATTERNS_PER_TRIAL;

   int        ntestPatterns = sizeof (TestPatternSuite) / sizeof (*TestPatternSuite);
   TestPattern const  *test;

   // -------------------------------------------------------
   // Allocate the memory for the channel-by-channel pointers
   // -------------------------------------------------------
   for (unsigned int idx = 0; idx < sizeof (dstPtrs) / sizeof (*dstPtrs); ++idx)
   {
      dstPtrs[idx] = (uint16_t *)memAlign (64, nframes_per_trial * sizeof (*dstPtrs[idx]));
      //dstPtrs[idx] = dstBuf + idx * nframes_per_trial;
   }


   std::vector<TpcAdcVector> dstVecs (NCHANNELS_PER_FRAME * NTRIALS);
   printf ("Vector capacity = %lu\n", dstVecs.capacity ());

   // -------------------------------------------------------
   // Allocate the memory for the channel-by-channel pointers
   // -------------------------------------------------------
   for (unsigned int idx = 0; idx < dstVecs.capacity (); ++idx)
   {
      TpcAdcVector &dstVec = dstVecs[idx];
      dstVec.reserve (nframes_per_trial);
      /*
      printf ("Vector[%4d] @ %p data @ %p capacity = %lu\n", 
              idx,
              (void *)&dstVec,
              (void *)dstVec.data (),
              dstVec.capacity ());
      */
   }




   // ----------------------------------------------------------------------
   // Integrity checks: contigious memory
   // -----------------------------------
   test = TestPatternSuite;
   for (int itestPattern = 0; itestPattern < ntestPatterns; ++itestPattern, ++test)
   {

      printf ("\nIntegrity check contiguous: using pattern = %s\n", test->name);
      (*test->create) (patterns, npatterns);

      for (int itrial = 0; itrial < ntrials; ++itrial)
      {
         fill (frames    + itrial * nframes_per_trial, 
               nframes_per_trial, 
               patterns  + itrial * npatterns_per_trial);
      }

      test_integrity   (dstBuf, frames,   nframes_per_trial, ntrials, 
                        patterns, npatterns_per_trial, npatterns);
   }
   // ----------------------------------------------------------------------




   // ----------------------------------------------------------------------
   // Integrity check: channel-by-channel memory
   // ------------------------------------------ 
   test = TestPatternSuite;
   for (int itestPattern = 0; itestPattern < ntestPatterns; ++itestPattern, ++test)
   {
      printf ("\nIntegrity check channel-by-channel: using pattern = %s\n", 
              test->name);
      (*test->create) (patterns, npatterns);

      for (int itrial = 0; itrial < ntrials; ++itrial)
      {
         fill (frames    + itrial * nframes_per_trial, 
               nframes_per_trial, 
               patterns  + itrial * npatterns_per_trial);
      }

      test_integrityPtrArray  (dstPtrs,  frames,   nframes_per_trial, ntrials, 
                               patterns, npatterns_per_trial, npatterns);
   }
   // ----------------------------------------------------------------------



   // ----------------------------------------------------------------------
   // Integrity check: vector memory
   // ------------------------------
   test = TestPatternSuite;
   for (int itestPattern = 0; itestPattern < ntestPatterns; ++itestPattern, ++test)
   {
      printf ("\nIntegrity check vector: using pattern = %s\n", 
              test->name);
      (*test->create) (patterns, npatterns);

      for (int itrial = 0; itrial < ntrials; ++itrial)
      {
         fill (frames    + itrial * nframes_per_trial, 
               nframes_per_trial, 
               patterns  + itrial * npatterns_per_trial);
      }

      test_integrityVector  (&dstVecs, frames,   nframes_per_trial, ntrials, 
                             patterns, npatterns_per_trial, npatterns);
   }
   // ----------------------------------------------------------------------



   // ----------------------------------------------------------------------
   // Performanace checks
   // -------------------
   for (int idx = 0; idx < 2; ++idx)
   {
   test = TestPatternSuite;
   for (int itestPattern = 0; itestPattern < ntestPatterns; ++itestPattern, ++test)
   {

      printf ("\nPerformance check contigiuous: using pattern = %s\n", 
              test->name);
      (*test->create) (patterns, npatterns);

      for (int itrial = 0; itrial < ntrials; ++itrial)
      {
         fill (frames    + itrial * nframes_per_trial, 
               nframes_per_trial, 
               patterns  + itrial * npatterns_per_trial);
      }

      test_performance (dstBuf,   frames,   nframes_per_trial, ntrials, 
                        patterns, npatterns_per_trial, npatterns);
   }
   // ----------------------------------------------------------------------  


   test = TestPatternSuite;
   for (int itestPattern = 0; itestPattern < ntestPatterns; ++itestPattern, ++test)
   {
      printf ("\nPerformance check pointer array: using pattern = %s\n", 
              test->name);
      (*test->create) (patterns, npatterns);

      for (int itrial = 0; itrial < ntrials; ++itrial)
      {
         fill (frames    + itrial * nframes_per_trial, 
               nframes_per_trial, 
               patterns  + itrial * npatterns_per_trial);
      }

      test_performancePtrArray (dstPtrs,  frames,   nframes_per_trial, ntrials, 
                                patterns, npatterns_per_trial, npatterns);
   }


   test = TestPatternSuite;
   for (int itestPattern = 0; itestPattern < ntestPatterns; ++itestPattern, ++test)
   {
      printf ("\nPerformance vector array: using pattern = %s\n", 
              test->name);
      (*test->create) (patterns, npatterns);

      for (int itrial = 0; itrial < ntrials; ++itrial)
      {
         fill (frames    + itrial * nframes_per_trial, 
               nframes_per_trial, 
               patterns  + itrial * npatterns_per_trial);
      }

      test_performanceVector (&dstVecs, frames,   nframes_per_trial, ntrials, 
                              patterns, npatterns_per_trial, npatterns);
   }
   }


   return 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static void print_integrity (char const *title, int nerrs, int npatterns)
{
   printf ("Checking %30s %0x/%0x patterns --- %s\n", 
           title, nerrs, npatterns, nerrs ? " *** FAILEED ***" : "PASSED");
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \typedef  TransposeMethod
  \brief    The call signature for transposing into contigious memory

  \param[out]     dst The destination memory for the transposed adcs
  \param[ in]  stride The number of adcs in each channel. This is
                      generally \a nframes, but can be larger to ensure
                      alignment.  It can also be used as space to append
                      more adcs to.
  \param[ in]  frames The source WibFrames
  \param[ in] nframes The number of frames to transpose
                                                                          */
/* ---------------------------------------------------------------------- */
typedef void (*TransposeMethod) (uint16_t          *dst, 
                                 int             stride, 
                                 WibFrame const *frames, 
                                 int            nframes);
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \struct TransposeTest
   \brief  Defines a transpose to contigious memory test
                                                                          */
/* ---------------------------------------------------------------------- */
struct TransposeTest
{
   char const          *name;  /*!< A descriptive name for the test       */
   TransposeMethod transpose;  /*!< The transpose test method             */
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \var    TransposeTests
  \brief  The suite of transpose to contigious memory tests
                                                                          */
/* ---------------------------------------------------------------------- */
TransposeTest TransposeTests[3] = 
{
   { "transpose128x8N",  WibFrame::transposeAdcs128x8N  },
   { "transpose128x16N", WibFrame::transposeAdcs128x16N },
   { "transpose128x32N", WibFrame::transposeAdcs128x32N },
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static void test_integrity (uint16_t         *dstBuf, 
                            WibFrame const   *frames,
                            int    nframes_per_trial,
                            int              ntrials,
                            uint16_t const *patterns,
                            int  npatterns_per_trial,
                            int            npatterns)
{
   // ---------------
   // 128x1 Expansion
   // ---------------
   memset (dstBuf, 0xff, sizeof (*dstBuf) * npatterns); 

   WibFrame::expandAdcs128xN   (dstBuf,   frames,   nframes_per_trial);
   int nerrs = check_expansion (dstBuf, patterns, npatterns_per_trial);
   print_integrity ("expandAdcs128x1",     nerrs, npatterns_per_trial);



   for (unsigned int idx = 0; 
        idx < sizeof (TransposeTests) / sizeof (TransposeTests[0]);
        ++idx)
   {
      TransposeTest const &test = TransposeTests[idx];
      memset (dstBuf, 0xff, sizeof (*dstBuf) * npatterns); 
      test.transpose (dstBuf,     nframes_per_trial, frames,   nframes_per_trial);
      int nerrs = check_transpose (dstBuf, patterns,    128,   nframes_per_trial);
      print_integrity             (test.name,         nerrs, npatterns_per_trial);
   }


   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static void test_performance (uint16_t         *dstBuf, 
                              WibFrame const*framesSrc,
                              int    nframes_per_trial,
                              int              ntrials,
                              uint16_t const *patterns,
                              int  npatterns_per_trial,
                              int            npatterns)
{
   struct timeval  dif[NTRIALS];


   // ---------------
   // 128x1 Expansion
   // ---------------
   {
   memset (dstBuf, 0xff, sizeof (*dstBuf) * NPATTERNS); 

   uint16_t          *dst = dstBuf;
   WibFrame const *frames = framesSrc;

   for (int itrial = 0; itrial < ntrials; ++itrial) 
   {
      struct timeval beg, end;
      gettimeofday (&beg, NULL);

      WibFrame::expandAdcs128xN (dst, frames, nframes_per_trial);
      dst    += 128 * nframes_per_trial;
      frames +=       nframes_per_trial;

      gettimeofday (&end, NULL);
      timersub     (&end, &beg, dif + itrial);
   }

   printf ("Checking expandAdcs128x1 %8x patterns\n", npatterns);
   int nerrs = check_expansion (dstBuf, patterns, npatterns);
   print ("expandAdcs128x1", nerrs, dif, ntrials, npatterns);
   }


   // ---------------
   // Transpose tests
   // ---------------
   for (unsigned int idx = 0; 
        idx < sizeof (TransposeTests) / sizeof (TransposeTests[0]);
        ++idx)
   {
      memset (dstBuf, 0xff, sizeof (*dstBuf) * NPATTERNS); 
      TransposeTest const *test = TransposeTests + idx;

      for (int itrial = 0; itrial < ntrials; ++itrial) 
      {
         uint16_t             *dst = dstBuf + npatterns_per_trial * itrial;
         WibFrame const    *frames = framesSrc;


         struct timeval beg, end;
         gettimeofday (&beg, NULL);

         test->transpose (dst, nframes_per_trial, frames, nframes_per_trial);
         frames += nframes_per_trial;

         gettimeofday (&end, NULL);
         timersub     (&end, &beg, dif + itrial);
      }

      printf ("Checking %s %8x patterns\n", test->name, npatterns);
      int nerrs = check_transpose (dstBuf,
                                   patterns,
                                   128,
                                   nframes_per_trial);
      print (test->name, nerrs, dif, ntrials, npatterns);
   }

   return;
}
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

   \brief Cheap print routine for debugging

   \param   d[in] Pointer to the adcs to print
   \param chn[in] The Adc channel number
                                                                          */
/* ---------------------------------------------------------------------- */
static void print_adcs (uint16_t const *d, int chn)  __attribute ((unused));
static void print_adcs (uint16_t const *d, int chn) 
{
   for (int idx = 0; idx < 64; idx += 8)
   {
      printf ("d[%2x.%2x] @%p"
              " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 ""
              " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 " %4.4" PRIx16 "\n",
              chn, idx, (void *)&d[idx],
              d[0], d[1], d[2], d[3],
              d[4], d[5], d[6], d[7]);
   }

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \typedef  TransposePtrArrayMethod
  \brief    The call signature for transposing into channel-by-channel
            memory.

  \param[out]     dst The destination array of pointers to receive 
                      each channel of transposed adcs.
  \param[ in]  offset The offset to store the first ADC at. This is 
                      generally 0, but could be used to append more
                      ADCs.
  \param[ in]  frames The source WibFrames
  \param[ in] nframes The number of frames to transpose
                                                                          */
/* ---------------------------------------------------------------------- */
typedef void (*TransposePtrArrayMethod) (uint16_t *const   *dst, 
                                         int             offset,
                                         WibFrame const *frames, 
                                         int            nframes);
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \struct TransposePtrArrayTest
   \brief  Defines a transpose to channel-by-channel memory test
                                                                          */
/* ---------------------------------------------------------------------- */
struct TransposePtrArrayTest
{
   char const                  *name;  /*!< A descriptive name for test   */
   TransposePtrArrayMethod transpose;  /*!< The transpose test method     */
};
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Resets the channel-by-channel destination memory so that each
         test starts with the same impossible pattern.

  \param[out]   dstPtrs Pointers to the channel-by-channel memory
  \param[ in] nchannels The number of channel-by-channel memories to reset,
  \param[ in]   nframes The number of entries in each channel memory.
                                                                          */
/* ---------------------------------------------------------------------- */
static void reset (uint16_t *const *dstPtrs, 
                   int            nchannels, 
                   int              nframes)
{
   //printf ("Clearing nchannels = %d for nframes = %d\n", nchannels, nframes);
   for (int ichan = 0; ichan < nchannels; ++ichan)
   {
      memset (dstPtrs[ichan], 
              0xff, 
              sizeof (*dstPtrs[ichan]) *nframes);
   }

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \var    TransposePtrArrayTests
  \brief  The suite of transpose to channel-by-channel memory tests
                                                                          */
/* ---------------------------------------------------------------------- */
TransposePtrArrayTest TransposePtrArrayTests[3] = 
{
   { "transpose128x32N(pa)", WibFrame::transposeAdcs128x32N },
   { "transpose128x16N(pa)", WibFrame::transposeAdcs128x16N },
   { "transpose128x8N(pa)",  WibFrame::transposeAdcs128x8N  }
};
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
static void test_integrityPtrArray (uint16_t *const *dstPtrs, 
                                    WibFrame const   *frames,
                                    int    nframes_per_trial,
                                    int              ntrials,
                                    uint16_t const *patterns,
                                    int  npatterns_per_trial,
                                    int            npatterns)
{
   int                     nchannels = npatterns / nframes_per_trial;
   TransposePtrArrayTest const *test = TransposePtrArrayTests;

   for (unsigned int idx = 0; 
        idx < sizeof (TransposeTests) / sizeof (TransposeTests[0]);
        ++idx, ++test)
   {
      reset           (dstPtrs, nchannels, nframes_per_trial);
      test->transpose (dstPtrs, 0, frames, nframes_per_trial);

      int nerrs = check_transpose (dstPtrs,
                                   patterns,
                                   128,
                                   nframes_per_trial);
      print_integrity (test->name, nerrs, npatterns_per_trial);
   }


   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
static void test_performancePtrArray (uint16_t *const *dstPtrs, 
                                      WibFrame const*framesSrc,
                                      int    nframes_per_trial,
                                      int              ntrials,
                                      uint16_t const *patterns,
                                      int  npatterns_per_trial,
                                      int            npatterns)
{
   int    nchannels = npatterns / nframes_per_trial / ntrials;
   TransposePtrArrayTest const *test = TransposePtrArrayTests;

   struct timeval  dif[NTRIALS];

   // ---------------
   // Transpose tests
   // ---------------
   for (unsigned int itest = 0; 
        itest < sizeof (TransposeTests) / sizeof (TransposeTests[0]);
        ++itest, ++test)
   {

      // Reset the destination memory
      reset (dstPtrs, nchannels * ntrials, nframes_per_trial);


      uint16_t *const    *dst = dstPtrs;
      WibFrame  const *frames = framesSrc;
      for (int itrial = 0; itrial < ntrials; ++itrial) 
      {
         struct timeval beg, end;
         gettimeofday (&beg, NULL);

         test->transpose (dst, 0, frames, nframes_per_trial);
         frames += nframes_per_trial;
         dst    += nchannels;

         gettimeofday (&end, NULL);
         timersub     (&end, &beg, dif + itrial);
      }

      printf ("Checking %s %8x patterns\n", test->name, npatterns);

      int nerrs = check_transpose (dstPtrs,
                                   patterns,
                                   128,
                                   nframes_per_trial);
      print (test->name, nerrs, dif, ntrials, npatterns);
   }



   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Resets the channel-by-channel destination memory so that each
         test starts with the same impossible pattern.

  \param[out]   dstPtrs Pointers to the channel-by-channel memory
  \param[ in] nchannels The number of channel-by-channel memories to reset,
  \param[ in]   nframes The number of entries in each channel memory.
                                                                          */
/* ---------------------------------------------------------------------- */
static void reset (std::vector<TpcAdcVector> *dstVecs)
{
   int nchannels = dstVecs->capacity ();
   //printf ("Clearing nchannels = %d\n", nchannels);
   for (int ichan = 0; ichan < nchannels; ++ichan)
   {
      TpcAdcVector &vec = (*dstVecs)[ichan];
      int       nframes = vec.capacity ();
      void       *data  = vec.data ();
      //printf ("Clearing dstVecs[%d] @%p for nbytes = %d\n",
      //        ichan, data, (int)sizeof (uint16_t) * nframes);
      memset (data,
              0xff, 
              sizeof (uint16_t) * nframes);
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \typedef  TransposeVectorMethod
  \brief    The call signature for transposing into channel-by-channel
            memory.

  \param[out]     dst The destination array of pointers to receive 
                      each channel of transposed adcs.
  \param[ in]  offset The offset to store the first ADC at. This is 
                      generally 0, but could be used to append more
                      ADCs.
  \param[ in]  frames The source WibFrames
  \param[ in] nframes The number of frames to transpose
                                                                          */
/* ---------------------------------------------------------------------- */
typedef void (*TransposeVectorMethod) (uint16_t *const   *dst, 
                                       int             offset,
                                       WibFrame const *frames, 
                                       int            nframes);
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \struct TransposeVectorTest
   \brief  Defines a transpose to channel-by-channel memory test
                                                                          */
/* ---------------------------------------------------------------------- */
struct TransposeVectorTest
{
   char const                *name;  /*!< A descriptive name for test     */
   TransposeVectorMethod transpose;  /*!< The transpose test method       */
};
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \var    TransposePtrArrayTests
  \brief  The suite of transpose to channel-by-channel memory tests
                                                                          */
/* ---------------------------------------------------------------------- */
TransposeVectorTest TransposeVectorTests[3] = 
{
   { "transpose128x32N(vec)", WibFrame::transposeAdcs128x32N },
   { "transpose128x16N(vec)", WibFrame::transposeAdcs128x16N },
   { "transpose128x8N(vec)",  WibFrame::transposeAdcs128x8N  }
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static void test_integrityVector    (std::vector<TpcAdcVector>
                                                                    *dstVecs,
                                     WibFrame               const   *frames,
                                     int                   nframes_per_trial,
                                     int                             ntrials,
                                     uint16_t                const *patterns,
                                     int                 npatterns_per_trial,
                                     int                           npatterns)
{

   int                   nchannels = npatterns / nframes_per_trial / ntrials;
   TransposeVectorTest const *test = TransposeVectorTests;


   //printf ("Nchannels.nframes = %d.%d\n", nchannels, nframes_per_trial);
   uint16_t **dstPtrs = (uint16_t **)::malloc (nchannels * sizeof (*dstPtrs));


   for (int ichan = 0; ichan < nchannels; ++ichan)
   {
      TpcAdcVector &dstVec = (*dstVecs)[ichan];
      dstVec.reserve (nframes_per_trial);
      dstPtrs [ichan] = dstVec.data ();
   }


   for (unsigned int idx = 0; 
        idx < sizeof (TransposeVectorTests) / sizeof (TransposeVectorTests[0]);
        ++idx, ++test)
   {
      reset           (dstPtrs, nchannels, nframes_per_trial);
      test->transpose (dstPtrs, 0, frames, nframes_per_trial);

      int nerrs = check_transpose (dstPtrs,
                                   patterns,
                                   128,
                                   nframes_per_trial);
      print_integrity (test->name, nerrs, npatterns_per_trial);
   }

   ::free (dstPtrs);

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static void test_performanceVector  (std::vector<TpcAdcVector> 
                                                                   *dstVecs,
                                     WibFrame              const *framesSrc,
                                     int                  nframes_per_trial,
                                     int                            ntrials,
                                     uint16_t              const  *patterns,
                                     int                npatterns_per_trial,
                                     int                          npatterns) 
{
   int           nchannels = npatterns / nframes_per_trial / ntrials;
   TransposeVectorTest const *test = TransposeVectorTests;

   struct timeval  dif[NTRIALS];

   uint16_t **dstPtrs = (uint16_t **)::malloc (nchannels * ntrials * sizeof (*dstPtrs));

   //printf ("Nchannels.nframes = %d.%d\n", nchannels, nframes_per_trial);

   // ---------------
   // Transpose tests
   // ---------------
   for (unsigned int itest = 0; 
        itest < sizeof (TransposeVectorTests) / sizeof (TransposeVectorTests[0]);
        ++itest, ++test)
   {

      // Reset the destination memory
      reset (dstVecs);

      int jchan = 0;

      uint16_t                  **dst = dstPtrs;
      std::vector<TpcAdcVector> *vecs = dstVecs;
      WibFrame  const         *frames = framesSrc;
      for (int itrial = 0; itrial < ntrials; ++itrial) 
      {
         struct timeval beg, end;
         gettimeofday (&beg, NULL);

         for (int ichan = 0; ichan < nchannels; ++ichan)
         {
            TpcAdcVector &dstVec = (*vecs)[jchan++];
            dstVec.reserve (nframes_per_trial);
            dst[ichan] = dstVec.data ();
         }


         test->transpose (dst, 0, frames, nframes_per_trial);
         frames += nframes_per_trial;
         dst    += nchannels;

         gettimeofday (&end, NULL);
         timersub     (&end, &beg, dif + itrial);
      }

      printf ("Checking %s %8x patterns\n", test->name, npatterns);

      int nerrs = check_transpose (dstPtrs,
                                   patterns,
                                   128,
                                   nframes_per_trial);
      print (test->name, nerrs, dif, ntrials, npatterns);
   }

   ::free (dstPtrs);

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Create a pattern with the time in the upper 8 bits and the
         the channel (at least 0-f) in the lower 4 bits

  \param[in]  patterns  The arrary of 12 bit patterns to fill
  \param[in] npatterns  The number of patterns
                                                                          */
/* ---------------------------------------------------------------------- */
static void create_tc (uint16_t *patterns, int npatterns)
{
   for (int itime = 0; itime < npatterns/128; ++itime)
   {
      uint16_t p = (itime << 4) & 0x0ff0;
      for (int ichan = 0; ichan < 128; ++ichan)
      {
         *patterns++ = p | (ichan & 0xf);
      }
   }
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Create random patterns

  \param[in]  patterns  The arrary of 16 bit patterns to fill
  \param[in] npatterns  The number of patterns
                                                                          */
/* ---------------------------------------------------------------------- */
static void create_random (uint16_t *patterns, int npatterns)
{
   srand (0xdeadbeef);
   for (int idx = 0; idx < npatterns; ++idx)
   {
      uint16_t      p = rand () & 0xfff;
      *patterns++ = p;
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
static void fill (WibFrame *frames, int nframes, uint16_t const *patterns)
{
   for (int iframe = 0; iframe < nframes; iframe++)
   {
      WibColdData (&cd)[2] = frames[iframe].getColdData ();
      uint64_t      *adcs0 = cd[0].locateAdcs12b ();
      uint64_t      *adcs1 = cd[1].locateAdcs12b ();

      fill64 (adcs0, patterns);
      fill64 (adcs1, patterns + 64);

      patterns += 128;
   }
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static void fill16 (uint64_t *src, uint16_t const *patterns)
{

   uint16_t const *p = patterns;
   uint64_t       *s = src;

   s[0] = packA (p[ 0], p[ 1], p[ 2], p[ 3], p[ 4], p[ 5]);
   s[1] = packB (p[ 5], p[ 6], p[ 7], p[ 8], p[ 9], p[10]);
   s[2] = packC (p[10], p[11], p[12], p[13], p[14], p[15]);
   

   return;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
static void fill64 (uint64_t *src, uint16_t const *patterns)
{

   uint16_t const *p = patterns;
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
static void print (char            const *title,
                   int                    nerrs, 
                   struct timeval const    *dif,
                   int                  ntrials,
                   int                npatterns)
{
   printf ("%s: error count/total = 0x%8x/%8x\n", title, nerrs, npatterns);
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




/* ---------------------------------------------------------------------- */
static int check_expansion (uint16_t const  *results, 
                            uint16_t const *patterns,
                            int            npatterns)
{
   int errcnt = 0;

   for (int idx = 0; idx < npatterns; ++idx)
   {
      uint16_t  result =  results[idx];
      uint16_t pattern = patterns[idx];
      if (result != pattern)
      {
         if (errcnt == 0)
         {
            puts ("Error    At  Results  !=  Expected\n"
                  "----- ----   -------  --  --------");
         }

         errcnt += 1;
         if (errcnt < 10)
         {
            printf ("%6d %4x     %4.4x !=     %4.4x\n",
                    errcnt, idx, result, pattern);
         }
      }
   }

   return errcnt;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- */
static int check_transpose  (uint16_t const  *results,
                             uint16_t const  *patterns,
                             int             nchannels,
                             int               nframes)
{
   /*
   printf ("Nchannels = %x Nframes = %x\n", nchannels, nframes);
   int stride = nframes;
   uint16_t const *dst = results;
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


      
   int errcnt = 0;
   for (int ichan = 0; ichan < nchannels; ++ichan)
   {
      for (int iframe = 0; iframe < nframes;  iframe++)
      {
         uint16_t  result = *results++;
         uint16_t pattern = *(patterns + iframe * nchannels + ichan);

         if (result != pattern)
         {
            if (errcnt == 0)
            {
               puts ("Error Chn.Time   Results != Expected\n"
                     "----- ---.----   ------- -- --------");
            }

            errcnt += 1;
            if (errcnt < 10)
            {
               printf ("%6d %3x.%4x     %4.4x !=     %4.4x\n",
                       errcnt, ichan, iframe, result, pattern);
            }
         }
      }
   }

   return errcnt;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
static int check_transpose  (uint16_t *const  *results,
                             uint16_t  const  *patterns,
                             int              nchannels,
                             int                nframes)
{
   /*
   printf ("Channel-by-Channel checker nchannels.nframes = %d.%d\n", 
            nchannels, nframes);

   int stride = nframes;
   uint16_t const *dst = results;
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


      
   int errcnt = 0;
   for (int ichan = 0; ichan < nchannels; ++ichan)
   {
      uint16_t const *r = results[ichan];
      for (int iframe = 0; iframe < nframes;  iframe++)
      {
         uint16_t  result = *r++;
         uint16_t pattern = *(patterns + iframe * nchannels + ichan);

         if (result != pattern)
         {
            if (errcnt == 0)
            {
               puts ("Error Chn.Time   Results != Expected\n"
                     "----- ---.----   ------- -- --------");
            }

            errcnt += 1;
            if (errcnt < 10)
            {
               printf ("%6d %3x.%4x     %4.4x !=     %4.4x\n",
                       errcnt, ichan, iframe, result, pattern);
            }
         }
      }
   }

   return errcnt;
}
/* ---------------------------------------------------------------------- */






/* ====================================================================== *\
 |                                                                        |
 |  This was an experimental test to expand the ADCs using the bextr      |
 |  instruction to access the ADCs.                                       | 
 |                                                                        |
 |  It is no faster than using shifts and ands, although it is a more     |
 |  compact (less bytes of instruction code). It would have been use to   |
 |  do the generic expansion, but the processors that support this        | 
 |  instruction also support AVX2. Since AVX2 is faster, it is used on    |
 |  those processors.                                                     |
 |                                                                        |
\* ---------------------------------------------------------------------- */
#if 0


/* ---------------------------------------------------------------------- */
static inline uint64_t _bextr_u64(uint64_t src, uint8_t beg, uint8_t len)
{
   uint64_t result;
   uint64_t ctl = (len << 8) | (beg << 0);
   asm ("bextr %2,%1,%0" : "=r"(result) : "r"(src),"r"(ctl));

   return result;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
static void expandAdcs16x1_cpuA_kernel (uint16_t *dst, uint64_t const *src)
{
   uint64_t w0;
   uint64_t w1;
   uint64_t w2;


   w0 = *src++;  
   dst[0] = _bextr_u64(w0, 0,  12);
   dst[1] = _bextr_u64(w0, 12, 12);
   dst[2] = _bextr_u64(w0, 24, 12);
   dst[3] = _bextr_u64(w0, 36, 12);
   dst[4] = _bextr_u64(w0, 48, 12);


   w1 = *src++; 
   dst[5] = (_bextr_u64 (w1, 0, 8) << 4) | _bextr_u64 (w0, 60, 4);
   dst[6] = _bextr_u64(w1,  8, 12);
   dst[7] = _bextr_u64(w1, 20, 12);
   dst[8] = _bextr_u64(w1, 32, 12);
   dst[9] = _bextr_u64(w1, 44, 12);


   w2 = *src++; 
   dst[10] = (_bextr_u64 (w2, 0, 4) << 8) | _bextr_u64 (w1, 56, 8);
   dst[11] = _bextr_u64(w2,  4, 12);
   dst[12] = _bextr_u64(w2, 16, 12);
   dst[13] = _bextr_u64(w2, 28, 12);
   dst[14] = _bextr_u64(w2, 40, 12);
   dst[15] = _bextr_u64(w2, 52, 12);

   return;
}



static inline void expandAdcs64x1_cpuA_kernel (uint16_t *dst, uint64_t const *src) 
{
   expandAdcs16x1_cpuA_kernel (dst + 0*16, &src[0]);
   expandAdcs16x1_cpuA_kernel (dst + 1*16, &src[3]);
   expandAdcs16x1_cpuA_kernel (dst + 2*16, &src[6]);
   expandAdcs16x1_cpuA_kernel (dst + 3*16, &src[9]);

   return;
}
/* ---------------------------------------------------------------------- */
#endif
/* ====================================================================== */
