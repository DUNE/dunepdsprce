// -*-Mode: C++;-*-

#ifndef RECORDS_WIB_FRAME_HH
#define RECORDS_WIB_FRAME_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     records/WibFrame.hh
 *  @brief    Definition of a WibFrame
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
 *  DUNE
 *
 *  @author
 *  russell@slac.stanford.edu
 *
 *  @par Date created:
 *  2017.10.188
 * *
 * @par Credits:
 * SLAC
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\

   HISTORY
   -------

   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2018.10.09 jjr Updated the Stream 1 & 2 Error fields to reflect a change
                  from a nibble to a byte.  This eliminates 8-b reserved
                  field that was in the next 8 bits.
                  Comments also corrected
   2018.07.13 jjr Corrected Mask0:Id from 0x3ff -> 0x7ff.  There are 11 
                  bits in the WIB identifier.  Also the Offset0::Crate
                  was incorrect 18 -> 16.
   2017.10.18 jjr Separate defintion from implementation
   ---------- --  --------------------------------------
   2017.10.05 jjr Added transposeAdcs128xN. These are methods that optimize
                  the transpose for an arbitrary number of frames.  
                  NOTE: Current implementation still limits this to
                        a multiple of 8.
   2017.08.16 jjr Cloned from RCE firmware version
   ---------- --- --------------------------------
   2017.04.19 jjr Updated to the format Eric Hazen published on 2017.03.13
   2016.10.18 jjr Corrected K28.5 definition, was 0xDC -> 0xBC
   2016.06.14 jjr Created

\* ---------------------------------------------------------------------- */




/*
   WORD     Contents
      0     Err[16] | Rsvd[24] | Slot [3] | Crate[5] | Fiber[3] | Version[5] | K28.5[8]
      1     Timestamp [64]

              ColdData Stream 1
      2     CvtCnt[16] | ChkSums_hi[16] | ChkSums_lo[16] | SErr_2[8] | SErr2_1[18]
      3              Hdrs[32]           |       Rsvd[16] |    ErrReg[16]
      4-15  ColdData.Adcs

            ColdData Stream 2
      16    CvtCnt[16] | ChkSums_hi[16] | ChkSums_lo[16] | SErr_2[8] | SErr2_1[18]
      17             Hdrs[32]           |       Rsvd[16] |    ErrReg[16]
      18-29 ColdData.Adcs


     K28.1 = 0x3c
     K28.2 = 0x5c
     K28.5 = 0xDC
     +----------------+----------------+----------------+----------------+
     |3333333333333333|2222222222222222|1111111111111111|                |
     |fedcba9876543210|fedcba9876543210|fedcba9876543210|fedcba9876543210| 
     +----------------+----------------+----------------+----------------+
 0   |      Error     |    Reserved[24]         SltCrate|FbrVersn   K28.5|
 1   |                    GPS TImestamp[64]                              |
     +================+================+================+================+
     |                            Channels 0 - 63                        |
     +----------------+----------------+----------------+----------------+
 2   |          Timestamp              |   CheckSums    | SErr 2  SErr 1 |
 3   |           Hdrs[7-0]             |    Reserved    |   Registers    |
     +----------------+----------------+----------------+----------------+
 4   |5555444444444444 3333333333332222 2222222211111111 1111000000000000| S1
 5   |aaaaaaaa99999999 9999888888888888 7777777777776666 6666666655555555|  &
 6   |ffffffffffffeeee eeeeeeeedddddddd ddddcccccccccccc bbbbbbbbbbbbaaaa| S2
     +----------------+----------------+----------------+----------------+
 7   |5555444444444444 3333333333332222 2222222211111111 1111000000000000| S3
 8   |aaaaaaa999999999 9999888888888888 7777777777776666 6666666655555555|  &
 9   |ffffffffffffeeee eeeeeeeedddddddd ddddcccccccccccc bbbbbbbbbbbbaaaa| S4
     +----------------+----------------+----------------+----------------+
10   |5555444444444444 3333333333332222 2222222211111111 1111000000000000| S5
11   |aaaaaaa999999999 9999888888888888 7777777777776666 6666666655555555|  &
12   |ffffffffffffeeee eeeeeeeedddddddd ddddcccccccccccc bbbbbbbbbbbbaaaa| S6
     +----------------+----------------+----------------+----------------+
13   |5555444444444444 3333333333332222 2222222211111111 1111000000000000| S7
14   |aaaaaaa999999999 9999888888888888 7777777777776666 6666666655555555|  &
15   |ffffffffffffeeee eeeeeeeedddddddd ddddcccccccccccc bbbbbbbbbbbbaaaa| S8
     +================+================+================+================+
     |                            Channels 64 - 127                      |
     +----------------+----------------+----------------+----------------+
16   |          Timestamp              |    CheckSums   | SErr 2  SErr 1 |
17   |           Hdrs[7-0]             |    Reserved    |    Register    |
     +----------------+----------------+----------------+----------------+
18   |5555444444444444 3333333333332222 2222222211111111 1111000000000000| S1
19   |aaaaaaaa99999999 9999888888888888 7777777777776666 6666666655555555|  &
20   |ffffffffffffeeee eeeeeeeedddddddd ddddcccccccccccc bbbbbbbbbbbbaaaa| S2
     +----------------+----------------+----------------+----------------+
21   |5555444444444444 3333333333332222 2222222211111111 1111000000000000| S3
22   |aaaaaaa999999999 9999888888888888 7777777777776666 6666666655555555|  &
23   |ffffffffffffeeee eeeeeeeedddddddd ddddcccccccccccc bbbbbbbbbbbbaaaa| S4
     +----------------+----------------+----------------+----------------+
24   |5555444444444444 3333333333332222 2222222211111111 1111000000000000| S5
25   |aaaaaaa999999999 9999888888888888 7777777777776666 6666666655555555|  &
26   |ffffffffffffeeee eeeeeeeedddddddd ddddcccccccccccc bbbbbbbbbbbbaaaa| S6
     +----------------+----------------+----------------+----------------+
27   |5555444444444444 3333333333332222 2222222211111111 1111000000000000| S7
28   |aaaaaaa999999999 9999888888888888 7777777777776666 6666666655555555|  &
29   |ffffffffffffeeee eeeeeeeedddddddd ddddcccccccccccc bbbbbbbbbbbbaaaa| S8
     +----------------+----------------+----------------+----------------+

*/


