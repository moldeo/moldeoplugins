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


#ifdef KINECT_OPENNI

    #include "moKinectUserGenerator.h"

#endif


#include "moArray.h"
moDefineDynamicArray(moKinectPostures)
moDefineDynamicArray(moKinectGestures)
moDefineDynamicArray(moKinectGestureRules)
moDefineDynamicArray(moVector3fs)

moP5 P5;

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
	pFData = NULL;
	pData2 = NULL;
	pImage = NULL;
	m_pDepthTexture = NULL;
	m_pDepthTexture2 = NULL;
	m_pDepthFloatTexture = NULL;
	m_pRGBTexture = NULL;
	m_pUserTexture = NULL;
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

  m_RightHand = NULL;
  m_RightHandX = NULL;
  m_RightHandY = NULL;
  m_RightHandZ = NULL;
  m_RightHandC = NULL;
  m_RightHandVx = NULL;
  m_RightHandVy = NULL;
  m_RightHandVz = NULL;

  m_LeftHand = NULL;
  m_LeftHandX = NULL;
  m_LeftHandY = NULL;
  m_LeftHandZ = NULL;
  m_LeftHandC = NULL;
  m_LeftHandVx = NULL;
  m_LeftHandVy = NULL;
  m_LeftHandVz = NULL;

  m_Head = NULL;
  m_HeadX = NULL;
  m_HeadY = NULL;
  m_HeadZ = NULL;
  m_HeadC = NULL;
  m_HeadVx = NULL;
  m_HeadVy = NULL;
  m_HeadVz = NULL;


  m_Body = NULL;
  m_BodyX = NULL;
  m_BodyY = NULL;
  m_BodyZ = NULL;
  m_BodyC = NULL;
  m_BodyVx = NULL;
  m_BodyVy = NULL;
  m_BodyVz = NULL;

  m_LeftKnee = NULL;
  m_LeftKneeX = NULL;
  m_LeftKneeY = NULL;
  m_LeftKneeZ = NULL;
  m_LeftKneeC = NULL;
  m_LeftKneeVx = NULL;
  m_LeftKneeVy = NULL;
  m_LeftKneeVz = NULL;

  m_RightKnee = NULL;
  m_RightKneeX = NULL;
  m_RightKneeY = NULL;
  m_RightKneeZ = NULL;
  m_RightKneeC = NULL;
  m_RightKneeVx = NULL;
  m_RightKneeVy = NULL;
  m_RightKneeVz = NULL;

  m_KneeSeparation = NULL;
  m_KneeSeparationX = NULL;
  m_KneeSeparationY = NULL;
  m_KneeSeparationZ = NULL;
  m_KneeSeparationC = NULL;
  m_KneeSeparationVx = NULL;
  m_KneeSeparationVy = NULL;
  m_KneeSeparationVz = NULL;

  m_ShouldersAngle = NULL;
  m_HandsAngle = NULL;

  m_Gesture = NULL;
  m_pDataMessage = NULL;
  m_OutletDataMessage = NULL;

	show_callback = false;
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

    p_configdefinition->Add( moText("update_on"), MO_PARAM_NUMERIC, KINECT_UPDATE_ON, moValue( "0", "INT"), moText("No,Yes") );
    p_configdefinition->Add( moText("verbose"), MO_PARAM_NUMERIC, KINECT_VERBOSE, moValue( "0", "INT"), moText("No,Yes") );

	return p_configdefinition;
}


