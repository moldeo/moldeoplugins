lib_LTLIBRARIES = libmoldeo_imageflow.la
libmoldeo_imageflow_la_SOURCES = moEffectImageFlow.cpp moEffectImageFlow.h
libmoldeo_imageflow_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_imageflow_la_LDFLAGS =
libmoldeo_imageflow_la_LIBADD = 
libmoldeo_imageflow_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

