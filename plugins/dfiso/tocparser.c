/******************************************************************************
 * Functions for parsing .toc files
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
#include <string.h>

#include "tocparser.h"

// get a sector from a msf-array
unsigned int msf2sec(char *msf) {
  return(((msf[0]*60+msf[1])*75)+msf[2]);
}

void sec2msf(unsigned int s, char *msf) {
  msf[0] = s/75/60;
  s = s - msf[0]*75*60;
  msf[1] = s/75;
  s = s - msf[1]*75;
  msf[2] = s;
}


// get size of a track given the sector
unsigned int tracklength(unsigned int s) {
  int i;
  
  i = 1;
  while((msf2sec(ti[i].start) < s) && i<=numtracks) i++;
  return(msf2sec(ti[--i].length));
}


// divide a string of xx:yy:zz into m, s, f
void tok2msf(char *time, char *msf) {
  char *token;

  token = strtok(time, ":");
  if(token) msf[0] = atoi(token); else msf[0]=0;
  token = strtok(NULL, ":");
  if(token) msf[1] = atoi(token); else msf[1]=0;
  token = strtok(NULL, ":");
  if(token) msf[2] = atoi(token); else msf[2]=0;
}


// this function tries to get the .toc file of the given .bin
// the neccessary data is put into the ti (trackinformation)-array
int parsetoc(char *isofile) {
  char tocname[1024];
  FILE *fi;
  char linebuf[256], dummy[256];
  char *token;
  char name[256];
  char time[20], time2[20];
  unsigned int i, t;
    
  // copy name of the iso and change extension from .bin to .toc
  strncpy(tocname, isofile, sizeof(tocname));
  token = strstr(tocname, ".bin");
  if(token) sprintf((char*)token, ".toc");
  else return(-1);
  
  numtracks = 0;

  if(!(fi = fopen(tocname, "r"))) {
    printf("Error opening %s!\n", tocname);
    return(-1);
  }

  memset(&ti, 0, sizeof(ti));

  // parse the .toc file
  while(fgets(linebuf, sizeof(linebuf), fi)) {
    // search for tracks
    strncpy(dummy, linebuf, sizeof(linebuf));
    token = strtok(dummy, " ");

    // a new track is found
    if(!strcmp(token, "TRACK")) {
      // get type of track
      token = strtok(NULL, " ");
      
      numtracks++;

      if(!strcmp(token, "MODE2_RAW\n")) {
        ti[numtracks].type = DATA;
        sec2msf(2*75, ti[numtracks].start);		// assume data track on 0:2:0
      }
      
      if(!strcmp(token, "AUDIO\n")) ti[numtracks].type = CDDA;
    }

    // interpretation of other lines
    if(!strcmp(token, "DATAFILE")) {
      sscanf(linebuf, "DATAFILE %s %s", name, time);
      tok2msf((char*)&time, (char*)&ti[numtracks].length);
    }

    if(!strcmp(token, "FILE")) {
      sscanf(linebuf, "FILE %s %s %s %s", name, dummy, time, time2);
      tok2msf((char*)&time, (char*)&ti[numtracks].start);
      tok2msf((char*)&time2, (char*)&ti[numtracks].length);
    }

    if(!strcmp(token, "START")) {
      sscanf(linebuf, "START %s", time);
      tok2msf((char*)&time, (char*)&ti[numtracks].gap);
    }
  } 

  fclose(fi);
  
  // calculate the true start of each track
  // start+gap+datasize (+2 secs of silence ? I dunno...)
  for(i=2; i<=numtracks; i++) {
    t = msf2sec(ti[1].start)+msf2sec(ti[1].length)+msf2sec(ti[i].start)+msf2sec(ti[i].gap);
    sec2msf(t, ti[i].start);
  }
   
  return(0);
}
