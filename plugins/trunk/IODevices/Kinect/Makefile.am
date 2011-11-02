lib_LTLIBRARIES = libmoldeo_kinect.la
libmoldeo_kinect_la_SOURCES = moKinect.cpp moKinect.h
libmoldeo_kinect_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS} ${OPENNI_CFLAGS} ${PCLCOMMON_CFLAGS} ${PCLIO_CFLAGS}
libmoldeo_kinect_la_LDFLAGS = ${OPENNI_LIBS} ${PCLCOMMON_LIBS} ${PCLIO_LIBS} -lpcl_features -lpcl_kdtree -lpcl_segmentation -lpcl_surface -lpcl_registration -lpcl_sample_consensus -lpcl_range_image -lpcl_filters
libmoldeo_kinect_la_LIBADD = 
libmoldeo_Kinect_la_LIBS = ${LIBMOLDEO_LIBS} ${OPENNI_LIBS} ${PCLCOMMON_LIBS} ${PCLIO_LIBS} -lpcl_features -lpcl_kdtree -lpcl_segmentation -lpcl_surface -lpcl_registration -lpcl_sample_consensus -lpcl_range_image -lpcl_filters

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

