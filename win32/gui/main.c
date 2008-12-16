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

HWND                hWndMain;
HINSTANCE           hModuleInst;

int                 gQuitMsg = 0;

static HDC          hDC;
static HDC          hdcmem;
static HBITMAP      hBm;
static BITMAP       bm;

long WINAPI WndProc(HWND hWnd, UINT iMessage, UINT wParam, LONG lParam)
{
   PAINTSTRUCT     ps;

   switch (iMessage)
   {
   case WM_DESTROY:
      DeleteObject(hBm);
      DeleteDC(hdcmem);
      PostQuitMessage(gQuitMsg);
      break;

   case WM_CREATE:
      hBm = LoadBitmap(hModuleInst, MAKEINTRESOURCE(IDB_SPLASH));
      GetObject(hBm, sizeof(BITMAP), (LPVOID)&bm);
      hDC = GetDC(hWnd);
      hdcmem = CreateCompatibleDC(hDC);
      ReleaseDC(hWnd, hDC);
      break;

   case WM_PAINT:
      hDC = BeginPaint(hWnd, &ps);
      SelectObject(hdcmem, hBm);
      BitBlt(hDC, 0, 0, bm.bmWidth, bm.bmHeight, hdcmem, 0, 0, SRCCOPY);
      EndPaint(hWnd, &ps);
      break;

   case WM_COMMAND:
      switch (wParam)
      {
      case ID_HELP_ABOUTPCSX:
         On_Help_About();
         break;

      case ID_FILE_RUNCD:
         On_File_RunCd();
         break;

      case ID_FILE_RUNCDTHROUGHBIOS:
         On_File_RunCdBios();
         break;

      case ID_FILE_RUNEXE:
         On_File_RunExe();
         break;

      case ID_FILE_EXIT:
         On_File_Exit();
         break;

      case ID_LOAD_SLOT1:
      case ID_LOAD_SLOT2:
      case ID_LOAD_SLOT3:
      case ID_LOAD_SLOT4:
      case ID_LOAD_SLOT5:
         On_File_LoadSlot(wParam - ID_LOAD_SLOT1);
         break;

      case ID_LOAD_OTHER:
         On_File_LoadSlot(-1);
         break;

      case ID_SAVE_SLOT1:
      case ID_SAVE_SLOT2:
      case ID_SAVE_SLOT3:
      case ID_SAVE_SLOT4:
      case ID_SAVE_SLOT5:
         On_File_SaveSlot(wParam - ID_LOAD_SLOT1);
         break;

      case ID_SAVE_OTHER:
         On_File_LoadSlot(-1);
         break;

      case ID_EMULATOR_RUN:
         On_Emulator_Run();
         break;

      case ID_EMULATOR_RESET:
         On_Emulator_Reset();
         break;

      case ID_CONFIGURATION_PLUGINSANDBIOS:
         On_Config_PluginAndBios();
         break;

      case ID_CONFIGURATION_GRAPHICS:
         On_Config_Graphics();
         break;

      case ID_CONFIGURATION_SOUND:
         On_Config_Sound();
         break;

      case ID_CONFIGURATION_CD:
         On_Config_Cd();
         break;

      case ID_CONFIGURATION_CONTROLLERS:
         On_Config_Controllers();
         break;

      case ID_CONFIGURATION_NETPLAY:
         On_Config_NetPlay();
         break;

      case ID_CONFIGURATION_CPU:
         On_Config_Cpu();
         break;

      case ID_CONFIGURATION_MEMORYCARDS:
         On_Config_Memcards();
         break;
      }
      break;

   default:
      return DefWindowProc(hWnd, iMessage, wParam, lParam);
   }

   return 0;
}

BOOL InitWindowsClass(HINSTANCE hInstance)
{
   WNDCLASS c;

   c.cbClsExtra = 0;
   c.cbWndExtra = 0;
   c.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
   c.hCursor = LoadCursor(NULL, IDC_ARROW);
   c.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
   c.hInstance = hInstance;
   c.lpfnWndProc = WndProc;
   c.lpszClassName = "WndPcsx";
   c.lpszMenuName = MAKEINTRESOURCE(IDR_MAINMENU);
   c.style = 0;

   return RegisterClass(&c);
}

BOOL InitWindows(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hWnd = CreateWindowA("WndPcsx", "PCSX - PlayStation Emulator", WS_CAPTION | WS_POPUPWINDOW | WS_MINIMIZEBOX,
      CW_USEDEFAULT, 0, 350, 220, NULL, NULL, hInstance, NULL);

   if (!hWnd)
      return FALSE;

   hWndMain = hWnd;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void StartGui()
{
   MSG Message;

   InitWindowsClass(hModuleInst);

   if (!InitWindows(hModuleInst, SW_SHOWNORMAL))
   {
      SysMessage("Failed initializing main window!");
      return;
   }

   while (GetMessage(&Message, 0, 0, 0))
   {
      TranslateMessage(&Message);
      DispatchMessage(&Message);
   }

   switch (Message.wParam)
   {
   case 1:
      break;

   case 2:
      break;
   }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
   LPTSTR lpCmdLine, int nCmdShow)
{
   hModuleInst = hInstance;

   LoadConfig();
   StartGui();
   SaveConfig();

   return 0;
}
