/*******************************************************************************

                              moEffectImageFlow3D.cpp

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                     *
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

  Copyright(C) 2012 Fabricio Costa

  Authors:
  Fabricio Costa

*******************************************************************************/

#include "moEffectImageFlow3D.h"

//========================
//  Factory
//========================

moEffectImageFlow3DFactory *m_EffectImageFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectImageFactory==NULL)
		m_EffectImageFactory = new moEffectImageFlow3DFactory();
	return(moEffectFactory*) m_EffectImageFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectImageFactory;
	m_EffectImageFactory = NULL;
}

moEffect*  moEffectImageFlow3DFactory::Create() {
	return new moEffectImageFlow3D();
}

void moEffectImageFlow3DFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectImageFlow3D::moEffectImageFlow3D() {
	SetName("imageflow3d");
}

moEffectImageFlow3D::~moEffectImageFlow3D() {
	Finish();
}

MOboolean
moEffectImageFlow3D::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( IMAGEFLOW3D_ALPHA, moText("alpha") );
	moDefineParamIndex( IMAGEFLOW3D_COLOR, moText("color") );
	moDefineParamIndex( IMAGEFLOW3D_SYNCRO, moText("syncro") );
	moDefineParamIndex( IMAGEFLOW3D_PHASE, moText("phase") );
	moDefineParamIndex( IMAGEFLOW3D_BLENDING, moText("blending") );
	moDefineParamIndex( IMAGEFLOW3D_IMAGES, moText("images") );
	moDefineParamIndex( IMAGEFLOW3D_VELOCITY, moText("velocity") );
	moDefineParamIndex( IMAGEFLOW3D_FLOW_MODE, moText("flow_mode") );
	moDefineParamIndex( IMAGEFLOW3D_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( IMAGEFLOW3D_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( IMAGEFLOW3D_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( IMAGEFLOW3D_ROTATEX, moText("rotatex") );
	moDefineParamIndex( IMAGEFLOW3D_ROTATEY, moText("rotatey") );
	moDefineParamIndex( IMAGEFLOW3D_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( IMAGEFLOW3D_SCALEX, moText("scalex") );
	moDefineParamIndex( IMAGEFLOW3D_SCALEY, moText("scaley") );
	moDefineParamIndex( IMAGEFLOW3D_SCALEZ, moText("scalez") );

	updateParameters();

	minx = 0.0;
	maxx = m_pResourceManager->GetRenderMan()->ScreenWidth();
	miny = 0.0;
	maxy = m_pResourceManager->GetRenderMan()->ScreenHeight();

	if (flow_mode == 0) {
	    /*horizontal flow*/
        if (0 < flow_velocity0)
        {
            flow_coord = minx;
            idx0 = 1;
            idx1 = 0;
        }
        else
        {
            flow_coord = maxx;
            idx0 = 0;
            idx1 = 1;
        }
	} else {
        /*vertical flow*/
        if (0 < flow_velocity0)
        {
            flow_coord = maxy;
            idx0 = 1;
            idx1 = 0;
        }
        else
        {
            flow_coord = miny;
            idx0 = 0;
            idx1 = 1;
        }

	}
/*
	Images.Init( GetConfig(), m_Config.GetParamIndex("images"), m_pResourceManager->GetTextureMan() );
	tex0 = Images.GetGLId(1);
	tex1 = Images.GetGLId(0);
*/
    moParam& Pimages( m_Config.GetParam( moR(IMAGEFLOW3D_IMAGES) ) );

    tex0  = Pimages.GetValue(0).GetSubValue(0).GetGLId( &state.tempo );
    tex1 = tex0;

    if ( Pimages.GetValuesCount() > 1 ) {

        tex1  = Pimages.GetValue(1).GetSubValue(0).GetGLId( &state.tempo );

    }

    flow_velocity_bak = flow_velocity = flow_velocity0;

	return true;
}

void moEffectImageFlow3D::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw(tempogral, parentstate);

    moParam& Pimages( m_Config.GetParam( moR(IMAGEFLOW3D_IMAGES) ) );



    /*Comentado: VERSION IMAGEFLOW 2d*/
        //glDisable(GL_DEPTH_TEST);
        //glClear ( GL_DEPTH_BUFFER_BIT );
        //m_pResourceManager->GetGLMan()->SetOrthographicView(maxx, maxy);

    float prop = 1.0f;
    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();
    if ( w == 0 || h == 0 ) { w  = 1; h = 1; prop = 1.0; }
    else {
      prop = (float) h / (float) w;
    }


	minx = -0.5;
	maxx = 0.5;
	miny = -0.5*prop;
	maxy = 0.5*prop;


    // Guardar y resetar la matriz de vista del modelo //
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glPushMatrix();                                     // Store The Modelview Matrix
	glLoadIdentity();									// Reset The View

    // Cambiar la proyeccion para una vista ortogonal //
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	gluPerspective(45.0f, 1/prop, 0.1f ,4000.0f);

    // Funcion de blending y de alpha channel //
    glEnable(GL_BLEND);

	glDisable(GL_ALPHA);

	glTranslatef(   m_Config.Eval( moR(IMAGEFLOW3D_TRANSLATEX) ),
                  m_Config.Eval( moR(IMAGEFLOW3D_TRANSLATEY)),
                  m_Config.Eval( moR(IMAGEFLOW3D_TRANSLATEZ))
              );

	glRotatef(  m_Config.Eval( moR(IMAGEFLOW3D_ROTATEX) ), 1.0, 0.0, 0.0 );
  glRotatef(  m_Config.Eval( moR(IMAGEFLOW3D_ROTATEY) ), 0.0, 1.0, 0.0 );
  glRotatef(  m_Config.Eval( moR(IMAGEFLOW3D_ROTATEZ) ), 0.0, 0.0, 1.0 );

	glScalef(   m_Config.Eval( moR(IMAGEFLOW3D_SCALEX)),
              m_Config.Eval( moR(IMAGEFLOW3D_SCALEY)),
              m_Config.Eval( moR(IMAGEFLOW3D_SCALEZ))
            );


    SetColor( m_Config[moR(IMAGEFLOW3D_COLOR)][MO_SELECTED], m_Config[moR(IMAGEFLOW3D_ALPHA)][MO_SELECTED], state );

    SetBlending( (moBlendingModes) m_Config[moR(IMAGEFLOW3D_BLENDING)][MO_SELECTED][0].Int() );

	float t;

	updateParameters( );

	flow_velocity_bak = flow_velocity;
	if (state.tempo.delta <= 1.0) flow_velocity = state.tempo.delta * flow_velocity0;
	else flow_velocity = (1.0 + 10.0 * (state.tempo.delta - 1.0)) * flow_velocity0;
	if (flow_velocity != flow_velocity_bak) MODebug2->Push(moText("Flow velocity: ") + FloatToStr(flow_velocity));

    //MODebug2->Push(moText("Flow velocity: ") + FloatToStr(flow_velocity));

    if (flow_mode == 0)
	{
        // horizontal flow.

		flow_coord = flow_coord + flow_velocity;

		if ((flow_coord < minx) || (maxx < flow_coord))
		{
			if (0 < flow_velocity)
			{
				idx1 = idx0;
				idx0 = (idx0 + 1) % Pimages.GetValuesCount();
			}
			else
			{
                idx0 = idx1;
                idx1 = (idx1 + 1) % Pimages.GetValuesCount();
			}
			/*
            tex0 = Images.GetGLId(idx0);
            tex1 = Images.GetGLId(idx1);
            */
            tex0 = Pimages.GetValue(idx0).GetSubValue(0).GetGLId( &state.tempo );
            tex1 = Pimages.GetValue(idx1).GetSubValue(0).GetGLId( &state.tempo );

			if (flow_coord < minx) flow_coord = maxx;
			if (maxx < flow_coord) flow_coord = minx;
		}

        t = 1.0 - (flow_coord  - minx) / (maxx - minx);

		glBindTexture(GL_TEXTURE_2D, tex0);
        glBegin(GL_QUADS);
            glTexCoord2f(t, 0.0);
            glVertex2f(minx, miny);

            glTexCoord2f(t, -1.0);
            glVertex2f(minx, maxy);

            glTexCoord2f(1.0, -1.0);
            glVertex2f(flow_coord, maxy);

            glTexCoord2f(1.0, 0.0);
            glVertex2f(flow_coord, miny);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, tex1);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0, 0.0);
            glVertex2f(flow_coord, miny);

            glTexCoord2f(0.0, -1.0);
            glVertex2f(flow_coord, maxy);

		    glTexCoord2f(t, -1.0);
            glVertex2f(maxx, maxy);

            glTexCoord2f(t, 0.0);
            glVertex2f(maxx, miny);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);

	}
	else
	{

        // vertical flow.
        flow_coord = flow_coord + flow_velocity;

		if ((flow_coord < miny) || (maxy < flow_coord))
		{
			if (0 < flow_velocity)
			{
				idx1 = idx0;
				idx0 = (idx0 + 1) % Pimages.GetValuesCount();
			}
			else
			{
                idx0 = idx1;
                idx1 = (idx1 + 1) % Pimages.GetValuesCount();
			}
			/*
            tex0 = Images.GetGLId(idx0);
            tex1 = Images.GetGLId(idx1);
            */
            tex0 = Pimages.GetValue(idx0).GetSubValue(0).GetGLId( &state.tempo );
            tex1 = Pimages.GetValue(idx1).GetSubValue(0).GetGLId( &state.tempo );

			if (flow_coord < miny) flow_coord = maxy;
			if (maxy < flow_coord) flow_coord = miny;
		}

        t = 1.0 - (flow_coord  - miny) / (maxy - miny);

		glBindTexture(GL_TEXTURE_2D, tex0);
		//glColor4f(1.0,0.0,0.0,1.0);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0, 1-t);
            glVertex2f(minx, miny);

            glTexCoord2f(0.0, 0.0);
            glVertex2f(minx, flow_coord);

            glTexCoord2f(1.0, 0.0);
            glVertex2f(maxx, flow_coord);

            glTexCoord2f(1.0, 1-t);
            glVertex2f(maxx, miny);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, tex1);
		//glColor4f(0.0,1.0,0.0,1.0);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0, 1.0);
            glVertex2f(minx, flow_coord);

            glTexCoord2f(0.0, 1-t);
            glVertex2f(minx, maxy);

		    glTexCoord2f(1.0, 1-t);
            glVertex2f(maxx, maxy);

            glTexCoord2f(1.0, 1.0);
            glVertex2f(maxx, flow_coord);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
	}



    // Dejamos todo como lo encontramos //
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
}


