INCLUDES = -DPIXMAPDIR=\"${datadir}/pixmaps/\" \
	-DLOCALE_DIR=\"${datadir}/locale/\" \
	-DDATADIR=\"${datadir}/psemu/\" \
	$(GTK2_CFLAGS) $(GLADE2_CFLAGS) \
	-DVERSION=0 -DREVISION=1 -DBUILD=0

libdir = $(prefix)/lib/games/psemu/
lib_LTLIBRARIES = libDFSound.la

libDFSound_la_SOURCES = spu.c cfg.c dma.c freeze.c psemu.c registers.c
if HAVE_ALSA
libDFSound_la_SOURCES +=  alsa.c
INCLUDES +=  -DUSEALSA
else
libDFSound_la_SOURCES +=  oss.c
endif
libDFSound_la_LDFLAGS = -module -avoid-version -lpthread
if HAVE_ALSA
libDFSound_la_LDFLAGS += $(ALSA_LIBS)
endif

bindir = $(prefix)/lib/games/psemu/
bin_PROGRAMS = cfgDFSound
cfgDFSound_SOURCES = spucfg-0.1df/main.c
cfgDFSound_LDADD = $(GTK2_LIBS) $(GLADE2_LIBS)

glade_DATA = spucfg-0.1df/dfsound.glade2
gladedir = $(datadir)/psemu/
EXTRA_DIST = $(glade_DATA)
