/*  Pcsx - Pc Psx Emulator
 *  Copyright (C) 1999-2002  Pcsx Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>

#include "Linux.h"
/* FIXME */
#include "../libpcsxcore/sio.h"

#include "config.h"

#ifdef ENABLE_NLS
#include <locale.h>
#endif

enum {
	DONT_USE_GUI,
	USE_GUI
};

enum {
	RUN,
	RUN_CD,
	RUN_CD_BIOS
};

int UseGui = USE_GUI;
char cdrfilename[MAXPATHLEN] = "";

void create_memory_card (char *filename, char *conf_mcd) {
	gchar *mcd;
	struct stat buf;
		
	mcd = g_build_filename (getenv("HOME"), MEMCARD_DIR, filename, NULL);

	/* Only create a memory card if an existing one does not exist */
	if (stat(mcd, &buf) == -1) {
		strcpy(conf_mcd, mcd);
		printf("Creating memory card: %s\n", mcd);
		CreateMcd(mcd);
	}
	g_free (mcd);
}

/* Set the default plugin name */
void set_default_plugin (char *plugin_name, char *conf_plugin_name) {
	if (strlen(plugin_name) != 0) {
		strcpy(conf_plugin_name, plugin_name);
		printf("Picking default plugin: %s\n", plugin_name);
	} else
		printf("No default plugin could be found for %s\n", conf_plugin_name);
	
}

int main(int argc, char *argv[]) {
	char *file = NULL;
    char BIOSDir[MAXPATHLEN];
	char dotdir[MAXPATHLEN];
	char path[MAXPATHLEN];
	struct stat buf;
	int runcd = 0;
	int loadst = 0;
	int i;

#ifdef ENABLE_NLS
	setlocale (LC_ALL, "");
	bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif


	// what is the name of the config file?
    	// it may be redefined by -cfg on the command line
	strcpy(cfgfile_basename, "pcsx.cfg");

    // read command line options
	for (i=1; i<argc; i++) {
		if (!strcmp(argv[i], "-runcd")) runcd = RUN_CD;
		else if (!strcmp(argv[i], "-runcdbios")) runcd = RUN_CD_BIOS;
		else if (!strcmp(argv[i], "-nogui")) UseGui = DONT_USE_GUI;
		else if (!strcmp(argv[i], "-psxout")) Config.PsxOut = 1;
		else if (!strcmp(argv[i], "-load")) loadst = atol(argv[++i]);
		else if (!strcmp(argv[i], "-cfg")) {
            strncpy(cfgfile_basename, argv[++i], MAXPATHLEN-100);	/* TODO buffer overruns */
            printf("Using config file %s.\n", cfgfile_basename);
		}
		else if (!strcmp(argv[i], "-cdfile")) {
			strncpy(cdrfilename, argv[++i], MAXPATHLEN);
			if (cdrfilename[0] != '/') {
				getcwd(path, MAXPATHLEN);
				if (strlen(path) + strlen(cdrfilename) + 1 < MAXPATHLEN) {
					strcat(path, "/");
					strcat(path, cdrfilename);
					strcpy(cdrfilename, path);
				} else
					cdrfilename[0]=0;
			}
			if (runcd == RUN)
				runcd = RUN_CD;
		}
        else if (!strcmp(argv[i], "-h") ||
			 !strcmp(argv[i], "-help")) {
			 printf(PACKAGE_STRING "\n");
			 printf("%s\n", _(
			 				" pcsx [options] [file]\n"
							"\toptions:\n"
							"\t-runcd\t\tRuns CD-ROM\n"
							"\t-runcdbios\tRuns CD-ROM through BIOS\n"
							"\t-cdfile FILE\tSend a filename to CD plugins that support it\n"
							"\t-nogui\t\tDon't open the GTK GUI\n"
							"\t-cfg FILE\tLoads desired configuration file (default: ~/.pcsx/pcsx.cfg)\n"
							"\t-psxout\t\tEnable PSX output\n"
							"\t-load STATENUM\tLoads savestate STATENUM (1-5)\n"
							"\t-h -help\tDisplay this message\n"
							"\tfile\t\tLoads file\n"));
			 return 0;
		} else file = argv[i];
	}

	memset(&Config, 0, sizeof(PcsxConfig));
	strcpy(Config.Net, "Disabled");
    
	if (UseGui == USE_GUI) gtk_init(NULL, NULL);

    // try to load config
    // if the config file doesn't exist
	if (LoadConfig() == -1) {
		if (UseGui == DONT_USE_GUI) {
			printf(_("PCSX cannot be configured without using the GUI -- you should restart without -nogui.\n"));
			return 1;
		}
	
        // Uh oh, no config file found, use some defaults
		Config.PsxAuto = 1;	/* ADB TODO */
		
		gchar *str_bios_dir = g_strconcat (getenv("HOME"), "/.pcsx/bios/", NULL);
		strcpy(Config.BiosDir,  str_bios_dir);
		g_free (str_bios_dir);

		gtk_init(NULL, NULL);

		// switch to bios dir for scanning
		gchar *str_plugin_dir = g_build_filename (getenv("HOME"), BIOS_DIR, NULL);
		chdir (str_plugin_dir);
		g_free (str_plugin_dir);

		// scan some default locations to find plugins
		// ADB TODO These are Linux-specific - what about Windows?
		gchar *currentdir;
		ScanPlugins("/usr/lib/games/psemu/");
		ScanPlugins("/usr/lib/games/psemu/lib/");
		ScanPlugins("/usr/lib/games/psemu/config/");
		ScanPlugins("/usr/local/lib/games/psemu/lib/");
		ScanPlugins("/usr/local/lib/games/psemu/config/");
		ScanPlugins("/usr/local/lib/games/psemu/");
		
		currentdir = g_strconcat (getenv("HOME"), "/.psemu-plugins/", NULL);
		ScanPlugins(currentdir);

		currentdir = g_strconcat (getenv("HOME"), "/.psemu/", NULL);
		ScanPlugins(currentdir);
		g_free (currentdir);

		// Update available plugins, but not GUI
		UpdatePluginsBIOS();

		// Pick some defaults, if they're available
		set_default_plugin (GpuConfS.plist[0], Config.Gpu);
		set_default_plugin (SpuConfS.plist[0], Config.Spu);
		set_default_plugin (CdrConfS.plist[0], Config.Cdr);
		set_default_plugin (Pad1ConfS.plist[0], Config.Pad1);
		set_default_plugin (Pad2ConfS.plist[0], Config.Pad2);
		set_default_plugin (BiosConfS.plist[0], Config.Bios);
		
		// create & load default memcards if they don't exist
		create_memory_card ("card1.mcd", Config.Mcd1);
		create_memory_card ("card2.mcd", Config.Mcd2);

		LoadMcds(Config.Mcd1, Config.Mcd2);

		SaveConfig();
	}

	// switch to plugin dotdir
	// this lets plugins work without modification!
	gchar *plugin_default_dir = g_build_filename (getenv("HOME"), PLUGINS_DIR, NULL);
	chdir(plugin_default_dir);	/* TODO Error checking - make sure this directory is available */
	g_free (plugin_default_dir);

	// release and load all plugins
	// don't try to load plugins if they aren't set!
	/* ADB TODO Why do we need to do this at startup? */
	if (plugins_configured() != FALSE) {
		ReleasePlugins();
		LoadPlugins();
		needreset = 1;
	}

	if (SysInit() == -1)
		return 1;	/* ADB TODO Error line. Note SysInit() doesn't return a value, so this will never happen! */

	if (UseGui == USE_GUI) {
		StartGui();
	} else {

		// the following only occurs if the gui isn't started

		if (OpenPlugins() == -1 || plugins_configured() == FALSE)
			return 1;	/* TODO Error reporting */
		SysReset();

		CheckCdrom();

		if (file != NULL)
			Load(file);
		else {
			if (runcd == RUN_CD) {
				LoadCdBios = 0;
				if (LoadCdrom() == -1) {
					ClosePlugins();
					printf(_("Could not load CD-ROM!\n"));
					return -1;
				}
			} else if (runcd == RUN_CD_BIOS)
				LoadCdBios = 1;
		}

		/* If a state has been specified, then load that */
		if (loadst) {
			StatesC = loadst-1;
			gchar *state_filename = get_state_filename (StatesC);
			LoadState(state_filename);	/* TODO Error checking */
			g_free (state_filename);
		}

		psxCpu->Execute();
	}
	return 0;
}

