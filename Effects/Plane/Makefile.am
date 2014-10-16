lib_LTLIBRARIES = libmoldeo_plane.la
libmoldeo_plane_la_SOURCES = moEffectPlane.cpp moEffectPlane.h
libmoldeo_plane_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_plane_la_LDFLAGS =
libmoldeo_plane_la_LIBADD = 
libmoldeo_plane_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

