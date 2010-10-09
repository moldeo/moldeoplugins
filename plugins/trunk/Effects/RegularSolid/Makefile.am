lib_LTLIBRARIES = libmod_regularsolid.la
libmod_regularsolid_la_SOURCES = moEffectRegularSolid.cpp
libmod_regularsolid_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_regularsolid_la_LDFLAGS =
libmod_regularsolid_la_LIBADD = 
libmod_regularsolid_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = Effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

