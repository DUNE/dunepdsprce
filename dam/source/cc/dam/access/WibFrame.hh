// -*-Mode: C++;-*-

#ifndef ACCESS_WIB_FRAME_HH
#define ACCESS_WIB_FRAME_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     access/WibFrame.hh
 *  @brief    Access methods for a WibFrame
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



#include "dam/records/WibFrame.hh"
#include "dam/util/BfExtract.hh"
#include <cinttypes>


namespace pdd    {
namespace access {


class WibColdData : public pdd::record::WibColdData
{
public:
//   WibColdData (pdd::record::WibColdData const *m_colddata);

public:
   /* ------------------------------------------------------------------ *\
    |  This first set of methods access directly from the data           |
    |  contained in the class. They are classic C++ getters.             |
   \* ------------------------------------------------------------------ */
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
   void        expandAdcs64x1 (int16_t        *dst) const;
   void        expandAdcs64x1 (int16_t *const *dst) const;


   /* ---------------------------------------------------------- *\
    |  Lower level routines likely seldom used, provided for     |
    |  mainly for completeness.                                  |         
   \*  --------------------------------------------------------- */
   unsigned int getCheckSumsLo  () const;
   unsigned int getCheckSumLoA  () const;
   unsigned int getCheckSumLoB  () const;
   
   unsigned int getCheckSumsHi  () const;
   unsigned int getCheckSumHiA  () const;
   unsigned int getCheckSumHiB  () const;


   /* ----------------------------------------------------------------- *\
    |                                                                   |
    |  This second set of methods access fields from the relevant       |
    |  64-bit word.  As such, they are more low-level and efficient.    |
    |  The price is that the user has to know a little more about the   |
    |  class.                                                           |
    |                                                                   |
    |  The user should pick the style to be appropriate with the task   |
    |  at hand.  For example, simple printing of the fields should      |
    |  chose the first, class access methods.  More performance         |
    |  demanding applications may be more inclined to use. In rare      |
    |  instances, one may have only the relevant 64-bit word, for       |
    |  example in situation where one is artificially composing data    |
    |  for testing purposes.                                            |
    |                                                                   |
    |  The argument is meant to be suggestive, \i.e.                    |
    |      - \a header0 -> From getHeader0 () method                    |
    |      - \a header1 -> From getHeader1 () method                    |
    |                                                                   |
    \* ---------------------------------------------------------------- */

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
   static void  expandAdcs64x1     (int16_t               *dst,
                                    uint64_t  const (&src)[12]);

public:
//   pdd::record::WibColdData const *m_colddata;
};
/* ---------------------------------------------------------------------- */




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
class WibFrame : public pdd::record::WibFrame
{
public:
   ///WibFrame (pdd::record::WibFrame const *frame);

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
   WibColdData const (& getColdData  () const)[2];
   WibColdData       (& getColdData  ()      )[2];

   void              expandAdcs128x1 (int16_t *dst) const;

   
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
   static void expandAdcs128x1      (int16_t              *dst,
                                     uint64_t const (&src)[12]);

   // Get all 128 channels for N frames */
   static void expandAdcs128xN      (int16_t              *dst,
                                     WibFrame const    *frames,
                                     int               nframes);

   // ----------------------------------------------------------
   // Transposers: Contigious memory 
   //-------------------------------

   // Transpose any arbitrary number of frames using the mose
   // optimal technique
   static void transposeAdcs128xN   (int16_t              *dst,
                                     int            ndstStride,
                                     WibFrame const    *frames,
                                     int               nframes);

   // Transpose 128 adcs x 8 time samples
   static void transposeAdcs128x8N  (int16_t              *dst,
                                     int            ndstStride,
                                     WibFrame const    *frames,
                                     int               nframes);

   // Transpose 128 adcs x 16*N time samples
   static void transposeAdcs128x16N (int16_t              *dst,
                                     int            ndstStride,
                                     WibFrame const    *frames,
                                     int               nframes);

