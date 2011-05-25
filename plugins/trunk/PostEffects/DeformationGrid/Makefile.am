lib_LTLIBRARIES = libmoldeo_deformationgrid.la
libmoldeo_deformationgrid_la_SOURCES = moDeformationGrid.cpp moDeformationGrid.h
libmoldeo_deformationgrid_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_deformationgrid_la_LDFLAGS =
libmoldeo_deformationgrid_la_LIBADD = 
libmoldeo_deformationgrid_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = posteffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

