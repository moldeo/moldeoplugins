lib_LTLIBRARIES = libmod_livedrawing2.la
libmod_livedrawing2_la_SOURCES = moEffectLiveDrawing2.cpp
libmod_livedrawing2_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_livedrawing2_la_LDFLAGS =
libmod_livedrawing2_la_LIBADD = 
libmod_livedrawing2_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

