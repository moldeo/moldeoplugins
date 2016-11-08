lib_LTLIBRARIES = libmoldeo_particlessimple.la
libmoldeo_particlessimple_la_SOURCES = moEffectParticlesSimple.cpp moEffectParticlesSimple.h
libmoldeo_particlessimple_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_particlessimple_la_LDFLAGS =
libmoldeo_particlessimple_la_LIBADD = 
libmoldeo_particlessimple_la_LIBS = ${LIBMOLDEO_LIBS} -lglut

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

