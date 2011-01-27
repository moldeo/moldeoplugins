lib_LTLIBRARIES = libmod_calibrationgrid.la
libmod_calibrationgrid_la_SOURCES = moCalibrationGrid.cpp moCalibrationGrid.h
libmod_calibrationgrid_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_calibrationgrid_la_LDFLAGS =
libmod_calibrationgrid_la_LIBADD = 
libmod_calibrationgrid_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = posteffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

