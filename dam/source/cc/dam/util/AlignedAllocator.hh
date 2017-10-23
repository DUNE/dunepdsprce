#ifndef PDD_ALIGNEDALLOCATOR_HH
#define PDD_ALIGNEDALLOCATOR_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     AlignedAllocator.hh
 *  @brief    Defines an aligned allocator to use with std containers
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
 *  pdd
 *
 *  @author
 *  <russell@slac.stanford.edu>
 *
 *  @par Date created:
 *  <2017/10/04>
 *
 * @par Credits:
 * SLAC
 *
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.10.04 jjr Created
  
\* ---------------------------------------------------------------------- */


#include <stdlib.h>
#include <malloc.h>


namespace pdd
{

/* ---------------------------------------------------------------------- *//*!

   \brief Template to define a custom std container allocator that
          allocates aligned memory.

   \param   N The deserved alignment. This must be a power of 2
   \param   T The type of the allocation
                                                                          */
/* ---------------------------------------------------------------------- */
template <int N, class T>
class AlignedAllocator
{
public:
  typedef size_t          size_type;
  typedef ptrdiff_t difference_type;
  typedef T*                pointer;
  typedef const T*    const_pointer;
  typedef T&              reference;
  typedef const T&  const_reference;
  typedef T              value_type;

  AlignedAllocator() {}
  AlignedAllocator(const AlignedAllocator&) {}


  pointer   allocate(size_type n, const void * = 0) 
            {
                T* t = (T*) memalign (N, n * sizeof(T));
                return t;
            }
  
  void      deallocate(void* p, size_type) 
            {
              if (p)
              {
                free(p);
              } 
            }

  pointer                 address(reference x) const { return &x; }
  const_pointer           address(const_reference x) const { return &x; }
  AlignedAllocator<N, T> &operator=(const AlignedAllocator&) { return *this; }
  void                    construct(pointer p, const T& val) 
                         { new ((T*) p) T(val); }
  void                   destroy(pointer p) { p->~T(); }

  size_type             max_size() const { return size_t(-1); }

  template <class U>
  struct rebind { typedef AlignedAllocator<N, U> other; };

  template <class U>
  AlignedAllocator(const AlignedAllocator<N, U>&) {}

  template <class U>
  AlignedAllocator& operator=(const AlignedAllocator<N, U>&) { return *this; }
};
/* ---------------------------------------------------------------------- */


}

#endif
