lib_LTLIBRARIES = libmoldeo_grid.la
libmoldeo_grid_la_SOURCES = moEffectGrid.cpp moEffectGrid.h \
supereg/pcx.cpp \
supereg/pcx.h \
supereg/sup3d.cpp \
supereg/sup3d.h \
supereg/supereg.h \
supereg/suppal.cpp \
supereg/suppal.h \
supereg/suprand.cpp \
supereg/suprand.h \
supereg/tgrdutil.cpp \
supereg/tgrdutil.h \
supereg/tgrid.cpp \
supereg/tgrid.h \
supereg/tipos.cpp \
supereg/tipos.h \
supereg/tmapa.cpp \
supereg/tmapa.h \
supereg/tmotor.cpp \
supereg/tmotor.h \
supereg/trdrmap.cpp \
supereg/trdrmap.h

libmoldeo_grid_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS} -I ./supereg -fpermissive
libmoldeo_grid_la_LDFLAGS =
libmoldeo_grid_la_LIBADD = 
libmoldeo_grid_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo/data
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

