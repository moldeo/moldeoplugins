lib_LTLIBRARIES = libmoldeo_mirrormorphing.la
libmoldeo_mirrormorphing_la_SOURCES = moPreEffectMirrorMorphing.cpp moPreEffectMirrorMorphing.h
libmoldeo_mirrormorphing_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_mirrormorphing_la_LDFLAGS =
libmoldeo_mirrormorphing_la_LIBADD = 
libmoldeo_mirrormorphing_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = preeffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

