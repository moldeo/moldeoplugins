ACLOCAL_AMFLAGS=-I m4

if DEBUG
DFLAGS = -g3 -O0 -fpermissive
else
DFLAGS = -O2 -fpermissive
endif

lib_LTLIBRARIES = libmoldeo_particlesfractal.la
libmoldeo_particlesfractal_la_SOURCES = moEffectParticlesFractal.cpp moEffectParticlesFractal.h moEffectParticlesFractalFactory.cpp moEffectParticlesFractalFactory.h
libmoldeo_particlesfractal_la_CPPFLAGS = ${DFLAGS} ${LIBMOLDEO_CFLAGS}
libmoldeo_particlesfractal_la_LDFLAGS =
libmoldeo_particlesfractal_la_LIBADD = 
libmoldeo_particlesfractal_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

