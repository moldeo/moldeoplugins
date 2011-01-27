lib_LTLIBRARIES = libmod_serial.la
libmod_serial_la_SOURCES = moSerial.cpp moSerial.h
libmod_serial_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_serial_la_LDFLAGS =
libmod_serial_la_LIBADD = 
libmod_serial_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