void moKinect::UpdateParameters() {

    if (!m_OutletDataMessage) {
        m_OutletDataMessage = m_Outlets.GetRef( GetOutletIndex( moText("DATAMESSAGE") ) );
        //MODebug2->Message("moOpenCV::FaceDetection > outlet FACE_POSITION_X: "+IntToStr((int)m_FacePositionX));
    } else {
      //m_OutletDataMessage->GetData()->SetInt(  (int)(number_of_sequence>0) );
      if (m_pDataMessage) {
        m_OutletDataMessage->GetData()->SetMessage(m_pDataMessage);
        m_OutletDataMessage->Update(true);
      }

    }

    if (m_pDataMessage)
      m_pDataMessage->Empty();

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
        verbose_on = m_Config.Int( moR(KINECT_VERBOSE) );
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


/** KINECT SKELETON INFO  */
        if (!m_RightHand) {
            m_RightHand = m_Outlets.GetRef( this->GetOutletIndex( moText("RIGHT_HAND") ) );
        }
        if (!m_RightHandX) {
            m_RightHandX = m_Outlets.GetRef( this->GetOutletIndex( moText("RIGHT_HAND_X") ) );
        }
        if (!m_RightHandY) {
            m_RightHandY = m_Outlets.GetRef( this->GetOutletIndex( moText("RIGHT_HAND_Y") ) );
        }
        if (!m_RightHandZ) {
            m_RightHandZ = m_Outlets.GetRef( this->GetOutletIndex( moText("RIGHT_HAND_Z") ) );
        }
        if (!m_RightHandC) {
            m_RightHandC = m_Outlets.GetRef( this->GetOutletIndex( moText("RIGHT_HAND_C") ) );
        }
        if (!m_RightHandVx) {
            m_RightHandVx = m_Outlets.GetRef( this->GetOutletIndex( moText("RIGHT_HAND_VX") ) );
        }
        if (!m_RightHandVy) {
            m_RightHandVy = m_Outlets.GetRef( this->GetOutletIndex( moText("RIGHT_HAND_VY") ) );
        }
        if (!m_RightHandVz) {
            m_RightHandVz = m_Outlets.GetRef( this->GetOutletIndex( moText("RIGHT_HAND_VZ") ) );
        }


        if (!m_LeftHand) {
            m_LeftHand = m_Outlets.GetRef( this->GetOutletIndex( moText("LEFT_HAND") ) );
        }
        if (!m_LeftHandX) {
            m_LeftHandX = m_Outlets.GetRef( this->GetOutletIndex( moText("LEFT_HAND_X") ) );
        }
        if (!m_LeftHandY) {
            m_LeftHandY = m_Outlets.GetRef( this->GetOutletIndex( moText("LEFT_HAND_Y") ) );
        }
        if (!m_LeftHandZ) {
            m_LeftHandZ = m_Outlets.GetRef( this->GetOutletIndex( moText("LEFT_HAND_Z") ) );
        }
        if (!m_LeftHandC) {
            m_LeftHandC = m_Outlets.GetRef( this->GetOutletIndex( moText("LEFT_HAND_C") ) );
        }
        if (!m_LeftHandVx) {
            m_LeftHandVx = m_Outlets.GetRef( this->GetOutletIndex( moText("LEFT_HAND_VX") ) );
        }
        if (!m_LeftHandVy) {
            m_LeftHandVy = m_Outlets.GetRef( this->GetOutletIndex( moText("LEFT_HAND_VY") ) );
        }
        if (!m_LeftHandVz) {
            m_LeftHandVz = m_Outlets.GetRef( this->GetOutletIndex( moText("LEFT_HAND_VZ") ) );
        }

        if (!m_Head) {
            m_Head = m_Outlets.GetRef( this->GetOutletIndex( moText("HEAD") ) );
        }
        if (!m_HeadX) {
            m_HeadX = m_Outlets.GetRef( this->GetOutletIndex( moText("HEAD_X") ) );
        }
        if (!m_HeadY) {
            m_HeadY = m_Outlets.GetRef( this->GetOutletIndex( moText("HEAD_Y") ) );
        }
        if (!m_HeadZ) {
            m_HeadZ = m_Outlets.GetRef( this->GetOutletIndex( moText("HEAD_Z") ) );
        }
        if (!m_HeadC) {
            m_HeadC = m_Outlets.GetRef( this->GetOutletIndex( moText("HEAD_C") ) );
        }
        if (!m_HeadVx) {
            m_HeadVx = m_Outlets.GetRef( this->GetOutletIndex( moText("HEAD_VX") ) );
        }
        if (!m_HeadVy) {
            m_HeadVy = m_Outlets.GetRef( this->GetOutletIndex( moText("HEAD_VY") ) );
        }
        if (!m_HeadVz) {
            m_HeadVz = m_Outlets.GetRef( this->GetOutletIndex( moText("HEAD_VZ") ) );
        }

        if (!m_Body) {
            m_Body = m_Outlets.GetRef( this->GetOutletIndex( moText("TORSO") ) );
        }
        if (!m_BodyX) {
            m_BodyX = m_Outlets.GetRef( this->GetOutletIndex( moText("TORSO_X") ) );
        }
        if (!m_BodyY) {
            m_BodyY = m_Outlets.GetRef( this->GetOutletIndex( moText("TORSO_Y") ) );
        }
        if (!m_BodyZ) {
            m_BodyZ = m_Outlets.GetRef( this->GetOutletIndex( moText("TORSO_Z") ) );
        }
        if (!m_BodyC) {
            m_BodyC = m_Outlets.GetRef( this->GetOutletIndex( moText("TORSO_C") ) );
        }
        if (!m_BodyVx) {
            m_BodyVx = m_Outlets.GetRef( this->GetOutletIndex( moText("TORSO_VX") ) );
        }
        if (!m_BodyVy) {
            m_BodyVy = m_Outlets.GetRef( this->GetOutletIndex( moText("TORSO_VY") ) );
        }
        if (!m_BodyVz) {
            m_BodyVz = m_Outlets.GetRef( this->GetOutletIndex( moText("TORSO_VZ") ) );
        }

        if (!m_RightKnee) {
            m_RightKnee = m_Outlets.GetRef( this->GetOutletIndex( moText("RIGHT_KNEE") ) );
        }
        if (!m_RightKneeX) {
            m_RightKneeX = m_Outlets.GetRef( this->GetOutletIndex( moText("RIGHT_KNEE_X") ) );
        }
        if (!m_RightKneeY) {
            m_RightKneeY = m_Outlets.GetRef( this->GetOutletIndex( moText("RIGHT_KNEE_Y") ) );
        }
        if (!m_RightKneeZ) {
            m_RightKneeZ = m_Outlets.GetRef( this->GetOutletIndex( moText("RIGHT_KNEE_Z") ) );
        }
        if (!m_RightKneeC) {
            m_RightKneeC = m_Outlets.GetRef( this->GetOutletIndex( moText("RIGHT_KNEE_C") ) );
        }
        if (!m_RightKneeVx) {
            m_RightKneeVx = m_Outlets.GetRef( this->GetOutletIndex( moText("RIGHT_KNEE_VX") ) );
        }
        if (!m_RightKneeVy) {
            m_RightKneeVy = m_Outlets.GetRef( this->GetOutletIndex( moText("RIGHT_KNEE_VY") ) );
        }
        if (!m_RightKneeVz) {
            m_RightKneeVz = m_Outlets.GetRef( this->GetOutletIndex( moText("RIGHT_KNEE_VZ") ) );
        }


        if (!m_LeftKnee) {
            m_LeftKnee = m_Outlets.GetRef( this->GetOutletIndex( moText("LEFT_KNEE") ) );
        }
        if (!m_LeftKneeX) {
            m_LeftKneeX = m_Outlets.GetRef( this->GetOutletIndex( moText("LEFT_KNEE_X") ) );
        }
        if (!m_LeftKneeY) {
            m_LeftKneeY = m_Outlets.GetRef( this->GetOutletIndex( moText("LEFT_KNEE_Y") ) );
        }
        if (!m_LeftKneeZ) {
            m_LeftKneeZ = m_Outlets.GetRef( this->GetOutletIndex( moText("LEFT_KNEE_Z") ) );
        }
        if (!m_LeftKneeC) {
            m_LeftKneeC = m_Outlets.GetRef( this->GetOutletIndex( moText("LEFT_KNEE_C") ) );
        }
        if (!m_LeftKneeVx) {
            m_LeftKneeVx = m_Outlets.GetRef( this->GetOutletIndex( moText("LEFT_KNEE_VX") ) );
        }
        if (!m_LeftKneeVy) {
            m_LeftKneeVy = m_Outlets.GetRef( this->GetOutletIndex( moText("LEFT_KNEE_VY") ) );
        }
        if (!m_LeftKneeVz) {
            m_LeftKneeVz = m_Outlets.GetRef( this->GetOutletIndex( moText("LEFT_KNEE_VZ") ) );
        }

        if (!m_KneeSeparation) {
            m_KneeSeparation = m_Outlets.GetRef( this->GetOutletIndex( moText("KNEE_SEPARATION") ) );
        }
        if (!m_KneeSeparationX) {
            m_KneeSeparationX = m_Outlets.GetRef( this->GetOutletIndex( moText("KNEE_SEPARATION_X") ) );
        }
        if (!m_KneeSeparationY) {
            m_KneeSeparationY = m_Outlets.GetRef( this->GetOutletIndex( moText("KNEE_SEPARATION_Y") ) );
        }
        if (!m_KneeSeparationZ) {
            m_KneeSeparationZ = m_Outlets.GetRef( this->GetOutletIndex( moText("KNEE_SEPARATION_Z") ) );
        }
        if (!m_KneeSeparationC) {
            m_KneeSeparationC = m_Outlets.GetRef( this->GetOutletIndex( moText("KNEE_SEPARATION_C") ) );
        }
        if (!m_KneeSeparationVx) {
            m_KneeSeparationVx = m_Outlets.GetRef( this->GetOutletIndex( moText("KNEE_SEPARATION_VX") ) );
        }
        if (!m_KneeSeparationVy) {
            m_KneeSeparationVy = m_Outlets.GetRef( this->GetOutletIndex( moText("KNEE_SEPARATION_VY") ) );
        }
        if (!m_KneeSeparationVz) {
            m_KneeSeparationVz = m_Outlets.GetRef( this->GetOutletIndex( moText("KNEE_SEPARATION_VZ") ) );
        }


        if (!m_ShouldersAngle) {
            m_ShouldersAngle = m_Outlets.GetRef( this->GetOutletIndex( moText("SHOULDERS_ANGLE") ) );
        }

        if (!m_HandsAngle) {
            m_HandsAngle = m_Outlets.GetRef( this->GetOutletIndex( moText("HANDS_ANGLE") ) );
        }

/** Gesture INFO */
        if (!m_Gesture) {
            m_Gesture = m_Outlets.GetRef( this->GetOutletIndex( moText("GESTURE") ) );
        }

        if (m_Gesture) {

            //m_Gesture->AddMessage( m_GestureRecognition.m_DataMessage );
/*
            moDataMessage* pDataMessage = new moDataMessage();
*/

/*
            pDataMessage->Add( moData("TESTING") );
            pDataMessage->Add( moData("TESTING 2") );
*/

            for( int d = 0; d< m_GestureRecognition.m_DataMessage.Count(); d++ ) {
              moData tdata = m_GestureRecognition.m_DataMessage.Get(d);
              //MODebug2->Message( moText("Testing datamessage sending: ") + moText( tdata.Text() ) );

            }

            //m_GestureRecognition.m_DataMessage.Add( moData("TESTING") );
            //m_GestureRecognition.m_DataMessage.Add( moData("TESTING 2") );
            m_Gesture->GetData()->SetMessage( (moDataMessage*)&m_GestureRecognition.m_DataMessage );
            //m_Gesture->GetData()->SetMessage( pDataMessage );
            m_Gesture->Update(true);

          }

        //RESET RIGHT HAND
        m_VRightHand = moVector4d( -1.0f, -1.0f, -1.0f, -1.0f );

        if (m_RightHandC) { m_RightHandC->GetData()->SetFloat( -1.0f ); m_RightHandC->Update(true);}
        if (m_RightHand) { m_RightHand->GetData()->SetVector( (moVector4d*)&m_VRightHand ); m_RightHand->Update(true); }

        //RESET LEFT HAND
        m_VLeftHand = moVector4d( -1.0f, -1.0f, -1.0f, -1.0f );

        if (m_LeftHandC) { m_LeftHandC->GetData()->SetFloat( -1.0f ); m_LeftHandC->Update(true);}
        if (m_LeftHand) { m_LeftHand->GetData()->SetVector( (moVector4d*)&m_VLeftHand ); m_LeftHand->Update(true); }

        //RESET HEAD
        m_VHead = moVector4d( -1.0f, -1.0f, -1.0f, -1.0f );

        if (m_HeadC) { m_HeadC->GetData()->SetFloat( -1.0f ); m_HeadC->Update(true);}
        if (m_Head) { m_Head->GetData()->SetVector( (moVector4d*)&m_VHead ); m_Head->Update(true); }

        //RESET TORSO
        m_VBody = moVector4d( -1.0f, -1.0f, -1.0f, -1.0f );

        if (m_BodyC) { m_BodyC->GetData()->SetFloat( -1.0f ); m_BodyC->Update(true);}
        if (m_Body) { m_Body->GetData()->SetVector( (moVector4d*)&m_VBody ); m_Body->Update(true); }


        //RESET RIGHT KNEE
        m_VRightKnee = moVector4d( -1.0f, -1.0f, -1.0f, -1.0f );

        if (m_RightKneeC) { m_RightKneeC->GetData()->SetFloat( -1.0f ); m_RightKneeC->Update(true);}
        if (m_RightKnee) { m_RightKnee->GetData()->SetVector( (moVector4d*)&m_VRightKnee ); m_RightKnee->Update(true); }

        //RESET LEFT KNEE
        m_VLeftKnee = moVector4d( -1.0f, -1.0f, -1.0f, -1.0f );

        if (m_LeftKneeC) { m_LeftKneeC->GetData()->SetFloat( -1.0f );m_LeftKneeC->Update(true);}
        if (m_LeftKnee) { m_LeftKnee->GetData()->SetVector( (moVector4d*)&m_VLeftKnee ); m_LeftKnee->Update(true); }

        //RESET LEFT KNEE
        m_VKneeSeparation = moVector4d( -1.0f, -1.0f, -1.0f, -1.0f );

        if (m_KneeSeparationC) { m_KneeSeparationC->GetData()->SetFloat( -1.0f );m_KneeSeparationC->Update(true);}
        if (m_KneeSeparation) { m_KneeSeparation->GetData()->SetVector( (moVector4d*)&m_VKneeSeparation ); m_KneeSeparation->Update(true); }
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
	moDefineParamIndex( KINECT_VERBOSE, moText("verbose") );

    m_pDataMessage = new moDataMessage();

    UpdateParameters();

/**
        falta algo?
*/
    moTexParam tparam = MODefTex2DParams;
    //tparam.internal_format = GL_RGBA32F_ARB;
    tparam.internal_format = GL_RGB;
/*
    m_OutputMode.nXRes = 320;
    m_OutputMode.nYRes = 240;
*/

    m_OutputMode.nXRes = 640;
    m_OutputMode.nYRes = 480;
    m_OutputMode.nFPS = 30;

    int Mid = -1;

#define KINECT_OPENNI 1
#ifdef KINECT_OPENNI

    m_nRetVal = m_Context.Init();
    if (!CheckError()) {
        MODebug2->Error("Couldn't initialize Kinect Context");
        this->m_bInitialized = false;
        return false;
    } else {
        MODebug2->Message("Kinect Context Initialized!!");
    }


    m_nRetVal = m_UserGenerator.Create(m_Context);
    if (!CheckError()) {
        MODebug2->Error("Couldn't initialize Kinect User Generator");
        this->m_bInitialized = false;
        return false;
    } else {
        {
        MODebug2->Message("Kinect User Generator Initialized!!");


        if (!m_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON))
        {
            MODebug2->Error( moText("Supplied user generator doesn't support skeleton") );
            this->m_bInitialized = false;
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
                this->m_bInitialized = false;
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

        int Uid = GetResourceManager()->GetTextureMan()->AddTexture( "KINECTUSER", m_OutputMode.nXRes, m_OutputMode.nYRes );
        if (Uid>0) {


            MODebug2->Message("KINECTUSER texture created!!");

            m_pUserTexture = GetResourceManager()->GetTextureMan()->GetTexture(Uid);

            if (m_pUserTexture) {

                m_pUserTexture->BuildEmpty(m_OutputMode.nXRes, m_OutputMode.nYRes);

                m_fbo_idx = m_pResourceManager->GetFBMan()->CreateFBO();

                /*optimizar*/
                pFBO = m_pResourceManager->GetFBMan()->GetFBO(m_fbo_idx);
                if (pFBO) {
                    pFBO->AddTexture(
                        m_OutputMode.nXRes,
                        m_OutputMode.nYRes,
                        m_pUserTexture->GetTexParam(),
                        m_pUserTexture->GetGLId(),
                        attach_point );
                    //opcion con depth buffer and stencil para escenas complejas
                    pFBO->AddDepthStencilBuffer();

                    //m_pUserTexture->SetFBO( pFBO );
                    //m_pUserTexture->SetFBOAttachPoint( attach_point );
                }


            }
        }
        }
    }

    /** Create a DepthGenerator node */

    m_nRetVal = m_Depth.Create(m_Context);
    if (!CheckError()) {

        MODebug2->Error("Couldn't create DepthGenerator");
        this->m_bInitialized = false;
        return false;

    } else if (1==1) {

        MODebug2->Message("Kinect DepthGenerator Created!!");

        m_DepthPixel = m_Depth.GetDeviceMaxDepth();

        MODebug2->Message( moText("m_DepthPixel:") + IntToStr(m_DepthPixel) );

        m_nRetVal = m_Depth.SetMapOutputMode(m_OutputMode);
        if (!CheckError()) {
            MODebug2->Error("Couldn't set Map Outputmode");
            this->m_bInitialized = false;
            return false;
        }

        /** WE CREATE THE 640x480 TEXTURE*/
        moTexParam tparam = MODefTex2DParams;
        tparam.internal_format = GL_RGB;
        int Mid = -1;

        /** DEPTH MAP texture*/
        Mid = GetResourceManager()->GetTextureMan()->AddTexture( "KINECTDEPTH", m_OutputMode.nXRes, m_OutputMode.nYRes, tparam );
        if (Mid>0) {
            m_pDepthTexture = GetResourceManager()->GetTextureMan()->GetTexture(Mid);

            MODebug2->Message("KINECTDEPTH texture created!!");

            m_pDepthTexture->BuildEmpty(m_OutputMode.nXRes, m_OutputMode.nYRes);

            Mid = GetResourceManager()->GetTextureMan()->AddTexture( "KINECTDEPTH2", m_OutputMode.nXRes, m_OutputMode.nYRes, tparam );
            if (Mid>0) {
                m_pDepthTexture2 = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
            }

            tparam.internal_format = GL_RGBA32F;
            Mid = GetResourceManager()->GetTextureMan()->AddTexture( "KINECTDEPTHFLOAT", m_OutputMode.nXRes, m_OutputMode.nYRes, tparam );
            if (Mid>0) {
                m_pDepthFloatTexture = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
            }


            pCloud = new moVector3f [m_OutputMode.nXRes*m_OutputMode.nYRes];
            //pCloudDif = new moVector3f[m_OutputMode.nXRes*m_OutputMode.nYRes];
            pCloudDif = new moVector3f [m_OutputMode.nXRes*m_OutputMode.nYRes];
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
        this->m_bInitialized = false;
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
        this->m_bInitialized = false;
        return false;
    } else {
        MODebug2->Message("Kinect All Generators Started!!");
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

  m_GestureRecognition.Init();
/*
  moKinectGesture SlideHorizontalRight;
  SlideHorizontalRight.m_GestureEvent = "SLIDE_HORIZONTAL_RIGHT";
  SlideHorizontalRight.m_Interval = moKinectGestureInterval( 6, 0, 6 );
  SlideHorizontalRight.AddGestureRule( MO_KIN_SPEED_PROP_HAND_LEFT,
                                            moVector3f( 1 ,//*Proportion VX over VY,
                                                        0 ,//only VX
                                                        0 // VY
                                                         ),
                                            moVector3f( 2.5f, 0, 0 ),
                                             moVector3f( 1000000.0f, 0, 0 ) );
  SlideHorizontalRight.AddGestureRule( MO_KIN_SPEED_ABS_HAND_LEFT,
                                            moVector3f( 0 ,//Lenght V
                                                        1 ,//only VX
                                                        0 //VY
                                                         ),
                                            moVector3f( 0, 1.0f, 0 ),
                                            moVector3f( 0, 100000.0f, 0 ) );

  //SlideHorizontalRight.AddGestureRule( MO_KIN_SPEED_ABS_HAND_LEFT, moVector3f( 0 , //Lenght 0 x
                                                                                 1 , //Y
                                                                                 ),
                                        moVector3f( 0, 0, -2 ),
                                        moVector3f( 0, 0, 2 ) );
  m_GestureRecognition.m_Gestures.Add( SlideHorizontalRight );


  moKinectGesture SlideVerticalRight;
  SlideVerticalRight.m_GestureEvent = "SLIDE_VERTICAL_RIGHT";
  SlideVerticalRight.m_Interval = moKinectGestureInterval( 6, 0, 6 );
  SlideVerticalRight.AddGestureRule( MO_KIN_SPEED_PROP_HAND_LEFT,
                                            moVector3f( 0 ,//Proportion VX over VY
                                                       1 ,//Proportion VX over VY
                                                       0 //VY
                                                        ),
                                            moVector3f( 0.0f, 2.5f, 0 ),
                                            moVector3f( 0, 1000000.0f, 0 ) );
  SlideVerticalRight.AddGestureRule(  MO_KIN_SPEED_ABS_HAND_LEFT,
                                      moVector3f( 0 ,//Length
                                                  0 ,//only X
                                                  1 //Y
                                                   ),
                                      moVector3f( 0, 0, 1.0f ),
                                      moVector3f( 0, 0, 100000.0f ) );
  m_GestureRecognition.m_Gestures.Add( SlideVerticalRight );
*/

  moKinectGesture TurnRight;
  TurnRight.m_GestureEvent = "TURN_RIGHT";
  TurnRight.m_Interval = moKinectGestureInterval( 2, 0, 2 );
  TurnRight.AddGestureRule(  MO_KIN_ANGLE_HANDS,
                              moVector3f( 1.0, 0, 0 ),
                              moVector3f( 0.2f, 0.0f, 0.0f ),
                              moVector3f( 10.0f, 0.0f, 0.0f ) );
  TurnRight.AddGestureRule(  MO_KIN_DIST_ABS_INTER_HANDS,
                              moVector3f( 1.0, 0, 0 ),
                              moVector3f( 1400.0f, 0.0f, 0.0f ),
                              moVector3f( 10000.0f, 0.0f, 0.0f ) );
  m_GestureRecognition.m_Gestures.Add( TurnRight );

/**
  moKinectGesture TurnValue;
  TurnValue.m_GestureEvent = "TURN_VALUE";
  TurnValue.m_Interval = moKinectGestureInterval( 6, 0, 6 );
  TurnValue.AddGestureRule(  MO_KIN_ANGLE_SHOULDERS,
                              moVector3f( 1.0, 0, 0 ),
                              moVector3f( -3.1415f, 0.0f, 0.0f ),
                              moVector3f( 3.1415f, 0.0f, 0.0f ) );
  m_GestureRecognition.m_Gestures.Add( TurnValue );
*/
  moKinectGesture TurnLeft;
  TurnLeft.m_GestureEvent = "TURN_LEFT";
  TurnLeft.m_Interval = moKinectGestureInterval( 2, 0, 2 );
  TurnLeft.AddGestureRule(  MO_KIN_ANGLE_HANDS,
                            moVector3f( 1.0, 0, 0 ),
                            moVector3f( -10.0f, 0.0f, 0.0f ),
                            moVector3f( -0.2f, 0.0f, 0.0f ) );
  TurnLeft.AddGestureRule(  MO_KIN_DIST_ABS_INTER_HANDS,
                              moVector3f( 1.0, 0, 0 ),
                              moVector3f( 1400.0f, 0.0f, 0.0f ),
                              moVector3f( 10000.0f, 0.0f, 0.0f ) );
  m_GestureRecognition.m_Gestures.Add( TurnLeft );



  moKinectGesture Kamehameha;
  Kamehameha.m_GestureEvent = "KAMEHAMEHA";
  Kamehameha.m_Interval = moKinectGestureInterval( 120, 0, 120 );
  Kamehameha.AddGestureRule(  MO_KIN_DIST_ABS_INTER_HANDS,
                              moVector3f( 1.0, 0, 0 ),
                              moVector3f( 10.0f, 0.0f, 0.0f ),
                              moVector3f( 100.0f, 0.0f, 0.0f ) );
  m_GestureRecognition.m_Gestures.Add( Kamehameha );


  moKinectGesture ManosJuntas;
  ManosJuntas.m_GestureEvent = "MANOS_JUNTAS";
  ManosJuntas.m_Interval = moKinectGestureInterval( 30, 0, 30 );
  ManosJuntas.AddGestureRule(  MO_KIN_DIST_ABS_INTER_HANDS,
                              moVector3f( 1.0, 0, 0 ),
                              moVector3f( 10.0f, 0.0f, 0.0f ),
                              moVector3f( 100.0f, 0.0f, 0.0f ) );
  m_GestureRecognition.m_Gestures.Add( ManosJuntas );


  moKinectGesture ManosArriba;
  ManosArriba.m_GestureEvent = "MANOS_ARRIBA";
  ManosArriba.m_Interval = moKinectGestureInterval( 20, 0, 20 );
  ManosArriba.AddGestureRule(  MO_KIN_DIST_ABS_INTER_HANDS,
                              moVector3f( 1.0, 0, 0 ),
                              moVector3f( 800.0f, 0.0f, 0.0f ),
                              moVector3f( 10000.0f, 0.0f, 0.0f ) );
  ManosArriba.AddGestureRule(  MO_KIN_REL_HAND_LEFT,
                              moVector3f( 0.0, 1, 0 ),
                              moVector3f( 0.0f, 400.0f, 0.0f ),
                              moVector3f( 0.0f, 10000.0f, 0.0f ) );
  ManosArriba.AddGestureRule(  MO_KIN_REL_HAND_RIGHT,
                              moVector3f( 0.0, 1, 0 ),
                              moVector3f( 0.0f, 400.0f, 0.0f ),
                              moVector3f( 0.0f, 10000.0f, 0.0f ) );
  m_GestureRecognition.m_Gestures.Add( ManosArriba );

  moKinectGesture AllOff;
  AllOff.m_GestureEvent = "ALLOFF";
  AllOff.m_Interval = moKinectGestureInterval( 12, 0, 12 );
  AllOff.AddGestureRule(  MO_KIN_DIST_ABS_HAND_LEFT_SHOULDER_LEFT,
                              moVector3f( 1.0, 0, 0 ),
                              moVector3f( 1.0f, 0.0f, 0.0f ),
                              moVector3f( 200.0f, 0.0f, 0.0f ) );
  AllOff.AddGestureRule(  MO_KIN_DIST_ABS_HAND_RIGHT_SHOULDER_RIGHT,
                              moVector3f( 1.0, 0, 0 ),
                              moVector3f( 1.0f, 0.0f, 0.0f ),
                              moVector3f( 200.0f, 0.0f, 0.0f ) );
  m_GestureRecognition.m_Gestures.Add( AllOff );



  moKinectGesture Lluvia;
  Lluvia.m_GestureEvent = "LLUVIA";
  Lluvia.m_Interval = moKinectGestureInterval( 10, 0, 10 );
  ///distancia Y
  Lluvia.AddGestureRule(  MO_KIN_DIST_ABS_INTER_HANDS,
                              moVector3f( 1.0, 1.0f, 0 ),
                              moVector3f( 100.0f, 0.0f, 0.0f ),
                              moVector3f( 400.0f, 60.0f, 0.0f ) );
  Lluvia.AddGestureRule(  MO_KIN_ANGLE_HANDS,
                              moVector3f( 1.0, 0, 0 ),
                              moVector3f( 0.0f, 0.0f, 0.0f ),
                              moVector3f( 0.3f, 0.0f, 0.0f ) );

  Lluvia.AddGestureRule(  MO_KIN_SPEED_ABS_HAND_LEFT,
                              moVector3f( 0 ,//Length
                                          0 ,//only X
                                          1 //Y
                                           ),
                              moVector3f( 0.0f, 0.0f, 20.0f ),
                              moVector3f( 0.0f, 0.0f, 80.0f ) );
  Lluvia.AddGestureRule(  MO_KIN_SPEED_ABS_HAND_RIGHT,
                              moVector3f( 0 ,//Length
                                          0 ,//only X
                                          1 //Y
                                           ),
                              moVector3f( 0.0f, 0.0f, 20.0f ),
                              moVector3f( 0.0f, 0.0f, 80.0f ) );

  m_GestureRecognition.m_Gestures.Add( Lluvia );


  /** ALETEO*/
  moKinectGesture Flutter;
  Flutter.m_GestureEvent = "FLUTTER";
  Flutter.m_Interval = moKinectGestureInterval( 20, 0, 20 );
  Flutter.AddGestureRule(  MO_KIN_DIST_ABS_INTER_HANDS,
                              moVector3f( 1.0, 0, 0 ),
                              moVector3f( 1400.0f, 0.0f, 0.0f ),
                              moVector3f( 10000.0f, 0.0f, 0.0f ) );
  Flutter.AddGestureRule(  MO_KIN_SPEED_ABS_HAND_LEFT,
                              moVector3f( 1 ,//Length
                                          0 ,//only X
                                          1 //Y
                                           ),
                              moVector3f( 10.0f, 0.0f, -50.0f ),
                              moVector3f( 100.0f, 0.0f, 50.0f ) );
  Flutter.AddGestureRule(  MO_KIN_SPEED_ABS_HAND_RIGHT,
                              moVector3f( 1 ,//Length
                                          0 ,//only X
                                          1 //Y
                                           ),
                              moVector3f( 10.0f, 0.0f, -50.0f ),
                              moVector3f( 100.0f, 0.0f, 50.0f ) );
  m_GestureRecognition.m_Gestures.Add( Flutter );


    /**MARK AS INITIALIZES*/
	this->m_bInitialized = true;

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



bool moKinect::DrawLimb(XnUserID player, XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2)
{
  /*
	if (!m_UserGenerator.GetSkeletonCap().IsTracking(player))
	{
		//printf("not tracked!\n");
		return true;
	}

	if (!m_UserGenerator.GetSkeletonCap().IsJointActive(eJoint1) ||
		!m_UserGenerator.GetSkeletonCap().IsJointActive(eJoint2))
	{
		return false;
	}
*/
	XnSkeletonJointPosition joint1, joint2;
	m_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint1, joint1);
	m_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint2, joint2);
/*
	if (joint1.fConfidence < 0.5 || joint2.fConfidence < 0.5)
	{
		return true;
	}
*/
	XnPoint3D pt[2];
	pt[0] = joint1.position;
	pt[1] = joint2.position;

	m_Depth.ConvertRealWorldToProjective(2, pt, pt);


	glVertex3i(pt[0].X, pt[0].Y, 0);
	glVertex3i(pt[1].X, pt[1].Y, 0);
	return true;
}

static const float DEG2RAD = 3.14159/180;

void moKinect::drawCircle(float x, float y, float radius)
{
   glBegin(GL_TRIANGLE_FAN);

   for (int i=0; i < 360; i++)
   {
      float degInRad = i*DEG2RAD;
      glVertex2f(x + cos(degInRad)*radius, y + sin(degInRad)*radius);
   }

   glEnd();
}
void moKinect::DrawJoint(XnUserID player, XnSkeletonJoint eJoint)
{
  /*
	if (!m_UserGenerator.GetSkeletonCap().IsTracking(player))
	{
		//printf("not tracked!\n");
		return;
	}

	if (!m_UserGenerator.GetSkeletonCap().IsJointActive(eJoint))
	{
		return;
	}
	*/

	XnSkeletonJointPosition joint;
	m_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, eJoint, joint);
/*
	if (joint.fConfidence < 0.7)
	{
		return;
	}
*/
	XnPoint3D pt;
	pt = joint.position;

	m_Depth.ConvertRealWorldToProjective(1, &pt, &pt);

	drawCircle(pt.X, pt.Y, 4);
}

