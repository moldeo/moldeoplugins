lib_LTLIBRARIES = libmod_trackerklt.la
libmod_trackerklt_la_SOURCES = moTrackerKLT.cpp \
moTrackerKLT.h \
klt/base.h\
klt/convolve.c\
klt/convolve.h\
klt/error.c\
klt/error.h\
klt/klt.c\
klt/klt.h\
klt/klt_util.c\
klt/klt_util.h\
klt/pnmio.c\
klt/pnmio.h\
klt/pyramid.c\
klt/pyramid.h\
klt/selectGoodFeatures.c\
klt/storeFeatures.c\
klt/trackFeatures.c\
klt/writeFeatures.c

libmod_trackerklt_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS} \
	-I ./klt
libmod_trackerklt_la_LDFLAGS =
libmod_trackerklt_la_LIBADD = 
libmod_trackerklt_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = resources
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

