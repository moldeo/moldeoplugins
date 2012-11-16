/*******************************************************************************

                                moKinect.h

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                    *
  *                                                                          *
  *   This code is distributed in the hope that it will be useful, but       *
  *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
  *   General Public License for more details.                               *
  *                                                                          *
  *   A copy of the GNU General Public License is available on the World     *
  *   Wide Web at <http://www.gnu.org/copyleft/gpl.html>. You can also       *
  *   obtain it by writing to the Free Software Foundation,                  *
  *   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.         *
  *                                                                          *
  ****************************************************************************

  Copyright(C) 2011 Fabricio Costa

  Authors:
  Fabricio Costa


*******************************************************************************/

#ifndef __MO_KINECT_H
#define __MO_KINECT_H


//#define KINECT_PCL
#define KINECT_OPENNI

/**

Eigen Library

*/


#include <iostream>


/**
*  KINECT OPENNI
*/
#ifdef KINECT_OPENNI
/*C wrapper*/
#include "XnOpenNI.h"
/*C** wrapper*/
#include <XnCodecIDs.h>
#include "XnCppWrapper.h"

#endif

/**
    PCL Library
*/

#ifdef KINECT_PCL

#include <Eigen/Dense>
using Eigen::MatrixXd;

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/openni_grabber.h>
#include <pcl/common/time.h>

#include <pcl/point_types.h>


#include <pcl/ModelCoefficients.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/features/normal_3d.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/kdtree/kdtree.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/segmentation/extract_clusters.h>

#endif

/**

MOLDEO

*/



#include "moTypes.h"
#include "moConfig.h"
#include "moDeviceCode.h"
#include "moEventList.h"
#include "moIODeviceManager.h"
#include "moTypes.h"

#ifdef MO_WIN32
  #include "SDL.h"
#else
  #include "SDL/SDL.h"
#endif


#ifdef KINECT_OPENNI

    #include "moKinectUserGenerator.h"

#endif


#ifdef KINECT_PCL
class SimpleOpenNIProcessor
{
public:
  void cloud_cb_ (const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr &cloud)
  {
    static unsigned count = 0;
    static double last = pcl::getTime ();
    if (++count == 30)
    {
      double now = pcl::getTime ();
      std::cout << "distance of center pixel :" << cloud->points [(cloud->width >> 1) * (cloud->height + 1)].z << " mm. Average framerate: " << double(count)/double(now - last) << " Hz" <<  std::endl;
      count = 0;
      last = now;
    }
  }

  void run ()
  {

    // create a new grabber for OpenNI devices

    pcl::Grabber* interface = new pcl::OpenNIGrabber();


    // make callback function from member function
    boost::function<void (const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr&)> f =
      boost::bind (&SimpleOpenNIProcessor::cloud_cb_, this, _1);

    // connect callback function for desired signal. In this case its a point cloud with color values
    boost::signals2::connection c = interface->registerCallback (f);

    // start receiving point clouds
    interface->start ();


    // wait until user quits program with Ctrl-C, but no busy-waiting -> sleep (1);
/*
    while (true)
      sleep(1);
*/

    // stop the grabber

    interface->stop ();

  }
};

#endif

enum moKinectParamIndex {

	KINECT_OFFSET_MIN=0,
	KINECT_OFFSET_MAX,

	KINECT_OFFSET_LEFT,
	KINECT_OFFSET_RIGHT,
	KINECT_OFFSET_TOP,
	KINECT_OFFSET_BOTTOM,

	KINECT_OFFSET_RADIUS,


	KINECT_REF_POINT_DEEP,
	KINECT_REF_POINT_WIDTH,
	KINECT_REF_POINT_HEIGHT,

	KINECT_REF_POINT_X,
	KINECT_REF_POINT_Y,
	KINECT_REF_POINT_Z,
	KINECT_REF_POINT_TILT,

	KINECT_OBJECT_RAD_MIN,
	KINECT_OBJECT_RAD_MAX,
	KINECT_CALIBRATE_BASE,
	KINECT_BASE_CAMERA_I,
	KINECT_BASE_CAMERA_J,

	KINECT_UMBRAL_CAMERA_PRESENCIA_Z_NEAR,
	KINECT_UMBRAL_CAMERA_PRESENCIA_Z_FAR,
	KINECT_UMBRAL_CAMERA_PRESENCIA_Y_OFFSET,

    KINECT_UMBRAL_TRACKEO_Y_OFFSET,
    KINECT_UMBRAL_TRACKEO_Z_NEAR,
    KINECT_UMBRAL_TRACKEO_Z_FAR,
    KINECT_TRACKEO_FACTOR_ANGULO,

    KINECT_UMBRAL_OBJETO_Z_NEAR,

    KINECT_VIEW_SCALEFACTOR,
	KINECT_VIEW_FOVY,

