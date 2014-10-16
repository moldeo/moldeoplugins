lib_LTLIBRARIES = libmoldeo_painter.la
libmoldeo_painter_la_SOURCES = moEffectPainter.cpp moEffectPainter.h
libmoldeo_painter_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_painter_la_LDFLAGS =
libmoldeo_painter_la_LIBADD = 
libmoldeo_painter_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

