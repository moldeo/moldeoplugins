/*******************************************************************************

                                moEffectGrid.cpp

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

#include "moEffectGrid.h"

//========================
//  Factory
//========================

moEffectGridFactory *m_EffectGridFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectGridFactory==NULL)
		m_EffectGridFactory = new moEffectGridFactory();
	return(moEffectFactory*) m_EffectGridFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectGridFactory;
	m_EffectGridFactory = NULL;
}

moEffect*  moEffectGridFactory::Create() {
	return new moEffectGrid();
}

void moEffectGridFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectGrid::moEffectGrid()
{
	devicecode = NULL;
	ncodes = 0;
	SetName("grid");

    Grid = NULL;
    m_pGesture = NULL;
    m_pOutletCameraHeight = NULL;
    turnAngle = 0.0f;
	//glActiveTextureARB = NULL;
	//glMultiTexCoord2fARB = NULL;
	//Funciones de multitexture
        // wglGetProcAddress reemplazado por glutGetProcAddress(by Andres)
	#ifdef _WIN32
	//glActiveTextureARB		=(PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
	//glMultiTexCoord2fARB	=(PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
	#else
	//glActiveTextureARB		=(PFNGLACTIVETEXTUREARBPROC) glutGetProcAddress("glActiveTextureARB");
	//glMultiTexCoord2fARB	=(PFNGLMULTITEXCOORD2FARBPROC) glutGetProcAddress("glMultiTexCoord2fARB");
	#endif
}

moEffectGrid::~moEffectGrid()
{
	Finish();
}


moConfigDefinition *
moEffectGrid::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("shader"), MO_PARAM_TEXT, GRID_SHADER, moValue( "", MO_VALUE_TXT) );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, GRID_TEXTURE, moValue( "default", MO_VALUE_TXT) );
	p_configdefinition->Add( moText("textureb"), MO_PARAM_TEXTURE, GRID_TEXTUREB, moValue( "default", MO_VALUE_TXT) );
	p_configdefinition->Add( moText("texturec"), MO_PARAM_TEXTURE, GRID_TEXTUREC, moValue( "default", MO_VALUE_TXT) );
	p_configdefinition->Add( moText("textured"), MO_PARAM_TEXTURE, GRID_TEXTURED, moValue( "default", MO_VALUE_TXT) );
	p_configdefinition->Add( moText("texturee"), MO_PARAM_TEXTURE, GRID_TEXTUREE, moValue( "default", MO_VALUE_TXT) );
	p_configdefinition->Add( moText("texturef"), MO_PARAM_TEXTURE, GRID_TEXTUREF, moValue( "default", MO_VALUE_TXT) );
	p_configdefinition->Add( moText("textureg"), MO_PARAM_TEXTURE, GRID_TEXTUREG, moValue( "default", MO_VALUE_TXT) );
	p_configdefinition->Add( moText("textureh"), MO_PARAM_TEXTURE, GRID_TEXTUREH, moValue( "default", MO_VALUE_TXT) );

	p_configdefinition->Add( moText("map"), MO_PARAM_TEXT, GRID_MAP, moValue( "mapa/mapax.mp", MO_VALUE_TXT) );
  p_configdefinition->Add( moText("map_position_x"), MO_PARAM_FUNCTION, GRID_MAP_POSITION_X, moValue( "1.0", "FUNCTION").Ref() );
  p_configdefinition->Add( moText("map_position_y"), MO_PARAM_FUNCTION, GRID_MAP_POSITION_Y, moValue( "1.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("scales"), MO_PARAM_NUMERIC, GRID_SCALES, moValue( "12", MO_VALUE_NUM_INT) );
	p_configdefinition->Add( moText("size"), MO_PARAM_NUMERIC, GRID_SIZE, moValue( "45", MO_VALUE_NUM_INT) );
	p_configdefinition->Add( moText("wireframe"), MO_PARAM_NUMERIC, GRID_WIREFRAME, moValue( "0", MO_VALUE_NUM_INT) );
	p_configdefinition->Add( moText("wireframe_width"), MO_PARAM_FUNCTION, GRID_WIREFRAME_WIDTH, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("wireframe_configuration"), MO_PARAM_FUNCTION, GRID_WIREFRAME_CONFIGURATION, moValue( "0.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("height_multiply"), MO_PARAM_FUNCTION, GRID_HEIGHT_MULTIPLY, moValue( "1.5", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("height_randomizer"), MO_PARAM_FUNCTION, GRID_HEIGHT_RANDOMIZER, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("planet_factor"), MO_PARAM_FUNCTION, GRID_PLANET_FACTOR, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("torus_factor"), MO_PARAM_FUNCTION, GRID_TORUS_FACTOR, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("wave_amplitude"), MO_PARAM_FUNCTION, GRID_WAVE_AMPLITUDE, moValue( "0.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("control_lift_angle"), MO_PARAM_FUNCTION, GRID_CONTROL_LIFT_ANGLE, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("control_lift_force"), MO_PARAM_FUNCTION, GRID_CONTROL_LIFT_FORCE, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("control_roll_angle"), MO_PARAM_FUNCTION, GRID_CONTROL_ROLL_ANGLE, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("control_roll_force"), MO_PARAM_FUNCTION, GRID_CONTROL_ROLL_FORCE, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("control_speed"), MO_PARAM_FUNCTION, GRID_CONTROL_SPEED, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("control_speed_acceleration"), MO_PARAM_FUNCTION, GRID_CONTROL_SPEED_ACCELERATION, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("control_minimum_speed"), MO_PARAM_FUNCTION, GRID_CONTROL_MINIMUM_SPEED, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("control_maximum_speed"), MO_PARAM_FUNCTION, GRID_CONTROL_MAXIMUM_SPEED, moValue( "0.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("control_gravity_force"), MO_PARAM_FUNCTION, GRID_CONTROL_GRAVITY_FORCE, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("control_minimum_altitude"), MO_PARAM_FUNCTION, GRID_CONTROL_MINIMUM_ALTITUDE, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("control_maximum_altitude"), MO_PARAM_FUNCTION, GRID_CONTROL_MAXIMUM_ALTITUDE, moValue( "0.0", "FUNCTION").Ref() );
  p_configdefinition->Add( moText("control_minimum_surface_altitude"), MO_PARAM_FUNCTION, GRID_CONTROL_MINIMUM_SURFACE_ALTITUDE, moValue( "1000.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, GRID_TRANSLATEX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, GRID_TRANSLATEY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, GRID_TRANSLATEZ, moValue( "0.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, GRID_SCALEX, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, GRID_SCALEY, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, GRID_SCALEZ, moValue( "1.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("rotatex"), MO_PARAM_TRANSLATEX, GRID_ROTATEX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_TRANSLATEY, GRID_ROTATEY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_TRANSLATEZ, GRID_ROTATEZ, moValue( "0.0", "FUNCTION").Ref() );

	return p_configdefinition;
}

MOboolean moEffectGrid::Init()
{
    if (!PreInit()) return true;

    if (glActiveTextureARB) {
    } else {
      glewInit();
      //MODebug2->Message( moText("moEffectGrid::Init >       glActiveTextureARB: ") + moText(IntToStr((int)glActiveTextureARB)) );
      //MODebug2->Message( moText("moEffectGrid::Init >       glMultiTexCoord2fARB: ") + moText(IntToStr((int)glMultiTexCoord2fARB)) );
    }

    moDefineParamIndex( GRID_INLET, moText("inlet") );
    moDefineParamIndex( GRID_OUTLET, moText("outlet") );
    moDefineParamIndex( GRID_SCRIPT, moText("script") );
    moDefineParamIndex( GRID_ALPHA, moText("alpha") );
    moDefineParamIndex( GRID_COLOR, moText("color") );
    moDefineParamIndex( GRID_SYNC, moText("syncro") );
    moDefineParamIndex( GRID_PHASE, moText("phase") );
    moDefineParamIndex( GRID_SHADER, moText("shader") );

    moDefineParamIndex( GRID_TEXTURE, moText("texture") );
    moDefineParamIndex( GRID_TEXTUREB, moText("textureb") );
    moDefineParamIndex( GRID_TEXTUREC, moText("texturec") );
    moDefineParamIndex( GRID_TEXTURED, moText("textured") );
    moDefineParamIndex( GRID_TEXTUREE, moText("texturee") );
    moDefineParamIndex( GRID_TEXTUREF, moText("texturef") );
    moDefineParamIndex( GRID_TEXTUREG, moText("textureg") );
    moDefineParamIndex( GRID_TEXTUREH, moText("textureh") );

    moDefineParamIndex( GRID_MAP, moText("map") );
    moDefineParamIndex( GRID_MAP_POSITION_X, moText("map_position_x") );
    moDefineParamIndex( GRID_MAP_POSITION_Y, moText("map_position_y") );
    moDefineParamIndex( GRID_SCALES, moText("scales") );
    moDefineParamIndex( GRID_SIZE, moText("size") );
    moDefineParamIndex( GRID_WIREFRAME, moText("wireframe") );
    moDefineParamIndex( GRID_WIREFRAME_WIDTH, moText("wireframe_width") );
    moDefineParamIndex( GRID_WIREFRAME_CONFIGURATION, moText("wireframe_configuration") );

    moDefineParamIndex( GRID_HEIGHT_MULTIPLY, moText("height_multiply") );
    moDefineParamIndex( GRID_HEIGHT_RANDOMIZER, moText("height_randomizer") );
    moDefineParamIndex( GRID_PLANET_FACTOR, moText("planet_factor") );
    moDefineParamIndex( GRID_TORUS_FACTOR, moText("torus_factor") );
    moDefineParamIndex( GRID_WAVE_AMPLITUDE, moText("wave_amplitude") );

    moDefineParamIndex( GRID_CONTROL_ROLL_ANGLE, moText("control_roll_angle") );
    moDefineParamIndex( GRID_CONTROL_ROLL_FORCE, moText("control_roll_force") );
    moDefineParamIndex( GRID_CONTROL_LIFT_ANGLE, moText("control_lift_angle") );
    moDefineParamIndex( GRID_CONTROL_LIFT_FORCE, moText("control_lift_force") );

    moDefineParamIndex( GRID_CONTROL_SPEED, moText("control_speed") );
    moDefineParamIndex( GRID_CONTROL_SPEED_ACCELERATION, moText("control_speed_acceleration") );
    moDefineParamIndex( GRID_CONTROL_MINIMUM_SPEED, moText("control_minimum_speed") );
    moDefineParamIndex( GRID_CONTROL_MAXIMUM_SPEED, moText("control_maximum_speed") );

    moDefineParamIndex( GRID_CONTROL_GRAVITY_FORCE, moText("control_gravity_force") );
    moDefineParamIndex( GRID_CONTROL_MINIMUM_ALTITUDE, moText("control_minimum_altitude") );
    moDefineParamIndex( GRID_CONTROL_MAXIMUM_ALTITUDE, moText("control_maximum_altitude") );
    moDefineParamIndex( GRID_CONTROL_MINIMUM_SURFACE_ALTITUDE, moText("control_minimum_surface_altitude") );

    Grid = new TEngine_Utility( m_Config, m_pResourceManager );
    MOTextures = m_pResourceManager->GetTextureMan();
    //Start Engine
    if (Grid) Grid->Start_Engine();



	return true;
}

void moEffectGrid::UpdateParameters() {

  int wireframe = m_Config.Int(moR(GRID_WIREFRAME));
  double wireframe_configuration = m_Config.Eval(moR(GRID_WIREFRAME_CONFIGURATION));
  double wireframe_width = m_Config.Eval(moR(GRID_WIREFRAME_WIDTH));

  double height_multiply = m_Config.Eval(moR(GRID_HEIGHT_MULTIPLY));
  double height_randomizer = m_Config.Eval(moR(GRID_HEIGHT_RANDOMIZER));
  double planet_factor = m_Config.Eval(moR(GRID_PLANET_FACTOR));
  double torus_factor = m_Config.Eval(moR(GRID_TORUS_FACTOR));
  double wave_amplitude = m_Config.Eval(moR(GRID_WAVE_AMPLITUDE));

  double control_roll_angle = m_Config.Eval(moR(GRID_CONTROL_ROLL_ANGLE));
  double control_roll_force = m_Config.Eval(moR(GRID_CONTROL_ROLL_FORCE));
  double control_lift_angle = m_Config.Eval(moR(GRID_CONTROL_LIFT_ANGLE));
  double control_lift_force = m_Config.Eval(moR(GRID_CONTROL_LIFT_FORCE));

  double control_speed = m_Config.Eval(moR(GRID_CONTROL_SPEED));
  double control_speed_acceleration = m_Config.Eval(moR(GRID_CONTROL_SPEED_ACCELERATION));
  double control_minimum_speed = m_Config.Eval(moR(GRID_CONTROL_MINIMUM_SPEED));
  double control_maximum_speed = m_Config.Eval(moR(GRID_CONTROL_MAXIMUM_SPEED));

  double control_gravity_force = m_Config.Eval(moR(GRID_CONTROL_GRAVITY_FORCE));
  double control_minimum_altitude = m_Config.Eval(moR(GRID_CONTROL_MINIMUM_ALTITUDE));
  double control_maximum_altitude = m_Config.Eval(moR(GRID_CONTROL_MAXIMUM_ALTITUDE));
  double control_minimum_surface_altitude = m_Config.Eval(moR(GRID_CONTROL_MINIMUM_SURFACE_ALTITUDE));
  double alpha = m_Config.Eval(moR(GRID_ALPHA));


  if (Grid) {

      Grid->Wireframe(wireframe==1);

      if (Grid->m_GridDefinition.map_position_x!=m_Config.Eval(moR(GRID_MAP_POSITION_X))
          ||
          Grid->m_GridDefinition.map_position_y!=m_Config.Eval(moR(GRID_MAP_POSITION_Y)) )  {

        Grid->m_GridDefinition.map_position_x = m_Config.Eval(moR(GRID_MAP_POSITION_X));
        Grid->m_GridDefinition.map_position_y = m_Config.Eval(moR(GRID_MAP_POSITION_Y));

        //Grid->Posicionar(Grid->m_GridDefinition.map_position_x, Grid->m_GridDefinition.map_position_y);
      }


      Grid->m_GridDefinition.map_position_x =  m_Config.Eval(moR(GRID_MAP_POSITION_X));
      Grid->m_GridDefinition.map_position_y =  m_Config.Eval(moR(GRID_MAP_POSITION_Y));
      Grid->m_GridDefinition.m_height_multiply = height_multiply;
      Grid->m_GridDefinition.m_planet_factor = planet_factor;
      Grid->m_GridDefinition.m_torus_factor = torus_factor;
      Grid->m_GridDefinition.m_wave_amplitude = wave_amplitude;
      Grid->m_GridDefinition.m_wireframe_width = wireframe_width;
      Grid->m_GridDefinition.m_wireframe_configuration = wireframe_configuration;
      Grid->m_GridDefinition.m_alpha = alpha*this->m_EffectState.alpha;
      Grid->m_GridDefinition.m_minimum_surface_altitude = control_minimum_surface_altitude;

      moVector4d gcolor = m_Config.EvalColor( moR(GRID_COLOR) );

      Grid->m_GridDefinition.m_red = gcolor.X();
      Grid->m_GridDefinition.m_green = gcolor.Y();
      Grid->m_GridDefinition.m_blue = gcolor.Z();
      /*Grid->m_GridDefinition.m_red = m_Config.GetParam(moR(GRID_COLOR)).GetValue().GetSubValue(MO_RED).Float()*m_EffectState.tintr;
      Grid->m_GridDefinition.m_green = m_Config.GetParam(moR(GRID_COLOR)).GetValue().GetSubValue(MO_GREEN).Float()*m_EffectState.tintg;
      Grid->m_GridDefinition.m_blue = m_Config.GetParam(moR(GRID_COLOR)).GetValue().GetSubValue(MO_BLUE).Float()*m_EffectState.tintb;
      *///MODebug2->Message( "height_multiply: "+ FloatToStr(height_multiply) );

      if ( control_speed!=0.0f ) {
          Grid->RapidezF =  control_speed;
          Grid->Rapidez = Grid->RapidezF;
      }

      if ( control_speed_acceleration!=0.0f ) {
          Grid->RapidezF+= control_speed_acceleration;
          Grid->Rapidez = Grid->RapidezF;
      }

      if ( Grid->CAM3D.Oz > control_maximum_altitude )
        Grid->CAM3D.Oz = control_maximum_altitude;

        if (fabs(control_roll_angle)>0.0f ) {

          if (fabs(control_roll_angle)>1.5f) {
              control_roll_angle = 1.5f;
          }
          if (fabs(control_roll_angle)<-1.5f) {
              control_roll_angle = -1.5f;
          }

          turnAngle+= -control_roll_angle/80.0f;

          inivectors( Grid->CAM3D.Ox, Grid->CAM3D.Oy, Grid->CAM3D.Oz, &Grid->CAM3D );
          turnview( turnAngle, 0.0, 0.0, &Grid->CAM3D );
          turnview( 0.0, control_roll_angle/3.2f, 0.0, &Grid->CAM3D );

          //float rollValue =
        }

        Grid->CAM3D.Oz-= control_gravity_force;

        if (Grid->CAM3D.Oz<control_minimum_altitude) {
          Grid->CAM3D.Oz = control_minimum_altitude;
        }

      //if (control_roll_angle) Grid->

    }

  if (!m_pOutletCameraHeight) {
    int outindex = GetOutletIndex( "CAMERA_HEIGHT" );
    if (outindex>-1) {
      m_pOutletCameraHeight = m_Outlets.GetRef( outindex );
    }

  }

  if (m_pOutletCameraHeight && Grid) {

    m_pOutletCameraHeight->GetData()->SetDouble( Grid->CAM3D.Oz );
    m_pOutletCameraHeight->Update(true);
  }


  if (!m_pGesture) {
      m_pGesture = m_Inlets.GetRef( this->GetInletIndex( moText("KINECTSIGNSGRID") ) );
      if (m_pGesture) {
        MODebug2->Message("KINECTSIGNSGRID found!");
      }
  }

  if (m_pGesture) {
    if (m_pGesture->Updated() && Grid) {

        moData* gData = m_pGesture->GetData();

        if (gData) {

          moDataMessage* gMessage = gData->Message();

          if (gMessage) {

            for( int m = 0; m < gMessage->Count(); m++ ) {

              moData DataGesture0 = gMessage->Get(0);
              moText gesturesign = DataGesture0.ToText();
              //moData* DataGesture1 = gMessage->Get(1);
              /**
              MODebug2->Message(
                                moText("moEffectGrid::UpdateParameters > GESTURE: count[")
                                + IntToStr( gMessage->Count() ) + "] 0: ["
                                + gesturesign +"]"
                              );*/

              if (gesturesign=="TURN_LEFT") {

                //Grid->Check_Keyb( GRID_TURN_LEFT );
                //Grid->Check_Keyb( GRID_TILT_LEFT );

              } else if (gesturesign=="TURN_RIGHT") {

                //Grid->Check_Keyb( GRID_TURN_RIGHT );
                //Grid->Check_Keyb( GRID_TILT_RIGHT );
                //if (Grid) Grid->Check_Keyb( GRID_TILT_RIGHT );
              }

              if (gesturesign=="FLUTTER") {
                //Grid->Check_Keyb( GRID_KEYDOWN );
                Grid->CAM3D.Oz+= control_lift_force;
                //MODebug2->Message( "moEffectGrid::UpdateParameters > Grid->CAM3D.Oz: " + FloatToStr(control_lift_force) +" control_lift_force:" + FloatToStr(control_lift_force)  );

              }

            }
          }
        }
    }
  }

}


