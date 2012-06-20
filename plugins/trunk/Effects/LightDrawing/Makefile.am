lib_LTLIBRARIES = libmoldeo_lightdrawing.la
libmoldeo_lightdrawing_la_SOURCES = moEffectLightDrawing.cpp moEffectLightDrawing.h
libmoldeo_lightdrawing_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_lightdrawing_la_LDFLAGS =
libmoldeo_lightdrawing_la_LIBADD = 
libmoldeo_lightdrawing_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

