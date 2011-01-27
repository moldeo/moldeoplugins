lib_LTLIBRARIES = libmod_videopanel.la
libmod_videopanel_la_SOURCES = moPostEffectVideoPanel.cpp moPostEffectVideoPanel.h
libmod_videopanel_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_videopanel_la_LDFLAGS =
libmod_videopanel_la_LIBADD = 
libmod_videopanel_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = posteffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

