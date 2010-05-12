lib_LTLIBRARIES = libmod_sampletest.la
libmod_sampletest_la_SOURCES = moCalibrationGrid.cpp  moEffectSampleTest.cpp
libmod_sampletest_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_sampletest_la_LDFLAGS =
libmod_sampletest_la_LIBADD = 
libmod_sampletest_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effect
datadir = ${prefix}/share/moldeo
libdir = ${exec_prefix}/lib/moldeo/${moduletype}
modulesdir = ${libdir}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

