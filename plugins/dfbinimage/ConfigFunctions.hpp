/************************************************************************

ConfigFunctions.hpp

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

#ifndef CONFIG_FUNCTIONS_HPP
#define CONFIG_FUNCTIONS_HPP

#include "FL/Fl_Radio_Round_Button.H"
#include "ConfigCallbacks.hpp"
#include "Utils.hpp"

// based on source generated from FLUID.
// this is the window that pops up when you use the Configure option

class ConfigWindow
{
public:
	ConfigWindow(){}
   void makeWindow();
   void show() { w->show(); }
   Fl_Window* w;
   Fl_Radio_Round_Button* repeatAllButton;
   Fl_Radio_Round_Button* repeatOneButton;
   Fl_Radio_Round_Button* playOneButton;
   Fl_Box* autorunBox;
   ~ConfigWindow(){ w->hide(); w->redraw(); delete w;}
};

class RunConfig
{
public:
   RunConfig() 
   {
	Fl::scheme("plastic");
	w.makeWindow();
	w.show();
	Fl::run();
#ifdef __LINUX__
      Fl::wait(); 
      Fl::flush(); 
#endif
   }

private:
   ConfigWindow w;
};

#endif
