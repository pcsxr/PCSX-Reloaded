/************************************************************************

PS2Open.cpp

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

#include "externs.h"
#include "CDTime.hpp"
#include "CDInterface.hpp"

extern CDInterface* theCD;

s32  CALLBACK CDVDinit()
{
  return CDRinit();
}

s32  CALLBACK CDVDopen()
{
  return CDRopen();
}

void CALLBACK CDVDclose()
{
  CDRclose();
}

void CALLBACK CDVDshutdown()
{
  CDRshutdown();
}

s32  CALLBACK CDVDreadTrack(cdvdLoc *Time)
{
   CDTime now((unsigned char*)Time, msfint);
   theCD->moveDataPointer(now);
   return 0;
}

u8*  CALLBACK CDVDgetBuffer()
{
  return (u8*)theCD->readDataPointer();
}

s32  CALLBACK CDVDgetTN(cdvdTN *Buffer)
{
   return CDRgetTN((unsigned char*)Buffer);
}

s32  CALLBACK CDVDgetTD(u8 Track, cdvdLoc *Buffer)
{
   return CDRgetTD(Track, (unsigned char*)Buffer);
}

