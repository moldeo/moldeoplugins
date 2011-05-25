lib_LTLIBRARIES = libmoldeo_masterchannel.la
libmoldeo_masterchannel_la_SOURCES = moMasterEffectMasterChannel.cpp moMasterEffectMasterChannel.h
libmoldeo_masterchannel_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_masterchannel_la_LDFLAGS =
libmoldeo_masterchannel_la_LIBADD = 
libmoldeo_masterchannel_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = mastereffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

