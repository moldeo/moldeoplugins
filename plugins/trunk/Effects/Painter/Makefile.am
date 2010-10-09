lib_LTLIBRARIES = libmod_painter.la
libmod_painter_la_SOURCES = moEffectPainter.cpp
libmod_painter_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_painter_la_LDFLAGS =
libmod_painter_la_LIBADD = 
libmod_painter_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = Effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

