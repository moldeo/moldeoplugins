lib_LTLIBRARIES = libmoldeo_postprocess2d.la
libmoldeo_postprocess2d_la_SOURCES = moPostEffectPostProcess2d.cpp moPostEffectPostProcess2d.h
libmoldeo_postprocess2d_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_postprocess2d_la_LDFLAGS =
libmoldeo_postprocess2d_la_LIBADD = 
libmoldeo_postprocess2d_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = posteffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

