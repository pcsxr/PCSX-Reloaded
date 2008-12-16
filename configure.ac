AC_INIT(libpcsxcore/psxhw.c)

AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE(pcsx-df, 1.816a)
AM_MAINTAINER_MODE

AC_CONFIG_HEADERS([include/config.h:include/config.h.in])

AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_STDC_HEADERS
AM_PROG_AS

AM_GNU_GETTEXT([external])

GETTEXT_PACKAGE=pcsx-df
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["${GETTEXT_PACKAGE}"], [gettext domain])

PKG_CHECK_MODULES(GLIB2, glib-2.0, [], AC_MSG_ERROR([*** glib2 not found!]))
PKG_CHECK_MODULES(GTK2, gtk+-2.0, [], AC_MSG_ERROR([*** libgtk2 not found!]))
PKG_CHECK_MODULES(GLADE2, libglade-2.0, [], AC_MSG_ERROR([*** libglade2 not found!]))
AC_ARG_ENABLE(nautilusburn, [  --enable-nautilusburn    build dfiso with libnautilusburn (default no)],
[ NAUTILUS_BURN="$enableval" ],[ NAUTILUS_BURN="no" ])
AM_CONDITIONAL(ENABLE_NAUTILUSBURN,test "x$NAUTILUS_BURN" = xyes)
if test "$NAUTILUS_BURN" = "yes"; then
	PKG_CHECK_MODULES(BURN, libnautilus-burn >= 2.15.3, [], AC_MSG_ERROR([*** libnautilus-burn not found!]))
	dnl AC_DEFINE(ENABLE_NAUTILUSBURN, 1, [Enable Nautilus burn library])
fi
AC_SUBST(GLIB2_CFLAGS)
AC_SUBST(GLIB2_LIBS)
AC_SUBST(GTK2_CFLAGS)
AC_SUBST(GTK2_LIBS)
AC_SUBST(GLADE2_CFLAGS)
AC_SUBST(GLADE2_LIBS)
AC_SUBST(BURN_CFLAGS)
AC_SUBST(BURN_LIBS)

AC_CONFIG_FILES([Makefile data/Makefile doc/Makefile libpcsxcore/Makefile gui/Makefile plugins/dfinput/Makefile plugins/dfsound/Makefile plugins/dfxvideo/Makefile plugins/dfcdrom/Makefile plugins/dfiso/Makefile plugins/dfOpenGL/Makefile pixmaps/Makefile po/Makefile.in])

dnl Check for -fno-dse option support
saved_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -fno-dse"
AC_CACHE_CHECK([for -fno-dse option support], ac_cv_c_no_dse_support,
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <stdio.h>]], [[]])],[ac_cv_c_no_dse_support=yes],[ac_cv_c_no_dse_support=no]))
CFLAGS="$saved_CFLAGS"
if test "$ac_cv_c_no_dse_support" = "yes"
then
  CFLAGS="$CFLAGS -fno-dse"
fi

dnl Check for ALSA 0.9.x

AC_ARG_ENABLE(alsa, [  --enable-alsa        include alsa 0.9 output plugin ],
[ BUILD_ALSA="$enableval" ],[ BUILD_ALSA="no" ])

if test "$BUILD_ALSA" = "yes"; then
   AC_CHECK_LIB(asound, snd_pcm_open, have_alsa=yes, have_alsa=no)
   AC_CHECK_HEADER(alsa/asoundlib.h, , have_alsa=no)
fi

if test "x$have_alsa" = xyes; then
	ALSA_LIBS="-lasound"
else
	ALSA_LIBS=""
fi
AM_CONDITIONAL(HAVE_ALSA,test "x$have_alsa" = xyes)
AC_SUBST(ALSA_LIBS)

AC_CHECK_HEADER(zlib.h, have_zlib=yes, have_zlib=no)
if test "x$have_zlib" = xno; then
	AC_MSG_ERROR([unable to find libz headers])
fi
AC_CHECK_HEADER(bzlib.h, have_bzlib=yes, have_bzlib=no)
if test "x$have_bzlib" = xno; then
	AC_MSG_ERROR([unable to find libbz2 headers])
fi

AC_ARG_ENABLE(dfbinimage, [  --enable-dfbinimage        build dfbinimage plugin (default no)],
[ BUILD_DFBINIMAGE="$enableval" ],[ BUILD_DFBINIMAGE="no" ])

DFBINIMAGE=""

