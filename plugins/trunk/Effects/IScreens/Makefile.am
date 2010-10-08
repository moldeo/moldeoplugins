lib_LTLIBRARIES = libmod_iscreens.la
libmod_iscreens_la_SOURCES = moEffectIScreens.cpp
libmod_iscreens_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_iscreens_la_LDFLAGS =
libmod_iscreens_la_LIBADD = 
libmod_iscreens_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = Effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

