ACLOCAL_AMFLAGS=-I m4

if DEBUG
DFLAGS = -g3 -O0 -fpermissive
else
DFLAGS = -O2 -fpermissive
endif

moduletype = preeffects
datadir = ${prefix}/share/moldeo/data
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = ${DFLAGS} ${HOSTCFLAGS} $(all_includes) -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
AM_CXXFLAGS = ${AM_CPPFLAGS}
AM_LDFLAGS = -ldl ${HOSTLDFLAGS}

lib_LTLIBRARIES = libmoldeo_erase.la
libmoldeo_erase_la_SOURCES = moPreEffectErase.cpp moPreEffectErase.h
libmoldeo_erase_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_erase_la_LDFLAGS =
libmoldeo_erase_la_LIBADD = 
libmoldeo_erase_la_LIBS = ${LIBMOLDEO_LIBS}

libmoldeo_erase_includedir = $(includedir)/moldeo/${moduletype}/erase
libmoldeo_erase_include_HEADERS = moPreEffectErase.h

METASOURCES = AUTO
