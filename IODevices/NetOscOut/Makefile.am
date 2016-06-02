lib_LTLIBRARIES = libmoldeo_netoscout.la
libmoldeo_netoscout_la_SOURCES = ip/IpEndpointName.cpp\
ip/IpEndpointName.h\
ip/NetworkingUtils.h\
ip/PacketListener.h\
ip/posix/NetworkingUtils.cpp\
ip/posix/UdpSocket.cpp\
ip/TimerListener.h\
ip/UdpSocket.h\
MessageMappingOscPacketListener.h\
moNetOSCOut.cpp\
moNetOSCOut.h\
OscException.h\
OscHostEndianness.h\
OscOutboundPacketStream.cpp\
OscOutboundPacketStream.h\
OscPacketListener.h\
OscPrintReceivedElements.cpp\
OscPrintReceivedElements.h\
OscReceivedElements.cpp\
OscReceivedElements.h\
OscTypes.cpp\
OscTypes.h
libmoldeo_netoscout_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS} -I ./ip -DOSC_HOST_LITTLE_ENDIAN=1
libmoldeo_netoscout_la_LDFLAGS =
libmoldeo_netoscout_la_LIBADD =
libmoldeo_netoscout_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

