lib_LTLIBRARIES = libmod_netoscout.la
libmod_netoscout_la_SOURCES = moNetOSCOut.cpp OscOutboundPacketStream.cpp OscPrintReceivedElements.cpp OscReceivedElements.cpp OscTypes.cpp
libmod_netoscout_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_netoscout_la_LDFLAGS =
libmod_netoscout_la_LIBADD = 
libmod_netoscout_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

