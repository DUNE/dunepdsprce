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
   2017.10.04 jjr Changed the vector signatures from std::vector<uint16_t>
                  to TpcAdcVector.  This is still a std::vector, but 
                  allocates memory on cache line boundaries.

                  Eliminated Identifier::getChannelndex.  The identifier
                  for a stream is common to all channels, so this 
                  functionality is at best unnecessary and, at worse, 
                  misleading, causing the user to think there is a 
                  different identifier for each channel.

                  Eliminated the nticks parameter from 
                  getMultiChannelData(uint16_t *adcs) const

                  This decreases its flexibility since the number of ADCs
                  in each channel is fixed at the number of ticks in the
                  event time window.  If there is a need for having 
                  control over this number (essentially the stride) then
                  a new method will be added.

   2017.08.31 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/access/TpcStream.hh"
#include "dam/TpcAdcVector.hh"


#include <cstdint>
#include <vector>


/* ---------------------------------------------------------------------- *//*!

   \brief Unpacks and accesses the data in one TPC Stream.  A TPC stream
          is associated with the data that is carried on one WIB fiber.
    
   \par
    In addition to the WIB fiber data, a TpcStream carries meta-information
    that lends context to and helps locate the adcs comprising the TPC
    data.

    Two primary methods are
       -# Defining the event time window.  This is used to select the
          correct amount of time samples.
       -# Transposing the channel and time order.  For many uses it is
          convenient to organize the data as vectors of time samples 
          (ADCS), by channel.
                                                                          */
/* ---------------------------------------------------------------------- */
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

   // Timestamp of beginning of the trimmed waveform
   timestamp_t getTimeStamp () const; 

   // timestmap of the beginning of the untrimmed event.
   timestamp_t getTimeStampUntrimmed () const; 

   // true if stream has a capture error on any tick
   bool hasCaptureError () const; 

   // true if stream has any checksum error
   bool hasChecksumError() const; 


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
      bool     isOkay          () const;

      uint32_t m_w32;
   };


   // ------------------------------------------------------------------
   // Return the stream's identifier.  This is a packed version of the
   // WIB's Crate.Slot.Fiber.  Access methods are provided by the 
   // Identifier class to retrieve the crate, slot and fiber.
   // ------------------------------------------------------------------
   Identifier getIdentifier  () const;


   // -------------------------------------------------------------------
   //
   //  Unpack all channels of a TPC stream and data. These come in 2 
   //  flavors
   //      -# One to return only the data within the event time window
   //      -# One to return all the data (the Untrimmed versions)
   //
   //  and each with three methods to allow flexible placement of the
   //  data
   //      -# In contiguous memory.  This is basically a two dimensional
   //         array adcs[NCHANNELS][NTICKS]
   //      -# In channel-by-channel arrays.  An array of NCHANNELS
   //         pointers are provided, with each pointing at least 
   //         enough memory to hold NTICKS worth of data.
   //      -# A vector of vectors.  While these can and will expand
   //         to hold the requistion number of channels and ticks, the
   //         caller is encourage to pre-size these vectors to avoid
   //         the overhead.  Note that if these are expanded, any 
   //         existing data is lost. (A reserve rather than resize
   //         is used to do the allocation.)
   //
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
   //        imposes some run-time penalities. These vectors should be pre-sized
   //        to accommodate the data. Failure to do so will result in having
   //        to save and restore a fair amount of context with each decode.
   //
   // ------------------------------------------------------------------------
   bool getMultiChannelData          (uint16_t                  *adcs) const;
   bool getMultiChannelData          (uint16_t                 **adcs) const;
   bool getMultiChannelData          (std::vector<TpcAdcVector> &adcs) const;

   bool getMultiChannelDataUntrimmed (uint16_t  *adcs,     int nticks) const;
   bool getMultiChannelDataUntrimmed (uint16_t **adcs,     int nticks) const;
   bool getMultiChannelDataUntrimmed (std::vector<TpcAdcVector> &adcs) const;

   // -----------------------
   // Mainly for internal use
   // -----------------------
public:
   pdd::access::TpcStream const *getStream ()       { return &m_stream; }
   pdd::access::TpcStream const &getStream () const { return  m_stream; }

private:
   pdd::access::TpcStream const m_stream;
};
/* ---------------------------------------------------------------------- */
#endif
