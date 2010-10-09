#
# autoheader
# libtoolize
# automake –add-missing –foreign
# autoconf
# ./configure
# make
# make install
#
AC_INIT([MoldeoPlugins], [0.7], [info@moldeointeractive.com.ar], [moldeoplugins])

AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_SUBDIRS(
	IODevices/Keyboard 
	IODevices/Midi 
	IODevices/Mixer 
	IODevices/Mouse 
       	IODevices/Serial 
	)
#	IODevices/NetOscIn 
#	IODevices/NetOscOut 
#	IODevices/NetTUIOIn 
#	IODevices/Tablet 
#	IODevices/TabletAsync
AC_CONFIG_SUBDIRS(
	)
#	Resources/OpenCV
#	Resources/Tracker
#	Resources/TrackerGpuKLT
#	Resources/TrackerGpuKLT2
#	Resources/TrackerKLT
AC_CONFIG_SUBDIRS(
	MasterEffects/Channel
	MasterEffects/Ligia
	MasterEffects/MasterChannel
	MasterEffects/Panel
	MasterEffects/PresetPanel
	)
AC_CONFIG_SUBDIRS(
	PostEffects/CalibrationGrid
	PostEffects/Debug
	PostEffects/DeformationGrid
	PostEffects/VideoWall
	PostEffects/VideoWallMorphing
	)
#	PostEffects/ColorFilter
#	PostEffects/PostProcess2d
#	PostEffects/VideoPanel
AC_CONFIG_SUBDIRS(
	Effects/CubeMap
	Effects/Democube
	Effects/Draw
	Effects/Icon
	Effects/Icon3D
	Effects/Image
	Effects/ImageFlow
	Effects/Painter
	Effects/Partiture
	Effects/Plane
	Effects/SampleTest
	Effects/SampleTest3d
	Effects/Sound
	Effects/Text
	Effects/Text3D
	Effects/Tunel
	)
#	Effects/Grid
#	Effects/IScreens
#	Effects/LightPercussion
#	Effects/Lights
#	Effects/LiveDrawing2
#	Effects/Movie
#	Effects/ParticlesSimple
#	Effects/Protein
#	Effects/RegularSolid
#	Effects/Scene3D
AC_CONFIG_SUBDIRS(
	PreEffects/Erase
	PreEffects/Mirror
	PreEffects/MirrorG
	)
#	PreEffects/MirrorMorphing

AC_OUTPUT(Makefile)