#include <cinttypes>


namespace pdd    {
namespace record {


/* ====================================================================== */
/* BEGIN: WibColdData                                                     */
/* ---------------------------------------------------------------------- *//*!

  \brief Laysout a cold data stream
                                                                          */
/* ---------------------------------------------------------------------- */
class WibColdData
{
   WibColdData () = delete;

   /* ------------------------------------------------------------------- *//*!
    *
    * \brief Describes the cold data streams from the front-ends.
    *
    * \par
    *  There are two cold data streams. Each stream is composed of two
    *  separate links. Each link has the same basic structure,
    *     -#  2 x 8-bit error words
    *     -#  1 x 8-bit reserved word
    *     -#  8 x 8-bit header words
    *     -# 64 x 12-bit ADCs densely packed.
    *
    *  This class is not quite pure. The first 16-bit word is provided
    *  by the WIB and contains information about any errors encountered
    *  when reading the 2 links.  Unfortunately, the nomenclature used
    *  to identify the links differs between the COLDDATA and WIB. The
    *  COLDDATA identifies the 2 links as A & B, whereas the WIB uses
    *  1 & 2.
    *
    *  Rather than adopting one or the other, the naming used here uses
    *  the names assigned by the originator. So A & B for COLDDATA and
    *  1 & 2 for the WIB
    *
   \* ------------------------------------------------------------------- */
public:
   /* ------------------------------------------------------------------- *//*!
      \brief The number of adcs in one ColdData stream                    */
   /* ------------------------------------------------------------------- */
   static const unsigned NAdcs = 64;


public:
   /* ------------------------------------------------------------------- *//*!
     \brief  Size, in bits, of the first ColdData word bit fields         */
   /* ------------------------------------------------------------------- */
   enum class Size0 : int
   {
                          /* -----------------------------------------    */
                          /* - Stream Errors - 1 16 bit field             */
                          /*      2 x 8 bit fields for Stream 1 & 2       */
                          /* -------------------------------------------- */
      StreamErr    = 16,  /*!< Size of the both Stream Error bit fields   */
      StreamErr1   =  8,  /*!< Size of the Stream Error 1 bit field       */
      StreamErr2   =  8,  /*!< Size of the Stream Error 2 bit field       */
                             /* ----------------------------------------- */


