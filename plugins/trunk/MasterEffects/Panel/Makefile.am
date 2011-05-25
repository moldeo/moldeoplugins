lib_LTLIBRARIES = libmoldeo_panel.la
libmoldeo_panel_la_SOURCES = moMasterEffectPanel.cpp moMasterEffectPanel.h
libmoldeo_panel_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_panel_la_LDFLAGS =
libmoldeo_panel_la_LIBADD = 
libmoldeo_panel_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = mastereffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

