lib_LTLIBRARIES = libmod_cubemap.la
libmod_cubemap_la_SOURCES = moEffectCubeMap.cpp *.c
libmod_cubemap_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_cubemap_la_LDFLAGS =
libmod_cubemap_la_LIBADD = 
libmod_cubemap_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