   // Transpose 128 adcs x 32*N time samples
   static void transposeAdcs128x32N (int16_t              *dst,
                                     int            ndstStride,
                                     WibFrame const    *frames,
                                     int               nframes);
   // ----------------------------------------------------------


   // ----------------------------------------------------------
   // Transposers: Channel by Channel memory 
   //---------------------------------------

   // Transpose any arbitrary number of frames using the mose
   // optimal technique
   static void transposeAdcs128xN   (int16_t  *const  dst[128],
                                     int            ndstStride,
                                     WibFrame  const   *frames,
                                     int               nframes);

   // Transpose 128 adcs x 8 time samples
   static void transposeAdcs128x8N  (int16_t  *const  dst[128],
                                     int            ndstStride,
                                     WibFrame  const   *frames,
                                     int               nframes);

   // Transpose 128 adcs x 16*N time samples
   static void transposeAdcs128x16N (int16_t  *const  dst[128],
                                     int            ndstStride,
                                     WibFrame  const   *frames,
                                     int               nframes);

   // Transpose 128 adcs x 32*N time samples
   static void transposeAdcs128x32N (int16_t  *const  dst[128],
                                     int            ndstStride,
                                     WibFrame  const   *frames,
                                     int               nframes);
public:
#if 0
   uint64_t               m_header; /*!< W16  0 -  3, the WIB header word */
   uint64_t            m_timestamp; /*!< W16  4 -  7, the timestamp       */
   WibColdData  const  *m_coldData; /*!< WIB cold data streams            */
#endif
};
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION: WibFrame                                               */
/* ---------------------------------------------------------------------- */
namespace pdd    {
namespace access {

inline WibFrame const &WibFrame::assign (uint64_t const *ptr)
{
   return reinterpret_cast<WibFrame const &>(*ptr);
}

inline unsigned int    WibFrame::getNAdcs     () const { return WibFrame::NColdData 
                                                              * WibColdData::NAdcs; }
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


inline WibColdData const (& WibFrame::getColdData () const)[WibFrame::NColdData]
{
   return reinterpret_cast< const WibColdData (&)[2]>(m_coldData);
}

inline WibColdData (& WibFrame::getColdData ())[WibFrame::NColdData]
{
   return reinterpret_cast< WibColdData (&)[2]>(m_coldData);
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


inline uint64_t     WibColdData::getHeader0      () const { return m_w0; }
inline uint64_t     WibColdData::getHeader1      () const { return m_w1; }

inline unsigned int WibColdData::getStreamErrs   () const { return getStreamErrs   (m_w0); }
inline unsigned int WibColdData::getStreamErr1   () const { return getStreamErr1   (m_w0); }
inline unsigned int WibColdData::getStreamErr2   () const { return getStreamErr2   (m_w0); }
inline unsigned int WibColdData::getReserved0    () const { return getReserved0    (m_w0); }
inline uint32_t     WibColdData::getCheckSums    () const { return getCheckSums    (m_w0); }
inline unsigned int WibColdData::getCheckSumsLo  () const { return getCheckSumsLo  (m_w0); }
inline unsigned int WibColdData::getCheckSumLoA  () const { return getCheckSumLoA  (m_w0); }
inline unsigned int WibColdData::getCheckSumLoB  () const { return getCheckSumLoB  (m_w0); }
inline unsigned int WibColdData::getCheckSumsHi  () const { return getCheckSumsHi  (m_w0); }
inline unsigned int WibColdData::getCheckSumHiA  () const { return getCheckSumHiA  (m_w0); }
inline unsigned int WibColdData::getCheckSumHiB  () const { return getCheckSumHiB  (m_w0); }
inline unsigned int WibColdData::getConvertCount () const { return getConvertCount (m_w0); }

inline unsigned int WibColdData::getCheckSumA    () const  { return getCheckSumA   (m_w0); }
inline unsigned int WibColdData::getCheckSumB    () const  { return getCheckSumB   (m_w0); }

inline void         WibColdData::expandAdcs64x1 (int16_t *dst) const
{
   uint64_t const (&src)[12] = locateAdcs12b ();
   expandAdcs64x1 (dst, src);
   return;
}
   

inline unsigned int WibColdData::getStreamErrs   (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::StreamErr, Offset0::StreamErr);
}

inline unsigned int WibColdData::getStreamErr1   (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::StreamErr1, Offset0::StreamErr1);
}

inline unsigned int WibColdData::getStreamErr2   (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::StreamErr2, Offset0::StreamErr2);
}

inline unsigned int WibColdData::getReserved0    (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::Reserved0, Offset0::Reserved0);
}

inline uint32_t     WibColdData::getCheckSums    (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::CheckSums, Offset0::CheckSums);
}

