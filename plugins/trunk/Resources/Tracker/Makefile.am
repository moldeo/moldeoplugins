lib_LTLIBRARIES = libmod_tracker.la
libmod_tracker_la_SOURCES = moTracker.cpp moTracker.h
libmod_tracker_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_tracker_la_LDFLAGS =
libmod_tracker_la_LIBADD = 
libmod_tracker_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = resources
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

