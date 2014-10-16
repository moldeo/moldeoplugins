lib_LTLIBRARIES = libmoldeo_mouse.la
libmoldeo_mouse_la_SOURCES = moMouse.cpp moMouse.h
libmoldeo_mouse_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_mouse_la_LDFLAGS =
libmoldeo_mouse_la_LIBADD = 
libmoldeo_mouse_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