void moKinect::DrawGestureRecognition() {

  ///AXES X: time, Y: value
  glColor4f( 1.0, 1.0, 1.0, 1.0 );

  float sscale = 100.0f;

  float arrowscale = 0.1f;
  float xpos=100.0f,ypos=100.0f;

  //glScalef( sscale, sscale, sscale );

  glLineWidth(4.0);
  glPointSize(4.0);

  glBegin(GL_LINES);
    glVertex3i( xpos, ypos+sscale, 0 );
    glVertex3i( xpos, ypos, 0 );
    glVertex3i( xpos+sscale, ypos, 0 );
    glVertex3i( xpos, ypos, 0 );
  glEnd();

  ///flecha X
  glBegin(GL_LINES);
    glVertex3f( xpos+sscale - sscale*arrowscale, ypos + sscale*arrowscale, 0.0 );
    glVertex3f( xpos+sscale, ypos, 0.0 );
    glVertex3f( xpos+sscale - sscale*arrowscale, ypos - sscale*arrowscale, 0.0 );
  glEnd();

  ///flecha Y
  glBegin(GL_LINES);
    glVertex3f( xpos - sscale*arrowscale, ypos + sscale - sscale*arrowscale, 0.0 );
    glVertex3f( xpos, ypos+sscale, 0.0 );
    glVertex3f( xpos + sscale*arrowscale, ypos + sscale - sscale*arrowscale, 0.0 );
    glVertex3f( xpos, ypos+sscale, 0.0 );
  glEnd();

  int smaxy = 10;

  for( int sy=0; sy < smaxy; sy++ ) {
    int sypos = ypos + sy * sscale/smaxy;
    glBegin(GL_LINES);
      glVertex3i( xpos, sypos, 0 );
      glVertex3i( xpos-10.0, sypos, 0 );
    glEnd();

  }

  ///Draw values:
  ///take ring gesture, and draw last gesture in position 0, to last... in ...

  int ringp = m_GestureRecognition.m_RingPosition;
  int ringmax = m_GestureRecognition.m_RingMaxPosition;
  int ri, steps = ringmax;
  int r;


  for( ri=ringp,r=0; steps>0; steps--,ri--,r++ ) {

    if (ri<0) ri = ringmax; ///loop ring

    moKinectPosture& Posture( m_GestureRecognition.m_RingGesture.m_Postures[ri] );

    //MODebug2->Push( "ri: " + IntToStr(ri) + " MO_KIN_REL_HAND_LEFT: " + FloatToStr( Posture.m_PostureVector[MO_KIN_REL_HAND_LEFT].X() ) );

    glBegin(GL_POINTS);

      ///DIST BETWEEN HANDS: RED
      glColor4f( 1.0f, 0.0f, 0.0f, 1.0f);
      glVertex3f( xpos + r, ypos + Posture.m_PostureVector[MO_KIN_DIST_ABS_INTER_HANDS].X() / 10.0f, 0.0 );

      glColor4f( 1.0f, 0.0f, 1.0f, 1.0f);
      glVertex3f( xpos + r, ypos + Posture.m_PostureVector[MO_KIN_DIST_ABS_HAND_LEFT_SHOULDER_LEFT].X() / 10.0f, 0.0 );
      glColor4f( 1.0f, 0.0f, 1.0f, 1.0f);
      glVertex3f( xpos + r, ypos + Posture.m_PostureVector[MO_KIN_DIST_ABS_HAND_RIGHT_SHOULDER_RIGHT].X() / 10.0f, 0.0 );

/*
      ///REL HAND LEFT (to neck): green
      glColor4f( 0.0f, 1.0f, 0.0f, 1.0f);
      glVertex3f( xpos+r, ypos+Posture.m_PostureVector[MO_KIN_REL_HAND_LEFT].X() / 10.0f, 0.0 );
      glColor4f( 0.0f, 0.7f, 0.0f, 1.0f);
      glVertex3f( xpos+r, ypos+Posture.m_PostureVector[MO_KIN_REL_HAND_LEFT].Y() / 10.0f, 0.0 );

      ///REL HAND RIGHT (to neck): blue
      glColor4f( 0.0f, 0.0f, 1.0f, 1.0f);
      glVertex3f( xpos+r, ypos+Posture.m_PostureVector[MO_KIN_REL_HAND_RIGHT].X() / 10.0f, 0.0 );
      glColor4f( 0.0f, 0.0f, 0.7f, 1.0f);
      glVertex3f( xpos+r, ypos+Posture.m_PostureVector[MO_KIN_REL_HAND_RIGHT].Y()  / 10.0f , 0.0 );

*/
      ///SPEED HAND RIGHT: blue
      //glColor4f( 0.6f, 0.6f, 1.0f, 1.0f);
      //glVertex3f( xpos+r, ypos+Posture.m_PostureVector[MO_KIN_SPEED_ABS_HAND_RIGHT].X(), 0.0 );
      /*
      glColor4f( 0.5f, 0.5f, 0.85f, 1.0f);
      glVertex3f( xpos+r, ypos+Posture.m_PostureVector[MO_KIN_SPEED_ABS_HAND_RIGHT].Y(), 0.0 );
      glColor4f( 0.5f, 0.5f, 0.7f, 1.0f);
      glVertex3f( xpos+r, ypos+Posture.m_PostureVector[MO_KIN_SPEED_ABS_HAND_RIGHT].Z(), 0.0 );
*/

      ///SPEED HAND LEFT: green
      //glColor4f( 0.6f, 1.0f, 0.6f, 1.0f);
      //glVertex3f( xpos+r, ypos+Posture.m_PostureVector[MO_KIN_SPEED_ABS_HAND_LEFT].X(), 0.0 );
      //glColor4f( 0.5f, 0.85f, 0.5f, 1.0f);
      /*
      glColor4f( 1.0f, 0.0f, 0.0f, 1.0f);
      glVertex3f( xpos+r, ypos+Posture.m_PostureVector[MO_KIN_SPEED_ABS_HAND_LEFT].Y(), 0.0 );
      glColor4f( 0.0f, 0.0f, 1.0f, 1.0f);
      glVertex3f( xpos+r, ypos+Posture.m_PostureVector[MO_KIN_SPEED_ABS_HAND_LEFT].Z(), 0.0 );
      */
      /*
      glColor4f(    0.0f, 1.0f, 1.0f, 1.0f);
      glVertex3f(   xpos + r,
                    ypos + Posture.m_PostureVector[MO_KIN_SPEED_PROP_HAND_LEFT].X(),
                    0.0 );
*/

      glColor4f(    1.0f, 1.0f, 0.5f, 1.0f);
      glVertex3f(   xpos + r,
                    ypos + 100.0f + Posture.m_PostureVector[MO_KIN_ANGLE_HANDS].X()*100.0f,
                    0.0 );

      glColor4f(    1.0f, 1.0f, 0.0f, 1.0f);
      glVertex3f(   xpos + r,
                    ypos + 100.0f + Posture.m_PostureVector[MO_KIN_ANGLE_SHOULDERS].X()*100.0f,
                    0.0 );

    glEnd();

  }


}


