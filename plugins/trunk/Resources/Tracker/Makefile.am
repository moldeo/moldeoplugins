lib_LTLIBRARIES = libmoldeo_tracker.la
libmoldeo_tracker_la_SOURCES = moTracker.cpp moTracker.h
libmoldeo_tracker_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_tracker_la_LDFLAGS =
libmoldeo_tracker_la_LIBADD = 
libmoldeo_tracker_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = resources
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

