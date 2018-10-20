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
   2017.11.10 jjr Added some error checking to look for corrupt records.
                  These would be records that have a length less than
                  the size of there header.
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
#include <stdlib.h>

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
   Reader           (char const *filename);
   virtual ~Reader () { return; }
   virtual int      open () = 0;
   virtual void     report (int err) = 0;
   virtual ssize_t  read (HeaderFragmentUnpack *header) = 0;
   virtual ssize_t  read (uint64_t *data, int n64, ssize_t nbytes) = 0;
   virtual int      close () = 0;

   enum class FileType
   {
      Reserved  = 0,  /*!< Reserved                                       */
      Binary    = 1,  /*!< Binary file                                    */
      TextGdb64 = 2   /*!< Text file from a GDB hex dump                  */
   };


protected:
   char const *m_filename; /*!< The file name                             */
};
/* ---------------------------------------------------------------------- */
/* INTERFACE:Reader                                                       */
/* ====================================================================== */




/* ====================================================================== */
/* INTERFACE:ReaderBinary                                                 */
/* ---------------------------------------------------------------------- *//*!

  \class ReaderBinay
  \brief Read a binary file
                                                                          */
/* ---------------------------------------------------------------------- */
class ReaderBinary : public Reader
{
public:
   ReaderBinary  (char const *filename);
  ~ReaderBinary  ();
   virtual int      open ();
   virtual void   report (int err);
   virtual ssize_t  read (HeaderFragmentUnpack *header);
   virtual ssize_t  read (uint64_t *data, int n64, ssize_t nbytes);
   virtual int     close ();

private:
   int               m_fd; /*!< The file descriptor                       */
};
/* ---------------------------------------------------------------------- */
/* INTERFACE:ReaderBinary                                                 */
/* ====================================================================== */




/* ====================================================================== */
/* INTERFACE:ReaderTextGdb64                                              */
/* ---------------------------------------------------------------------- */
class ReaderTextGdb64 : public Reader
{
public:
   ReaderTextGdb64 (char const *filename);
  ~ReaderTextGdb64 ();
   virtual int      open   ();
   virtual void   report   (int err);
   virtual ssize_t  read   (HeaderFragmentUnpack *header);
   virtual ssize_t  read   (uint64_t *data, int n64, ssize_t nbytes); 
   virtual int     close   ();

   ssize_t get (uint64_t *val);

private:

   int             m_cur;  /*!< Current index                             */
   int             m_len;  /*!< Current length                            */
   uint64_t  m_values[2];  /*!< the buffer of values                      */
   FILE          *m_file;  /*!< The file handle                           */
   char    m_linebuf[80];  /*!< The line buffer                           */

};   
/* ---------------------------------------------------------------------- */
/* INTERFACE: ReaderTestGdb64                                             */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION: Reader                                                 */
/* ---------------------------------------------------------------------- *//*!

  \brief  Sets the file to be opened, but does not open the file

  \param[in] filename  The name of the file to open
                                                                          */
/* ---------------------------------------------------------------------- */
inline Reader::Reader (char const *filename) :
   m_filename (filename)
{
   return;
}
/* ---------------------------------------------------------------------- */
/* IMPLEMENTATION: Reader                                                 */
/* ====================================================================== */




/* ====================================================================== */
/* IMPLEMENTATION: ReaderBinary                                           */
/* ---------------------------------------------------------------------- *//*!

  \brief  Sets the file to be opened, but does not open the file

  \param[in] filename  The name of the file to open
                                                                          */
/* ---------------------------------------------------------------------- */
inline ReaderBinary::ReaderBinary (char const *filename) :
   Reader (filename),
   m_fd   (-1)
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
inline int ReaderBinary::open ()
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
inline void ReaderBinary::report (int err)
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

  \brief Destructor for binary files
                                                                          */
