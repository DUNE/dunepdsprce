// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcCompressedContext.hh
 *  @brief    Unpacks the decompression decoding context
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



#include "TpcCompressedContext.hh"


/* ---------------------------------------------------------------------- *//*!

  \brief  Decodes the decompression decoding context

  \param[in]        bfu  The bit stream context
  \param[in]        buf  The 64-bit stream of bits
  \param[in] nsamplesM1  The number of number of samples less 1
                                                                          */
/* ---------------------------------------------------------------------- */
TpcCompressHistogram:TpcCompressedContext (BFU            &bfu, 
                                           uint64_t const *buf,
                                           int      nsamplesM1)
{
   int left = nsamplesM1;

   int position = _bfu_get_pos  (bfu);
   int format   = _bfu_extractR (bfu, buf, position,  4);
   int nbins    = _bfu_extractR (bfu, buf, position,  8) + 1;
   int mbits    = _bfu_extractR (bfu, buf, position,  4);
   int nbits    = mbits;

   // Return decoding context
  *first        = _bfu_extractR (bfu, buf, position, 12);
  *novrflw      = _bfu_extractR (bfu, buf, position,  4);
  *nrbins       = nbins;

   for (int ibin = 0; ibin < nbins; ibin++)
   {
      // Extract the bits
      int cnts   = left ? _bfu_extractR (bfu, buf, position, nbits) : 0;

      bins[ibin] =  cnts;

      /* STRIP removed 2018-06-28 to reduce output
      if ( (ibin & 0xf) == 0x0) std::cout << "Hist[" << std::hex << std::setw(2) << ibin << "] ";
      std::cout <<  ' ' << std::hex << std::setw(3) << cnts  << ":" << std::hex << std::setw(1) << nbits;
      if ( (ibin & 0xf) == 0xf) std::cout << std::endl;
      */

      left -= cnts;

      nbits = 32 - __builtin_clz (left);
      if (nbits > mbits) nbits = mbits;
   }

   return position;
}
