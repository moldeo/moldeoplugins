lib_LTLIBRARIES = libmoldeo_mirrorg.la
libmoldeo_mirrorg_la_SOURCES = moPreEffectMirrorG.cpp moPreEffectMirrorG.h
libmoldeo_mirrorg_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_mirrorg_la_LDFLAGS =
libmoldeo_mirrorg_la_LIBADD = 
libmoldeo_mirrorg_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = preeffects
datadir = ${prefix}/share/moldeo/data
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

