lib_LTLIBRARIES = libmoldeo_mixer.la
libmoldeo_mixer_la_SOURCES = moMixer.cpp moMixer.h
libmoldeo_mixer_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_mixer_la_LDFLAGS =
libmoldeo_mixer_la_LIBADD = 
libmoldeo_mixer_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

