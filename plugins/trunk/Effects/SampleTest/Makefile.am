lib_LTLIBRARIES = libmod_sampletest.la
libmod_sampletest_la_SOURCES = moEffectSampleTest.cpp moEffectSampleTest.h
libmod_sampletest_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_sampletest_la_LDFLAGS =
libmod_sampletest_la_LIBADD = 
libmod_sampletest_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

