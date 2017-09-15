// -*-Mode: C++;-*-

#ifndef PD_DAM_BFEXTRACT_HH
#define PD_DAM_BFEXTRACT_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     BfExtract.hh
 *  @brief    Proto-Dune Data bit field extraction routines
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
 *  <2017/08/16>
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
   2017.08.16 jjr Created
  
\* ---------------------------------------------------------------------- */


#define PDD_DEBUG(_mask, _offset, _val)                                   \
   fprintf (stderr,                                                       \
           "Mask: %8.8" PRIx32 " Offset: %8.8x Value: %8.8" PRIx32 "\n",  \
            static_cast<uint32_t>(  _mask),                               \
            static_cast<     int>(_offset),                               \
            static_cast<uint32_t>(  _val)),                               \


#define PDD_EXTRACT(_type, _val, _mask, _offset)                         \
   (((_val) >> static_cast<int>(_offset)) & static_cast<_type>(_mask))

#define PDD_EXTRACT64(_val, _mask, _offset)                              \
        PDD_EXTRACT(uint64_t, _val, _mask, _offset)
   
#define PDD_EXTRACT32(_val, _mask, _offset)                              \
        PDD_EXTRACT(uint32_t, _val, _mask, _offset)



#endif
