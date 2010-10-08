lib_LTLIBRARIES = libmod_particlessimple.la
libmod_particlessimple_la_SOURCES = moEffectParticlesSimple.cpp
libmod_particlessimple_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_particlessimple_la_LDFLAGS =
libmod_particlessimple_la_LIBADD = 
libmod_particlessimple_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = Effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

