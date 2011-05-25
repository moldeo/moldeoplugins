lib_LTLIBRARIES = libmoldeo_ligia.la
libmoldeo_ligia_la_SOURCES = moMasterEffectLigia.cpp moMasterEffectLigia.h
libmoldeo_ligia_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_ligia_la_LDFLAGS =
libmoldeo_ligia_la_LIBADD = 
libmoldeo_ligia_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = mastereffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

