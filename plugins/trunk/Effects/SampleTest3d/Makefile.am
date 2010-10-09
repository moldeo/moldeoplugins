lib_LTLIBRARIES = libmod_sampletest3d.la
libmod_sampletest3d_la_SOURCES = moEffectSampleTest3d.cpp
libmod_sampletest3d_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_sampletest3d_la_LDFLAGS =
libmod_sampletest3d_la_LIBADD = 
libmod_sampletest3d_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = Effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

