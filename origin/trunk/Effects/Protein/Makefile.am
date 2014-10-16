lib_LTLIBRARIES = libmoldeo_protein.la
libmoldeo_protein_la_SOURCES = moEffectProtein.cpp moEffectProtein.h
libmoldeo_protein_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_protein_la_LDFLAGS =
libmoldeo_protein_la_LIBADD = 
libmoldeo_protein_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

