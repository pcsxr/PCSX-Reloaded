/************************************************************************

Preferences.cpp

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

#include "Preferences.hpp"

Preferences::Preferences()
   : initialized(false)
{
   this->open();
}

Preferences::~Preferences()
{	
}

void Preferences::write()
{
#ifdef WIN32
   HKEY myKey;
   DWORD myDisp;

   RegCreateKeyEx(HKEY_CURRENT_USER,"Software\\Vision Thing\\PSEmu Pro\\CDR\\MoobyCDR",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&myKey,&myDisp);

   std::map<std::string, std::string>::iterator itr = prefsMap.begin();
   while (itr != prefsMap.end())
   {
      RegSetValueEx(myKey,(itr->first).c_str(),0,REG_SZ,(LPBYTE) (itr->second).c_str(),itr->second.size() + 1);
      itr++;
   }
   RegCloseKey(myKey);

#else
   Fl_Preferences app(Fl_Preferences::USER, "Vision Thing", "PSEmu Pro/CDR" );
   Fl_Preferences myPrefs(app, "MoobyCDR");

   std::map<std::string, std::string>::iterator itr = prefsMap.begin();

   while (itr != prefsMap.end())
   {
      myPrefs.set((itr->first).c_str(), (itr->second).c_str());
      itr++;
   }
#endif
}

void Preferences::open()
{
   if (!initialized)
   {
      initialized = true;
      allPrefs.push_back(volumeString);
      allPrefs.push_back(repeatString);
      allPrefs.push_back(autorunString);
      allPrefs.push_back(lastrunString);
      allPrefs.push_back(cacheSizeString);
      allPrefs.push_back(cachingModeString);
      allPrefs.push_back(subEnableString);
   
      const size_t bufSize = 1024;
      char* buffer = new char[bufSize];
   
      // win32 code kindly pilfered from PEOPS =)
#ifdef WIN32
      HKEY myKey;
      DWORD type;
      DWORD size;
   
      if (RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\Vision Thing\\PSEmu Pro\\CDR\\MoobyCDR",0,KEY_ALL_ACCESS,&myKey)==ERROR_SUCCESS)
      {
         std::list<std::string>::iterator itr = allPrefs.begin();
         while (itr != allPrefs.end())
         {      
            size = bufSize;
            type = REG_SZ;
            if(RegQueryValueEx(myKey,(*itr).c_str(),0,&type,(LPBYTE)buffer,&size)==ERROR_SUCCESS)
               prefsMap[*itr] = std::string(buffer);
            else
               prefsMap[*itr] = std::string("");
            itr++;
         }
   
         RegCloseKey(myKey);
      }
   
#else
      Fl_Preferences app(Fl_Preferences::USER, "Vision Thing", "PSEmu Pro/CDR" );
      Fl_Preferences myPrefs(app, "MoobyCDR");
   
      std::list<std::string>::iterator itr = allPrefs.begin();
      while (itr != allPrefs.end())
      {
         myPrefs.get((*itr).c_str(), buffer, "");
         prefsMap[*itr] = std::string(buffer);
         itr++;
      }
#endif

      delete [] buffer;
   }
}
