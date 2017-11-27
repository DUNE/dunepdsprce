#// -*-Mode: C++;-*-

#ifndef PDD_WIB_FRAME_IMPL_HH
#define PDD_WIB_FRAME_IMPL_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     WibFrame-Impl.hh
 *  @brief    WibFrameImpl Base class for AVX2/AVX/Generic implementation
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
 *  kvtsang@slac.stanford.edu
 *
 *  @par Date created:
 *  2017.11.22
 *
 * @par Credits:
 * SLAC
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\

   HISTORY
   -------

   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.11.27 pt  Added WibFrameImpl. Mostly copied from WibFrame.cc.
\* ---------------------------------------------------------------------- */


#include "dam/access/WibFrame.hh"

namespace pdd {
namespace access {

class WibFrameImpl
{
    public:
       virtual void expandAdcs16_init_kernel ()  = 0;

       virtual void expandAdcs64x1_kernel  (uint16_t       *dst, 
                                            uint64_t const *src) = 0;

       virtual void transposeAdcs16x8_kernel  (uint16_t       *dst, 
                                               int          offset, 
                                               uint64_t const *src) = 0;

       virtual void transposeAdcs16x8_kernel  (uint16_t *const *dst,
                                               int           offset, 
                                               uint64_t const  *src) = 0;

/* ---------------------------------------------------------------------- *//*!

  \brief  Transpose 8N time samples for 16 channels

  \param[out]   dst The destination array
  \param[ in]    n8 The number of groups of 8 channels, \e i.e. the N in
                    transpose16x8N
  \param[in] offset The number of elements in on channel's destination
                    array.
  \param[in]    src The source array
                                                                          */
/* ---------------------------------------------------------------------- */
inline void transposeAdcs16x8N_kernel (uint16_t       *dst, 
                                       int              n8,
                                       int          stride,
                                       uint64_t const *src)
{
   for (int idx = 0; idx < n8; ++idx)
   {
      transposeAdcs16x8_kernel (dst, stride, src + idx * 8 * sizeof (WibFrame) / sizeof (*src));
      dst += 8;
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
inline void transposeAdcs16x16_kernel (uint16_t       *dst, 
                                       int          stride, 
                                       uint64_t const *src)
{
   transposeAdcs16x8N_kernel (dst, 2, stride, src);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
inline void transposeAdcs16x32_kernel (uint16_t       *dst, 
                                       int          stride, 
                                       uint64_t const *src)
{
   transposeAdcs16x8N_kernel (dst, 4, stride, src);
}
/* ---------------------------------------------------------------------- */
/* END: CONTIGIOUS TRANSPOSITION                                          */
/* ====================================================================== */





/* ====================================================================== */
/* BEGIN: CHANNEL-BY-CHANNEL TRANSPOSITION                                */
/* ---------------------------------------------------------------------- *//*!

  \brief  Transpose 8N time samples for 16 channels

  \param[out]   dst Pointers to 16 arrays to receive the transposed data
  \param[ in]    n8 The number of groups of 8 channels, \e i.e. the N in
                    transpose16x8N
  \param[in] offset The number of elements in on channel's destination
                    array.
  \param[in]    src The source array
                                                                          */
/* ---------------------------------------------------------------------- */
inline void transposeAdcs16x8N_kernel (uint16_t *const *dst, 
                                       int               n8,
                                       int           offset,
                                       uint64_t const  *src)
{
   for (int idx = 0; idx < n8; ++idx)
   {
      transposeAdcs16x8_kernel (dst, offset, src);
      src    += 8 * sizeof (WibFrame) / sizeof (*src);
      offset += 8;
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
inline void transposeAdcs16x16_kernel (uint16_t *const *dst, 
                                       int           offset,
                                       uint64_t const  *src)
{
   transposeAdcs16x8N_kernel (dst, 2, offset, src);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
inline void transposeAdcs16x32_kernel (uint16_t *const *dst, 
                                       int           offset, 
                                       uint64_t const  *src)
{
   transposeAdcs16x8N_kernel (dst, 4, offset, src);
}
/* ---------------------------------------------------------------------- */
/* END: CONTIGIOUS TRANSPOSITION                                          */
/* ====================================================================== */

};
}}

#endif
