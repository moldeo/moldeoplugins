lib_LTLIBRARIES = libmoldeo_netoscout.la
libmoldeo_netoscout_la_SOURCES = moNetOSCOut.cpp\
moNetOSCOut.h
libmoldeo_netoscout_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}  ${LIBLO_CFLAGS}
libmoldeo_netoscout_la_LDFLAGS = ${LIBLO_LIBS}
libmoldeo_netoscout_la_LIBADD =
libmoldeo_netoscout_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

