lib_LTLIBRARIES = libmoldeo_tabletasync.la
libmoldeo_tabletasync_la_SOURCES = moTabletAsync.cpp moTabletAsync.h
libmoldeo_tabletasync_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_tabletasync_la_LDFLAGS =
libmoldeo_tabletasync_la_LIBADD = 
libmoldeo_tabletasync_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

