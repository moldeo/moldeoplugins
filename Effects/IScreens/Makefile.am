lib_LTLIBRARIES = libmoldeo_iscreens.la
libmoldeo_iscreens_la_SOURCES = moEffectIScreens.cpp
libmoldeo_iscreens_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_iscreens_la_LDFLAGS =
libmoldeo_iscreens_la_LIBADD = 
libmoldeo_iscreens_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

