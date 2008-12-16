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

#ifndef MAIN_H
#define MAIN_H

#ifdef _MSC_VER
#pragma once
#endif

#include <windows.h>

#include "psxcommon.h"
#include "psemu_plugin_defs.h"
#include "system.h"

#include "cfg.h"
#include "res/resource.h"
#include "uiproc.h"
#include "aboutdlg.h"
#include "configdlg.h"
#include "cpuconfigdlg.h"
#include "memcarddlg.h"
#include "netplaydlg.h"

extern HWND                hWndMain;
extern HINSTANCE           hModuleInst;
extern int                 gQuitMsg;

#endif
