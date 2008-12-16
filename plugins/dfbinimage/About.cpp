/************************************************************************

About.cpp

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


#include "defines.h"
#include "externs.h"
#include "Utils.hpp"
#include <string.h>

//  About.cpp
//  has the config, about, and test functions and related structs

//  lots of this code was taken from PEOPS.  THANK YOU!!!!

const  unsigned char version	= 1;	  // do not touch - library for PSEmu 1.x

#define REVISION  2
#define BUILD 8

#ifdef _DEBUG
#ifdef SUPER_DEBUG
static char *libraryName		= "Mooby2 cd disk image super debug driver";
#else
static char *libraryName		= "Mooby2 cd disk image debug driver";
#endif
#else
static char *libraryName		= "BIN Image Reader";
#endif

//static char *PluginAuthor		= "Mooby";

static FPSEAbout aboutMooby = {FPSE_CDROM, 
   BUILD, 
   REVISION, 
   FPSE_OK, 
   "mooby", 
#ifdef _DEBUG
#ifdef SUPER_DEBUG
   "Mooby2 cd disk image super debug driver",
#else
   "Mooby2 cd disk image debug driver",
#endif
#else
   "Mooby2 cd disk image driver",
#endif
   "moo"};


/** PSX emu interface **/

// lots of this code was swiped from PEOPS.  thank you =)
char * CALLBACK PSEgetLibName(void)
{
 return libraryName;
}

unsigned long CALLBACK PSEgetLibType(void)
{
 return	PSE_LT_CDR;
}

unsigned long CALLBACK PSEgetLibVersion(void)
{
 return version<<16|REVISION<<8|BUILD;
}

void CALLBACK CDRabout(void)
{
   std::ostringstream out;
   out << libraryName;
   moobyMessage(out.str());
}

long CALLBACK CDRtest(void)
{
   moobyMessage("Of course it'll work.");
   return 0;
}

/** FPSE interface **/

void   CD_About(UINT32 *par)
{
   memcpy(par, &aboutMooby, sizeof(FPSEAbout));
}

/** PCSX2 interface **/

void CALLBACK CDVDabout()
{
   CDRabout();
}

s32  CALLBACK CDVDtest()
{
  return CDRtest();
}

u32   CALLBACK PS2EgetLibType(void)
{
   return PS2E_LT_CDVD;
}

u32   CALLBACK PS2EgetLibVersion(void)
{
   return PSEgetLibVersion();
}

char* CALLBACK PS2EgetLibName(void)
{
   return PSEgetLibName();  
}

