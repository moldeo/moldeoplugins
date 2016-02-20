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

AM_CPPFLAGS = ${DFLAGS} ${HOSTCFLAGS} $(all_includes) -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\" -I/usr/include/opencv2
AM_CXXFLAGS = ${AM_CPPFLAGS}
AM_LDFLAGS = -ldl ${HOSTLDFLAGS}

lib_LTLIBRARIES = libmoldeo_opencv.la
libmoldeo_opencv_la_SOURCES = moOpenCV.cpp moOpenCV.h
libmoldeo_opencv_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS} ${LIBOPENCV_CFLAGS}
libmoldeo_opencv_la_LDFLAGS =
libmoldeo_opencv_la_LIBADD =
libmoldeo_opencv_la_LIBS = ${LIBMOLDEO_LIBS} ${LIBOPENCV_LIBS}

libmoldeo_opencv_includedir = $(includedir)/moldeo/${moduletype}/opencv
libmoldeo_opencv_include_HEADERS = moOpenCV.h

METASOURCES = AUTO
