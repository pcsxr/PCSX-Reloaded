//
// Copyright (c) 2008, Wei Mingzhi <whistler@openoffice.org>
// All rights reserved.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "main.h"

// Init mem and plugins
int SysInit()
{
   return 0; // TODO
}

// Resets mem
void SysReset()
{
   // TODO
}

// Called on VBlank (to update i.e. pads)
void SysUpdate()
{
   // TODO
}

// Returns to the Gui
void SysRunGui()
{
   // TODO
}

// Close mem and plugins
void SysClose()
{
   // TODO
}

// Printf used by bios syscalls
void SysPrintf(char *fmt, ...)
{
#ifdef EMU_LOG
#ifndef LOG_STDOUT
   va_list list;
   char msg[512];

   va_start(list,fmt);
   vsprintf(msg,fmt,list);
   va_end(list);

   fprintf(emuLog, "%s", msg);
#endif
#endif
}

// Message used to print msg to users
void SysMessage(char *fmt, ...)
{
   va_list list;
   char tmp[512];

   va_start(list, fmt);
   vsprintf(tmp, fmt, list);
   va_end(list);

   MessageBoxA(NULL, (LPCSTR)tmp, "PCSX", 0);
}

// Loads Library
void *SysLoadLibrary(char *lib)
{
   return (void *)LoadLibraryA(lib);
}

// Gets previous error loading sysbols
const char *SysLibError()
{
   static char sz[256];

   DWORD dw = GetLastError();
   if (dw == 0)
      return NULL;

   sprintf(sz, "Error loading Symbol: %d", dw);
   return sz;
}

// Loads Symbol from Library
void *SysLoadSym(void *lib, char *sym)
{
   return (void *)GetProcAddress((HINSTANCE)lib, sym);
}

// Closes Library
void SysCloseLibrary(void *lib)
{
   FreeLibrary((HINSTANCE)lib);
}
