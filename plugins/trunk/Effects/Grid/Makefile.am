lib_LTLIBRARIES = libmod_grid.la
libmod_grid_la_SOURCES = moEffectGrid.cpp
libmod_grid_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_grid_la_LDFLAGS =
libmod_grid_la_LIBADD = 
libmod_grid_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

