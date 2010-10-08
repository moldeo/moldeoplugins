lib_LTLIBRARIES = libmod_opencv.la
libmod_opencv_la_SOURCES = moOpenCV.cpp
libmod_opencv_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_opencv_la_LDFLAGS =
libmod_opencv_la_LIBADD = 
libmod_opencv_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = Resources
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

