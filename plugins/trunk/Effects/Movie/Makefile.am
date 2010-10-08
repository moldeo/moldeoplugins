lib_LTLIBRARIES = libmod_movie.la
libmod_movie_la_SOURCES = moEffectMovie.cpp
libmod_movie_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_movie_la_LDFLAGS =
libmod_movie_la_LIBADD = 
libmod_movie_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = Effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

