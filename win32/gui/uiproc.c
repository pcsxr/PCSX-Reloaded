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

void On_File_RunCd()
{
}

void On_File_RunCdBios()
{
}

void On_File_RunExe()
{
}

void On_File_Exit()
{
   gQuitMsg = 0;
   DestroyWindow(hWndMain);
   hWndMain = NULL;
}

void On_File_LoadSlot(int iSlot)
{
}

void On_File_SaveSlot(int iSlot)
{
}

void On_Emulator_Run()
{
}

void On_Emulator_Reset()
{
}

void On_Config_PluginAndBios()
{
   ShowConfigDialog();
}

void On_Config_Graphics()
{
}

void On_Config_Sound()
{
}

void On_Config_Cd()
{
}

void On_Config_Controllers()
{
}

void On_Config_NetPlay()
{
   ShowNetPlayDialog();
}

void On_Config_Cpu()
{
   ShowCPUConfigDialog();
}

void On_Config_Memcards()
{
   ShowMemcardDialog();
}

void On_Help_About()
{
   ShowAboutDialog();
}
