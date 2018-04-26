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

haarcascadesdir = ${datadir}/haarcascades
hogcascadesdir = ${datadir}/hogcascades
lbpcascadesdir = ${datadir}/lbpcascades
vec_filesdir = ${datadir}/vec_files

AM_CPPFLAGS = ${DFLAGS} ${HOSTCFLAGS} $(all_includes) -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\" -I/usr/include/opencv2
AM_CXXFLAGS = ${AM_CPPFLAGS}
AM_LDFLAGS = -ldl ${HOSTLDFLAGS}

lib_LTLIBRARIES = libmoldeo_opencv.la
libmoldeo_opencv_la_SOURCES = moOpenCV.cpp moOpenCV.h
libmoldeo_opencv_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS} ${LIBOPENCV_CFLAGS}
libmoldeo_opencv_la_LDFLAGS = -L/usr/local/share/OpenCV/3rdparty/lib
libmoldeo_opencv_la_LIBADD = ${LIBOPENCV_LIBS} -ltbb -lippicv
libmoldeo_opencv_la_LIBS = ${LIBMOLDEO_LIBS} -ltbb -lippicv

libmoldeo_opencv_includedir = $(includedir)/moldeo/${moduletype}/opencv
libmoldeo_opencv_include_HEADERS = moOpenCV.h

dist_haarcascades_DATA = ./haarcascades/*.*
dist_hogcascades_DATA = ./hogcascades/*.*
dist_lbpcascades_DATA = ./lbpcascades/*.*
dist_vec_files_DATA = ./vec_files/*.*

METASOURCES = AUTO
