lib_LTLIBRARIES = libmoldeo_tablet.la
libmoldeo_tablet_la_SOURCES = moTablet.cpp moTablet.h
libmoldeo_tablet_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_tablet_la_LDFLAGS =
libmoldeo_tablet_la_LIBADD = 
libmoldeo_tablet_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

