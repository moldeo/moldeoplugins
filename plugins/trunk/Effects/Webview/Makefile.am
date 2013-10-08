lib_LTLIBRARIES = libmoldeo_webview.la
libmoldeo_webview_la_SOURCES =  moEffectWebview.cpp \
                                moEffectWebview.h\
                                awesomium/common/gl_texture_surface.h\
                                awesomium/common/gl_texture_surface.cc
libmoldeo_webview_la_CPPFLAGS = ${LIBMOLDEO_CFLAGS} -I./awesomium/common -I./awesomium/include
libmoldeo_webview_la_LDFLAGS = -lawesomium-1-7
libmoldeo_webview_la_LIBADD =
libmoldeo_webview_la_LIBS = ${LIBMOLDEO_LIBS}

moduletype = effects
datadir = ${prefix}/share/moldeo
modulesdir = ${exec_prefix}/lib/moldeo
libdir = ${modulesdir}/${moduletype}
docdir = ${prefix}/share/doc/moldeo

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" -DMODULESDIR=\"$(modulesdir)\"
ACLOCAL_AMFLAGS=-I m4

