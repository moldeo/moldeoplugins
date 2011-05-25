lib_LTLIBRARIES = libmoldeo_partiture.la
libmoldeo_partiture_la_SOURCES = moCHeader.cpp moCNote.cpp moCPentagram.cpp moCTrack.cpp moEffectPartiture.cpp \
moCHeader.h moCNote.h moCPentagram.h moCTrack.h moEffectPartiture.h
libmoldeo_partiture_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmoldeo_partiture_la_LDFLAGS =
libmoldeo_partiture_la_LIBADD = 
libmoldeo_partiture_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