	KINECT_OBJECT_COLOR1_TOP,
	KINECT_OBJECT_COLOR1_BOTTOM,

	KINECT_OBJECT_COLOR2_TOP,
	KINECT_OBJECT_COLOR2_BOTTOM,

	KINECT_OBJECT_COLOR3_TOP,
	KINECT_OBJECT_COLOR3_BOTTOM,

	KINECT_OBJECT_MIN_SURFACE,
	KINECT_OBJECT_MAX_SURFACE,

	KINECT_OBJECT_MIN_VOLUME,
	KINECT_OBJECT_MAX_VOLUME,

	KINECT_OBJECT_TEXTURE1,
	KINECT_OBJECT_TEXTURE2,
	KINECT_OBJECT_TEXTURE3,

	KINECT_UPDATE_ON
};

//Buttons
#define KINECT_BUTTON_LEFT 0
#define KINECT_BUTTON_MIDDLE 1
#define KINECT_BUTTON_RIGHT 2
//Wheels
#define MOTION_REL_X 0
#define MOTION_REL_Y 1
//Position
#define POSITION_X 0
#define POSITION_Y 1

//channel0virtual
#define SDL_MOTION_REL_X_LEFT	0
#define SDL_MOTION_REL_Y_LEFT	1
#define SDL_MOTION_REL_X_MIDDLE	2
#define SDL_MOTION_REL_Y_MIDDLE	3
#define SDL_MOTION_REL_X_RIGHT	4
#define SDL_MOTION_REL_Y_RIGHT	5
#define SDL_MOTION_POS_X		6
#define SDL_MOTION_POS_Y		7

class  moKinectButton {
public:
	MOboolean pushed;
	SDLMod pushedmod;
	MOboolean released;
	SDLMod releasedmod;
	MOint pressed;
	SDLMod pressedmod;

	moKinectButton() {
		pushed = MO_FALSE;
		pushedmod = KMOD_NONE;
		released = MO_FALSE;
		releasedmod = KMOD_NONE;
		pressed = 0;
		pressedmod = KMOD_NONE;
	}

	MOboolean Init() {
		pushed = MO_FALSE;
		pushedmod = KMOD_NONE;
		released = MO_FALSE;
		releasedmod = KMOD_NONE;
		pressed = 0;
		pressedmod = KMOD_NONE;
		return true;
	}

	void Push(SDLMod mod) {
		pushed = MO_TRUE;
		pushedmod =  mod;
		released = MO_FALSE;
		releasedmod = KMOD_NONE;
		pressedmod = KMOD_NONE;
	}

	MOint Release(SDLMod mod) {
		released = MO_TRUE;
		releasedmod = mod;
		//se soltó pero nunca se pulsó(es un error) o no coincide el mapa de keys especiales
		if((pushed == MO_FALSE) ||(releasedmod!=pushedmod)) {
			Init();
			return MO_FALSE;
		} else {
			pressed = pressed + 1;
			pressedmod = mod;
			pushed = MO_FALSE;
			pushedmod = KMOD_NONE;
			released = MO_FALSE;
			releasedmod = KMOD_NONE;
			return MO_TRUE;
		}
	}
};

class  moKinectWheel {
public:
	MOboolean motion;//verdadero si se movio
	MOint rel;
	MOint kinectmod;

	moKinectWheel() {
		motion = MO_FALSE;
		rel = 0;
		kinectmod = 0;
	}

	MOboolean Init() {
		motion = MO_FALSE;
		rel = 0;
		kinectmod = 0;
		return true;
	}

	void Motion(MOint r) {
		if(r!=0) {
			rel = r;
			motion = MO_TRUE;
		}
	}
};

class  moKinectPosition {
public:
	MOboolean motion;//verdadero si se movio
	MOint pos;
	MOint kinectmod;

	moKinectPosition() {
		motion = MO_FALSE;
		pos = 0;
	}

	MOboolean Init() {
		motion = MO_FALSE;
		pos = 0;
		return true;
	}

	void Motion(MOint r) {
		if(r!=pos) {
			pos = r;
			motion = MO_TRUE;
		}
	}
};


class  moKinectCode {
public:
	moText strcod;
	MOint code;
	MOint kinectcod;//mapeo type SDL
	MOint type;//pushedr:0 ,prendedor:1, selector:2, seteador:3,...otros
	MOint especiales;//keys especiales, como alt, caps lock, etc...
	MOint state;//0 MO_FALSE(no esta activo el device) MO_ON(está on) MO_OFF(está off)
	MOint value;//depende
};


class  moKinect : public moIODevice {

public:

    moKinect();
    virtual ~moKinect();

	MOboolean		Init();
	MOboolean		Finish();

    void			Update(moEventList*);
    MOswitch		GetStatus( MOdevcode);
    MOswitch		SetStatus( MOdevcode,MOswitch);
	MOint			GetValue( MOdevcode);
    MOdevcode		GetCode( moText);

