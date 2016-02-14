/*******************************************************************************

                                moEffectCamera.cpp

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
  Andres Colubri

*******************************************************************************/

#include "moEffectCamera.h"
/**
GList*
gst_camera_capturer_enum_devices(gchar* device_name)
{
  GstElement* device;
  GstPropertyProbe* probe;
  GValueArray* va;
  GList* list=NULL;
  guint i=0;

  device = gst_element_factory_make (device_name, "source");
  gst_element_set_state(device, GST_STATE_READY);
  gst_element_get_state(device, NULL, NULL, 5 * GST_SECOND);
  if (!device || !GST_IS_PROPERTY_PROBE(device))
    goto finish;
  probe = GST_PROPERTY_PROBE (device);
  va = gst_property_probe_get_values_name (probe, "device-name");
  if (!va)
    goto finish;
  for(i=0; i < va->n_values; ++i) {
    GValue* v = g_value_array_get_nth(va, i);
    list = g_list_append(list, g_string_new(g_value_get_string(v)));
  }
  g_value_array_free(va);

finish:
  {
    gst_element_set_state (device, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT (device));
    return list;
  }
}
*/
//========================
//  Factory
//========================

moEffectCameraFactory *m_EffectCameraFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectCameraFactory==NULL)
		m_EffectCameraFactory = new moEffectCameraFactory();
	return(moEffectFactory*) m_EffectCameraFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectCameraFactory;
	m_EffectCameraFactory = NULL;
}

moEffect*  moEffectCameraFactory::Create() {
	return new moEffectCamera();
}

void moEffectCameraFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectCamera::moEffectCamera() {
	SetName("camera");

	m_pAnim = NULL;
	m_pCamera = NULL;
	m_pCameraGraph = NULL;
	m_pCameraTexture = NULL;
}

moEffectCamera::~moEffectCamera() {
	Finish();
}


