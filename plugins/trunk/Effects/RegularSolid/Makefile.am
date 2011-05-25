lib_LTLIBRARIES = libmoldeo_regularsolid.la
libmoldeo_regularsolid_la_SOURCES = moEffectRegularSolid.cpp moEffectRegularSolid.h
libmoldeo_regularsolid_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_regularsolid_la_LDFLAGS =
libmoldeo_regularsolid_la_LIBADD = 
libmoldeo_regularsolid_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

