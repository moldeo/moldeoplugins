lib_LTLIBRARIES = libmod_channel.la
libmod_channel_la_SOURCES = moMasterEffectChannel.cpp
libmod_channel_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_channel_la_LDFLAGS =
libmod_channel_la_LIBADD = 
libmod_channel_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = mastereffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