void moEffectGrid::Draw( moTempo* tempogral,moEffectState* parentstate)
{
	int I, J, texture_a, texture_b;

  UpdateParameters();

    PreDraw( tempogral, parentstate);

  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
    // Funcion de blending y de alpha channel //

  glEnable(GL_BLEND);
  //glDisable(GL_ALPHA);
  glEnable(GL_ALPHA);
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glDisable(GL_DEPTH_TEST);

    glEnable(GL_ACTIVE_TEXTURE_ARB);

	if (glActiveTexture) glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);glEnable(GL_TEXTURE_2D);
	if (glActiveTexture) glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(GRID_TEXTURE), &m_EffectState.tempo ) );
  //glBindTexture(GL_TEXTURE_2D, m_Config.GetGLId(moR(GRID_TEXTURE))  );

	if (glActiveTexture) glActiveTextureARB(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	if (glActiveTexture) glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(GRID_TEXTUREB), &m_EffectState.tempo ) );

	if (glActiveTexture) glActiveTextureARB(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	if (glActiveTexture) glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(GRID_TEXTUREC), &m_EffectState.tempo ) );

	if (glActiveTexture) glActiveTextureARB(GL_TEXTURE3);
	glEnable(GL_TEXTURE_2D);
	if (glActiveTexture) glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(GRID_TEXTURED), &m_EffectState.tempo ) );

	if (glActiveTexture) glActiveTextureARB(GL_TEXTURE4);
	glEnable(GL_TEXTURE_2D);
	if (glActiveTexture) glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(GRID_TEXTUREE), &m_EffectState.tempo ) );

	if (glActiveTexture) glActiveTextureARB(GL_TEXTURE5);
	glEnable(GL_TEXTURE_2D);
	if (glActiveTexture) glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(GRID_TEXTUREF), &m_EffectState.tempo ) );

	if (glActiveTexture) glActiveTextureARB(GL_TEXTURE6);
	glEnable(GL_TEXTURE_2D);
	if (glActiveTexture) glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(GRID_TEXTUREG), &m_EffectState.tempo ) );

	if (glActiveTexture) glActiveTextureARB(GL_TEXTURE7);
	glEnable(GL_TEXTURE_2D);
	if (glActiveTexture) glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(GRID_TEXTUREH), &m_EffectState.tempo ) );


	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glColor4f(  m_Config.GetParam(color).GetValue().GetSubValue(MO_RED).Float()*m_EffectState.tintr,
                    m_Config.GetParam(color).GetValue().GetSubValue(MO_GREEN).Float()*m_EffectState.tintg,
                    m_Config.GetParam(color).GetValue().GetSubValue(MO_BLUE).Float()*m_EffectState.tintb,
                    m_Config.GetParam(color).GetValue().GetSubValue(MO_ALPHA).Float()*m_EffectState.alpha);

	//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

	glTranslatef(   m_Config.Eval( moR(GRID_TRANSLATEX) ),
                  m_Config.Eval( moR(GRID_TRANSLATEY)),
                  m_Config.Eval( moR(GRID_TRANSLATEZ))
              );

	glRotatef(  m_Config.Eval( moR(GRID_ROTATEZ) ), 0.0, 0.0, 1.0 );
    glRotatef(  m_Config.Eval( moR(GRID_ROTATEY) ), 0.0, 1.0, 0.0 );
    glRotatef(  m_Config.Eval( moR(GRID_ROTATEX) ), 1.0, 0.0, 0.0 );

	glScalef(   m_Config.Eval( moR(GRID_SCALEX)),
              m_Config.Eval( moR(GRID_SCALEY)),
              m_Config.Eval( moR(GRID_SCALEZ))
            );

	if (Grid) {
      Grid->Loop_Engine();
	}

	//if (glActiveTextureARB) glActiveTextureARB(GL_TEXTURE0_ARB);
	//	glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(GRID_TEXTURE), &m_EffectState.tempo ) );
    //glBindTexture(GL_TEXTURE_2D, m_Config.GetGLId(moR(GRID_TEXTURE)) );

	//glDisable(GL_ACTIVE_TEXTURE_ARB);
  //glEnable(GL_BLEND);
  //glEnable(GL_ALPHA);

	// Dejamos todo como lo encontramos //
    glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix


}



