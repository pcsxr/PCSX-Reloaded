/******************************************************************************
 * Functions for threaded CDDA-playback from a raw cd-image
 * for usage in the dfiso-plugin of PCSX-DF
 *
 * Copyright (C) 2008  Stefan Sikora <hoshy[at]schrauberstube.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "cdriso.h"
#include "alsaplay.h"
#include "tocparser.h"

// Globals
FILE *fi = NULL;

pthread_t pthread_id, lthread_id;
pthread_cond_t loadcond;
pthread_mutex_t mutex;

unsigned int actualtrack;			// which track is currently played ?

// 2 buffers are defined, one for loading and one for playback
#define SNDBUFLEN 2352*100			// must be a multiple of 2352 = 1 raw sector = 0.013333 sec of sound

char sndbuffer1[SNDBUFLEN];
char sndbuffer2[SNDBUFLEN];

char *playbuffer, *loadbuffer;
int playindex = 0;
int loadlength;
int playlength;

int playflag;

// load raw sectors to a buffer, clear length on error
void loadsector(char *buf) {
	if(!fread(buf, SNDBUFLEN, 1, fi)) {
//		printf("ERROR: no CDDA-data left\n");
		loadlength = 0;
	}
}


// this thread plays audio data
void *playthread() {
	char *swapbuffer;
	// when this thread starts, the loadthread can begin to load
	// after SNDBUFLEN is played, the loadthread can load another chunk
	// the thread plays as long as playflag is set
	while(playflag) {
		// push data to ALSA
		if(playlength >= SNDBUFLEN) {
			play_ALSA(playbuffer, SNDBUFLEN/2/2);		// 2 bytesfor each of the 2 channels
			playlength -= SNDBUFLEN;
		}
		else {
			play_ALSA(playbuffer, playlength/2/2);
			playlength = 0;
			playflag = 0;
		}

		// swap the buffers
		if(playflag && loadlength) pthread_mutex_lock(&mutex);
		swapbuffer = playbuffer;
		playbuffer = loadbuffer;
		loadbuffer = swapbuffer;
		pthread_mutex_unlock(&mutex);
		
		// raise condition for loadthread
		pthread_cond_signal(&loadcond);
	}
	return(0);
}


// this thread loads audio data from a file
void *loadthread() {
	while(loadlength > 0) {
		loadsector(loadbuffer);
		loadlength -= SNDBUFLEN;

		// wait for rising condition
		pthread_cond_wait(&loadcond, &mutex);
	}
	loadlength = 0;
	return(0);
}


// start the playback
int startCDDA(unsigned int offset) {
	int opened;
	
	if(actualtrack == offset) return(0);		// this track is currently playing
	actualtrack = offset;
	
	if(fi == NULL) opened = 0; else opened = 1;
	
	if(!opened) {
		if(init_ALSA()) {
			printf("Couldn't initialise ALSA!\n");
			return(-1);
		}
	
		if(!(fi = fopen(IsoFile, "rb"))) {
			printf("Couldn't open file!\n");
			return(-1);
		}
	}

	loadlength = tracklength(offset)*2352;
	playlength = loadlength;
	fseek(fi, offset, SEEK_SET);
	loadsector((char *)&sndbuffer1);
	loadlength -= SNDBUFLEN;
	playflag = 1;

	if(!opened) {
		loadbuffer = (char *)&sndbuffer2;
		playbuffer = (char *)&sndbuffer1;

		pthread_cond_init(&loadcond, NULL);
		pthread_mutex_init(&mutex, NULL);
		pthread_create(&pthread_id, NULL, playthread, NULL);
		pthread_create(&lthread_id, NULL, loadthread, NULL);
	}
}

void stopCDDA() {
	if(fi != NULL) {
		// stop the threads
		loadlength = 0;
		pthread_join(lthread_id, NULL);
printf("lthread joined\n");
		playflag = 0;
		pthread_join(pthread_id, NULL);
printf("pthread joined\n");
		pthread_cond_destroy(&loadcond);
		pthread_mutex_destroy(&mutex);
	
		fclose(fi);
		uninit_ALSA();
		
		fi = NULL;
	}
}
