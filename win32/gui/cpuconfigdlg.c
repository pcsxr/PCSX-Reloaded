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

static void UpdateDialog(HWND hDlg)
{
   #define GETCONFIG(value, ctl)                       \
   {                                                   \
      SendMessage(GetDlgItem(hDlg, ctl), BM_SETCHECK,  \
         value ? BST_CHECKED : BST_UNCHECKED, 0);      \
   }

   GETCONFIG(Config.Xa, IDC_CHECK_XA);
   GETCONFIG(Config.Cdda, IDC_CHECK_CDDA);
   GETCONFIG(Config.Sio, IDC_CHECK_SIO);
   GETCONFIG(Config.Cpu, IDC_CHECK_CPU);
   GETCONFIG(Config.SpuIrq, IDC_CHECK_SPUIRQ);
   GETCONFIG(Config.RCntFix, IDC_CHECK_RCNTFIX);
   GETCONFIG(Config.Mdec, IDC_CHECK_MDEC);
   GETCONFIG(Config.VSyncWA, IDC_CHECK_VSYNCWA);

   #undef GETCONFIG

   if (Config.PsxAuto)
   {
      SendMessage(GetDlgItem(hDlg, IDC_RADIO_PSXAUTO), BM_SETCHECK,
         BST_CHECKED, 0);
      SendMessage(GetDlgItem(hDlg, IDC_RADIO_NTSC), BM_SETCHECK,
         BST_UNCHECKED, 0);
      SendMessage(GetDlgItem(hDlg, IDC_RADIO_PAL), BM_SETCHECK,
         BST_UNCHECKED, 0);
   }
   else if (Config.PsxType == 0) // NTSC = 0, PAL = 1
   {
      SendMessage(GetDlgItem(hDlg, IDC_RADIO_PSXAUTO), BM_SETCHECK,
         BST_UNCHECKED, 0);
      SendMessage(GetDlgItem(hDlg, IDC_RADIO_NTSC), BM_SETCHECK,
         BST_CHECKED, 0);
      SendMessage(GetDlgItem(hDlg, IDC_RADIO_PAL), BM_SETCHECK,
         BST_UNCHECKED, 0);
   }
   else
   {
      SendMessage(GetDlgItem(hDlg, IDC_RADIO_PSXAUTO), BM_SETCHECK,
         BST_UNCHECKED, 0);
      SendMessage(GetDlgItem(hDlg, IDC_RADIO_NTSC), BM_SETCHECK,
         BST_UNCHECKED, 0);
      SendMessage(GetDlgItem(hDlg, IDC_RADIO_PAL), BM_SETCHECK,
         BST_CHECKED, 0);
   }
}

static void SaveToConfig(HWND hDlg)
{
   #define SETCONFIG(value, ctl)                       \
   {                                                   \
      value = ((SendMessage(GetDlgItem(hDlg, ctl),     \
         BM_GETCHECK, 0, 0) == BST_CHECKED) ? 1 : 0);  \
   }

   SETCONFIG(Config.Xa, IDC_CHECK_XA);
   SETCONFIG(Config.Cdda, IDC_CHECK_CDDA);
   SETCONFIG(Config.Sio, IDC_CHECK_SIO);
   SETCONFIG(Config.Cpu, IDC_CHECK_CPU);
   SETCONFIG(Config.SpuIrq, IDC_CHECK_SPUIRQ);
   SETCONFIG(Config.RCntFix, IDC_CHECK_RCNTFIX);
   SETCONFIG(Config.Mdec, IDC_CHECK_MDEC);
   SETCONFIG(Config.VSyncWA, IDC_CHECK_VSYNCWA);

   #undef SETCONFIG

   if (SendMessage(GetDlgItem(hDlg, IDC_RADIO_PSXAUTO), BM_GETCHECK, 0, 0) == BST_CHECKED)
   {
      Config.PsxAuto = 1;
   }
   else if (SendMessage(GetDlgItem(hDlg, IDC_RADIO_NTSC), BM_GETCHECK, 0, 0) == BST_CHECKED)
   {
      Config.PsxAuto = 0;
      Config.PsxType = 0;
   }
   else
   {
      Config.PsxAuto = 0;
      Config.PsxType = 1;
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
      }
      break;

   case WM_CLOSE:
      SaveToConfig(hDlg);
      EndDialog(hDlg, 0);
      return TRUE;
   }

   return FALSE;
}

void ShowCPUConfigDialog()
{
   DialogBox(hModuleInst, MAKEINTRESOURCE(IDD_CPUCONFIG), hWndMain, DlgProc);
}
