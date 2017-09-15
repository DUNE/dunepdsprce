// -*-Mode: C++;-*-

#ifndef PDD_WIB_FRAME_HH
#define PDD_WIB_FRAME_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     WibFrame.hh
 *  @brief    Core file for the DUNE compresssion
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
 *  2017.08.16
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
      2     CvtCnt[16] | ChkSums_hi[16] | ChkSums_lo[16] | Rsvd[8] | Err2_1[8]
      3              Hdrs[32]           |       Rsvd[16] |    ErrReg[16]
      4-15  ColdData.Adcs

            ColdData Stream 2
      16    CvtCnt[16] | ChkSums_hi[16] | ChkSums_lo[16] | Rsvd[8] | Err2_1[8]
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
     |                            Channels 0 - 127                       |
     +----------------+----------------+----------------+----------------+
 2   |          Timestamp              |   CheckSums    |  Rsvd   SErr   |
 3   |           Hdrs[7-0]             |    Reserved    |  Error Register|
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
     |                            Channels 128 - 255                     |
     +----------------+----------------+----------------+----------------+
16   |          Timestamp              |    CheckSums   |  Rsvd   SErr   |
17   |           Hdrs[7-0]             |    Reserved    |  Error Register|
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


#include "dam/BfExtract.hh"
#include <cinttypes>


namespace pdd      {
namespace fragment {


/* ---------------------------------------------------------------------- *//*!
 *
 *  \brief This is the data as it appears coming into the data handling
 *         module.  
 *
 * \par
 * This defines the layout only and methods to access the various fields.
 * It is meant to be as lean as possible. Instantiation is generally by
 * only be reference. Assume \e ptr is a \e uint64_t pointer, then this
 * the code
 *
 * \code
 *    WibFrame const &wibFrame = WibFrame::Assign (ptr);
 * \endcode
 *
\* ---------------------------------------------------------------------- */
class WibFrame 
{
public:
   static const unsigned int VersionNumber = 1;
   static const unsigned int     NColdData = 2;

   class ColdData;

public:
   /*
    | Initialize a reference to a WibFrame from a bare pointer that is
    | assumed to contain the 30 64-bit words compromising a WibFrame.
   */
   WibFrame const &assign (uint64_t const *ptr);

   unsigned int      getNAdcs     () const;
   unsigned int      getCommaChar () const;
   unsigned int      getVersion   () const;
   unsigned int      getId        () const;
   unsigned int      getFiber     () const;
   unsigned int      getCrate     () const;
   unsigned int      getSlot      () const;

   unsigned int      getReserved  () const;
   unsigned int      getWibErrors () const;

   uint64_t          getHeader    () const;
   uint64_t          getTimestamp () const;
   ColdData const (& getColdData  () const)[2];
   ColdData       (& getColdData  ()      )[2];

   void              expandAdcs128x1 (uint16_t *dst) const;

   
   static unsigned int getCommaChar (uint64_t     header);
   static unsigned int getVersion   (uint64_t     header);
   static unsigned int getId        (uint64_t     header);
   static unsigned int getFiber     (uint64_t     header);
   static unsigned int getCrate     (uint64_t     header);
   static unsigned int getSlot      (uint64_t     header);

   static unsigned int getIdFiber   (unsigned int header);
   static unsigned int getIdCrate   (unsigned int header);
   static unsigned int getIdSlot    (unsigned int header);

   static unsigned int getReserved  (uint64_t     header);
   static unsigned int getWibErrors (uint64_t     header);

   
   // Get all 128 channels for 1 frame */
   static void expandAdcs128x1      (uint16_t             *dst,
                                     uint64_t const (&src)[12]);

   // Get all 128 channels for N frames */
   static void expandAdcs128xN      (uint16_t             *dst,
                                     WibFrame const    *frames,
                                     int               nframes);

   // ----------------------------------------------------------
   // Transposers: Contigious memory 
   //-------------------------------

