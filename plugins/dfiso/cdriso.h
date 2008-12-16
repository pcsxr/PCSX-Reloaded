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

#define VERSION 1

// configuration
char IsoFile[1024];
char passedfile[1024];
char CdDev[256];
#define CDDEV_DEF	"/dev/cdrom"
char LastUsedDir[256];
char AutoLoad[1];

extern FILE *cdHandle;

#define CD_FRAMESIZE_RAW	2352
#define DATA_SIZE	(CD_FRAMESIZE_RAW-12)

unsigned char cdbuffer[CD_FRAMESIZE_RAW * 10];

long CDRinit(void);
long CDRshutdown(void);
long CDRopen(void);
long CDRclose(void);
long CDRgetTN(unsigned char *);
long CDRgetTD(unsigned char , unsigned char *);
long CDRreadTrack(unsigned char *);
unsigned char * CDRgetBuffer(void);
long CDRconfigure(void);
long CDRtest(void);
void CDRabout(void);
long CDRplay(unsigned char *);
long CDRstop(void);
