// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcFragment.cc
 *  @brief    Access methods for the TPC data record
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
   2018.09.13 jjr Added protection in constructor to avoid accessing 
                  non-existent streams.  Due to an error in rceServer
                  it sometimes indicates that there are 2 streams when, in
                  fact, there is only 1.
   2016.10.06 jjr Changed method void Print () -> void print () const
   2017.08.29 jjr Created
  
\* ---------------------------------------------------------------------- */


// ----------------------------------
// Set the implementation to external
// ----------------------------------
#define   TPCFRAGMENT_IMPL extern

#include "TpcFragment-Impl.hh"
#include "DataFragment-Impl.hh"
#include "TpcStream-Impl.hh"
#include <cstddef>


namespace pdd    {
namespace access {
/* ---------------------------------------------------------------------- *//*!

  \brief Construct the Tpc Fragment Accessor

  \param[in] df Generic DataFragment
                                                                          */
/* ---------------------------------------------------------------------- */
TpcFragment::TpcFragment (DataFragment const &df) :
   m_df (df)
{
   // ---------------------------------------
   // Make sure this is not an empty fragment
   // ---------------------------------------
   if (df.isTpcEmpty ()) 
   {
      m_nstreams = 0;
      return;
   }

   pdd::record::Data       const      *data = df.getData ();
   pdd::record::TpcStream  const *rawStream =  
                  reinterpret_cast<decltype (rawStream)>(data);

   pdd::record::TpcStreamHeader const *hdr =
                 reinterpret_cast<decltype(hdr)>(rawStream);


   // ----------------------------------------
   // Should check that the number of streams 
   // does not exceed the allotted storage.
   // ----------------------------------------
   int nstreams                = hdr->getLeft () + 1;
   if (nstreams>2)
     {
       std::cout << "RCE TPCFragment constructor nstreams " << nstreams << " > 2.  Skipping" << std::endl;
       m_nstreams = 0;
       return;
     }
   int istream                 = 0; 
   pdd::Trailer const *trailer = df.getTrailer ();


   for (istream = 0; istream < nstreams; ++istream)
   {
      if (reinterpret_cast<void const *>(rawStream) >= 
          reinterpret_cast<void const *>(trailer))
      {
         break;
      }
      m_tpcStreams[istream].construct (rawStream);
   
      uint64_t        n64 = data->getN64 ();
      uint64_t const *p64 = reinterpret_cast<decltype (p64)>(rawStream);
      int            left = hdr->getLeft ();


      // -----------------------------------------------------
      // Could check that the number left matches the expected
      // -----------------------------------------------------
      
      // ---------------
      // Quit if no more
      // ---------------
      if (left == 0) break;

      // -----------------------------
      // More, get the next Tpc Stream
      // -----------------------------
      rawStream  = reinterpret_cast<decltype (rawStream)>(p64 + n64);
   }

   
   // -----------------------------------------------------
   // Should check that the number of found streams matches
   // -----------------------------------------------------
   if (istream != nstreams)
   {
      // ERRROR
   }

   m_nstreams = istream;
   

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Routine to print at least some part of a Tpc Fragment.

  \par
   Given the size and complexity of a TpcFragment, it is not feasible
   to provide a one-size-fits-all, so this routine is more of an outline
   of its contents.
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcFragment::print () const
{
   puts ("TpcFrament print not ready yet");
   return;
}
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
/* ---------------------------------------------------------------------- */
} /* END: namespace pdd                                                   */
/* ====================================================================== */


      
