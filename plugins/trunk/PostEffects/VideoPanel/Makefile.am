lib_LTLIBRARIES = libmoldeo_videopanel.la
libmoldeo_videopanel_la_SOURCES = moPostEffectVideoPanel.cpp moPostEffectVideoPanel.h
libmoldeo_videopanel_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_videopanel_la_LDFLAGS =
libmoldeo_videopanel_la_LIBADD = 
libmoldeo_videopanel_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = posteffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

