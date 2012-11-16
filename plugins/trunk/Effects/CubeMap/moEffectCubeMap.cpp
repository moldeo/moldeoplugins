/*******************************************************************************

                               moEffectCubeMap.cpp

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

#include "moEffectCubeMap.h"

//========================
//  Factory
//========================

moEffectCubeMapFactory *m_EffectCubeMapFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectCubeMapFactory==NULL)
		m_EffectCubeMapFactory = new moEffectCubeMapFactory();
	return(moEffectFactory*) m_EffectCubeMapFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectCubeMapFactory;
	m_EffectCubeMapFactory = NULL;
}

moEffect*  moEffectCubeMapFactory::Create() {
	return new moEffectCubeMap();
}

void moEffectCubeMapFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectCubeMap::moEffectCubeMap() {
	SetName("cubemap");
}

moEffectCubeMap::~moEffectCubeMap() {
	Finish();
}

MOboolean
moEffectCubeMap::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( CUBEMAP_ALPHA, moText("alpha") );
	moDefineParamIndex( CUBEMAP_COLOR, moText("color") );
	moDefineParamIndex( CUBEMAP_SYNC, moText("syncro") );
	moDefineParamIndex( CUBEMAP_PHASE, moText("phase") );
	moDefineParamIndex( CUBEMAP_TEXTURELEFT, moText("textureleft") );
	moDefineParamIndex( CUBEMAP_TEXTUREFRONT, moText("texturefront") );
	moDefineParamIndex( CUBEMAP_TEXTURERIGHT, moText("textureright") );
	moDefineParamIndex( CUBEMAP_TEXTUREBACK, moText("textureback") );
	moDefineParamIndex( CUBEMAP_TEXTURETOP, moText("texturetop") );
	moDefineParamIndex( CUBEMAP_TEXTUREBOTTOM, moText("texturebottom") );
	moDefineParamIndex( CUBEMAP_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( CUBEMAP_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( CUBEMAP_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( CUBEMAP_ROTATEX, moText("rotatex") );
	moDefineParamIndex( CUBEMAP_ROTATEY, moText("rotatey") );
	moDefineParamIndex( CUBEMAP_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( CUBEMAP_SCALEX, moText("scalex") );
	moDefineParamIndex( CUBEMAP_SCALEY, moText("scaley") );
	moDefineParamIndex( CUBEMAP_SCALEZ, moText("scalez") );
	moDefineParamIndex( CUBEMAP_INLET, moText("inlet") );
	moDefineParamIndex( CUBEMAP_OUTLET, moText("outlet") );

	return true;
}

void moEffectCubeMap::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw( tempogral,parentstate);

    glEnable(GL_DEPTH_TEST);							// Disables Depth Testing
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	//gluLookAt(0, 0, 0, 0, 0, 1, 0, 1, 0);
	gluPerspective(45,1.33f,0.0f,3.0f);
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix


    glTranslatef(   m_Config.Eval( moR(CUBEMAP_TRANSLATEX) ),
                  m_Config.Eval( moR(CUBEMAP_TRANSLATEY)),
                  m_Config.Eval( moR(CUBEMAP_TRANSLATEZ))
              );

	glRotatef(  m_Config.Eval( moR(CUBEMAP_ROTATEX) ), 1.0, 0.0, 0.0 );
    glRotatef(  m_Config.Eval( moR(CUBEMAP_ROTATEY) ), 0.0, 1.0, 0.0 );
    glRotatef(  m_Config.Eval( moR(CUBEMAP_ROTATEZ) ), 0.0, 0.0, 1.0 );

	glScalef(   m_Config.Eval( moR(CUBEMAP_SCALEX)),
              m_Config.Eval( moR(CUBEMAP_SCALEY)),
              m_Config.Eval( moR(CUBEMAP_SCALEZ))
            );

/*
	glColor4f(  m_Config[moR(CUBEMAP_COLOR)][MO_SELECTED][MO_RED].Fun()->Eval(m_EffectState.tempo.ang) * state.tintr,
                m_Config[moR(CUBEMAP_COLOR)][MO_SELECTED][MO_GREEN].Fun()->Eval(m_EffectState.tempo.ang) * state.tintg,
                m_Config[moR(CUBEMAP_COLOR)][MO_SELECTED][MO_BLUE].Fun()->Eval(m_EffectState.tempo.ang) * state.tintb,
				m_Config[moR(CUBEMAP_COLOR)][MO_SELECTED][MO_ALPHA].Fun()->Eval(m_EffectState.tempo.ang) *
				m_Config[moR(CUBEMAP_ALPHA)].GetData()->Fun()->Eval(m_EffectState.tempo.ang) * state.alpha);
*/

    SetColor( m_Config[moR(CUBEMAP_COLOR)], m_Config[moR(CUBEMAP_ALPHA)], m_EffectState );

    MOuint ILeft = m_Config.GetGLId( moR(  CUBEMAP_TEXTURELEFT ), &m_EffectState.tempo );
    MOuint IRight = m_Config.GetGLId( moR(  CUBEMAP_TEXTURERIGHT ), &m_EffectState.tempo );
    MOuint IFront = m_Config.GetGLId( moR(  CUBEMAP_TEXTUREFRONT ), &m_EffectState.tempo );
    MOuint IBack = m_Config.GetGLId( moR(  CUBEMAP_TEXTUREBACK ), &m_EffectState.tempo );
    MOuint ITop = m_Config.GetGLId( moR(  CUBEMAP_TEXTURETOP ), &m_EffectState.tempo );
    MOuint IBottom = m_Config.GetGLId( moR(  CUBEMAP_TEXTUREBOTTOM ), &m_EffectState.tempo );


  //front
  glBindTexture( GL_TEXTURE_2D, IFront );
  glBegin(GL_QUADS);
    glTexCoord2f(1,0);
    glVertex3f(-1,-1,-1);
    glTexCoord2f(0,0);
    glVertex3f(1,-1,-1);
    glTexCoord2f(0,1);
    glVertex3f(1,1,-1);
    glTexCoord2f(1,1);
    glVertex3f(-1,1,-1);
  glEnd();

