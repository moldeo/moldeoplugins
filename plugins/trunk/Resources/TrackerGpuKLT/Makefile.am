lib_LTLIBRARIES = libmoldeo_trackergpuklt.la
libmoldeo_trackergpuklt_la_SOURCES = moTrackerGpuKLT.cpp moTrackerGpuKLT.h
libmoldeo_trackergpuklt_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_trackergpuklt_la_LDFLAGS =
libmoldeo_trackergpuklt_la_LIBADD = 
libmoldeo_trackergpuklt_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = resources
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

