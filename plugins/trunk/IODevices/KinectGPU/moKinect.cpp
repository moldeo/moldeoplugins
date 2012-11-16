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

    m_pTex1 = NULL;
    m_pTex2 = NULL;
    m_pTex3 = NULL;

    pDataObj1 = NULL;
    pDataObj2 = NULL;
    pDataObj3 = NULL;

    id_tex1 = -1;
    id_tex2 = -1;
    id_tex3 = -1;

    dif_tex1 = -1;
    dif_tex2 = -1;
    dif_tex3 = -1;

	show_callback = false;
	#ifdef KINECT_PCL
        m_center_curvature = 0.0;
        m_CenterNormal.normal[0] = 0.0;
        m_CenterNormal.normal[1] = 0.0;
        m_CenterNormal.normal[2] = 0.0;
        m_CenterNormal.normal[3] = 0.0;

        m_ReferenceNormal = m_CenterNormal;
	#endif

    m_pSrcTexture = NULL;
    m_pDest0Texture = NULL;
    m_pDest1Texture = NULL;
    m_pDest2Texture = NULL;
    m_pDestDiff1Texture = NULL;
    m_pDestDiff2Texture = NULL;

    m_pTFDest0Texture = NULL;
    m_pTFDest1Texture = NULL;
    m_pTFDest2Texture = NULL;
    m_pTFDestDiff1Texture = NULL;
    m_pTFDestDiff2Texture = NULL;

    m_pTrackerSystemData = NULL;
    m_pBucketDiff1 = NULL;
    m_pBucketDiff2 = NULL;
}

moKinect::~moKinect() {
	Finish();
}