DIR *dir;

int SysInit() {

#ifdef GTE_DUMP
	gteLog = fopen("gteLog.txt","wb");
	setvbuf(gteLog, NULL, _IONBF, 0);
#endif

#ifdef EMU_LOG
#ifndef LOG_STDOUT
	emuLog = fopen("emuLog.txt","wb");
#else
	emuLog = stdout;
#endif
	setvbuf(emuLog, NULL, _IONBF, 0);
#endif

	if (psxInit() == -1) {
		/* TODO Error handling */
		printf("PSX emulator couldn't be initialised.\n");
		return -1;
	}
	

	LoadMcds(Config.Mcd1, Config.Mcd2);	/* TODO Do we need to have this here, or in the calling main() function?? */

	return 0;
}

void SysReset() {
	psxReset();
}

void SysClose() {
	psxShutdown();
	ReleasePlugins();

	if (emuLog != NULL) fclose(emuLog);
}

void SysPrintf(char *fmt, ...) {
	va_list list;
	char msg[512];

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	if (Config.PsxOut) printf ("%s", msg);
#ifdef EMU_LOG
#ifndef LOG_STDOUT
	fprintf(emuLog, "%s", msg);
#endif
#endif
}

void *SysLoadLibrary(char *lib) {
	return dlopen(lib, RTLD_NOW);
}

void *SysLoadSym(void *lib, char *sym) {
	return dlsym(lib, sym);
}

const char *SysLibError() {
	return dlerror();
}

void SysCloseLibrary(void *lib) {
	dlclose(lib);
}

void SysUpdate() {
	PADhandleKey(PAD1_keypressed());
	PADhandleKey(PAD2_keypressed());
}

/* ADB TODO Replace RunGui() with StartGui ()*/
void SysRunGui() {
	StartGui();
}
