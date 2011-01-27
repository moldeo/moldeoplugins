lib_LTLIBRARIES = libmod_tabletasync.la
libmod_tabletasync_la_SOURCES = moTabletAsync.cpp moTabletAsync.h
libmod_tabletasync_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_tabletasync_la_LDFLAGS =
libmod_tabletasync_la_LIBADD = 
libmod_tabletasync_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

