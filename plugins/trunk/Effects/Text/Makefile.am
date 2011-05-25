lib_LTLIBRARIES = libmoldeo_text.la
libmoldeo_text_la_SOURCES = moEffectText.cpp moEffectText.h
libmoldeo_text_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_text_la_LDFLAGS =
libmoldeo_text_la_LIBADD = 
libmoldeo_text_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

