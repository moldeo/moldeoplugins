lib_LTLIBRARIES = libmod_icon3d.la
libmod_icon3d_la_SOURCES = moEffectIcon3D.cpp moEffectIcon3D.h
libmod_icon3d_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_icon3d_la_LDFLAGS =
libmod_icon3d_la_LIBADD = 
libmod_icon3d_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

