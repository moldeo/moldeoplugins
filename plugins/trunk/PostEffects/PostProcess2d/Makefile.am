lib_LTLIBRARIES = libmod_postprocess2d.la
libmod_postprocess2d_la_SOURCES = moPostEffectPostProcess2d.cpp moPostEffectPostProcess2d.h
libmod_postprocess2d_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_postprocess2d_la_LDFLAGS =
libmod_postprocess2d_la_LIBADD = 
libmod_postprocess2d_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = posteffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