void moEffectImageFlow3D::Interaction( moIODeviceManager *IODeviceManager ) {
}

MOboolean moEffectImageFlow3D::Finish()
{
    return PreFinish();
}

void moEffectImageFlow3D::updateParameters()
{
	//flow_velocity0 = m_Config[moR(IMAGEFLOW3D_VELOCITY)].GetData()->Fun()->Eval(state.tempo.ang);
	flow_velocity0 = m_Config.Eval(moR(IMAGEFLOW3D_VELOCITY),state.tempo.ang );
	flow_mode = m_Config.Int(moR(IMAGEFLOW3D_FLOW_MODE) );
}

moConfigDefinition *
moEffectImageFlow3D::GetDefinition( moConfigDefinition *p_configdefinition ) {
	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("images"), MO_PARAM_TEXTURE, IMAGEFLOW3D_IMAGES );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, IMAGEFLOW3D_BLENDING );
	p_configdefinition->Add( moText("velocity"), MO_PARAM_FUNCTION, IMAGEFLOW3D_VELOCITY, moValue("0.001", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("flow_mode"), MO_PARAM_NUMERIC, IMAGEFLOW3D_FLOW_MODE, moValue("0", "INT").Ref() );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, IMAGEFLOW3D_TRANSLATEX, moValue("0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, IMAGEFLOW3D_TRANSLATEY, moValue("0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, IMAGEFLOW3D_TRANSLATEZ, moValue("-1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, IMAGEFLOW3D_ROTATEX, moValue("0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, IMAGEFLOW3D_ROTATEY, moValue("0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, IMAGEFLOW3D_ROTATEZ, moValue("0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, IMAGEFLOW3D_SCALEX, moValue("1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, IMAGEFLOW3D_SCALEY, moValue("1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, IMAGEFLOW3D_SCALEZ, moValue("1.0", MO_VALUE_FUNCTION).Ref() );

	return p_configdefinition;
}
