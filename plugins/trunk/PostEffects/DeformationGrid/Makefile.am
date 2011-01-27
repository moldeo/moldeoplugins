lib_LTLIBRARIES = libmod_deformationgrid.la
libmod_deformationgrid_la_SOURCES = moDeformationGrid.cpp moDeformationGrid.h
libmod_deformationgrid_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_deformationgrid_la_LDFLAGS =
libmod_deformationgrid_la_LIBADD = 
libmod_deformationgrid_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = posteffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