bool moKinect::CheckError() {
    #ifdef KINECT_OPENNI
    if (m_nRetVal != XN_STATUS_OK)
    {
        MODebug2->Error( moText("OpenNI Status Error:") + xnGetStatusString(m_nRetVal) );
        return false;
    }
    #endif
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
	p_configdefinition->Add( moText("offset_max"), MO_PARAM_FUNCTION, KINECT_OFFSET_MAX, moValue( "10000.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("offset_left"), MO_PARAM_FUNCTION, KINECT_OFFSET_LEFT, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("offset_right"), MO_PARAM_FUNCTION, KINECT_OFFSET_RIGHT, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("offset_top"), MO_PARAM_FUNCTION, KINECT_OFFSET_TOP, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("offset_bottom"), MO_PARAM_FUNCTION, KINECT_OFFSET_BOTTOM, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("offset_radius"), MO_PARAM_FUNCTION, KINECT_OFFSET_RADIUS, moValue( "1000.0", "FUNCTION").Ref() );

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

    p_configdefinition->Add( moText("umbral_objeto_z_near"), MO_PARAM_NUMERIC, KINECT_UMBRAL_OBJETO_Z_NEAR, moValue( "1300", "INT") );


    p_configdefinition->Add( moText("view_scalefactor"), MO_PARAM_FUNCTION, KINECT_VIEW_SCALEFACTOR, moValue( "0.7", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("view_fovy"), MO_PARAM_FUNCTION, KINECT_VIEW_FOVY, moValue( "45", "FUNCTION").Ref() );

/*
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
*/

    p_configdefinition->Add( moText("object_color1_bottom"), MO_PARAM_COLOR, KINECT_OBJECT_COLOR1_BOTTOM );
    p_configdefinition->Add( moText("object_color1_top"), MO_PARAM_COLOR, KINECT_OBJECT_COLOR1_TOP );

    p_configdefinition->Add( moText("object_color2_bottom"), MO_PARAM_COLOR, KINECT_OBJECT_COLOR2_BOTTOM );
    p_configdefinition->Add( moText("object_color2_top"), MO_PARAM_COLOR, KINECT_OBJECT_COLOR2_TOP );

    p_configdefinition->Add( moText("object_color3_bottom"), MO_PARAM_COLOR, KINECT_OBJECT_COLOR3_BOTTOM );
    p_configdefinition->Add( moText("object_color3_top"), MO_PARAM_COLOR, KINECT_OBJECT_COLOR3_TOP );

    p_configdefinition->Add( moText("object_min_surface"), MO_PARAM_NUMERIC, KINECT_OBJECT_MIN_SURFACE, moValue( "500", "FLOAT") );
    p_configdefinition->Add( moText("object_max_surface"), MO_PARAM_NUMERIC, KINECT_OBJECT_MAX_SURFACE, moValue( "2000", "FLOAT") );

    p_configdefinition->Add( moText("object_min_volume"), MO_PARAM_NUMERIC, KINECT_OBJECT_MIN_VOLUME, moValue( "500", "FLOAT") );
    p_configdefinition->Add( moText("object_max_volume"), MO_PARAM_NUMERIC, KINECT_OBJECT_MAX_VOLUME, moValue( "2000", "FLOAT") );


    p_configdefinition->Add( moText("texture1"), MO_PARAM_TEXTURE, KINECT_OBJECT_TEXTURE1, moValue( "default", MO_VALUE_TXT) );
    p_configdefinition->Add( moText("texture2"), MO_PARAM_TEXTURE, KINECT_OBJECT_TEXTURE1, moValue( "default", MO_VALUE_TXT) );
    p_configdefinition->Add( moText("texture3"), MO_PARAM_TEXTURE, KINECT_OBJECT_TEXTURE1, moValue( "default", MO_VALUE_TXT) );

    p_configdefinition->Add( moText("update_on"), MO_PARAM_NUMERIC, KINECT_UPDATE_ON, moValue( "0", "INT") );

	return p_configdefinition;
}


void moKinect::UpdateParameters() {

    m_Offset = moVector2f(
                            m_Config.Eval( moR(KINECT_OFFSET_MIN) ),
                            m_Config.Eval( moR(KINECT_OFFSET_MAX) )
                          );

    m_OffsetBox = moVector4f(
                            m_Config.Eval( moR(KINECT_OFFSET_LEFT) ),
                            m_Config.Eval( moR(KINECT_OFFSET_RIGHT) ),
                            m_Config.Eval( moR(KINECT_OFFSET_TOP) ),
                            m_Config.Eval( moR(KINECT_OFFSET_BOTTOM) )
                          );

    m_OffsetRadius = m_Config.Eval( moR(KINECT_OFFSET_RADIUS) );

    m_RadMin = m_Config.Eval( moR(KINECT_OBJECT_RAD_MIN));
    m_RadMax = m_Config.Eval( moR(KINECT_OBJECT_RAD_MAX));

    calibrate_base = m_Config.Int( moR(KINECT_CALIBRATE_BASE));

    m_DataLock.Lock();

    m_ReferencePoint = moVector3f(  m_Config.Eval( moR(KINECT_REF_POINT_X) ),
                                    m_Config.Eval( moR(KINECT_REF_POINT_Y) ),
                                    m_Config.Eval( moR(KINECT_REF_POINT_Z) )
                                  );

    m_ReferencePointDimension = moVector3f(  m_Config.Eval( moR(KINECT_REF_POINT_WIDTH) ),
                                    m_Config.Eval( moR(KINECT_REF_POINT_HEIGHT) ),
                                    m_Config.Eval( moR(KINECT_REF_POINT_DEEP) )
                                  );

        base_camera_i = m_Config.Int( moR(KINECT_BASE_CAMERA_I));
        base_camera_j = m_Config.Int( moR(KINECT_BASE_CAMERA_J));

        umbral_camera_presencia_y_offset = m_Config.Int( moR(KINECT_UMBRAL_CAMERA_PRESENCIA_Y_OFFSET));
        umbral_camera_presencia_z_near = m_Config.Int( moR(KINECT_UMBRAL_CAMERA_PRESENCIA_Z_NEAR));
        umbral_camera_presencia_z_far = m_Config.Int( moR(KINECT_UMBRAL_CAMERA_PRESENCIA_Z_FAR));


        umbral_trackeo_y_offset = m_Config.Int( moR(KINECT_UMBRAL_TRACKEO_Y_OFFSET));
        umbral_trackeo_z_near = m_Config.Int( moR(KINECT_UMBRAL_TRACKEO_Z_NEAR));
        umbral_trackeo_z_far = m_Config.Int( moR(KINECT_UMBRAL_TRACKEO_Z_FAR));
        trackeo_factor_angulo = m_Config.Eval( moR(KINECT_TRACKEO_FACTOR_ANGULO));

        umbral_objeto_z_near = m_Config.Int( moR(KINECT_UMBRAL_OBJETO_Z_NEAR ) );

        update_on = m_Config.Int( moR(KINECT_UPDATE_ON) );
/*
        moTexture Tex1( m_Config.Texture( moR(KINECT_OBJECT_TEXTURE1) ) );
        moTexture Tex2( m_Config.Texture( moR(KINECT_OBJECT_TEXTURE2) ) );
        moTexture Tex3(  m_Config.Texture( moR(KINECT_OBJECT_TEXTURE3) ) );


        if ( id_tex1!=Tex1.GetMOId() ) {

            id_tex1 = Tex1.GetMOId();

            if ( pDataObj1!=NULL) delete [] pDataObj1;

            pDataObj1 = new unsigned char [ Tex1.GetWidth()*Tex1.GetHeight()*3];

            if (pDataObj1)
                Tex1.GetBuffer( pDataObj1, GL_RGB, GL_UNSIGNED_BYTE );
        }
*/
        //m_pResourceManager->GetTextureMan()->GetTexture();

        m_Color1Bottom = m_Config.EvalColor( moR( KINECT_OBJECT_COLOR1_BOTTOM ) );
        m_Color1Top = m_Config.EvalColor( moR( KINECT_OBJECT_COLOR1_TOP ) );

        m_Color2Bottom = m_Config.EvalColor( moR( KINECT_OBJECT_COLOR2_BOTTOM ) );
        m_Color2Top = m_Config.EvalColor( moR( KINECT_OBJECT_COLOR2_TOP ) );

        m_Color3Bottom = m_Config.EvalColor( moR( KINECT_OBJECT_COLOR3_BOTTOM ) );
        m_Color3Top = m_Config.EvalColor( moR( KINECT_OBJECT_COLOR3_TOP ) );


        m_MinSurface = m_Config.Double( moR(KINECT_OBJECT_MIN_SURFACE ) );
        m_MaxSurface = m_Config.Double( moR(KINECT_OBJECT_MAX_SURFACE ) );

        m_MinVolume = m_Config.Double( moR(KINECT_OBJECT_MIN_VOLUME ) );
        m_MaxVolume = m_Config.Double( moR(KINECT_OBJECT_MAX_VOLUME ) );



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
	moDefineParamIndex( KINECT_OFFSET_LEFT, moText("offset_left") );
	moDefineParamIndex( KINECT_OFFSET_RIGHT, moText("offset_right") );
	moDefineParamIndex( KINECT_OFFSET_TOP, moText("offset_top") );
	moDefineParamIndex( KINECT_OFFSET_BOTTOM, moText("offset_bottom") );
	moDefineParamIndex( KINECT_OFFSET_RADIUS, moText("offset_radius") );


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

    moDefineParamIndex( KINECT_UMBRAL_OBJETO_Z_NEAR, moText("umbral_objeto_z_near") );

	moDefineParamIndex( KINECT_VIEW_SCALEFACTOR, moText("view_scalefactor") );
	moDefineParamIndex( KINECT_VIEW_FOVY, moText("view_fovy") );


    moDefineParamIndex( KINECT_OBJECT_COLOR1_TOP, moText("object_color1_top") );
    moDefineParamIndex( KINECT_OBJECT_COLOR1_BOTTOM, moText("object_color1_bottom") );

    moDefineParamIndex( KINECT_OBJECT_COLOR2_TOP, moText("object_color2_top") );
    moDefineParamIndex( KINECT_OBJECT_COLOR2_BOTTOM, moText("object_color2_bottom") );

    moDefineParamIndex( KINECT_OBJECT_COLOR3_TOP, moText("object_color3_top") );
    moDefineParamIndex( KINECT_OBJECT_COLOR3_BOTTOM, moText("object_color3_bottom") );


    moDefineParamIndex( KINECT_OBJECT_MIN_SURFACE, moText("object_min_surface") );
    moDefineParamIndex( KINECT_OBJECT_MAX_SURFACE, moText("object_max_surface") );

    moDefineParamIndex( KINECT_OBJECT_MIN_VOLUME, moText("object_min_volume") );
    moDefineParamIndex( KINECT_OBJECT_MAX_VOLUME, moText("object_max_volume") );


	moDefineParamIndex( KINECT_OBJECT_TEXTURE1, moText("texture1") );
    moDefineParamIndex( KINECT_OBJECT_TEXTURE2, moText("texture2") );
    moDefineParamIndex( KINECT_OBJECT_TEXTURE3, moText("texture3") );

    moDefineParamIndex( KINECT_UPDATE_ON, moText("update_on") );

/**
        falta algo?
*/
    moTexParam tparam = MODefTex2DParams;
    //tparam.internal_format = GL_RGBA32F_ARB;
    tparam.internal_format = GL_RGB;
/*
    m_OutputMode.nXRes = 640;
    m_OutputMode.nYRes = 480;*/
    m_OutputMode.nXRes = 640;
    m_OutputMode.nYRes = 480;
    m_OutputMode.nFPS = 30;

    UpdateParameters();


    int Mid = -1;


#ifdef KINECT_PCL
    /** RGB texture*/
    Mid = GetResourceManager()->GetTextureMan()->AddTexture( "KINECTRGB", m_OutputMode.nXRes, m_OutputMode.nYRes, tparam );
    if (Mid>0) {
        m_pRGBTexture = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
        m_pRGBTexture->BuildEmpty(m_OutputMode.nXRes, m_OutputMode.nYRes);
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

/*
        m_fbo_idx = m_pResourceManager->GetFBMan()->CreateFBO();
        pFBO = m_pResourceManager->GetFBMan()->GetFBO(m_fbo_idx);

        pFBO->AddTexture(   m_OutputMode.nXRes,
                            m_OutputMode.nYRes,
                            m_pDepthTexture->GetTexParam(),
                            m_pDepthTexture->GetGLId(),
                            attach_point );

        m_pDepthTexture->SetFBO( pFBO );
        m_pDepthTexture->SetFBOAttachPoint( attach_point );
*/
#endif



#ifdef KINECT_OPENNI

    m_nRetVal = m_Context.Init();
    if (!CheckError()) {
        MODebug2->Error("Couldn't initialize Kinect Context");
        return false;
    } else {
        MODebug2->Message("Kinect Context Initialized!!");
    }

/*
    m_nRetVal = m_UserGenerator.Create(m_Context);
    if (!CheckError()) {
        MODebug2->Error("Couldn't initialize Kinect User Generator");
        return false;
    } else {

        MODebug2->Message("Kinect User Generator Initialized!!");


        if (!m_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
        {
            MODebug2->Error( moText("Supplied user generator doesn't support skeleton") );
            return false;
        }


        m_nRetVal = m_UserGenerator.RegisterUserCallbacks( User_NewUser, User_LostUser, (void*)NULL, hUserCallbacks);
        //CHECK_RC(m_nRetVal, "Register to user callbacks");

        m_nRetVal = m_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart(UserCalibration_CalibrationStart, NULL, hCalibrationStart);
        //CHECK_RC(nRetVal, "Register to calibration start");

        m_nRetVal = m_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete(UserCalibration_CalibrationComplete, NULL, hCalibrationComplete);
        //CHECK_RC(m_nRetVal, "Register to calibration complete");

        if (m_UserGenerator.GetSkeletonCap().NeedPoseForCalibration())
        {
            m_bNeedPose = TRUE;
            if (!m_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
            {
                MODebug2->Error( moText("Pose required, but not supported"));
                return false;
            }
            m_nRetVal = m_UserGenerator.GetPoseDetectionCap().RegisterToPoseDetected(UserPose_PoseDetected, NULL, hPoseDetected);
            //CHECK_RC(nRetVal, "Register to Pose Detected");
            m_UserGenerator.GetSkeletonCap().GetCalibrationPose(m_strPose);
        }

        m_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

        m_nRetVal = m_UserGenerator.GetSkeletonCap().RegisterToCalibrationInProgress(MyCalibrationInProgress, NULL, hCalibrationInProgress);
        //CHECK_RC(nRetVal, "Register to calibration in progress");

        m_nRetVal = m_UserGenerator.GetPoseDetectionCap().RegisterToPoseInProgress(MyPoseInProgress, NULL, hPoseInProgress);
        //CHECK_RC(nRetVal, "Register to pose in progress");

    }
*/
    /** Create a DepthGenerator node */

    m_nRetVal = m_Depth.Create(m_Context);
    if (!CheckError()) {

        MODebug2->Error("Couldn't create DepthGenerator");
        return false;

    } else {

        MODebug2->Message("Kinect DepthGenerator Created!!");

        m_DepthPixel = m_Depth.GetDeviceMaxDepth();

        MODebug2->Message( moText("m_DepthPixel:") + IntToStr(m_DepthPixel) );

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

            MODebug2->Message("KINECTDEPTH texture created!!");

            m_pDepthTexture->BuildEmpty(m_OutputMode.nXRes, m_OutputMode.nYRes);

/*
            Mid = GetResourceManager()->GetTextureMan()->AddTexture( "KINECTDEPTH2", m_OutputMode.nXRes, m_OutputMode.nYRes, tparam );
            if (Mid>0) {
                m_pDepthTexture2 = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
            }

            pCloud = new moVector3f [m_OutputMode.nXRes*m_OutputMode.nYRes];
            pCloudDif = new moVector3f [m_OutputMode.nXRes*m_OutputMode.nYRes];
*/
/*
            m_fbo_idx = m_pResourceManager->GetFBMan()->CreateFBO();
			pFBO = m_pResourceManager->GetFBMan()->GetFBO(m_fbo_idx);

			pFBO->AddTexture(   m_OutputMode.nXRes,
                                m_OutputMode.nYRes,
                                m_pDepthTexture->GetTexParam(),
                                m_pDepthTexture->GetGLId(),
                                attach_point );

            m_pDepthTexture->SetFBO( pFBO );
            m_pDepthTexture->SetFBOAttachPoint( attach_point );
*/

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

        MODebug2->Message("Kinect ImageGenerator Created!!");

        /** WE CREATE THE 640x480 TEXTURE*/
        moTexParam tparam = MODefTex2DParams;
        //tparam.internal_format = GL_RGBA32F_ARB;
        tparam.internal_format = GL_RGB;
        int Mid = -1;

        /** RGB texture*/
        Mid = GetResourceManager()->GetTextureMan()->AddTexture( "KINECTRGB", m_OutputMode.nXRes, m_OutputMode.nYRes, tparam );
        if (Mid>0) {
            m_pRGBTexture = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
            m_pRGBTexture->BuildEmpty(m_OutputMode.nXRes, m_OutputMode.nYRes);

            MODebug2->Message("KINECTRGB texture created!!");
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
        MODebug2->Error("Couldn't start all generators");
        return false;
    } else {
        MODebug2->Message("Kinect All Generators Started!!");
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

#ifdef KINECT_PCL
    if (update_on<1) {

        if (interface  ) interface->stop();
        return;
    } else {
        if (interface ) interface->start();
    }
#endif



#ifdef KINECT_OPENNI

if (update_on>0) {


    /** USER GENERATOR */

    //m_UserGenerator
    /*
    if (m_UserGenerator) {
        m_nRetVal = m_Context.WaitOneUpdateAll(m_UserGenerator);
        if (!CheckError()) {
            MODebug2->Error("Kinect Failed to update data.");
        } else {

            /// TODO: hahcer algo aqui (dibujar los esqueletos ? )
        }
    }
    */

    /** UPDATE IMAGE*/
    if (m_RGBImage.GetRGB24ImageMap()) {
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
                                if (i==m_OutputMode.nXRes/2 && j==m_OutputMode.nYRes/2) {
                                    //MODebug2->Push( IntToStr((*pImageMap).nRed) + moText(" > ") + IntToStr(pImage[i*3+j*3*m_OutputMode.nXRes]) );
                                }
                                pImageMap++;
                            }
                    }

                    if (m_pRGBTexture) {
                        //MODebug2->Push("Building");
                        m_pRGBTexture->BuildFromBuffer( m_OutputMode.nXRes, m_OutputMode.nYRes, pImage, GL_RGB, GL_UNSIGNED_BYTE );
                    }
                }
            }

        }
    }

    /** UPDATE DEPTH*/
    if (m_Depth.GetDepthMap()) {
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
                /*
                if (!pData2) {
                    pData2 =  new unsigned char [ m_OutputMode.nXRes * m_OutputMode.nYRes * 3 ];

                }
    */

                float x,y,z;

    /*
                int s = 0;

                int disoff = 0.0;

                disoff = (int)(m_Offset.Y() - m_Offset.X());

                float distance_r = 0.0;

                int n_base_max = 9;
                float minDistance = -10;

                int ical,jcal;
                int indice_base_h[n_base_max*2];
                int indice_base_v[n_base_max*2];
                int n_base_h;
                int n_base_v;

                n_base_h = 0;
                n_base_v = 0;
                ical = 0;
                jcal = 0;
                if (disoff==0) disoff=1.0;
    */
                if (pData /*&& pData2*/ ) {
                    for( int j=0; j<m_OutputMode.nYRes; j++ ) {
                        for( int i=0; i<m_OutputMode.nXRes; i++ ) {


                            //pData[i*3+j*m_OutputMode.nXRes*3] = *pDepthMap % 255;
                            //pData[i*3+1+j*m_OutputMode.nXRes*3] = *pDepthMap % 255;
                            //pData[i*3+2+j*m_OutputMode.nXRes*3] = *pDepthMap % 255;
                            z = (float)(*pDepthMap);

                            float radi = 0.0;

                            if (

                                    m_Offset.X()<(*pDepthMap)
                                && (*pDepthMap)<m_Offset.Y()

                                && (int)(m_OffsetBox.X())<=i
                                && i<=(int)(m_OffsetBox.Y())
                                && (int)(m_OffsetBox.Z())<=j
                                && j<=(int)(m_OffsetBox.W())

                                && radi<=m_OffsetRadius

                                ) {

                                pData[i*3+j*m_OutputMode.nXRes*3] = (*pDepthMap - (int)m_Offset.X() );
                                pData[i*3+j*m_OutputMode.nXRes*3] = ( pData[i*3+j*m_OutputMode.nXRes*3] >> 1 ) % 255;
                                //pData[i*3+1+j*m_OutputMode.nXRes*3] = (*pDepthMap - disoff) % 255;
                                //pData[i*3+2+j*m_OutputMode.nXRes*3] = (*pDepthMap - disoff) % 255;
                                pData[i*3+1+j*m_OutputMode.nXRes*3] = pData[i*3+j*m_OutputMode.nXRes*3];
                                pData[i*3+2+j*m_OutputMode.nXRes*3] = pData[i*3+j*m_OutputMode.nXRes*3];
    /*
                                pData2[i*3+j*m_OutputMode.nXRes*3] = 0;
                                pData2[i*3+1+j*m_OutputMode.nXRes*3] = 0;
                                pData2[i*3+2+j*m_OutputMode.nXRes*3] = 0;
    */

    /*
                                float z2 = ( (float) z * scaleFactor );
    */
                                //x = (i - 320) * (z2 + minDistance) * scaleFactor;
                                //y = (240 - j) * (z2 + minDistance) * scaleFactor;
    /*
                                x = ( i - m_OutputMode.nXRes/2 ) * z2 / (m_OutputMode.nXRes/2);
                                y = ( m_OutputMode.nYRes/2 - j ) * z2 / (m_OutputMode.nYRes/2);
    */
    /*

                                if (pCloud && pCloudDif) {

                                    pCloud[i+j*m_OutputMode.nXRes] = moVector3f( x,y,z );

                                //difference with Reference
                                    pCloudDif[i+j*m_OutputMode.nXRes] = pCloud[i+j*m_OutputMode.nXRes] - m_ReferencePoint;

                                    distance_r = pCloudDif[i+j*m_OutputMode.nXRes].Length();
    */
                                    /*

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
                                    */

                                    /*si ya sacamos una normal*/
                                    /*
                                    if (BaseNormal.Length() > 0.01) {
                                        //calculamos los puntos que estan sobre la base
                                        moVector3f ResV = pCloud[i+j*m_OutputMode.nXRes] - m_ReferencePoint;
                                        float res = BaseNormal.Dot( ResV );
                                        //si ya tenemos un resultado del escalar, si enegativo esta por encima...
                                        if ( res < 0.1 ) {
                                             pData[i*3+j*m_OutputMode.nXRes*3] = 0;
                                             pData[i*3+1+j*m_OutputMode.nXRes*3] = (*pDepthMap - disoff) % 255;
                                             pData[i*3+2+j*m_OutputMode.nXRes*3] = 0;

                                            pData2[i*3+j*m_OutputMode.nXRes*3] = (*pDepthMap - disoff) % 255;
                                            pData2[i*3+1+j*m_OutputMode.nXRes*3] = (*pDepthMap - disoff) % 255;
                                            pData2[i*3+2+j*m_OutputMode.nXRes*3] = (*pDepthMap - disoff) % 255;
                                        }

                                    }
                                    */

                                    /*
                                    if ( i>=(m_OutputMode.nXRes/2-1) && i<=(m_OutputMode.nXRes/2+1) && j>=(m_OutputMode.nYRes/2-1) && j<=(m_OutputMode.nYRes/2+1)) {

                                        pData[i*3+j*m_OutputMode.nXRes*3] = 0;
                                        pData[i*3+1+j*m_OutputMode.nXRes*3] = 255;
                                        pData[i*3+2+j*m_OutputMode.nXRes*3] = 0;
                                        if (i== m_OutputMode.nXRes/2 && j== m_OutputMode.nYRes/2) {
                                            //
                                            //MODebug2->Push( IntToStr(*pDepthMap) + moText("(0,0) <x> ") + FloatToStr(x)
                                            //           + moText(" <y> ") + FloatToStr(y)
                                            //           + moText(" <z> ") + FloatToStr(z)
                                            //           + moText(" <d> ") + FloatToStr(distance_r) );

                                        }
                                    }

                                    if (i>=69 && i<=71 && j>=69 && j<=71) {
                                        pData[i*3+j*m_OutputMode.nXRes*3] = 0;
                                        pData[i*3+1+j*m_OutputMode.nXRes*3] = 255;
                                        pData[i*3+2+j*m_OutputMode.nXRes*3] = 255;
                                        if (i==70 && j==70) {

                                            //MODebug2->Push( IntToStr(*pDepthMap) + moText("(70,70) <x> ") + FloatToStr(x)
                                              //         + moText(" <y> ") + FloatToStr(y)
                                                //       + moText(" <z> ") + FloatToStr(z)
                                                  //     + moText(" <d> ") + FloatToStr(distance_r) );
                                         }
                                    }
                                    */
        /*
                                } //fin analisis
    */
                            } else {

                                pData[i*3+j*m_OutputMode.nXRes*3] = 0;
                                pData[i*3+1+j*m_OutputMode.nXRes*3] = 0;
                                pData[i*3+2+j*m_OutputMode.nXRes*3] = 0;
    /*
                                pData2[i*3+j*m_OutputMode.nXRes*3] = 0;
                                pData2[i*3+1+j*m_OutputMode.nXRes*3] = 0;
                                pData2[i*3+2+j*m_OutputMode.nXRes*3] = 0;
    */
                            }


                            pDepthMap++;
                        }
                    }
                    /*CALCULAMOS LA NORMAL DE LA BASE*/
                    /*
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
                    */

                    if (m_pDepthTexture) {
                        m_pDepthTexture->BuildFromBuffer( m_OutputMode.nXRes, m_OutputMode.nYRes, pData, GL_RGB, GL_UNSIGNED_BYTE );
                    }
    /*
                    if (m_pDepthTexture2) {
                        m_pDepthTexture2->BuildFromBuffer( m_OutputMode.nXRes, m_OutputMode.nYRes, pData2, GL_RGB, GL_UNSIGNED_BYTE );
                    }
    */

                }
            }
        }
    }

            m_pSrcTexture = m_pDepthTexture;

            if (m_pSrcTexture) {
                //crea textura que recibe frame0 desde el SrcTexture

                //crea textura que recibe frame1

                //crea textura que recibe frame_diff
                int idt = -1;

                ///creamos la textura de destino de la copia:
                /// y su shader asociado
                if (!m_pTFDest2Texture && m_pSrcTexture->GetWidth()>0 ) {
                        moTextArray copy_filter_0;
                        copy_filter_0.Add( moText("KINECTDEPTH shaders/Copy.cfg res:64x64 kinectdepth0dest2") );
                        int idx = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->LoadFilters( &copy_filter_0 );
                        if (idx>-1) {
                            m_pTFDest2Texture = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->Get(idx-1);
                            MODebug2->Push( moText("filter loaded m_pTFDest2Texture") );
                        }
                }


                ///creamos la textura de destino de la copia del cuadro anterior
                /// y su shader asociado
                if (!m_pTFDest1Texture && m_pSrcTexture->GetWidth()>0 ) {
                    moTextArray copy_filter_0;
                    copy_filter_0.Add( moText("kinectdepth0dest2 shaders/Copy.cfg res:64x64 kinectdepth0dest1") );
                    int idx = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->LoadFilters( &copy_filter_0 );
                    if (idx>-1) {
                        m_pTFDest1Texture = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->Get(idx-1);
                        MODebug2->Push( moText("filter loaded m_pTFDest1Texture") );
                    } else MODebug2->Push( moText("filter NOT loaded m_pTFDest1Texture") );
                }


                ///creamos la textura de destino de la copia del cuadro anterior
                /// y su shader asociado
                if (!m_pTFDest0Texture && m_pSrcTexture->GetWidth()>0 ) {
                    moTextArray copy_filter_0;
                    copy_filter_0.Add( moText("kinectdepth0dest1 shaders/Copy.cfg res:64x64 kinectdepth0dest0") );
                    int idx = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->LoadFilters( &copy_filter_0 );
                    if (idx>-1) {
                        m_pTFDest0Texture = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->Get(idx-1);
                        MODebug2->Push( moText("filter loaded m_pTFDest0Texture") );
                    } else MODebug2->Push( moText("filter NOT loaded m_pTFDest0Texture") );
                }


                ///creamos la textura final, que contendra la diferencia entre el cuadro actual y el anterior..
                /// y su shader asociado
                if (!m_pTFDestDiff2Texture && m_pSrcTexture->GetWidth()>0 ) {
                    moTextArray copy_filter_0;
                    copy_filter_0.Add( moText("kinectdepth0dest1 kinectdepth0dest2 shaders/Diff.cfg res:64x64 kinectdepth0diff2") );
                    int idx = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->LoadFilters( &copy_filter_0 );
                    if (idx>-1) {
                        m_pTFDestDiff2Texture = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->Get(idx-1);
                        if (m_pTFDestDiff2Texture) {
                            m_pDestDiff2Texture = m_pTFDestDiff2Texture->GetDestTex()->GetTexture(0);
                            MODebug2->Push( moText("filter loaded m_pTFDestDiff2Texture") );
                        } else MODebug2->Push( moText("filter NOT loaded m_pTFDestDiff2Texture") );
                    }
                }

                ///creamos la textura final, que contendra la diferencia entre el cuadro anterior y el ante-ultimo..
                /// y su shader asociado
                if (!m_pTFDestDiff1Texture && m_pSrcTexture->GetWidth()>0 ) {
                    moTextArray copy_filter_0;
                    copy_filter_0.Add( moText("kinectdepth0dest0 kinectdepth0dest1 shaders/Diff.cfg res:64x64 kinectdepth0diff1") );
                    int idx = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->LoadFilters( &copy_filter_0 );
                    if (idx>-1) {
                        m_pTFDestDiff1Texture = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->Get(idx-1);
                        if (m_pTFDestDiff1Texture) {
                            m_pDestDiff1Texture = m_pTFDestDiff1Texture->GetDestTex()->GetTexture(0);
                            MODebug2->Push( moText("filter loaded m_pTFDestDiff1Texture") );
                        }
                    }
                }


/*

                pFBO->AddTexture(   160,
                                    120,
                                    m_pSrcTexture->GetTexParam(),
                                    m_pSrcTexture->GetGLId(),
                                    attach_point );

                m_pSrcTexture->SetFBO( pFBO );
                m_pSrcTexture->SetFBOAttachPoint( attach_point );
*/
                ///Guarda el frame ante-anterior....
                if (m_pTFDest0Texture) {
                    m_pTFDest0Texture->Apply( (GLuint)0 );
                }


                ///Guarda el frame anterior....
                if (m_pTFDest1Texture) {
                    m_pTFDest1Texture->Apply( (GLuint)0 );
                }


                ///Guarda el frame actual.... (esto lo hacemos ultimo para no perder los anteriores.
                if (m_pTFDest2Texture) {
                    m_pTFDest2Texture->Apply( (GLuint)0 );
                }

                ///Calcular la diferencia del frame anterior con el anterior a este
                if (m_pTFDestDiff1Texture) {
                    m_pTFDestDiff1Texture->Apply( (GLuint)0 );
                }

                ///Calcular la diferencia con el frame anterior
                if (m_pTFDestDiff2Texture) {
                    m_pTFDestDiff2Texture->Apply( (GLuint)0 );
                }


                if (m_pDestDiff2Texture) {

                    /// BAJAR A BUFFER Y PASAR A ARRAY DE TRACKING FEATURES.... (BLOBS CON OPENCV ?)

                        if (!m_pBucketDiff2) {
                            MODebug2->Push("moOpenCV::UpdateParameters > moBucket Object created to receive TextureFilters Data.");
                            m_pBucketDiff2 = new moBucket();
                        }

                        if (m_pBucketDiff2) {
                            ///ATENCION!!! al hacer un BuildBucket, hay que hacer un EmptyBucket!!!
                            //pBucket->BuildBucket( pTexSample->m_VideoFormat.m_BufferSize, 0);
                            m_pBucketDiff2->BuildBucket( m_pDestDiff2Texture->GetWidth()*m_pDestDiff2Texture->GetHeight()*4, 0);
                            m_pDestDiff2Texture->GetBuffer( (void*) m_pBucketDiff2->GetBuffer(), GL_RGBA, GL_UNSIGNED_BYTE );

                            ///asignamos el bucket al videosample
                            //pTexSample->m_pSampleBuffer = pBucket;
                        }

                        if (!m_pTrackerSystemData) {
                            MODebug2->Push("moKinectGPU::UpdateParameters > creating moTrackerSystemData() Object.");
                            m_pTrackerSystemData = new moTrackerSystemData();
                            if (m_pTrackerSystemData) {
                                MODebug2->Push( "moKinectGPU::UpdateParameters > moTrackerSystemData() Object OK.");
                                MODebug2->Push( moText("moKinectGPU::UpdateParameters > m_pDestDiff2Texture->GetWidth().")
                                               + IntToStr( m_pDestDiff2Texture->GetWidth() )
                                               + moText(" m_pDestDiff2Texture->GetHeight().")
                                               + IntToStr( m_pDestDiff2Texture->GetHeight() )
                                                );

                            }
                        }

                        if (m_pTrackerSystemData) {

                            m_pTrackerSystemData->GetVideoFormat().m_Width = m_pDestDiff2Texture->GetWidth();
                            m_pTrackerSystemData->GetVideoFormat().m_Height = m_pDestDiff2Texture->GetHeight();
                            m_pTrackerSystemData->GetVideoFormat().m_BufferSize = m_pDestDiff2Texture->GetWidth()*m_pDestDiff2Texture->GetHeight();

                            for(int i=0; i<m_pTrackerSystemData->GetFeatures().Count(); i++ ) {
                                if (m_pTrackerSystemData->GetFeatures().GetRef(i)!=NULL)
                                delete m_pTrackerSystemData->GetFeatures().GetRef(i);
                            }

                            ///RESET DATA !!!!
                            m_pTrackerSystemData->GetFeatures().Empty();
                            m_pTrackerSystemData->ResetMatrix();

                            ///GET NEW DATA!!!!
                            moTrackerFeature* TF = NULL;
                            MOubyte* pBuf = NULL;

                            if (m_pBucketDiff2)
                                pBuf = (MOubyte*) m_pBucketDiff2->GetBuffer();

                            int validfeatures = 0;

                            float sumX = 0.0f,sumY = 0.0f;
                            float sumN = 0.0f;
                            float varX = 0.0f, varY = 0.0f;
                            float minX = 1.0f, minY = 1.0;
                            float maxX = 0.0f, maxY = 0.0;

                            float vel=0.0,acc=0.0,tor=0.0;
                            float velAverage = 0.0, accAverage =0.0, torAverage=0.0;
                            moVector2f velAverage_v(0,0);

                            for(int j=0; j < m_pTrackerSystemData->GetVideoFormat().m_Height && pBuf; j++ ) {
                                for(int i=0; i < m_pTrackerSystemData->GetVideoFormat().m_Width && pBuf; i++ ) {
                                    if (pBuf) {

                                        int idx = i+j*m_pTrackerSystemData->GetVideoFormat().m_Width;

                                        if ( 0<=idx && idx < m_pBucketDiff2->GetSize()  ) {
                                            int r = pBuf[idx*4];
                                            //int g = pBuf[idx*4+1];
                                            //int b = pBuf[idx*4+2];
                                            //int a = pBuf[idx*4+3];
                                            if ( r > 0) {
                                                //crear el feature aquí....
                                                TF = new moTrackerFeature();
                                                if (TF) {
                                                    TF->x = (float) i / (float)m_pTrackerSystemData->GetVideoFormat().m_Width;
                                                    TF->y =  (float) j / (float)m_pTrackerSystemData->GetVideoFormat().m_Height;
                                                    TF->val = 0;
                                                    TF->valid = 1;
                                                    TF->tr_x = TF->x;
                                                    TF->tr_y = TF->y;

                                                    ///CALCULATE AVERAGE FOR BARYCENTER AND VARIANCE
                                                    sumX+= TF->x;
                                                    sumY+= TF->y;

                                                    sumN+= 1.0f;

                                                    if (sumN==1.0f) {
                                                        /*
                                                        MODebug2->Push( moText("moOpenCV::UpdateParameters > TF > TF->x:")
                                                                    + FloatToStr(TF->x)
                                                                    + moText(" TF->y:")
                                                                    + FloatToStr(TF->y)
                                                                   );
                                                        */
                                                    }

                                                    ///maximos
                                                    if (TF->x>maxX) maxX = TF->x;
                                                    if (TF->y>maxY) maxY = TF->y;

                                                    ///minimos
                                                    if (TF->x<minX) minX = TF->x;
                                                    if (TF->y<minY) minY = TF->y;

                                                    ///esta es simplemente una matriz que cuenta la cantidad de....
                                                    m_pTrackerSystemData->SetPositionMatrix( TF->x, TF->y, 1 );
                                                    ///genera la matrix de referencia rapida por zonas
                                                    ///m_pTrackerSystemData->SetPositionMatrix( TF );

                                                    ///CALCULATE VELOCITY AND ACCELERATION
                                                    TF->ap_x = TF->a_x;
                                                    TF->ap_y = TF->a_x;
                                                    TF->vp_x = TF->v_x;
                                                    TF->vp_y = TF->v_x;
                                                    TF->v_x = TF->x - TF->tr_x;
                                                    TF->v_y = TF->y - TF->tr_y;
                                                    TF->a_x = TF->v_x - TF->vp_x;
                                                    TF->a_y = TF->v_y - TF->vp_y;
                                                    TF->t_x = TF->a_x - TF->ap_x;
                                                    TF->t_y = TF->a_y - TF->ap_y;

                                                    vel = moVector2f( TF->v_x, TF->v_y ).Length();
                                                    acc = moVector2f( TF->a_x, TF->a_y ).Length();
                                                    tor = moVector2f( TF->t_x, TF->t_y ).Length();
                                                    velAverage+= vel;
                                                    accAverage+= acc;
                                                    torAverage+= tor;
                                                    velAverage_v+= moVector2f( fabs(TF->v_x), fabs(TF->v_y) );


                                                    if ( vel >= 0.001 && vel <=0.05 ) m_pTrackerSystemData->SetMotionMatrix( TF->x, TF->y, 1 );
                                                    if ( acc >= 0.001 ) m_pTrackerSystemData->SetAccelerationMatrix( TF->x, TF->y, 1 );

                                                }
                                                m_pTrackerSystemData->GetFeatures().Add(TF);
                                            }
                                        }
                                    }
                                }
                            }


                            moVector2f previous_B = m_pTrackerSystemData->GetBarycenter();
                            moVector2f previous_BM = m_pTrackerSystemData->GetBarycenterMotion();

                            moVector2f BarPos;
                            moVector2f BarMot;
                            moVector2f BarAcc;

                            m_pTrackerSystemData->SetBarycenter( 0, 0 );
                            m_pTrackerSystemData->SetBarycenterMotion( 0, 0);
                            m_pTrackerSystemData->SetBarycenterAcceleration( 0, 0 );

                            m_pTrackerSystemData->SetMax( 0, 0 );
                            m_pTrackerSystemData->SetMin( 0, 0 );

                            m_pTrackerSystemData->SetDeltaValidFeatures( m_pTrackerSystemData->GetValidFeatures() - (int)sumN );
                            m_pTrackerSystemData->SetValidFeatures( (int)sumN );

                            if (sumN>=1.0f) {

                                BarPos = moVector2f( sumX/sumN, sumY/sumN);

                                BarMot = BarPos - previous_B;
                                BarAcc = BarMot - previous_BM;

                                m_pTrackerSystemData->SetBarycenter( BarPos.X(), BarPos.Y() );
                                m_pTrackerSystemData->SetBarycenterMotion( BarMot.X(), BarMot.Y() );
                                m_pTrackerSystemData->SetBarycenterAcceleration( BarAcc.X(), BarAcc.Y() );

                                velAverage = velAverage / (float)sumN;
                                accAverage = accAverage / (float)sumN;
                                torAverage = torAverage / (float)sumN;
                                velAverage_v = velAverage_v * 1.0f / (float)sumN;

                                m_pTrackerSystemData->SetAbsoluteSpeedAverage( velAverage );
                                m_pTrackerSystemData->SetAbsoluteAccelerationAverage( accAverage );
                                m_pTrackerSystemData->SetAbsoluteTorqueAverage( torAverage );

                                m_pTrackerSystemData->SetMax( maxX, maxY );
                                m_pTrackerSystemData->SetMin( minX, minY );

                                ///CALCULATE VARIANCE FOR EACH COMPONENT

                                moVector2f Bar = m_pTrackerSystemData->GetBarycenter();
                                for(int i=0; i<m_pTrackerSystemData->GetFeatures().Count(); i++ ) {
                                    TF = m_pTrackerSystemData->GetFeatures().GetRef(i);
                                    if (TF) {
                                        if (TF->val>=0) {
                                            varX+= moMathf::Sqr( TF->x - Bar.X() );
                                            varY+= moMathf::Sqr( TF->y - Bar.Y() );
                                        }
                                    }
                                }
                                m_pTrackerSystemData->SetVariance( varX/sumN, varY/sumN );
                                //m_pTrackerSystemData->SetVariance( velAverage_v.X(), velAverage_v.Y() );

                                /*
                                MODebug2->Push( moText("TrackerKLT: varX: ") + FloatToStr( m_pTrackerSystemData->GetVariance().X())
                                               + moText(" varY: ") + FloatToStr(m_pTrackerSystemData->GetVariance().Y()) );
                                               */

                                ///CALCULATE CIRCULAR MATRIX
                                for(int i=0; i<m_pTrackerSystemData->GetFeatures().Count(); i++ ) {
                                    TF = m_pTrackerSystemData->GetFeatures().GetRef(i);
                                    if (TF) {
                                        if (TF->val>=0) {
                                            m_pTrackerSystemData->SetPositionMatrixC( TF->x, TF->y, 1 );
                                            vel = moVector2f( TF->v_x, TF->v_y ).Length();
                                            //acc = moVector2f( TF->a_x, TF->a_y ).Length();
                                            if (vel>=0.01) m_pTrackerSystemData->SetMotionMatrixC( TF->x, TF->y, 1 );
                                        }
                                    }
                                }
                                /*

                                MODebug2->Push( moText("moOpenCV::UpdateParameters > ValidFeatures: ")
                                                                + FloatToStr(sumN)
                                                                + moText(" GetValidFeatures:")
                                                                + IntToStr( m_pTrackerSystemData->GetValidFeatures() )
                                                               );*/



                            }


                        }

                        if (m_Outlets.Count()>=1 && m_Outlets[0] && m_pTrackerSystemData) {
                            m_Outlets[0]->GetData()->SetPointer( (MOpointer) m_pTrackerSystemData, sizeof(moTrackerSystemData) );
                            m_Outlets[0]->Update(); ///to notify Inlets!!
                        }
                        if (m_pBucketDiff2) {
                            m_pBucketDiff2->EmptyBucket();
                        }


                }

            }


    /** Draw some 3d Stuff */

    if (pFBO && m_pDepthTexture) {
/*
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
                            //m_Config[moR(PARTICLES_VIEWX)].GetData()->Fun()->Eval(m_EffectState.tempo.ang)
                            0.0,
                            //m_Config[moR(PARTICLES_VIEWY)].GetData()->Fun()->Eval(m_EffectState.tempo.ang)
                            0.0,
                            //m_Config[moR(PARTICLES_VIEWZ)].GetData()->Fun()->Eval(m_EffectState.tempo.ang)
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

        //PRINTING ADDITIONAL
        m_pResourceManager->GetGLMan()->SetOrthographicView( m_pDepthTexture->GetWidth(), m_pDepthTexture->GetHeight() );

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        ancho = 80;
        alto = 50;
        glTranslatef( base_camera_i, base_camera_j, 0 );
        glColor4f( 0,0.5,0.0,1 );
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

        ancho = 10;
        alto = 10;
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
*/
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
} //FIN updateon < 1
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
        moVector3d HSV = RGBtoHSV( (double)m_BaseRGB.X(), (double)m_BaseRGB.Y(), (double)m_BaseRGB.Z() );
        m_BaseHSV = moVector3f( HSV.X(),HSV.Y(),HSV.Z() );
        moText texto;
        texto= moText("BasePosition: x:")+FloatToStr(m_BasePosition.X());
        texto+= moText(" y:")+FloatToStr(m_BasePosition.Y());
        texto+= moText(" z:")+FloatToStr(m_BasePosition.Z());
        texto+= moText(" Normal: x:")+FloatToStr( m_BaseNormal.normal[0] );
        texto+= moText(" y:")+FloatToStr(m_BaseNormal.normal[1]);
        texto+= moText(" z:")+FloatToStr(m_BaseNormal.normal[2]);
        texto+= moText(" RGB: r:")+FloatToStr( m_BaseRGB.X() );
        texto+= moText(" g:")+FloatToStr( m_BaseRGB.Y() );
        texto+= moText(" b:")+FloatToStr( m_BaseRGB.Z() );
        texto+= moText(" HSV: h:")+FloatToStr( m_BaseHSV.X() );
        texto+= moText(" s:")+FloatToStr( m_BaseHSV.Y() );
        texto+= moText(" v:")+FloatToStr( m_BaseHSV.Z() );
        MODebug2->Push(texto);
    }



    //m_CenterNormal.curvature = cloud_normals->points[ (_ww  >> 1) * (_hh + 1) ].curvature;

    moText texto;
    static unsigned int cc = 0;

    int curvature_index = this->GetOutletIndex( moText("CURVATURE") );
    moOutlet* outCurvature = m_Outlets.GetRef( curvature_index );

    if (outCurvature) {

        if ( m_TargetPosition.Z() < 1.28 ) m_CenterNormal.curvature = -1;

        outCurvature->GetData()->SetLong( 100 * m_CenterNormal.curvature );
        outCurvature->Update(true);
    }

    int r_index = this->GetOutletIndex( moText("R") );
    int g_index = this->GetOutletIndex( moText("G") );
    int b_index = this->GetOutletIndex( moText("B") );

    moOutlet* outR = m_Outlets.GetRef( r_index );
    moOutlet* outG = m_Outlets.GetRef( g_index );
    moOutlet* outB = m_Outlets.GetRef( b_index );

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

    int NDIF1_index = this->GetOutletIndex( moText("NDIF1") );
    int NDIF2_index = this->GetOutletIndex( moText("NDIF2") );
    int NDIF3_index = this->GetOutletIndex( moText("NDIF3") );

    int SURFACE_index = this->GetOutletIndex( moText("SURFACE") );

    int SURFACE1_index = this->GetOutletIndex( moText("SURFACE1") );
    int SURFACE2_index = this->GetOutletIndex( moText("SURFACE2") );
    int SURFACE3_index = this->GetOutletIndex( moText("SURFACE3") );

    moOutlet* outNDIF1 = m_Outlets.GetRef( NDIF1_index );
    moOutlet* outNDIF2 = m_Outlets.GetRef( NDIF2_index );
    moOutlet* outNDIF3 = m_Outlets.GetRef( NDIF3_index );

    moOutlet* outSURFACE = m_Outlets.GetRef( SURFACE_index );

    moOutlet* outSURFACE1 = m_Outlets.GetRef( SURFACE1_index );
    moOutlet* outSURFACE2 = m_Outlets.GetRef( SURFACE2_index );
    moOutlet* outSURFACE3 = m_Outlets.GetRef( SURFACE3_index );
