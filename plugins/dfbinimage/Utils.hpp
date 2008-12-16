/************************************************************************

VirusCDR Utils.hpp

Copyright (C) 2007 Virus
Copyright (C) 2002 mooby

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

************************************************************************/

#pragma warning(disable:4786)

#ifndef UTILS_HPP
#define UTILS_HPP

#include <sstream>
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/filename.H>

#ifndef _FLU_
#include <FL/Fl_File_Chooser.H>
#endif
#ifdef _FLU_
#include <FLU/Flu_File_Chooser.h>
#endif

#include <stdlib.h>
#ifdef _WINDOWS
#include <windows.h>
#endif

/* 
   This file is for any utility function or constant that has use outside the scope of any
   single file
*/

// specifies how you want the MSF time formatted
// msf/fsm -> if time is specified in minute, second, frame or backwards
// int/bcd -> integer or binary coded decimal format
enum TDTNFormat
{
   msfint,
   fsmint,
   fsmbcd,
   msfbcd
};

// a quick way to tell which API set is being used
enum EMUMode
{
   psemu,
   fpse
};

// converts uchar in c to BCD character
#define intToBCD(c) (unsigned char)((c%10) | ((c/10)<<4))

// converts BCD number in c to uchar
#define BCDToInt(c) (unsigned char)((c & 0x0F) + 10 * ((c & 0xF0) >> 4))

static const std::string theUsualSuspects = 
  "Common image files (*.{bz,bz.index,Z,Z.table,bin,bwi,img,iso})";

// CD time constants
static const unsigned int secondsPerMinute = 60;
static const unsigned int framesPerSecond = 75;
static const unsigned int framesPerMinute = framesPerSecond * secondsPerMinute;
static const unsigned int bytesPerFrame = 2352;
static const unsigned int bytesPerSecond = bytesPerFrame * framesPerSecond;
static const unsigned int bytesPerMinute = bytesPerSecond * secondsPerMinute;

inline void moobyMessage(const std::string& message)
{
   Fl::scheme("plastic");
   fl_message("%s", message.c_str());
#ifdef __LINUX__
   Fl::wait();
   Fl::flush();
#endif
}

inline void moobyMessage(const char* message)
{
   Fl::scheme("plastic");
   fl_message("%s", message);
#ifdef __LINUX__
   Fl::wait();
   Fl::flush();
#endif
}

inline char* moobyFileChooser(const char* message, const char* filespec, 
                              const std::string& last = std::string())
{
   char* toReturn;
   if (last.size())
#ifdef _FLU_
      toReturn = (char*)flu_file_chooser(message, filespec, last.c_str());
   else
      toReturn = (char*)flu_file_chooser(message, filespec, NULL);
#else 
      toReturn = (char*)fl_file_chooser(message, filespec, last.c_str());
   else
      toReturn = (char*)fl_file_chooser(message, filespec, NULL);
#endif


#ifdef __LINUX__
   Fl::wait();
   Fl::flush();
#endif
   return toReturn;
}

inline int moobyAsk(const char* message)
{
//   int i = fl_ask(message);
   int i = fl_choice(message, fl_no, fl_yes, 0);
#ifdef __LINUX__
   Fl::wait();
   Fl::flush();
#endif
   return i;
}

// since most binary data is stored little endian, this flips
// the data bits for big endian systems
template <class T>
inline void flipBits(T& num)
{
#if 0
   char* front = (char*)&num;
   char* end = front + sizeof(T) - 1;
   char t;
   while (front < end)
   {
      t = *front;
      *front = *end;
      *end = t;
      front++;
      end--;
   }
#endif
}

// given a string and a number, returns the ' ' delimited word
// at position num
inline std::string word(const std::string& str, const unsigned long num)
{
   if (str == std::string())
      return str;

   unsigned long i;
   std::string::size_type startpos = 0;
   std::string::size_type endpos = 0;

   for(i = 0; i < num; i++)
   {
      startpos = str.find_first_not_of(' ', endpos);
      if (startpos == std::string::npos)
         return std::string();

      endpos = str.find_first_of(' ', startpos);
      if (endpos == std::string::npos)
      {
         endpos = str.size();
      }
   }
   return str.substr(startpos, endpos - startpos);
}

// turns a string into its lowercase...
inline std::string tolcstr(const std::string& s)
{
   std::string::size_type i;
   std::string toReturn(s);
   for (i = 0; i < s.size(); i++)
      toReturn[i] = tolower(s[i]);
   return toReturn;
}

// tries to match the extension ext to the end of the file name
inline bool extensionMatches(const std::string& file, const std::string& ext)
{
   if (file.size() < ext.size()) return false;
   return (tolcstr(file.substr(file.size() - ext.size())) == ext);
}

// returns what the name of the program running this plugin is.
inline std::string getProgramName(void)
{
   std::string toReturn;

#ifdef _WINDOWS
   char buf[1024];
   if (GetModuleFileName(NULL, (char*)&buf, 1024) == 0)
   {
      // i hope this never happens....  we'll just make an assumption ;)
      toReturn = "epsxe";
   }
   else
   {
      toReturn = buf;
   }
#else // for the UNIXes
   char* buf = getenv("_");
   if (buf == NULL)
   {
      toReturn = "pcsx";
   }
   else
   {
      toReturn = buf;
   }
#endif

	// strips all the useless crud from the retuned string
   toReturn = fl_filename_name(toReturn.c_str());
   std::string::size_type i;
   i = toReturn.find_last_of('.');
   if (i != std::string::npos)
      toReturn.erase(i);
   for (i = 0; i < toReturn.size(); i++)
      toReturn[i] = tolower(toReturn[i]);
   return toReturn;
}

#endif
