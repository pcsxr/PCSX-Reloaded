libdir = $(prefix)/lib/games/psemu/
lib_LTLIBRARIES = libDFInput.la

libDFInput_la_SOURCES = pad.c padjoy.h
libDFInput_la_LDFLAGS = -version-info 0:1:0 -lpthread

INCLUDES = -DPIXMAPDIR=\"${datadir}/pixmaps/\" \
	-DLOCALE_DIR=\"${datadir}/locale/\" \
	-DDATADIR=\"${datadir}/psemu/\" \
	$(GTK2_CFLAGS) $(GLADE2_CFLAGS) \
	-DVERSION=0 -DREVISION=1 -DBUILD=0

bindir = $(prefix)/lib/games/psemu/
bin_PROGRAMS = cfgDFInput
cfgDFInput_SOURCES = cfg.c padjoy.h
cfgDFInput_LDADD = $(GTK2_LIBS) $(GLADE2_LIBS)

glade_DATA = dfinput.glade2
gladedir = $(datadir)/psemu/
EXTRA_DIST = $(glade_DATA)
