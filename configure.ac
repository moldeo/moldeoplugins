#
# autoheader
# libtoolize
# automake –add-missing –foreign
# autoconf
# ./configure
# make
# make install
#
AC_INIT([MoldeoPlugins], [1.1.0.1], [info@moldeointeractive.com.ar ], [moldeoplugins])

AC_CANONICAL_HOST
AC_CANONICAL_BUILD
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AM_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4])

dnl Set install paths.
if test "x$prefix" = "xNONE" ; then
  prefix="/usr/local"
fi
if test "x$exec_prefix" = "xNONE" ; then
  exec_prefix=$prefix
fi

case "$host" in
        *-*-mingw*|*-*-cygwin*)
                AC_DEFINE(PLATFORM_WIN32, 1, [Platform is Win32])
                PLATFORM_STRING="Win32"
                ;;
        *-*-linux*)
                AC_DEFINE(PLATFORM_LINUX, 1, [Platform is Linux])
                PLATFORM_STRING="Linux"
                ;;
        *-*-darwin*)
		AC_MSG_WARN([Hello])
                AC_DEFINE(PLATFORM_APPLE, 1, [Platform is Apple])
                PLATFORM_STRING="Apple"
                ;;
        *)
                AC_MSG_WARN([*** Please add $host to configure.ac checks!])
                ;;
esac


case "$host" in
        i?86-* | k?-* | athlon-* | pentium*-)
                AC_DEFINE(ARCH_X86, 1, [Architecture is x86])
                ARCH_SPECIFIC_CFLAGS=""
                ARCH_STRING="X86"
                ;;
        x86_64-*)
                AC_DEFINE(ARCH_X86_64, 1, [Architecture is x86-64])
                ARCH_SPECIFIC_CFLAGS="-DUSE_ADDR64"
                ARCH_STRING="X86-64"
                with_boost_libdir="/usr/lib/x86_64-linux-gnu"
                libdir=$prefix"/lib"
                ;;
	armv7l-*)
                AC_DEFINE(ARCH_X86_64, 1, [Architecture is armv7l x86-64])
                ARCH_SPECIFIC_CFLAGS="-DUSE_ADDR64"
                ARCH_STRING="X86-64"
                libdir=$prefix"/lib"
                ;;
        ppc-* | powerpc-*)
		AC_MSG_WARN([HI THERE!])
                AC_DEFINE(ARCH_PPC, 1, [Architecture is PowerPC])
                ARCH_SPECIFIC_CFLAGS=""
                ARCH_STRING="PowerPC"
                ;;
        *)
                AC_MSG_ERROR([Unknown Architecture])
                ;;
esac

AC_CONFIG_SUBDIRS(
	PreEffects/Erase
	PreEffects/Mirror
	PreEffects/MirrorG
)
#	PreEffects/MirrorMorphing

AC_CONFIG_SUBDIRS(
	IODevices/Keyboard
	IODevices/Mixer
	IODevices/Mouse
	IODevices/NetOscIn
	IODevices/NetOscOut
	IODevices/Serial
	IODevices/NetTUIOIn
	IODevices/Midi
  IODevices/Tablet
)
#	IODevices/Kinect
#	IODevices/TabletAsync

AC_CONFIG_SUBDIRS(
	Resources/TrackerKLT
)
# Resources/Tracker
# Resources/OpenCV
#	Resources/TrackerGpuKLT
#	Resources/TrackerGpuKLT2
#
AC_CONFIG_SUBDIRS(
	MasterEffects/Channel
	MasterEffects/Ligia
	MasterEffects/MasterChannel
	MasterEffects/Panel
	MasterEffects/PresetPanel
	)
AC_CONFIG_SUBDIRS(
	PostEffects/Debug
	PostEffects/CalibrationGrid
	PostEffects/DeformationGrid
	PostEffects/VideoWall
	PostEffects/VideoWallMorphing
	PostEffects/PostProcess2d
	)
#	PostEffects/ColorFilter
#
#	PostEffects/VideoPanel

AC_CONFIG_SUBDIRS(
	Effects/Icon
	Effects/Image
	Effects/Camera
	Effects/Movie
	Effects/Plane
	Effects/ParticlesSimple
	Effects/Sound
	Effects/Sound3D
	Effects/Tunel
	Effects/Grid
	Effects/CubeMap
	Effects/Icon3D
	Effects/ImageFlow
	Effects/ImageFlow3D
	Effects/Text
	Effects/Text3D
	Effects/LiveDrawing2
)
#	Effects/Lights
#	Effects/LightPercussion
#	Effects/Sound
#	Effects/Draw
#	Effects/SampleTest
#	Effects/SampleTest3d
#	Effects/Democube
#	Effects/Painter
#	Effects/Partiture
#	Effects/IScreens
#	Effects/Protein
#	Effects/RegularSolid
#	Effects/Scene3D


AC_OUTPUT(Makefile)
