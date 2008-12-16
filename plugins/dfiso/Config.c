/***************************************************************************
 *   Copyright (C) 2007 PCSX-df Team						               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cdriso.h"

void LoadConf() {
	FILE *f;
	char cfg[255];

	sprintf(cfg, "dfiso.cfg");
	f = fopen(cfg, "r");
	if (f == NULL) {
		strcpy(IsoFile, "");
		strcpy(CdDev, CDDEV_DEF);
		return;
	}
	fscanf(f, "IsoFile = %[^\n]\n", IsoFile);
	fscanf(f, "CdDev   = %[^\n]\n", CdDev);
	fscanf(f, "LastUsedDir = %[^\n]\n", LastUsedDir);
	fscanf(f, "AutoLoad = %[^\n]\n", AutoLoad);
	if (!strncmp(IsoFile, "CdDev   =", 9)) *IsoFile = 0; // quick fix
	if (*CdDev == 0) strcpy(CdDev, CDDEV_DEF);
	fclose(f);
}

void SaveConf() {
	FILE *f;
	char cfg[255];

	sprintf(cfg, "dfiso.cfg");
	f = fopen(cfg, "w");
	if (f == NULL)
		return;
	fprintf(f, "IsoFile = %s\n", IsoFile);
	fprintf(f, "CdDev   = %s\n", CdDev);
	fprintf(f, "LastUsedDir = %s\n", LastUsedDir);
	fprintf(f, "AutoLoad = %s\n", AutoLoad);
	fclose(f);
}
