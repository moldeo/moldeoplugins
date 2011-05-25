lib_LTLIBRARIES = libmoldeo_erase.la
libmoldeo_erase_la_SOURCES = moPreEffectErase.cpp moPreEffectErase.h
libmoldeo_erase_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_erase_la_LDFLAGS =
libmoldeo_erase_la_LIBADD = 
libmoldeo_erase_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = preeffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4


libmoldeo_erase_includedir = $(includedir)/moldeo/${moduletype}/erase
libmoldeo_erase_include_HEADERS = moPreEffectErase.h

METASOURCES = AUTO
