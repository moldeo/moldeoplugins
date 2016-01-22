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

lib_LTLIBRARIES = libmoldeo_plane.la
libmoldeo_plane_la_SOURCES = moEffectPlane.cpp moEffectPlane.h
libmoldeo_plane_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_plane_la_LDFLAGS =
libmoldeo_plane_la_LIBADD = 
libmoldeo_plane_la_LIBS = ${LIBMOLDEO_LIBS}

libmoldeo_image_includedir = $(includedir)/moldeo/${moduletype}/plane
libmoldeo_image_include_HEADERS = moEffectPlane.h

METASOURCES = AUTO