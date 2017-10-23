// -*-Mode: C++;-*-

#ifndef RECORDS_TPCRANGES_HH
#define RECORDS_TPCRANGES_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     records/TpcRanges.hh
 *  @brief    Proto-Dune Data Tpc Range records definition
 *  @verbatim
 *                               Copyright 2013
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
 *  <2017/08/07>
 *
 * @par Credits:
 * SLAC
 *
 * This gives the layout and definition of a TpcRanges record.  This record
 * is found in the data section of a TpcFragment and gives both the
 * timestamp ranges of both the untrimmed and trimmed data along with
 * information to locate the trimmed data within the data packets.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.10.13 jjr Moved from dam/access -> dam/records
   2017.08.07 jjr Created
  
\* ---------------------------------------------------------------------- */

#include "dam/access/Headers.hh"
#include <cstdint> 
#include <cstdio>

namespace pdd    {
namespace record {

/* ====================================================================== */
/* CLASSS DEFINITIONS                                                     */
/* ---------------------------------------------------------------------- *//*!

   \brief This specializes the bridge word of a standard header for use
          Ranges usage.
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcRangesHeader : public pdd::Header2
{
private:
   TpcRangesHeader () = delete;

public:
   class Bridge
   {
   private:
      Bridge () = delete;

   public:

      /* ---------------------------------------------------------------- *//*!

         \enum  class Size
         \brief Enumerates the sizes of the Bridge bit fields.
                                                                          */
      /* -------------------------------------------------------------- - */
      enum class Size: int
      {
         Format   =  4,  /*!< Size of the format field                    */
         Reserved =  8   /*!< Size of the reserved field                  */
      };
      /* ---------------------------------------------------------------- */



      /* ---------------------------------------------------------------- *//*!

         \enum  class Offset
         \brief Enumerates the right justified offsets of the Bridge bit 
                fields.
                                                                          */
      /* ---------------------------------------------------------------- */
      enum class Offset: int
      {
         Format    =  0, /*!< Offset of the format field                  */
         Reserved  =  4  /*!< Offset of the reserved field                */
      };
      /* ---------------------------------------------------------------- */


      /* ---------------------------------------------------------------- *//*!

        \enum  class Offset
        \brief Enumerates the right justified masks of the Bridge bit 
               fields.
                                                                          */
      /* ---------------------------------------------------------------- */
      enum class Mask: uint32_t
      {
         Format   =  0x0000000f, /*!< Mask for the format field           */
         Reserved =  0x000000ff  /*!< Mask for the reserved field         */
      };
      /* ---------------------------------------------------------------- */
   };
   /* ------------------------------------------------------------------- */
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!
 
   \struct TpcRangesIndices
   \brief  Defines the beginning and ending of the event in terms
           of an index into the data packets for one contributor.

     An index consists of two 16-bit values. The upper 16-bits gives the
     packet number and the lower 16-bits gives the offset into the packet.
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcRangesIndices
{
private:
   TpcRangesIndices () = delete;

public:
   uint32_t   m_begin; /*!< Index of beginning of event time sample */
   uint32_t     m_end; /*!< Index to ending   of event time sample  */
   uint32_t m_trigger; /*!< Index to event triggering time sample   */    
   
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */   


/* ---------------------------------------------------------------------- *//*!
  
  \struct TpcRangesTimestamps
  \brief  Gives the beginning and ending timestamp of the data this 
          contributor
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcRangesTimestamps
{
private:
   TpcRangesTimestamps () = delete;
   
public:
   uint64_t m_begin;  /*!< Begining timestamp of the range                */
   uint64_t   m_end;  /*!< Ending   timestamp of the range                */
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ------------------------------------------------------------------- *//*!

  \struct TpcRangesWindow
  \brief  Gives the timestamps of the beginning, ending and trigger.

  \note
  This window is a property of the trigger and, therefore, in practice
  is common for all contributors.  However for completeness it is 
  include with each.
                                                                       */
/* ------------------------------------------------------------------- */
class TpcRangesWindow
{
private:
   TpcRangesWindow () = delete;

public:
   uint64_t    m_begin; /*!< Beginning timestamp of the event window      */
   uint64_t      m_end; /*!< Begin     timestamp of the event window      */
   uint64_t  m_trigger; /*!< Triggering timestamp                         */
      
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \class TpcRangesDescriptor
  \brief The range definitions for one a contributor
   
  Since contributors are feed by different WIB fibers, there
  is no guarantee of synchonization on these streams due to missing
  WIB frames.  Therefore, each contributor stream must have is own
  range description.
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcRangesDescriptor
{
private:
   TpcRangesDescriptor () = delete;

public:
   TpcRangesIndices       m_indices; /*!< Indices to event time samples    */
   TpcRangesTimestamps m_timestamps; /*!< Begin/end of dpacket timestamps  */

} __attribute__ ((packed));
/* `---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \class TpcRangesBody
  \brief Defines the timestamps and packet indices of both the untrimmed
         and trimmed (the event) windows
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcRangesBody
{
private:
   TpcRangesBody () = delete;

public:
   TpcRangesDescriptor m_dsc; /*!< Range descriptor                       */
   TpcRangesWindow  m_window; /*!< Beginning and ending event timestamps  */

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief The Tpc Ranges record.  This gives the timestamp ranges of 
         both the trimmed (event) and untrimmed data.
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcRanges : public TpcRangesHeader
{
public:
   TpcRanges () = delete;

public:
   TpcRangesBody  m_body;   /*!< The body of the TpcRange record          */

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
/*    END: class TpcRanges                                                */
}  /* END: Namespace record                                               */
}  /* END: Namespace pdd                                                  */
/* ====================================================================== */

#endif
