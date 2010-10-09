lib_LTLIBRARIES = libmod_image.la
libmod_image_la_SOURCES = moEffectImage.cpp
libmod_image_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_image_la_LDFLAGS =
libmod_image_la_LIBADD = 
libmod_image_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = Effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

