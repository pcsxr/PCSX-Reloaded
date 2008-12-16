
INCLUDES = -DPACKAGE_DATA_DIR=\"${datadir}/pcsx/\" \
	-DPIXMAPDIR=\"${datadir}/pixmaps/\" \
	-DLOCALE_DIR=\"${datadir}/locale/\" \
	$(GTK2_CFLAGS) $(GLADE2_CFLAGS) \
	-I$(top_srcdir)/libpcsxcore -I$(top_srcdir)/include

bin_PROGRAMS = pcsx

pcsx_SOURCES = \
	LnxMain.c	\
	Plugin.c	\
	Plugin.h	\
	Config.c	\
	Gtk2Gui.c	\
	hdebug.c
	
pcsx_LDADD = \
	$(GTK2_LIBS) $(GLADE2_LIBS) -lpthread ../libpcsxcore/libpcsxcore.la
