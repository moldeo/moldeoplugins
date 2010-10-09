lib_LTLIBRARIES = libmod_nettuioin.la
libmod_nettuioin_la_SOURCES = moNetTUIOIn.cpp
libmod_nettuioin_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_nettuioin_la_LDFLAGS =
libmod_nettuioin_la_LIBADD = 
libmod_nettuioin_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

