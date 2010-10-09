lib_LTLIBRARIES = libmod_scene3d.la
libmod_scene3d_la_SOURCES = moEffectScene3D.cpp
libmod_scene3d_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_scene3d_la_LDFLAGS =
libmod_scene3d_la_LIBADD = 
libmod_scene3d_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

