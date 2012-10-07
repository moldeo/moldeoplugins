#
# autoheader
# libtoolize
# automake –add-missing –foreign
# autoconf
# ./configure
# make
# make install
#
AC_INIT([MoldeoPlugins], [0.7.9.7], [info@moldeointeractive.com.ar], [moldeoplugins])

AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_SUBDIRS(
	IODevices/Keyboard 
	IODevices/Mixer 
	IODevices/Mouse 
	IODevices/NetOscIn 
	IODevices/NetOscOut
	IODevices/Serial
	)
#	IODevices/Kinect	
#	 
#	IODevices/Midi 
#	
#	IODevices/NetTUIOIn
#	IODevices/Tablet 
#	IODevices/TabletAsync
AC_CONFIG_SUBDIRS(
	Resources/TrackerKLT
)
#	Resources/OpenCV
#	Resources/Tracker
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
	PostEffects/CalibrationGrid
	PostEffects/Debug
	PostEffects/DeformationGrid
	PostEffects/VideoWall
	PostEffects/VideoWallMorphing
	PostEffects/PostProcess2d
	)
#	PostEffects/ColorFilter
#	
#	PostEffects/VideoPanel
AC_CONFIG_SUBDIRS(
	Effects/CubeMap
	Effects/Draw
	Effects/Icon
	Effects/Icon3D
	Effects/Image
	Effects/ImageFlow
	Effects/Movie
	Effects/Plane
	Effects/SampleTest
	Effects/SampleTest3d
	Effects/Sound
	Effects/Text
	Effects/Text3D
	Effects/Tunel
	Effects/ParticlesSimple
	)

#	Effects/Grid
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
AC_CONFIG_SUBDIRS(
	PreEffects/Erase
	PreEffects/Mirror
	PreEffects/MirrorG
	)
#	PreEffects/MirrorMorphing

AC_OUTPUT(Makefile)
