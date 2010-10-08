lib_LTLIBRARIES = libmod_netoscin.la
libmod_netoscin_la_SOURCES = moNetOSCIn.cpp OscOutboundPacketStream.cpp OscPrintReceivedElements.cpp OscReceivedElements.cpp OscTypes.cpp
libmod_netoscin_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS}
libmod_netoscin_la_LDFLAGS =
libmod_netoscin_la_LIBADD = 
libmod_netoscin_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = IODevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

