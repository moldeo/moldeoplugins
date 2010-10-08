lib_LTLIBRARIES = libmod_trackerklt.la
libmod_trackerklt_la_SOURCES = moTrackerKLT.cpp
libmod_trackerklt_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_trackerklt_la_LDFLAGS =
libmod_trackerklt_la_LIBADD = 
libmod_trackerklt_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = Resources
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

