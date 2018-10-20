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
   2018.10.20 jjr Replaced getTimestamps(Untrimmed) with getRange(Untrimmed)
                  This method adds the number of ticks in the trimmed/
                  untrimmed ranges in addition to their timestamps

   2018.09.11 jjr Add 
                   isTpcNormal  ()
                   isTpcDamaged ()
                   getDataFormatType ()

   2018.08.30 jjr Added getTimestamps(Untrimmed) to get the begin, trigger
                  and end timestamps of this TpcStream 

   2018.07.26 jjr Changed ADC type to int16_t

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

   // Query record type
   bool isTpcNormal  () const;
   bool isTpcDamaged () const;


   /* ------------------------------------------------------------------ *//*!

      \brief Enumerates the format of the stored data
                                                                         */
   /* ------------------------------------------------------------------ */
   enum class DataFormatType
   {
      Unknown    = -1, /*!< Unknown, a corruption or new type has been
                            introduced without this software being
                            updated                                      */
      Mixed      =  0, /*!< There are packets of mixed types             */
      WibFrame   =  1, /*!< All packets contain raw WIB frames           */
      Compressed =  2  /*!< All packets contain compressed data          */
   }; 

   // Query data storage method
   DataFormatType getDataFormatType  () const;


   // Number of channels serviced by this fragment
   size_t getNChannels() const;


   //  The methods below give access to information about the timestamps
   //  and number of ticks in
   //     1. The event window
   //        These are the timestamps and number of ticks that defined
   //        the event. e.g. for a 3msecs window, the timespan will be
   //        be 3 msecs and the number of ticks will be 6000.
   //
   //    2.  The untrimmed data
   //        These are the actual timestamps for the beginning and ending
   //        samples and the actual number of ticks in the untrimmed data.
   //        Due to the possible dropped frames the number of ticks may 
   //        not correspond to the time span.
   //
   //    3.  The trimmed data
   //        These are the actual timestamps for the beginning and ending
   //        samples and the actula number of ticks in the trimmed data.
   //        Due to the possible dropped frames the number of ticks may 
   //        not correspond to the time span.
   //
   //  While there are calls to get the number of ticks and beginning 
   //  timestamps these should be considered obsolete. Getting some of
   //  these values is computationally expensive, making getting all the
   //  information at once cheaper.


   // The event window timestamps, returns the # of ticks in the window
   // These values are defined by the pre and post trigger sample counts
   // relative to the actual trigger time.  As such, they may not lie 
   // exactly on a WIB frame timestamp which are quantized to 25 ticks,
   // or .5 usecs.
   size_t      getNTicksWindow     () const;
   timestamp_t getTimeStampWindow  () const;
   uint32_t    getRangeWindow      (size_t       *nticks,
                                    timestamp_t   *begin,
                                    timestamp_t *trigger,
                                    timestamp_t     *end) const;

   // The untrimmed data timestamps, returns the actual # of untrimmed samples
   // The return value defines how many samples the getMultiChannelDataUntrimmed
   // methods will unpack and, in the case of the pseudo 2D array method
   // the number of elements in the channel dimension
   size_t      getNTicksUntrimmed     () const;
   timestamp_t getTimeStampUntrimmed  () const; 
   uint32_t    getRangeUntrimmed (size_t       *nticks, 
                                  timestamp_t   *begin, 
                                  timestamp_t     *end) const;


   // The trimmed data timestamps, returns the actual # of trimmed samples
   // The return value defines how many samples the getMultiChannelData
   // methods will unpack and, in the case of the pseudo 2D array method
   // the number of elements in the channel dimension
   size_t      getNTicks     () const;
   timestamp_t getTimeStamp  () const; 
   uint32_t    getRange      (size_t       *nticks,
                              timestamp_t   *begin,
                              timestamp_t     *end) const;

   // true if stream has a capture error on any tick
   // Not implemented
   bool hasCaptureError () const; 

   // true if stream has any checksum error
   // Not implemented
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
   uint32_t   getStatus      () const;



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
   bool getMultiChannelData          (int16_t                   *adcs) const;
   bool getMultiChannelData          (int16_t                  **adcs) const;
   bool getMultiChannelData          (std::vector<TpcAdcVector> &adcs) const;

   bool getMultiChannelDataUntrimmed (int16_t   *adcs,     int nticks) const;
   bool getMultiChannelDataUntrimmed (int16_t  **adcs,     int nticks) const;
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
