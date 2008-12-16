/************************************************************************

TrackParser.hpp

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


#ifndef TRACKINFO_HPP
#define TRACKINFO_HPP

#include "CDTime.hpp"

#include <iomanip>

// This class stores all the information we need to know about one track.
// It's just a glorified struct.
class TrackInfo
{
public:
   TrackInfo() : trackNumber(0) {}

   TrackInfo(const CDTime& tl)
      : trackNumber(0), trackStart(0, CDTime::abFrame), trackLength(tl) {}

   inline friend std::ostream& operator<<(std::ostream& o, const TrackInfo& ti)
   {
      o << std::setw(2) << ti.trackNumber << ' ' << ti.trackStart.getMSF() 
        << ' ' << ti.trackEnd.getMSF() << ' ' << ti.trackLength.getMSF() << std::endl;
      return o;
   }

   unsigned long trackNumber;
   CDTime trackStart;
   CDTime trackEnd;
   CDTime trackLength;
};

#endif
