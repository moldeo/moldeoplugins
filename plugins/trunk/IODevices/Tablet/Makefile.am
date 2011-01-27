lib_LTLIBRARIES = libmod_tablet.la
libmod_tablet_la_SOURCES = moTablet.cpp moTablet.h
libmod_tablet_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_tablet_la_LDFLAGS =
libmod_tablet_la_LIBADD = 
libmod_tablet_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

