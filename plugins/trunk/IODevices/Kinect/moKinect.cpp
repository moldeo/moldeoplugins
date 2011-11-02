/*******************************************************************************

                              moKinect.cpp

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

  Copyright(C) 2006 Fabricio Costa

  Authors:
  Fabricio Costa


*******************************************************************************/

#include "moKinect.h"

#define MO_KINECT_CFG_STRCOD 0
#define MO_KINECT_CFG_TYPE 1
#define MO_KINECT_CFG_KINECTCOD 2

//========================
//  Factory
//========================

moKinectFactory *m_MouseFactory = NULL;

MO_PLG_API moIODeviceFactory* CreateIODeviceFactory(){
	if (m_MouseFactory==NULL)
		m_MouseFactory = new moKinectFactory();
	return (moIODeviceFactory*) m_MouseFactory;
}

MO_PLG_API void DestroyIODeviceFactory(){
	delete m_MouseFactory;
	m_MouseFactory = NULL;
}

moIODevice*  moKinectFactory::Create() {
	return new moKinect();
}

void moKinectFactory::Destroy(moIODevice* fx) {
	delete fx;
}


//========================
//  Mouse
//========================
moVector3d RGBtoHSV( double r, double g, double b ) {

    // RGB are from 0..1, H is from 0..360, SV from 0..1
    double maxC = b;
    if (maxC < g) maxC = g;
    if (maxC < r) maxC = r;
    double minC = b;
    if (minC > g) minC = g;
    if (minC > r) minC = r;

    double delta = maxC - minC;

    double V = maxC;
    double S = 0;
    double H = 0;

    if (delta == 0)
    {
        H = 0;
        S = 0;
    }
    else
    {
        S = delta / maxC;
        double dR = 60*(maxC - r)/delta + 180;
        double dG = 60*(maxC - g)/delta + 180;
        double dB = 60*(maxC - b)/delta + 180;
        if (r == maxC)
            H = dB - dG;
        else if (g == maxC)
            H = 120 + dR - dB;
        else
            H = 240 + dG - dR;
    }

    if (H<0)
        H+=360;
    if (H>=360)
        H-=360;

    return moVector3d(H,S,V);
}

moKinect::moKinect() {
	Codes = NULL;
	SetName("kinect");

	pData = NULL;
	pData2 = NULL;
	pImage = NULL;
	m_pDepthTexture = NULL;
	m_pDepthTexture2 = NULL;
	m_pRGBTexture = NULL;
	pCloud = NULL;
	pCloudDif = NULL;

	show_callback = false;
	m_center_curvature = 0.0;
	m_CenterNormal.normal[0] = 0.0;
	m_CenterNormal.normal[1] = 0.0;
	m_CenterNormal.normal[2] = 0.0;
	m_CenterNormal.normal[3] = 0.0;

	m_ReferenceNormal = m_CenterNormal;
}

moKinect::~moKinect() {
	Finish();
}

bool moKinect::CheckError() {
    if (m_nRetVal != XN_STATUS_OK)
    {
        MODebug2->Error( moText("OpenNI Status Error:") + xnGetStatusString(m_nRetVal) );
        return false;
    }
    return true;

}

