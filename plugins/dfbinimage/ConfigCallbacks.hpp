/************************************************************************

ConfigCallbacks.hpp

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

#ifndef CONFIG_CALLBACKS_HPP
#define CONFIG_CALLBACKS_HPP

#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Progress.H>

void bzCompress(Fl_Button*, void*);
void bzDecompress(Fl_Button*, void*);
void zCompress(Fl_Button*, void*);
void zDecompress(Fl_Button*, void*);
void CDDAVolume(Fl_Value_Slider*, void*);
void cacheSize(Fl_Value_Slider*, void*);
void newCaching(Fl_Check_Button*, void*);
void subEnable(Fl_Check_Button*, void*);
void repeatAllCDDA(Fl_Check_Button*, void* val = NULL);
void repeatOneCDDA(Fl_Check_Button*, void* val = NULL);
void playOneCDDA(Fl_Check_Button*, void* val = NULL);
void configOK(Fl_Return_Button*, void*);
void chooseAutorunImage(Fl_Button*, void*);
void clearAutorunImage(Fl_Button*, void*);

#endif