moConfigDefinition *
moEffectCamera::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );

  moText camOptions = "default";
  /*
  moTextArray cams = m_pResourceManager->GetVideoMan()->GetCameraNames();
  for( int c=0; c<cams.Count(); c++) {
    camOptions+= ","+cams[c];
  }
*/
	p_configdefinition->Add( moText("camera"), MO_PARAM_TEXT, (int)CAMERA_CAMERA, moValue("default","TXT").Ref(), camOptions );

	p_configdefinition->Add( moText("color_format"), MO_PARAM_NUMERIC, (MOint)CAMERA_COLOR_FORMAT /* 0 : RGB , 1:YUV */, moValue("0","NUM").Ref(), moText("rgb,yuv") );
	p_configdefinition->Add( moText("width"), MO_PARAM_NUMERIC, CAMERA_HEIGHT, moValue("0","NUM").Ref() );
	p_configdefinition->Add( moText("height"), MO_PARAM_NUMERIC, CAMERA_WIDTH, moValue("0","NUM").Ref() );

	p_configdefinition->Add( moText("color_bits"), MO_PARAM_NUMERIC, CAMERA_COLOR_BITS /*24*/, moValue("24","NUM").Ref(), "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32" );
	p_configdefinition->Add( moText("scale_width"), MO_PARAM_NUMERIC, CAMERA_SCALE_WIDTH, moValue("0","NUM").Ref() );
	p_configdefinition->Add( moText("scale_height"), MO_PARAM_NUMERIC, CAMERA_SCALE_HEIGHT, moValue("0","NUM").Ref() );
	p_configdefinition->Add( moText("flip_horizontal"), MO_PARAM_NUMERIC, CAMERA_FLIP_HORIZONTAL, moValue("0","NUM").Ref() );
	p_configdefinition->Add( moText("flip_vertical"), MO_PARAM_NUMERIC, CAMERA_FLIP_VERTICAL, moValue("0","NUM").Ref() );

	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, CAMERA_TEXTURE, moValue("default", "TXT" ) );

	p_configdefinition->Add( moText("position"), MO_PARAM_FUNCTION, CAMERA_POSITION, moValue( "0.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("speed"), MO_PARAM_FUNCTION, CAMERA_SPEED, moValue( "1.0","FUNCTION").Ref() );
  p_configdefinition->Add( moText("volume"), MO_PARAM_FUNCTION, CAMERA_VOLUME, moValue( "1.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("balance"), MO_PARAM_FUNCTION, CAMERA_BALANCE, moValue( "0.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("brightness"), MO_PARAM_FUNCTION, CAMERA_BRIGHTNESS, moValue( "0.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("contrast"), MO_PARAM_FUNCTION, CAMERA_CONTRAST, moValue( "1.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("saturation"), MO_PARAM_FUNCTION, CAMERA_SATURATION, moValue( "1.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("hue"), MO_PARAM_FUNCTION, CAMERA_HUE, moValue( "0.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("cameramode"), MO_PARAM_NUMERIC, CAMERA_MODE, moValue( "0","NUM").Ref() );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, CAMERA_BLENDING);
	p_configdefinition->Add( moText("startplaying"), MO_PARAM_NUMERIC, CAMERA_STARTPLAYING, moValue( "0","NUM").Ref() );
	p_configdefinition->Add( moText("loop"), MO_PARAM_NUMERIC, CAMERA_LOOP, moValue( "0","NUM").Ref() );
	p_configdefinition->Add( moText("interpolation"), MO_PARAM_NUMERIC, CAMERA_INTERPOLATION, moValue( "0","NUM").Ref() );
	p_configdefinition->Add( moText("pos_text_x"), MO_PARAM_FUNCTION, CAMERA_POSTEXX, moValue( "0.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("pos_text_y"), MO_PARAM_FUNCTION, CAMERA_POSTEXY, moValue( "0.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("anc_text_x"), MO_PARAM_FUNCTION, CAMERA_ANCTEXX, moValue( "1.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("alt_text_y"), MO_PARAM_FUNCTION, CAMERA_ALTTEXY, moValue( "1.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("pos_cuad_x"), MO_PARAM_FUNCTION, CAMERA_POSCUADX, moValue( "0.0","FUNCTION").Ref()  );
	p_configdefinition->Add( moText("pos_cuad_y"), MO_PARAM_FUNCTION, CAMERA_POSCUADY, moValue( "0.0","FUNCTION").Ref()  );
	p_configdefinition->Add( moText("anc_cuad_x"), MO_PARAM_FUNCTION, CAMERA_ANCCUADX, moValue( "1.0","FUNCTION").Ref()  );
	p_configdefinition->Add( moText("alt_cuad_y"), MO_PARAM_FUNCTION, CAMERA_ALTCUADY, moValue( "1.0","FUNCTION").Ref()  );

	p_configdefinition->Add( moText("showcameradata"), MO_PARAM_NUMERIC, CAMERA_SHOWCAMERADATA, moValue( "0","NUM").Ref() );
	p_configdefinition->Add( moText("display_x"), MO_PARAM_FUNCTION, CAMERA_DISPLAY_X, moValue( "-0.5","FUNCTION").Ref()  );
	p_configdefinition->Add( moText("display_y"), MO_PARAM_FUNCTION, CAMERA_DISPLAY_Y, moValue( "-0.35","FUNCTION").Ref()  );
	p_configdefinition->Add( moText("display_width"), MO_PARAM_FUNCTION, CAMERA_DISPLAY_WIDTH, moValue( "1.0","FUNCTION").Ref()  );
	p_configdefinition->Add( moText("display_height"), MO_PARAM_FUNCTION, CAMERA_DISPLAY_HEIGHT, moValue( "0.05","FUNCTION").Ref()  );

	return p_configdefinition;
}


MOboolean
moEffectCamera::Init() {

  if (!PreInit()) return false;

  moDefineParamIndex( CAMERA_INLET, moText("inlet") );
  moDefineParamIndex( CAMERA_OUTLET, moText("outlet") );
  // Hacer la inicializacion de este efecto en particular.
  moDefineParamIndex( CAMERA_ALPHA, moText("alpha") );
  moDefineParamIndex( CAMERA_COLOR, moText("color") );
  moDefineParamIndex( CAMERA_SYNC, moText("syncro") );
  //moDefineParamIndex( CAMERA_SCRIPT, moText("script") );
  moDefineParamIndex( CAMERA_CAMERA, moText("camera") );

  moDefineParamIndex( CAMERA_COLOR_FORMAT, moText("color_format") );
  moDefineParamIndex( CAMERA_WIDTH, moText("width") );
  moDefineParamIndex( CAMERA_HEIGHT, moText("height") );
  moDefineParamIndex( CAMERA_COLOR_BITS, moText("color_bits") );
  moDefineParamIndex( CAMERA_SCALE_WIDTH, moText("scale_width") );
  moDefineParamIndex( CAMERA_SCALE_HEIGHT, moText("scale_height") );
  moDefineParamIndex( CAMERA_FLIP_HORIZONTAL, moText("flip_horizontal") );
  moDefineParamIndex( CAMERA_FLIP_VERTICAL, moText("flip_vertical") );

  moDefineParamIndex( CAMERA_TEXTURE, moText("texture") );

  moDefineParamIndex( CAMERA_MODE, moText("cameramode") );
  moDefineParamIndex( CAMERA_BLENDING, moText("blending") );
  moDefineParamIndex( CAMERA_STARTPLAYING, moText("startplaying") );
  moDefineParamIndex( CAMERA_LOOP, moText("loop") );

  moDefineParamIndex( CAMERA_POSITION, moText("position") );
  moDefineParamIndex( CAMERA_SPEED, moText("speed") );
  moDefineParamIndex( CAMERA_VOLUME, moText("volume") );
  moDefineParamIndex( CAMERA_BALANCE, moText("balance") );
  moDefineParamIndex( CAMERA_BRIGHTNESS, moText("brightness") );
  moDefineParamIndex( CAMERA_CONTRAST, moText("contrast") );
  moDefineParamIndex( CAMERA_SATURATION, moText("saturation") );
  moDefineParamIndex( CAMERA_HUE, moText("hue") );


  moDefineParamIndex( CAMERA_SHOWCAMERADATA, moText("showcameradata") );
  moDefineParamIndex( CAMERA_INTERPOLATION, moText("interpolation") );
  moDefineParamIndex( CAMERA_POSTEXX, moText("pos_text_x") );
  moDefineParamIndex( CAMERA_POSTEXY, moText("pos_text_y") );
  moDefineParamIndex( CAMERA_ANCTEXX, moText("anc_text_x") );
  moDefineParamIndex( CAMERA_ALTTEXY, moText("alt_text_y") );
  moDefineParamIndex( CAMERA_POSCUADX, moText("pos_cuad_x") );
  moDefineParamIndex( CAMERA_POSCUADY, moText("pos_cuad_y") );
  moDefineParamIndex( CAMERA_ANCCUADX, moText("anc_cuad_x") );
  moDefineParamIndex( CAMERA_ALTCUADY, moText("alt_cuad_y") );

  moDefineParamIndex( CAMERA_DISPLAY_X, moText("display_x") );
  moDefineParamIndex( CAMERA_DISPLAY_Y, moText("display_y") );
  moDefineParamIndex( CAMERA_DISPLAY_WIDTH, moText("display_width") );
  moDefineParamIndex( CAMERA_DISPLAY_HEIGHT, moText("display_height") );

	m_bDisplayOn = false;

	showcameradata = true; /** Start timer!! to shut down display info*/

/*
  m_DeviceName = m_Config.Text(moR(CAMERA_CAMERA));
  InitDevice(m_DeviceName);
*/
  UpdateCamera();
	return true;
}


bool
moEffectCamera::CheckIfDeviceNameExists( const moText& camera ) {
  /**GET A LIST OF DEVICES*/
  MODebug2->Message("moEffectCamera::CheckIfDeviceNameExists...");
  moVideoManager* VMan = m_pResourceManager->GetVideoMan();


  MODebug2->Message("moEffectCamera::CheckIfDeviceNameExists > ...GetCaptureDevices(true) ");
  CapDevs = VMan->GetCaptureDevices(true);
  MODebug2->Message("moEffectCamera::CheckIfDeviceNameExists > ...GetCaptureDevices(true) OK !!!");
  moCamera* Camera = NULL;
  bool founded = false;
  if (VMan->GetCameraCount()>0) {
    MODebug2->Message("moEffectCamera::CheckIfDeviceNameExists > At least we found"+IntToStr(VMan->GetCameraCount())+". Great!");
    for(  int c=0;c<VMan->GetCameraCount(); c++ ) {
      Camera = VMan->GetCamera(c);

      if (Camera) {
        if (Camera->GetCaptureDevice().GetName()==camera
            ||
            Camera->GetCaptureDevice().GetLabelName()==camera ) {
          founded = true;
        }
        else {
          MODebug2->Message( "Camera Device: " + Camera->GetCaptureDevice().GetName() );
        }

      }
    }
  }
  if (founded) return true;

  if (camera=="default" && VMan->GetCameraCount() > 0 ) {
    MODebug2->Message("moEffectCamera::CheckIfDeviceNameExists > default selected, at least one camera device is loaded.");
    return true;

  }

  if (camera=="default" && CapDevs.Count()>0 ) {
    moCaptureDevice Cap = CapDevs.Get(0);
    if (Cap.IsPresent()) {
      MODebug2->Message("moEffectCamera::CheckIfDeviceNameExists > default selected, at least one camera device is available. Cap.: "
                    +Cap.GetLabelName());
    } else {
      MODebug2->Message("moEffectCamera::CheckIfDeviceNameExists > default selected, available but not present?");
    }
    return true;
  }

  return false;
}

bool
moEffectCamera::InitDevice( moText camera ) {

  moVideoManager* VMan = m_pResourceManager->GetVideoMan();

  if ( CheckIfDeviceNameExists(camera) ) {
    MODebug2->Message("moEffectCamera::InitDevice > camera found: " + camera );
    /** ITS IN THE LIST given by VideoMan */
    m_DeviceName = camera;

    /** now check if we can get the Camera... */
    m_pCamera = VMan->GetCameraByName( m_DeviceName, (bool)true /*LOAD!*/, m_CaptureDevice );
    if (m_pCamera) {
      m_pCameraTexture = m_pCamera->GetTexture();
      m_pCameraGraph = m_pCamera->GetVideoGraph();
      MODebug2->Message("moEffectCamera::InitDevice > Camera: " + m_pCamera->GetDeviceName() );

    }

  } else {
    MODebug2->Error("moEffectCamera::InitDevice > camera device not found: " + camera );
    m_DeviceName = camera;
  }

}

void moEffectCamera::UpdateCamera() {

  moText camera_v = m_Config.Text(moR(CAMERA_CAMERA));

  moVideoFormat VF;
  int srcw = m_Config.Int(moR( CAMERA_WIDTH ));
  int srch = m_Config.Int(moR( CAMERA_HEIGHT ));
  int srcbpp = m_Config.Int(moR( CAMERA_COLOR_BITS ));
  int flH = m_Config.Int(moR( CAMERA_FLIP_HORIZONTAL ));
  int flV = m_Config.Int(moR( CAMERA_FLIP_VERTICAL ));

  moCaptureDevice CD( "custom", moText(""), moText(""), 0, srcw, srch, srcbpp, flH, flV );

  CD.SetLabelName( camera_v );

  VF.m_Width = m_Config.Int( moR( CAMERA_SCALE_WIDTH ) );
  VF.m_Height = m_Config.Int( moR( CAMERA_SCALE_HEIGHT ) );
  VF.m_BitCount = m_Config.Int( moR( CAMERA_COLOR_BITS ) );

  VF.m_ColorMode = (moColorMode) m_Config.Int( moR( CAMERA_COLOR_FORMAT ) );

  CD.SetVideoFormat( VF );

  if ( m_CaptureDevice.GetVideoFormat().m_ColorMode!=CD.GetVideoFormat().m_ColorMode
      ||
      m_CaptureDevice.GetVideoFormat().m_Width!=CD.GetVideoFormat().m_Width
      ||
      m_CaptureDevice.GetVideoFormat().m_Height!=CD.GetVideoFormat().m_Height
      ||
      m_CaptureDevice.GetSourceWidth()!=CD.GetSourceWidth()
      ||
      m_CaptureDevice.GetSourceHeight()!=CD.GetSourceHeight()
      ) {
    //RELOAD CAMERA
    m_CaptureDevice = CD;
    m_DeviceName = "";
  }


  /**
  chequeamos si hubo un cambio de camara:

  */
  if ( m_DeviceName == camera_v ) {
    ///just check the device is on and transmitting
  } else {
    if ( InitDevice( camera_v ) ) {

    }
  }


}

void moEffectCamera::UpdateParameters() {

  float Volume,Balance,Brightness,Contrast,Saturation,Hue;
  long  UserPosition;
  moTexture *pTexture;
  bool bReBalance = false;

  UpdateCamera();

  ///user position, convert from function to long...
  UserPosition = (long)m_Config.Eval(moR(CAMERA_POSITION));

  ///sound balance
  Volume = m_Config.Eval(moR(CAMERA_VOLUME));
  Balance = m_Config.Eval(moR(CAMERA_BALANCE));

  ///video balance
  Brightness = m_Config.Eval(moR(CAMERA_BRIGHTNESS));
  Contrast = m_Config.Eval(moR(CAMERA_CONTRAST));
  Saturation = m_Config.Eval(moR(CAMERA_SATURATION));
  Hue = m_Config.Eval(moR(CAMERA_HUE));

	m_bInterpolation = m_Config.Int(moR(CAMERA_INTERPOLATION));

	//pTexture = m_Config[moR(CAMERA_TEXTURE)][MO_SELECTED][0].Texture();


  /**
  *
  *
  *     BALANCES DE VOLUMEN Y CONTRASTE
  *       salto de posicion por el usuario
  *
  */
  if (m_pCameraGraph) {

      if (Volume!=m_Volume || bReBalance) { m_Volume=Volume; m_pCameraGraph->SetVolume(m_Volume); }
      if (Balance!=m_Balance || bReBalance) { m_Balance=Balance; m_pCameraGraph->SetBalance(m_Balance);}

      if (Brightness!=m_Brightness || bReBalance) { m_Brightness=Brightness; m_pCameraGraph->SetBrightness(m_Brightness);}
      if (Contrast!=m_Contrast || bReBalance) { m_Contrast=Contrast; m_pCameraGraph->SetContrast(m_Contrast);}
      if (Saturation!=m_Saturation || bReBalance) { m_Saturation=Saturation; m_pCameraGraph->SetSaturation(m_Saturation);}
      if (Hue!=m_Hue || bReBalance) { m_Hue=Hue; m_pCameraGraph->SetHue(m_Hue);}
      bReBalance = false;
  }

}

void moEffectCamera::Draw( moTempo* tempogral, moEffectState* parentstate )
{
    MOuint FrameGLid;

    MOdouble PosTextX,  AncTextX,  PosTextY,  AltTextY;
    MOdouble PosTextX0, PosTextX1, PosTextY0, PosTextY1;
    MOdouble PosCuadX,  AncCuadX,  PosCuadY,  AltCuadY;
    MOdouble PosCuadX0, PosCuadX1, PosCuadY0, PosCuadY1;

    moRenderManager* mRender = m_pResourceManager->GetRenderMan();
    moGLManager* mGL = m_pResourceManager->GetGLMan();
    moTextureManager* mT = m_pResourceManager->GetTextureMan();

    if (mRender==NULL || mGL==NULL) return;

    int w = mRender->ScreenWidth();
    int h = mRender->ScreenHeight();

    UpdateParameters();

    PreDraw( tempogral, parentstate);

#ifndef OPENGLESV2
  // Guardar y resetar la matriz de vista del modelo //
  glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
  glLoadIdentity();									// Reset The View
  // Cambiar la proyeccion para una vista ortogonal //
  glDisable(GL_DEPTH_TEST);       // Disables Depth Testing
  glEnable(GL_ALPHA);
 //m_pResourceManager->GetGLMan()->SetOrthographicView();
#endif
	cameramode = m_Config.Int(moR(CAMERA_MODE));

  //SetColor( m_Config[moR(CAMERA_COLOR)][MO_SELECTED], m_Config[moR(CAMERA_ALPHA)][MO_SELECTED], state );
  SetColor( m_Config[moR(CAMERA_COLOR)], m_Config[moR(CAMERA_ALPHA)], m_EffectState );

  SetBlending( (moBlendingModes) m_Config.Int( moR(CAMERA_BLENDING) ) );

	//aquí debemos modificar Images.Get(Image,&m_EffectState.tempo) para los distintos modos de reproduccion
	//0: PLAY NORMAL 1X [25 fps o 29.97]
	//1: FF 2X []
	//2: RW -2X []
	//3: FFF 8X []
	//4: RRW -8X []
	//5: STOP []
	//6: SEEK( position )

  //VCRPlaySpeed();
  if (m_pAnim) {
//    if (m_pAnim->IsPlaying())
  //    MODebug2->Push(" FramePosition: "+IntToStr(m_FramePosition) + " FPS: " + FloatToStr(m_FramesPerSecond) +" TimeCode: " + moVideoManager::FramesToTimecode(m_FramePosition,m_FramesPerSecond) );
  }

	PosTextX = m_Config.Eval(moR(CAMERA_POSTEXX));
  AncTextX = m_Config.Eval(moR(CAMERA_ANCTEXX));
	PosTextY = m_Config.Eval(moR(CAMERA_POSTEXY));
  AltTextY = m_Config.Eval(moR(CAMERA_ALTTEXY));

	PosCuadX = m_Config.Eval(moR(CAMERA_POSCUADX));
  AncCuadX = m_Config.Eval(moR(CAMERA_ANCCUADX));
	PosCuadY = m_Config.Eval(moR(CAMERA_POSCUADY));
  AltCuadY = m_Config.Eval(moR(CAMERA_ALTCUADY));

	if (m_pCameraTexture) {
		PosTextX0 = PosTextX * m_pCameraTexture->GetMaxCoordS();
		PosTextX1 =(PosTextX + AncTextX)* m_pCameraTexture->GetMaxCoordS();
		PosTextY0 =(1 - PosTextY)*m_pCameraTexture->GetMaxCoordT();
		PosTextY1 =(1 - PosTextY - AltTextY)*m_pCameraTexture->GetMaxCoordT();
	} else {
		PosTextX0 = PosTextX;
		PosTextX1 =(PosTextX + AncTextX);
		PosTextY0 =(1 - PosTextY);
		PosTextY1 =(1 - PosTextY - AltTextY);
	}

	PosCuadX0 = PosCuadX-AncCuadX/2;
	PosCuadX1 = PosCuadX + AncCuadX/2;
	PosCuadY1 = PosCuadY - AltCuadY/2;
	PosCuadY0 = PosCuadY + AltCuadY/2;

    moPlaneGeometry ImageQuad( AncCuadX, AltCuadY, 1, 1 );
    moMaterial Material;
    if (m_pCameraTexture) {
        Material.m_Map = m_pCameraTexture;
        
    } else {
        Material.m_Map = mT->GetTexture( mT->GetTextureMOId( "default", false ) );
    }
    
    if (Material.m_Map) {
      Material.m_MapGLId = Material.m_Map->GetGLId();
    } else Material.m_MapGLId = 0;
    //Material.m_Color = moColor( 1.0, 1.0, 1.0 );
    moVector4d color = m_Config.EvalColor( moR(CAMERA_COLOR) );
    Material.m_Color = moColor( color.X(), color.Y(), color.Z() );

    moGLMatrixf Model;
    Model.MakeIdentity();
    Model.Translate( PosCuadX+AncCuadX/2, PosCuadY+AltCuadY/2, 0.0 );

    moMesh Mesh( ImageQuad, Material );
    Mesh.SetModelMatrix( Model );


    moCamera3D Camera3D;
    //mGL->SetDefaultOrthographicView( w, h );
    mGL->SetOrthographicView( w, h, 0.0, 1.0, 0.0, 1.0, -1.0, 1.0 );
    Camera3D = mGL->GetProjectionMatrix();
    //mRender->Render( &Mesh, &Camera3D );

#ifndef OPENGLESV2

  glEnable(GL_TEXTURE_2D);
  //glBindTexture(GL_TEXTURE_2D, m_Config.GetGLId(moR(CAMERA_TEXTURE), &m_EffectState.tempo ) );
glBindTexture(GL_TEXTURE_2D, Material.m_MapGLId );


	glBegin(GL_QUADS);
		glTexCoord2f( PosTextX0, PosTextY1);
		glVertex2f ( PosCuadX0+AncCuadX/2, PosCuadY0+AltCuadY/2);

		glTexCoord2f( PosTextX1, PosTextY1);
		glVertex2f ( PosCuadX1+AncCuadX/2, PosCuadY0+AltCuadY/2);

    glTexCoord2f( PosTextX1, PosTextY0);
		glVertex2f ( PosCuadX1+AncCuadX/2, PosCuadY1+AltCuadY/2);

		glTexCoord2f( PosTextX0, PosTextY0);
		glVertex2f ( PosCuadX0+AncCuadX/2, PosCuadY1+AltCuadY/2);
	glEnd();

#else
 mRender->Render( &Mesh, &Camera3D );
#endif



	showcameradata = m_Config.Int( moR(CAMERA_SHOWCAMERADATA) );

  m_DisplayX = m_Config.Eval( moR(CAMERA_DISPLAY_X) );
  m_DisplayY = m_Config.Eval( moR(CAMERA_DISPLAY_Y) );
  m_DisplayW = m_Config.Eval( moR(CAMERA_DISPLAY_WIDTH) );
  m_DisplayH = m_Config.Eval( moR(CAMERA_DISPLAY_HEIGHT) );

	if (showcameradata) {
		//DrawDisplayInfo( PosCuadX0, PosCuadY0 );
	}

  glEnable(GL_TEXTURE_2D);
  EndDraw();

#ifndef OPENGLESV2
    glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();
#endif
}


MOboolean moEffectCamera::Finish()
{

    if (m_pCameraGraph) {
        m_pCameraGraph->Stop();
        //m_pCamera = NULL;
    }
    PreFinish();
	return true;
}

void moEffectCamera::Interaction( moIODeviceManager *IODeviceManager ) {

}

void
moEffectCamera::Update( moEventList *Events ) {

	//get the pointer from the Moldeo Object sending it...
	moMoldeoObject::Update(Events);

}
