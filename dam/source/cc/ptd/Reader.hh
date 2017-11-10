// -*-Mode: C++;-*-

#ifndef PTD_READER_HH
#define PTD_READER_HH

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     Reader.hh
 *  @brief    Simple interface class to read RCE data fragments from
 *            a file
 *
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
\* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2017.11.10 jjr Replaced fprintf to stderr to printf.  Using less, a
                  common thing to do, causes the output to be intermingled
                  making it hard to read.
   2017.10.05 jjr Added in data reader to ensure all the data gets read.
   2017.07.27 jjr Created
  
\* ---------------------------------------------------------------------- */



#include "dam/HeaderFragmentUnpack.hh"

#include <cstdio>
#include <cinttypes>
#include <cerrno>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


/* ====================================================================== */
/* INTERFACE: Reader                                                      */
/* ---------------------------------------------------------------------- *//*!

  \class Reader
  \brief Simple interface to read fragment records from a file
                                                                          */
/* ---------------------------------------------------------------------- */
class Reader
{
public:
   Reader        (char const *filename);
   int      open ();
   void   report (int err);
   ssize_t  read (HeaderFragmentUnpack *header);
   ssize_t  read (uint64_t *data, int n64, ssize_t nbytes);
   int     close ();

private:
   int               m_fd; /*!< The file descriptor                       */
   char const *m_filename; /*!< The file name                             */
};
/* ---------------------------------------------------------------------- */
/* INTERFACE: Reader                                                      */
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION: Reader                                                 */
/* ---------------------------------------------------------------------- *//*!

  \brief  Sets the file to be opened, but does not open the file

  \param[in] filename  The name of the file to open
                                                                          */
/* ---------------------------------------------------------------------- */
inline Reader::Reader (char const *filename) :
   m_fd       (-1),
   m_filename (filename)
{
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Opens the previously specified file
  \retval == 0, OKAY
  \retval != 0, standard Unix error code
                                                                          */
/* ---------------------------------------------------------------------- */
inline int Reader::open ()
{
   m_fd = ::open (m_filename, O_RDONLY);
   if (m_fd > 0) return 0;
   else          return errno;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief Reports the error to stderr

   \param[in] err The standard Unix error number to report
                                                                          */
/* ---------------------------------------------------------------------- */
inline void Reader::report (int err)
{
   if (err)
   {
      printf ("Error : could not open file: %s\n"
               "Reason: %d -> %s\n", 
               m_filename,
               err, strerror (err));
   }
   else
   {
      printf ("Processing: %s\n", 
               m_filename);
   }
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief     Reads, what should be a fragment header from the file stream
   \return    The status return from the standard read

   \param[in] header The header to populate

   \note
    This only reads the correct number of bytes into \a header, it does
    not check that this is, in fact, a header.
                                                                          */
/* ---------------------------------------------------------------------- */
inline ssize_t Reader::read (HeaderFragmentUnpack *header)
{
   ssize_t nbytes = ::read (m_fd, header, sizeof (*header));
   
   if (nbytes != sizeof (*header))
   {
      if (nbytes == 0) return 0;
      
      printf ("Error: reading header\n"
               "       returned %d bytes, should have returned %d errno = %d\n",
               (int)nbytes, (int)sizeof (*header), errno);
   }

   return nbytes;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief  Reads the \e rest of the data fragment into the specified buffer
   \return The status return from the standard read

   \param[in]   data  The buffer to receive the data
   \param[in] nbytes  The number of bytes to read
                                                                          */
/* ---------------------------------------------------------------------- */
inline ssize_t Reader::read (uint64_t *data, int n64, ssize_t nbytes)
{
   unsigned toRead = n64 * sizeof (uint64_t ) - nbytes;
   unsigned left   = toRead;

   uint8_t *buf =  reinterpret_cast<decltype (buf)>(data)  + nbytes;
   while (1)
   {
      nbytes = ::read (m_fd, buf, left);
   
      if (nbytes != left)
      {
         // Done or error
         if (nbytes <= 0)
         {
            if (nbytes < 0)
            {
               printf ("Error: reading data\n"
                       "       returned %d bytes, should have returned %d errno = %d\n",
                        (int)nbytes, toRead, errno);
            }

            return nbytes;
         }

         //printf ("Read again left = %u\n", left);

         // More to read
         left -= nbytes;
         data += nbytes;
      }
      else 
      {
         return toRead;
      }
   }
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief  Closes the file
   \return The status return from the standard close
                                                                          */
/* ---------------------------------------------------------------------- */
inline int Reader::close ()
{
   int iss = ::close (m_fd);
   if (iss == 0)
   {
      m_fd = -1;
   }

   return iss;
}
/* ---------------------------------------------------------------------- */
/* IMPLEMENTATION: Reader                                                 */
/* ====================================================================== */


#endif
