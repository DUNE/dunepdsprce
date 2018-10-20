#ifndef TPCWINDOW_HH
#define TPCWINDOW_HH

// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     TpcWindow.hh
 *  @brief    Captures the parameters of a TpcStream window
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
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2018.09.13 jjr Created
  
\* ---------------------------------------------------------------------- */


#include <dam/TpcStreamUnpack.hh>
#include <cstdio>


namespace TpcWindow
{
enum class Type
{
   Trimmed   = 0,
   Untrimmed = 1
};


/* ---------------------------------------------------------------------- */
template<enum TpcWindow::Type>
class Window
{
public:
   Window (TpcStreamUnpack const *tpcStream);

public:
   void print (char const *title)
   {
    printf ("%s: %6zu  %8.8" PRIx64 " %8.8" PRIx64 "\n",
            title, m_nticks, m_times[0], m_times[1]);
   }


   void print ();


public:
   uint64_t m_times[2];
   size_t     m_nticks;
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
template<> inline 
Window<TpcWindow::Type::Untrimmed>::Window (TpcStreamUnpack const *tpcStream)
{
   tpcStream->getRangeUntrimmed (&m_nticks,
                                 &m_times[0],
                                 &m_times[1]);


   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Print the trimmed and untrimmed times and ticks
                                                                          */
/* ---------------------------------------------------------------------- */
template<> inline
void TpcWindow::Window<TpcWindow::Type::Untrimmed>::print ()
{
   print (" Untrimmed");
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
template<>
inline
Window<TpcWindow::Type::Trimmed>::Window (TpcStreamUnpack const *tpcStream)
{
   tpcStream->getRange (&m_nticks,
                        &m_times[0],
                        &m_times[1]);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Print the trimmed and untrimmed times and ticks
                                                                          */
/* ---------------------------------------------------------------------- */
template<> inline
void Window<TpcWindow::Type::Trimmed>::print ()
{
   print ("   Trimmed");
   return;
}
/* ---------------------------------------------------------------------- */
}

#endif
