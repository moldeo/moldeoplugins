lib_LTLIBRARIES = libmoldeo_opencv.la
libmoldeo_opencv_la_SOURCES = moOpenCV.cpp moOpenCV.h

libmoldeo_opencv_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_opencv_la_LDFLAGS =
libmoldeo_opencv_la_LIBADD = 
libmoldeo_opencv_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = resources
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

