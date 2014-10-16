lib_LTLIBRARIES = libmoldeo_videowallmorphing.la
libmoldeo_videowallmorphing_la_SOURCES = moPostEffectVideoWallMorphing.cpp moPostEffectVideoWallMorphing.h
libmoldeo_videowallmorphing_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_videowallmorphing_la_LDFLAGS =
libmoldeo_videowallmorphing_la_LIBADD = 
libmoldeo_videowallmorphing_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = posteffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

