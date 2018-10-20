// -*-Mode: C++;-*-

#ifndef RECORD_DATAFRAGMENT_HH
#define RECORD_DATAFRAGMENT_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     record/DataFragment.hh
 *  @brief    RCE data fragment record definition
 *            decoding a binary test file.
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
   2018.08.30 jjr Added TpcEmpty to record types
                  Corrected isTpc; had cut and paste error that used
                  TpcNormal twice in forming the bit mask of TPC record
                  types.
   2017.08.29 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/access/Headers.hh"
#include "dam/records/Identifier.hh"
#include "dam/records/Originator.hh"
#include <cinttypes>


namespace pdd    {
namespace record {
/* ---------------------------------------------------------------------- *//*!

  \brief Specialized fragment header for Data
                                                                          */
/* ---------------------------------------------------------------------- */
class DataFragmentHeader : public pdd::Header0
{
   explicit DataFragmentHeader () { return; }

public:
   enum class RecType 
   {
      Reserved_0   = 0,  /*!< Reserved for future use                     */
      Originator   = 1,  /*!< Originator record type                      */
      TpcNormal    = 2,  /*!< Normal  TPC data, \e i.e. no errors         */
      TpcDamaged   = 3,  /*!< Damaged TPC data, \e i.e. has errors        */
      TpcEmpty     = 4   /*!< No      TPC data,                           */
   };

   RecType getRecType () const;


   Identifier getIdentifier () const  { return m_identifier; }
   bool       isTpc         () const  { return isTpc        (getRecType ()); }
   bool       isTpcNormal   () const  { return isTpcNormal  (getRecType ()); }
   bool       isTpcDamaged  () const  { return isTpcDamaged (getRecType ()); }
   bool       isTpcEmpty    () const  { return isTpcEmpty   (getRecType ()); }


   static bool isTpcNormal  (RecType recType);
   static bool isTpcDamaged (RecType recType);
   static bool isTpcEmpty   (RecType recType);
   static bool isTpc        (RecType recType);

public:
   pdd::record::Identifier m_identifier;
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
} /* END: namespace record                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION: DataFragmentHeader                                     */
/* ---------------------------------------------------------------------- */
namespace pdd    {
namespace record {

/* ---------------------------------------------------------------------- */
inline pdd::record::DataFragmentHeader::RecType 
                 DataFragmentHeader::getRecType () const
{
   return static_cast<DataFragmentHeader::RecType>(Header0::getSubtype ());
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
inline bool         DataFragmentHeader::
       isTpcNormal (DataFragmentHeader::RecType recType)
{
   return (recType == DataFragmentHeader::RecType::TpcNormal);
}

inline bool          DataFragmentHeader::
       isTpcDamaged (DataFragmentHeader::RecType recType)
{
   return (recType == DataFragmentHeader::RecType::TpcDamaged);
}

inline bool          DataFragmentHeader::
       isTpcEmpty   (DataFragmentHeader::RecType recType)
{
   return (recType == DataFragmentHeader::RecType::TpcEmpty);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
inline bool   DataFragmentHeader::
       isTpc (DataFragmentHeader::RecType recType)
{
   static const uint32_t TpcTypes = 
      (1 << (static_cast<int>(DataFragmentHeader::RecType::TpcNormal ))) |
      (1 << (static_cast<int>(DataFragmentHeader::RecType::TpcDamaged))) |
      (1 << (static_cast<int>(DataFragmentHeader::RecType::TpcEmpty  )));

   int rtm = 1 << static_cast<uint32_t>(recType);

   return (rtm & TpcTypes) != 0;
}
/* ---------------------------------------------------------------------- */
/*   END: DataFragmentHeader                                              */
} /* END: namespace record                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */
#endif
