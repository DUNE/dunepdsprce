// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcStream.cc
 *  @brief    Access methods for the RCE Tpc Stream data records
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
   2017.08.29 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "dam/TpcStream.hh"
#include "dam/TpcRecords.hh"

#include <cstdio>

namespace pdd    {
namespace access {

/* ---------------------------------------------------------------------- *//*!

  \brief Constructs the accessor for a TpcStream data record

  \param[in] dr  Pointer to the TpcStream data record

   The TpcStream data record consists of a number of subrecords. These
   subrecords are located by a serial scan of the record and pointers
   to the subrecords are populated.  Subrecords that do not exist have
   there pointers set to NULL.
                                                                          */
/* ---------------------------------------------------------------------- */
TpcStream::TpcStream (pdd::fragment::TpcStream const *stream)
{
   construct (stream);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief This is the delegated constructor for a TPC data record.

   The TpcStream data record consists of a number of subrecords. These
   subrecords are located by a serial scan of the record and pointers
   to the subrecords are populated.  Subrecords that do not exist have
   there pointers set to NULL.
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcStream::construct (pdd::fragment::TpcStream const *stream)
{
   
   auto tpc = m_record = reinterpret_cast<decltype (m_record)>(stream);

   int32_t      left64 = tpc->getN64  ();
   uint64_t const *p64 = reinterpret_cast<decltype (p64)>(tpc->getBody ());

   m_ranges = 0;
   m_toc    = 0;
   m_packet = 0;


   // ----------------------------------------
   // Scan for the records in this data record
   // ----------------------------------------
   do
   {
      uint64_t hdr = p64[0];


      // !!! KLUDGE: Getting the format and type should be methods
      // !!! KLUDGE: This should scan for these types
      int                          fmt = hdr & 0xf;
      pdd::fragment::TpcStream::RecType type = 
           static_cast<pdd::fragment::TpcStream::RecType>((hdr >> 4) & 0xf);


      if      (type == pdd::fragment::TpcStream::RecType::Ranges)
      {
         m_ranges  = reinterpret_cast<decltype (m_ranges)>(p64);
      }

      else if (type == pdd::fragment::TpcStream::RecType::Toc)
      {
         m_toc    = reinterpret_cast<decltype (m_toc)>(p64);
      }

      else if (type == pdd::fragment::TpcStream::RecType::Packets)
      {
         m_packet = reinterpret_cast<decltype (m_packet)>(p64);
      }

      int32_t n64;
      
      // --------------------------
      // Advance to the next record
      // --------------------------
      if (fmt == 1) 
      {
         n64 = pdd::Header1::getN64 (hdr);
      }
      else if (fmt == 2)
      {
         n64 = pdd::Header2::getN64 (hdr);
      }
      else if (fmt == 0)
      {
         n64 = pdd::Header0::getN64 (hdr);
      }
      else
      {
         break;
      }

      p64    += n64;
      left64 -= n64;
      
   }
   while (left64 > 0);

   return;
}

uint32_t TpcStream::getCsf () const { return m_record->getCsf  (); }
int      TpcStream::getLeft() const { return m_record->getLeft (); }

/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
/* ---------------------------------------------------------------------- */
} /* END: namespace pdd                                                   */
/* ====================================================================== */
