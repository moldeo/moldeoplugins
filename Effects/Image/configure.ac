#
# autoheader
# libtoolize
# automake –add-missing –foreign
# autoconf
# ./configure
# make
# make install
#
AC_INIT([Image], [1.0.0.0], [info@moldeointeractive.com.ar], [image])

AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE
AC_CONFIG_MACRO_DIR([m4])
AM_CONFIG_HEADER(config.h)

AC_LIBTOOL_CONFIG

AC_PROG_CC
AC_PROG_CC_STDC
AC_C_CONST
AC_PROG_LIBTOOL
AM_PROG_LIBTOOL


AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_CXX_C_O

PKG_CHECK_MODULES([LIBMOLDEO], [libmoldeo] ,has_libmoldeo=yes)

AC_ARG_WITH( [gles],
AS_HELP_STRING([--with-gles],
[use open gl es 2, default: no]),
[with_gles=check])


AS_IF([test "x$with_gles" != xno],
[PKG_CHECK_MODULES([GL], [glesv2, egl] ,has_glesv2=yes)],
[PKG_CHECK_MODULES([GL], [gl, glew] ,has_gl=yes)])

AC_ARG_ENABLE(host,
[--enable-host[=host name] Specify the host os name:
			64: 64 bit linux os [default]
			86: 32 bits linux os
                        raspbian: RASPBIAN/debian 8 32 bits],
[ if test "x$enableval" = "x86" ; then
	host_name=86
  elif test "x$enableval" = "x64" ; then
	host_name=64
  elif test "x$enableval" = "xraspbian" ; then
	host_name="raspbian"
	host_cflags="-I/opt/vc/include -DOPENGLESV2"
	host_ldflags="-L/opt/vc/lib"
  else
	echo 
	echo "Error!"
	echo "Unknown host name"
	exit -1
  fi
])
AC_SUBST([HOSTCFLAGS],$host_cflags)
AC_SUBST([HOSTLDFLAGS],$host_ldflags)

dnl autoscan results
AC_CHECK_FUNCS([floor])
AC_CHECK_FUNCS([getcwd])
AC_CHECK_FUNCS([memmove])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([pow])
AC_CHECK_FUNCS([rint])
AC_CHECK_FUNCS([sqrt])
AC_CHECK_FUNCS([strchr])
AC_CHECK_FUNCS([strstr])
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([float.h])
AC_CHECK_HEADERS([limits.h])
AC_CHECK_HEADERS([stddef.h])
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_TYPES([ptrdiff_t])
AC_C_INLINE
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_VPRINTF
AC_HEADER_STDBOOL
AC_HEADER_STDC
AC_PREREQ
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
LT_INIT
AC_STRUCT_TM
AC_TYPE_SIZE_T

AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
               [enable debugging, default: no]),
[case "${enableval}" in
             yes) debug=true ;;
             no)  debug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],
[debug=false])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")

AC_OUTPUT(Makefile)