    virtual moConfigDefinition *GetDefinition( moConfigDefinition *p_configdefinition );

    void UpdateParameters();

    moVector3f  m_ReferencePoint;
    moVector3f  m_ReferencePointDimension;

    moVector2f  m_Offset;
    moVector4f  m_OffsetBox;
    float  m_OffsetRadius;

    float       m_RadMin,m_RadMax;

    moVector3f  m_CenterPoint;

    moVector3f*  pCloud;
    moVector3f*  pCloudRGB;
    moVector3f*  pCloudDif;

    int calibrate_base;
    int base_camera_i;
    int base_camera_j;

    int umbral_camera_presencia_z_near;
    int umbral_camera_presencia_z_far;
    int umbral_camera_presencia_y_offset;

    int umbral_trackeo_y_offset;
    int umbral_trackeo_z_near;
    int umbral_trackeo_z_far;
    float trackeo_factor_angulo;

    int umbral_objeto_z_near;

    float scaleFactor;
    float fovy;

    int m_haypresencia;

    #ifdef KINECT_PCL
        pcl::Normal m_BaseNormal;
    #else
        moVector3f BaseNormal;
    #endif
    moVector3f m_BasePosition;
    moVector3f m_BaseRGB;
    moVector3f m_BaseHSV;

    #ifdef KINECT_PCL
        pcl::Normal m_TargetNormal;
    #endif
    moVector3f m_TargetPosition;
    moVector3f m_TargetRGB;
    moVector3f m_TargetHSV;

    moVector4d m_Color1Bottom;
    moVector4d m_Color1Top;

    moVector4d m_Color2Bottom;
    moVector4d m_Color2Top;

    moVector4d m_Color3Bottom;
    moVector4d m_Color3Top;

    float m_MinSurface;
    float m_MaxSurface;

    float m_MinVolume;
    float m_MaxVolume;


    int m_fbo_idx;
    MOuint attach_point;
    moFBO* pFBO;

#ifdef KINECT_PCL
    /*ATENCION: esta función es la que procesa la PCL */
    void cloud_cb_ (const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr &cloud);
#endif

    bool show_callback;

    moLock  m_DataLock;

    int update_on;

    int NDIF1;
    int NDIF2;
    int NDIF3;
    int SURFACE1;
    int SURFACE2;
    int SURFACE3;

    int SURFACE;
    int VOLUME;

private:

    moConfig config;


    /**  PCL */
    #ifdef KINECT_PCL
        pcl::Grabber* interface;

        SimpleOpenNIProcessor v;
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_xyz;
        pcl::PointCloud<pcl::PointXYZRGB>::Ptr PclMsg;
        pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> ne;

        float m_center_curvature;
        pcl::Normal m_CenterNormal;
        pcl::Normal m_ReferenceNormal;
    #endif

	/** OPENNI KINECT*/
	moKinectButton Buttons[3];
	moKinectWheel Wheels[2];
	moKinectPosition Position[2];

	moKinectCode *Codes;
	MOuint ncodes;

	//BUFFER DE POSICIONES ANTERIORES
	//MOint nchars;
	//MOint Text[256];
	//MOswitch TextActive;

	//CONSOLA VIRTUAL
	moConfig configcv;
	MOuint channel0virtual[6];//simulamos rueditas para el channel virtual
	MOuint masterchannelvirtual[6];//simulamos rueditas para el channel virtual

#ifdef KINECT_OPENNI
    xn::Context m_Context;
    xn::DepthGenerator m_Depth;
    xn::ImageGenerator m_RGBImage;
    xn::IRGenerator m_IRImage;

    XnDepthPixel m_DepthPixel;
    XnStatus m_nRetVal;

#endif
    XnMapOutputMode m_OutputMode;

    moTexture* m_pDepthTexture;
    moTexture* m_pRGBTexture;
    moTexture* m_pIRTexture;

    moTexture* m_pDepthTexture2;

    unsigned char* pData;
    unsigned char* pData2;
    unsigned char* pImage;
    unsigned char* pIR;

    unsigned char* pDataObj1;
    unsigned char* pDataObj2;
    unsigned char* pDataObj3;

    int id_tex1;
    int id_tex2;
    int id_tex3;

    float dif_tex1;
    float dif_tex2;
    float dif_tex3;

    moTexture* m_pTex1;
    moTexture* m_pTex2;
    moTexture* m_pTex3;

    bool CheckError();

};


class moKinectFactory : public moIODeviceFactory {

public:
    moKinectFactory() {}
    virtual ~moKinectFactory() {}
    moIODevice* Create();
    void Destroy(moIODevice* fx);
};

extern "C"
{
MO_PLG_API moIODeviceFactory* CreateIODeviceFactory();
MO_PLG_API void DestroyIODeviceFactory();
}

#endif
