lib_LTLIBRARIES = libmod_videowallmorphing.la
libmod_videowallmorphing_la_SOURCES = moPostEffectVideoWallMorphing.cpp moPostEffectVideoWallMorphing.h
libmod_videowallmorphing_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_videowallmorphing_la_LDFLAGS =
libmod_videowallmorphing_la_LIBADD = 
libmod_videowallmorphing_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = posteffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

