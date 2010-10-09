lib_LTLIBRARIES = libmod_imageflow.la
libmod_imageflow_la_SOURCES = moEffectImageFlow.cpp
libmod_imageflow_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_imageflow_la_LDFLAGS =
libmod_imageflow_la_LIBADD = 
libmod_imageflow_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

