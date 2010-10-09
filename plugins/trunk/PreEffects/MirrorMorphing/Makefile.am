lib_LTLIBRARIES = libmod_mirrormorphing.la
libmod_mirrormorphing_la_SOURCES = moPreEffectMirrorMorphing.cpp
libmod_mirrormorphing_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_mirrormorphing_la_LDFLAGS =
libmod_mirrormorphing_la_LIBADD = 
libmod_mirrormorphing_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = preeffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

