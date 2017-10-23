// -*-Mode: C++;-*-

#ifndef ACCESS_IDENTIFIER_HH
#define ACCESS_IDENTIFIER_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     access/Identifier.hh
 *  @brief    Identifier accesss defintion
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

   class Identifier;

}
}
/* ====================================================================== */




/* ====================================================================== */
/* class Identifier                                                       */
/* ---------------------------------------------------------------------- */
namespace pdd    {
namespace access {
/* ---------------------------------------------------------------------- */
class Identifier 
{
public:
   Identifier (pdd::record::Identifier const *id) :
      m_id (id)
   {
      return;
   }


public:
   pdd::record::Identifier const*getIdentifier         () const;
   uint32_t                      getFormat             () const;
   uint32_t                      getSrc         (int idx) const;
   uint32_t                      getSequence           () const;
   uint64_t                      getTimestamp          () const;
   void                          print                 () const;


   static uint32_t getFormat   (pdd::record::Identifier const *id);
   static uint32_t getType     (pdd::record::Identifier const *id);
   static uint32_t getSrc0     (pdd::record::Identifier const *id);
   static uint32_t getSrc1     (pdd::record::Identifier const *id);
   static uint32_t getSrc      (pdd::record::Identifier const *id, int idx);
   static uint32_t getSequence (pdd::record::Identifier const *id);
   static uint64_t getTimestamp(pdd::record::Identifier const *id);
   static void     print       (Identifier const *identifier);

private:
   pdd::record::Identifier const *m_id;

} __attribute__ ((packed));
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION : Identifier                                            */
/* ---------------------------------------------------------------------- */
namespace pdd     {
namespace access  {


inline pdd::record::Identifier const *Identifier::getIdentifier () const 
{ 
   return m_id;
}

inline uint32_t Identifier::getFormat () const 
{ 
   return getFormat (m_id);
}

inline uint32_t Identifier::getSrc (int idx) const 
{ 
   return getSrc (m_id, idx); 
}

inline uint32_t Identifier::getSequence () const 
{ 
   return getSequence (m_id);
}

inline uint64_t Identifier::getTimestamp () const 
{ 
   return  getTimestamp (m_id);
}

inline void Identifier::print () const 
{ 
   print (this);
}
/* ---------------------------------------------------------------------- */
} /* END: namespace access                                                */
} /* END: namespace pdd                                                   */
/* ====================================================================== */

#endif
