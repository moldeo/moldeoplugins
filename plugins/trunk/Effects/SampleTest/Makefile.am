lib_LTLIBRARIES = libmoldeo_sampletest.la
libmoldeo_sampletest_la_SOURCES = moEffectSampleTest.cpp moEffectSampleTest.h
libmoldeo_sampletest_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_sampletest_la_LDFLAGS =
libmoldeo_sampletest_la_LIBADD = 
libmoldeo_sampletest_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

