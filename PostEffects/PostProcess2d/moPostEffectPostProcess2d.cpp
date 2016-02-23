/*******************************************************************************

                               moPostEffectPostProcess2d.cpp

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *   (at your option) any later version.                                    *
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

  Copyright (C) 2006 Fabricio Costa

  Authors:
  Fabricio Costa


*******************************************************************************/

#include "moPostEffectPostProcess2d.h"

//========================
//  Factory
//========================

moPostEffectPostProcess2dFactory *m_EffectIconoFactory = NULL;

MO_PLG_API moPostEffectFactory* CreatePostEffectFactory(){
	if (m_EffectIconoFactory==NULL)
		m_EffectIconoFactory = new moPostEffectPostProcess2dFactory();
	return (moPostEffectFactory*) m_EffectIconoFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectIconoFactory;
	m_EffectIconoFactory = NULL;
}

moPostEffect*  moPostEffectPostProcess2dFactory::Create() {
	return new moPostEffectPostProcess2d();
}

void moPostEffectPostProcess2dFactory::Destroy(moPostEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================
moPostEffectPostProcess2d::moPostEffectPostProcess2d() {

	SetName("postprocess2d");

}

moPostEffectPostProcess2d::~moPostEffectPostProcess2d() {
	Finish();
}

MOboolean moPostEffectPostProcess2d::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( PP2D_INLET, moText("inlet") );
	moDefineParamIndex( PP2D_OUTLET, moText("outlet") );
	moDefineParamIndex( PP2D_SCRIPT, moText("script") );
	moDefineParamIndex( PP2D_ALPHA, moText("alpha") );
	moDefineParamIndex( PP2D_COLOR, moText("color") );
	moDefineParamIndex( PP2D_SYNC, moText("syncro") );
	moDefineParamIndex( PP2D_PHASE, moText("phase") );
	moDefineParamIndex( PP2D_TEXTURE, moText("texture") );
	moDefineParamIndex( PP2D_BLENDING, moText("blending") );
	moDefineParamIndex( PP2D_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( PP2D_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( PP2D_ROTATE, moText("rotate") );
	moDefineParamIndex( PP2D_SCALEX, moText("scalex") );
	moDefineParamIndex( PP2D_SCALEY, moText("scaley") );


    Tx = Ty  = Rz = 0.0;
	Sx = Sy = 1.0;

	return true;
}


void moPostEffectPostProcess2d::Draw( moTempo* tempogral, moEffectState* parentstate )
{

    float ancho = 0.5, alto=0.5;

    PreDraw( tempogral, parentstate);

    // Guardar y resetar la matriz de vista del modelo //
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glPushMatrix();                                     // Store The Modelview Matrix
	glLoadIdentity();									// Reset The View

    // Cambiar la proyeccion para una vista ortogonal //
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

    m_pResourceManager->GetGLMan()->SetDefaultOrthographicView();
    alto = 0.5 * m_pResourceManager->GetRenderMan()->ScreenProportion();
	//gluPerspective(45.0f, 1/prop, 0.1f ,4000.0f);

    // Funcion de blending y de alpha channel //
    glEnable(GL_BLEND);

	glDisable(GL_ALPHA);

    /// Draw //
    glTranslatef(  m_Config.Eval( moR(PP2D_TRANSLATEX)) + Tx,
                   m_Config.Eval( moR(PP2D_TRANSLATEY)) + Ty,
                   0.0);

    ///solo rotamos en el eje Z (0,0,1) ya que siempre estaremos perpedicular al plano (X,Y)
    glRotatef(  m_Config.Eval( moR(PP2D_ROTATE)), 0.0, 0.0, 1.0 );

    glScalef(   m_Config.Eval( moR(PP2D_SCALEX))*Sx,
                m_Config.Eval( moR(PP2D_SCALEY))*Sy,
                  1.0);

  SetColor( m_Config[moR(PP2D_COLOR)], m_Config[moR(PP2D_ALPHA)], m_EffectState );

  glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(PP2D_TEXTURE), &m_EffectState.tempo ) );

  SetBlending( (moBlendingModes) m_Config.Int( moR(PP2D_BLENDING) ) );

	glBegin(GL_QUADS);
		glTexCoord2f( 0.0, 0.0);
		glVertex2f( -ancho, -alto);

		glTexCoord2f( 1.0, 0.0);
		glVertex2f(  ancho, -alto);

		glTexCoord2f( 1.0, 1.0);
		glVertex2f(  ancho,  alto);

		glTexCoord2f( 0.0, 1.0);
		glVertex2f( -ancho,  alto);
	glEnd();

    // Dejamos todo como lo encontramos //
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix


}

MOboolean moPostEffectPostProcess2d::Finish()
{
    return PreFinish();
}

void moPostEffectPostProcess2d::Interaction( moIODeviceManager *IODeviceManager ) {

	moDeviceCode *temp;
	MOint did,cid,state,valor;

	moPostEffect::Interaction( IODeviceManager );

	if (devicecode!=NULL)
	for(int i=0; i<ncodes; i++) {

		temp = devicecode[i].First;

		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			state = IODeviceManager->IODevices().GetRef(did)->GetStatus(cid);
			valor = IODeviceManager->IODevices().GetRef(did)->GetValue(cid);
			if (state)
			switch(i) {
				case MO_PP2D_TRANSLATE_X:
					Tx = valor;
					MODebug2->Push(IntToStr(valor));
					break;
				case MO_PP2D_TRANSLATE_Y:
					Ty = m_pResourceManager->GetRenderMan()->RenderHeight() - valor;
					MODebug2->Push(IntToStr(valor));
					break;
				case MO_PP2D_SCALE_X:
					Sx+=((float) valor / (float) 256.0);
					MODebug2->Push(IntToStr(valor));
					break;
				case MO_PP2D_SCALE_Y:
					Sy+=((float) valor / (float) 256.0);
					MODebug2->Push(IntToStr(valor));
					break;
			}
		temp = temp->next;
		}
	}

}

moConfigDefinition *
moPostEffectPostProcess2d::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, PP2D_TEXTURE, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, PP2D_BLENDING, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, PP2D_TRANSLATEX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, PP2D_TRANSLATEY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotate"), MO_PARAM_ROTATEZ, PP2D_ROTATE );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, PP2D_SCALEX );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, PP2D_SCALEY );
	return p_configdefinition;
}



void
moPostEffectPostProcess2d::Update( moEventList *Events ) {

	//get the pointer from the Moldeo Object sending it...
	moMoldeoObject::Update(Events);


}
