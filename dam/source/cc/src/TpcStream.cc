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
   2017.10.09 jjr Moved some methods to TpcRecords-Inline.hh for better
                  performance while making them external here for external
                  users.
   2017.08.29 jjr Created
  
\* ---------------------------------------------------------------------- */

// Do not inline the TpcStream-Inline methods.
#define TPCSTREAM_IMPL extern

#include "TpcStream-Impl.hh"
#include <cstdio>

namespace pdd    {
namespace record {



/* ---------------------------------------------------------------------- *//*!

  \class TpcStream
  \brief The TPC Stream record.  

   This may include any of the following records

     -# Table of Contents, 
        this provides a description and the location of each TPC data
        packet. It allows random location of both the first packet in
        the HLS streams and the packets within that stream.  Future
        versions will allow random location of the channels within the
        packets
    -#  Error Record
        This is an optional/as needed record describing any error
        conditions
    -#  The Tpc Data packets.

                                                                          */
/* ---------------------------------------------------------------------- */
class TpcStream : public TpcStreamHeader
{
private:
   TpcStream () = delete;

public:
   void const *getBody () const { return this + 1; }

public:
   // ----------------
   // TPC Record Types
   // ---------------
   enum class RecType
   {
      Reserved   = 0,   /*!< Reserved                                     */
      Toc        = 1,   /*!< Table of Contents record                     */
      Ranges     = 2,   /*!< Event range descriptor record                */
      Packets    = 3    /*!< Data packets                                 */
   };

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
}  /* Namespace:: record                                                  */
/* ---------------------------------------------------------------------- */
}  /* Namespace:: pdd                                                     */
/* ====================================================================== */



namespace pdd    {
namespace access {
/* ---------------------------------------------------------------------- *//*!

  \brief Constructs the accessor for a TpcStream data record

  \param[in] dr  Pointer to the TpcStream data record

   The TpcStream data record consists of a number of subrecords. These
   subrecords are located by a serial scan for the subrecords and 
   pointers to the subrecords are populated.  Subrecords that do not exist
   have their pointers set to NULL.
                                                                          */
/* ---------------------------------------------------------------------- */
TpcStream::TpcStream (pdd::record::TpcStream const *stream)
{
   construct (stream);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief This is the delegated constructor for a TPC data record.

   The TpcStream data record consists of a number of subrecords. These
   subrecords are located by a serial scan for the subrecords and 
   pointers to the subrecords are populated.  Subrecords that do not exist
   have their pointers set to NULL.
                                                                          */
/* ---------------------------------------------------------------------- */
void TpcStream::construct (pdd::record::TpcStream const *stream)
{   
   auto tpc = m_stream = reinterpret_cast<decltype (m_stream)>(stream);

   // -----------------------------------------------------
   // Get the number of 64-bit left to process
   // This is length of the data record - header length (1)
   // -----------------------------------------------------
   int32_t      left64 = tpc->getN64  () 
                       - sizeof (pdd::record::TpcStreamHeader) 
                       / sizeof (uint64_t);
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
      pdd::record::TpcStream::RecType type = 
           static_cast<pdd::record::TpcStream::RecType>((hdr >> 4) & 0xf);


      if      (type == pdd::record::TpcStream::RecType::Ranges)
      {
         m_ranges  = reinterpret_cast<decltype (m_ranges)>(p64);
      }

      else if (type == pdd::record::TpcStream::RecType::Toc)
      {
         m_toc    = reinterpret_cast<decltype (m_toc)>(p64);
      }

      else if (type == pdd::record::TpcStream::RecType::Packets)
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
/* ---------------------------------------------------------------------- */
}}


namespace pdd { namespace record {
/* ---------------------------------------------------------------------- */
void TpcStreamHeader::print () const 
{ 
   print (this);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
void TpcStreamHeader::print (DataHeader const *dh)
{
   uint64_t         hdr = dh->retrieve ();
   unsigned int  format = DataHeader::getFormat (hdr);
   unsigned int    type = DataHeader::getType   (hdr);
   unsigned int     n64 = DataHeader::getN64    (hdr);
   uint32_t      bridge = DataHeader::getBridge (hdr);
   
   printf ("%-10.10s: Type.Format = %1.1x.%1.1x Length = %6.6x Bridge = %8.8" 
           PRIx32 "\n",
           "DataRecord",
           type,
           format,
           n64,
           bridge);
   
   return;
}
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
/* ---------------------------------------------------------------------- */
} /* END: namespace pdd                                                   */
/* ====================================================================== */

