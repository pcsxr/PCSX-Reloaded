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

void UpdateDialog(HWND hDlg)
{
   SendMessageA(GetDlgItem(hDlg, IDC_SLOT1_EDIT), WM_SETTEXT, 0,
      (LPARAM)Config.Mcd1);
   SendMessageA(GetDlgItem(hDlg, IDC_SLOT2_EDIT), WM_SETTEXT, 0,
      (LPARAM)Config.Mcd2);
}

void SaveToConfig(HWND hDlg)
{
   SendMessageA(GetDlgItem(hDlg, IDC_SLOT1_EDIT), WM_GETTEXT, 256,
      (LPARAM)Config.Mcd1);
   SendMessageA(GetDlgItem(hDlg, IDC_SLOT2_EDIT), WM_GETTEXT, 256,
      (LPARAM)Config.Mcd2);

   Config.Mcd1[255] = '\0';
   Config.Mcd2[255] = '\0';
}

static void OpenMemcardFile(HWND hDlg, int mcd)
{
   OPENFILENAME ofn;

   char szFileName[MAXPATHLEN];
   char szFileTitle[MAXPATHLEN];
   char szFilter[1024];
   char *str;

   memset(&szFileName, 0, sizeof(szFileName));
   memset(&szFileTitle, 0, sizeof(szFileTitle));
   memset(&szFilter, 0, sizeof(szFilter));

   SendMessage(GetDlgItem(hDlg, mcd == 1 ? IDC_SLOT1_EDIT : IDC_SLOT2_EDIT),
      WM_GETTEXT, (WPARAM)MAXPATHLEN, (LPARAM)szFileName);

   strcpy(szFilter, "Memory Card Files (*.mcr;*.mc;*.mem;*.vgs;*.mcd;*.gme;*.ddf)");
   str = szFilter + strlen(szFilter) + 1; 
   strcpy(str, "*.mcr;*.mc;*.mem;*.vgs;*.mcd;*.gme;*.ddf");
   str += strlen(str) + 1;
   strcpy(str, "All Files (*.*)");
   str += strlen(str) + 1;
   strcpy(str, "*.*");

   ofn.lStructSize = sizeof(OPENFILENAME);
   ofn.hwndOwner = hDlg;
   ofn.lpstrFilter = szFilter;
   ofn.lpstrCustomFilter = NULL;
   ofn.nMaxCustFilter = 0;
   ofn.nFilterIndex = 1;
   ofn.lpstrFile = szFileName;
   ofn.nMaxFile = MAXPATHLEN;
   ofn.lpstrInitialDir = ".";
   ofn.lpstrFileTitle = szFileTitle;
   ofn.nMaxFileTitle = MAXPATHLEN;
   ofn.lpstrTitle = NULL;
   ofn.lpstrDefExt = "mcr";
   ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;

   if (GetOpenFileName((LPOPENFILENAME)&ofn))
   {
      SendMessage(GetDlgItem(hDlg, mcd == 1 ? IDC_SLOT1_EDIT : IDC_SLOT2_EDIT),
         WM_SETTEXT, (WPARAM)strlen(szFileName), (LPARAM)szFileName);
   }
}

static BOOL CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
   case WM_INITDIALOG:
      UpdateDialog(hDlg);
      return TRUE;

   case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      case IDCLOSE:
         SaveToConfig(hDlg);
         EndDialog(hDlg, 0);
         return TRUE;

      case IDC_SLOT1_BROWSE:
         OpenMemcardFile(hDlg, 1);
         return TRUE;

      case IDC_SLOT2_BROWSE:
         OpenMemcardFile(hDlg, 2);
         return TRUE;
      }
      break;

   case WM_CLOSE:
      SaveToConfig(hDlg);
      EndDialog(hDlg, 0);
      return TRUE;
   }

   return FALSE;
}

void ShowMemcardDialog()
{
   DialogBox(hModuleInst, MAKEINTRESOURCE(IDD_MEMCARD), hWndMain, DlgProc);
}
