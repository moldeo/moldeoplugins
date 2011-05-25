lib_LTLIBRARIES = libmoldeo_debug.la
libmoldeo_debug_la_SOURCES = moPostEffectDebug.cpp moPostEffectDebug.h
libmoldeo_debug_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_debug_la_LDFLAGS =
libmoldeo_debug_la_LIBADD = 
libmoldeo_debug_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = posteffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

