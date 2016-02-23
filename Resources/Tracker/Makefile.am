ACLOCAL_AMFLAGS=-I m4

if DEBUG
DFLAGS = -g3 -O0 -fpermissive
else
DFLAGS = -O2 -fpermissive
endif

moduletype = resources
datadir = ${prefix}/share/moldeo/data
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = ${DFLAGS} ${HOSTCFLAGS} $(all_includes) -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
AM_CXXFLAGS = ${AM_CPPFLAGS}
AM_LDFLAGS = -ldl ${HOSTLDFLAGS}

lib_LTLIBRARIES = libmoldeo_tracker.la
libmoldeo_tracker_la_SOURCES = moTracker.cpp moTracker.h
libmoldeo_tracker_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_tracker_la_LDFLAGS =
libmoldeo_tracker_la_LIBADD =
libmoldeo_tracker_la_LIBS = ${LIBMOLDEO_LIBS}

libmoldeo_tracker_includedir = $(includedir)/moldeo/${moduletype}/tracker
libmoldeo_tracker_include_HEADERS = moTracker.h

METASOURCES = AUTO
