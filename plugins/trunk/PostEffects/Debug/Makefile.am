lib_LTLIBRARIES = libmod_debug.la
libmod_debug_la_SOURCES = moPostEffectDebug.cpp
libmod_debug_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_debug_la_LDFLAGS =
libmod_debug_la_LIBADD = 
libmod_debug_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = posteffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

