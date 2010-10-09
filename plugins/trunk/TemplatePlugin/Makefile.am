lib_LTLIBRARIES = libmod_`'PLUGINID`'.la
libmod_`'PLUGINID`'_la_SOURCES = PLUGINSOURCES
libmod_`'PLUGINID`'_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_`'PLUGINID`'_la_LDFLAGS =
libmod_`'PLUGINID`'_la_LIBADD = 
libmod_`'PLUGINID`'_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = PLUGINTYPE
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

