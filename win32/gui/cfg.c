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

// set some default values
static void InitConfig()
{
   strcpy(Config.Bios, "HLE");
   strcpy(Config.BiosDir, "./bios");
   strcpy(Config.Cdr, "");
   strcpy(Config.Mcd1, "memcards/card1.mcd");
   strcpy(Config.Mcd2, "memcards/card2.mcd");
   strcpy(Config.Net, "Disabled");
   strcpy(Config.Pad1, "");
   strcpy(Config.Pad2, "");
   strcpy(Config.PluginsDir, "./plugins");
   strcpy(Config.Spu, "");
   strcpy(Config.Gpu, "");

   Config.Cdda = 0; // enable CDDA by default
   Config.Cpu = 0; // Enable Recompiler Core by default
   Config.Mdec = 0; // use color mdec by default
   Config.PsxOut = 0;
   Config.PsxType = 0; // use NTSC by default
   Config.RCntFix = 0;
   Config.Sio = 0;
   Config.SpuIrq = 0;
   Config.UseNet = 0;
   Config.VSyncWA = 0;
   Config.Xa = 0;
   Config.PsxAuto = 1;
   Config.Dbg = 0;
}

// load configurations from the registry
void LoadConfig()
{
   HKEY          hkey;
   LONG          size, type;

   InitConfig();

   if (RegOpenKeyA(HKEY_CURRENT_USER, "SOFTWARE\\Pcsx", &hkey) == ERROR_SUCCESS)
   {
      #define LOAD_VALUE(s, value, out)                              \
      {                                                              \
         size = s;                                                   \
         RegQueryValueEx(hkey, value, 0, &type, (LPBYTE)out, &size); \
      }

      LOAD_VALUE(sizeof(Config.Cdda), "Cdda", &Config.Cdda);
      LOAD_VALUE(sizeof(Config.Cpu), "Cpu", &Config.Cpu);
      LOAD_VALUE(sizeof(Config.PsxType), "PsxType", &Config.PsxType);
      LOAD_VALUE(sizeof(Config.RCntFix), "RCntFix", &Config.RCntFix);
      LOAD_VALUE(sizeof(Config.Sio), "Sio", &Config.Sio);
      LOAD_VALUE(sizeof(Config.SpuIrq), "SpuIrq", &Config.SpuIrq);
      LOAD_VALUE(sizeof(Config.UseNet), "UseNet", &Config.UseNet);
      LOAD_VALUE(sizeof(Config.VSyncWA), "VSyncWA", &Config.VSyncWA);
      LOAD_VALUE(sizeof(Config.Xa), "Xa", &Config.Xa);
      LOAD_VALUE(sizeof(Config.PsxAuto), "PsxAuto", &Config.PsxAuto);

      LOAD_VALUE(256, "Bios", Config.Bios);
      LOAD_VALUE(MAXPATHLEN, "BiosDir", Config.BiosDir);
      LOAD_VALUE(256, "Cdr", Config.Cdr);
      LOAD_VALUE(256, "Mcd1", Config.Mcd1);
      LOAD_VALUE(256, "Mcd2", Config.Mcd2);
      LOAD_VALUE(256, "Net", Config.Net);
      LOAD_VALUE(256, "Pad1", Config.Pad1);
      LOAD_VALUE(256, "Pad2", Config.Pad2);
      LOAD_VALUE(MAXPATHLEN, "PluginsDir", Config.PluginsDir);
      LOAD_VALUE(256, "Spu", Config.Spu);
      LOAD_VALUE(256, "Gpu", Config.Gpu);

      RegCloseKey(hkey);

      #undef LOAD_VALUE
   }
}

// save configurations into the registry
void SaveConfig()
{
   HKEY             hkey;

   if (RegCreateKeyA(HKEY_CURRENT_USER, "SOFTWARE\\Pcsx", &hkey) == ERROR_SUCCESS)
   {
      RegSetValueExA(hkey, "Cdda", 0, REG_DWORD, (LPBYTE)&Config.Cdda, sizeof(Config.Cdda));
      RegSetValueExA(hkey, "Cpu", 0, REG_DWORD, (LPBYTE)&Config.Cpu, sizeof(Config.Cpu));
      RegSetValueExA(hkey, "PsxType", 0, REG_DWORD, (LPBYTE)&Config.PsxType, sizeof(Config.PsxType));
      RegSetValueExA(hkey, "RCntFix", 0, REG_DWORD, (LPBYTE)&Config.RCntFix, sizeof(Config.RCntFix));
      RegSetValueExA(hkey, "Sio", 0, REG_DWORD, (LPBYTE)&Config.Sio, sizeof(Config.Sio));
      RegSetValueExA(hkey, "SpuIrq", 0, REG_DWORD, (LPBYTE)&Config.SpuIrq, sizeof(Config.SpuIrq));
      RegSetValueExA(hkey, "UseNet", 0, REG_DWORD, (LPBYTE)&Config.UseNet, sizeof(Config.UseNet));
      RegSetValueExA(hkey, "VSyncWA", 0, REG_DWORD, (LPBYTE)&Config.VSyncWA, sizeof(Config.VSyncWA));
      RegSetValueExA(hkey, "Xa", 0, REG_DWORD, (LPBYTE)&Config.Xa, sizeof(Config.Xa));
      RegSetValueExA(hkey, "PsxAuto", 0, REG_DWORD, (LPBYTE)&Config.PsxAuto, sizeof(Config.PsxAuto));

      RegSetValueExA(hkey, "Bios", 0, REG_SZ, (LPBYTE)Config.Bios, strlen(Config.Bios) + 1);
      RegSetValueExA(hkey, "BiosDir", 0, REG_SZ, (LPBYTE)Config.BiosDir, strlen(Config.BiosDir) + 1);
      RegSetValueExA(hkey, "Cdr", 0, REG_SZ, (LPBYTE)Config.Cdr, strlen(Config.Cdr) + 1);
      RegSetValueExA(hkey, "Mcd1", 0, REG_SZ, (LPBYTE)Config.Mcd1, strlen(Config.Mcd1) + 1);
      RegSetValueExA(hkey, "Mcd2", 0, REG_SZ, (LPBYTE)Config.Mcd2, strlen(Config.Mcd2) + 1);
      RegSetValueExA(hkey, "Net", 0, REG_SZ, (LPBYTE)Config.Net, strlen(Config.Net) + 1);
      RegSetValueExA(hkey, "Pad1", 0, REG_SZ, (LPBYTE)Config.Pad1, strlen(Config.Pad1) + 1);
      RegSetValueExA(hkey, "Pad2", 0, REG_SZ, (LPBYTE)Config.Pad2, strlen(Config.Pad2) + 1);
      RegSetValueExA(hkey, "PluginsDir", 0, REG_SZ, (LPBYTE)Config.PluginsDir, strlen(Config.PluginsDir) + 1);
      RegSetValueExA(hkey, "Spu", 0, REG_SZ, (LPBYTE)Config.Spu, strlen(Config.Spu) + 1);
      RegSetValueExA(hkey, "Gpu", 0, REG_SZ, (LPBYTE)Config.Gpu, strlen(Config.Gpu) + 1);

      RegCloseKey(hkey);
   }
}
