lib_LTLIBRARIES = libmoldeo_videowall.la
libmoldeo_videowall_la_SOURCES = moPostEffectVideoWall.cpp moPostEffectVideoWall.h
libmoldeo_videowall_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_videowall_la_LDFLAGS =
libmoldeo_videowall_la_LIBADD = 
libmoldeo_videowall_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = posteffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

