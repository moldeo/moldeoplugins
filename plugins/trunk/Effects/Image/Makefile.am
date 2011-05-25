lib_LTLIBRARIES = libmoldeo_image.la
libmoldeo_image_la_SOURCES = moEffectImage.cpp moEffectImage.h
libmoldeo_image_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_image_la_LDFLAGS =
libmoldeo_image_la_LIBADD = 
libmoldeo_image_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

