lib_LTLIBRARIES = libmoldeo_cubemap.la
libmoldeo_cubemap_la_SOURCES = moEffectCubeMap.cpp moEffectCubeMap.h
libmoldeo_cubemap_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_cubemap_la_LDFLAGS =
libmoldeo_cubemap_la_LIBADD = 
libmoldeo_cubemap_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

