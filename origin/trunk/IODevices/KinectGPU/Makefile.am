lib_LTLIBRARIES = libmoldeo_kinectgpu.la
libmoldeo_kinect_la_SOURCES = moKinect.cpp moKinect.h
libmoldeo_kinect_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS} -I /usr/include/ni
libmoldeo_kinect_la_LDFLAGS = -lOpenNI
libmoldeo_kinect_la_LIBADD = 
libmoldeo_Kinect_la_LIBS = ${LIBMOLDEO_LIBS} -lOpenNI

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