   // Transpose 128 adcs x 8 time samples
   static void transposeAdcs128x8   (uint16_t             *dst,
                                     int            ndstStride,
                                     WibFrame const     *frame);

   // Transpose 128 adcs x 16*N time samples
   static void transposeAdcs128x16N (uint16_t             *dst,
                                     int            ndstStride,
                                     WibFrame const    *frames,
                                     int               nframes);

   // Transpose 128 adcs x 32*N time samples
   static void transposeAdcs128x32N (uint16_t             *dst,
                                     int            ndstStride,
                                     WibFrame const    *frames,
                                     int               nframes);
   // ----------------------------------------------------------


   // ----------------------------------------------------------
   // Transposers: Channel by Channel memory 
   //---------------------------------------

   // Transpose 128 adcs x 8 time samples
   static void transposeAdcs128x8   (uint16_t        *dst[128],
                                     int            ndstStride,
                                     WibFrame const     *frame);

   // Transpose 128 adcs x 16*N time samples
   static void transposeAdcs128x16N (uint16_t        *dst[128],
                                     int            ndstStride,
                                     WibFrame const    *frames,
                                     int               nframes);

   // Transpose 128 adcs x 32*N time samples
   static void transposeAdcs128x32N (uint16_t        *dst[128],
                                     int            ndstStride,
                                     WibFrame const    *frames,
                                     int               nframes);



public:
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
   struct ColdData
   {
   public:
      /* ---------------------------------------------------------------- *//*!
         \brief The number of adcs in one ColdData stream                 */
      /* ---------------------------------------------------------------- */
      static const unsigned NAdcs = 64;


   public:
      /* --------------------------------------------------------------- *\
       |  This first set of methods access directly from the data        |
       |  contained in the class. They are classic C++ getters.          |
      \* --------------------------------------------------------------- */
      uint64_t     getHeader0      () const;
      uint64_t     getHeader1      () const;

      unsigned int getStreamErrs   () const;
      unsigned int getStreamErr1   () const;
      unsigned int getStreamErr2   () const;

      unsigned int getReserved0    () const;
      uint32_t     getCheckSums    () const;
      unsigned int getCheckSumA    () const;
      unsigned int getCheckSumB    () const;

      unsigned int getConvertCount () const;
      unsigned int getErrRegister  () const;
      unsigned int getReserved1    () const;
      uint32_t     getHdrs         () const;
      unsigned int getHdr   (int idx) const;


      // Locate the packed 12-bit adcs data
      uint64_t const    (&locateAdcs12b () const)[12];
      uint64_t          (&locateAdcs12b ())[12];


      // Expand on ColdData stream of 12-bit adcs -> 16-bits
      void        expandAdcs64x1 (uint16_t *dst) const;


      /* ------------------------------------------------------- *\
       |  Lower level routines likely seldom used, provided for  |
       |  mainly for completeness.                               |         
      \*  ------------------------------------------------------ */
      unsigned int getCheckSumsLo  () const;
      unsigned int getCheckSumLoA  () const;
      unsigned int getCheckSumLoB  () const;

      unsigned int getCheckSumsHi  () const;
      unsigned int getCheckSumHiA  () const;
      unsigned int getCheckSumHiB  () const;


      /* --------------------------------------------------------------- *\
       |                                                                 |
       |  This second set of methods access fields from the relevant     |
       |  64-bit word.  As such, they are more low-level and efficient.  |
       |  The price is that the user has to know a little more about the |
       |  class.                                                         |
       |                                                                 |
       |  The user should pick the style to be appropriate with the task |
       |  at hand.  For example, simple printing of the fields should    |
       |  chose the first, class access methods.  More performance       |
       |  demanding applications may be more inclined to use. In rare    |
       |  instances, one may have only the relevant 64-bit word, for     |
       |  example in situation where one is artificially composing data  |
       |  for testing purposes.                                          |
       |                                                                 |
       |  The argument is meant to be suggestive, \i.e.                  |
       |      - \a header0 -> From getHeader0 () method                  |
       |      - \a header1 -> From getHeader1 () method                  |
       |                                                                 |
      \* --------------------------------------------------------------- */