void
moKinect::Update(moEventList *Events) {
	MOuint i;
	moEvent *actual,*tmp;

  UpdateParameters();

#define KINECT_OPENNI
#ifdef KINECT_OPENNI
if (update_on>0 && this->Initialized() ) {

    //MODebug2->Message("moKinect::Update > Updating");

    m_nRetVal = m_Context.WaitNoneUpdateAll();


    //m_nRetVal = m_Context.WaitOneUpdateAll(m_UserGenerator);

   // MODebug2->Message("moKinect::Update > m_Context.WaitNoneUpdateAll() " + IntToStr(m_nRetVal) );

    /** USER GENERATOR */
    //m_nRetVal = m_Context.WaitOneUpdateAll(m_UserGenerator);
    if (!CheckError() || m_nRetVal!=XN_STATUS_OK) {
        MODebug2->Error("Kinect Failed to update data.");
    } else if (1==1) {

        /**  TODO: hacer algo aqui (dibujar los esqueletos ? ) */
        XnUserID aUsers[15] = {0};
        XnUInt16 nUsers = 15;
        m_UserGenerator.GetUsers(aUsers, nUsers);

        bool process_user_in_front = false;
        XnPoint3D pt;
        XnSkeletonJointPosition xnbody;

        int userinfront = -1;
        double min_distance_center = 100000.0f;
        double distanceCenter = -1.0;
        bool bTrackingNeed  = true;

        //MODebug2->Message("moKinect::Update > Users:" + IntToStr(nUsers));

        for (i = 0; i < nUsers; ++i)
        {

            if (
                 !bTrackingNeed
                ||
                ( bTrackingNeed
                  &&
                  m_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i]) )
                 ) {

              m_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_TORSO, xnbody);
              pt = xnbody.position;
              m_Depth.ConvertRealWorldToProjective(1, &pt, &pt);

              distanceCenter = fabs( pt.X - 320.0 );

              if (verbose_on) {
                  MODebug2->Message(" user: " + IntToStr(i) + " aUsers[i]:" + IntToStr(aUsers[i]) + " pt.X:" + FloatToStr(pt.X) + " dcenter:" + FloatToStr( distanceCenter) );
              }
              if (distanceCenter<=min_distance_center) {
                userinfront = i;
                min_distance_center = distanceCenter;
                process_user_in_front = true;
              }
            }
        }

        //UNCOMMENT TO FORCE TRACKING FIRST USER
        //process_user_in_front = true;
        //userinfront = 0;

        if (process_user_in_front && verbose_on)
            MODebug2->Message("USER IN FRONT userinfront: " + IntToStr(userinfront) + " dcenter:" + FloatToStr( distanceCenter) );

        //if (userinfront>=0)  MODebug2->Message("moKinect > User in front: " + IntToStr(userinfront) + " x:" + FloatToStr(pt.X) );
        /// if (nUsers>0) nUsers = 1

        for (i = 0; i < nUsers; ++i)
        {

            /// SOLO ENVIA INFO DEL USUARIO EN LA ZONA DE ACTIVACION
            if ( process_user_in_front && userinfront == i ) {

                //sacar posicion de la mano....(pasar esto a un inlet, para poder ser pasado a la consola y ser consultado...) enviar como feature???
                //NO, enviar como mano....
                XnSkeletonJointPosition xnrighthand;
                m_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_HAND, xnrighthand);
                pt = xnrighthand.position;
                m_Depth.ConvertRealWorldToProjective(1, &pt, &pt);
                m_VRightHand = moVector4d( pt.X, pt.Y, pt.Z, xnrighthand.fConfidence );
                float auxpos = 0.0f;
                if (m_RightHandX) {
                    auxpos = m_RightHandX->GetData()->Float();
                    m_RightHandX->GetData()->SetFloat( (m_VRightHand.X()/640.0) -0.5f ); m_RightHandX->Update(true);
                    if (m_RightHandVx) {
                        m_RightHandVx->GetData()->SetFloat( auxpos - m_RightHandX->GetData()->Float() );
                        m_RightHandVx->Update(true);
                    }
                }
                if (m_RightHandY) {
                    auxpos = m_RightHandY->GetData()->Float();
                    m_RightHandY->GetData()->SetFloat( (0.5f - m_VRightHand.Y()/480.0) ); m_RightHandY->Update(true);
                    if (m_RightHandVy) {
                        m_RightHandVy->GetData()->SetFloat( auxpos - m_RightHandY->GetData()->Float() );
                        m_RightHandVy->Update(true);
                    }
                }
                if (m_RightHandZ) {
                    auxpos = m_RightHandZ->GetData()->Float();
                    m_RightHandZ->GetData()->SetFloat( m_VRightHand.Z() ); m_RightHandZ->Update(true);
                    if (m_RightHandVz) {
                        m_RightHandVz->GetData()->SetFloat( auxpos - m_RightHandZ->GetData()->Float() );
                        m_RightHandVz->Update(true);
                    }
                }
                if (m_RightHandC) { m_RightHandC->GetData()->SetFloat( xnrighthand.fConfidence ); m_RightHandC->Update(true); }
                if (m_RightHand) { m_RightHand->GetData()->SetVector( (moVector4d*)&m_VRightHand ); m_RightHand->Update(true); }

                //MODebug2->Message("moKinect::Update > RightHand: user" + IntToStr(i) + " X:" + FloatToStr(xnrighthand.position.X) + " Y:" + FloatToStr(xnrighthand.position.Y) + " C:" + FloatToStr(xnrighthand.fConfidence) );
                XnSkeletonJointPosition xnlefthand;
                m_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_HAND, xnlefthand);
                pt = xnlefthand.position;
                m_Depth.ConvertRealWorldToProjective(1, &pt, &pt);
                m_VLeftHand = moVector4d( pt.X, pt.Y, pt.Z, xnlefthand.fConfidence );

                if (m_LeftHandX) {
                    auxpos = m_LeftHandX->GetData()->Float();
                    m_LeftHandX->GetData()->SetFloat( (m_VLeftHand.X()/640.0) -0.5f ); m_LeftHandX->Update(true);
                    if (m_LeftHandVx) {
                        m_LeftHandVx->GetData()->SetFloat( auxpos - m_LeftHandX->GetData()->Float() );
                        m_LeftHandVx->Update(true);
                    }
                }
                if (m_LeftHandY) {
                    auxpos = m_LeftHandY->GetData()->Float();
                    m_LeftHandY->GetData()->SetFloat( (0.5f - m_VLeftHand.Y()/480.0) ); m_LeftHandY->Update(true);
                    if (m_LeftHandVy) {
                        m_LeftHandVy->GetData()->SetFloat( auxpos - m_LeftHandY->GetData()->Float() );
                        m_LeftHandVy->Update(true);
                    }
                }
                if (m_LeftHandZ) {
                    auxpos = m_LeftHandZ->GetData()->Float();
                    m_LeftHandZ->GetData()->SetFloat( m_VLeftHand.Z() ); m_LeftHandZ->Update(true);
                    if (m_LeftHandVz) {
                        m_LeftHandVz->GetData()->SetFloat( auxpos - m_LeftHandZ->GetData()->Float() );
                        m_LeftHandVz->Update(true);
                    }
                }
                if (m_LeftHandC) { m_LeftHandC->GetData()->SetFloat( xnlefthand.fConfidence ); m_LeftHandC->Update(true); }
                if (m_LeftHand) { m_LeftHand->GetData()->SetVector( (moVector4d*)&m_VLeftHand ); m_LeftHand->Update(true); }

                if (verbose_on) {
                  MODebug2->Message("moKinect::Update > LeftHand: user" + IntToStr(i)
                                  + " X:" + FloatToStr(xnlefthand.position.X) + " Y:" + FloatToStr(xnlefthand.position.Y)
                                  + " Z:" + FloatToStr(xnlefthand.position.Z) );
                }

                XnSkeletonJointPosition xnhead;
                m_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_HEAD, xnhead);
                pt = xnhead.position;
                m_Depth.ConvertRealWorldToProjective(1, &pt, &pt);
                m_VHead = moVector4d( pt.X, pt.Y, pt.Z, xnhead.fConfidence );

                if (m_HeadX) { m_HeadX->GetData()->SetFloat( (m_VHead.X()/640.0) -0.5f ); m_HeadX->Update(true); }
                if (m_HeadY) { m_HeadY->GetData()->SetFloat( (0.5f - m_VHead.Y()/480.0) ); m_HeadY->Update(true); }
                if (m_HeadZ) { m_HeadZ->GetData()->SetFloat( m_VHead.Z() ); m_HeadZ->Update(true); }
                if (m_HeadC) { m_HeadC->GetData()->SetFloat( xnhead.fConfidence ); m_HeadC->Update(true); }
                if (m_Head) { m_Head->GetData()->SetVector( (moVector4d*)&m_VHead ); m_Head->Update(true); }

                /** TORSO */
                m_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_TORSO, xnbody);
                pt = xnbody.position;
                m_Depth.ConvertRealWorldToProjective(1, &pt, &pt);
                m_VBody = moVector4d( pt.X, pt.Y, pt.Z, xnbody.fConfidence );

                if (m_BodyX) { m_BodyX->GetData()->SetFloat( (m_VBody.X()/640.0) -0.5f ); m_BodyX->Update(true); }
                if (m_BodyY) { m_BodyY->GetData()->SetFloat( (0.5f - m_VBody.Y()/480.0) ); m_BodyY->Update(true); }
                if (m_BodyZ) { m_BodyZ->GetData()->SetFloat( m_VBody.Z() ); m_BodyZ->Update(true); }
                if (m_BodyC) { m_BodyC->GetData()->SetFloat( xnbody.fConfidence ); m_BodyC->Update(true); }
                if (m_Body) { m_Body->GetData()->SetVector( (moVector4d*)&m_VBody ); m_Body->Update(true); }

                //MODebug2->Message("moKinect::Update > " + FloatToStr(m_BodyX->GetData()->Float()) );
                //MODebug2->Message("moKinect::Update > RightHand: user" + IntToStr(i) + " X:" + FloatToStr(xnrighthand.position.X) + " Y:" + FloatToStr(xnrighthand.position.Y) + " C:" + FloatToStr(xnrighthand.fConfidence) );
                XnSkeletonJointPosition xnleftknee;
                m_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_LEFT_KNEE, xnleftknee);
                pt = xnleftknee.position;
                m_Depth.ConvertRealWorldToProjective(1, &pt, &pt);
                m_VLeftKnee = moVector4d( pt.X, pt.Y, pt.Z, xnleftknee.fConfidence );

                if (m_LeftKneeX) {
                    auxpos = m_LeftKneeX->GetData()->Float();
                    m_LeftKneeX->GetData()->SetFloat( (m_VLeftKnee.X()/640.0) -0.5f ); m_LeftKneeX->Update(true);
                    if (m_LeftKneeVx) {
                        m_LeftKneeVx->GetData()->SetFloat( auxpos - m_LeftKneeX->GetData()->Float() );
                        m_LeftKneeVx->Update(true);
                    }
                }
                if (m_LeftKneeY) {
                    auxpos = m_LeftKneeY->GetData()->Float();
                    m_LeftKneeY->GetData()->SetFloat( (0.5f - m_VLeftKnee.Y()/480.0) ); m_LeftKneeY->Update(true);
                    if (m_LeftKneeVy) {
                        m_LeftKneeVy->GetData()->SetFloat( auxpos - m_LeftKneeY->GetData()->Float() );
                        m_LeftKneeVy->Update(true);
                    }
                }
                if (m_LeftKneeZ) {
                    auxpos = m_LeftKneeZ->GetData()->Float();
                    m_LeftKneeZ->GetData()->SetFloat( m_VLeftKnee.Z() ); m_LeftKneeZ->Update(true);
                    if (m_LeftKneeVz) {
                        m_LeftKneeVz->GetData()->SetFloat( auxpos - m_LeftKneeVz->GetData()->Float() );
                        m_LeftKneeVz->Update(true);
                    }
                }
                if (m_LeftKneeC) { m_LeftKneeC->GetData()->SetFloat( xnleftknee.fConfidence ); m_LeftKneeC->Update(true); }
                if (m_LeftKnee) { m_LeftKnee->GetData()->SetVector( (moVector4d*)&m_VLeftKnee ); m_LeftKnee->Update(true); }



                //MODebug2->Message("moKinect::Update > RightHand: user" + IntToStr(i) + " X:" + FloatToStr(xnrighthand.position.X) + " Y:" + FloatToStr(xnrighthand.position.Y) + " C:" + FloatToStr(xnrighthand.fConfidence) );
                XnSkeletonJointPosition xnrightknee;
                m_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(aUsers[i], XN_SKEL_RIGHT_KNEE, xnrightknee);
                pt = xnrightknee.position;
                m_Depth.ConvertRealWorldToProjective(1, &pt, &pt);
                m_VRightKnee = moVector4d( pt.X, pt.Y, pt.Z, xnrightknee.fConfidence );

                if (m_RightKneeX) {
                    auxpos = m_RightKneeX->GetData()->Float();
                    m_RightKneeX->GetData()->SetFloat( (m_VRightKnee.X()/640.0) -0.5f ); m_RightKneeX->Update(true);
                    if (m_RightKneeVx) {
                        m_RightKneeVx->GetData()->SetFloat( auxpos - m_RightKneeX->GetData()->Float() );
                        m_RightKneeVx->Update(true);
                    }
                }
                if (m_RightKneeY) {
                    auxpos = m_RightKneeY->GetData()->Float();
                    m_RightKneeY->GetData()->SetFloat( (0.5f - m_VRightKnee.Y()/480.0) ); m_RightKneeY->Update(true);
                    if (m_RightKneeVy) {
                        m_RightKneeVy->GetData()->SetFloat( auxpos - m_RightKneeY->GetData()->Float() );
                        m_RightKneeVy->Update(true);
                    }
                }
                if (m_RightKneeZ) {
                    auxpos = m_RightKneeZ->GetData()->Float();
                    m_RightKneeZ->GetData()->SetFloat( m_VRightKnee.Z() ); m_RightKneeZ->Update(true);
                    if (m_RightKneeVz) {
                        m_RightKneeVz->GetData()->SetFloat( auxpos - m_RightKneeVz->GetData()->Float() );
                        m_RightKneeVz->Update(true);
                    }
                }
                if (m_RightKneeC) { m_RightKneeC->GetData()->SetFloat( xnrightknee.fConfidence ); m_RightKneeC->Update(true); }
                if (m_RightKnee) { m_RightKnee->GetData()->SetVector( (moVector4d*)&m_VRightKnee ); m_RightKnee->Update(true); }

                /** KNEE SEPARATION **/
                m_VKneeSeparation = moVector4d( m_VLeftKnee.X() - m_VRightKnee.X(),
                                               m_VLeftKnee.Y() - m_VRightKnee.Y(),
                                               m_VLeftKnee.Z() - m_VRightKnee.Z(),
                                               (m_VLeftKnee.W() + m_VRightKnee.W())*0.5 );

                if (m_KneeSeparationX) {
                    auxpos = m_KneeSeparationX->GetData()->Float();
                    m_KneeSeparationX->GetData()->SetFloat( m_VKneeSeparation.X()/640.0 ); m_KneeSeparationX->Update(true);
                    if (m_KneeSeparationVx) {
                        m_KneeSeparationVx->GetData()->SetFloat( auxpos - m_KneeSeparationX->GetData()->Float() );
                        m_KneeSeparationVx->Update(true);
                    }
                }

                if (m_KneeSeparationY) {
                    auxpos = m_KneeSeparationY->GetData()->Float();
                    m_KneeSeparationY->GetData()->SetFloat( m_VKneeSeparation.Y()/480.0 ); m_KneeSeparationY->Update(true);
                    if (m_KneeSeparationVy) {
                        m_KneeSeparationVy->GetData()->SetFloat( auxpos - m_KneeSeparationY->GetData()->Float() );
                        m_KneeSeparationVy->Update(true);
                    }
                }

                if (m_KneeSeparationZ) {
                    auxpos = m_KneeSeparationZ->GetData()->Float();
                    m_KneeSeparationZ->GetData()->SetFloat( m_VKneeSeparation.Z() ); m_KneeSeparationZ->Update(true);
                    if (m_KneeSeparationVz) {
                        m_KneeSeparationVz->GetData()->SetFloat( auxpos - m_KneeSeparationZ->GetData()->Float() );
                        m_KneeSeparationVz->Update(true);
                    }
                }



                if (m_pDataMessage) {
                    m_pDataMessage->Empty();//EMPTY IN UpdateParameters()
                    moData pData;

                    pData.SetText( moText("1/fader1") );
                    m_pDataMessage->Add(pData);

                    /*pData.SetText( moText("USER") );
                    m_pDataMessage->Add(pData);

                    pData.SetInt( (int)(i) );
                    m_pDataMessage->Add(pData);

                    pData.SetText( moText("TORSOX") );
                    m_pDataMessage->Add(pData);*/

                    pData.SetFloat(  sqrt(m_VBody.X()*m_VBody.X() + m_VBody.Y()*m_VBody.Y())+ m_VBody.Z()*m_VBody.Z());
                    m_pDataMessage->Add(pData);

                    /*
                    pData.SetText( moText("TORSOY") );
                    m_pDataMessage->Add(pData);
*/
                   // pData.SetFloat(  m_VBody.Y());
                   // m_pDataMessage->Add(pData);

                   // pData.SetFloat(  m_VBody.Z());
                   // m_pDataMessage->Add(pData);
                /*
                    moText ccc = "";
                    for( int c=0; c<m_pDataMessage->Count(); c++) {
                      ccc = ccc + m_pDataMessage->Get(c).ToText();
                    }
                    //MODebug2->Push(ccc);
                */
                  }
            }

        }

        m_GestureRecognition.UpdateBegin();

        if (process_user_in_front) {
          m_GestureRecognition.UpdateUser( aUsers[userinfront] );

          moVector3f angleVec = m_GestureRecognition.m_ActualPosture.m_PostureVector.Get( (int) MO_KIN_ANGLE_SHOULDERS );
          float angleVal = angleVec.X();
          if (m_ShouldersAngle) {
            m_ShouldersAngle->GetData()->SetFloat(angleVal);
            m_ShouldersAngle->Update(true);
            //MODebug2->Message("moKinect::Update() > m_ShouldersAngle: " + FloatToStr(angleVal) );
          }

          angleVec = m_GestureRecognition.m_ActualPosture.m_PostureVector.Get( (int) MO_KIN_ANGLE_HANDS );
          angleVal = angleVec.X();
          if (m_HandsAngle) {
              m_HandsAngle->GetData()->SetFloat(angleVal);
              m_HandsAngle->Update(true);
              //MODebug2->Message("moKinect::Update() > m_HandsAngle: "
              //                  + m_HandsAngle->GetData()->ToText()  );
          }

        }



        m_GestureRecognition.UpdateEnd();

        if (process_user_in_front==false) {
          /** RESET ALL VALUES !!!*/

          if (m_BodyX) {
            m_BodyX->GetData()->SetFloat( 0.0 );
            m_BodyX->Update(true);
          }

          if (m_BodyY) {
            m_BodyY->GetData()->SetFloat( 0.0 );
            m_BodyY->Update(true);
          }

          if (m_BodyZ) {
            m_BodyZ->GetData()->SetFloat( 0.0 );
            m_BodyZ->Update(true);
          }

          if (m_LeftKneeZ) {
            m_LeftKneeZ->GetData()->SetFloat( 0.0 );
            m_LeftKneeZ->Update(true);
          }

          if (m_RightKneeX) {
            m_RightKneeX->GetData()->SetFloat( 0.0 );
            m_RightKneeX->Update(true);
          }

          if (m_RightKneeY) {
            m_RightKneeY->GetData()->SetFloat( 0.0 );
            m_RightKneeY->Update(true);
          }

          if (m_RightKneeZ) {
            m_RightKneeZ->GetData()->SetFloat( 0.0 );
            m_RightKneeZ->Update(true);
          }

          if (m_KneeSeparationVx) {
            m_KneeSeparationVx->GetData()->SetFloat( 0.0 );
            m_KneeSeparationVx->Update(true);
          }

          if (m_KneeSeparationVx) {
            m_KneeSeparationVx->GetData()->SetFloat( 0.0 );
            m_KneeSeparationVx->Update(true);
          }

          if (m_KneeSeparationVz) {
            m_KneeSeparationVz->GetData()->SetFloat( 0.0 );
            m_KneeSeparationVz->Update(true);
          }

          if (m_KneeSeparationX) {
            m_KneeSeparationX->GetData()->SetFloat( 0.0 );
            m_KneeSeparationX->Update(true);
          }

          if (m_KneeSeparationY) {
            m_KneeSeparationY->GetData()->SetFloat( 0.0 );
            m_KneeSeparationY->Update(true);
          }

          if (m_KneeSeparationZ) {
            m_KneeSeparationZ->GetData()->SetFloat( 0.0 );
            m_KneeSeparationZ->Update(true);
          }

        }


        if (pFBO && m_pUserTexture) {

            pFBO->Bind();
            pFBO->SetDrawTexture( m_pUserTexture->GetFBOAttachPoint() );
            //MODebug2->Message("moKinect:: Drawing user texture");
            //glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();

            //xn::SceneMetaData sceneMD;
            //xn::DepthMetaData depthMD;
            //m_Depth.GetMetaData(depthMD);
            //glOrtho(0, depthMD.XRes(), depthMD.YRes(), 0, -1.0, 1.0);
            //m_pResourceManager->GetGLMan()->SetOrthographicView();
            m_pResourceManager->GetGLMan()->SetOrthographicView( m_pUserTexture->GetWidth(), m_pUserTexture->GetHeight() );
            glDisable(GL_TEXTURE_2D);


            for (i = 0; i < nUsers; ++i)
            {

              if (i==0) glColor4f( 1.0, 1.0, 1.0, 1.0 ); //white
              if (i==1) glColor4f( 0.0, 1.0, 0.0, 1.0 ); //green
              if (i==2) glColor4f( 1.0, 0.0, 0.0, 1.0 ); //red
              if (i==3) glColor4f( 1.0, 1.0, 0.0, 1.0 ); //yellow
              if (i==4) glColor4f( 0.0, 1.0, 1.0, 1.0 ); //cyan

              //if (m_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i])) {

                  //P5.triangle( 0+i*5, 0, 5+i*5, 5, 0+i*5, 10 );
                  drawCircle( 10 + i*5, 10, 8);
              //}

              // Try to draw all joints
              DrawJoint(aUsers[i], XN_SKEL_HEAD);
              DrawJoint(aUsers[i], XN_SKEL_NECK);
              DrawJoint(aUsers[i], XN_SKEL_TORSO);
              DrawJoint(aUsers[i], XN_SKEL_WAIST);

              DrawJoint(aUsers[i], XN_SKEL_LEFT_COLLAR);
              DrawJoint(aUsers[i], XN_SKEL_LEFT_SHOULDER);
              DrawJoint(aUsers[i], XN_SKEL_LEFT_ELBOW);
              DrawJoint(aUsers[i], XN_SKEL_LEFT_WRIST);
              DrawJoint(aUsers[i], XN_SKEL_LEFT_HAND);
              DrawJoint(aUsers[i], XN_SKEL_LEFT_FINGERTIP);

              DrawJoint(aUsers[i], XN_SKEL_RIGHT_COLLAR);
              DrawJoint(aUsers[i], XN_SKEL_RIGHT_SHOULDER);
              DrawJoint(aUsers[i], XN_SKEL_RIGHT_ELBOW);
              DrawJoint(aUsers[i], XN_SKEL_RIGHT_WRIST);
              DrawJoint(aUsers[i], XN_SKEL_RIGHT_HAND);
              DrawJoint(aUsers[i], XN_SKEL_RIGHT_FINGERTIP);

              DrawJoint(aUsers[i], XN_SKEL_LEFT_HIP);
              DrawJoint(aUsers[i], XN_SKEL_LEFT_KNEE);
              DrawJoint(aUsers[i], XN_SKEL_LEFT_ANKLE);
              DrawJoint(aUsers[i], XN_SKEL_LEFT_FOOT);

              DrawJoint(aUsers[i], XN_SKEL_RIGHT_HIP);
              DrawJoint(aUsers[i], XN_SKEL_RIGHT_KNEE);
              DrawJoint(aUsers[i], XN_SKEL_RIGHT_ANKLE);
              DrawJoint(aUsers[i], XN_SKEL_RIGHT_FOOT);

              // Draw Limbs
              glBegin(GL_LINES);
              DrawLimb(aUsers[i], XN_SKEL_HEAD, XN_SKEL_NECK);

              DrawLimb(aUsers[i], XN_SKEL_NECK, XN_SKEL_LEFT_SHOULDER);
              DrawLimb(aUsers[i], XN_SKEL_LEFT_SHOULDER, XN_SKEL_LEFT_ELBOW);
              if (!DrawLimb(aUsers[i], XN_SKEL_LEFT_ELBOW, XN_SKEL_LEFT_WRIST))
              {
                  DrawLimb(aUsers[i], XN_SKEL_LEFT_ELBOW, XN_SKEL_LEFT_HAND);
              }
              else
              {
                  DrawLimb(aUsers[i], XN_SKEL_LEFT_WRIST, XN_SKEL_LEFT_HAND);
                  DrawLimb(aUsers[i], XN_SKEL_LEFT_HAND, XN_SKEL_LEFT_FINGERTIP);
              }


              DrawLimb(aUsers[i], XN_SKEL_NECK, XN_SKEL_RIGHT_SHOULDER);
              DrawLimb(aUsers[i], XN_SKEL_RIGHT_SHOULDER, XN_SKEL_RIGHT_ELBOW);
              if (!DrawLimb(aUsers[i], XN_SKEL_RIGHT_ELBOW, XN_SKEL_RIGHT_WRIST))
              {
                  DrawLimb(aUsers[i], XN_SKEL_RIGHT_ELBOW, XN_SKEL_RIGHT_HAND);
              }
              else
              {
                  DrawLimb(aUsers[i], XN_SKEL_RIGHT_WRIST, XN_SKEL_RIGHT_HAND);
                  DrawLimb(aUsers[i], XN_SKEL_RIGHT_HAND, XN_SKEL_RIGHT_FINGERTIP);
              }

              DrawLimb(aUsers[i], XN_SKEL_LEFT_SHOULDER, XN_SKEL_TORSO);
              DrawLimb(aUsers[i], XN_SKEL_RIGHT_SHOULDER, XN_SKEL_TORSO);

              DrawLimb(aUsers[i], XN_SKEL_TORSO, XN_SKEL_LEFT_HIP);
              DrawLimb(aUsers[i], XN_SKEL_LEFT_HIP, XN_SKEL_LEFT_KNEE);
              DrawLimb(aUsers[i], XN_SKEL_LEFT_KNEE, XN_SKEL_LEFT_FOOT);

              DrawLimb(aUsers[i], XN_SKEL_TORSO, XN_SKEL_RIGHT_HIP);
              DrawLimb(aUsers[i], XN_SKEL_RIGHT_HIP, XN_SKEL_RIGHT_KNEE);
              DrawLimb(aUsers[i], XN_SKEL_RIGHT_KNEE, XN_SKEL_RIGHT_FOOT);

              DrawLimb(aUsers[i], XN_SKEL_LEFT_HIP, XN_SKEL_RIGHT_HIP);
              glEnd();
            }

            /** DIBUJAR EL GRAFICO DE GESTURES.... */
            DrawGestureRecognition();

            pFBO->Unbind();
        }


    }

    /** UPDATE IMAGE*/
    //m_nRetVal = m_Context.WaitOneUpdateAll(m_RGBImage);
    if (!CheckError()) {
        MODebug2->Error("Kinect Failed to update data.");
    } else {
        //const XnRGB24Pixel*    pImageMap = m_RGBImage.GetRGB24ImageMap();

        xn::ImageMetaData imageMD;

        m_RGBImage.GetMetaData(imageMD);
        const XnUInt8* pImageData = imageMD.Data();


        if (pImageData) {

            if (!pImage) {
                pImage =  new unsigned char [ m_OutputMode.nXRes * m_OutputMode.nYRes * 3 ];

            }

            /** esto se puede acelerar...*/
            if (pImage) {
                /*
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
                }*/
                if (m_pRGBTexture) {
                    //MODebug2->Push("Building");
                    m_pRGBTexture->BuildFromBuffer( m_OutputMode.nXRes, m_OutputMode.nYRes, pImageData, GL_RGB, GL_UNSIGNED_BYTE );
                }
            }

        }

    }

    /** UPDATE DEPTH*/
    //m_nRetVal = m_Context.WaitOneUpdateAll(m_Depth);
    if (!CheckError() && 1==2) {
        MODebug2->Error("Kinect Failed to update data.");
    } else if (1==1) {
        //const XnDepthPixel* pDepthMap = m_Depth.GetDepthMap();
        xn::DepthMetaData depthMD;

        m_Depth.GetMetaData(depthMD);
        /** OPTIMIZAR */
        const XnDepthPixel* pDepthMap = depthMD.Data();

        if ( pDepthMap) {

            //MODebug2->Message("Getting Depth Map");

            /** Si es por primera vez: crear la textura: OUTLET... de textura??

            */

            if (!pData) {
                pData =  new unsigned char [ m_OutputMode.nXRes * m_OutputMode.nYRes * 3 ];

            }
            if (!pFData) {
                pFData =  new float [ m_OutputMode.nXRes * m_OutputMode.nYRes * 4 ];

            }
            if (!pData2) {
                pData2 =  new unsigned char [ m_OutputMode.nXRes * m_OutputMode.nYRes * 3 ];

            }

            int s = 0;

            int disoff = 0.0;

            disoff = (int)(m_Offset.Y() - m_Offset.X());

            float distance_r = 0.0;

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
            if (disoff==0) disoff=1.0;

            if (pData && pData2 && pFData) {
                for( int j=0; j<m_OutputMode.nYRes; j++ ) {
                    for( int i=0; i<m_OutputMode.nXRes; i++ ) {


                        //pData[i*3+j*m_OutputMode.nXRes*3] = *pDepthMap % 255;
                        //pData[i*3+1+j*m_OutputMode.nXRes*3] = *pDepthMap % 255;
                        //pData[i*3+2+j*m_OutputMode.nXRes*3] = *pDepthMap % 255;
                        z = (float)(*pDepthMap);

                        float radi = 0.0;

                        if (

                                m_Offset.X()<z
                            && z<m_Offset.Y()

                            && (int)(m_OffsetBox.X())<=i
                            && i<=(int)(m_OffsetBox.Y())
                            && (int)(m_OffsetBox.Z())<=j
                            && j<=(int)(m_OffsetBox.W())

                            && radi<=m_OffsetRadius

                            ) {


                            pData[i*3+j*m_OutputMode.nXRes*3] = (z - (int)m_Offset.X() );
                            pData[i*3+j*m_OutputMode.nXRes*3] = (float)( z*255.0f/disoff);
                            //pData[i*3+j*m_OutputMode.nXRes*3] = ( pData[i*3+j*m_OutputMode.nXRes*3] >> 3 ) % 255;
                            //pData[i*3+1+j*m_OutputMode.nXRes*3] = (*pDepthMap - disoff) % 255;
                            //pData[i*3+2+j*m_OutputMode.nXRes*3] = (*pDepthMap - disoff) % 255;

                            //FLOAT TEXTURE
                            pFData[i*4+j*m_OutputMode.nXRes*4] = z/10000.0f;

                            //grey values:

                            pFData[i*4+1+j*m_OutputMode.nXRes*4] = pFData[i*4+j*m_OutputMode.nXRes*4];
                            pFData[i*4+2+j*m_OutputMode.nXRes*4] = pFData[i*4+j*m_OutputMode.nXRes*4];
                            pFData[i*4+3+j*m_OutputMode.nXRes*4] = 1.0f;


                            pData[i*3+1+j*m_OutputMode.nXRes*3] = pData[i*3+j*m_OutputMode.nXRes*3];
                            pData[i*3+2+j*m_OutputMode.nXRes*3] = pData[i*3+j*m_OutputMode.nXRes*3];

                            pData2[i*3+j*m_OutputMode.nXRes*3] = 0;
                            pData2[i*3+1+j*m_OutputMode.nXRes*3] = 0;
                            pData2[i*3+2+j*m_OutputMode.nXRes*3] = 0;
/*
                            float z2 = ( (float) z * scaleFactor );
                            //x = (i - 320) * (z2 + minDistance) * scaleFactor;
                            //y = (240 - j) * (z2 + minDistance) * scaleFactor;
                            x = ( i - m_OutputMode.nXRes/2 ) * z2 / (m_OutputMode.nXRes/2);
                            y = ( m_OutputMode.nYRes/2 - j ) * z2 / (m_OutputMode.nYRes/2);


                            if (pCloud && pCloudDif) {

                                pCloud[i+j*m_OutputMode.nXRes] = moVector3f( x,y,z );

                            //difference with Reference
                                pCloudDif[i+j*m_OutputMode.nXRes] = pCloud[i+j*m_OutputMode.nXRes] - m_ReferencePoint;

                                distance_r = pCloudDif[i+j*m_OutputMode.nXRes].Length();
                            } //fin analisis
*/
                        } else {

                            pData[i*3+j*m_OutputMode.nXRes*3] = 0;
                            pData[i*3+1+j*m_OutputMode.nXRes*3] = 0;
                            pData[i*3+2+j*m_OutputMode.nXRes*3] = 0;

                            pData2[i*3+j*m_OutputMode.nXRes*3] = 0;
                            pData2[i*3+1+j*m_OutputMode.nXRes*3] = 0;
                            pData2[i*3+2+j*m_OutputMode.nXRes*3] = 0;

                            pFData[i*4+j*m_OutputMode.nXRes*4] = 0.0f;
                            pFData[i*4+1+j*m_OutputMode.nXRes*4] = 0.0f;
                            pFData[i*4+2+j*m_OutputMode.nXRes*4] = 0.0f;
                            pFData[i*4+3+j*m_OutputMode.nXRes*4] = 0.0f;

                        }


                        pDepthMap++;
                    }
                }

                if (m_pDepthTexture) {
                    m_pDepthTexture->BuildFromBuffer( m_OutputMode.nXRes, m_OutputMode.nYRes, pData, GL_RGB, GL_UNSIGNED_BYTE );
                }

                if (m_pDepthFloatTexture) {
                    m_pDepthFloatTexture->BuildFromBuffer( m_OutputMode.nXRes, m_OutputMode.nYRes, pFData, GL_RGBA, GL_FLOAT );
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
/*
                if (m_pDepthTexture2) {
                    m_pDepthTexture2->BuildFromBuffer( m_OutputMode.nXRes, m_OutputMode.nYRes, pData2, GL_RGB, GL_UNSIGNED_BYTE );
                }
*/

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


} //FIN updateon < 1
#endif

    moMoldeoObject::Update( Events );

}


MOboolean
moKinect::Finish() {

	if(Codes!=NULL)
		delete [] Codes;
	Codes = NULL;
	ncodes = 0;

  if (m_Depth.IsValid()) {
    m_Depth.StopGenerating();
    m_Depth.Release();
  }

  if (m_UserGenerator.IsValid()) {
    m_UserGenerator.StopGenerating();
    m_UserGenerator.Release();
  }

  if (m_IRImage.IsValid()) {
    m_IRImage.StopGenerating();
    m_IRImage.Release();
  }

  m_Context.Release();
  cout << "Kinect Released" << endl;

	return true;
}


#ifdef KINECT_OPENNI

//#include "moKinectUserGenerator.cpp"


#include <map>
std::map<XnUInt32, std::pair<XnCalibrationStatus, XnPoseDetectionStatus> > m_Errors;



void MyCalibrationInProgress(xn::SkeletonCapability& capability, XnUserID id, XnCalibrationStatus calibrationError, void* pCookie)
{
	m_Errors[id].first = calibrationError;
}
void MyPoseInProgress(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID id, XnPoseDetectionStatus poseError, void* pCookie)
{
	m_Errors[id].second = poseError;
}



// Callback: New user was detected
void User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	//printf("New User %d\n", nId);

	//moAbstract::MODebug2->Push( moText("New user:") + IntToStr(nId));

	// New user found
	if (m_bNeedPose)
	{
		m_UserGenerator.GetPoseDetectionCap().StartPoseDetection(m_strPose, nId);
	}
	else
	{
		m_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
	}
}


// Callback: An existing user was lost
void User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	//moAbstract::MODebug2->Push( moText("Lost user:") + IntToStr(nId));
	//printf("Lost user %d\n", nId);
}


