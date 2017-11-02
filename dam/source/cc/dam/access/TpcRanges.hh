// -*-Mode: C++;-*-

#ifndef ACCESS_TPCRECORDS_HH
#define ACCESS_TPCRECORDS_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     access/TpcRecords.hh
 *  @brief    Proto-Dune Data Tpc Data records access definition
 *  @verbatim
 *                               Copyright 2013
 *                                    by
 *
 *                       The Board of Trustees of the
 *                    Leland Stanford Junior University.
 *                           All rights reserved.
 *
 *  @endverbatim
 *
 *  @par Facility:
 *  pdd
 *
 *  @author
 *  <russell@slac.stanford.edu>
 *
 *  @par Date created:
 *  <2017/08/07>
 *
 * @par Credits:
 * SLAC
 *
 * This defines access method to a TpcRanges record.  This record
 * is found in the data section of a TpcFragment and gives both the
 * timestamp ranges of both the untrimmed and trimmed data along with
 * information to locate the trimmed data within the data packets.

 * This layout the format and primitive access methods to the data
 * found in a TpcNormal and TpcDamaged records.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.10.27 jjr Added __attribute ((unused)) to m_rsvd fields.  gcc on
                  the MAC complains about unused class members.
   2017.10.12 jjr Moved from dam/access -> dam/records
   2017.08.07 jjr Created
  
\* ---------------------------------------------------------------------- */

#include "dam/access/Headers.hh"
#include <cstdint> 
#include <cstdio>


/* ====================================================================== */
/* FORWARD REFERENCES                                                     */
/* ---------------------------------------------------------------------- */
namespace pdd    {
namespace record {

   class TpcRangesHeader;

   class TpcRangesBody;
   class TpcRangesIndices;
   class TpcRangesTimestamps;
   class TpcRangesWindow;

   class TpcRanges;

}}
/* ====================================================================== */