      static unsigned int getStreamErrs   (uint64_t header0);
      static unsigned int getStreamErr1   (uint64_t header0);
      static unsigned int getStreamErr2   (uint64_t header0);

      static unsigned int getReserved0    (uint64_t header0);
      static uint32_t     getCheckSums    (uint64_t header0);
      static unsigned int getCheckSumsLo  (uint64_t header0);
      static unsigned int getCheckSumLoA  (uint64_t header0);
      static unsigned int getCheckSumLoB  (uint64_t header0);
      static unsigned int getCheckSumsHi  (uint64_t header0);
      static unsigned int getCheckSumHiA  (uint64_t header0);
      static unsigned int getCheckSumHiB  (uint64_t header0);
      static unsigned int getConvertCount (uint64_t header0);

      static unsigned int getCheckSumA    (uint64_t header0);
      static unsigned int getCheckSumB    (uint64_t header0);

      static unsigned int getErrRegister  (uint64_t header1);
      static unsigned int getReserved1    (uint64_t header1);
      static uint32_t     getHdrs         (uint64_t header1);
      static unsigned int getHdr          (uint64_t header1, int idx);
      static unsigned int getHdr          (uint32_t header1, int idx);

      // Expand on the specified stream of 12-bit adcs -> 16-bits
      static void  expandAdcs64x1     (uint16_t            *dst,
                                       uint64_t const (&src)[12]);


   private:
      /* ---------------------------------------------------------------- *//*!
         \brief  Size, in bits, of the first ColdData word bit fields     */
      /* ---------------------------------------------------------------- */
      enum class Size0 : int
      {
                             /* ----------------------------------------- */
                             /* - Stream Errors - 1 8 bit field           */
                             /*   2 x 4 bit fields for Stream 1 & 2       */
                             /* ----------------------------------------- */
         StreamErr    =  8,  /*!< Size of the both Stream Error bit fields*/
         StreamErr1   =  4,  /*!< Size of the Stream Error 1 bit field    */
         StreamErr2   =  4,  /*!< Size of the Stream Error 2 bit field    */
                             /* ----------------------------------------- */

         Reserved0    =  8,  /*!< Size of the reserved field              */

                             /* ----------------------------------------- */
                             /* - CheckSums 32 bits,                      */
                             /*   2 x 16 bits for Lo & HI A|B checksum    */
                             /*   4 x 8 bits for Lo A,Lo B,Hi A and Hi B  */
                             /* ----------------------------------------- */
         CheckSums    = 32,  /*!< Size of the all the checksum fields     */
         CheckSumsLo  = 16,  /*!< Size of the low checksum fields         */
         CheckSumLoA  =  8,  /*!< Size of the low checksum field, stream A*/
         CheckSumLoB  =  8,  /*!< Size of the hi  checksum field, stream A*/
         CheckSumsHi  = 16,  /*!< Size of the hi  checksum fields         */
         CheckSumHiA  =  8,  /*!< Size of the hi  checksum field, stream A*/
         CheckSumHiB  =  8,  /*!< Size of the hi  checksum field, stream B*/
                             /* ----------------------------------------- */

         ConvertCount = 16   /*!< Size of the cold data convert count     */
      };


      /* ---------------------------------------------------------------- *//*!
         \brief  Right justified offsets to the first ColdData word bit
                 fields                                                   */
      /* ---------------------------------------------------------------- */
      enum class Offset0 : int
      {
                             /* ----------------------------------------- */
                             /* - Stream Errors - 1 8 bit field           */
                             /*   2 x 4 bit fields for Stream 1 & 2       */
                             /* ----------------------------------------- */
         StreamErr    =  0,  /*!< Offset to both Stream Error bit fields  */
         StreamErr1   =  0,  /*!< Offset to Stream Error 1 bit field      */
         StreamErr2   =  4,  /*!< Offset to Stream Error 2 bit field      */
                             /* ----------------------------------------- */

