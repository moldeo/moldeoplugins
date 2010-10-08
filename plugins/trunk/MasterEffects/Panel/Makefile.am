lib_LTLIBRARIES = libmod_panel.la
libmod_panel_la_SOURCES = moMasterEffectPanel.cpp
libmod_panel_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_panel_la_LDFLAGS =
libmod_panel_la_LIBADD = 
libmod_panel_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = MasterEffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

