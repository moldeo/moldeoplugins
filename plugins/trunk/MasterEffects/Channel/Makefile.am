lib_LTLIBRARIES = libmoldeo_channel.la
libmoldeo_channel_la_SOURCES = moMasterEffectChannel.cpp moMasterEffectChannel.h
libmoldeo_channel_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_channel_la_LDFLAGS =
libmoldeo_channel_la_LIBADD = 
libmoldeo_channel_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = mastereffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