namespace pdd    {
namespace access {
/* ====================================================================== */
/* CLASSS DEFINITIONS                                                     */
/* ---------------------------------------------------------------------- *//*!

   \brief Access to the TPC data packet bridge word
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcRangesBridge
{
public:
   TpcRangesBridge (uint32_t bridge);

public:
   uint32_t            getBridge      () const;

   unsigned int        getRecordFormat  () const;
   uint32_t            getRangesReserved() const;

   static unsigned int getRecordFormat   (uint32_t bridge);
   static uint32_t     getRangesReserved (uint32_t bridge);

public:
   uint32_t m_w32;  /*!< The bridge word                                  */
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief This specializes the bridge word of a standard header for use
          Ramges usage.
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcRangesHeader
{
public:
   TpcRangesHeader (pdd::record::TpcRangesHeader const *header);

public:
   pdd::record::TpcRangesHeader const *getHeader         () const;
   uint32_t                            getBridge         () const;
   uint32_t                            getRecordFormat   () const;
   uint32_t                            getRangesReserved () const;
   void                                print             () const;

   static unsigned int getRecordFormat   (pdd::record::TpcRangesHeader const *header);
   static uint32_t     getRangesReserved (pdd::record::TpcRangesHeader const *header);
   static void         print             (pdd::record::TpcRangesHeader const *header);

   static unsigned int getRecordFormat   (uint32_t w32);
   static uint32_t     getRangesReserved (uint32_t w32);

private:
   pdd::record::TpcRangesHeader const *m_header;
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!
   
  \struct TpcRangesIndices
  \brief  Defines the beginning and ending of the event in terms of an 
          index into the data packets for one contributor.

   An index consists of values. A packet number and an offset into the 
   packet number.
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcRangesIndices
{
public:
   TpcRangesIndices (pdd::record::TpcRangesIndices const *indices,
                     unsigned int                          format);


public:
   uint32_t   getBegin   () const;
   uint32_t   getEnd     () const;
   uint32_t   getTrigger () const;

   static uint32_t getBegin   (pdd::record::TpcRangesIndices const *indices,
                               uint32_t                              bridge);
   static uint32_t getEnd     (pdd::record::TpcRangesIndices const *indices,
                               uint32_t                              bridge);
   static uint64_t getTrigger (pdd::record::TpcRangesIndices const *indices,
                               uint32_t                              bridge);
                                                            
   static int getPacket (uint32_t index);
   static int getOffset (uint32_t index);

private:
   pdd::record::TpcRangesIndices const *m_indices;
   uint32_t                              m_bridge;
   uint32_t                                m_rsvd __attribute__ ((unused));

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \struct TpcRangesTimestamps
  \brief  Gives the beginning and ending timestamp of the data this 
          contributor stream
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcRangesTimestamps
{
public:
   TpcRangesTimestamps (pdd::record::TpcRangesTimestamps const *timestamps,
                        uint32_t                                    bridge);

public: 
   pdd::record::TpcRangesTimestamps const *getTimestamps () const;
   uint64_t                                getBegin      () const;
   uint64_t                                getEnd        () const;

   static uint64_t 
   getBegin   (pdd::record::TpcRangesTimestamps const *timestamps,
               uint32_t                                    bridge);
                     
   static uint64_t
   getEnd     (pdd::record::TpcRangesTimestamps const *timestamps,
               uint32_t                                    bridge);

private:
   pdd::record::TpcRangesTimestamps const *m_timestamps;
   uint32_t                                    m_bridge;
   uint32_t                                      m_rsvd __attribute ((unused));

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */   



/* ---------------------------------------------------------------------- *//*!
   
  \class TpcRangesWindow
  \brief  Gives the timestamps of the beginning, ending and trigger.

  \note
  This window is a property of the trigger and, therefore, is common
  for all contributors. However for convenience it included in each 
  contributor stream.
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcRangesWindow
{
public:
   TpcRangesWindow (pdd::record::TpcRangesWindow const *window,
                    uint32_t                            bridge);

public:
   uint64_t getBegin   () const;
   uint64_t getEnd     () const;
   uint64_t getTrigger () const;

   static uint64_t getBegin   (pdd::record::TpcRangesWindow const *window,
                               uint32_t                            bridge);

   static uint64_t getEnd     (pdd::record::TpcRangesWindow const *window,
                               uint32_t                            bridge);

   static uint64_t getTrigger (pdd::record::TpcRangesWindow const *window,
                               uint32_t                            bridge);

private:
   pdd::record::TpcRangesWindow const *m_window;
   uint32_t                            m_bridge;
   uint32_t                              m_rsvd  __attribute ((unused));
      
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \class TpcRangesBody
  \brief Defines the timestamps and packet indices of both the untrimmed
         and trimmed (the event) windows
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcRangesBody
{
public:
   TpcRangesBody (pdd::record::TpcRanges     const *ranges);
   TpcRangesBody (pdd::record::TpcRangesBody const   *body,
                  uint32_t                          bridge,
                  uint32_t                          nbytes);

public:
   pdd::record::TpcRangesBody       const *getBody        () const;
   uint32_t                                getBridge      () const;
   unsigned int                            getFormat      () const;
   uint32_t                                getNbytes      () const;

   pdd::record::TpcRangesIndices    const *getIndices     () const;
   pdd::record::TpcRangesTimestamps const *getTimestamps () const;
   pdd::record::TpcRangesWindow     const *getWindow     () const;

   static void print         (pdd::record::TpcRangesBody const *body,
                              uint32_t                        bridge);

   static  pdd::record::TpcRangesIndices       const *
           getIndices    (pdd::record::TpcRangesBody const *body,
                          uint32_t                       bridge);

   static pdd::record::TpcRangesTimestamps     const *
          getTimestamps (pdd::record::TpcRangesBody const *body,
                         uint32_t                        bridge);

   static pdd::record::TpcRangesWindow     const *
                             getWindow     (pdd::record::TpcRangesBody const *body,
                                            uint32_t                        bridge);

   void        print (unsigned int format) const;

private:
   pdd::record::TpcRangesBody const *m_body;
   uint32_t                        m_bridge;
   uint32_t                        m_nbytes;

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \brief The Tpc Ranges record.  This gives the timestamp ranges of 
         both the trimmed (event) and untrimmed data.
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcRanges 
{
public:
   TpcRanges (pdd::record::TpcRanges const *ranges);

public:
   pdd::record::TpcRanges       const *getRecord () const;
   pdd::record::TpcRangesHeader const *getHeader () const;
   uint32_t                            getBridge () const;
   pdd::record::TpcRangesBody   const *getBody   () const;
   void                                print     () const;

   static pdd::record::TpcRangesHeader const *
                       getHeader (pdd::record::TpcRanges const *ranges);

   static uint32_t     getBridge (pdd::record::TpcRanges const *ranges);

   static pdd::record::TpcRangesBody   const *
                       getBody   (pdd::record::TpcRanges const *ranges);

   static void         print     (pdd::record::TpcRanges const *ranges);
   // ------------------------------------------------------------------



   // ------------------------------------------------------------------
   // TpcRangesHeader/Bridge methods
   // ------------------------------
   unsigned int        getRecordFormat  () const;
   uint32_t            getRangesReserved() const;

   static unsigned int getRecordFormat   (pdd::record::TpcRanges const *ranges);
   static uint32_t     getRangesReserved (pdd::record::TpcRanges const *ranges);
   // ------------------------------------------------------------------



   // ------------------------------------------------------------------
   // TpcRangesBody methods
   // ---------------------
   pdd::record::TpcRangesIndices    const *getIndices    () const;
   pdd::record::TpcRangesTimestamps const *getTimestamps () const;
   pdd::record::TpcRangesWindow     const *getWindow     () const;

   static pdd::record::TpcRangesIndices    const *
          getIndices    (pdd::record::TpcRanges const *ranges);

   static pdd::record::TpcRangesTimestamps const *
          getTimestamps (pdd::record::TpcRanges const *ranges);

   static pdd::record::TpcRangesWindow     const *
          getWindow     (pdd::record::TpcRanges const *ranges);


   // ------------------------------------------------------------------


private:
   pdd::record::TpcRanges  const *m_ranges; 
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
}  /* Namespace:: access                                                  */
}  /* Namespace:: pdd                                                     */
/* ---------------------------------------------------------------------- */



/* ====================================================================== */
/* IMPLEMENTATION                                                         */
/* ---------------------------------------------------------------------- */

namespace pdd    {
namespace access {

/* ====================================================================== */
/* IMPLEMENTATION: TpcRangesBridge                                        */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the Tpc Ranges record's bridge word accessor

  \param[in] bridge The bridge word
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcRangesBridge::TpcRangesBridge (uint32_t bridge) :
   m_w32 (bridge)
{
   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return the format of the Tpc Packet record
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcRangesBridge::getRecordFormat () const
{
   return getRecordFormat (m_w32);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return the reserved field of Tpc Ranges bridge word

  \note
   This is provided for completeness and diagnostic/debugging purposes.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcRangesBridge::getRangesReserved () const
{
   return getRangesReserved (m_w32);
}
/* ---------------------------------------------------------------------- */
/* END: TpcRangesBridge                                                   */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION TpcRangesHeader                                         */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the TpcRangesHeader accesser

  \param[in] header  Pointer to the TpcRangesHeader definition record
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcRangesHeader::
       TpcRangesHeader (pdd::record::TpcRangesHeader const *header) :
   m_header (header)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the TpcRangesHeader definition record
  \return A pointer to the TpcRangesHeader definition record
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcRangesHeader const *
                    TpcRangesHeader::getHeader () const
{
   return m_header;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return the TpcRangesHeader format 
  \return The TpcRangesHeader format 

  \par
   This value is usually used only internally but is provided for
   completeness and diagnostic and debugging purposes
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcRangesHeader::getRecordFormat () const
{
   return getRecordFormat (m_header);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return the TpcRangesHeader reserved field value 
  \return The TpcRangesHeader reserved field value.

  \par
   This value is usually used only internally but is provided for
   completeness and diagnostic and debugging purposes
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcRangesHeader::getRangesReserved () const
{
   return getRangesReserved (m_header); 
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Print the TpcRangesHeader
                                                                          */
/* ---------------------------------------------------------------------- */
inline void  TpcRangesHeader::print () const
{
   print (m_header);
}
/* ---------------------------------------------------------------------- */
/* END: TpcRangesHeader                                                   */
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION TpcRangesBody                                           */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the TpcRangesBody accesser

  \param[in] ranges  Pointer to the TpcRanges definition record
  \param[in] bridge  The bridge word of the TpcRanges record
  \param[in] nbytes  The number of bytes in the record body
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcRangesBody::TpcRangesBody (pdd::record::TpcRangesBody const *body,
                                     uint32_t                        bridge,
                                     uint32_t                        nbytes) :
   m_body   (  body),
   m_bridge (bridge),
   m_nbytes (nbytes)
{
   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the TpcRangesBody definition record body
  \return A pointer to the TpcRangesBody definition record
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcRangesBody const *
                    TpcRangesBody::getBody () const
{
   return m_body;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return the bridge word of TpcRangesBody record class
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcRangesBody::getBridge () const
{
   return m_bridge;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return the format of TpcRangesBody record class
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcRangesBody::getFormat () const
{
   return TpcRangesBridge::getRecordFormat (m_bridge);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return the number of bytes in the TpcRangesBody definition.
          record body
  \return the number of bytes in the TpcRangesBody definition record body.
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcRangesBody::getNbytes () const
{
   return m_nbytes;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the TpcRangesIndices definition 
          record
  \return A pointer to the TpcRangesIndices definition record
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcRangesIndices    const *
                    TpcRangesBody::getIndices () const
{
   return getIndices (m_body, m_bridge);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the TpcRangesTimestamps definition 
          record
  \return A pointer to the TpcRangesTimestamps definition record
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcRangesTimestamps            const *
                    TpcRangesBody::getTimestamps () const
{
   return getTimestamps (m_body, m_bridge);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

   \brief  Return a pointer to the TpcRangesWindow definition 
           record
   \return A pointer to the TpcRangesWindow definition record
                                                                          */
/* ---------------------------------------------------------------------- */
inline  pdd::record::TpcRangesWindow    const *
                     TpcRangesBody::getWindow () const
{
   return getWindow (m_body, m_bridge);
}
/* ---------------------------------------------------------------------- */
/* END: TpcRangesBody                                                     */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION:: TpcRangesIndices                                      */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for TpcRangesIndices accessor

  \param[in] indices A pointer to the TpcRangesIndices definition
  \param[in] bridge  The recors's bridge word
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcRangesIndices::
       TpcRangesIndices (pdd::record::TpcRangesIndices const *indices,
                         uint32_t                              bridge) :
          m_indices (indices),
          m_bridge  ( bridge),
          m_rsvd    (      0)
{
   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the data packet index for the beginning of the event
  \return The data packet index for the beginning of the event
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t  TpcRangesIndices::getBegin () const
{
   return getBegin (m_indices, m_bridge);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the data packet index for the ending of the event
  \return The data packet index for the ending of the event
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcRangesIndices::getEnd () const
{
   return getEnd (m_indices, m_bridge);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the data packet index for the triggering sample of the 
          event.
  \return The data packet index for the triggering sample of the event
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcRangesIndices::getTrigger () const
{
   return getTrigger (m_indices, m_bridge);
}
/* ---------------------------------------------------------------------- */
/* END::  TpcRangesIndices                                                */
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION :: TpcRangesTimestamps                                  */
/* ---------------------------------------------------------------------- *//*!

  \brief Construct for TpcRangesTimestamps

  \param[in] timestamps  A pointer to the TpcRangesTimestamps definition 
  \param[in] bridge      The record's bridge word
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcRangesTimestamps::
       TpcRangesTimestamps (pdd::record::TpcRangesTimestamps const *timestamps,
                            uint32_t                                    bridge) :
          m_timestamps (timestamps),
          m_bridge     (    bridge),
          m_rsvd       (         0)
{
   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns a pointer to the TpcRengesTimestamps definition.
  \return A pointer to the TpcRengesTimestamps definition.
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcRangesTimestamps const *
                    TpcRangesTimestamps::getTimestamps () const
{
   return m_timestamps;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the timestamp of the beginning of the data packets.
  \return The timestamp of the beginning of the data packets.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TpcRangesTimestamps::getBegin () const
{
   return getBegin (m_timestamps, m_bridge);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Returns the timestamp of the ending of the data packets.
  \return The timestamp of beginning of beginning of the data
          packets.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TpcRangesTimestamps::getEnd () const
{
   return getEnd (m_timestamps, m_bridge);
}
/* ---------------------------------------------------------------------- */
/* END: TpcRangesTimestamps                                               */
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION:: TpcRangesBody::Window                                 */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for a TpcRangesWindow accesser

  \param[in] window Pointer to the underlying window data
  \param[in] bridge The record's bridge word
                                                                         */
/* ----------------------------------------------------------------------*/
inline TpcRangesWindow::
       TpcRangesWindow (pdd::record::TpcRangesWindow const *window,
                        uint32_t                            bridge) :
          m_window (window),
          m_bridge (bridge),
          m_rsvd   (     0)
{
   return;
}
/* ----------------------------------------------------------------------*/


/* ---------------------------------------------------------------------- *//*!

  \brief      Get the beginning timestamp of the event window
  \param[in]  The beginning timestamp of the event
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TpcRangesWindow::getBegin   () const
{
   return getBegin (m_window, m_bridge);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief      Get the ending timestamp of the event window
  \param[in]  The ending timestamp of the event
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TpcRangesWindow::getEnd () const
{
   return getEnd (m_window, m_bridge);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief      Get the trigger timestamp of the event window
  \param[in]  The ending timestamp of the event
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint64_t TpcRangesWindow::getTrigger () const
{
   return getTrigger (m_window, m_bridge);
}
/* ---------------------------------------------------------------------- */
/*   END: TpcRangeWindow                                                  */
/* ====================================================================== */







/* ====================================================================== */
/* IMPLEMENTATION TpcRanges                                               */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the TpcRanges accessor 

  \param[in] ranges  Pointer to the TpcRanges definition record
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcRanges::TpcRanges (pdd::record::TpcRanges const *ranges) :
   m_ranges (ranges)
{
   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the TpcRanges definition record
  \return A pointer to the TpcRanges definition record
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcRanges const *TpcRanges::getRecord () const 
{ 
   return m_ranges;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer to the TPC Ranges record header

  \param[in] ranges The TPC Ranges record
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcRangesHeader const 
       *TpcRanges::getHeader (pdd::record::TpcRanges const *ranges)
{
   return reinterpret_cast<pdd::record::TpcRangesHeader const *>(ranges);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the TpcRangesHeader definition record
  \return A pointer to the TpcRangesHeader definition record
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcRangesHeader const *TpcRanges::getHeader () const 
{ 
   return getHeader (m_ranges);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return the bridge word as a 32-bit immediate value
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcRanges::getBridge () const
{
   return getBridge (m_ranges);
}

/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Get the format of Tpc Ranges record itself
  \return The format of Tpc Ranges record itself
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcRanges::getRecordFormat () const 
{ 
   return getRecordFormat (m_ranges);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the TpcRangesBody definition record
  \return A pointer to the TpcRangesHeader definition record
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcRangesBody const *TpcRanges::getBody () const 
{ 
   return getBody (m_ranges);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return the TpcRanges record bridge word's reserved field

  \note
   This is provided for completeness and diagnostic/debugging reasons.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcRanges::getRangesReserved() const
{
   return getRangesReserved (m_ranges);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the TpcRanges Window class
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcRangesWindow const *
       TpcRanges::getWindow ()      const
{
   return getWindow (m_ranges);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer the TpcRanges packet indices class
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcRangesIndices const *
         TpcRanges::getIndices    () const
{
   return getIndices (m_ranges);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer the TpcRanges packet timestamps class
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcRangesTimestamps const *
       TpcRanges::getTimestamps () const
{
   return getTimestamps (m_ranges);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Primitive print method
                                                                          */
/* ---------------------------------------------------------------------- */
inline void  TpcRanges::print () const 
{ 
   print (m_ranges); 
} 
/* ---------------------------------------------------------------------- */
/*   END: TpcRanges                                                       */
} /* END: namespace access                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */

#endif
