INCLUDES = -DPACKAGE_DATA_DIR=\"${datadir}/pcsx/\" \
	-DPIXMAPDIR=\"${datadir}/pixmaps/\" \
	-DLOCALE_DIR=\"${datadir}/locale/\" \
	$(FLTK_CXXFLAGS) -D__LINUX__

libdir = $(prefix)/lib/games/psemu/
lib_LTLIBRARIES = libDFBinImage.la
libDFBinImage_la_LIBADD = -lstdc++ -lz -lbz2 -lpthread $(FLTK_LDFLAGS) -lportaudio
libDFBinImage_la_LDFLAGS = -module -avoid-version
libDFBinImage_la_SOURCES = About.cpp CDDAData.cpp ConfigCallbacks.cpp ConfigFunctions.cpp TrackParser.cpp FileInterface.cpp Globals.cpp Open.cpp PS2Open.cpp Preferences.cpp SubchannelData.cpp