      Reserved0    =  0,  /*!< Old reserved field has been eliminated    */

                          /* -----------------------------------------    */
                          /* - CheckSums 32 bits,                         */
                          /*   2 x 16 bits for Lo & HI A|B checksum       */
                          /*   4 x 8 bits for Lo A,Lo B,Hi A and Hi B     */
                          /* -------------------------------------------- */
      CheckSums    = 32,  /*!< Size of the all the checksum fields        */
      CheckSumsLo  = 16,  /*!< Size of the low checksum fields            */
      CheckSumLoA  =  8,  /*!< Size of the low checksum field, stream A   */
      CheckSumLoB  =  8,  /*!< Size of the hi  checksum field, stream A   */
      CheckSumsHi  = 16,  /*!< Size of the hi  checksum fields            */
      CheckSumHiA  =  8,  /*!< Size of the hi  checksum field, stream A   */
      CheckSumHiB  =  8,  /*!< Size of the hi  checksum field, stream B   */
                          /* -------------------------------------------- */

      ConvertCount = 16   /*!< Size of the cold data convert count        */
   };


   /* ------------------------------------------------------------------- *//*!
     \brief  Right justified offsets to the first ColdData word bit fields*/
   /* ------------------------------------------------------------------- */
   enum class Offset0 : int
   {
                          /* -----------------------------------------    */
                          /* - Stream Errors - 1 8 bit field              */
                          /*   2 x 4 bit fields for Stream 1 & 2          */
                          /* -----------------------------------------    */
      StreamErr    =  0,  /*!< Offset to both Stream Error bit fields     */
      StreamErr1   =  0,  /*!< Offset to Stream Error 1 bit field         */
      StreamErr2   =  8,  /*!< Offset to Stream Error 2 bit field         */
                          /* -------------------------------------------- */

      Reserved0    =  0,  /*!< Offset to the reserved field --Eliminated  */

                          /* -------------------------------------------- */
                          /* - CheckSums 32 bits,                         */ 
                          /*   2 x 16 bits for Lo & HI A|B checksum       */
                          /*   4 x 8 bits for Lo A,Lo B,Hi A and Hi B     */
                         /* -------------------------------------------- */
      CheckSums    = 16,  /*!< Offset to all the checksum fields          */
      CheckSumsLo  = 16,  /*!< Offset to low checksum fields              */
      CheckSumLoA  = 16,  /*!< Offset to low checksum field, stream A     */
      CheckSumLoB  = 24,  /*!< Offset to hi  checksum field, stream B     */
      CheckSumsHi  = 32,  /*!< Offset to hi  checksum fields              */
      CheckSumHiA  = 32,  /*!< Offset to hi  checksum field, stream A     */
      CheckSumHiB  = 40,  /*!< Offset to hi  checksum field, stream B     */
                          /* -------------------------------------------- */

      ConvertCount = 48   /*!< Size of the cold data convert count        */
   };
   /* ------------------------------------------------------------------- */


   /* ------------------------------------------------------------------- *//*!
      \brief  Right justified bit mask for the ColdData word bit fields   */
   /* ------------------------------------------------------------------- */
   enum class Mask0 : uint32_t
   {
                             /* ----------------------------------------- */
                             /* - Stream Errors - 1 16 bit field          */
                             /*   2 x 8 bit fields for Stream 1 & 2       */
                             /* ----------------------------------------- */
      StreamErr    = 0xffff, /*!< Mask for  Stream Error bit fields       */
      StreamErr1   = 0xff,   /*!< Mask for  Stream Error 1 bit field      */
      StreamErr2   = 0xff,   /*!< Mask for  Stream Error 2 bit field      */
                             /* ----------------------------------------- */
      Reserved0    = 0x00,   /*!< Mask for the reserved field--Eliminated */


