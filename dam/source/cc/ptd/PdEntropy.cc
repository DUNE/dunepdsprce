// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     PdEntropy.cc
 *  @brief    Calcuates the entropy for data packets
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
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.09.15 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "Reader.hh"
#include "dam/HeaderFragmentUnpack.hh"
#include "dam/DataFragmentUnpack.hh"
#include "dam/TpcFragmentUnpack.hh"
#include "dam/TpcStreamUnpack.hh"
#include "dam/access/WibFrame.hh"


#include <cstdlib>
#include <cstring>
#include <cinttypes>
#include <cstdio>


static void print_summary (TpcStreamUnpack const *tpcStream);

/* ---------------------------------------------------------------------- *//*!

  \class  Prms
  \brief  The configuration parameters
                                                                          */
/* ---------------------------------------------------------------------- */
class Prms
{
public:
   Prms (int argc, char *const argv[]);

public:
   int            m_npackets; /*!< Number of 1024 packets to extract      */
   int            m_ifilecnt; /*!< Input  file name count                 */ 
   char *const *m_ifilenames; /*!< Input  file name                       */
   Reader::FileType
                 m_ifiletype; /*!< The input file type                    */
   int           m_printhist; /*!< Print histograms                       */
   int              m_ovrflw; /*!< Overflow trigger                       */

};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Constructor to extract the readers command line parameters

  \param[in] argc The count  of the command line parameters
  \param[in] argv The vector of the command line parameters
                                                                          */
/* ---------------------------------------------------------------------- */
Prms::Prms (int argc, char *const argv[]) :
   m_npackets   (0x7fffffff),
   m_ifilecnt   (0),
   m_ifilenames (NULL),
   m_ifiletype  (Reader::FileType::Binary),
   m_printhist  (0),
   m_ovrflw     (0x7fffffff)

