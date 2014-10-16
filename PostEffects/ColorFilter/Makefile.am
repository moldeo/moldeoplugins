lib_LTLIBRARIES = libmoldeo_colorfilter.la
libmoldeo_colorfilter_la_SOURCES = moPostEffectColorFilter.cpp moPostEffectColorFilter.h
libmoldeo_colorfilter_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_colorfilter_la_LDFLAGS =
libmoldeo_colorfilter_la_LIBADD = 
libmoldeo_colorfilter_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = posteffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

