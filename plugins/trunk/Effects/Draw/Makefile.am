lib_LTLIBRARIES = libmod_draw.la
libmod_draw_la_SOURCES = moEffectDraw.cpp
libmod_draw_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_draw_la_LDFLAGS =
libmod_draw_la_LIBADD = 
libmod_draw_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = Effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

