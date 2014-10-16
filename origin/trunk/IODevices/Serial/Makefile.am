lib_LTLIBRARIES = libmoldeo_serial.la
libmoldeo_serial_la_SOURCES = moSerial.cpp moSerial.h
libmoldeo_serial_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_serial_la_LDFLAGS =
libmoldeo_serial_la_LIBADD = 
libmoldeo_serial_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

