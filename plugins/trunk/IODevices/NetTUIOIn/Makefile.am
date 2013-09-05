
lib_LTLIBRARIES = libmoldeo_nettuioin.la

libmoldeo_nettuioin_la_SOURCES = ip/IpEndpointName.h\
ip/NetworkingUtils.h\
ip/PacketListener.h\
ip/TimerListener.h\
ip/UdpSocket.h\
osc/MessageMappingOscPacketListener.h\
moNetTUIOIn.cpp\
moNetTUIOIn.h\
osc/OscException.h\
osc/OscHostEndianness.h\
osc/OscOutboundPacketStream.cpp\
osc/OscOutboundPacketStream.h\
osc/OscPacketListener.h\
osc/OscPrintReceivedElements.cpp\
osc/OscPrintReceivedElements.h\
osc/OscReceivedElements.cpp\
osc/OscReceivedElements.h\
osc/OscTypes.cpp\
osc/OscTypes.h\
ip/posix/NetworkingUtils.cpp\
ip/posix/UdpSocket.cpp\
tuio/TuioClient.cpp\
tuio/TuioServer.cpp\
tuio/TuioTime.cpp

libmoldeo_nettuioin_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS} -I ./ip -I ./osc -I ./tuio -Ituio -DOSC_HOST_LITTLE_ENDIAN=1

libmoldeo_nettuioin_la_LDFLAGS =
libmoldeo_nettuioin_la_LIBADD = 
libmoldeo_nettuioin_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = iodevices
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

