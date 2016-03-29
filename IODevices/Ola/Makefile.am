ACLOCAL_AMFLAGS=-I m4

if DEBUG
DFLAGS = -g3 -O0 -fpermissive
else
DFLAGS = -O2 -fpermissive
endif

moduletype = iodevices
datadir = ${prefix}/share/moldeo/data
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = ${DFLAGS} ${HOSTCFLAGS} $(all_includes) -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
AM_CXXFLAGS = ${AM_CPPFLAGS}
AM_LDFLAGS = -ldl ${HOSTLDFLAGS}

lib_LTLIBRARIES = libmoldeo_ola.la
libmoldeo_ola_la_SOURCES = moOla.cpp\
moOla.h
libmoldeo_ola_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS} ${LIBOLA_CFLAGS}
libmoldeo_ola_la_LDFLAGS =
libmoldeo_ola_la_LIBADD = ${LIBOLA_LIBS}
libmoldeo_ola_la_LIBS = ${LIBMOLDEO_LIBS}

libmoldeo_ola_includedir = $(includedir)/moldeo/${moduletype}/ola
libmoldeo_ola_include_HEADERS = moOla.h

METASOURCES = AUTO
