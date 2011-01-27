lib_LTLIBRARIES = libmod_ligia.la
libmod_ligia_la_SOURCES = moMasterEffectLigia.cpp moMasterEffectLigia.h
libmod_ligia_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_ligia_la_LDFLAGS =
libmod_ligia_la_LIBADD = 
libmod_ligia_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = mastereffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

