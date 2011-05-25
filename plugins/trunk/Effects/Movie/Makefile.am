lib_LTLIBRARIES = libmoldeo_movie.la
libmoldeo_movie_la_SOURCES = moEffectMovie.cpp moEffectMovie.h
libmoldeo_movie_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_movie_la_LDFLAGS =
libmoldeo_movie_la_LIBADD = 
libmoldeo_movie_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

