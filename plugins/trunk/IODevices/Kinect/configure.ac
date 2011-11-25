#
# autoheader
# libtoolize
# automake –add-missing –foreign
# autoconf
# ./configure
# make
# make install
#
AC_INIT([Kinect], [0.7], [info@moldeointeractive.com.ar], [kinect])

AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE
AC_CONFIG_MACRO_DIR([m4])
AM_CONFIG_HEADER(config.h)

AC_LIBTOOL_CONFIG

AC_PROG_CC
AC_PROG_CC_STDC
AC_C_CONST
AC_PROG_LIBTOOL

AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_CXX_C_O

PKG_CHECK_MODULES([LIBMOLDEO], [libmoldeo] ,has_libmoldeo=yes)

PKG_CHECK_MODULES([OPENNI], [openni-dev] ,has_openni=yes)

PKG_CHECK_MODULES([EIGEN3], [eigen3] ,has_eigen3=yes)

PKG_CHECK_MODULES([PCLCOMMON], [pcl_common-1.2] ,has_pcl_common=yes)
PKG_CHECK_MODULES([PCLIO], [pcl_io-1.2] ,has_pcl_io=yes)
PKG_CHECK_MODULES([PCLFILTERS], [pcl_filters-1.2 ] ,has_pcl_filters=yes)
PKG_CHECK_MODULES([PCLFEATURES], [pcl_features-1.2 ] ,has_pcl_features=yes)



AC_OUTPUT(Makefile)
