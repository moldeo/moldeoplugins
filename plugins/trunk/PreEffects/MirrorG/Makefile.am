lib_LTLIBRARIES = libmod_mirrorg.la
libmod_mirrorg_la_SOURCES = moPreEffectMirrorG.cpp moPreEffectMirrorG.h
libmod_mirrorg_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_mirrorg_la_LDFLAGS =
libmod_mirrorg_la_LIBADD = 
libmod_mirrorg_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = preeffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

