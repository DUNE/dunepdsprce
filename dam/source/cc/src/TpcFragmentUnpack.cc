// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcFragmentUnpack.cc
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
   2017.08.29 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/DataFragmentUnpack.hh"
#include "dam/TpcFragmentUnpack.hh"
#include "dam/TpcStream.hh"
#include "dam/TpcRecords.hh"
#include <cstddef>



/* ---------------------------------------------------------------------- *//*!

  \brief Construct the Tpc Fragment Accessor

  \param[in] df Generic DataFragment
                                                                          */
/* ---------------------------------------------------------------------- */
TpcFragmentUnpack::TpcFragmentUnpack (DataFragmentUnpack const &df) :
   m_df (df)
{
   pdd::fragment::Data       const      *data = df.getData ();
   pdd::fragment::TpcStream  const *rawStream = 
                  reinterpret_cast<decltype (rawStream)>(data);

   // ----------------------------------------
   // Should check that the number of streams 
   // does not exceed the allotted storage.
   // ----------------------------------------
   int nstreams = rawStream->getLeft () + 1;
   int istream  = 0; 


   for (int istream = 0; istream < nstreams; ++istream)
   {

      m_tpcStreams[istream++].m_rawStream.construct (rawStream);
   
      uint64_t        n68 = data->getN64 ();
      uint64_t const *p68 = reinterpret_cast<decltype (p68)>(rawStream);
      int            left = rawStream->getLeft ();


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
      rawStream  = reinterpret_cast<decltype (rawStream)>(p68) + n68;
      m_tpcStreams[istream].m_rawStream.construct (rawStream);
   }

   
   // -----------------------------------------------------
   // Should check that the number of found streams matches
   // -----------------------------------------------------
   if (istream != nstreams)
   {
      // ERRROR
   }

   m_nstreams = nstreams;
   

   return;
}
/* ---------------------------------------------------------------------- */



void TpcFragmentUnpack::Print ()
{
   return;
}



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the number of Tpc streams
  \return The number of Tpc streams
                                                                          */
/* ---------------------------------------------------------------------- */
int TpcFragmentUnpack::getNStreams () const 
{
   return m_nstreams;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the specfied TpcStream or NULL if non-existent
  \return The specfied TpcStream or NULL if non-existent
                                                                          */
/* ---------------------------------------------------------------------- */
TpcStreamUnpack const *TpcFragmentUnpack::getStream (int istream) const 
{
   return  istream < m_nstreams
         ? &m_tpcStreams[istream] 
         : reinterpret_cast<TpcStreamUnpack const *>(0);
}
/* ---------------------------------------------------------------------- */


      
