/******************************************************************************
 * General functions for playing samples with ALSA
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
#include <alsa/asoundlib.h>
#include "alsaplay.h"

snd_pcm_t *pcm_handleplay;								// Handle for playback device

int set_hw_ALSA(snd_pcm_t *pcm_handle) {
    int rate = 44100;									// sample rate
    int exact_rate;	
    snd_pcm_hw_params_t *hwparams;							// hardware parameters
	unsigned period_time = 0;
	unsigned buffer_time = 0;
	snd_pcm_uframes_t period_frames = 0;
	snd_pcm_uframes_t buffer_frames = 0;
  
    snd_pcm_hw_params_alloca(&hwparams);

    /* Init hwparams with full configuration space */
    if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0) {
      fprintf(stderr, "ALSA: Can not configure this PCM device.\n");
      return(-1);
    }

    if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
      fprintf(stderr, "ALSA: Error setting access.\n");
      return(-1);
    }
  
    if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_BE) < 0) {
      fprintf(stderr, "ALSA: Error setting format.\n");
      return(-1);
    }

    // Set sample rate. If the exact rate is not supported 
    // by the hardware, use nearest possible rate.          
    exact_rate = rate;
    if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_rate, 0) < 0) {
      fprintf(stderr, "ALSA: Error setting rate.\n");
      return(-1);
    }
    if (rate != exact_rate) {
      fprintf(stderr, "ALSA: The rate %d Hz is not supported by your hardware.\nUsing %d Hz instead.\n", rate, exact_rate);
    }

    if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, 2) < 0) {
      fprintf(stderr, "ALSA: Error setting channels.\n");
      return(-1);
    }

	snd_pcm_hw_params_get_buffer_time_max(hwparams, &buffer_time, 0);
	if (buffer_time > 500000) buffer_time = 500000;

	if (buffer_time > 0) period_time = buffer_time / 4;
	else period_frames = buffer_frames / 4;

	if (period_time > 0) snd_pcm_hw_params_set_period_time_near(pcm_handle, hwparams, &period_time, 0);
	else snd_pcm_hw_params_set_period_size_near(pcm_handle, hwparams, &period_frames, 0);

	if (buffer_time > 0) snd_pcm_hw_params_set_buffer_time_near(pcm_handle, hwparams, &buffer_time, 0);
	else snd_pcm_hw_params_set_buffer_size_near(pcm_handle, hwparams, &buffer_frames);
						
    if (snd_pcm_hw_params(pcm_handle, hwparams) < 0) {
      fprintf(stderr, "ALSA: Error setting HW params.\n");
      return(-1);
    }
	return(0);
}


int init_ALSA() {
    char *pcm_name;				// name of PCM-device
    pcm_name = strdup("default");
  
    if (snd_pcm_open(&pcm_handleplay, pcm_name, SND_PCM_STREAM_PLAYBACK, 0) < 0) {
      fprintf(stderr, "ALSA: Error opening PCM-playback device %s\n", pcm_name);
      return(-1);
    }
	if (set_hw_ALSA(pcm_handleplay)) return(-1);
	  
	return(0);
}


void uninit_ALSA() {
    snd_pcm_close(pcm_handleplay);
}


void play_ALSA(unsigned char *data, int size) {
	int pcmreturn;
	while ((pcmreturn = snd_pcm_writei(pcm_handleplay, data, size)) < 0) {
        snd_pcm_prepare(pcm_handleplay);
//        fprintf(stderr, "ALSA: Buffer Underrun\n");
    }
}