////
    if (outNDIF1) {
        outNDIF1->GetData()->SetInt(NDIF1);
        outNDIF1->Update();
    }
    if (outNDIF2) {
        outNDIF2->GetData()->SetInt(NDIF2);
        outNDIF2->Update();
    }
    if (outNDIF3) {
        outNDIF3->GetData()->SetInt(NDIF3);
        outNDIF3->Update();
    }
////
    if (outSURFACE) {
        outSURFACE->GetData()->SetInt(SURFACE);
        outSURFACE->Update();
    }
////
    if (outSURFACE1) {
        outSURFACE1->GetData()->SetInt(SURFACE1);
        outSURFACE1->Update();
    }
    if (outSURFACE2) {
        outSURFACE2->GetData()->SetInt(SURFACE2);
        outSURFACE2->Update();
    }
    if (outSURFACE3) {
        outSURFACE3->GetData()->SetInt(SURFACE3);
        outSURFACE3->Update();
    }
/*
    int pomelo_index = this->GetOutletIndex( moText("POMELO") );
    int naranja_index = this->GetOutletIndex( moText("NARANJA") );
    int sandia_index = this->GetOutletIndex( moText("SANDIA") );

    moOutlet* outPOMELO = m_Outlets.GetRef( pomelo_index );
    moOutlet* outNARANJA = m_Outlets.GetRef( naranja_index );
    moOutlet* outSANDIA = m_Outlets.GetRef( sandia_index );

    if (outPOMELO) {
        outPOMELO->GetData()->SetFloat( dif_tex1 );
        outPOMELO->Update(true);
        if (dif_tex1>-1) {
            //MODebug2->Push( moText("dif_tex1:") + FloatToStr( dif_tex1 ) );
        }
    }
    if (outNARANJA) {
        outNARANJA->GetData()->SetFloat( dif_tex2 );
        outNARANJA->Update(true);
    }
    if (outSANDIA) {
        outSANDIA->GetData()->SetFloat( dif_tex3 );
        outSANDIA->Update(true);
    }
*/


    moVector3d HSV = RGBtoHSV( m_TargetRGB.X(), m_TargetRGB.Y(), m_TargetRGB.Z() );

    m_TargetHSV = moVector3f( HSV.X(), HSV.Y(), HSV.Z() );

    int h_index = this->GetOutletIndex( moText("H") );
    int s_index = this->GetOutletIndex( moText("S") );
    int v_index = this->GetOutletIndex( moText("V") );

    moOutlet* outH = m_Outlets.GetRef( h_index );
    moOutlet* outS = m_Outlets.GetRef( s_index );
    moOutlet* outV = m_Outlets.GetRef( v_index );

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
    moOutlet* outHayPresencia = m_Outlets.GetRef( hay_presencia_index );
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
            //m_pRGBTexture->BuildFromBuffer( m_OutputMode.nXRes, m_OutputMode.nYRes, pImage, GL_RGB, GL_UNSIGNED_BYTE );
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
                            //m_Config[moR(PARTICLES_VIEWX)].GetData()->Fun()->Eval(m_EffectState.tempo.ang)
                            0.0,
                            //m_Config[moR(PARTICLES_VIEWY)].GetData()->Fun()->Eval(m_EffectState.tempo.ang)
                            0.0,
                            //m_Config[moR(PARTICLES_VIEWZ)].GetData()->Fun()->Eval(m_EffectState.tempo.ang)
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

        ancho = 80;
        alto = 50;
        glTranslatef( base_camera_i, base_camera_j, 0 );
        glColor4f( 0.0,0.5,0.5, 0.5 );
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

        ancho = 10;
        alto = 10;
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

