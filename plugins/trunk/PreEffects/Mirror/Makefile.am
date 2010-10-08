lib_LTLIBRARIES = libmod_mirror.la
libmod_mirror_la_SOURCES = moPreEffectMirror.cpp
libmod_mirror_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_mirror_la_LDFLAGS =
libmod_mirror_la_LIBADD = 
libmod_mirror_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = PreEffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

