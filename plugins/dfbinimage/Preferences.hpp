/************************************************************************

Preferences.hpp

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

#ifndef PREFERENCES_HPP
#define PREFERENCES_HPP

#include <map>
#include <list>
#include <string>
#include <FL/Fl_Preferences.H>

static char* repeatString = "repeat";
static char* volumeString = "volume";
static char* autorunString = "autorun";
static char* lastrunString = "lastrun";
static char* cacheSizeString = "cachesize";
static char* cachingModeString = "cachemode";
static char* subEnableString = "subenable";

// these are the repeat mode strings
static char* repeatAllString = "repeatAll";
static char* repeatOneString = "repeatOne";
static char* playOneString = "playOne";
static char* oldCachingString = "old";
static char* newCachingString = "new";

// a wrapper for preference information.
class Preferences
{
public:
   Preferences();
   ~Preferences();
	void write();

private:
	void open();
	bool initialized;

public:
   std::map<std::string, std::string> prefsMap;
   std::list<std::string> allPrefs;
};

#endif