         Reserved0    =  8,  /*!< Offset to the reserved field            */


                             /* ----------------------------------------- */
                             /* - CheckSums 32 bits,                      */ 
                             /*   2 x 16 bits for Lo & HI A|B checksum    */
                             /*   4 x 8 bits for Lo A,Lo B,Hi A and Hi B  */
                             /* ----------------------------------------- */
         CheckSums    = 16,  /*!< Offset to all the checksum fields       */
         CheckSumsLo  = 16,  /*!< Offset to low checksum fields           */
         CheckSumLoA  = 16,  /*!< Offset to low checksum field, stream A  */
         CheckSumLoB  = 24,  /*!< Offset to hi  checksum field, stream A  */
         CheckSumsHi  = 32,  /*!< Offset to hi  checksum fields           */
         CheckSumHiA  = 32,  /*!< Offset to hi  checksum field, stream A  */
         CheckSumHiB  = 40,  /*!< Offset to hi  checksum field, stream B  */
                             /* ----------------------------------------- */

         ConvertCount = 48   /*!< Size of the cold data convert count     */
      };
      /* ---------------------------------------------------------------- */


      /* ---------------------------------------------------------------- *//*!
         \brief  Right justified bit mask for the ColdData word bit fields*/
      /* ---------------------------------------------------------------- */
      enum class Mask0 : uint32_t
      {
                                /* -------------------------------------- */
                                /* - Stream Errors - 1 8 bit field        */
                                /*   2 x 4 bit fields for Stream 1 & 2    */
                                /* -------------------------------------- */
         StreamErr    = 0xff,   /*!< Offset to Stream Error bit fields    */
         StreamErr1   = 0x0f,   /*!< Offset to Stream Error 1 bit field   */
         StreamErr2   = 0x0f,   /*!< Offset to Stream Error 2 bit field   */
                                /* -------------------------------------- */

         Reserved0    = 0xff,   /*!< Offset to the reserved field         */


                                /* -------------------------------------- */
                                /* - CheckSums 32 bits,                   */
                                /*   2 x 16 bits for Lo & HI A|B checksum */
                                /*   4 x 8 bits for LoA,LoB,HiA and HiB   */
                                /* -------------------------------------- */
         CheckSums    = 0xffffffff,
                               /*!< Offset to all the checksum fields     */
         CheckSumsLo  = 0xffff,/*!< Offset to low checksum fields         */
         CheckSumLoA  = 0xff,  /*!< Offset to low checksum field, stream A*/
         CheckSumLoB  = 0xff,  /*!< Offset to hi  checksum field, stream A*/
         CheckSumsHi  = 0xffff,/*!< Offset to hi  checksum fields         */
         CheckSumHiA  = 0xff,  /*!< Offset to hi  checksum field, stream A*/
         CheckSumHiB  = 0xff,  /*!< Offset to hi  checksum field, stream B*/
                               /* --------------------------------------- */

         ConvertCount = 0xffff /*!< Size of the cold data convert count   */
      };
      /* ---------------------------------------------------------------- */




      /* ---------------------------------------------------------------- *//*!
         \brief  Size, in bits, of the second ColdData word bit fields    */
      /* ---------------------------------------------------------------- */
      enum class Size1 : int
      {
         ErrRegister  = 16,  /*!< Size of the error register bit fields   */
         Reserved1    = 16,  /*!< Size of the reserved field              */
         Hdrs         = 32,  /*!< Size of the all the checksum fields     */
         Hdr          =  4   /*!< Size of 1 header                        */
      };
      /* ---------------------------------------------------------------- */


      /* ---------------------------------------------------------------- *//*!
         \brief  Right justified offsets to the second ColdData word bit
                 fields                                                   */
      /* ---------------------------------------------------------------- */
      enum class Offset1 : int
      {
         ErrRegister  =  0,  /*!< Offset to the error register bit fields */
         Reserved1    = 16,  /*!< Offset to the reserved field            */
         Hdrs         = 32   /*!< Offset to the all the checksum fields   */
      };
      /* ---------------------------------------------------------------- */


