STRIP_FPIC = sh $(top_srcdir)/strip_fPIC.sh

SUFFIXES = .asm

.asm.lo:
	$(LIBTOOL) --tag=CC --mode=compile \
	$(STRIP_FPIC) $(NASM) -f elf -d ELF -I${srcdir}/ $<

INCLUDES = -DPIXMAPDIR=\"${datadir}/pixmaps/\" \
	-DLOCALE_DIR=\"${datadir}/locale/\" \
	-DDATADIR=\"${datadir}/psemu/\" \
	$(GTK2_CFLAGS) $(GLADE2_CFLAGS) \
	-DVERSION=XVideo -DNUMBER=0.1.0

libdir = $(prefix)/lib/games/psemu/
lib_LTLIBRARIES = libDFXVideo.la

libDFXVideo_la_SOURCES = gpu.c cfg.c draw.c fps.c key.c menu.c prim.c soft.c zn.c
if ARCH_X86
libDFXVideo_la_SOURCES += i386.asm
endif
libDFXVideo_la_LDFLAGS = -module -avoid-version
libDFXVideo_la_LDFLAGS += -lXv -lXext -lXtst

bindir = $(prefix)/lib/games/psemu/
bin_PROGRAMS = cfgDFXVideo
cfgDFXVideo_SOURCES = gpucfg-0.1df/main.c
cfgDFXVideo_LDADD = $(GTK2_LIBS) $(GLADE2_LIBS) -lXext

glade_DATA = gpucfg-0.1df/dfxvideo.glade2
gladedir = $(datadir)/psemu/
EXTRA_DIST = $(glade_DATA)