                             /* ----------------------------------------- */
                             /* - CheckSums 32 bits,                      */
                             /*   2 x 16 bits for Lo & HI A|B checksum    */
                             /*   4 x 8 bits for LoA,LoB,HiA and HiB      */
                             /* ----------------------------------------- */
      CheckSums    = 0xffffffff,
                            /*!< Mask for  all the checksum fields        */
      CheckSumsLo  = 0xffff,/*!< Mask for  low checksum fields            */
      CheckSumLoA  = 0xff,  /*!< Mask for  low checksum field, stream A   */
      CheckSumLoB  = 0xff,  /*!< Mask for  low checksum field, stream B   */
      CheckSumsHi  = 0xffff,/*!< Mask for  hi  checksum fields            */
      CheckSumHiA  = 0xff,  /*!< Mask for  hi  checksum field, stream A   */
      CheckSumHiB  = 0xff,  /*!< Mask for  hi  checksum field, stream B   */
                            /* ------------------------------------------ */

      ConvertCount = 0xffff /*!< Size of the cold data convert count      */
   };
   /* ------------------------------------------------------------------- */




   /* ------------------------------------------------------------------- *//*!
      \brief  Size, in bits, of the second ColdData word bit fields       */
   /* ------------------------------------------------------------------- */
      enum class Size1 : int
      {
         ErrRegister  = 16,  /*!< Size of the error register bit fields      */
         Reserved1    = 16,  /*!< Size of the reserved field                 */
         Hdrs         = 32,  /*!< Size of the all the checksum fields        */
         Hdr          =  4   /*!< Size of 1 header                           */
      };
      /* ------------------------------------------------------------------- */


      /* -----------------------------------------------p------------------- *//*!
         \brief  Right justified offsets to the second ColdData word bit
                 fields                                                      */
      /* ------------------------------------------------------------------- */
      enum class Offset1 : int
      {
         ErrRegister  =  0,  /*!< Offset to the error register bit fields    */
         Reserved1    = 16,  /*!< Offset to the reserved field               */
         Hdrs         = 32   /*!< Offset to the all the checksum fields      */
      };
      /* ------------------------------------------------------------------- */


      /* ------------------------------------------------------------------- *//*!
         \brief  Right justified bit mask for the second ColdData word bit 
                 fields                                                      */
      /* ------------------------------------------------------------------- */
      enum class Mask1 : uint32_t
      {
         ErrRegister  = 0xff,      /*!< Mask for error register bit field    */
         Reserved1    = 0xff,      /*!< Mask for the reserved field          */
         Hdrs         = 0xffffffff,/*!< Mask for the array header fields     */
         Hdr          = 0xf        /*!< Mask for a single header             */
      };
      /* ------------------------------------------------------------------- */

      
protected:
   uint64_t      m_w0;  /*!< ColdData header word 0                       */
   uint64_t      m_w1;  /*!< ColdData header word 1                       */
   uint64_t m_adcs[12]; /*!< The 64 x 12-bit adcs                         */
   };
/* ---------------------------------------------------------------------- */
/*   END: class WibColdData                                               */
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!
 *
 *  \brief This is the data as it appears coming into the data handling
 *         module.  
 *
 * \par
 * This defines the layout only.
 *
\* ---------------------------------------------------------------------- */
class WibFrame 
{
public:
   static const unsigned int VersionNumber = 1;
   static const unsigned int     NColdData = 2;


protected:
   /* ------------------------------------------------------------------- *//*!
      \brief  8b/10b comma characters                                     */
   /* ------------------------------------------------------------------- */
   enum class K28 : uint8_t
   {
      K28_1 = 0x3c,
      K28_2 = 0x5C,
      K28_5 = 0xBC
   };
   /* ------------------------------------------------------------------- */


