ACLOCAL_AMFLAGS=-I m4

if DEBUG
DFLAGS = -g3 -O0 -fpermissive
else
DFLAGS = -O2 -fpermissive
endif

moduletype = iodevices
datadir = ${prefix}/share/moldeo/data
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = ${DFLAGS} ${HOSTCFLAGS} $(all_includes) -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
AM_CXXFLAGS = ${AM_CPPFLAGS}
AM_LDFLAGS = -ldl ${HOSTLDFLAGS}

lib_LTLIBRARIES = libmoldeo_gpio.la
libmoldeo_gpio_la_SOURCES = moGPIO.cpp\
moGPIO.h
libmoldeo_gpio_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS} -I ./wiringPi -DOSC_HOST_LITTLE_ENDIAN=1
libmoldeo_gpio_la_LDFLAGS =
libmoldeo_gpio_la_LIBADD = 
libmoldeo_gpio_la_LIBS = ${LIBMOLDEO_LIBS}

libmoldeo_image_includedir = $(includedir)/moldeo/${moduletype}/image
libmoldeo_image_include_HEADERS = moEffectIcon.h

METASOURCES = AUTO