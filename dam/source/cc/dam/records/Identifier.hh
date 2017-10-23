// -*-Mode: C++;-*-

#ifndef RECORDS_IDENTIFIER_HH
#define RECORDS_IDENTIFIER_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     records/Identifier.hh
 *  @brief    Identifier defintion
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
 *  proto-dune DAM 
 *
 *  @author
 *  <russell@slac.stanford.edu>
 *
 *  @par Date created:
 *  <2017/10/12>
 *
 * @par Credits:
 * SLAC
 *
 * @par
 * An Identifier is not a record, but rather the auxilliary block of the
 * fragment header.  As such it has no formal heading of its own, but
 * rather uses information in the fragment header to define its length
 * and format.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.10.12 jjr Separated from Headers.hh
  
\* ---------------------------------------------------------------------- */


#include <cinttypes>



/* ====================================================================== */
/* class Identifier                                                       */
/* ---------------------------------------------------------------------- */
namespace pdd     {
namespace record  {
/* ---------------------------------------------------------------------- *//*!

  \class Identifier
  \brief Gives the spatial (which electronics components) and temporal 
         (the timestamp/instance) of the fragment

   This class is normally used asthe auxilliary block of the fragment 
   header.
                                                                          */
/* ---------------------------------------------------------------------- */
class Identifier
{
public:
   explicit Identifier ()  { return; }

public:
   /* ------------------------------------------------------------------- *//*!

     \enum  class FormatType
     \brief This enumerates the identifier formats. It is really more
            for documentation than usage
                                                                          */
   /* ------------------------------------------------------------------- */
   enum class FormatType
   {
      _0 = 0,  /*!< Only 1 source identifier                              */
      _1 = 1   /*!< Two source identifiers                                */
   };
   /* ------------------------------------------------------------------- */

   enum class Size
   {
      Format   =  4, /*!< Size of the format field                       */
      Reserved =  4, /*!< Reserved, must be 0                            */
      Src0     = 12, /*!< Channel bundle 0 source identifier (from WIB)  */
      Src1     = 12, /*!< Channel bundle 1 source identified (from WIB)  */
      Sequence = 32  /*!< Overall sequence number                        */
   };

   enum class Mask : uint32_t
   {
      Format   = 0x0000000f,
      Type     = 0x0000000f,
      Src0     = 0x00000fff,
      Src1     = 0x00000fff,
      Sequence = 0xffffffff
   };

   enum Offset
   {
      Format    = 0,
      Type      = 4,
      Src0      = 8,
      Src1      = 20,
      Sequence  = 32
   };


public:
   uint64_t       m_w64; /*!< 64-bit packed word of format,srcs, sequence */
   uint64_t m_timestamp; /*!< The identifying timestamp                   */
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
/*   END: Identifier                                                      */
} /* END: namespace record                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */

#endif
