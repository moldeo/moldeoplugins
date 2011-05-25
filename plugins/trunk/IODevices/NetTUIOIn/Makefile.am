lib_LTLIBRARIES = libmoldeo_nettuioin.la
libmoldeo_nettuioin_la_SOURCES = moNetTUIOIn.cpp moNetTUIOIn.h
libmoldeo_nettuioin_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_nettuioin_la_LDFLAGS =
libmoldeo_nettuioin_la_LIBADD = 
libmoldeo_nettuioin_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

