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

lib_LTLIBRARIES = libmoldeo_image.la
libmoldeo_image_la_SOURCES = moEffectImage.cpp moEffectImage.h
libmoldeo_image_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_image_la_LDFLAGS =
libmoldeo_image_la_LIBADD = 
libmoldeo_image_la_LIBS = ${LIBMOLDEO_LIBS}


