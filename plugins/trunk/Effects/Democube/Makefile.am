lib_LTLIBRARIES = libmod_democube.la
libmod_democube_la_SOURCES = moEffectDemocube.cpp
libmod_democube_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_democube_la_LDFLAGS =
libmod_democube_la_LIBADD = 
libmod_democube_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = Effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

