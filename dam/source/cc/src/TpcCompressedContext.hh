// -*-Mode: C++;-*-

#ifndef TPCCOMPRESSDEDCONTEXT_HH
#define TPCCOMPRESSDEDCONTEXT_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcCompressedContext.hh
 *  @brief    The decoding context
 *  @verbatim
 *                               Copyright 2018
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
 *  <2018/07/17>
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
   2018.07.17 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "BFU.h"


/* ---------------------------------------------------------------------- *//*!

  \brief The decoding context
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcCompressedContext
{
public:
   TpcCompressedContext (BFU bfu, uint64_t const *buf, int nsamplesM1);

public:
   uint32_t     m_nbins;  /*!< Index of last non-zero bin          */
   uint32_t m_firstAdcs;  /*!< First ADCs                          */
   uint32_t   m_novrflw;  /*!< Number of bits in the overflow ADCs */
   uint16_t  m_bins[64];  /*!< The histogram bins                  */
};
/* ---------------------------------------------------------------------- */


#endif
