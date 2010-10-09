lib_LTLIBRARIES = libmod_erase.la
libmod_erase_la_SOURCES = moPreEffectErase.cpp
libmod_erase_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_erase_la_LDFLAGS =
libmod_erase_la_LIBADD = 
libmod_erase_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = preeffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

