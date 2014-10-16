lib_LTLIBRARIES = libmoldeo_scene3d.la
libmoldeo_scene3d_la_SOURCES = moEffectScene3D.cpp moEffectScene3D.h
libmoldeo_scene3d_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_scene3d_la_LDFLAGS =
libmoldeo_scene3d_la_LIBADD = 
libmoldeo_scene3d_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