#ifdef KINECT_PCL

void moKinect::cloud_cb_ (const pcl::PointCloud<pcl::PointXYZRGB>::ConstPtr &cloud)
{

        /// 100 x 100 pixels , centered on platform: coordinate: platform_i, platform_j

    size_t _ww = 80; /// ancho en pixeles de la zona de sensado de objeto
    size_t _hh = 50; /// alto en pixeles de la zona de sensado de objeto
    size_t platform_i = m_OutputMode.nXRes / 2; /// punto medio i
    size_t platform_j = m_OutputMode.nYRes / 2; /// punto medio j
    size_t left_src_i = 0;
    size_t top_src_j = 0;

    m_DataLock.Lock();

    moVector3d O1_HSV_low = RGBtoHSV( m_Color1Bottom.X(), m_Color1Bottom.Y(), m_Color1Bottom.Z());
    moVector3d O1_HSV_high = RGBtoHSV( m_Color1Top.X(), m_Color1Top.Y(), m_Color1Top.Z());

    moVector3d O2_HSV_low = RGBtoHSV( m_Color2Bottom.X(), m_Color2Bottom.Y(), m_Color2Bottom.Z());
    moVector3d O2_HSV_high = RGBtoHSV( m_Color2Top.X(), m_Color2Top.Y(), m_Color2Top.Z());

    moVector3d O3_HSV_low = RGBtoHSV( m_Color3Bottom.X(), m_Color3Bottom.Y(), m_Color3Bottom.Z());
    moVector3d O3_HSV_high = RGBtoHSV( m_Color3Top.X(), m_Color3Top.Y(), m_Color3Top.Z());

    platform_i = min( base_camera_i, platform_i);
    platform_j = min( base_camera_j, platform_j);

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

    int track_minz = 100000;

    if (pImage && pData && pData2) {
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

                         track_minz = momin( ztrack, track_minz );

                        ///distancia al naranja
                        ///distancia al amarillo
                        ///distancia a la sandia: esto puede venir definido directamente desde la eleccion de (naranja,pomelo,sandia)
/*
                        float alf = ( 255.0 - cloud->points[index_src].b) / 255.0;

                        float sum = 0.0;

                        sum = (int) (alf * 255  * (ztrack - umbral_trackeo_z_near) / (  umbral_trackeo_z_far -  umbral_trackeo_z_near));
                        sum+= (int) (alf * cloud->points[index_src].r);
                        sum+= (int) (alf * cloud->points[index_src].g);
                        (sum < 255) ? sum = 0.0 : sum = sum / 3;

                        pData2[index_dst] = sum;
                        pData2[index_dst+1] = sum;
                        pData2[index_dst+2] = sum;
*/

                    }
                }

            } //fin for 2
        } //fin for1


        /// SEGUNDA PASDADA PARA DEPTH2

        for (size_t jj = 0; jj < m_OutputMode.nYRes; jj++) {

            for (size_t ii = 0; ii < m_OutputMode.nXRes; ii++) {

                index_src = ii+jj*m_OutputMode.nXRes;
                index_dst = ii*3+jj*m_OutputMode.nXRes*3;

                /// calculamos la zona de trackeo
                /// if < platform_j => el z se va alejando a medida que subimos proporcionalmente al angulo tilt...
                int ztrack = 0;

                int zcol = (int)( (float) (cloud->points[index_src].z * 1000.0f) ) ;

                pData2[index_dst] = 0;
                pData2[index_dst+1] = 0;
                pData2[index_dst+2] = 0;

                if (  jj < ( platform_j - umbral_trackeo_y_offset )  ) {
                    ///calculamos el z nuevo, con tilt:
                    ztrack = zcol;
                    ztrack-= (platform_j - jj) * trackeo_factor_angulo;

                    if ( umbral_trackeo_z_near < ztrack   &&   ztrack < umbral_trackeo_z_far && (track_minz + 100 ) > ztrack ) {

                        moVector3d iRGB = moVector3d( cloud->points[index_src].r, cloud->points[index_src].g, cloud->points[index_src].b );
                        moVector3d iHSV = RGBtoHSV( iRGB.X(), iRGB.Y(), iRGB.Z()  );
                        bool inrange = false;
                        ///distancia al amarillo


                        if (
                             (O1_HSV_low.X() <= iHSV.X()) && (iHSV.X() <= O1_HSV_high.X())
                            )
                             {
                                if ( ( O1_HSV_low.Y() <= iHSV.Y() ) && ( iHSV.Y() <= O1_HSV_high.Y() ) )  {
                                    if ( iHSV.Z() > 0.7 ) inrange = true;
                                }

                        }

                        if (
                             (O2_HSV_low.X() <= iHSV.X()) && (iHSV.X() <= O2_HSV_high.X())
                            )
                             {
                                if ( ( O2_HSV_low.Y() <= iHSV.Y() ) && ( iHSV.Y() <= O2_HSV_high.Y() ) )  {
                                    if ( iHSV.Z() > 0.7 ) inrange = true;
                                }
                        }


                        if (O3_HSV_low.X()>O3_HSV_high.X()) {
                            O3_HSV_high.X()+= 360.0;
                            iHSV.X()+= 360.0;
                        }
                        if (
                             (O3_HSV_low.X() <= iHSV.X()) && (iHSV.X() <= O3_HSV_high.X())
                            )
                             {
                                if ( ( O3_HSV_low.Y() <= iHSV.Y() ) && ( iHSV.Y() <= O3_HSV_high.Y() ) )  {

                                    if ( iHSV.Z() > 0.7 ) inrange = true;

                                }
                        }



                        if (inrange) {
                            ///distancia al naranja

                            ///distancia a la sandia: esto puede venir definido directamente desde la eleccion de (naranja,pomelo,sandia)
                            pData2[index_dst] = 255;
                            pData2[index_dst+1] = 255;
                            pData2[index_dst+2] = 255;
/*

                            float alf = ( 255.0 - cloud->points[index_src].b) / 255.0;

                            float sum = 0.0;

                            sum = (int) (alf * 255  * (ztrack - umbral_trackeo_z_near) / (  umbral_trackeo_z_far -  umbral_trackeo_z_near));
                            sum+= (int) (alf * cloud->points[index_src].r);
                            sum+= (int) (alf * cloud->points[index_src].g);
                            (sum < 255) ? sum = 0.0 : sum = sum / 3;

                            pData2[index_dst] = sum;
                            pData2[index_dst+1] = sum;
                            pData2[index_dst+2] = sum;
 */
                        }
                    }
                }

            } //fin for 2

        } //fin for 1






    } //fin pData && &&

    index_src = platform_i+platform_j*m_OutputMode.nXRes;

    m_TargetPosition = moVector3f( cloud->points[index_src].x, cloud->points[index_src].y, cloud->points[index_src].z );
    m_TargetRGB = moVector3f( cloud->points[index_src].r, cloud->points[index_src].g, cloud->points[index_src].b );

    m_DataLock.Unlock();

    float dif_H_object1;
    float dif_S_object1;
    float dif_L_object1;

    int ndif_object1 = 0;
    int ndif_object2 = 0;
    int ndif_object3 = 0;

    if (++count2==60) {

        pcl::PointCloud<pcl::PointXYZ>::Ptr cloudxyz (new pcl::PointCloud<pcl::PointXYZ> ());

        ///esto se podria acelerar
        //creando el análisis únicamente sobre la plataforma
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

        // medimos ancho y alto de los objetos ??
        //por ahora en el centro


        dif_tex1 = 0.0;
        int ndif = 0;
        //dif_tex2

        moVector3f vColDif;
        float vColDifLen;
        float minz = 10000.0;

        float difH1;
        float el_x = 0.0,el_y=0.0,el_z=0.0;

        float O1_minx=1000,O1_maxx=0,O1_miny=1000,O1_maxy=0,O1_minz = 100000;
        float O2_minx=1000,O2_maxx=0,O2_miny=1000,O2_maxy=0,O2_minz = 100000;
        float O3_minx=1000,O3_maxx=0,O3_miny=1000,O3_maxy=0,O3_minz = 100000;




        float H_mean = 0.0;
        float S_mean = 0.0;
        float L_mean = 0.0;

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

                el_x = cloud->points[index_src].x*1000.0;
                el_y = cloud->points[index_src].y*1000.0;
                el_z = cloud->points[index_src].z*1000.0;
                minz = momin( el_z, minz );

                if (  (double) (minz)  > (double)umbral_objeto_z_near ) {

                    moVector3d iRGB = moVector3d( cloud->points[index_src].r, cloud->points[index_src].g, cloud->points[index_src].b );
                    moVector3d iHSV = RGBtoHSV( iRGB.X(), iRGB.Y(), iRGB.Z()  );
                    moVector3f iHSVf = moVector3f( iHSV.X(), iHSV.Y(), iHSV.Z() );

                    vColDif = m_BaseHSV - iHSVf;

                    //saturacion muy baja el croma no cuenta...
                    if (iHSVf.Y()<0.1) {
                        //chroma cercano a 0 es grises
                        if (iHSVf.X()<0.05) {
                           vColDif.X() = 0; //chroma en 0
                        }

                        if (  179.95<=iHSVf.X() && iHSVf.X()<=180.05) {
                            vColDif.X() = 0; //chroma en 0
                        }

                    }
                    vColDifLen = vColDif.Length();

                    if (vColDifLen>0.1) {

                        dif_tex1+= vColDifLen;
                        H_mean+= iHSV.X();
                        S_mean+= iHSV.Y();
                        L_mean+= iHSV.Z();
                        ndif+= 1;
                    }

                    if (
                         (O1_HSV_low.X() <= iHSV.X()) && (iHSV.X() <= O1_HSV_high.X())
                        //&& (O1_HSV_low.Y() <= iHSV.Y()) && (iHSV.Y() <= O1_HSV_high.Y())
                        //&& (O1_HSV_low.Z() <= iHSV.Z()) && (iHSV.Z() <= O1_HSV_high.Z())
                        )
                         {
                            if ( ( O1_HSV_low.Y() <= iHSV.Y() ) && ( iHSV.Y() <= O1_HSV_high.Y() ) )  {
                            //    if ( ( O1_HSV_low.Z() <= iHSV.Z() ) && ( iHSV.Z() <= O1_HSV_high.Z() ) )  {

                                    ndif_object1+= 1;
                                    O1_minx = momin( el_x, O1_minx );
                                    O1_miny = momin( el_y, O1_miny );
                                    O1_minz = momin( el_z, O1_minz );
                            //    }
                            }

                    }

                    if (
                         (O2_HSV_low.X() <= iHSV.X()) && (iHSV.X() <= O2_HSV_high.X())
                        )
                         {
                            if ( ( O2_HSV_low.Y() <= iHSV.Y() ) && ( iHSV.Y() <= O2_HSV_high.Y() ) )  {
                               // if ( ( O2_HSV_low.Z() <= iHSV.Z() ) && ( iHSV.Z() <= O2_HSV_high.Z() ) )  {
                                    ndif_object2+= 1;
                                    O2_minx = momin( el_x, O2_minx );
                                    O2_miny = momin( el_y, O2_miny );
                                    O2_minz = momin( el_z, O2_minz );
                               // }
                            }

                    }

                    if (O3_HSV_low.X()>O3_HSV_high.X()) {
                        O3_HSV_high.X()+= 360.0;
                        iHSV.X()+= 360.0;
                    }

                    if (
                         (O3_HSV_low.X() <= iHSV.X() ) && (iHSV.X() <= O3_HSV_high.X())
                        )
                         {
                            if ( ( O3_HSV_low.Y() <= iHSV.Y() ) && ( iHSV.Y() <= O3_HSV_high.Y() ) )  {
                               // if ( ( O2_HSV_low.Z() <= iHSV.Z() ) && ( iHSV.Z() <= O2_HSV_high.Z() ) )  {
                                    ndif_object3+= 1;
                                    O3_minx = momin( el_x, O3_minx );
                                    O3_miny = momin( el_y, O3_miny );
                                    O3_minz = momin( el_z, O3_minz );
                               // }
                            }

                    }


                }

/*
                if (id_tex1!=-1 && pDataObj1) {
                    size_t index_buf;
                    index_buf = index_src*3;
                    dif_tex1+= abs(cloud->points[index_src].r - pDataObj1[ index_buf ]);
                    dif_tex1+= abs(cloud->points[index_src].g - pDataObj1[ index_buf + 1]);
                    dif_tex1+= abs(cloud->points[index_src].b - pDataObj1[ index_buf + 2]);

                }
*/

            }

        }

        /// presencia de objeto...

        if (ndif>0) {
            dif_tex1 = dif_tex1 / ndif;
            H_mean = H_mean / (float)ndif;
            S_mean = S_mean / (float)ndif;
            L_mean = L_mean / (float)ndif;
        }
        texto = moText(" [vColDifLen]: ")
                + FloatToStr( dif_tex1 )
                + moText(" [ndif]: ")
                + IntToStr( ndif )
                + moText(" [minz]: ")
                + FloatToStr( minz )
                ;

        MODebug2->Push( texto );
        texto = moText(" [Mean]: [H]: ")
                + FloatToStr( H_mean )
                + moText(" [S]: ")
                + FloatToStr( S_mean )
                + moText(" [L]: ")
                + FloatToStr( L_mean )
                ;

        MODebug2->Push( texto );

        texto = moText(" ndif_object1: ")
                + IntToStr(ndif_object1)
                + moText(" ndif_object2: ")
                + IntToStr(ndif_object2)
                + moText(" ndif_object3: ")
                + IntToStr(ndif_object3);
