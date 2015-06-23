/*******************************************************************************

                              moEffectTunel.cpp

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
  Andrés Colubri

*******************************************************************************/

#include "moEffectTunel.h"

//========================
//  Factory
//========================

moEffectTunelFactory *m_EffectTunelFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectTunelFactory==NULL)
		m_EffectTunelFactory = new moEffectTunelFactory();
	return(moEffectFactory*) m_EffectTunelFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectTunelFactory;
	m_EffectTunelFactory = NULL;
}

moEffect*  moEffectTunelFactory::Create() {
	return new moEffectTunel();
}

void moEffectTunelFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectTunel::moEffectTunel()
{
	SetName("tunel");
}

moEffectTunel::~moEffectTunel()
{
	Finish();
}

moConfigDefinition *
moEffectTunel::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, TUNEL_TEXTURE, moValue( "default", "TXT")  );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, TUNEL_TRANSLATEX );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, TUNEL_TRANSLATEY );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, TUNEL_TRANSLATEZ );
	p_configdefinition->Add( moText("rotate"), MO_PARAM_ROTATEZ, TUNEL_ROTATE );
	p_configdefinition->Add( moText("target_translatex"), MO_PARAM_TRANSLATEX, TUNEL_TARGET_TRANSLATEX );
	p_configdefinition->Add( moText("target_translatey"), MO_PARAM_TRANSLATEY, TUNEL_TARGET_TRANSLATEY );
	p_configdefinition->Add( moText("target_translatez"), MO_PARAM_TRANSLATEZ, TUNEL_TARGET_TRANSLATEZ );
	p_configdefinition->Add( moText("target_rotate"), MO_PARAM_ROTATEZ, TUNEL_TARGET_ROTATE );
	p_configdefinition->Add( moText("sides"), MO_PARAM_NUMERIC, TUNEL_SIDES, moValue("12", "INT" ) );
	p_configdefinition->Add( moText("segments"), MO_PARAM_NUMERIC, TUNEL_SEGMENTS, moValue("32", "INT" ) );
  p_configdefinition->Add( moText("wireframe"), MO_PARAM_NUMERIC, TUNEL_SEGMENTS, moValue("0", "INT" ) );

	return p_configdefinition;
}

