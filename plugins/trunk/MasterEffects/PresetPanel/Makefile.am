lib_LTLIBRARIES = libmoldeo_presetpanel.la
libmoldeo_presetpanel_la_SOURCES = moMasterEffectPresetPanel.cpp moMasterEffectPresetPanel.h
libmoldeo_presetpanel_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_presetpanel_la_LDFLAGS =
libmoldeo_presetpanel_la_LIBADD = 
libmoldeo_presetpanel_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = mastereffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

