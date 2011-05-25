lib_LTLIBRARIES = libmoldeo_mirror.la
libmoldeo_mirror_la_SOURCES = moPreEffectMirror.cpp moPreEffectMirror.h
libmoldeo_mirror_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_mirror_la_LDFLAGS =
libmoldeo_mirror_la_LIBADD = 
libmoldeo_mirror_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = preeffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