// Callback: Detected a pose
void UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{
	//printf("Pose %s detected for user %d\n", strPose, nId);
	moAbstract::MODebug2->Push( moText("Pose ") + moText(strPose) + moText(" detected for user: ") + IntToStr(nId));

	m_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
	m_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}
// Callback: Started calibration
void UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie)
{
    //moAbstract::MODebug2->Push( moText("Calibration started for user:") + IntToStr(nId));
	//printf("Calibration started for user %d\n", nId);
}
// Callback: Finished calibration
void UserCalibration_CalibrationEnd(xn::SkeletonCapability& capability, XnUserID nId, XnBool bSuccess, void* pCookie)
{
	if (bSuccess)
	{
		// Calibration succeeded
		//moAbstract::MODebug2->Push( moText("Calibration complete, start tracking user:") + IntToStr(nId));
		//printf("Calibration complete, start tracking user %d\n", nId);
		m_UserGenerator.GetSkeletonCap().StartTracking(nId);
	}
	else
	{
		// Calibration failed
		//printf("Calibration failed for user %d\n", nId);
		//moAbstract::MODebug2->Push( moText("Calibration failed for user:") + IntToStr(nId));
		if (m_bNeedPose)
		{
			m_UserGenerator.GetPoseDetectionCap().StartPoseDetection(m_strPose, nId);
		}
		else
		{
			m_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
		}
	}
}

