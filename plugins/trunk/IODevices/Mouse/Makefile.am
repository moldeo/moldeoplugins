lib_LTLIBRARIES = libmod_mouse.la
libmod_mouse_la_SOURCES = moMouse.cpp moMouse.h
libmod_mouse_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_mouse_la_LDFLAGS =
libmod_mouse_la_LIBADD = 
libmod_mouse_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