      /* ---------------------------------------------------------------- *//*!
         \brief  Right justified bit mask for the second ColdData word bit 
                 fields                                                   */
      /* ---------------------------------------------------------------- */
      enum class Mask1 : uint32_t
      {
         ErrRegister  = 0xff,      /*!< Mask for error register bit field */
         Reserved1    = 0xff,      /*!< Mask for the reserved field       */
         Hdrs         = 0xffffffff,/*!< Mask for the array header fields  */
         Hdr          = 0xf        /*!< Mask for a single header          */
      };
      /* ---------------------------------------------------------------- */

      
   private:
      uint64_t      m_w0;  /*!< ColdData header word 0                    */
      uint64_t      m_w1;  /*!< ColdData header word 1                    */
      uint64_t m_adcs[12]; /*!< The 64 x 12-bit adcs                      */
   };
   /* ------------------------------------------------------------------- */
   /* END: class ColdData                                                 */
   /* ------------------------------------------------------------------- */


private:
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
      Crate     = 18,   /*!< Offset to the Crate # WIB identifier subfield*/
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
      Id        = 0x3ff, /*!< Mask for the WIB identifier field           */
      Fiber     =   0x7, /*!< Mask for the Fiber # WIB identifier subfield*/
      Crate     =  0x1f, /*!< Mask for the Crate # WIB identifier subfield*/
      Slot      =   0x7, /*!< Mask for the Slot  # WIB identifier subfield*/
                         /* --------------------------------------------- */

