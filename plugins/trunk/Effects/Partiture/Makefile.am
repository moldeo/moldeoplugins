lib_LTLIBRARIES = libmod_partiture.la
libmod_partiture_la_SOURCES = moCHeader.cpp moCNote.cpp moCPentagram.cpp moCTrack.cpp moEffectPartiture.cpp
libmod_partiture_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_partiture_la_LDFLAGS =
libmod_partiture_la_LIBADD = 
libmod_partiture_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = Effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