moConfigDefinition *
moKinect::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moIODevice::GetDefinition( p_configdefinition );
	//p_configdefinition->Add( moText("mididevice"), MO_PARAM_TEXT, MIDI_DEVICE, moValue( "", "TXT") );

	p_configdefinition->Add( moText("reference_point_x"), MO_PARAM_FUNCTION, KINECT_REF_POINT_X, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("reference_point_y"), MO_PARAM_FUNCTION, KINECT_REF_POINT_Y, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("reference_point_z"), MO_PARAM_FUNCTION, KINECT_REF_POINT_Z, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("reference_point_tilt"), MO_PARAM_FUNCTION, KINECT_REF_POINT_TILT, moValue( "0.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("offset_min"), MO_PARAM_FUNCTION, KINECT_OFFSET_MIN, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("offset_max"), MO_PARAM_FUNCTION, KINECT_OFFSET_MAX, moValue( "0.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("reference_point_deep"), MO_PARAM_FUNCTION, KINECT_REF_POINT_DEEP, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("reference_point_width"), MO_PARAM_FUNCTION, KINECT_REF_POINT_WIDTH, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("reference_point_height"), MO_PARAM_FUNCTION, KINECT_REF_POINT_HEIGHT, moValue( "0.0", "FUNCTION").Ref() );

    p_configdefinition->Add( moText("object_rad_min"), MO_PARAM_FUNCTION, KINECT_OBJECT_RAD_MIN, moValue( "5.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("object_rad_max"), MO_PARAM_FUNCTION, KINECT_OBJECT_RAD_MAX, moValue( "15.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("calibrate_base"), MO_PARAM_NUMERIC, KINECT_CALIBRATE_BASE, moValue( "0", "INT") );
    p_configdefinition->Add( moText("base_camera_i"), MO_PARAM_NUMERIC, KINECT_BASE_CAMERA_I, moValue( "320", "INT") );
	p_configdefinition->Add( moText("base_camera_j"), MO_PARAM_NUMERIC, KINECT_BASE_CAMERA_J, moValue( "240", "INT") );
    p_configdefinition->Add( moText("umbral_camera_presencia_z_near"), MO_PARAM_NUMERIC, KINECT_UMBRAL_CAMERA_PRESENCIA_Z_NEAR, moValue( "800", "INT") );
	p_configdefinition->Add( moText("umbral_camera_presencia_z_far"), MO_PARAM_NUMERIC, KINECT_UMBRAL_CAMERA_PRESENCIA_Z_FAR, moValue( "2000", "INT") );
    p_configdefinition->Add( moText("umbral_camera_presencia_y_offset"), MO_PARAM_NUMERIC, KINECT_UMBRAL_CAMERA_PRESENCIA_Y_OFFSET, moValue( "10", "INT") );

    p_configdefinition->Add( moText("umbral_trackeo_y_offset"), MO_PARAM_NUMERIC, KINECT_UMBRAL_TRACKEO_Y_OFFSET, moValue( "5", "INT") );
	p_configdefinition->Add( moText("umbral_trackeo_z_near"), MO_PARAM_NUMERIC, KINECT_UMBRAL_TRACKEO_Z_NEAR, moValue( "900", "INT") );
    p_configdefinition->Add( moText("umbral_trackeo_z_far"), MO_PARAM_NUMERIC, KINECT_UMBRAL_TRACKEO_Z_FAR, moValue( "2500", "INT") );
    p_configdefinition->Add( moText("trackeo_factor_angulo"), MO_PARAM_FUNCTION, KINECT_TRACKEO_FACTOR_ANGULO, moValue( "0.0", "FUNCTION").Ref() );


    p_configdefinition->Add( moText("view_scalefactor"), MO_PARAM_FUNCTION, KINECT_VIEW_SCALEFACTOR, moValue( "0.7", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("view_fovy"), MO_PARAM_FUNCTION, KINECT_VIEW_FOVY, moValue( "45", "FUNCTION").Ref() );



	return p_configdefinition;
}


void moKinect::UpdateParameters() {

    m_ReferencePoint = moVector3f(  m_Config.Eval( moR(KINECT_REF_POINT_X) ),
                                    m_Config.Eval( moR(KINECT_REF_POINT_Y) ),
                                    m_Config.Eval( moR(KINECT_REF_POINT_Z) )
                                  );

    m_ReferencePointDimension = moVector3f(  m_Config.Eval( moR(KINECT_REF_POINT_WIDTH) ),
                                    m_Config.Eval( moR(KINECT_REF_POINT_HEIGHT) ),
                                    m_Config.Eval( moR(KINECT_REF_POINT_DEEP) )
                                  );

    m_Offset = moVector2f(
                            m_Config.Eval( moR(KINECT_OFFSET_MIN) ),
                            m_Config.Eval( moR(KINECT_OFFSET_MAX) )
                          );
    m_RadMin = m_Config.Eval( moR(KINECT_OBJECT_RAD_MIN));
    m_RadMax = m_Config.Eval( moR(KINECT_OBJECT_RAD_MAX));

    calibrate_base = m_Config.Int( moR(KINECT_CALIBRATE_BASE));

    m_DataLock.Lock();

        base_camera_i = m_Config.Int( moR(KINECT_BASE_CAMERA_I));
        base_camera_j = m_Config.Int( moR(KINECT_BASE_CAMERA_J));


        umbral_camera_presencia_y_offset = m_Config.Int( moR(KINECT_UMBRAL_CAMERA_PRESENCIA_Y_OFFSET));
        umbral_camera_presencia_z_near = m_Config.Int( moR(KINECT_UMBRAL_CAMERA_PRESENCIA_Z_NEAR));
        umbral_camera_presencia_z_far = m_Config.Int( moR(KINECT_UMBRAL_CAMERA_PRESENCIA_Z_FAR));


        umbral_trackeo_y_offset = m_Config.Int( moR(KINECT_UMBRAL_TRACKEO_Y_OFFSET));
        umbral_trackeo_z_near = m_Config.Int( moR(KINECT_UMBRAL_TRACKEO_Z_NEAR));
        umbral_trackeo_z_far = m_Config.Int( moR(KINECT_UMBRAL_TRACKEO_Z_FAR));
        trackeo_factor_angulo = m_Config.Eval( moR(KINECT_TRACKEO_FACTOR_ANGULO));

    m_DataLock.Unlock();

    scaleFactor = m_Config.Eval( moR(KINECT_VIEW_SCALEFACTOR));
    fovy = m_Config.Eval( moR(KINECT_VIEW_FOVY));
}

MOboolean
moKinect::Init() {

	MOuint i,j,coparam;
	MOuint nvirtualcodes,cv,ncv;
	moText cvstr;
	moText conf;

    moText text;


	Codes = NULL;

	//levantamos el config del kinect
    if (moMoldeoObject::Init()) {
        moMoldeoObject::CreateConnectors();
    } else return false;


	moDefineParamIndex( KINECT_OFFSET_MIN, moText("offset_min") );
	moDefineParamIndex( KINECT_OFFSET_MAX, moText("offset_max") );

    moDefineParamIndex( KINECT_REF_POINT_DEEP, moText("reference_point_deep") );
	moDefineParamIndex( KINECT_REF_POINT_WIDTH, moText("reference_point_width") );
	moDefineParamIndex( KINECT_REF_POINT_HEIGHT, moText("reference_point_height") );

    moDefineParamIndex( KINECT_REF_POINT_X, moText("reference_point_x") );
	moDefineParamIndex( KINECT_REF_POINT_Y, moText("reference_point_y") );
	moDefineParamIndex( KINECT_REF_POINT_Z, moText("reference_point_z") );

    moDefineParamIndex( KINECT_OBJECT_RAD_MIN, moText("object_rad_min") );
	moDefineParamIndex( KINECT_OBJECT_RAD_MAX, moText("object_rad_max") );

	moDefineParamIndex( KINECT_CALIBRATE_BASE, moText("calibrate_base") );
	moDefineParamIndex( KINECT_BASE_CAMERA_I, moText("base_camera_i") );
	moDefineParamIndex( KINECT_BASE_CAMERA_J, moText("base_camera_j") );

    moDefineParamIndex( KINECT_UMBRAL_CAMERA_PRESENCIA_Z_NEAR, moText("umbral_camera_presencia_z_near") );
    moDefineParamIndex( KINECT_UMBRAL_CAMERA_PRESENCIA_Z_FAR, moText("umbral_camera_presencia_z_far") );
    moDefineParamIndex( KINECT_UMBRAL_CAMERA_PRESENCIA_Y_OFFSET, moText("umbral_camera_presencia_y_offset") );


    moDefineParamIndex( KINECT_UMBRAL_TRACKEO_Y_OFFSET, moText("umbral_trackeo_y_offset") );
    moDefineParamIndex( KINECT_UMBRAL_TRACKEO_Z_NEAR, moText("umbral_trackeo_z_near") );
    moDefineParamIndex( KINECT_UMBRAL_TRACKEO_Z_FAR, moText("umbral_trackeo_z_far") );
    moDefineParamIndex( KINECT_TRACKEO_FACTOR_ANGULO, moText("trackeo_factor_angulo") );

	moDefineParamIndex( KINECT_VIEW_SCALEFACTOR, moText("view_scalefactor") );
	moDefineParamIndex( KINECT_VIEW_FOVY, moText("view_fovy") );



    UpdateParameters();

/**
        falta algo?
*/
    moTexParam tparam = MODefTex2DParams;
    //tparam.internal_format = GL_RGBA32F_ARB;
    tparam.internal_format = GL_RGB;

    m_OutputMode.nXRes = 640;
    m_OutputMode.nYRes = 480;
    m_OutputMode.nFPS = 30;

    int Mid = -1;

    /** RGB texture*/
    Mid = GetResourceManager()->GetTextureMan()->AddTexture( "KINECTRGB", m_OutputMode.nXRes, m_OutputMode.nYRes, tparam );
    if (Mid>0) {
        m_pRGBTexture = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
    } else {
        MODebug2->Error("Couldn't create texture: KINECTRGB");
    }

    /** DEPTH texture*/
    Mid = GetResourceManager()->GetTextureMan()->AddTexture( "KINECTDEPTH", m_OutputMode.nXRes, m_OutputMode.nYRes, tparam );
    if (Mid>0) {
        m_pDepthTexture = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
        m_pDepthTexture->BuildEmpty(m_OutputMode.nXRes, m_OutputMode.nYRes);
    } else {
        MODebug2->Error("Couldn't create texture: KINECTDEPTH");
    }

    Mid = GetResourceManager()->GetTextureMan()->AddTexture( "KINECTDEPTH2", m_OutputMode.nXRes, m_OutputMode.nYRes, tparam );
    if (Mid>0) {
        m_pDepthTexture2 = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
        m_pDepthTexture2->BuildEmpty(m_OutputMode.nXRes, m_OutputMode.nYRes);
    } else {
        MODebug2->Error("Couldn't create texture: KINECTDEPTH2");
    }



        m_fbo_idx = m_pResourceManager->GetFBMan()->CreateFBO();
        pFBO = m_pResourceManager->GetFBMan()->GetFBO(m_fbo_idx);

        pFBO->AddTexture(   m_OutputMode.nXRes,
                            m_OutputMode.nYRes,
                            m_pDepthTexture->GetTexParam(),
                            m_pDepthTexture->GetGLId(),
                            attach_point );

        m_pDepthTexture->SetFBO( pFBO );
        m_pDepthTexture->SetFBOAttachPoint( attach_point );





#ifdef KINECT_OPENNI
    m_nRetVal = m_Context.Init();
    if (!CheckError()) {
        MODebug2->Error("Couldn't initialize Kinect Context");
        return false;
    } else {
        MODebug2->Message("Kinect Context Initialized!!");
    }
#endif


#ifdef KINECT_OPENNI
    /** Create a DepthGenerator node */

    m_nRetVal = m_Depth.Create(m_Context);
    if (!CheckError()) {

        MODebug2->Error("Couldn't create DepthGenerator");
        return false;

    } else {

        MODebug2->Message("Kinect DepthGenerator Created!!");

        m_DepthPixel = m_Depth.GetDeviceMaxDepth();

        MODebug2->Message( moText("m_DepthPixel:") + IntToStr(m_DepthPixel) );

        m_OutputMode.nXRes = 640;
        m_OutputMode.nYRes = 480;
        m_OutputMode.nFPS = 30;

        m_nRetVal = m_Depth.SetMapOutputMode(m_OutputMode);
        if (!CheckError()) {
            MODebug2->Error("Couldn't set Map Outputmode");
            return false;
        }

        /** WE CREATE THE 640x480 TEXTURE*/
        moTexParam tparam = MODefTex2DParams;
        //tparam.internal_format = GL_RGBA32F_ARB;
        tparam.internal_format = GL_RGB;
        int Mid = -1;

        /** DEPTH MAP texture*/
        Mid = GetResourceManager()->GetTextureMan()->AddTexture( "KINECTDEPTH", m_OutputMode.nXRes, m_OutputMode.nYRes, tparam );
        if (Mid>0) {
            m_pDepthTexture = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
            m_pDepthTexture->BuildEmpty(m_OutputMode.nXRes, m_OutputMode.nYRes);

            Mid = GetResourceManager()->GetTextureMan()->AddTexture( "KINECTDEPTH2", m_OutputMode.nXRes, m_OutputMode.nYRes, tparam );
            if (Mid>0) {
                m_pDepthTexture2 = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
            }

            pCloud = new moVector3f [m_OutputMode.nXRes*m_OutputMode.nYRes];
            //pCloudDif = new moVector3f[m_OutputMode.nXRes*m_OutputMode.nYRes];
            pCloudDif = new moVector3f [m_OutputMode.nXRes*m_OutputMode.nYRes];

            m_fbo_idx = m_pResourceManager->GetFBMan()->CreateFBO();
			pFBO = m_pResourceManager->GetFBMan()->GetFBO(m_fbo_idx);

			pFBO->AddTexture(   m_OutputMode.nXRes,
                                m_OutputMode.nYRes,
                                m_pDepthTexture->GetTexParam(),
                                m_pDepthTexture->GetGLId(),
                                attach_point );

            m_pDepthTexture->SetFBO( pFBO );
            m_pDepthTexture->SetFBOAttachPoint( attach_point );

            ///now we can draw:
            //m_pResourceManager->GetFBMan()->BindFBO( m_fbo_idx, attach_point );

            //pFBO->Bind();
            //pFBO->SetDrawTexture( m_pDepthTexture->GetFBOAttachPoint() );


        } else {
            MODebug2->Error("Couldn't create texture: KINECTDEPTH");
        }

    }


    /** Create an Image Generator */
    m_nRetVal = m_RGBImage.Create(m_Context);
    if (!CheckError()) {

        MODebug2->Error("Couldn't create ImageGenerator");
        return false;

    } else {

        /** WE CREATE THE 640x480 TEXTURE*/
        moTexParam tparam = MODefTex2DParams;
        //tparam.internal_format = GL_RGBA32F_ARB;
        tparam.internal_format = GL_RGB;
        int Mid = -1;

        /** RGB texture*/
        Mid = GetResourceManager()->GetTextureMan()->AddTexture( "KINECTRGB", m_OutputMode.nXRes, m_OutputMode.nYRes, tparam );
        if (Mid>0) {
            m_pRGBTexture = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
        } else {
            MODebug2->Error("Couldn't create texture: KINECTRGB");
        }

    }

    /** Create an IR Image Generator */
    /*
    m_nRetVal = m_IRImage.Create(m_Context);
    if (!CheckError()) {

        MODebug2->Error("Couldn't create IRGenerator");
        return false;

    } else {


        moTexParam tparam = MODefTex2DParams;
        //tparam.internal_format = GL_RGBA32F_ARB;
        tparam.internal_format = GL_RGB;
        int Mid = -1;


        Mid = GetResourceManager()->GetTextureMan()->AddTexture( "KINECTIR", m_OutputMode.nXRes, m_OutputMode.nYRes, tparam );
        if (Mid>0) {
            m_pIRTexture = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
        } else {
            MODebug2->Error("Couldn't create texture: KINECTIR");
        }

    }
*/



    /** GENERATING ALL */

    m_nRetVal = m_Context.StartGeneratingAll();
    if (!CheckError()) {
        MODebug2->Error("Couldn't start DepthGenerator");
        return false;
    } else {
        MODebug2->Message("Kinect DepthGenerator Started!!");
    }
#endif


#ifdef KINECT_PCL
    interface = new pcl::OpenNIGrabber();

    if (interface) {

        MODebug2->Message("OpenNIGrabber interface created.");

        boost::function<void (const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr&)> f =
            boost::bind (&moKinect::cloud_cb_, this, _1);

        // connect callback function for desired signal. In this case its a point cloud with color values
        boost::signals2::connection c = interface->registerCallback (f);

        // start receiving point clouds
        interface->start ();



    } else {
        MODebug2->Error("Couldn't create OpenNIGrabber interface");
        return false;
    }
#endif


	//levantamos los codes definidos
	coparam = m_Config.GetParamIndex("code");
	m_Config.SetCurrentParamIndex(coparam);
	ncodes = m_Config.GetValuesCount(coparam);

	Codes = new moKinectCode [ncodes];

	//ahora traduzco la info
    #ifdef _DEBUG
        text = moText("(KINECT) : ncodes: ") + IntToStr(ncodes);
        MODebug2->Log(text);
    #endif



	for( i = 0; i < ncodes; i++) {
		m_Config.SetCurrentValueIndex(coparam,i);
		Codes[i].strcod = m_Config.GetParam().GetValue().GetSubValue(MO_KINECT_CFG_STRCOD).Text();
		Codes[i].code = i;

		/** especial*/
		/*Codes[i].kinectcod = getMouseCod( m_Config.GetParam().GetValue().GetSubValue(MO_KINECT_CFG_KINECTCOD).Text() );*/

		Codes[i].type = m_Config.GetParam().GetValue().GetSubValue(MO_KINECT_CFG_TYPE).Int();
		Codes[i].state = MO_FALSE;//inicializamos en vacio

		if(Codes[i].kinectcod==-1) {
            text = moText("(kinect) : no se encuentra el code: ");
            text += Codes[i].strcod;
			MODebug2->Error(text);
			return false;
		}
	}



#ifdef KINECT_PCL
/*
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr msg(new pcl::PointCloud<pcl::PointXYZRGB>);

    PclMsg = msg;
*/
/*
    //pcl::PCDReader reader;
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
    //reader.read ("table_scene_lms400.pcd", *cloud);

    pcl::VoxelGrid<pcl::PointXYZ> vg;
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered (new pcl::PointCloud<pcl::PointXYZ>);
    //vg.setInputCloud (cloud);
    //vg.setLeafSize (0.01, 0.01, 0.01);

    //vg.filter (*cloud_filtered);

    MODebug2->Push( moText("point size:") + IntToStr(cloud->points.size() )   );
*/

/*
    msg->header.frame_id = "some_tf_frame";
    msg->height = 480;
    msg->width = 640;
     //msg->fields.resize(4);
     //msg->fields[0].name = "x";
     //msg->fields[1].name = "y";
     //msg->fields[2].name = "z";
     //msg->fields[3].name = "rgb";
    msg->is_dense = true;
    msg->points.resize(640*480);

    for( int v=0,i=0;v<480;v++)
    {
      for ( int u = 0; u<640;u++,i++)
       {
        pcl::PointXYZ result;
        result.x = 0.0;  //(u-cx)*(RawDepthtoMeters(depth[i]) + minDistance*scaleFactor*(cx/cy);
        result.y = 0.0;  //(v-cy)*(RawDepthtoMeters(depth[i]) + minDistance*scaleFactor;
        result.z = 100.0; //RawDepethtoMeters(depth[i]);
        msg->points.push.back(result);
       }
    }
*/

    //v.run ();
#endif

	return true;
}


MOswitch
moKinect::SetStatus(MOdevcode devcode, MOswitch state) {

	return true;
}


//notar que el devcode es a su vez el indice dentro del array de Codes
MOswitch
moKinect::GetStatus(MOdevcode devcode) {
    if (Codes && 0<=devcode && devcode<ncodes)
        return(Codes[devcode].state);
    else
        return -1;
}

MOint
moKinect::GetValue( MOdevcode devcode) {
    if (Codes && 0<=devcode && devcode<ncodes)
        return(Codes[devcode].value);
    else
        return -1;
}


MOdevcode
moKinect::GetCode(moText strcod) {

	MOint codi;
	MOuint i;
	MOint param,value;

	param = m_Config.GetCurrentParamIndex();
	value = m_Config.GetCurrentValueIndex( param );

	codi = m_Config.GetParamIndex( "code" );
	m_Config.SetCurrentParamIndex( codi );
	m_Config.FirstValue();

	for( i = 0; i < m_Config.GetValuesCount( codi ); i++) {
		m_Config.SetCurrentValueIndex( codi, i );
		if(!stricmp( strcod, m_Config.GetParam().GetValue().GetSubValue(MO_KINECT_CFG_STRCOD).Text() ) )
			return i;
	}

	m_Config.SetCurrentParamIndex( param );
	m_Config.SetCurrentValueIndex( param, value );

	return(-1);//error, no encontro
}

void
moKinect::Update(moEventList *Events) {
	MOuint i;
	moEvent *actual,*tmp;

    UpdateParameters();

#ifdef KINECT_OPENNI

    /** UPDATE DEPTH*/
    m_nRetVal = m_Context.WaitOneUpdateAll(m_Depth);
    if (!CheckError()) {
        MODebug2->Error("Kinect Failed to update data.");
    } else {
        const XnDepthPixel* pDepthMap = m_Depth.GetDepthMap();
        if ( pDepthMap ) {

            //MODebug2->Message("Getting Depth Map");

            /** Si es por primera vez: crear la textura: OUTLET... de textura??

            */

            if (!pData) {
                pData =  new unsigned char [ m_OutputMode.nXRes * m_OutputMode.nYRes * 3 ];

            }
            if (!pData2) {
                pData2 =  new unsigned char [ m_OutputMode.nXRes * m_OutputMode.nYRes * 3 ];

            }

            int s = 0;

            int disoff = 0.0;

            disoff = (int)(m_Offset.Y() - m_Offset.X());

            float distance_r = 0.0;

            /*moVector3f ReferencePointDimensionAux;
            if (m_ReferencePointDimension.Z()>0.01) {
                ReferencePointDimensionAux = moVector3f( m_ReferencePointDimension.X()/m_ReferencePointDimension.Z(),
                                                   m_ReferencePointDimension.Y()/m_ReferencePointDimension.Z(),
                                                   m_ReferencePointDimension.Z() );
            } else {
                ReferencePointDimensionAux = moVector3f( 0.0, 0.0, 0.0);
            }*/

            int n_base_max = 9;
            float minDistance = -10;
            float x,y,z;
            int ical,jcal;
            int indice_base_h[n_base_max*2];
            int indice_base_v[n_base_max*2];
            int n_base_h;
            int n_base_v;

            n_base_h = 0;
            n_base_v = 0;
            ical = 0;
            jcal = 0;

            if (pData && pData2) {
                for( int j=0; j<m_OutputMode.nYRes; j++ ) {
                    for( int i=0; i<m_OutputMode.nXRes; i++ ) {


                        //pData[i*3+j*m_OutputMode.nXRes*3] = *pDepthMap % 255;
                        //pData[i*3+1+j*m_OutputMode.nXRes*3] = *pDepthMap % 255;
                        //pData[i*3+2+j*m_OutputMode.nXRes*3] = *pDepthMap % 255;
                        z = (float)(*pDepthMap);

                        if ( m_Offset.X()<(*pDepthMap) && (*pDepthMap)<m_Offset.Y()) {

                            pData[i*3+j*m_OutputMode.nXRes*3] = (*pDepthMap - disoff) % 255;//(*pDepthMap - disoff) % 255;
                            //pData[i*3+1+j*m_OutputMode.nXRes*3] = (*pDepthMap - disoff) % 255;
                            //pData[i*3+2+j*m_OutputMode.nXRes*3] = (*pDepthMap - disoff) % 255;
                            pData[i*3+1+j*m_OutputMode.nXRes*3] = 0;
                            pData[i*3+2+j*m_OutputMode.nXRes*3] = 0;

                            pData2[i*3+j*m_OutputMode.nXRes*3] = 0;
                            pData2[i*3+1+j*m_OutputMode.nXRes*3] = 0;
                            pData2[i*3+2+j*m_OutputMode.nXRes*3] = 0;

                            float z2 = ( (float) z * scaleFactor );
                            //x = (i - 320) * (z2 + minDistance) * scaleFactor;
                            //y = (240 - j) * (z2 + minDistance) * scaleFactor;
                            x = (i - 320) * z2 / 320;
                            y = (240 - j) * z2 / 240;


                            if (pCloud && pCloudDif) {

                                pCloud[i+j*m_OutputMode.nXRes] = moVector3f( x,y,z );

                            //difference with Reference
                                pCloudDif[i+j*m_OutputMode.nXRes] = pCloud[i+j*m_OutputMode.nXRes] - m_ReferencePoint;

                                distance_r = pCloudDif[i+j*m_OutputMode.nXRes].Length();

                                if (distance_r > m_RadMax ) {
                                    //pData[i*3+1+j*m_OutputMode.nXRes*3] = 0;
                                } else if ( distance_r > m_RadMin) {
                                    pData[i*3+j*m_OutputMode.nXRes*3] = 0;
                                    pData[i*3+1+j*m_OutputMode.nXRes*3] = 0;
                                    pData[i*3+2+j*m_OutputMode.nXRes*3] = ((int)(distance_r) % 255);
                                } else if (distance_r < m_RadMin ) {
                                    pData[i*3+j*m_OutputMode.nXRes*3] = 255;
                                    pData[i*3+1+j*m_OutputMode.nXRes*3] = 0;
                                    pData[i*3+2+j*m_OutputMode.nXRes*3] = 255;

                                    if ( calibrate_base==1 && n_base_h < n_base_max  && abs(ical-i)>1 && i>ical ) {
                                        indice_base_h[ n_base_h*2 ] = i;
                                        indice_base_h[ n_base_h*2 + 1 ] = j;
                                        ical = i;
                                        jcal = j;
                                        n_base_h++;
                                        MODebug2->Push("Se agrego un indice a la base H");
                                    }
                                    if (j>jcal) {
                                        ical = 0;
                                    }
                                }

                                /*si ya sacamos una normal*/
                                if (BaseNormal.Length() > 0.01) {
                                    /**calculamos los puntos que estan sobre la base*/
                                    moVector3f ResV = pCloud[i+j*m_OutputMode.nXRes] - m_ReferencePoint;
                                    float res = BaseNormal.Dot( ResV );
                                    /*si ya tenemos un resultado del escalar, si enegativo esta por encima...*/
                                    if ( res < 0.1 ) {
                                         pData[i*3+j*m_OutputMode.nXRes*3] = 0;
                                         pData[i*3+1+j*m_OutputMode.nXRes*3] = (*pDepthMap - disoff) % 255;
                                         pData[i*3+2+j*m_OutputMode.nXRes*3] = 0;

                                        pData2[i*3+j*m_OutputMode.nXRes*3] = (*pDepthMap - disoff) % 255;
                                        pData2[i*3+1+j*m_OutputMode.nXRes*3] = (*pDepthMap - disoff) % 255;
                                        pData2[i*3+2+j*m_OutputMode.nXRes*3] = (*pDepthMap - disoff) % 255;
                                    }

                                }

                                if ( i>=319 && i<=321 && j>=239 && j<=241) {

                                    pData[i*3+j*m_OutputMode.nXRes*3] = 0;
                                    pData[i*3+1+j*m_OutputMode.nXRes*3] = 255;
                                    pData[i*3+2+j*m_OutputMode.nXRes*3] = 0;
                                    if (i==320 && j==240) {
                                        /*
                                        MODebug2->Push( IntToStr(*pDepthMap) + moText("(0,0) <x> ") + FloatToStr(x)
                                                   + moText(" <y> ") + FloatToStr(y)
                                                   + moText(" <z> ") + FloatToStr(z)
                                                   + moText(" <d> ") + FloatToStr(distance_r) );
                                                   */
                                    }
                                }

                                if (i>=69 && i<=71 && j>=69 && j<=71) {
                                    pData[i*3+j*m_OutputMode.nXRes*3] = 0;
                                    pData[i*3+1+j*m_OutputMode.nXRes*3] = 255;
                                    pData[i*3+2+j*m_OutputMode.nXRes*3] = 255;
                                    if (i==70 && j==70) {
                                        /*
                                        MODebug2->Push( IntToStr(*pDepthMap) + moText("(70,70) <x> ") + FloatToStr(x)
                                                   + moText(" <y> ") + FloatToStr(y)
                                                   + moText(" <z> ") + FloatToStr(z)
                                                   + moText(" <d> ") + FloatToStr(distance_r) );*/
                                     }
                                }
                            } //fin analisis

                        } else {

                            pData[i*3+j*m_OutputMode.nXRes*3] = 0;
                            pData[i*3+1+j*m_OutputMode.nXRes*3] = 0;
                            pData[i*3+2+j*m_OutputMode.nXRes*3] = 0;

                            pData2[i*3+j*m_OutputMode.nXRes*3] = 0;
                            pData2[i*3+1+j*m_OutputMode.nXRes*3] = 0;
                            pData2[i*3+2+j*m_OutputMode.nXRes*3] = 0;

                        }


                        pDepthMap++;
                    }
                }
                /*CALCULAMOS LA NORMAL DE LA BASE*/
                if ( calibrate_base==1 && n_base_h == n_base_max ) {

                    moVector3f vU,vV,vN;

                    BaseNormal = moVector3f(0,0,0);

                    if (pCloud && pCloudDif)
                    for(int nb=0; nb<(n_base_max-4);nb++) {
                        vU = pCloud[indice_base_h[0+nb*2]+indice_base_h[1+nb*2]*m_OutputMode.nXRes] - pCloud[ indice_base_h[2+nb*2]+indice_base_h[3+nb*2]*m_OutputMode.nXRes ];
                        vV = pCloud[indice_base_h[0+nb*2]+indice_base_h[1+nb*2]*m_OutputMode.nXRes] - pCloud[ indice_base_h[0+nb*2]+(indice_base_h[0+nb*2]+1)*m_OutputMode.nXRes ];
                        vU.Normalize();
                        vV.Normalize();
                        vN = vU.Cross(vV);
                        vN.Normalize();
                        BaseNormal = BaseNormal+vN;

                    }

                    BaseNormal.Normalize();
                    MODebug2->Push( moText("Se encontró una normal:")
                                            + moText(" x:") + FloatToStr( BaseNormal.X() )
                                            + moText(" y:") + FloatToStr( BaseNormal.Y() )
                                            + moText(" z:") + FloatToStr( BaseNormal.Z() )
                            );
                }

                if (m_pDepthTexture) {
                    m_pDepthTexture->BuildFromBuffer( m_OutputMode.nXRes, m_OutputMode.nYRes, pData, GL_RGB, GL_UNSIGNED_BYTE );
                }

                if (m_pDepthTexture2) {
                    m_pDepthTexture2->BuildFromBuffer( m_OutputMode.nXRes, m_OutputMode.nYRes, pData2, GL_RGB, GL_UNSIGNED_BYTE );
                }

            }
        }
    }


    /** Draw some 3d Stuff */

    if (pFBO && m_pDepthTexture) {

        pFBO->Bind();
        pFBO->SetDrawTexture( m_pDepthTexture->GetFBOAttachPoint() );

        float prop = (float)m_pDepthTexture->GetHeight() / (float)m_pDepthTexture->GetWidth();

        //m_pResourceManager->GetGLMan()->SetOrthographicView();
         m_pResourceManager->GetGLMan()->SetPerspectiveView( m_pDepthTexture->GetWidth(), m_pDepthTexture->GetHeight() );

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        /// changing FOV, pasar esto a PerspectiveView
        gluPerspective( fovy, 1.0 / prop, 0.1f ,4000.0f);
        /// point is Kinect: 0,0,0  , ViewPoint is any point ahead (0,0,Z>0 )
        gluLookAt(		0.0,
                            0.0,
                            0.0,
                            //m_Config[moR(PARTICLES_VIEWX)].GetData()->Fun()->Eval(state.tempo.ang)
                            0.0,
                            //m_Config[moR(PARTICLES_VIEWY)].GetData()->Fun()->Eval(state.tempo.ang)
                            0.0,
                            //m_Config[moR(PARTICLES_VIEWZ)].GetData()->Fun()->Eval(state.tempo.ang)
                            1000.0,
                            0, 1, 0);

        //glDisable(GL_DEPTH_TEST);

        float ancho = 10, alto = 10; //milimetros = 10 cm

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef( -m_ReferencePoint.X(), -m_ReferencePoint.Y(), m_ReferencePoint.Z() );
        //glRotatef( moTimeManager::MoldeoTimer->Duration()/100, 0.0, 0.0, 1.0 );

        glBindTexture( GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

        glColor4f( 1,0.5,0.5,1 );
        glBegin(GL_QUADS);
          glTexCoord2f( 0.0, 1.0 );
          glVertex3f( -0.5*ancho, -0.5*alto, 0 );

          glTexCoord2f( 1.0, 1.0 );
          glVertex3f(  0.5*ancho, -0.5*alto, 0);

          glTexCoord2f( 1.0, 0.0 );
          glVertex3f(  0.5*ancho,  0.5*alto, 0);

          glTexCoord2f( 0.0, 0.0 );
          glVertex3f( -0.5*ancho,  0.5*alto, 0);
        glEnd();


        //m_ReferencePoint*= 1;

        glLoadIdentity();

        glLineWidth((GLfloat)3.0);
        glBegin(GL_LINES);
            glColor4f( 1,1,1,1 );
            glVertex3f( -m_ReferencePoint.X(), -m_ReferencePoint.Y(), m_ReferencePoint.Z() );
            glColor4f( 1,1,1,1 );
            glVertex3f( -m_ReferencePoint.X()-BaseNormal.X()*100, -m_ReferencePoint.Y()-BaseNormal.Y()*100, m_ReferencePoint.Z()+BaseNormal.Z()*100 );
        glEnd();

        /**
        *
        *   PRINTING ADDITIONAL
        *
        */
        m_pResourceManager->GetGLMan()->SetOrthographicView( m_pDepthTexture->GetWidth(), m_pDepthTexture->GetHeight() );

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef( base_camera_i, base_camera_j, 0 );
        glColor4f( 1,1,1,1 );
        glBegin(GL_QUADS);
          glTexCoord2f( 0.0, 1.0 );
          glVertex3f( -0.5*ancho, -0.5*alto, 0 );

          glTexCoord2f( 1.0, 1.0 );
          glVertex3f(  0.5*ancho, -0.5*alto, 0);

          glTexCoord2f( 1.0, 0.0 );
          glVertex3f(  0.5*ancho,  0.5*alto, 0);

          glTexCoord2f( 0.0, 0.0 );
          glVertex3f( -0.5*ancho,  0.5*alto, 0);
        glEnd();



        pFBO->Unbind();

    }


    m_nRetVal = m_Context.WaitOneUpdateAll(m_RGBImage);
    if (!CheckError()) {
        MODebug2->Error("Kinect Failed to update data.");
    } else {
        const XnRGB24Pixel*    pImageMap = m_RGBImage.GetRGB24ImageMap();
        if (pImageMap) {

            if (!pImage) {
                pImage =  new unsigned char [ m_OutputMode.nXRes * m_OutputMode.nYRes * 3 ];

            }

            if (pImage) {
                for( int j=0; j<m_OutputMode.nYRes; j++ ) {
                    for( int i=0; i<m_OutputMode.nXRes; i++ ) {
                            pImage[i*3+j*m_OutputMode.nXRes*3] = (*pImageMap).nRed;
                            pImage[i*3+1+j*m_OutputMode.nXRes*3] = (*pImageMap).nGreen;
                            pImage[i*3+2+j*m_OutputMode.nXRes*3] = (*pImageMap).nBlue;
                            if (i==320 && j==240) {
                                //MODebug2->Push( IntToStr((*pImageMap).nRed) + moText(" > ") + IntToStr(pImage[i*3+j*3*m_OutputMode.nXRes]) );
                            }
                            pImageMap++;
                        }
                }

                if (m_pRGBTexture) {
                    m_pRGBTexture->BuildFromBuffer( m_OutputMode.nXRes, m_OutputMode.nYRes, pImage, GL_RGB, GL_UNSIGNED_BYTE );
                }
            }
        }

    }

/*
    m_nRetVal = m_Context.WaitOneUpdateAll(m_IRImage);
    if (!CheckError()) {
        MODebug2->Error("Kinect Failed to update data.");
    } else {
        const XnIRPixel*    pImageMap = m_IRImage.GetIRMap();
        if (pImageMap) {

            if (!pIR) {
                pIR =  new unsigned char [ m_OutputMode.nXRes * m_OutputMode.nYRes * 3 ];

            }

            if (pIR) {
                for( int j=0; j<m_OutputMode.nYRes; j++ ) {
                    for( int i=0; i<m_OutputMode.nXRes; i++ ) {
                            pImage[i*3+j*m_OutputMode.nXRes*3] = (*pImageMap);
                            pImage[i*3+1+j*m_OutputMode.nXRes*3] = (*pImageMap);
                            pImage[i*3+2+j*m_OutputMode.nXRes*3] = (*pImageMap);
                            if (i==320 && j==240) {
                                MODebug2->Push( IntToStr((*pImageMap)) + moText(" > ") + IntToStr(pImage[i*3+j*3*m_OutputMode.nXRes]) );
                            }
                            pImageMap++;
                        }
                }

                if (m_pIRTexture) {
                    m_pIRTexture->BuildFromBuffer( m_OutputMode.nXRes, m_OutputMode.nYRes, pIR, GL_RGB, GL_UNSIGNED_BYTE );
                }
            }
        }

    }

*/

#endif


#ifdef KINECT_PCL

    m_DataLock.Lock();

    if ( show_callback == false ) {

        show_callback = true;

    }

    m_ReferenceNormal = m_CenterNormal;


    if ( calibrate_base==1) {

        /** grabamos la normal de la base */
        m_BaseNormal = m_CenterNormal;
        m_BasePosition = m_TargetPosition;
        m_BaseRGB = m_TargetRGB;
        moText texto;
        texto= moText("BasePosition: x:")+FloatToStr(m_BasePosition.X());
        texto+= moText(" y:")+FloatToStr(m_BasePosition.Y());
        texto+= moText(" z:")+FloatToStr(m_BasePosition.Z());
        texto+= moText(" Normal: x:")+FloatToStr( m_BaseNormal.normal[0] );
        texto+= moText(" y:")+FloatToStr(m_BaseNormal.normal[1]);
        texto+= moText(" z:")+FloatToStr(m_BaseNormal.normal[2]);
        MODebug2->Push(texto);
    }

    //m_CenterNormal.curvature = cloud_normals->points[ (_ww  >> 1) * (_hh + 1) ].curvature;

    moText texto;
    static unsigned int cc = 0;

    int curvature_index = this->GetOutletIndex( moText("CURVATURE") );
    moOutlet* outCurvature = m_Outlets.Get( curvature_index );

    if (outCurvature) {

        outCurvature->GetData()->SetLong( 100 * m_CenterNormal.curvature );
        outCurvature->Update(true);
    }

    int r_index = this->GetOutletIndex( moText("R") );
    int g_index = this->GetOutletIndex( moText("G") );
    int b_index = this->GetOutletIndex( moText("B") );

    moOutlet* outR = m_Outlets.Get( r_index );
    moOutlet* outG = m_Outlets.Get( g_index );
    moOutlet* outB = m_Outlets.Get( b_index );

    if (outR) {
        outR->GetData()->SetFloat( m_TargetRGB.X() );
        outR->Update(true);
    }
    if (outG) {
        outG->GetData()->SetFloat( m_TargetRGB.Y() );
        outG->Update(true);
    }
    if (outB) {
        outB->GetData()->SetFloat( m_TargetRGB.Z() );
        outB->Update(true);
    }

    moVector3d HSV = RGBtoHSV( m_TargetRGB.X(), m_TargetRGB.Y(), m_TargetRGB.Z() );

    m_TargetHSV = moVector3f( HSV.X(), HSV.Y(), HSV.Z() );

    int h_index = this->GetOutletIndex( moText("H") );
    int s_index = this->GetOutletIndex( moText("S") );
    int v_index = this->GetOutletIndex( moText("V") );

    moOutlet* outH = m_Outlets.Get( h_index );
    moOutlet* outS = m_Outlets.Get( s_index );
    moOutlet* outV = m_Outlets.Get( v_index );

    if (outH) {
        outH->GetData()->SetFloat( m_TargetHSV.X() );
        outH->Update(true);
    }
    if (outS) {
        outS->GetData()->SetFloat( m_TargetHSV.Y() );
        outS->Update(true);
    }
    if (outV) {
        outV->GetData()->SetFloat( m_TargetHSV.Z() );
        outV->Update(true);
    }


    ///si es superior a 0  o max  es que hay presencia
    int hay_presencia_index = this->GetOutletIndex( moText("HAY_PRESENCIA") );
    moOutlet* outHayPresencia = m_Outlets.Get( hay_presencia_index );
    if (outHayPresencia) {
        outHayPresencia->GetData()->SetLong( m_haypresencia );
        outHayPresencia->Update(true);
    }


    if (++cc==60 && outCurvature) {
        texto = moText(" CURVATURE Updated: ") + IntToStr( outCurvature->GetData()->Long() );
        texto+= moText(" R: ") + FloatToStr( m_TargetRGB.X() );
        texto+= moText(" G: ") + FloatToStr( m_TargetRGB.Y() );
        texto+= moText(" B: ") + FloatToStr( m_TargetRGB.Z() );


        texto+= moText(" H: ") + FloatToStr( m_TargetHSV.X() );
        texto+= moText(" S: ") + FloatToStr( m_TargetHSV.Y() );
        texto+= moText(" V: ") + FloatToStr( m_TargetHSV.Z());
        //MODebug2->Push(texto);

        texto = moText(" HAY_PRESENCIA Updated: ") + IntToStr( outHayPresencia->GetData()->Long() );
        //MODebug2->Push(texto);
        cc= 0;

    }


    //m_CenterPoint = m_ReferencePoint;

    /*
     MODebug2->Push( moText(" Normal <x> ") + FloatToStr(m_CenterNormal.normal[0])
                   + moText(" <y> ") + FloatToStr(m_CenterNormal.normal[1])
                   + moText(" <z> ") + FloatToStr(m_CenterNormal.normal[2])
                   + moText(" <curvature> ") + FloatToStr(m_CenterNormal.curvature) );
    */
    if (pImage) {

        if (m_pRGBTexture) {
            m_pRGBTexture->BuildFromBuffer( m_OutputMode.nXRes, m_OutputMode.nYRes, pImage, GL_RGB, GL_UNSIGNED_BYTE );
        }
    }


    if (pData) {

        if (m_pDepthTexture) {
            m_pDepthTexture->BuildFromBuffer( m_OutputMode.nXRes, m_OutputMode.nYRes, pData, GL_RGB, GL_UNSIGNED_BYTE );
        }
    }

    if (pData2) {
        if (m_pDepthTexture2) {
            m_pDepthTexture2->BuildFromBuffer( m_OutputMode.nXRes, m_OutputMode.nYRes, pData2, GL_RGB, GL_UNSIGNED_BYTE );
        }
    }


    if (pFBO && m_pDepthTexture) {

        pFBO->Bind();
        pFBO->SetDrawTexture( m_pDepthTexture->GetFBOAttachPoint() );

        float prop = (float)m_pDepthTexture->GetHeight() / (float)m_pDepthTexture->GetWidth();

        //m_pResourceManager->GetGLMan()->SetOrthographicView();
         m_pResourceManager->GetGLMan()->SetPerspectiveView( m_pDepthTexture->GetWidth(), m_pDepthTexture->GetHeight() );

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        /// changing FOV, pasar esto a PerspectiveView
        gluPerspective( fovy, 1.0 / prop, 0.1f ,4000.0f);
        /// point is Kinect: 0,0,0  , ViewPoint is any point ahead (0,0,Z>0 )
        gluLookAt(		0.0,
                            0.0,
                            0.0,
                            //m_Config[moR(PARTICLES_VIEWX)].GetData()->Fun()->Eval(state.tempo.ang)
                            0.0,
                            //m_Config[moR(PARTICLES_VIEWY)].GetData()->Fun()->Eval(state.tempo.ang)
                            0.0,
                            //m_Config[moR(PARTICLES_VIEWZ)].GetData()->Fun()->Eval(state.tempo.ang)
                            1000.0,
                            0, 1, 0);

        //glDisable(GL_DEPTH_TEST);

        float ancho = 10, alto = 10; //milimetros = 10 cm

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef( -m_ReferencePoint.X(), -m_ReferencePoint.Y(), m_ReferencePoint.Z() );
        //glRotatef( moTimeManager::MoldeoTimer->Duration()/100, 0.0, 0.0, 1.0 );

        glBindTexture( GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

        glColor4f( 1,0.5,0.5,1 );
        glBegin(GL_QUADS);
          glTexCoord2f( 0.0, 1.0 );
          glVertex3f( -0.5*ancho, -0.5*alto, 0 );

          glTexCoord2f( 1.0, 1.0 );
          glVertex3f(  0.5*ancho, -0.5*alto, 0);

          glTexCoord2f( 1.0, 0.0 );
          glVertex3f(  0.5*ancho,  0.5*alto, 0);

          glTexCoord2f( 0.0, 0.0 );
          glVertex3f( -0.5*ancho,  0.5*alto, 0);
        glEnd();


        //m_ReferencePoint*= 1;

        glLoadIdentity();

        glLineWidth((GLfloat)3.0);
        glBegin(GL_LINES);
            glColor4f( 1,1,1,1 );
            glVertex3f( -m_BasePosition.X(), -m_BasePosition.Y(), m_BasePosition.Z() );
            glColor4f( 1,1,1,1 );
            glVertex3f( -m_BasePosition.X()-m_BaseNormal.normal[0]*100, -m_BasePosition.Y()-m_BaseNormal.normal[1]*100, m_BasePosition.Z()+m_BaseNormal.normal[2]*100 );
        glEnd();

        /**
        *
        *   PRINTING ADDITIONAL
        *
        */
        m_pResourceManager->GetGLMan()->SetOrthographicView( m_pDepthTexture->GetWidth(), m_pDepthTexture->GetHeight() );

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslatef( base_camera_i, base_camera_j, 0 );
        glColor4f( 1,1,1,1 );
        glBegin(GL_QUADS);
          glTexCoord2f( 0.0, 1.0 );
          glVertex3f( -0.5*ancho, -0.5*alto, 0 );

          glTexCoord2f( 1.0, 1.0 );
          glVertex3f(  0.5*ancho, -0.5*alto, 0);

          glTexCoord2f( 1.0, 0.0 );
          glVertex3f(  0.5*ancho,  0.5*alto, 0);

          glTexCoord2f( 0.0, 0.0 );
          glVertex3f( -0.5*ancho,  0.5*alto, 0);
        glEnd();



        pFBO->Unbind();

    }

    m_DataLock.Unlock();

#endif

    moMoldeoObject::Update( Events );

}


void moKinect::cloud_cb_ (const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr &cloud)
{

        /// 100 x 100 pixels , centered on platform: coordinate: platform_i, platform_j
    size_t _ww = 80;
    size_t _hh = 50;
    size_t platform_i = 320;
    size_t platform_j = 240;
    size_t left_src_i = 0;
    size_t top_src_j = 0;


    m_DataLock.Lock();


    platform_i = base_camera_i;
    platform_j = base_camera_j;

    m_DataLock.Unlock();



    static unsigned count = 0;
    static unsigned count2 = 0;
    static double last = pcl::getTime ();

    moText texto;

    if (++count == 90)
    {

        double now = pcl::getTime ();
        //std::cout << "distance of center pixel :" << cloud->points [(cloud->width >> 1) * (cloud->height + 1)].z << " mm. Average framerate: " << double(count)/double(now - last) << " Hz" <<  std::endl;

        texto = moText("OpenNIGrabber callback");
        texto+= moText(" distance of platform pixel: ") + FloatToStr( cloud->points [ platform_i + platform_j * cloud->width].z );
        texto+= moText(" m. Average framerate: ") + FloatToStr( double(count)/double(now - last) );
        MODebug2->Push(texto);

        count = 0;
        last = now;

    }

    m_DataLock.Lock();

    if (!pImage) {
        pImage =  new unsigned char [ m_OutputMode.nXRes * m_OutputMode.nYRes * 3 ];
    }

    if (!pData) {
        pData =  new unsigned char [ m_OutputMode.nXRes * m_OutputMode.nYRes * 3 ];
    }

    ///RGB y Alpha como Z (depth)
    /// r=z    g=r   b=g    a = (1.0 - b) blue > no hay en las frutas...
    if (!pData2) {
        pData2 =  new unsigned char [ m_OutputMode.nXRes * m_OutputMode.nYRes * 3 ];
    }


    int index_src;
    int index_dst;
    //float tiltangulo = moMath.Tan( moMath.DegToRad((float) camera_angulo ) );
    m_haypresencia = 0; ///va sumando por cada punto que este dentro del rango...

    if (pImage && pData && pData2)
    for (size_t jj = 0; jj < m_OutputMode.nYRes; jj++) {
        for (size_t ii = 0; ii < m_OutputMode.nXRes; ii++) {

            index_src = ii+jj*m_OutputMode.nXRes;
            index_dst = ii*3+jj*m_OutputMode.nXRes*3;

            pImage[index_dst] = cloud->points[index_src].r;
            pImage[index_dst+1] = cloud->points[index_src].g;
            pImage[index_dst+2] = cloud->points[index_src].b;

            int zcol = (int)( (float) (cloud->points[index_src].z * 1000.0f) ) ;
            pData[index_dst] = zcol % 255;
            pData[index_dst+1] = 0;
            pData[index_dst+2] = 0;

            /// calculo si hay presencia

            if (  jj < ( platform_j - umbral_camera_presencia_y_offset )  ) {

                if ( umbral_camera_presencia_z_near< zcol && zcol < umbral_camera_presencia_z_far) {
                    m_haypresencia+= 1;
                }
            }

            /// calculamos la zona de trackeo
            /// if < platform_j => el z se va alejando a medida que subimos proporcionalmente al angulo tilt...
            int ztrack = 0;
            pData2[index_dst] = 0;
            pData2[index_dst+1] = 0;
            pData2[index_dst+2] = 0;

            if (  jj < ( platform_j - umbral_trackeo_y_offset )  ) {
                ///calculamos el z nuevo, con tilt:
                ztrack = zcol;
                ztrack-= (platform_j - jj) * trackeo_factor_angulo;

                if ( umbral_trackeo_z_near < ztrack   &&   ztrack < umbral_trackeo_z_far  ) {

                    ///distancia al naranja
                    ///distancia al amarillo
                    ///distancia a la sandia: esto puede venir definido directamente desde la eleccion de (naranja,pomelo,sandia)

                    float alf = ( 255.0 - cloud->points[index_src].b) / 255.0;

                    float sum = 0.0;

                    sum = (int) (alf * 255  * (ztrack - umbral_trackeo_z_near) / (  umbral_trackeo_z_far -  umbral_trackeo_z_near));
                    sum+= (int) (alf * cloud->points[index_src].r);
                    sum+= (int) (alf * cloud->points[index_src].g);
                    (sum < 255) ? sum = 0.0 : sum = sum / 3;

                    pData2[index_dst] = sum;
                    pData2[index_dst+1] = sum;
                    pData2[index_dst+2] = sum;

                }
            }


        }
    }

    index_src = platform_i+platform_j*m_OutputMode.nXRes;

    m_TargetPosition = moVector3f( cloud->points[index_src].x, cloud->points[index_src].y, cloud->points[index_src].z );
    m_TargetRGB = moVector3f( cloud->points[index_src].r, cloud->points[index_src].g, cloud->points[index_src].b );

    m_DataLock.Unlock();


    if (++count2==60) {

        pcl::PointCloud<pcl::PointXYZ>::Ptr cloudxyz (new pcl::PointCloud<pcl::PointXYZ> ());

        ///esto se podria acelerar
        /**creando el análisis únicamente sobre la plataforma*/
/*
        size_t _ww = cloud->width;
        size_t _hh = cloud->height;
        cloudxyz->points.resize(cloud->size());
        for (size_t i = 0; i < cloudxyz->points.size(); i++) {
            cloudxyz->points[i].x = cloud->points[i].x;
            cloudxyz->points[i].y = cloud->points[i].y;
            cloudxyz->points[i].z = cloud->points[i].z;
        }
*/

        cloudxyz->points.resize( _ww * _hh);

        left_src_i = platform_i - _ww / 2;
        top_src_j = platform_j - _hh / 2;

        /*
        medimos ancho y alto de los objetos ??
        por ahora en el centro
        */

        for (size_t hh = 0; hh < _hh; hh++) {

            for (size_t ww = 0; ww < _ww; ww++) {
                //size_t index_src = (platform_i + ( ww + ( cloud->width - _ww )/2 ) ) + (platform_j + ( hh + ( cloud->height- _hh ) /2 ))*cloud->width;
                size_t index_src_i = ww + left_src_i;
                size_t index_src_j = hh + top_src_j;
                size_t index_src = index_src_i + index_src_j * cloud->width;
                size_t index_dst = ww  + hh*_ww;
                //index_src = max( index_src, 0 );
                //index_src = min( index_src, _ww*_hh );

                cloudxyz->points[ index_dst ].x = cloud->points[index_src].x;
                cloudxyz->points[ index_dst ].y = cloud->points[index_src].y;
                cloudxyz->points[ index_dst ].z = cloud->points[index_src].z;
            }

        }



        //pcl::copyPointCloud( cloud_rgb->makeShared(),  cloudxyz->makeShared() );

        ///Compute normals
        ne.setInputCloud ( cloudxyz );


        pcl::KdTreeFLANN<pcl::PointXYZ>::Ptr tree (new pcl::KdTreeFLANN<pcl::PointXYZ> ());
        ne.setSearchMethod (tree);

        // Output datasets
        pcl::PointCloud<pcl::Normal>::Ptr cloud_normals (new pcl::PointCloud<pcl::Normal>);

        // Use all neighbors in a sphere of radius 3cm
        ne.setRadiusSearch ( 0.03 );

        // Compute the features
        ne.compute (*cloud_normals);

        /** Ahora en funcion de las normales estudiamos que dimensiones tiene el objeto*/
        /*
        float i_left_curvo = _ww;//en indice
        float x_left_curvo = 0; //en metros

        float i_right_curvo = 0;
        float x_right_curvo = 0;

        float j_top_curvo = _hh;
        float y_top_curvo = 0;

        float j_bottom_curvo = 0;
        float y_bottom_curvo = 0;

        pcl::Normal Nactual;
        int cluster_p = 0;

        float curv = 0.0;

        for (size_t hh = 0; hh < _hh; hh++) {

            for (size_t ww = 0; ww < _ww; ww++) {


                index_src = ww+hh*_ww;
                Nactual = cloud_normals->points[index_src];
                /// solo procesamos aquellas normales que tengan curvatura...
                if (Nactual.curvature)
                    curv+= Nactual.curvature;

                if ( Nactual.curvature > 0.01 ) {

                    cluster_p++;

                    if ( i_left_curvo > ww ) {
                        i_left_curvo = ww ;
                        x_left_curvo = cloudxyz->points[ index_src ].x;
                    }

                    if ( i_right_curvo < ww ) {
                        i_right_curvo = ww;
                        x_right_curvo = cloudxyz->points[index_src ].x;
                    }

                    if ( j_top_curvo > hh ) {
                        j_top_curvo = hh;
                        y_top_curvo = cloudxyz->points[ index_src ].y;
                    }

                    if ( j_bottom_curvo < hh ) {
                        j_bottom_curvo = hh;
                        y_bottom_curvo = cloudxyz->points[ index_src ].y;
                    }

                }

            }
        }

        float dim_w = fabs(x_right_curvo - x_left_curvo);
        float dim_h = fabs(y_top_curvo - y_bottom_curvo);
        curv = curv / (_ww*_hh);

        texto= moText(" dimensiones: w:") + FloatToStr( dim_w );
        texto+= moText(" h:") + FloatToStr( dim_h );
        texto+= moText(" Cluster:") + IntToStr( cluster_p );
        texto+= moText(" Curvature:") + FloatToStr( curv );
        MODebug2->Push(texto);
        */

        //if (cloud_normals) {
            texto= moText(" normals size: ") + IntToStr( cloud_normals->size() );
            texto+= moText(" curvature for the center pixel: ") + FloatToStr( cloud_normals->points [(_ww >> 1) * (_hh + 1)].curvature );
        //}

            MODebug2->Push(texto);


            m_DataLock.Lock();

            moVector3f Pos;
            pcl::Normal Norm;

            Pos = m_BasePosition;
            Norm = m_BaseNormal;

            m_DataLock.Unlock();

            float sumsca;
            float sumA;
            int pointUp;
            moVector3f VPoint;
            moVector3f NPlane;

            NPlane = moVector3f( m_BaseNormal.normal[0],
                                m_BaseNormal.normal[1],
                                m_BaseNormal.normal[2] );

            pointUp = 0;
            sumA = 0.0;

            for (size_t hh = 0; hh < _hh; hh++) {
                for (size_t ww = 0; ww < _ww; ww++) {
                    index_src = ww+hh*_ww;
                    VPoint = Pos - moVector3f(cloudxyz->points[ index_src ].x,
                                              cloudxyz->points[ index_src ].y,
                                              cloudxyz->points[ index_src ].z);

                    sumsca = NPlane.Dot( VPoint );

                    if (sumsca > 0.01 ) {
                        pointUp+= 1;
                        sumA+= sumsca;
                    }

                }
            }
            MODebug2->Push( moText("PointCloud pointUp: ")
                       + IntToStr(pointUp)
                       + moText(" sumscalares: ") + FloatToStr(sumA)  );


        /// segunda pasada
        /// extraccion del volumen
/*
        pcl::VoxelGrid<pcl::PointXYZ> vg;
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered (new pcl::PointCloud<pcl::PointXYZ>);
        vg.setInputCloud ( cloudxyz );
        vg.setLeafSize (0.005f, 0.005f, 0.005f);
        vg.filter (*cloud_filtered);
        MODebug2->Push( moText("PointCloud after filtering has: ")
                       + IntToStr(cloud_filtered->points.size ())
                       + moText(" data points.") );

*/
/*
        /// Create the segmentation object for the planar model and set all the parameters
        pcl::SACSegmentation<pcl::PointXYZ> seg;
        pcl::PointIndices::Ptr inliers (new pcl::PointIndices);
        pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients);
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_plane (new pcl::PointCloud<pcl::PointXYZ> ());
        pcl::PCDWriter writer;
        seg.setOptimizeCoefficients (true);
        seg.setModelType (pcl::SACMODEL_PLANE);
        seg.setMethodType (pcl::SAC_RANSAC);
        seg.setMaxIterations (100);
        seg.setDistanceThreshold (0.02);
*/

/*
        int i=0, nr_points = (int) cloud_filtered->points.size ();
        while (cloud_filtered->points.size () > 0.3 * nr_points)
        {
            /// Segment the largest planar component from the remaining cloud
            seg.setInputCloud(cloud_filtered);
            seg.segment (*inliers, *coefficients); //*
            if (inliers->indices.size () == 0)
            {
              //std::cout << "Could not estimate a planar model for the given dataset." << std::endl;
              MODebug2->Push(moText("Could not estimate a planar model for the given dataset.") );
              break;
            }

            /// Extract the planar inliers from the input cloud
            pcl::ExtractIndices<pcl::PointXYZ> extract;
            extract.setInputCloud (cloud_filtered);
            extract.setIndices (inliers);
            extract.setNegative (false);

            /// Write the planar inliers to disk
            extract.filter (*cloud_plane); //*
            //std::cout << "PointCloud representing the planar component: " << cloud_plane->points.size () << " data points." << std::endl;
            MODebug2->Push( moText("PointCloud representing the planar component:") + IntToStr(cloud_plane->points.size ()) + moText(" data points")  );

            /// Remove the planar inliers, extract the rest
            extract.setNegative (true);
            extract.filter (*cloud_filtered); //*
        }
*/
/*
        /// Creating the KdTree object for the search method of the extraction
        pcl::KdTree<pcl::PointXYZ>::Ptr kktree (new pcl::KdTreeFLANN<pcl::PointXYZ>);
        kktree->setInputCloud (cloud_filtered);

        std::vector<pcl::PointIndices> cluster_indices;
        pcl::EuclideanClusterExtraction<pcl::PointXYZ> ec;
        ec.setClusterTolerance (0.01); // 2cm
        ec.setMinClusterSize (5);
        ec.setMaxClusterSize (1000);
        ec.setSearchMethod (kktree);
        ec.setInputCloud( cloud_filtered);
        ec.extract (cluster_indices);

        int j = 0;
        int np = 0;
        for (std::vector<pcl::PointIndices>::const_iterator it = cluster_indices.begin (); it != cluster_indices.end (); ++it)
        {
            np = (*it).indices.size();
            MODebug2->Push( moText("PointCloud cluster points: ") + IntToStr(np) );
            j++;
        }

        MODebug2->Push( moText("PointCloud clusters: ") + IntToStr(j) );
*/

/*
        int j = 0;
        for (std::vector<pcl::PointIndices>::const_iterator it = cluster_indices.begin (); it != cluster_indices.end (); ++it)
        {
            pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_cluster (new pcl::PointCloud<pcl::PointXYZ>);
            for (std::vector<int>::const_iterator pit = it->indices.begin (); pit != it->indices.end (); pit++)
                cloud_cluster->points.push_back (cloud_filtered->points[*pit]); //*
            cloud_cluster->width = cloud_cluster->points.size ();
            cloud_cluster->height = 1;
            cloud_cluster->is_dense = true;

            //std::cout << "PointCloud representing the Cluster: " << cloud_cluster->points.size () << " data points." << std::endl;
            MODebug2->Push(
                            moText("PointCloud representing the Cluster: ")
                           + IntToStr(cloud_cluster->points.size ())
                           + moText(" data points.")
                            );

            //std::stringstream ss;
            //ss << "cloud_cluster_" << j << ".pcd";
            //writer.write<pcl::PointXYZ> (ss.str (), *cloud_cluster, false); //*
            j++;
        }
*/
        m_DataLock.Lock();

        //m_CenterNormal = cloud_normals->points[ (_ww  >> 1) * (_hh + 1) ];
        m_CenterNormal = cloud_normals->points[ (_ww  >> 1) * (_hh + 1)  ];
        m_TargetNormal = m_CenterNormal;

        m_DataLock.Unlock();


        count2 = 0;

    }


    /**Show Cloud*/

    if (pFBO && m_pDepthTexture && m_pResourceManager
        && m_pResourceManager->GetRenderMan()
        && m_pResourceManager->GetGLMan()
        && show_callback
        ) {

        /// Lock render manager
        ///m_pResourceManager->GetRenderMan()->BeginUpdateDevice();
/*
        pFBO->Bind();
        pFBO->SetDrawTexture( m_pDepthTexture->GetFBOAttachPoint() );


        float prop = (float)m_pDepthTexture->GetHeight() / (float)m_pDepthTexture->GetWidth();

        //m_pResourceManager->GetGLMan()->SetOrthographicView();
         m_pResourceManager->GetGLMan()->SetPerspectiveView( m_pDepthTexture->GetWidth(), m_pDepthTexture->GetHeight() );

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        // changing FOV, pasar esto a PerspectiveView
        gluPerspective( fovy, 1.0 / prop, 0.1f ,4000.0f);
        // point is Kinect: 0,0,0  , ViewPoint is any point ahead (0,0,Z>0 )
        gluLookAt(		0.0,
                            0.0,
                            0.0,
                            //m_Config[moR(PARTICLES_VIEWX)].GetData()->Fun()->Eval(state.tempo.ang)
                            0.0,
                            //m_Config[moR(PARTICLES_VIEWY)].GetData()->Fun()->Eval(state.tempo.ang)
                            0.0,
                            //m_Config[moR(PARTICLES_VIEWZ)].GetData()->Fun()->Eval(state.tempo.ang)
                            1000.0,
                            0, 1, 0);

        //glDisable(GL_DEPTH_TEST);

        float ancho = 100, alto = 100; //milimetros = 10 cm

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        int a=400,b=280;

        moVector3f Punto;
        moVector3f RGB;

        if (cloud) {

            //moVector3f pCenter = pCloud[ a + b*m_OutputMode.nXRes ];

            Punto = moVector3f( cloud->points [(cloud->width >> 1) * (cloud->height + 1)].x,
                                cloud->points [(cloud->width >> 1) * (cloud->height + 1)].y,
                                cloud->points [(cloud->width >> 1) * (cloud->height + 1)].z
                               );

            glColor4f( 1,1,0,1 );
            glLineWidth((GLfloat)10.0);

            glBegin(GL_LINES);
                glColor4f( 1,1,0,1 );
                glVertex3f( Punto.X(), -Punto.Y(), Punto.Z() );
                glColor4f( 1,1,1,1 );
                glVertex3f( Punto.X()+30, -Punto.Y()+30, Punto.Z() );
            glEnd();


            MODebug2->Push( moText("pCloud:")
                                            + moText(" x:") + FloatToStr( pCenter.X() )
                                            + moText(" y:") + FloatToStr( pCenter.Y() )
                                            + moText(" z:") + FloatToStr( pCenter.Z() )
                            );

        }

        pFBO->Unbind();
*/
        /// Unlock render manager
        ///m_pResourceManager->GetRenderMan()->EndUpdateDevice();

    }

    m_DataLock.Unlock();

}


MOboolean
moKinect::Finish() {

	if(Codes!=NULL)
		delete [] Codes;
	Codes = NULL;
	ncodes = 0;

	return true;
}



