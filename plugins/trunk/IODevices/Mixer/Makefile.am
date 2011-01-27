lib_LTLIBRARIES = libmod_mixer.la
libmod_mixer_la_SOURCES = moMixer.cpp moMixer.h
libmod_mixer_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_mixer_la_LDFLAGS =
libmod_mixer_la_LIBADD = 
libmod_mixer_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

