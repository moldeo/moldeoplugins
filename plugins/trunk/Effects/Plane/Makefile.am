lib_LTLIBRARIES = libmod_plane.la
libmod_plane_la_SOURCES = moEffectPlane.cpp moEffectPlane.h
libmod_plane_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_plane_la_LDFLAGS =
libmod_plane_la_LIBADD = 
libmod_plane_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

