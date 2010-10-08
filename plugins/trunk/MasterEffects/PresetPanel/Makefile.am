lib_LTLIBRARIES = libmod_presetpanel.la
libmod_presetpanel_la_SOURCES = moMasterEffectPresetPanel.cpp
libmod_presetpanel_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_presetpanel_la_LDFLAGS =
libmod_presetpanel_la_LIBADD = 
libmod_presetpanel_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = MasterEffects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

