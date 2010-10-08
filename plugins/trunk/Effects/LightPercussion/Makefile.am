lib_LTLIBRARIES = libmod_lightpercussion.la
libmod_lightpercussion_la_SOURCES = moEffectLightPercussion.cpp
libmod_lightpercussion_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_lightpercussion_la_LDFLAGS =
libmod_lightpercussion_la_LIBADD = 
libmod_lightpercussion_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = Effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

