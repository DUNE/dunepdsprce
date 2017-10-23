// -*-Mode: C++;-*-

#ifndef ACCESS_DATA_HH
#define ACCESS_DATA_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     access/Data.hh
 *  @brief    Data Record access defintion
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


#include <cinttypes>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace pdd    {
namespace record {

   class DataHeader;
   class Data;

}
}
/* ====================================================================== */




/* ====================================================================== */
/* CLASS DEFINTIONS                                                      */
/* ---------------------------------------------------------------------- */
namespace pdd    {
namespace access {
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief Data Record
                                                                          */
/* ---------------------------------------------------------------------- */
class Data 
{
public:
   Data (pdd::record::Data const *data) :
      m_data (data)
   {
      return;
   }

public:
   pdd::record::Data const *getRecord () const;
   void                         print () const;

   static void print (Data const *data);

public:
   pdd::record::Data const *m_data;
};
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */






/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */
namespace pdd    {
namespace access {


inline pdd::record::Data const *Data::getRecord () const { return m_data; }
inline void                         Data::print () const { print (this);  }


/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */

#endif
