lib_LTLIBRARIES = libmoldeo_livedrawing2.la
libmoldeo_livedrawing2_la_SOURCES = moEffectLiveDrawing2.cpp moEffectLiveDrawing2.h
libmoldeo_livedrawing2_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_livedrawing2_la_LDFLAGS =
libmoldeo_livedrawing2_la_LIBADD = 
libmoldeo_livedrawing2_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

