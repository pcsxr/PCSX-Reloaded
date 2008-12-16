/************************************************************************

ConfigFunctions.cpp

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
#include <iostream>
#include "Utils.hpp"
#include "Preferences.hpp"
#include "ConfigFunctions.hpp"

extern Preferences prefs;

void ConfigWindow::makeWindow()
{
  { 
    Fl_Double_Window* o = new Fl_Double_Window(250, 500, "VirusCDR Configuration");
    w = o;
    { Fl_Radio_Round_Button* o = new Fl_Radio_Round_Button(20,20,220,20, "Repeat all CDDA tracks");
      o->value(prefs.prefsMap[repeatString] == repeatAllString);
      o->callback((Fl_Callback*)repeatAllCDDA, this);
      repeatAllButton = o;
    }
    { Fl_Radio_Round_Button* o = new Fl_Radio_Round_Button(20,40,220,20, "Repeat one CDDA track");
      o->value(prefs.prefsMap[repeatString] == repeatOneString);
      o->callback((Fl_Callback*)repeatOneCDDA, this);
      repeatOneButton = o;
    }
    { Fl_Radio_Round_Button* o = new Fl_Radio_Round_Button(20,60,220,20, "Play one CDDA track and stop");
      o->value(prefs.prefsMap[repeatString] == playOneString);
      o->callback((Fl_Callback*)playOneCDDA, this);
      playOneButton = o;
    }
    { Fl_Value_Slider* o = new Fl_Value_Slider(20, 90, 210, 25, "CDDA Volume");
      o->type(1);
      o->range(0, 100);
      o->step(1);
      o->value(atof(prefs.prefsMap[volumeString].c_str()));
      o->callback((Fl_Callback*)CDDAVolume);
    }
	new Fl_Box(5, 150, 250, 25, "bz/track compression");
    //new Fl_Box(5, 280, 250, 25, "Z/table compression");
    { Fl_Button* o = new Fl_Button(20, 180, 95, 25, "Compress");
      o->callback((Fl_Callback*)bzCompress);
    }
    { Fl_Button* o = new Fl_Button(130, 180, 95, 25, "Decompress");
      o->callback((Fl_Callback*)bzDecompress);
    }/*
    { Fl_Button* o = new Fl_Button(20, 305, 95, 25, "Compress");
      o->callback((Fl_Callback*)zCompress);
    }
    { Fl_Button* o = new Fl_Button(130, 305, 95, 25, "Decompress");
      o->callback((Fl_Callback*)zDecompress);
    }*/
    { 
      Fl_Box* o;
      if (prefs.prefsMap[autorunString] == std::string(""))
         o = new Fl_Box(0, 340, 250, 25, "No autorun image selected");   
      else
         o = new Fl_Box(0, 340, 250, 25, prefs.prefsMap[autorunString].c_str());
      autorunBox = o;
    }
    {
      Fl_Button* o = new Fl_Button(20, 370, 200, 25, "Choose an autorun image");
      o->callback((Fl_Callback*)chooseAutorunImage, this);
    }
    {
      Fl_Button* o = new Fl_Button(20, 405, 200, 25, "Clear the autorun image");
      o->callback((Fl_Callback*)clearAutorunImage, this);
    }
    { Fl_Return_Button* o = new Fl_Return_Button(165, 465, 80, 25, "OK");
      o->callback((Fl_Callback*)configOK, this);
    }
    { Fl_Check_Button* o = new Fl_Check_Button(20,220,220,20, "Read subchannel data");
      o->value(prefs.prefsMap[subEnableString] != std::string());
      o->callback((Fl_Callback*)subEnable);
    }
    { Fl_Check_Button* o = new Fl_Check_Button(20,255,220,20, "Alternative caching mode");
      o->value(prefs.prefsMap[cachingModeString] == newCachingString);
      o->callback((Fl_Callback*)newCaching);
    }
    { Fl_Value_Slider* o = new Fl_Value_Slider(20, 280, 210, 25, "Cache size (in Megabytes)");
      o->type(1);
      o->range(1, 700);
      o->step(1);
      o->value(atoi(prefs.prefsMap[cacheSizeString].c_str())/446);
      o->callback((Fl_Callback*)cacheSize);
    }
    o->end();
  }
}



/** configure plugin external functions **/

long CALLBACK CDRconfigure(void)
{
	RunConfig();
	prefs.write();
	return 0;
}


int	CD_Configure(UINT32 *par)
{
	RunConfig();
	prefs.write();
	return 0;
}

void CALLBACK CDVDconfigure()
{
   CDRconfigure();
}
