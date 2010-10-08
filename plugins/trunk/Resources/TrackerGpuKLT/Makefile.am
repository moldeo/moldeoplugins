lib_LTLIBRARIES = libmod_trackergpuklt.la
libmod_trackergpuklt_la_SOURCES = moTrackerGpuKLT.cpp
libmod_trackergpuklt_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_trackergpuklt_la_LDFLAGS =
libmod_trackergpuklt_la_LIBADD = 
libmod_trackergpuklt_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = Resources
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

