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
#include <gtk/gtk.h>
#include <glade/glade.h>

#ifdef ENABLE_NAUTILUSBURN
	#include <nautilus-burn.h>
	#include <nautilus-burn-drive-selection.h>
#else
	#include <dirent.h>
#endif

#include "cdriso.h"


char *LibName = "ISO Image Reader";

const unsigned char revision = VERSION;
const unsigned char build = BUILD;

#ifdef ENABLE_NAUTILUSBURN
NautilusBurnDriveMonitor *monitor;
NautilusBurnDrive *drive;
GList *drives;
#endif

#ifndef ENABLE_NAUTILUSBURN
/* this functions returns a list of possible cdrom-devices as a combo-box*/
/* first guess is a scan in /sys/block fallback to /dev/cdrom on failure */
GtkWidget *get_drivelist() {
	FILE			*fi;
	DIR				*blockdir;
	struct dirent	*entry;
	char			fname[50];
	char			signature[6];
	GtkWidget 		*cbox;
	
	cbox = gtk_combo_box_new_text();
	// first of all set a default device
	gtk_combo_box_append_text((GtkComboBox*)cbox, "/dev/cdrom");
	gtk_combo_box_append_text((GtkComboBox*)cbox, "/dev/hda");
	gtk_combo_box_append_text((GtkComboBox*)cbox, "/dev/scd0");
	gtk_combo_box_append_text((GtkComboBox*)cbox, "/dev/sr0");
	gtk_combo_box_append_text((GtkComboBox*)cbox, "/dev/sg0");

	// now other device found in system are prepended
	if(blockdir = opendir("/sys/block")) {
		// get each entry of block devices
		while(entry = readdir(blockdir)) {
			// if it's not '.' or '..' scan it for media="cdrom"
			if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
			
			snprintf(fname, 50, "/sys/block/%s/device/media", entry->d_name);
			if(fi = fopen(fname, "rb")) {
				memset(&signature, 0, sizeof(signature));
				fread(&signature, sizeof(signature)-1, 1, fi);
				if(!strcmp((char*)&signature, "cdrom")) {
					snprintf(fname, 50, "/dev/%s", entry->d_name);
					gtk_combo_box_prepend_text((GtkComboBox*)cbox, fname);
				}
				fclose(fi);			
			}
		}
		closedir(blockdir);
	}
	
	// set the first drive as default. if scan failed this is /dev/cdrom
	gtk_combo_box_set_active((GtkComboBox*)cbox,0);
	strcpy(CdDev, gtk_combo_box_get_active_text((GtkComboBox*)cbox));

	return(cbox);
}
#endif


void SysMessageDialog (gint message_type, char *primary, char *secondary) {
	GtkWidget *MsgDlg;
	MsgDlg = gtk_message_dialog_new (NULL,
					GTK_DIALOG_MODAL, message_type,
					GTK_BUTTONS_CLOSE,
					primary);
	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (MsgDlg), secondary);

	gtk_dialog_run (GTK_DIALOG (MsgDlg));
	gtk_widget_destroy (MsgDlg);
}

GtkWidget *ConfDlg;
GtkWidget *Edit, *CdEdit;
GtkWidget *FileSel;
GtkWidget *Progress;
GtkWidget *auto_load_checkbutton, *auto_load_chooser;

GtkWidget *Method;

int stop = 0;

void OnClose (GtkWidget *widget, gpointer user_data) {
	gchar *filename;
	
	/* Get the current values from the Configuration widgets */
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (auto_load_checkbutton)))
		strcpy (AutoLoad, "Y");
	else
		strcpy (AutoLoad, "N");

	filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (auto_load_chooser));
	if (filename)
		strcpy (IsoFile, gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (auto_load_chooser)));

	SaveConf();
	
	gtk_widget_destroy(ConfDlg);
	gtk_main_quit();
}


void on_auto_load_toggled (GtkWidget *widget, gpointer user_data) {
	gtk_widget_set_sensitive (auto_load_chooser,
			gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (auto_load_checkbutton)));
	
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (auto_load_checkbutton)))
		strcpy (AutoLoad, "Y");
	else
		strcpy (AutoLoad, "N");
}