//top
  glBindTexture( GL_TEXTURE_2D, ITop );
  glBegin(GL_QUADS);
    glTexCoord2f(1,0);
    glVertex3f(-1,1,-1);
    glTexCoord2f(0,0);
    glVertex3f(1,1,-1);
    glTexCoord2f(0,1);
    glVertex3f(1,1,1);
    glTexCoord2f(1,1);
    glVertex3f(-1,1,1);
  glEnd();

  //left
  glBindTexture( GL_TEXTURE_2D, ILeft );
  glBegin(GL_QUADS);
    glTexCoord2f(1,1);
    glVertex3f(-1,1,1);
    glTexCoord2f(0,1);
    glVertex3f(-1,1,-1);
    glTexCoord2f(0,0);
    glVertex3f(-1,-1,-1);
    glTexCoord2f(1,0);
    glVertex3f(-1,-1,1);
  glEnd();

  //right
  glBindTexture( GL_TEXTURE_2D, IRight );
  glBegin(GL_QUADS);
    glTexCoord2f(0,1);
    glVertex3f(1,1,1);
    glTexCoord2f(1,1);
    glVertex3f(1,1,-1);
    glTexCoord2f(1,0);
    glVertex3f(1,-1,-1);
    glTexCoord2f(0,0);
    glVertex3f(1,-1,1);
  glEnd();

  //bottom
  glBindTexture( GL_TEXTURE_2D, IBottom );
  glBegin(GL_QUADS);
    glTexCoord2f(1,1);
    glVertex3f(-1,-1,-1);
    glTexCoord2f(0,1);
    glVertex3f(1,-1,-1);
    glTexCoord2f(0,0);
    glVertex3f(1,-1,1);
    glTexCoord2f(1,0);
    glVertex3f(-1,-1,1);
  glEnd();


  //back
  glBindTexture( GL_TEXTURE_2D, IBack );
  glBegin(GL_QUADS);
    glTexCoord2f(0,0);
    glVertex3f(-1,-1,1);
    glTexCoord2f(1,0);
    glVertex3f(1,-1,1);
    glTexCoord2f(1,1);
    glVertex3f(1,1,1);
    glTexCoord2f(0,1);
    glVertex3f(-1,1,1);
  glEnd();

    glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix

}

MOboolean moEffectCubeMap::Finish()
{
    return PreFinish();
}

moConfigDefinition *
moEffectCubeMap::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("texturefront"), MO_PARAM_TEXTURE, CUBEMAP_TEXTUREFRONT, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("textureright"), MO_PARAM_TEXTURE, CUBEMAP_TEXTURERIGHT, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("textureback"), MO_PARAM_TEXTURE, CUBEMAP_TEXTUREBACK, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("textureleft"), MO_PARAM_TEXTURE, CUBEMAP_TEXTURELEFT, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("texturetop"), MO_PARAM_TEXTURE, CUBEMAP_TEXTURETOP, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("texturebottom"), MO_PARAM_TEXTURE, CUBEMAP_TEXTUREBOTTOM, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, CUBEMAP_TRANSLATEX );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, CUBEMAP_TRANSLATEY );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, CUBEMAP_TRANSLATEZ );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, CUBEMAP_ROTATEX );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, CUBEMAP_ROTATEY );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, CUBEMAP_ROTATEZ );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, CUBEMAP_SCALEX );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, CUBEMAP_SCALEY );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, CUBEMAP_SCALEZ );
	return p_configdefinition;
}
