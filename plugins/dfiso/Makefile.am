libdir = $(prefix)/lib/games/psemu/
lib_LTLIBRARIES = libDFIso.la

libDFIso_la_SOURCES = cdriso.c Config.c alsaplay.c cddaplay.c tocparser.c
libDFIso_la_LIBADD = $(GTK2_LIBS) -lasound -lpthread
libDFIso_la_LDFLAGS = -module -avoid-version
libDFIso_la_CFLAGS = $(AM_CFLAGS)

INCLUDES = -DPIXMAPDIR=\"${datadir}/pixmaps/\" \
	-DLOCALE_DIR=\"${datadir}/locale/\" \
	-DDATADIR=\"${datadir}/psemu/\" \
	$(GTK2_CFLAGS) \
	$(GLADE2_CFLAGS) \
	$(BURN_CFLAGS) \
	-DREVISION=1 -DBUILD=0 -D__LINUX__

if ENABLE_NAUTILUSBURN
INCLUDES += -DENABLE_NAUTILUSBURN
endif

bindir = $(prefix)/lib/games/psemu/
bin_PROGRAMS = cfgDFIso
cfgDFIso_SOURCES = conf.c Config.c
cfgDFIso_LDADD = $(GTK2_LIBS) $(GLADE2_LIBS) $(BURN_LIBS) -lasound -lpthread

glade_DATA = dfiso.glade2
gladedir = $(datadir)/psemu/
EXTRA_DIST = $(glade_DATA)
