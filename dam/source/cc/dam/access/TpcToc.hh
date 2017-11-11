// -*-Mode: C++;-*-

#ifndef ACCESS_TPCTOC_HH
#define ACCESS_TPCTOC_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcToc.hh
 *  @brief    Proto-Dune Data Tpc Table Of Contents records access
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
 *  <2017/10/15>
 *
 * @par Credits:
 * SLAC
 *
 * This define the acccess method for the Table of Contents records found
 * in a Tpc record.
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.10.16 jjr Moved from dam/access -> dam/records
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

   class    TpcTocHeader;
   class TpcTocPacketDsc;
   class      TpcTocBody;
   class          TpcToc;

}
}


namespace pdd    {
namespace access {

   class WibFrame;
}
}
/* ====================================================================== */




namespace pdd    {
namespace access {



/* ====================================================================== */
/* CLASSS DEFINITIONS                                                     */
/* ---------------------------------------------------------------------- *//*!

   \brief Access to the TPC data packet bridge word
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcTocBridge
{
public:
   TpcTocBridge (uint32_t bridge);

public:
   uint32_t            getBridge       () const;

   unsigned int        getRecordFormat () const;
   int                 getNPacketDscs  () const;

   static unsigned int getRecordFormat (uint32_t bridge);
   static int          getNPacketDscs  (uint32_t bridge);

public:
   uint32_t m_w32;  /*!< The bridge word                                  */
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief This specializes the bridge word of a standard header for use
          Toc usage.
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcTocHeader 
{
public:
   TpcTocHeader (pdd::record::TpcTocHeader const *header);

public:
   pdd::record::TpcTocHeader const *getHeader      () const;
   uint32_t                         getBridge      () const;
   unsigned int                     getRecordFormat() const;
   int                              getNPacketDscs () const;
   void                             print          () const;

   static unsigned int getRecordFormat(pdd::record::TpcTocHeader const *header);
   static int          getNPacketDscs (pdd::record::TpcTocHeader const *header);
   static void         print          (pdd::record::TpcTocHeader const *header);


   static unsigned int getRecordFormat (uint32_t w32);
   static int          getNPacketDscs  (uint32_t w32);

public:
   pdd::record::TpcTocHeader const *m_header;

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \class TpcTocPacketDsc
  \brief Access to a TpcTocPacket descriptor, essentially one 
         table of contents entry.
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcTocPacketDsc
{
public:
   TpcTocPacketDsc (pdd::record::TpcTocPacketDsc const *dsc);

public:
   pdd::record::TpcTocPacketDsc       const *getDsc () const;
   unsigned int                           getFormat () const;
   unsigned int                             getType () const;
   uint32_t                             getOffset64 () const;
   bool                                  isWibFrame () const;
   unsigned int                       getNWibFrames () const;


   static unsigned int     getFormat (pdd::record::TpcTocPacketDsc const *dsc);
   static unsigned int       getType (pdd::record::TpcTocPacketDsc const *dsc);
   static uint32_t       getOffset64 (pdd::record::TpcTocPacketDsc const *dsc);
   static bool            isWibFrame (pdd::record::TpcTocPacketDsc const *dsc);
   static unsigned int getNWibFrames (pdd::record::TpcTocPacketDsc const *dsc);


   static unsigned int getFormat (pdd::record::TpcTocPacketDsc dsc);
   static unsigned int   getType (pdd::record::TpcTocPacketDsc dsc);
   static uint32_t   getOffset64 (pdd::record::TpcTocPacketDsc dsc);
   static bool        isWibFrame (pdd::record::TpcTocPacketDsc dsc);


/* ---------------------------------------------------------------------- */

   private:
   pdd::record::TpcTocPacketDsc const *m_dsc;
} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */




/* ---------------------------------------------------------------------- *//*!

  \class TpcTocBody
  \brief Table of contents for the data pieces
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcTocBody
{
public:
   TpcTocBody (pdd::record::TpcToc     const  *toc);
   TpcTocBody (pdd::record::TpcTocBody const *body,
               unsigned int                 bridge,
               uint32_t                     nbytes);

public:
   pdd::record::TpcTocBody      const       *getBody () const;
   uint32_t                                getBridge () const;
   unsigned int                            getFormat () const;
   uint32_t                                getNbytes () const;

   uint32_t                     const        *getW32 () const;
   pdd::record::TpcTocPacketDsc const *getPacketDscs () const;
   pdd::record::TpcTocPacketDsc const *getPacketDsc  (int idx) const;

   void                     print (unsigned int bridge) const;

   WibFrame const *locateWibFrames ();
   WibFrame const *locateWibFrame  (int idx);


   static uint32_t const *
          getW32       (pdd::record::TpcTocBody const *body);

   static pdd::record::TpcTocPacketDsc const *
          getPacketDsc (pdd::record::TpcTocBody const *body,
                        int                            idx);

   static pdd::record::TpcTocPacketDsc const *
          getPacketDscs (pdd::record::TpcTocBody const *body);
   
   static void      print (pdd::record::TpcTocBody const *body, 
                           unsigned int                 bridge);

private:
   pdd::record::TpcTocBody const *m_body; /*!< The TOC record body        */
   uint32_t                     m_bridge; /*!< The bridge word            */
   uint32_t                     m_nbytes; /*!< Length in nybtes           */

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief The table of contents record = header + body
                                                                          */
/* ---------------------------------------------------------------------- */
class TpcToc 
{
public:
   TpcToc (pdd::record::TpcToc const *toc);

public:
   // Direct methods
   pdd::record::TpcToc       const *getRecord () const;
   pdd::record::TpcTocHeader const *getHeader () const;
   uint32_t                         getBridge () const;
   pdd::record::TpcTocBody   const *getBody   () const;
   void                             print     () const;

   static pdd::record::TpcTocHeader const 
                         *getHeader (pdd::record::TpcToc const *toc);

   static uint32_t        getBridge (pdd::record::TpcToc const *toc);

   static pdd::record::TpcTocBody   const 
                         *getBody   (pdd::record::TpcToc const *toc);

   static  void              print  (pdd::record::TpcToc const *toc);
   // ------------------------------------------------------------------



   // ------------------------------------------------------------------
   // TpcTocHeader/Bridge methods
   // ---------------------------
   unsigned int           getRecordFormat () const;
   int                    getNPacketDscs  () const;

   static unsigned int    getRecordFormat (pdd::record::TpcToc const *toc);
   static int             getNPacketDscs  (pdd::record::TpcToc const *toc);
   // ------------------------------------------------------------------


   // ------------------------------------------------------------------
   // TpcTocPacketDsc methods
   // -----------------------
   pdd::record::TpcTocPacketDsc const *getPacketDscs ()        const;
   pdd::record::TpcTocPacketDsc const *getPacketDsc  (int idx) const;

   static pdd::record::TpcTocPacketDsc  const *
                          getPacketDscs (pdd::record::TpcToc const *toc);

   static pdd::record::TpcTocPacketDsc const *
                          getPacketDsc (pdd::record::TpcToc const *toc,
                                         int                        idx);
   // ------------------------------------------------------------------

private:
   pdd::record::TpcToc const *m_toc;

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
/* END DEFINITIONS                                                        */
/* ====================================================================== */



/* ====================================================================== */
/* IMPLEMENTATION: TpcTocBridge                                           */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the Tpc Table Of Contents bridge word accessor

  \param[in] bridge The bridge word
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcTocBridge::TpcTocBridge (uint32_t bridge) :
   m_w32 (bridge)
{
   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return the format of the Table of Contents Record
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcTocBridge::getRecordFormat () const
{
   return getRecordFormat (m_w32);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return the number of active descriptors in the 
         Table of Contents Record

  \note
   The number of active descriptors does not include the terminating
   descriptor. 
                                                                          */
/* ---------------------------------------------------------------------- */
inline int TpcTocBridge::getNPacketDscs () const
{
   return getNPacketDscs (m_w32);
}
/* ---------------------------------------------------------------------- */
/* END: TpcTocBridge                                                      */
/* ====================================================================== */



/* ====================================================================== */
/* IMPLEMENTATION: TpcTocHeader                                           */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for a Tpc Table Of Contents Header

  \param[in] header A pointer to the Tpc Table Of Contents Header record
                    definition.
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcTocHeader::TpcTocHeader (pdd::record::TpcTocHeader const *header) :
   m_header (header)
{
   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Get the format of the Tpc Table Of Contents record
  \return The format of the Tpc Table Of Contents record
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcTocHeader::getRecordFormat () const
{ 
   return getRecordFormat (m_header);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Get the number of Tpc Table of Contents descriptors, essentially
          a Table Of Contents entry.
  \return The number of Tpc Table of Contents descriptors
                                                                          */
/* ---------------------------------------------------------------------- */
inline int TpcTocHeader::getNPacketDscs () const 
{ 
   return getNPacketDscs (m_header);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Print a Tpc Table Of Contents record header
                                                                          */
/* ---------------------------------------------------------------------- */
inline void TpcTocHeader::print () const 
{ 
   print (m_header); 
}
/* ---------------------------------------------------------------------- */
/* END : TpcTocHeader                                                     */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION: TpcTocPacketDsc                                        */
/* ---------------------------------------------------------------------- */
inline TpcTocPacketDsc::
       TpcTocPacketDsc (pdd::record::TpcTocPacketDsc const *dsc) :
   m_dsc (dsc)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the packet descriptor
  \return A pointer to the packet descriptor
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcTocPacketDsc const *
                    TpcTocPacketDsc::getDsc () const
{
   return m_dsc;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the format of the descriptor itself
  \return  The format of the descriptor.

  \par
   The format defines the layout of the descriptor, not the data packet
   it is describing.  It is generally only for internal consumption,
   allowing the proper decoding of the descriptor, but is provided for
   completeness and debugging/diagnostic purposes.
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcTocPacketDsc::getFormat () const
{
   return getFormat (m_dsc);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief   Get the packet format type
  \return  The packet format type
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcTocPacketDsc::getType () const
{
   return getType (m_dsc);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Returns the 64-bit word offset into the Tpc packets payload
         of the begining of the packet
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcTocPacketDsc::getOffset64 () const
{
   return getOffset64 (m_dsc);
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Tests if the packet format is a raw Wib Frame
                                                                          */
/* ---------------------------------------------------------------------- */
inline bool TpcTocPacketDsc::isWibFrame () const
{
   return isWibFrame (m_dsc);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Returns the number of WIB frames in this packet
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcTocPacketDsc::getNWibFrames () const
{
   return getNWibFrames (m_dsc);
}
/* END: TpcTocPacketDsc                                                   */
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION: TpcTocBody                                             */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor for the body of Table of Contents

  \param[in]   body  A pointer to the Tpc Table of Contents record body
  \param[in] bridge  The bridge word
  \param[in] nbytes  The number of 64-bit words in the record body
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcTocBody::TpcTocBody  (pdd::record::TpcTocBody const *body,
                                unsigned int                 bridge,
                                uint32_t                     nbytes) :
   m_body   (  body),
   m_bridge (bridge),
   m_nbytes (nbytes)
{ 
   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return the bridge word of TpcTocBody record class
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcTocBody::getBridge () const
{
   return m_bridge;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief   Returns a bare 32-bit pointer to the contents 
  \return  A bare 32-bit 

  \note
   This is not a main line method. It is used primarily for debugging.
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcTocBody const *TpcTocBody::getBody () const
{
   return m_body;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return the format of TpcTocBody record class
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcTocBody::getFormat () const
{
   return TpcTocBridge::getRecordFormat (m_bridge);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return the number of bytes in the TpcTocBody record
                                                                          */
/* ---------------------------------------------------------------------- */
inline  uint32_t TpcTocBody::getNbytes () const
{
   return m_nbytes;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief   Returns a bare 32-bit pointer to the contents 
  \return  A bare 32-bit 

  \note
   This is not a main line method. It is used primarily for debugging.
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t const *TpcTocBody::getW32 () const
{
   return reinterpret_cast<uint32_t const *>(this);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the list of Tpc Table Of Contents packet
          descriptors.
  \return A pointer to the list of Tpc Table Of Contents packet
          descriptors.
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcTocPacketDsc const *TpcTocBody::getPacketDscs () const
{
   uint32_t const         *ptr = getW32 ();
   pdd::record::TpcTocPacketDsc const 
                          *dscs = reinterpret_cast<decltype (dscs)>(ptr);
   return dscs;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Return a pointer to the specified Tpc Table Of Contents packet
          descriptor.
  \return A pointer to the specified Tpc Table Of Contents packet
          descriptors.

  \param[in] idx  The index of the Table Of Contents packet
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcTocPacketDsc const *TpcTocBody::getPacketDsc (int idx) const
{
   return getPacketDsc (m_body, idx);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Prints the body of Table of Contents

  \param[in] bridge  The bridge word this Table of Contents record
                                                                          */
/* ---------------------------------------------------------------------- */
inline void TpcTocBody::print (unsigned int bridge) const 
{ 
   print (m_body, bridge);
}
/* ---------------------------------------------------------------------- */
/* END : TpcTocBody                                                       */
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION: TpcToc                                                 */
/* ---------------------------------------------------------------------- *//*!

  \brief Constructor to Tpc Table of Contents record accessor

  \param[in] toc  A pointer to the Tpc Table of Contents record accessor
                                                                          */
/* ---------------------------------------------------------------------- */
inline TpcToc::TpcToc (pdd::record::TpcToc const *toc) :
   m_toc (toc)
{
   return;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer the Tpc Table of Contents record
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcToc const *TpcToc::getRecord () const
{
   return m_toc;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer to the TPC Table Of Contents record header

  \param[in] toc The TPC Table Of Contents record 
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcTocHeader const 
      *TpcToc::getHeader (pdd::record::TpcToc const *toc)
{
   return reinterpret_cast<pdd::record::TpcTocHeader const *>(toc);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer the Tpc Table of Contents record header
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcTocHeader const *TpcToc::getHeader () const
{
   return getHeader (m_toc);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return the bridge word as a 32-bit immediate value
                                                                          */
/* ---------------------------------------------------------------------- */
inline uint32_t TpcToc::getBridge () const
{
   return getBridge (m_toc);
}

/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Get the format of Tpc Table of Contents itself
  \return The format of Tpc Table of Contents itself
                                                                          */
/* ---------------------------------------------------------------------- */
inline unsigned int TpcToc::getRecordFormat () const 
{ 
   return getRecordFormat (m_toc);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer the Tpc Table of Contents record body
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcTocBody const *TpcToc::getBody () const
{
   return getBody (m_toc);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer to the list of TpcTocPacket Descriptors
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcTocPacketDsc const *TpcToc::getPacketDscs () const
{
   return getPacketDscs (m_toc);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer to the specified of TpcTocPacket Descriptor

  \param[in] idx The index of the desired descriptor
                                                                          */
/* ---------------------------------------------------------------------- */
inline pdd::record::TpcTocPacketDsc const *TpcToc::getPacketDsc (int idx) const
{
   return getPacketDsc (m_toc, idx);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Return a pointer the Tpc Table of Contents record body
                                                                          */
/* ---------------------------------------------------------------------- */
inline int TpcToc::getNPacketDscs () const
{
   return getNPacketDscs (m_toc);
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Rudimentary print method
                                                                          */
/* ---------------------------------------------------------------------- */
inline void TpcToc::print () const 
{ 
   print (m_toc);
}
/* ---------------------------------------------------------------------- */
/* END: TpcToc                                                            */
/* ====================================================================== */



/* ---------------------------------------------------------------------- */
}  /* Namespace:: record                                                  */
}  /* Namespace:: pdd                                                     */
/* ---------------------------------------------------------------------- */

#endif
