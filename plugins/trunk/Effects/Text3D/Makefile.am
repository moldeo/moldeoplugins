lib_LTLIBRARIES = libmoldeo_text3d.la
libmoldeo_text3d_la_SOURCES = moEffectText3D.cpp moEffectText3D.h
libmoldeo_text3d_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_text3d_la_LDFLAGS =
libmoldeo_text3d_la_LIBADD = 
libmoldeo_text3d_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

