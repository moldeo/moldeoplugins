lib_LTLIBRARIES = libmoldeo_draw.la
libmoldeo_draw_la_SOURCES = moEffectDraw.cpp moEffectDraw.h
libmoldeo_draw_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_draw_la_LDFLAGS =
libmoldeo_draw_la_LIBADD = 
libmoldeo_draw_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

