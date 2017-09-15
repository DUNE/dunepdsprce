// -*-Mode: C++;-*-

#ifndef PDD_TPCSTREAMUNPACK_HH
#define PDD_TPCSTREAMUNPACK_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcStreamUnpack.hh
 *  @brief    Proto-Dune Online/Offline Data Tpc Access Routines
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
 *  pdd
 *
 *  @author
 *  <russell@slac.stanford.edu>
 *
 *  @par Date created:
 *  <2017/08/31>
 *
 * @par Credits:
 * SLAC
 *
 * This layout the format and primitive access methods to the data
 * found in a TpcStream.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.08.31 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/TpcStream.hh"

#include <cstddef>
#include <cstdint>
#include <vector>




class TpcStreamUnpack
{
  public:
   explicit TpcStreamUnpack () { return; }
   TpcStreamUnpack (pdd::access::TpcStream const &tpcStream);

   typedef uint64_t timestamp_t;

   void print(); 

   // Number of channels serviced by this fragment
   size_t getNChannels() const;


   // Trimmed number of time samples
   size_t getNTicks   () const;


   // Untrimmed number of time samples in a stream
   size_t getNTicksUntrimmed () const;


   // -----------------------------------------------------------------
   // Stream Identification
   // This is currently the WIB's Crate.Slot.Fiber, but is kept generic
   // -----------------------------------------------------------------
   class Identifier
   {
   public:
      Identifier (uint32_t wrd) : m_w32 (wrd) { return; }

        //  Return Crate, Slot, Fiber, Channel Index
      uint32_t getCrate        () const;
      uint32_t getSlot         () const;
      uint32_t getFiber        () const;
      uint32_t getChannelIndex () const;
      bool     isOkay          () const;

      uint32_t m_w32;
   };

   Identifier getIdentifier  () const;

   
   // method to unpack all channels in a fragment
   bool getMultiChannelData (uint16_t  *adcs, int nticks)              const;
   bool getMultiChannelData (uint16_t **adcs)                          const;
   bool getMultiChannelData (std::vector<std::vector<uint16_t>> &adcs) const;


   //  Returns true if no errors were found and false otherwise. 
   //  To discuss:  Do we want fine-grained error return codes?  
   //  Some possible errors  
   //     1) missing data for one or more channels,
   //     2) timestamp mismatches for different channels within a fragment, 
   //     3) checksum or capture error check. 
   //
   //  The interface assumes that memory has been pre-allocated by the caller
   //  using the information from
   //     1) getNChannels (), which gives then number of channels available
   //     2) getNTicks    (), which gives the number of ticks in each channel
   //
   //  There are interfaces to support different ways of representing this
   //     1) A contigous block that is really a 2-D array [Nchannels][Nticks]
   //        Since this is a same size for each channel, Nticks must be the
   //        largest value.
   //     2) An array of Nchannel pointers, each pointing to an array largest
   //        enough to hold the number of ticks as returned by
   //        getNticks (ichannel)
   //     3. A vector of vectors. This is the most versatile, but likely 
   //        imposes some run-time penalities. These vectors should be presized
   //        to accommodate the data. Failure to do so will result in having
   //        to save and restore a fair amount of context with each decode.
   //         --> I may jacket underlying decode method to ensure this does not
   //             happen, but this is yet another overhead, having to check
   //             and resize the arrays.  Have to see what the cost is for this.
   bool getMultiChannelDataUntrimmed (uint16_t  *adcs, int nticks)              const;
   bool getMultiChannelDataUntrimmed (uint16_t **adcs, int nticks)              const;
   bool getMultiChannelDataUntrimmed (std::vector<std::vector<uint16_t>> &adcs) const;

   
   // Timestamp of beginning of the trimmed waveform
   timestamp_t getTimeStamp () const; 

   // timestmap of the beginning of the untrimmed event.
   timestamp_t getTimeStampUntrimmed () const; 

   // true if stream has a capture error on any tick
   bool hasCaptureError () const; 

   // true if stream has any checksum error
   bool hasChecksumError() const; 


public:
   pdd::access::TpcStream m_rawStream;
};
/* ---------------------------------------------------------------------- */
#endif