      Reserved  = 0xffffff, /*!< Mask for the reserved word field         */
      WibErrors = 0xffff    /*!< Mask for the WIB error field             */
   };
   /* ------------------------------------------------------------------- */

private:
   uint64_t               m_header; /*!< W16  0 -  3, the WIB header word */
   uint64_t            m_timestamp; /*!< W16  4 -  7, the timestamp       */
   ColdData  m_coldData[NColdData]; /*!< WIB 2 cold data streams          */
};
/* ---------------------------------------------------------------------- */
} /* END: namespace fragment                                              */
} /* END: namespace pdd                                                   */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION: WibFrame                                               */
/* ---------------------------------------------------------------------- */
namespace pdd      {
namespace fragment {

inline WibFrame const &WibFrame::assign (uint64_t const *ptr)
{
   return reinterpret_cast<WibFrame const &>(*ptr);
}

inline unsigned int    WibFrame::getNAdcs     () const { return NColdData * ColdData::NAdcs; }
inline uint64_t        WibFrame::getHeader    () const { return m_header;    }
inline uint64_t        WibFrame::getTimestamp () const { return m_timestamp; }

inline unsigned int    WibFrame::getCommaChar () const { return getCommaChar (m_header); }
inline unsigned int    WibFrame::getVersion   () const { return getVersion   (m_header); }
inline unsigned int    WibFrame::getId        () const { return getId        (m_header); }
inline unsigned int    WibFrame::getFiber     () const { return getFiber     (m_header); }
inline unsigned int    WibFrame::getCrate     () const { return getFiber     (m_header); }
inline unsigned int    WibFrame::getSlot      () const { return getSlot      (m_header); }


inline unsigned int    WibFrame::getReserved  () const { return getReserved  (m_header); }
inline unsigned int    WibFrame::getWibErrors () const { return getWibErrors (m_header); }


inline unsigned int WibFrame::getReserved (uint64_t header)
{
   return PDD_EXTRACT64 (header, Mask0::Reserved, Offset0::Reserved);
}

inline unsigned int WibFrame::getWibErrors (uint64_t header)
{
   return PDD_EXTRACT64 (header, Mask0::WibErrors, Offset0::WibErrors);
}

inline WibFrame::ColdData const (& WibFrame::getColdData () const)[WibFrame::NColdData]
{
   return m_coldData;
}

inline WibFrame::ColdData (& WibFrame::getColdData ())[WibFrame::NColdData]
{
   return m_coldData;
}


inline unsigned int WibFrame::getCommaChar (uint64_t header)
{
   return PDD_EXTRACT64 (header, Mask0::CommaChar, Offset0::CommaChar);
}


inline unsigned int WibFrame::getVersion (uint64_t header)
{
   return PDD_EXTRACT64 (header, Mask0::Version, Offset0::Version);
}


inline unsigned int WibFrame::getId (uint64_t header)
{
   return PDD_EXTRACT64 (header, Mask0::Id, Offset0::Id);
}


inline unsigned int WibFrame::getFiber (uint64_t header)
{
   return PDD_EXTRACT64 (header, Mask0::Fiber, Offset0::Fiber);
}


inline unsigned int WibFrame::getCrate (uint64_t header)
{
   return PDD_EXTRACT64 (header, Mask0::Crate, Offset0::Crate);
}

inline unsigned int WibFrame::getSlot (uint64_t header)
{
   return PDD_EXTRACT64 (header, Mask0::Slot, Offset0::Slot);
}


inline unsigned int WibFrame::getIdFiber (unsigned int id)
{
   return PDD_EXTRACT32 (id, Mask0::Fiber, OffsetId::Fiber);
}

inline unsigned int WibFrame::getIdCrate (unsigned int id)
{
   return PDD_EXTRACT32 (id, Mask0::Crate, OffsetId::Crate);
}


inline unsigned int WibFrame::getIdSlot (unsigned int id)
{
   return PDD_EXTRACT32 (id, Mask0::Slot, OffsetId::Slot);
}


inline uint64_t     WibFrame::ColdData::getHeader0      () const { return m_w0; }
inline uint64_t     WibFrame::ColdData::getHeader1      () const { return m_w1; }

inline unsigned int WibFrame::ColdData::getStreamErrs   () const { return getStreamErrs   (m_w0); }
inline unsigned int WibFrame::ColdData::getStreamErr1   () const { return getStreamErr1   (m_w0); }
inline unsigned int WibFrame::ColdData::getStreamErr2   () const { return getStreamErr2   (m_w0); }
inline unsigned int WibFrame::ColdData::getReserved0    () const { return getReserved0    (m_w0); }
inline uint32_t     WibFrame::ColdData::getCheckSums    () const { return getCheckSums    (m_w0); }
inline unsigned int WibFrame::ColdData::getCheckSumsLo  () const { return getCheckSumsLo  (m_w0); }
inline unsigned int WibFrame::ColdData::getCheckSumLoA  () const { return getCheckSumLoA  (m_w0); }
inline unsigned int WibFrame::ColdData::getCheckSumLoB  () const { return getCheckSumLoB  (m_w0); }
inline unsigned int WibFrame::ColdData::getCheckSumsHi  () const { return getCheckSumsHi  (m_w0); }
inline unsigned int WibFrame::ColdData::getCheckSumHiA  () const { return getCheckSumHiA  (m_w0); }
inline unsigned int WibFrame::ColdData::getCheckSumHiB  () const { return getCheckSumHiB  (m_w0); }
inline unsigned int WibFrame::ColdData::getConvertCount () const { return getConvertCount (m_w0); }

inline unsigned int WibFrame::ColdData::getCheckSumA    () const  { return getCheckSumA   (m_w0); }
inline unsigned int WibFrame::ColdData::getCheckSumB    () const  { return getCheckSumB   (m_w0); }

inline void         WibFrame::ColdData::expandAdcs64x1 (uint16_t *dst) const
{
   uint64_t const (&src)[12] = locateAdcs12b ();
   expandAdcs64x1 (dst, src);
   return;
}
   

inline unsigned int WibFrame::ColdData::getStreamErrs   (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::StreamErr, Offset0::StreamErr);
}

inline unsigned int WibFrame::ColdData::getStreamErr1   (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::StreamErr1, Offset0::StreamErr1);
}