void UserCalibration_CalibrationComplete(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus, void* pCookie)
{
	if (eStatus == XN_CALIBRATION_STATUS_OK)
	{
		// Calibration succeeded
		//printf("Calibration complete, start tracking user %d\n", nId);
		//moAbstract::MODebug2->Push( moText("Calibration complete, start tracking user:") + IntToStr(nId));
		m_UserGenerator.GetSkeletonCap().StartTracking(nId);
	}
	else
	{
		// Calibration failed
		//printf("Calibration failed for user %d\n", nId);
		//moAbstract::MODebug2->Push( moText("Calibration failed for user:") + IntToStr(nId));
		if (m_bNeedPose)
		{
			m_UserGenerator.GetPoseDetectionCap().StartPoseDetection(m_strPose, nId);
		}
		else
		{
			m_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
		}
	}
}

// Save calibration to file
void SaveCalibration()
{
	XnUserID aUserIDs[20] = {0};
	XnUInt16 nUsers = 20;
	m_UserGenerator.GetUsers(aUserIDs, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
		// Find a user who is already calibrated
		if (m_UserGenerator.GetSkeletonCap().IsCalibrated(aUserIDs[i]))
		{
			// Save user's calibration to file
			m_UserGenerator.GetSkeletonCap().SaveCalibrationDataToFile(aUserIDs[i], XN_CALIBRATION_FILE_NAME);
			break;
		}
	}
}
// Load calibration from file
void LoadCalibration()
{
	XnUserID aUserIDs[20] = {0};
	XnUInt16 nUsers = 20;
	m_UserGenerator.GetUsers(aUserIDs, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
		// Find a user who isn't calibrated or currently in pose
		if (m_UserGenerator.GetSkeletonCap().IsCalibrated(aUserIDs[i])) continue;
		if (m_UserGenerator.GetSkeletonCap().IsCalibrating(aUserIDs[i])) continue;

		// Load user's calibration from file
		XnStatus rc = m_UserGenerator.GetSkeletonCap().LoadCalibrationDataFromFile(aUserIDs[i], XN_CALIBRATION_FILE_NAME);
		if (rc == XN_STATUS_OK)
		{
			// Make sure state is coherent
			m_UserGenerator.GetPoseDetectionCap().StopPoseDetection(aUserIDs[i]);
			m_UserGenerator.GetSkeletonCap().StartTracking(aUserIDs[i]);
		}
		break;
	}
}


