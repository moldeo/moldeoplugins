lib_LTLIBRARIES = libmod_`'PLUGINNAME`'.la
libmod_`'PLUGINNAME`'_la_SOURCES = PLUGINSOURCES
libmod_`'PLUGINNAME`'_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_`'PLUGINNAME`'_la_LDFLAGS =
libmod_`'PLUGINNAME`'_la_LIBADD = 
libmod_`'PLUGINNAME`'_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = PLUGINTYPE
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