void
moEffectGrid::Interaction( moIODeviceManager *IODeviceManager ) {
	//aqui se parsean los codisp
	moDeviceCode *temp,*aux;
	MOint did,cid,state,e,valor,estaux,elaux;

	if(devicecode!=NULL)
	for(int i=0; i<ncodes;i++) {
		temp = devicecode[i].First;
		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			state = IODeviceManager->IODevices().GetRef(did)->GetStatus(cid);
			valor = IODeviceManager->IODevices().GetRef(did)->GetValue(cid);
			if(state)
			switch(i) {
				case MO_GRID_UP:
					if (Grid) Grid->Check_Keyb(GRID_KEYUP);
					break;
				case MO_GRID_DOWN:
					if (Grid) Grid->Check_Keyb(GRID_KEYDOWN);
					break;
				case MO_GRID_TURN_LEFT:
					if (Grid) Grid->Check_Keyb(GRID_TURN_LEFT);
					break;
				case MO_GRID_TURN_RIGHT:
					if (Grid) Grid->Check_Keyb(GRID_TURN_RIGHT);
					break;
				case MO_GRID_TILT_LEFT:
					if (Grid) Grid->Check_Keyb(GRID_TILT_LEFT);
					break;
				case MO_GRID_TILT_RIGHT:
					if (Grid) Grid->Check_Keyb(GRID_TILT_RIGHT);
					break;
				case MO_GRID_FORWARD:
					if (Grid) Grid->Check_Keyb(GRID_FORWARD);
					break;
				case MO_GRID_BACK:
					Grid->Check_Keyb(GRID_BACK);
					break;
				case MO_GRID_PITCH:
					if (Grid) Grid->Pitch(valor);
					MODebug2->Push(IntToStr(valor));
					break;
				case MO_GRID_TRIM:
					if (Grid) Grid->Trim(valor);
					//MODebug2->Push(IntToStr(valor));
					break;
				case MO_GRID_WIREFRAME:
					if (Grid) Grid->Wireframe(-1);
					if (Grid) MODebug2->Push(IntToStr(Grid->m_GridDefinition.m_wireframe_mode));
					break;
        case MO_GRID_RESETPOSITION:
					if (Grid) Grid->Check_Keyb(GRID_RESET);
					break;
			}
		temp = temp->next;
		}
	}

	if (moIsTimerStopped()  || m_EffectState.tempo.State()==MO_TIMERSTATE_STOPPED ) {
    if (Grid) Grid->Check_Keyb(GRID_RESET);
	}
}

MOboolean moEffectGrid::Finish()
{
	if (Grid) {
      Grid->Stop_Engine();
      delete Grid;
      Grid = NULL;
  }

    return PreFinish();
}