void
moKinectPosture::Reset() {
    m_PostureVectorOld = m_PostureVector;
    m_PostureVector.Empty();
    m_PostureVector.Init( MO_KIN_MAX + 1, moVector3f(0.0f, 0.0f, 0.0f) );
}

void
moKinectPosture::Calculate( XnUserID player ) {

    XnSkeletonJointPosition joint;
    XnSkeletonJoint eJoint;
    XnPoint3D pt;

    moVector3f VectorElement;
    moVector3f VectorElementNeck;
    moVector3f JointPosition;
    moVector3f JointPositionLeftHand;
    moVector3f JointPositionRightHand;

    moVector3f JointPositionRightShoulder;
    moVector3f JointPositionLeftShoulder;
    moVector3f VectorElementShoulderAngle;
    moMathf    mathFloat;

    //MODebug2->Push( "player: " + IntToStr(player) );

    //save old...


    m_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, XN_SKEL_NECK, joint);
    VectorElementNeck = moVector3f( joint.position.X, joint.position.Y, joint.position.Z );
    //MODebug2->Push( "VectorElementNeck: X: " + FloatToStr(VectorElementNeck.X()) + " Y: " + FloatToStr(VectorElementNeck.Y()) );


///====== POSITION ===========

    m_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, XN_SKEL_LEFT_HAND, joint);
    JointPositionLeftHand = moVector3f( joint.position.X, joint.position.Y, joint.position.Z );
    JointPosition = JointPositionLeftHand - VectorElementNeck;
    m_PostureVector.Set( MO_KIN_REL_HAND_LEFT,  JointPosition );
    //MODebug2->Push( "JointPositionLeftHand: X: " + FloatToStr(JointPosition.X()) + " Y: " + FloatToStr(JointPosition.Y()) );
    //MODebug2->Push( "m_PostureVector[MO_KIN_REL_HAND_LEFT]: X: " + FloatToStr(m_PostureVector[MO_KIN_REL_HAND_LEFT].X()) + " Y: " + FloatToStr(m_PostureVector[MO_KIN_REL_HAND_LEFT].Y()) );

    m_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, XN_SKEL_RIGHT_HAND, joint);
    JointPositionRightHand = moVector3f( joint.position.X, joint.position.Y, joint.position.Z );
    JointPosition = JointPositionRightHand - VectorElementNeck;
    m_PostureVector.Set( MO_KIN_REL_HAND_RIGHT, JointPosition);
    //MODebug2->Push( "JointPositionRightHand: X: " + FloatToStr(JointPositionRightHand.X()) + " Y: " + FloatToStr(JointPositionRightHand.Y()) );

    m_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, XN_SKEL_RIGHT_SHOULDER, joint);
    JointPositionRightShoulder = moVector3f( joint.position.X, joint.position.Y, joint.position.Z );
    JointPosition = JointPositionRightShoulder - VectorElementNeck;
    m_PostureVector.Set( MO_KIN_REL_SHOULDER_RIGHT, JointPosition);
    //MODebug2->Push( "JointPositionRightHand: X: " + FloatToStr(JointPositionRightHand.X()) + " Y: " + FloatToStr(JointPositionRightHand.Y()) );

    m_UserGenerator.GetSkeletonCap().GetSkeletonJointPosition(player, XN_SKEL_LEFT_SHOULDER, joint);
    JointPositionLeftShoulder = moVector3f( joint.position.X, joint.position.Y, joint.position.Z );
    JointPosition = JointPositionLeftShoulder - VectorElementNeck;
    m_PostureVector.Set( MO_KIN_REL_SHOULDER_LEFT, JointPosition);
    //MODebug2->Push( "JointPositionRightHand: X: " + FloatToStr(JointPositionRightHand.X()) + " Y: " + FloatToStr(JointPositionRightHand.Y()) );


