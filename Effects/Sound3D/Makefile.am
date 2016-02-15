
ACLOCAL_AMFLAGS=-I m4

if DEBUG
DFLAGS = -g3 -O0 -fpermissive
else
DFLAGS = -O2 -fpermissive
endif

datadir = ${prefix}/share/moldeo/data
modulesdir = ${libdir}/moldeo
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = ${DFLAGS} ${HOSTCFLAGS} $(all_includes) -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
AM_CXXFLAGS = ${AM_CPPFLAGS}
AM_LDFLAGS = -ldl ${HOSTLDFLAGS}

lib_LTLIBRARIES = libmoldeo_sound3d.la
libmoldeo_sound3d_la_SOURCES = moEffectSound3D.cpp moEffectSound3D.h
libmoldeo_sound3d_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS} ${FREEALUT_CFLAGS} ${SDL2MIXER_CFLAGS}
libmoldeo_sound3d_la_LDFLAGS =
libmoldeo_sound3d_la_LIBADD = ${LIBMOLDEO_LIBS} ${FREEALUT_LIBS} ${SDL2MIXER_LIBS}
libmoldeo_sound3d_la_LIBS = 


moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo


