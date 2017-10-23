// -*-Mode: C++;-*-

#ifndef RECORDS_DATA_HH
#define RECORDS_DATA_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     records/Data.hh
 *  @brief    Data record defintion
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
 * A data record consists of the bits and bytes that most people would
 * call the primary data plus meta-data that contain information to help
 * in the decoding and to add context to that data.  The record itself
 * is more analogous to a file system directory in that it only contains
 * other records (which they themselves could be considered directories
 * or files).  
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.10.12 jjr Separated from Headers.hh
  
\* ---------------------------------------------------------------------- */


#include <dam/access/Headers.hh>
#include <cinttypes>




/* ====================================================================== */
/* RECORD DEFINTIONS                                                      */
/* ---------------------------------------------------------------------- */
namespace pdd    {
namespace record {
/* ---------------------------------------------------------------------- *//*!

   \brief Specializes a standard header for a generic Data Record
                                                                          */
/* ---------------------------------------------------------------------- */
class DataHeader : public Header1
{
public:
   explicit DataHeader () { return; }
}  __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief Specializes a standard header for a generic Data Record
                                                                          */
/* ---------------------------------------------------------------------- */
class DataBody
{
public:
   explicit DataBody () { return; }

public:
   uint64_t m_w64[1];  /*!< Consists of a series of records               */
}  __attribute__ ((packed));
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

   \brief Specializes a standard header for a generic Data Record
                                                                          */
/* ---------------------------------------------------------------------- */
class Data : public DataHeader
{
   Data () = delete;

public:
   DataBody const m_body;  /*!< The body of the Data Record               */
}  __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
} /* END: namespace record                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */

#endif
