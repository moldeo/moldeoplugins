lib_LTLIBRARIES = libmoldeo_lights.la
libmoldeo_lights_la_SOURCES = moEffectLights.cpp moEffectLights.h
libmoldeo_lights_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_lights_la_LDFLAGS =
libmoldeo_lights_la_LIBADD = 
libmoldeo_lights_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

