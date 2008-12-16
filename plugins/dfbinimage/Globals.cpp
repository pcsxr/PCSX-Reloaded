/************************************************************************

Globals.cpp

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
#include "Utils.hpp"
#include "CDInterface.hpp"
#include "Preferences.hpp"

/** global data for the plugin **/

// the cd data
CDInterface* theCD = NULL;
std::string passedfile = "";
Preferences prefs;

// a return code
int rc = 0;

// the format of the TD and TN calls
TDTNFormat tdtnformat = msfint;

// the name of the emulator that's using the plugin
std::string programName;

// whether it's using the psemu or fpse interface
EMUMode mode = psemu;
