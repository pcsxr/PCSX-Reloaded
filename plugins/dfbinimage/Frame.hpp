/************************************************************************

Frame.hpp

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

#ifndef FRAME_HPP
#define FRAME_HPP

#include "Utils.hpp"

// one frame of CD data, 2352 bytes
class Frame
{
public:
      // note that the data is uninitialized w/default constructor
   Frame()
   {
      data = new unsigned char[bytesPerFrame];
   }

   Frame(const unsigned char* d)
   {
      data = new unsigned char[bytesPerFrame];
      memcpy(data, d, bytesPerFrame);
   }

   Frame(const Frame& r)
   {
      data = new unsigned char[bytesPerFrame];
      memcpy(data, r.data, bytesPerFrame);
   }

   ~Frame() { delete [] data; }

   Frame& operator=(const Frame& r)
   {
      memcpy(data, r.data, bytesPerFrame);
      return *this;
   }

   Frame& operator=(const unsigned char* buf)
   {
      memcpy(data, buf, bytesPerFrame);
      return *this;
   }

   Frame& operator=(const char* buf)
   {
      memcpy(data, buf, bytesPerFrame);
      return *this;
   }
   
   int operator==(const Frame& r)
   {
      return memcmp(data, r.data, bytesPerFrame);
   }

   unsigned char* operator*() const
   {
      return data;
   }

private:
   unsigned char* data;
};



#endif
