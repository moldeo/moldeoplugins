lib_LTLIBRARIES = libmod_masterchannel.la
libmod_masterchannel_la_SOURCES = moMasterEffectMasterChannel.cpp
libmod_masterchannel_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_masterchannel_la_LDFLAGS =
libmod_masterchannel_la_LIBADD = 
libmod_masterchannel_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = mastereffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