   /* ------------------------------------------------------------------- *//*!
     \brief Sizes of the bits fields in Word 0                            */
   /* ------------------------------------------------------------------- */
   enum class Size0 : int
   {
      CommaChar =  8,   /*!< Size of the 8b/10b comma character           */
      Version   =  5,   /*!< Size of the version                          */

                        /* ---------------------------------------------- */
                        /*-- The WIB Identifier, 11 bits, 3 subfields --- */
      Id        = 11,   /*!< Size of the WIB identifier field             */
      Fiber     =  3,   /*!< Size of Fiber # WIB identifier subfield      */
      Crate     =  5,   /*!< Size of Crate # WIB identifier subfield      */
      Slot      =  3,   /*!< Size of Slot  # WIB identifier subfield      */
                        /* ---------------------------------------------- */

      Reserved  = 24,   /*!< Size of the reserved word field              */
      WibErrors = 16    /*!< Size of the WIB error field                  */
   };
   /* ------------------------------------------------------------------- */


   /* ------------------------------------------------------------------- *//*!
     \brief Right justified offset of the bits fields in Word 0           */
   /* ------------------------------------------------------------------- */
   enum class Offset0 : int
   {
      CommaChar =  0,   /*!< Offset to the 8b/10b comma character         */
      Version   =  8,   /*!< Offset to the version                        */

                        /* ---------------------------------------------- */
                        /*-- The WIB Identifier, 11 bits, 3 subfields --- */
      Id        = 13,   /*!< Offset to the WIB identifier field           */
      Fiber     = 13,   /*!< Offset to the Fiber # WIB identifier subfield*/
      Crate     = 16,   /*!< Offset to the Crate # WIB identifier subfield*/
      Slot      = 21,   /*!< Offset to the Slot  # WIB identifier subfield*/
                        /* ---------------------------------------------- */

      Reserved  = 24,   /*!< Size of the reserved word field              */
      WibErrors = 48    /*!< Size of the WIB error field                  */
   };
   /* ------------------------------------------------------------------- */



   /* ------------------------------------------------------------------- *//*!
     \brief Right justified offset to the bit fields within an id         */
   /* ------------------------------------------------------------------- */
   enum class OffsetId : int
   {
      Fiber     = 0,    /*!< Offset to the Fiber # within an id           */
      Crate     = 5,    /*!< Offset to the Crate # within an id           */
      Slot      = 8     /*!< Offset to the Crate # within an id           */
   };
   /* ------------------------------------------------------------------- */


   /* ------------------------------------------------------------------- *//*!
     \brief Right justified masks of the bits fields in Word 0            */
   /* ------------------------------------------------------------------- */
   enum class Mask0 : uint32_t
   {
      CommaChar =  0xff, /*!< Offset to the 8b/10b comma character        */
      Version   =  0x1f, /*!< Offset to the version                       */

                          /* -------------------------------------------- */
                          /*-- The WIB Identifier, 11 bits, 3 subfields - */
      Id        = 0x7ff, /*!< Mask for the WIB identifier field           */
      Fiber     =   0x7, /*!< Mask for the Fiber # WIB identifier subfield*/
      Crate     =  0x1f, /*!< Mask for the Crate # WIB identifier subfield*/
      Slot      =   0x7, /*!< Mask for the Slot  # WIB identifier subfield*/
                         /* --------------------------------------------- */

      Reserved  = 0xffffff, /*!< Mask for the reserved word field         */
      WibErrors = 0xffff    /*!< Mask for the WIB error field             */
   };
   /* ------------------------------------------------------------------- */

public:
   uint64_t               m_header; /*!< W16  0 -  3, the WIB header word */
   uint64_t            m_timestamp; /*!< W16  4 -  7, the timestamp       */
   WibColdData  m_coldData[NColdData]; /*!< WIB 2 cold data streams       */
};
/* ---------------------------------------------------------------------- */
/*   END: WibFrame                                                        */
} /* END: namespace access                                                   */
} /* END: namespace pdd                                                      */
/* =====================================================================---= */


#endif

