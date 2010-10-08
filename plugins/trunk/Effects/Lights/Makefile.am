lib_LTLIBRARIES = libmod_lights.la
libmod_lights_la_SOURCES = moEffectLights.cpp
libmod_lights_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_lights_la_LDFLAGS =
libmod_lights_la_LIBADD = 
libmod_lights_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = Effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