///====== VECTORS ==============

    VectorElement = JointPositionRightShoulder - JointPositionLeftShoulder;
    float mSign = mathFloat.Sign( VectorElement.Y() );
    float angleShoulders = mSign * VectorElement.Angle( moVector3f( 1.0f, 0.0f, 0.0f ) );
    m_PostureVector.Set( MO_KIN_VECTOR_SHOULDERS, VectorElement);

    VectorElement = JointPositionRightHand - JointPositionLeftHand;
    mSign = mathFloat.Sign( VectorElement.Y() );
    float angleHands = mSign*VectorElement.Angle( moVector3f( 1.0f, 0.0f, 0.0f ) );
    m_PostureVector.Set( MO_KIN_VECTOR_SHOULDERS, VectorElement);

///====== ANGLES ==============

    m_PostureVector.Set( MO_KIN_ANGLE_SHOULDERS, moVector3f( angleShoulders, 0.0, 0.0 ) );
    m_PostureVector.Set( MO_KIN_ANGLE_HANDS,  moVector3f( angleHands, 0.0, 0.0 ));


///====== DISTANCES ===========

    //absolute distance between hands
    JointPosition = JointPositionLeftHand - JointPositionRightHand;
    //se podria agregar el valor absoluto...
    JointPosition = moVector3f( JointPosition.Length(), 0.0f, 0.0f);
    m_PostureVector.Set( MO_KIN_DIST_ABS_INTER_HANDS, JointPosition );
    //MODebug2->Push( "InterHands: Lenght: " + FloatToStr(JointPosition.X()));
    //MODebug2->Push( "InterHands: Lenght: " + FloatToStr(m_PostureVector[MO_KIN_DIST_ABS_INTER_HANDS].X()) );

    JointPosition = JointPositionLeftHand - VectorElementNeck;
    JointPosition = moVector3f( JointPosition.Length(), 0.0f, 0.0f);
    m_PostureVector.Set( MO_KIN_DIST_ABS_HAND_LEFT_REL, JointPosition);

    JointPosition = JointPositionRightHand - VectorElementNeck;
    JointPosition = moVector3f( JointPosition.Length(), 0.0f, 0.0f);
    m_PostureVector.Set(  MO_KIN_DIST_ABS_HAND_RIGHT_REL, JointPosition );



    JointPosition = JointPositionLeftHand - JointPositionLeftShoulder;
    JointPosition = moVector3f( JointPosition.Length(), 0.0f, 0.0f);
    m_PostureVector.Set( MO_KIN_DIST_ABS_HAND_LEFT_SHOULDER_LEFT, JointPosition );

    JointPosition = JointPositionLeftHand - JointPositionRightShoulder;
    JointPosition = moVector3f( JointPosition.Length(), 0.0f, 0.0f);
    m_PostureVector.Set( MO_KIN_DIST_ABS_HAND_LEFT_SHOULDER_RIGHT, JointPosition );

    JointPosition = JointPositionRightHand - JointPositionRightShoulder;
    JointPosition = moVector3f( JointPosition.Length(), 0.0f, 0.0f);
    m_PostureVector.Set( MO_KIN_DIST_ABS_HAND_RIGHT_SHOULDER_RIGHT, JointPosition );

    JointPosition = JointPositionRightHand - JointPositionLeftShoulder;
    JointPosition = moVector3f( JointPosition.Length(), 0.0f, 0.0f);
    m_PostureVector.Set( MO_KIN_DIST_ABS_HAND_RIGHT_SHOULDER_LEFT, JointPosition );

///====== SPEED ===========
    float prop_horizontal = 0.0f;
    float prop_vertical = 0.0f;

    //SPEED AND PROP LEFT
    JointPosition = m_PostureVector[MO_KIN_REL_HAND_LEFT] - m_PostureVectorOld[MO_KIN_REL_HAND_LEFT];
    if (JointPosition.Y()==0.0f) { prop_horizontal = fabs(JointPosition.X()); prop_vertical = 0.0f; }
    if (JointPosition.X()==0.0f) { prop_horizontal = 0.0f; prop_vertical = fabs(JointPosition.Y()); }
    if (JointPosition.Y()!=0.0f && JointPosition.X()!=0.0f) {
      prop_horizontal = fabs(JointPosition.X() / JointPosition.Y());
      prop_vertical = fabs(JointPosition.Y() / JointPosition.X());
    }
    JointPosition = moVector3f( JointPosition.Length(), JointPosition.X(), JointPosition.Y());
    m_PostureVector.Set( MO_KIN_SPEED_ABS_HAND_LEFT, JointPosition );
    JointPosition = moVector3f( prop_horizontal, prop_vertical, 0.0f );
    m_PostureVector.Set( MO_KIN_SPEED_PROP_HAND_LEFT, JointPosition );

    //SPEED AND PROP RIGHT
    JointPosition = m_PostureVector[MO_KIN_REL_HAND_RIGHT] - m_PostureVectorOld[MO_KIN_REL_HAND_RIGHT];
    if (JointPosition.Y()==0.0f) { prop_horizontal = fabs(JointPosition.X()); prop_vertical = 0.0f; }
    if (JointPosition.X()==0.0f) { prop_horizontal = 0.0f; prop_vertical = fabs(JointPosition.Y()); }
    if (JointPosition.Y()!=0.0f && JointPosition.X()!=0.0f) {
        prop_horizontal = fabs(JointPosition.X() / JointPosition.Y());
        prop_vertical = fabs(JointPosition.Y() / JointPosition.X());
    }
    JointPosition = moVector3f( JointPosition.Length(), JointPosition.X(), JointPosition.Y());
    m_PostureVector.Set( MO_KIN_SPEED_ABS_HAND_RIGHT , JointPosition);
    JointPosition = moVector3f( prop_horizontal, prop_vertical, 0.0f );
    m_PostureVector.Set( MO_KIN_SPEED_PROP_HAND_RIGHT , JointPosition);


}



#endif



