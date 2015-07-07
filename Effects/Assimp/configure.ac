#
# autoheader
# libtoolize
# automake –add-missing –foreign
# autoconf
# ./configure
# make
# make install
#
AC_INIT([Assimp], [0.9], [info@moldeointeractive.com.ar], [assimp])

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

PKG_CHECK_MODULES([LIBASSIMP], [assimp] ,has_assimp=yes)

AC_OUTPUT(Makefile)
