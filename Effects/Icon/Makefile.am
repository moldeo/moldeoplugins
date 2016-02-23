ACLOCAL_AMFLAGS=-I m4

if DEBUG
DFLAGS = -g3 -O0 -fpermissive
else
DFLAGS = -O2 -fpermissive
endif

moduletype = effects
datadir = ${prefix}/share/moldeo/data
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = ${DFLAGS} ${HOSTCFLAGS} $(all_includes) -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
AM_CXXFLAGS = ${AM_CPPFLAGS}
AM_LDFLAGS = -ldl ${HOSTLDFLAGS}

lib_LTLIBRARIES = libmoldeo_icon.la
libmoldeo_icon_la_SOURCES = moEffectIcon.cpp moEffectIcon.h
libmoldeo_icon_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_icon_la_LDFLAGS =
libmoldeo_icon_la_LIBADD =
libmoldeo_icon_la_LIBS = ${LIBMOLDEO_LIBS}

libmoldeo_icon_includedir = $(includedir)/moldeo/${moduletype}/image
libmoldeo_icon_include_HEADERS = moEffectIcon.h

METASOURCES = AUTO