inline unsigned int WibFrame::ColdData::getStreamErr2   (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::StreamErr2, Offset0::StreamErr2);
}

inline unsigned int WibFrame::ColdData::getReserved0    (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::Reserved0, Offset0::Reserved0);
}

inline uint32_t     WibFrame::ColdData::getCheckSums    (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::CheckSums, Offset0::CheckSums);
}

inline unsigned int WibFrame::ColdData::getCheckSumsLo  (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::CheckSumsLo, Offset0::CheckSumsLo);
}

inline unsigned int WibFrame::ColdData::getCheckSumLoA  (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::CheckSumLoA, Offset0::CheckSumLoA);
}


inline unsigned int WibFrame::ColdData::getCheckSumLoB  (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::CheckSumLoB, Offset0::CheckSumLoB);
}

inline unsigned int WibFrame::ColdData::getCheckSumsHi  (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::CheckSumsHi, Offset0::CheckSumsHi);
}

inline unsigned int WibFrame::ColdData::getCheckSumHiA  (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::CheckSumHiA, Offset0::CheckSumHiA);
}

inline unsigned int WibFrame::ColdData::getCheckSumHiB  (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::CheckSumHiB, Offset0::CheckSumHiB);
}

inline unsigned int WibFrame::ColdData::getConvertCount (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::ConvertCount, Offset0::ConvertCount);
}

inline unsigned int WibFrame::ColdData::getCheckSumA   (uint64_t w0)
{
   return (getCheckSumHiA (w0) << static_cast<int>(Size0::CheckSumLoA))
         | getCheckSumLoA (w0);
}

inline unsigned int WibFrame::ColdData::getCheckSumB   (uint64_t w0)
{
   return (getCheckSumHiB (w0) << static_cast<int>(Size0::CheckSumLoB))
         | getCheckSumLoB (w0);
}

inline uint64_t const (& WibFrame::ColdData::locateAdcs12b () const)[12]
{
   return m_adcs;
}

inline uint64_t (& WibFrame::ColdData::locateAdcs12b ())[12]
{
   return m_adcs;
}


inline unsigned int WibFrame::ColdData::getErrRegister   () const 
{ 
   return getErrRegister (m_w1);
}

inline unsigned int WibFrame::ColdData::getReserved1     () const 
{ 
   return getReserved1   (m_w1); 
}

inline unsigned int WibFrame::ColdData::getHdrs          () const 
{ 
   return getHdrs        (m_w1); 
}

inline unsigned int WibFrame::ColdData::getHdr    (int idx) const
 {
    return getHdr         (m_w1, idx); 
}

inline unsigned int WibFrame::ColdData::getErrRegister  (uint64_t w1)
{
   return PDD_EXTRACT64 (w1, Mask1::ErrRegister, Offset1::ErrRegister);
}


inline unsigned int WibFrame::ColdData::getReserved1    (uint64_t w1)
{
   return PDD_EXTRACT64 (w1, Mask1::Reserved1, Offset1::Reserved1);
}

inline uint32_t WibFrame::ColdData::getHdrs          (uint64_t w1)
{
   return PDD_EXTRACT64 (w1, Mask1::Hdrs, Offset1::Hdrs);
}

inline uint32_t WibFrame::ColdData::getHdr (uint64_t w1, int idx)
{
   auto   hdrs = getHdrs (w1);
   return getHdr (hdrs, idx);
}

inline uint32_t WibFrame::ColdData::getHdr (uint32_t hdrs, int idx)
{
   uint8_t hdr = (hdrs >> static_cast<int>(Size1::Hdr) * idx)
                       & static_cast<uint32_t>(Mask1::Hdr);
   return hdr;
}
/* ---------------------------------------------------------------------- */
/*   END: IMPLEMENTATION: class WibFrame                                  */
} /* END: namespace fragment                                              */
} /* END: namespace pdd                                                   */
/* ====================================================================== */


#endif
   
