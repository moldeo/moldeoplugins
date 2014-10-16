lib_LTLIBRARIES = libmoldeo_democube.la
libmoldeo_democube_la_SOURCES = moEffectDemocube.cpp
libmoldeo_democube_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_democube_la_LDFLAGS =
libmoldeo_democube_la_LIBADD = 
libmoldeo_democube_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

