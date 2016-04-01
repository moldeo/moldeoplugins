lib_LTLIBRARIES = libmoldeo_midi.la
libmoldeo_midi_la_SOURCES = moMidi.cpp moMidi.h
libmoldeo_midi_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS} 
libmoldeo_midi_la_LDFLAGS =
libmoldeo_midi_la_LIBADD = -lportmidi
libmoldeo_midi_la_LIBS =  ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