{
   char c;
   while ( (c = getopt (argc, argv, "O:n:o:bgp:")) != -1)
   {
      switch (c)
      {
      case 'b': { m_ifiletype = Reader::FileType::Binary;    break; }
      case 'g': { m_ifiletype = Reader::FileType::TextGdb64; break; }
      case 'n': { m_npackets  = strtoul (optarg, NULL, 0);   break; }
      case 'O': { m_ovrflw    = strtoul (optarg, NULL, 0);   break; }
      case 'p': { m_printhist = strtoul (optarg, NULL, 0);   break; }
      }
   }  

   if (optind < argc)
   {
      m_ifilenames = &argv[optind];
      m_ifilecnt   = argc - optind;
   }
   else
   {
      fprintf (stderr, "Error: No input file provided\n");
      exit (-1);
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \class Entropy
  \brief Calculates the entropy of the data packets
                                                                          */
/* ---------------------------------------------------------------------- */
class Entropy
{
public:
   static size_t const MaxBuf = 10 * 1024 * 1024;

public:
   Entropy (int npackets, int printhist, int ovrflw);

public:
   ~Entropy ();

public:
   int  open    (char const *ifilename, Reader::FileType ifiletype);
   int  read    ();
   int  close   ();
   bool process ();
   bool entropy (TpcStreamUnpack const *tpcStream);

   static void histogram (uint16_t      *hist, int nhist, 
                          int16_t const *adcs, int nticks);

public: 
   Reader *m_reader;

   int   m_npackets; /*!< Number of packets to process                    */
   int      m_ntogo; /*!< Number of packets left to process               */
   int  m_printhist; /*!< Print histograms                                */
   int     m_ovrflw; /*!< Print channel's ADCs if # overflows>=this value */
   uint64_t  *m_buf; /*!< Pointer to the input data buffer                */
};
/* ---------------------------------------------------------------------- */


Entropy::Entropy (int npackets, int printhist, int ovrflw) 
{
   m_npackets  = npackets;
   m_ntogo     = m_npackets;
   m_ovrflw    = ovrflw;
   m_printhist = printhist;
   m_buf       = reinterpret_cast<decltype (m_buf)>(malloc (MaxBuf));
   return;
}
/* ---------------------------------------------------------------------- */   




/* ---------------------------------------------------------------------- *//*!

  \brief Entropy destructor
                                                                          */
/* ---------------------------------------------------------------------- */
Entropy::~Entropy ()
{
   delete m_reader;
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
int Entropy::open (char const *ifilename, Reader::FileType ifiletype)
{
   m_reader = &ReaderCreate (ifilename, ifiletype);
   if (m_reader == 0) return -1;

   int err = m_reader->open ();
   return err;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Close the input file reader
                                                                          */
/* ---------------------------------------------------------------------- */
int Entropy::close ()
{
   delete m_reader;
   m_reader = 0;
   return 0;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Read the next event
  \retval == 0  Success
  \retval == 1  EOF
                                                                          */
/* ---------------------------------------------------------------------- */
int Entropy::read ()
{

   HeaderFragmentUnpack *header = HeaderFragmentUnpack::assign (m_buf);
   ssize_t               nbytes = m_reader->read (header);

   if (nbytes <= 0)
   {
      if (nbytes == 0) 
      {
         // If hit eof, return 'done'
         m_reader->close ();
         return 1;
      }

      else
      {
         // Anything else is an error
         exit (-1);
      }
   }

   // ----------------------------------------------------
   // Get the number of 64-bit words in this data fragment
   // and read the body of the data fragment.
   // ----------------------------------------------------
   uint64_t  n64 = header->getN64 ();
   ssize_t nread = m_reader->read (m_buf, n64, nbytes);
   if (nread <= 0)
   {
      if (nread)
      {
         fprintf (stderr, "Error: Incomplete or corrupted record\n");
         exit (-1);
      }
   }

   return 0;
}
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief Process an RCE data fragment

  \param[in] buf The data fragment to process
                                                                          */
/* ---------------------------------------------------------------------- */
bool Entropy::process ()
{
   uint64_t const *buf = m_buf;

   // -----------------------------------------------
   // Interpret this as a generic RCE Fragment Header
   // -----------------------------------------------
   HeaderFragmentUnpack const header (buf);


   // ----------------------------
   // Is this an RCE Data Fragment
   // ----------------------------
   if (header.isData ())
   {
      // --------------------------------------------
      // Turn the buffer into a generic data fragment
      // --------------------------------------------
      DataFragmentUnpack df (buf);
      //df.print ();


      // ----------------------------------------------
      // Is this record an error-free TPC data fragment
      // ---------------------------------------------
      if (df.isTpcNormal ())
      {
         // ------------------------------------------------
         // Having verified that this is a TPC data fragment
         // can now convert it an analyze it as such
         // ------------------------------------------------
         TpcFragmentUnpack tpcFragment (df);

         // --------------------------------------------
         // Get the number and loop over the TPC streams
         // --------------------------------------------
         int nstreams = tpcFragment.getNStreams ();

         for (int istream = 0; istream < nstreams; ++istream)
         {
            TpcStreamUnpack const *tpcStream = tpcFragment.getStream (istream);
            bool                        done = entropy (tpcStream);
            if (done) { return true; }
         }
      }
      else
      {
         char const *tpcType = df.isTpcEmpty  () ? "TpcEmpty"
                             : "TpcUnknown";
         
         printf ("Have TpcStream data type: %s\n", tpcType);
         
      }
   }

   return false;
}
/* ---------------------------------------------------------------------- */


static void  print (uint16_t const *hist, int nbins, int ichan, int ipacket);

/* ---------------------------------------------------------------------- *//*!

  \brief Calculates the entropy of the specified stream
         TpcStreamUnpack interface.

  \param[in]  tpcStream  The target TPC stream.
                                                                          */
/* ---------------------------------------------------------------------- */
bool Entropy::entropy (TpcStreamUnpack const *tpcStream)
{
   using namespace pdd;
   using namespace pdd::access;

   print_summary (tpcStream);

   int nchannels = tpcStream->getNChannels       ();
   int nticks    = tpcStream->getNTicksUntrimmed ();
   int adcCnt    = nchannels * nticks;

   int     adcNBytes = adcCnt    * sizeof (int16_t);
   int16_t *adcsBuf = reinterpret_cast <decltype (adcsBuf)>(malloc (adcNBytes));


   bool okay = tpcStream->getMultiChannelDataUntrimmed (adcsBuf, nticks);
   if (okay)
   {
      int16_t const *adcs = adcsBuf;
      uint16_t   hist[32];
      int           nbins = sizeof (hist) / sizeof (hist[0]); 

      for (int ichan = 0; ichan < nchannels; ichan++)
      {
         for (int ipacket = 0; ipacket < nticks/1024; ipacket++)
         {
            uint16_t hist[32];
            {
               histogram (hist, nbins, adcs + ipacket * 1024, 1024);
               if (hist[0] >= m_ovrflw) 
               {
                  printf ("\nLarge overflow\n");
                  print (hist, nbins, ichan, ipacket);

                  for (int idx = 0; idx < 1024; idx++)
                  {
                     if ( (idx & 0xf) == 0) putchar ('\n');
                     printf (" 0x%3.3x", adcs[ipacket * 1024 + idx]);
                  }
                  putchar ('\n');

               }
               else if (ichan < m_printhist)
               {
                  print (hist, nbins, ichan, ipacket);
               }
            }
         }

         adcs += nticks;
      }


   }

   free (adcsBuf);

   return false;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
void Entropy::histogram (uint16_t      *hist, int nhist, 
                         int16_t const *adcs, int nticks)
{
   memset (hist, 0, sizeof (*hist) * nhist);
   uint16_t prv = adcs[0];

   //// printf ("Diff[  0]:    ");
   for (int itick = 1; itick < nticks; itick++)
   {
      int  cur = adcs[itick];
      int diff = prv - cur;

/*
      if ( (itick & 0x1f) == 0) printf ("Diff[%3x]:", itick);
      printf (" %3d", diff);
      if ( (itick & 0x1f) == 0x1f) putchar ('\n');
*/                                    

      int sym  = (diff << 1) | 1;
      if (diff < 0)
      {
         sym = -sym + 1;
      }

      if (sym >= nhist)
      {
         sym = 0;
      }

      hist[sym]++;
      prv = cur;
   }

   return;
}

/* ---------------------------------------------------------------------- */
static void  print (uint16_t const *hist, int nbins, int ichan, int ipacket)
{
   for (int ibin = 0; ibin < nbins; ibin++)
   {
      if ( (ibin & 0x1f) ==    0) printf ("Hist[%3d.%2d.%2d] =", ichan, ipacket, ibin);
      printf (" %3x", (int)hist[ibin]);
      if ( (ibin & 0x1f) == 0x1f) putchar ('\n');
   }

   if (nbins & 0x1f) putchar ('\n');

   return;
}
/* ---------------------------------------------------------------------- */



static int calculate (Entropy &entropy);



/* ---------------------------------------------------------------------- */
int main (int argc, char *const argv[])
{
   // -----------------------------------
   // Extract the command line parameters
   // -----------------------------------
   Prms     prms (argc, argv);
   Entropy  entropy (prms.m_npackets, prms.m_printhist, prms.m_ovrflw);

   for (int idx = 0; idx < prms.m_ifilecnt; idx++)
   {
      printf ("Processing: %s\n", prms.m_ifilenames[idx]);
      int status = entropy.open (prms.m_ifilenames[idx],
                                 prms.m_ifiletype);
      if (status) break;

      bool done = calculate (entropy);
      entropy.close ();

      if (done) break;
   }

   
   ///entropy.~Entropy ();

   return 0;
}
/* ---------------------------------------------------------------------- */


static int calculate (Entropy &entropy)
{
   while (1)
   {
      {
         int done = entropy.read  ();
         if (done) break;
      }

      {
         bool done = entropy.process ();
         if (done) break;
      }
   }

   return 0;
}
/* ---------------------------------------------------------------------- */



#if 1
/* ---------------------------------------------------------------------- *//*!

  \brief Prints a summary of the specified \a tpcStream

  \param[in] tpcStream The TPC stream
  \param[in]  nwritten Number of frames written so far
  \param[in]     total The total to write
                                                                          */
/* ---------------------------------------------------------------------- */
static void print_summary (TpcStreamUnpack const *tpcStream)
{
   TpcStreamUnpack::Identifier id = tpcStream->getIdentifier ();
   
   printf ("Processing: WibId:%2d.%1d.%1d\n",
           id.getCrate (),
           id.getSlot  (),
           id.getFiber ());


   return;
}
/* ---------------------------------------------------------------------- */
#endif