/*
        texto =  moText("     bottom H:")
                + FloatToStr( O1_HSV_low.X() )
                + moText(" S:")
                + FloatToStr( O1_HSV_low.Y() )
                + moText(" L:")
                + FloatToStr( O1_HSV_low.Z() )
                ;
        MODebug2->Push( texto );

        texto = moText("top H:")
                + FloatToStr( O1_HSV_high.X() )
                + moText(" S:")
                + FloatToStr( O1_HSV_high.Y() )
                + moText(" L:")
                + FloatToStr( O1_HSV_high.Z() )
                ;
*/
        MODebug2->Push( texto );

        texto = moText(" minx 1: ")
                + FloatToStr(O1_minx)
                + moText(" maxx 1: ")
                + FloatToStr(O1_maxx)
                + moText(" miny 1: ")
                + FloatToStr(O1_miny)
                + moText(" maxy 1: ")
                + FloatToStr(O1_maxy)
                + moText(" minz 1: ")
                + FloatToStr(O1_minz)
                + moText(" superficie: ")
                + FloatToStr( (O1_maxy-O1_miny) * (O1_maxx-O1_minx));
        MODebug2->Push( texto );

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

            NDIF1 = ndif_object1;
            NDIF2 = ndif_object2;
            NDIF3 = ndif_object3;
            SURFACE1 = (int) ( (O1_maxy-O1_miny) * (O1_maxx-O1_minx));
            SURFACE2 = (int) ( (O2_maxy-O2_miny) * (O2_maxx-O2_minx));
            SURFACE3 = (int) ( (O3_maxy-O3_miny) * (O3_maxx-O3_minx));

            SURFACE = ndif;

            m_DataLock.Unlock();
/*
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
*/

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


    //Show Cloud

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
                            //m_Config[moR(PARTICLES_VIEWX)].GetData()->Fun()->Eval(m_EffectState.tempo.ang)
                            0.0,
                            //m_Config[moR(PARTICLES_VIEWY)].GetData()->Fun()->Eval(m_EffectState.tempo.ang)
                            0.0,
                            //m_Config[moR(PARTICLES_VIEWZ)].GetData()->Fun()->Eval(m_EffectState.tempo.ang)
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

#endif

MOboolean
moKinect::Finish() {

	if(Codes!=NULL)
		delete [] Codes;
	Codes = NULL;
	ncodes = 0;

	return true;
}


#ifdef KINECT_OPENNI

#include "moKinectUserGenerator.cpp"


#endif



