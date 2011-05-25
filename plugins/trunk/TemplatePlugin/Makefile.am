lib_LTLIBRARIES = libmoldeo_`'PLUGINID`'.la
libmoldeo_`'PLUGINID`'_la_SOURCES = PLUGINSOURCES
libmoldeo_`'PLUGINID`'_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_`'PLUGINID`'_la_LDFLAGS =
libmoldeo_`'PLUGINID`'_la_LIBADD = 
libmoldeo_`'PLUGINID`'_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = PLUGINTYPE
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