MOboolean moEffectTunel::Init()
{
    if (!PreInit()) return false;

	moDefineParamIndex( TUNEL_INLET, moText("inlet") );
	moDefineParamIndex( TUNEL_OUTLET, moText("outlet") );
	moDefineParamIndex( TUNEL_SCRIPT, moText("script") );

	moDefineParamIndex( TUNEL_ALPHA, moText("alpha") );
	moDefineParamIndex( TUNEL_COLOR, moText("color") );
	moDefineParamIndex( TUNEL_SYNC, moText("syncro") );
	moDefineParamIndex( TUNEL_PHASE, moText("phase") );
	moDefineParamIndex( TUNEL_TEXTURE, moText("texture") );
	moDefineParamIndex( TUNEL_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( TUNEL_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( TUNEL_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( TUNEL_ROTATE, moText("rotate") );
	moDefineParamIndex( TUNEL_TARGET_TRANSLATEX, moText("target_translatex") );
	moDefineParamIndex( TUNEL_TARGET_TRANSLATEY, moText("target_translatey") );
	moDefineParamIndex( TUNEL_TARGET_TRANSLATEZ, moText("target_translatez") );
	moDefineParamIndex( TUNEL_TARGET_ROTATE, moText("target_rotate") );

  moDefineParamIndex( TUNEL_SIDES, moText("sides") );
  moDefineParamIndex( TUNEL_SEGMENTS, moText("segments") );
  moDefineParamIndex( TUNEL_WIREFRAME, moText("wireframe") );


	return true;
}

void moEffectTunel::UpdateParameters() {

  sides = m_Config.Int( moR(TUNEL_SIDES) );

  if (sides>MAXTUNELSIDES) sides = MAXTUNELSIDES;

  segments = m_Config.Int( moR(TUNEL_SEGMENTS) );

  if (segments>MAXTUNELSEGMENTS) segments = MAXTUNELSEGMENTS;

  wireframe_mode = m_Config.Int( moR(TUNEL_WIREFRAME) ) == 1;

}

void moEffectTunel::Draw( moTempo* tempogral,moEffectState* parentstate)
{
	int I, J, texture_a;
	GLdouble C, J1, J2;
	GLdouble Angle, Speed;

  BeginDraw( tempogral, parentstate);

  UpdateParameters();

	Angle =(m_EffectState.tempo.getTempo()/2.0);
	Speed =(m_EffectState.tempo.getTempo()/2.0)*TEXTURE_SPEED;

  // Guardar y resetar la matriz de vista del modelo //
  glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
  glPushMatrix();                                     // Store The Modelview Matrix
	glLoadIdentity();									// Reset The View

  // Funcion de blending y de alpha channel //
  glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_ALPHA);

  // Cambiar la proyeccion para usar el z-buffer //
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_DEPTH_BUFFER_BIT);


  if (wireframe_mode) {
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  }

    // Draw
	glTranslatef(   m_Config.Eval( moR(TUNEL_TRANSLATEX) ),
                  m_Config.Eval( moR(TUNEL_TRANSLATEY) ),
                  m_Config.Eval( moR(TUNEL_TRANSLATEZ) )
              );

  glRotatef(  m_Config.Eval( moR(TUNEL_ROTATE)), 0.0, 0.0, 1.0 );

  /*
  moTexture* pImage = (moTexture*) m_Config.Texture( [moR(TUNEL_TEXTURE) );
  if (pImage!=NULL) {
      if (pImage->GetType()==MO_TYPE_MOVIE) {
          glBlendFunc(GL_SRC_COLOR,GL_ONE_MINUS_SRC_COLOR);
      } else {
          glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      }
  }
  */
  glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(TUNEL_TEXTURE)) );

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

	// setup TUNEL coordinates
	float fsides = (float)sides;
	float fsegments = (float)segments;
	int segments_end = ( segments * 2 ) / 3 ;

	for( I=0; I<=sides; I++)
		for( J=0; J<segments; J++)
		{
      float radius = (3.0 - J/fsides);//se achica con la distancia...
      /*
      float target_x = m_Config.Eval( moR(TUNEL_TARGET_TRANSLATEX));
      float target_y = m_Config.Eval( moR(TUNEL_TARGET_TRANSLATEY));
      float target_z = m_Config.Eval( moR(TUNEL_TARGET_TRANSLATEZ));
      float target_rotate = m_Config.Eval( moR(TUNEL_TARGET_ROTATE)) * 2.0f * MO_PI / 180.0f;
      */
      float target_x = m_Config.Eval( moR(TUNEL_TARGET_TRANSLATEX)) * ( 2*sin((Angle+2.0*J)/29) + cos((Angle+2*J)/13) - 2*sin(Angle/29) - cos(Angle/13) );
      float target_y = m_Config.Eval( moR(TUNEL_TARGET_TRANSLATEY)) * ( 2*cos((Angle+2.0*J)/33) + sin((Angle+2*J)/17) - 2*cos(Angle/33) - sin(Angle/17) );
      float target_z = 0.0;
      float target_rotate = (J/fsides) * m_Config.Eval( moR(TUNEL_TARGET_ROTATE)) * 2.0f * MO_PI / 180.0f;
      ///fade rotation with distance...

			Tunnels[I][J].X = radius*cos( target_rotate + (I* 2*MO_PI )/fsides ) + target_x;// + 2*sin((Angle+2.0*J)/29) + cos((Angle+2*J)/13) - 2*sin(Angle/29) - cos(Angle/13);
			Tunnels[I][J].Y = radius*sin( target_rotate + (I* 2*MO_PI )/fsides ) + target_y;// + 2*cos((Angle+2.0*J)/33) + sin((Angle+2*J)/17) - 2*cos(Angle/33) - sin(Angle/17);
			Tunnels[I][J].Z = -J + (target_z*J) / (float)segments ;
		}


	// draw TUNEL
	for( J=segments-2; J>=0; J--)
	{
		// precalculate texture v coords for speed
		J1 = J/fsegments + Speed;
		J2 =(J+1.0)/fsegments + Speed;

		// near the end of the TUNEL, fade the effect away
		if(J > segments_end )
			C = 1.0-(J-segments_end)/(segments-segments_end);
		else
			C = 1.0;

    SetColor( m_Config[moR(TUNEL_COLOR)], m_Config[moR(TUNEL_ALPHA)], m_EffectState );

		glBegin(GL_QUADS);
		for( I=0; I<sides; I++)
		{
			glTexCoord2f((I-3.0)/fsides, J1);
			glVertex3f(Tunnels[  I][   J].X,
              Tunnels[  I][   J].Y,
              Tunnels[  I][   J].Z);

			glTexCoord2f((I-2.0)/fsides, J1);
			glVertex3f(Tunnels[I+1][   J].X,
              Tunnels[I+1][   J].Y,
              Tunnels[I+1][   J].Z);

			glTexCoord2f((I-2.0)/fsides, J2);
			glVertex3f(Tunnels[I+1][ J+1].X,
              Tunnels[I+1][ J+1].Y,
              Tunnels[I+1][ J+1].Z);

			glTexCoord2f((I-3.0)/fsides, J2);
			glVertex3f(Tunnels[  I][ J+1].X,
              Tunnels[  I][ J+1].Y,
              Tunnels[  I][ J+1].Z);
		}
		glEnd();
	}

    // Dejamos todo como lo encontramos //
    glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix

  EndDraw();
}

MOboolean moEffectTunel::Finish()
{
    return PreFinish();
}



