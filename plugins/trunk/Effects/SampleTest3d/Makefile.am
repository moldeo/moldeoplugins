lib_LTLIBRARIES = libmoldeo_sampletest3d.la
libmoldeo_sampletest3d_la_SOURCES = moEffectSampleTest3d.cpp moEffectSampleTest3d.h
libmoldeo_sampletest3d_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_sampletest3d_la_LDFLAGS =
libmoldeo_sampletest3d_la_LIBADD = 
libmoldeo_sampletest3d_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

