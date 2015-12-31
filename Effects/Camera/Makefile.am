lib_LTLIBRARIES = libmoldeo_camera.la
libmoldeo_camera_la_SOURCES = moEffectCamera.cpp moEffectCamera.h
libmoldeo_camera_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_camera_la_LDFLAGS =
libmoldeo_camera_la_LIBADD = 
libmoldeo_camera_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo/data
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

