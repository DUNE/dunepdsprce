// -*-Mode: C++;-*-

#ifndef RECORDS_TPCPACKET_HH
#define RECORDS_TPCPACKET_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     record/TpcPacket.hh
 *  @brief    Proto-Dune Data Packet definition
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
 *  <2017/10/17>
 *
 * @par Credits:
 * SLAC
 *
 * This defines the format of a TpcDataPacket record found in a Tpc record.
 * The exact layout depends on the format type (WibFrame, compressed, etc),
 * so this is more of a placeholder than anything.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.11.10 jjr Changed the definition of m_w64 to be m_w64[1]. It was
                  incorrectly defined as a uint64_t const *.
   2017.10.16 jjr Moved from dam/access -> dam/records
   2017.08.07 jjr Created
  
\* ---------------------------------------------------------------------- */

#include "dam/access/Headers.hh"
#include <cstdint> 
#include <cstdio>

namespace pdd    {
namespace record {


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
class WibFrame;
/* ====================================================================== */




/* ====================================================================== */
/* CLASSS DEFINITIONS                                                     */
/* ---------------------------------------------------------------------- *//*!

   \brief Header of a TPC data packet
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcPacketHeader : public Header1
{
private:
   TpcPacketHeader () = delete;


public:
   /* ------------------------------------------------------------------- *//*!

     \brief The TpcPacket Bridge word
                                                                          */
   /* ------------------------------------------------------------------- */
   class Bridge 
   {
   private:
      Bridge () = delete;


   public:
      /* ---------------------------------------------------------------- *//*!

         \enum  class Size
         \brief Enumerates the sizes of the bridge word's bit fields.
                                                                          */
      /* ---------------------------------------------------------------- */
      enum class Size: int 
      {
         Format    =  4,   /*!< Size of the bridge format field           */
         Reserved  = 28    /*!< Size of the bridge reserved field         */
      };
      /* ---------------------------------------------------------------- */


      /* ---------------------------------------------------------------- *//*!

         \enum  class Offset
         \brief Enumerates the right justified offsets of the bridge word's 
                bit  fields.
                                                                          */
      /* ---------------------------------------------------------------- */
      enum class Offset: int
      {
         Format    =  0,   /*!< Offset of the bridge format field         */
         Reserved  =  4    /*!< Offset of the bridge reserved field       */
      };
      /* ---------------------------------------------------------------- */


      /* ---------------------------------------------------------------- *//*!

         \enum  class Offset
         \brief Enumerates the right justified masks of the bridge word's
                bit fields.
                                                                          */
      /* ---------------------------------------------------------------- */
      enum class Mask: uint32_t
      {
         Format    = 0xf,      /*!< Mask of the bridge format field       */
         Reserved  = 0xfffffff /*!< Mask of the bridge reserved field     */
      };
      /* ---------------------------------------------------------------- */
};


} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief Body of a TPC data packet
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcPacketBody 
{
private:
   TpcPacketBody () = delete;

public:
   uint64_t const m_w64[1]; /*!< The record body                          */

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief A TPC data packet record
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcPacket : public TpcPacketHeader
{
private:
   TpcPacket () = delete;

public:
   TpcPacketBody m_body;

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
}  /* Namespace:: record                                                  */
}  /* Namespace:: pdd                                                     */
/* ---------------------------------------------------------------------- */
/* END DEFINITIONS                                                        */
/* ====================================================================== */


#endif
