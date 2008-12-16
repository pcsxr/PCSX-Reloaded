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
#include <sys/stat.h>
#include <gtk/gtk.h>

#include "cdriso.h"
#include "Config.h"

#include "cddaplay.h"
#include "tocparser.h"

FILE *cdHandle = NULL;

// define the type of this plugin for compatability with PSEMU
#define PSE_LT_CDR 1

char *LibName = "ISO Image Reader";
const unsigned char version = 1;	// PSEmu 1.x library
const unsigned char revision = VERSION;
const unsigned char build = BUILD;

#define MSF2SECT(m,s,f)	(((m)*60+(s)-2)*75+(f))
#define btoi(b)		((b)/16*10 + (b)%16)		// BCD to u_char

char *PSEgetLibName(void) {
	return LibName;
}

unsigned long PSEgetLibType(void) {
	return PSE_LT_CDR;
}

unsigned long PSEgetLibVersion(void) {
	return version << 16 | revision << 8 | build;
}

void ExecCfg(char *arg) {
	char cfg[256];
	struct stat buf;

	strcpy(cfg, "./cfgDFIso");
	if (stat(cfg, &buf) != -1) {
		sprintf(cfg, "%s %s", cfg, arg);
		system(cfg);
	} else
		printf("cfgDFIso file not found!\n");
}

long CDRconfigure() {
	ExecCfg("configure");

	return 0;
}

void CDRabout() {
	ExecCfg("about");
}

long CDRinit(void) {
	passedfile[0]=0;
	return 0;
}

long CDRshutdown(void) {
	return 0;
}


/* This function is invoked by the front-end when opening an ISO
   file for playback */
long CDRopen(void) {
	GtkWidget *chooser;

	if (cdHandle != NULL)
		return 0;				/* it's already open */

	LoadConf();

	if ( (strncmp (AutoLoad, "Y", 1) != 0 || !(IsoFile)) && !passedfile[0] ) {
		/* If the user does not want to auto-load an ISO, or the ISO has not been set,
		   then we need to choose an ISO from the filesystem. Note that if AutoLoad = Y
		   then the IsoFile will be set from the Configuration dialog */
		gchar *filename;
		chooser = gtk_file_chooser_dialog_new ("Open PSX ISO File",
											   NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
											   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
											   GTK_STOCK_OK, GTK_RESPONSE_OK,
											   NULL);

		gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (chooser),
											 LastUsedDir);

		GtkFileFilter *psxfilter = gtk_file_filter_new ();
		gtk_file_filter_add_pattern (psxfilter, "*.bin");
		gtk_file_filter_add_pattern (psxfilter, "*.img");
		gtk_file_filter_add_pattern (psxfilter, "*.iso");
		gtk_file_filter_set_name (psxfilter, "PSX Image Files (*.bin, *.img, *.iso)");
		gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (chooser), psxfilter);

		GtkFileFilter *allfilter = gtk_file_filter_new ();
		gtk_file_filter_add_pattern (allfilter, "*.*");
		gtk_file_filter_set_name (allfilter, "All Files (*.*)");
		gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (chooser), allfilter);
	
		if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_OK) {
			filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
		
			/* Update the last used directory with that of the specified ISO */
			strcpy (LastUsedDir, g_path_get_dirname (filename));
			SaveConf();

			gtk_widget_destroy (GTK_WIDGET (chooser));
			while (gtk_events_pending()) gtk_main_iteration();
		} else {
			gtk_widget_destroy (GTK_WIDGET (chooser));
			while (gtk_events_pending()) gtk_main_iteration();
			return -1;
		}

		strcpy (IsoFile, filename);
		g_free (filename);
	}

	if (passedfile[0])
		strncpy(IsoFile, passedfile, 1024);

	cdHandle = fopen(IsoFile, "rb");
	if (cdHandle == NULL) return -1;

	// get neccessary track-information for cdda-playing
	// TODO: if -1 is returned, CDDA should be turned off !
	parsetoc(IsoFile);
	
	return 0;
}

long CDRclose(void) {
	if (cdHandle == NULL)
		return 0;

	fclose(cdHandle);
	cdHandle = NULL;
	
	return 0;
}

// return Starting and Ending Track
// buffer:
//  byte 0 - start track
//  byte 1 - end track
long CDRgetTN(unsigned char *buffer) {
	buffer[0] = 1;
	if(numtracks>0) buffer[1] = numtracks;
	else buffer[1] = 1;
	return 0;
}

// return Track Time
// buffer:
//  byte 0 - frame
//  byte 1 - second
//  byte 2 - minute
long CDRgetTD(unsigned char track, unsigned char *buffer) {
	if(numtracks>0 && track<=numtracks) {
		buffer[2] = ti[track].start[0];
		buffer[1] = ti[track].start[1];
		buffer[0] = ti[track].start[2];
	} else {
		buffer[2] = 0;
		buffer[1] = 2;
		buffer[0] = 0;
	}
	return 0;
}

// read track
// time : byte 0 - minute ; byte 1 - second ; byte 2 - frame
// uses bcd format
long CDRreadTrack(unsigned char *time) {

	if (cdHandle == NULL) return -1;

	fseek(cdHandle, MSF2SECT(btoi(time[0]), btoi(time[1]), btoi(time[2])) * CD_FRAMESIZE_RAW + 12, SEEK_SET);
	fread(cdbuffer, 1, DATA_SIZE, cdHandle);

	return 0;
}

// return readed track
unsigned char *CDRgetBuffer(void) {
	return (char *)&cdbuffer;
}

// plays cdda audio
// sector : byte 0 - minute ; byte 1 - second ; byte 2 - frame
// does NOT uses bcd format
long CDRplay(unsigned char *time) {
	printf("Starting cdda-audio... ;-)\n");
	printf("m: %i s: %i f: %i\n", time[0], time[1], time[2]);

	// missing: handle REPEATing of audio-tracks
	startCDDA(MSF2SECT(time[0], time[1], time[2]) * CD_FRAMESIZE_RAW);

	return 0;
}

// stops cdda audio
long CDRstop(void) {
	printf("Stopping cdda-audio... \n");
	stopCDDA();
	printf("stopped!\n");
	return 0;
}

/* This function is only used if invoked by the front-end */
long CDRtest(void) {
	if (*IsoFile == 0)
		return 0;
	cdHandle = fopen(IsoFile, "rb");
	if (cdHandle == NULL)
		return -1;
	fclose(cdHandle);
	cdHandle = NULL;
	return 0;
}

long CDRsetfilename(char *filename)
{
	strncpy(passedfile, filename, 1024);
	return 0;
}
