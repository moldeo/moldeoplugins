lib_LTLIBRARIES = libmoldeo_netoscin.la
libmoldeo_netoscin_la_SOURCES = moNetOSCIn.cpp\
moNetOSCIn.h

libmoldeo_netoscin_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS} ${LIBLO_CFLAGS} -DOSC_HOST_LITTLE_ENDIAN=1
libmoldeo_netoscin_la_LDFLAGS =
libmoldeo_netoscin_la_LIBADD = ${LIBLO_LIBS}
libmoldeo_netoscin_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