/* Get the device name currently selected */
#ifdef ENABLE_NAUTILUSBURN
drive_changed (NautilusBurnDriveSelection *selection,
			   NautilusBurnDrive          *drive)
{
	printf("Device name is %s\n", nautilus_burn_drive_get_device (drive));
	strcpy (CdDev, nautilus_burn_drive_get_device (drive)); 	 
}
#else
static void
drive_changed (GtkComboBox *cbox, gpointer user_data) {
	strcpy(CdDev, gtk_combo_box_get_active_text((GtkComboBox*)cbox));
	printf("active CD-Rom: %s\n", CdDev);
}
#endif

long CDRconfigure(void) {
	int i;
	GtkWidget *widget, *close_button;
	GladeXML *xml;
	
	LoadConf();
	
	xml = glade_xml_new(DATADIR "dfiso.glade2", "Config", NULL);
    if (!xml) {
		g_warning("We could not load the interface!");
		return;
    }
	ConfDlg = glade_xml_get_widget(xml, "Config");
	glade_xml_signal_autoconnect (xml);

	/* Configuration widgets */
	auto_load_checkbutton = glade_xml_get_widget(xml, "auto_load_checkbutton");
	g_signal_connect_data(GTK_OBJECT(auto_load_checkbutton), "clicked",
			GTK_SIGNAL_FUNC(on_auto_load_toggled), xml, NULL, G_CONNECT_AFTER);
	
	auto_load_chooser = glade_xml_get_widget(xml, "auto_load_chooser");
	gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (auto_load_chooser), IsoFile);

	if (strncmp (AutoLoad, "Y", 1) == 0) {
		gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (auto_load_checkbutton), TRUE);
		gtk_widget_set_sensitive (auto_load_chooser, TRUE);
	} else {
		gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (auto_load_checkbutton), FALSE);
		gtk_widget_set_sensitive (auto_load_chooser, FALSE);
	}
	
	/* Create new ISO widgets */
	close_button = glade_xml_get_widget(xml, "btn_Close");
	g_signal_connect_data(GTK_OBJECT(close_button), "clicked",
			GTK_SIGNAL_FUNC(OnClose), xml, NULL, G_CONNECT_AFTER);

	/* Get the CD device */
	widget = glade_xml_get_widget(xml, "table1");
#ifdef ENABLE_NAUTILUSBURN
	monitor = nautilus_burn_get_drive_monitor ();
	drives = nautilus_burn_drive_monitor_get_drives (monitor);
	drive = drives->data;
	
	nautilus_burn_init ();
	
	GtkWidget *cdlist = nautilus_burn_drive_selection_new ();
	g_signal_connect (cdlist, "drive-changed",
					  G_CALLBACK (drive_changed), NULL);
#else
	GtkWidget *cdlist = get_drivelist();
	g_signal_connect (cdlist, "changed",
					  G_CALLBACK (drive_changed), NULL);
#endif
	gtk_table_attach_defaults (GTK_TABLE (widget), cdlist, 1, 2, 0, 1);
	gtk_widget_show (cdlist);

	gtk_widget_show (ConfDlg);
	
	gtk_main();

#ifdef ENABLE_NAUTILUSBURN
	nautilus_burn_shutdown ();
#endif
	
	return 0;
}

void CDRabout(void) {
	gchar* version;
	
	version = g_strdup_printf("%d.%d\n", revision, build);

	GtkWidget *AboutDlg = gtk_about_dialog_new ();
	gtk_about_dialog_set_name (GTK_ABOUT_DIALOG (AboutDlg),
				LibName);
	gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (AboutDlg),
				version);
	g_free (version);

	gtk_dialog_run (GTK_DIALOG (AboutDlg));
	gtk_widget_destroy (AboutDlg);

}

int main(int argc, char *argv[]) {

	
	if(argc<2 || argc >3) {
		printf("Usage: cdriso configure|about|message\n");
		return(-1);
	}

	gtk_init(NULL, NULL);
	
	if (!strcmp(argv[1], "configure")) {
		CDRconfigure();
	} else if (!strcmp(argv[1], "about")) {
		CDRabout();
	} else if (!strcmp(argv[1], "message")) {
		if (argc > 2) SysMessageDialog(1, "PCSX-DF DFISO-Plugin:", argv[2]);
	}
	
	return 0;
}

