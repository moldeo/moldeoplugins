#
# autoheader
# libtoolize
# automake –add-missing –foreign
# autoconf
# ./configure
# make
# make install
#
AC_INIT([MoldeoPlugins], [0.9.1.0], [info@moldeointeractive.com.ar], [moldeoplugins])

AM_INIT_AUTOMAKE
AM_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4])

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
)
#	IODevices/Kinect
#	IODevices/Tablet 
#	IODevices/TabletAsync

AC_CONFIG_SUBDIRS(
	Resources/TrackerKLT
	Resources/OpenCV
)
#	Resources/Tracker
#	Resources/TrackerGpuKLT
#	Resources/TrackerGpuKLT2
#	
#AC_CONFIG_SUBDIRS(
#	MasterEffects/Channel
#	MasterEffects/Ligia
#	MasterEffects/MasterChannel
#	MasterEffects/Panel
#	MasterEffects/PresetPanel
#	)
AC_CONFIG_SUBDIRS(
	PostEffects/Debug
	)
#	PostEffects/CalibrationGrid
#	PostEffects/DeformationGrid
#	PostEffects/VideoWall
#	PostEffects/VideoWallMorphing
#	PostEffects/PostProcess2d
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
	Effects/Tunel
	Effects/Grid
)
#	Effects/Sound
#	Effects/CubeMap
#	Effects/Draw
#	Effects/Icon3D
#	Effects/ImageFlow
#	Effects/ImageFlow3D
#	Effects/SampleTest
#	Effects/SampleTest3d
#	Effects/Text
#	Effects/Text3D
#	Effects/Democube
#	Effects/Painter
#	Effects/Partiture
#	Effects/IScreens
#	Effects/LightPercussion
#	Effects/Lights
#	Effects/LiveDrawing2
#	
#	Effects/Protein
#	Effects/RegularSolid
#	Effects/Scene3D


AC_OUTPUT(Makefile)