if test "$BUILD_DFBINIMAGE" = "yes"; then
	AC_PROG_CXX

	AC_CHECK_PROG(FLTKCONFIG, fltk-config, fltk-config)
	fltk_cxxflags=`fltk-config --cxxflags`
	fltk_ldflags=`fltk-config --ldflags`
	FLTK_CXXFLAGS="$fltk_cxxflags"
	FLTK_LDFLAGS="$fltk_ldflags"
	AC_SUBST(FLTK_CXXFLAGS)
	AC_SUBST(FLTK_LDFLAGS)

	AC_CHECK_HEADER(portaudio.h, have_portaudio=yes, have_portaudio=no)
	if test "x$have_portaudio" = xno; then
		AC_MSG_ERROR([unable to find libportaudio headers])
	fi
	AC_CHECK_HEADER(bzlib.h, have_bzlib=yes, have_bzlib=no)
	if test "x$have_bzlib" = xno; then
		AC_MSG_ERROR([unable to find libbz2 headers])
	fi

	DFBINIMAGE="plugins/dfbinimage"
	AC_SUBST(DFBINIMAGE)
	AC_CONFIG_FILES([plugins/dfbinimage/Makefile])
fi

AC_CHECK_HEADER(X11/extensions/Xv.h, have_xv=yes, have_xv=no)
if test "x$have_xv" = xno; then
	AC_MSG_ERROR([unable to find xv headers])
fi
AC_CHECK_HEADER(X11/extensions/XTest.h, have_xtest=yes, have_xtest=no)
if test "x$have_xtest" = xno; then
	AC_MSG_ERROR([unable to find xtest headers])
fi

AM_CONDITIONAL(ARCH_X86, false)
AM_CONDITIONAL(ARCH_X86_64, false)
AM_CONDITIONAL(ARCH_PPC, false)

AC_ARG_ENABLE(dynarec, [  --enable-dynarec=...        force selection of dynamic recompiler platform (x86, x86_64, ppc) (default: autodetect)],
[ DYNAREC="$enableval" ],[ DYNAREC="auto" ])


if test "x$DYNAREC" = xauto; then
	DYNARECSEL="auto"
else if test "x$DYNAREC" = xx86; then
	DYNARECSEL="x86"
else if test "x$DYNAREC" = xx86_64; then
	DYNARECSEL="x86_64"
else if test "x$DYNAREC" = xppc; then
	DYNARECSEL="ppc"
else
	AC_MSG_WARN([Dynamic Recompiler "$DYNAREC" not found.  Autodetecting...])
	DYNARECSEL="auto"
fi
fi
fi
fi

if test "x$DYNARECSEL" = xauto; then
	if expr x"$target_cpu" : 'xi.86' > /dev/null; then
		DYNARECSEL="x86"
	fi

	if expr x"$target_cpu" : 'xx86_64' > /dev/null; then
		DYNARECSEL="x86_64"
	fi

	if expr x"$target_cpu" : 'xpowerpc' > /dev/null; then
		DYNARECSEL="ppc"
	fi
fi

if test "x$DYNARECSEL" = xx86; then
        AC_DEFINE([__i386__], [1], [Define if we are compiling for x86 architectures.])
        AM_CONDITIONAL(ARCH_X86, true)

	AC_PATH_PROG([NASM],[nasm],[missing])
	if test "$NASM" = "missing"; then
	        AC_MSG_ERROR([unable to find nasm, needed to build dfx11video])
	fi
	AC_MSG_RESULT([Dynamic Recompiler selected: x86])
fi

if test "x$DYNARECSEL" = xx86_64; then
        AC_DEFINE([__x86_64__], [1], [Define if we are compiling for x86_64 architectures.])
        AM_CONDITIONAL(ARCH_X86_64, true)
	dnl CFLAGS+=" -m64 "
	dnl AC_COMPILE_IFELSE(AC_LANG_PROGRAM,,AC_MSG_ERROR([Cannot compile with -m64])) 
	AC_MSG_RESULT([Dynamic Recompiler selected: x86_64])
fi

if test "x$DYNARECSEL" = xppc; then
        AC_DEFINE([__ppc__], [1], [Define if we are compiling for powerpc architectures.])
        AM_CONDITIONAL(ARCH_PPC, true)
	AC_MSG_RESULT([Dynamic Recompiler selected: ppc])
fi

AC_C_BIGENDIAN(AC_DEFINE([__BIGENDIAN__],[],[define on a big endian system]))

AC_DEFINE([__LINUX__], [1], [Define if building on a GNU/Linux system.])
AC_DEFINE([MAXPATHLEN], [4096], [Define to the maximum length of any path.])

LD_FLAGS="-lm"

AC_OUTPUT

