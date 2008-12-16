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

static BOOL CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
   case WM_INITDIALOG:
      return TRUE;

   case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      case IDC_CLOSE:
         EndDialog(hDlg, 0);
         return TRUE;
      }
      break;

   case WM_CLOSE:
      EndDialog(hDlg, 0);
      return TRUE;
   }

   return FALSE;
}

void ShowAboutDialog()
{
   DialogBox(hModuleInst, MAKEINTRESOURCE(IDD_ABOUT), hWndMain, DlgProc);
}