inline unsigned int WibColdData::getCheckSumsLo  (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::CheckSumsLo, Offset0::CheckSumsLo);
}

inline unsigned int WibColdData::getCheckSumLoA  (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::CheckSumLoA, Offset0::CheckSumLoA);
}


inline unsigned int WibColdData::getCheckSumLoB  (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::CheckSumLoB, Offset0::CheckSumLoB);
}

inline unsigned int WibColdData::getCheckSumsHi  (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::CheckSumsHi, Offset0::CheckSumsHi);
}

inline unsigned int WibColdData::getCheckSumHiA  (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::CheckSumHiA, Offset0::CheckSumHiA);
}

inline unsigned int WibColdData::getCheckSumHiB  (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::CheckSumHiB, Offset0::CheckSumHiB);
}

inline unsigned int WibColdData::getConvertCount (uint64_t w0)
{
   return PDD_EXTRACT64 (w0, Mask0::ConvertCount, Offset0::ConvertCount);
}

inline unsigned int WibColdData::getCheckSumA   (uint64_t w0)
{
   return (getCheckSumHiA (w0) << static_cast<int>(Size0::CheckSumLoA))
         | getCheckSumLoA (w0);
}

inline unsigned int WibColdData::getCheckSumB   (uint64_t w0)
{
   return (getCheckSumHiB (w0) << static_cast<int>(Size0::CheckSumLoB))
         | getCheckSumLoB (w0);
}

inline uint64_t const (& WibColdData::locateAdcs12b () const)[12]
{
   return m_adcs;
}

inline uint64_t (& WibColdData::locateAdcs12b ())[12]
{
   return m_adcs;
}


inline unsigned int WibColdData::getErrRegister   () const 
{ 
   return getErrRegister (m_w1);
}

inline unsigned int WibColdData::getReserved1     () const 
{ 
   return getReserved1   (m_w1); 
}

inline unsigned int WibColdData::getHdrs          () const 
{ 
   return getHdrs        (m_w1); 
}

inline unsigned int WibColdData::getHdr    (int idx) const
 {
    return getHdr         (m_w1, idx); 
}

inline unsigned int WibColdData::getErrRegister  (uint64_t w1)
{
   return PDD_EXTRACT64 (w1, Mask1::ErrRegister, Offset1::ErrRegister);
}


inline unsigned int WibColdData::getReserved1    (uint64_t w1)
{
   return PDD_EXTRACT64 (w1, Mask1::Reserved1, Offset1::Reserved1);
}

inline uint32_t WibColdData::getHdrs          (uint64_t w1)
{
   return PDD_EXTRACT64 (w1, Mask1::Hdrs, Offset1::Hdrs);
}

inline uint32_t WibColdData::getHdr (uint64_t w1, int idx)
{
   auto   hdrs = getHdrs (w1);
   return getHdr (hdrs, idx);
}

inline uint32_t WibColdData::getHdr (uint32_t hdrs, int idx)
{
   uint8_t hdr = (hdrs >> static_cast<int>(Size1::Hdr) * idx)
                        & static_cast<uint32_t>(Mask1::Hdr);
   return hdr;
}
/* ---------------------------------------------------------------------- */
/*   END: IMPLEMENTATION: class WibFrame                                  */
} /* END: namespace access                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */

#endif
