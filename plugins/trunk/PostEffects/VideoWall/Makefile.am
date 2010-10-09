lib_LTLIBRARIES = libmod_videowall.la
libmod_videowall_la_SOURCES = moPostEffectVideoWall.cpp
libmod_videowall_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_videowall_la_LDFLAGS =
libmod_videowall_la_LIBADD = 
libmod_videowall_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = posteffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

