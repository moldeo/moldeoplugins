lib_LTLIBRARIES = libmod_text.la
libmod_text_la_SOURCES = moEffectText.cpp
libmod_text_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_text_la_LDFLAGS =
libmod_text_la_LIBADD = 
libmod_text_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = Effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