/* ---------------------------------------------------------------------- */
ReaderBinary::~ReaderBinary ()
{
   if (m_fd >= 0) close ();
   return;
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
inline ssize_t ReaderBinary::read (HeaderFragmentUnpack *header)
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
inline ssize_t ReaderBinary::read (uint64_t *data, int n64, ssize_t nbytes)
{
   ssize_t recSize = n64 * sizeof (uint64_t);
   if (recSize < nbytes)
   {
      printf ("Error: Record size %u < header size (%u)\n"
              "       This generally indicates a corrupt record\n",
              (unsigned)recSize, (unsigned)nbytes);
      return -1;
   }

   unsigned toRead = recSize - nbytes;
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
                       "       returned %d bytes, should have returned %u errno = %d\n",
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
inline int ReaderBinary::close ()
{
   int iss = ::close (m_fd);
   if (iss == 0)
   {
      m_fd = -1;
   }

   return iss;
}
/* ---------------------------------------------------------------------- */
/* IMPLEMENTATION: ReaderBinary                                           */
/* ====================================================================== */





/* ====================================================================== */
/* IMPLEMENTATION: ReaderTextGdb64                                        */
/* ---------------------------------------------------------------------- *//*!

  \brief  Sets the file to be opened, but does not open the file

  \param[in] filename  The name of the file to open
                                                                          */
/* ---------------------------------------------------------------------- */
inline ReaderTextGdb64::ReaderTextGdb64 (char const *filename) :
   Reader (filename),
   m_cur  (0),
   m_len  (0),
   m_file (0)
{

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Destructor for GDB text dump files
                                                                          */
/* ---------------------------------------------------------------------- */
ReaderTextGdb64::~ReaderTextGdb64 ()
{
   if (m_file) close ();
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief  Opens the previously specified file
  \retval == 0, OKAY
  \retval != 0, standard Unix error code
                                                                          */
/* ---------------------------------------------------------------------- */
inline int ReaderTextGdb64::open ()
{
   m_file= ::fopen (m_filename, "r");
   if (m_file == 0) return 0;
   else             return errno;
}
/* ---------------------------------------------------------------------- */



ssize_t ReaderTextGdb64::get (uint64_t *val)
{
   if (m_cur == m_len)
   {
      char *line = fgets (m_linebuf, sizeof (m_linebuf), m_file);
      size_t nbytes = strlen (line);
      if (line == 0)
      {
         if (errno)
         {
            fprintf (stderr, 
                     "Error: reading header\n"
                     "        errno = %d\n",
                     errno);
            return 0;
         }
      }

      char *next;
      uint64_t adr __attribute__ ((unused));
      adr          = strtoull (line, &next, 0);
      next        += 2;
      m_values[0]  = strtoull (next, &next, 0);
      if (next != line + nbytes)
      {
         m_values[1] = strtoull (next, &next, 0);
         m_len       = 2;
      }
      else
      {
         m_len       = 1;
      }
         
      *val = m_values[0];
      m_cur = 1;
   }
   else
   {
      *val = m_values[1];
      m_cur = 2;
   }

   return 8;
}


/* ---------------------------------------------------------------------- *//*!

   \brief Reports the error to stderr

   \param[in] err The standard Unix error number to report
                                                                          */
/* ---------------------------------------------------------------------- */
inline void ReaderTextGdb64::report (int err)
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
inline ssize_t ReaderTextGdb64::read (HeaderFragmentUnpack *header)
{
   ssize_t nbytes = get (reinterpret_cast<uint64_t *>(header));
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
inline ssize_t ReaderTextGdb64::read (uint64_t *data, int n64, ssize_t nbytes)
{
   data += nbytes / sizeof (uint64_t);
   for (int idx = 0; idx < n64; idx++)
   {
      ssize_t got = get (data + idx);
      if (got != 8) return 0;
   }

   return n64 * sizeof (uint64_t);

}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

   \brief  Closes the file
   \return The status return from the standard close
                                                                          */
/* ---------------------------------------------------------------------- */
inline int ReaderTextGdb64::close ()
{
   int iss = ::fclose (m_file);
   if (iss == 0)
   {
      m_file = 0;
   }

   return iss;
}
/* ---------------------------------------------------------------------- */
/* IMPLEMENTATION: ReaderTextGdb64                                        */
/* ====================================================================== */



/* ====================================================================== */
/* IMPLEMENTATION  ReaderFactory                                          */
/* ---------------------------------------------------------------------- */
static inline Reader &ReaderCreate (char const       *filename, 
                                    Reader::FileType  filetype)
{
   if      (filetype == Reader::FileType::Binary)
   {
      Reader *reader = new ReaderBinary    (filename);
      return  *reader;
   }
   else if (filetype == Reader::FileType::TextGdb64)
   {
      Reader *reader = new ReaderTextGdb64 (filename);
      return *reader;
   }
   else
   {
      fprintf (stderr, 
               "Error::nknown reader type %d\n", 
               static_cast<int>(filetype));
      exit (-1);
   }
}
/* ---------------------------------------------------------------------- */
/* IMPLEMENTATION  ReaderFactory                                          */
/* ====================================================================== */
#endif
