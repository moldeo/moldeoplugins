ACLOCAL_AMFLAGS=-I m4

if DEBUG
DFLAGS = -g3 -O0 -fpermissive
else
DFLAGS = -O2 -fpermissive
endif

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\" 


lib_LTLIBRARIES = libmoldeo_assimp.la
libmoldeo_assimp_la_SOURCES = moEffectAssimp.cpp moEffectAssimp.h
libmoldeo_assimp_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS} ${LIBASSIMP_CFLAGS}
libmoldeo_assimp_la_LDFLAGS =
libmoldeo_assimp_la_LIBADD = ${LIBMOLDEO_LIBS} ${LIBASSIMP_LIBS}
libmoldeo_assimp_la_LIBS = ${LIBMOLDEO_LIBS} ${LIBASSIMP_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

