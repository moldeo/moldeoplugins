lib_LTLIBRARIES = libmoldeo_keyboard.la
libmoldeo_keyboard_la_SOURCES = moKeyboard.cpp moKeyboard.h
libmoldeo_keyboard_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_keyboard_la_LDFLAGS =
libmoldeo_keyboard_la_LIBADD = 
libmoldeo_keyboard_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

