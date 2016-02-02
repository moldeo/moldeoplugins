/*******************************************************************************

                               moEffectIcon.cpp

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

#include "moEffectIcon.h"
#include "moFilterManager.h"

//========================
//  Factory
//========================

moEffectIconFactory *m_EffectIconoFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if (m_EffectIconoFactory==NULL)
		m_EffectIconoFactory = new moEffectIconFactory();
	return (moEffectFactory*) m_EffectIconoFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectIconoFactory;
	m_EffectIconoFactory = NULL;
}

moEffect*  moEffectIconFactory::Create() {
	return new moEffectIcon();
}

void moEffectIconFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================
moEffectIcon::moEffectIcon() {

	SetName("icon");

}

moEffectIcon::~moEffectIcon() {
	Finish();
}

moConfigDefinition *
moEffectIcon::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, ICON_TEXTURE, moValue( "default", MO_VALUE_TXT) );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, ICON_BLENDING, moValue( "0", MO_VALUE_NUM ).Ref() );
	p_configdefinition->Add( moText("width"), MO_PARAM_FUNCTION, ICON_WIDTH, moValue( "1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("height"), MO_PARAM_FUNCTION, ICON_HEIGHT, moValue( "1.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, ICON_TRANSLATEX, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, ICON_TRANSLATEY, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("rotate"), MO_PARAM_ROTATEZ, ICON_ROTATE );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, ICON_SCALEX );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, ICON_SCALEY );
	return p_configdefinition;
}

MOboolean moEffectIcon::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( ICON_ALPHA, moText("alpha") );
	moDefineParamIndex( ICON_COLOR, moText("color") );
	moDefineParamIndex( ICON_SYNC, moText("syncro") );
	moDefineParamIndex( ICON_PHASE, moText("phase") );
	moDefineParamIndex( ICON_TEXTURE, moText("texture") );
	moDefineParamIndex( ICON_BLENDING, moText("blending") );
	moDefineParamIndex( ICON_WIDTH, moText("width") );
	moDefineParamIndex( ICON_HEIGHT, moText("height") );
	moDefineParamIndex( ICON_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( ICON_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( ICON_ROTATE, moText("rotate") );
	moDefineParamIndex( ICON_SCALEX, moText("scalex") );
	moDefineParamIndex( ICON_SCALEY, moText("scaley") );
	moDefineParamIndex( ICON_INLET, moText("inlet") );
	moDefineParamIndex( ICON_OUTLET, moText("outlet") );

    Tx = 0.0;
    Ty = 0.0;
    iTx = 0.0;
    iTy = 0.0;

    Sx = 1.0;
    Sy = 1.0;

	return true;
}


void moEffectIcon::Draw( moTempo* tempogral, moEffectState* parentstate )
{

    float ancho, alto;

    BeginDraw( tempogral, parentstate);

    moRenderManager* mRender = m_pResourceManager->GetRenderMan();
    moGLManager* mGL = m_pResourceManager->GetGLMan();

    if (mRender==NULL || mGL==NULL) return;

    int w = mRender->ScreenWidth();
    int h = mRender->ScreenHeight();

    mGL->SetDefaultOrthographicView( w, h);

    glEnable(GL_ALPHA);
    glDisable(GL_DEPTH_TEST);       // Disables Depth Testing
    glDepthMask(GL_FALSE);
    //glDisable(GL_DEPTH);

    ancho = m_Config.Eval( moR(ICON_WIDTH));
    alto = m_Config.Eval( moR(ICON_HEIGHT));

    //SetColor( m_Config[moR(ICON_COLOR)][MO_SELECTED], m_Config[moR(ICON_ALPHA)][MO_SELECTED], state );
    SetColor( m_Config[moR(ICON_COLOR)], m_Config[moR(ICON_ALPHA)], m_EffectState );

    SetBlending( (moBlendingModes) m_Config.Int( moR(ICON_BLENDING) ) );

    moData* TD = m_Config[moR(ICON_TEXTURE)].GetData();
    if (TD && TD->Texture()==NULL) UpdateConnectors();
    glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(ICON_TEXTURE), &m_EffectState.tempo ) );

    moPlaneGeometry IconQuad( ancho, alto, 1, 1 );
    moMaterial Material;
    Material.m_Map = TD->Texture();
    //Material.m_Color = moColor( 1.0, 1.0, 1.0 );
    moVector4d color = m_Config.EvalColor( moR(ICON_COLOR) );
    Material.m_Color = moColor( color.X(), color.Y(), color.Z() );

    moGLMatrixf Model;
    Model.MakeIdentity();
    Model.Scale( m_Config.Eval( moR(ICON_SCALEX)), m_Config.Eval( moR(ICON_SCALEY)), 1.0 );
    Model.Rotate( m_Config.Eval( moR(ICON_ROTATE))*moMathf::DEG_TO_RAD, 0.0, 0.0, 1.0 );
    Model.Translate( m_Config.Eval( moR(ICON_TRANSLATEX)), m_Config.Eval( moR(ICON_TRANSLATEY)), 0.0 );

    moMesh Mesh( IconQuad, Material );
    Mesh.SetModelMatrix( Model );


    moCamera3D Camera3D;
    Camera3D = mGL->GetProjectionMatrix();

#ifndef OPENGLESV2

    /// Draw //
    glTranslatef(  m_Config.Eval( moR(ICON_TRANSLATEX)) + Tx,
                   m_Config.Eval( moR(ICON_TRANSLATEY)) + Ty,
                   0.0);

    ///solo rotamos en el eje Z (0,0,1) ya que siempre estaremos perpedicular al plano (X,Y)
    glRotatef(  m_Config.Eval( moR(ICON_ROTATE)), 0.0, 0.0, 1.0 );

    glScalef(   m_Config.Eval( moR(ICON_SCALEX))*Sx,
                m_Config.Eval( moR(ICON_SCALEY))*Sy,
                  1.0);

    glBegin(GL_QUADS);
      glTexCoord2f( 0.0, 1.0 );
      glVertex2f( -0.5*ancho, -0.5*alto);

      glTexCoord2f( 1.0, 1.0 );
      glVertex2f(  0.5*ancho, -0.5*alto);

      glTexCoord2f( 1.0, 0.0 );
      glVertex2f(  0.5*ancho,  0.5*alto);

      glTexCoord2f( 0.0, 0.0 );
      glVertex2f( -0.5*ancho,  0.5*alto);
    glEnd();

#else
    mRender->Render( &Mesh, &Camera3D );
#endif
    int Tracker = GetInletIndex("TRACKERKLT" );
    if (Tracker > -1) {
      moInlet* pInlet = GetInlets()->Get(Tracker);
      if (pInlet ) {
          if (pInlet->Updated()) {

            moTrackerSystemData* pData = (moTrackerSystemData*) pInlet->GetData()->Pointer();
            if (pData) {
                //MODebug2->Message("Icon > Draw > Drawing Features");
                pData->DrawFeatures( 1.0, -1.0, 0.5, 0.5 );

                /* chequeando info
                MODebug2->Push( moText("Icon varX: ") + FloatToStr( pData->GetVariance().X())
                       + moText(" varY: ") + FloatToStr(pData->GetVariance().Y()) );
                       */
            }
          }
      }
    }
#ifndef OPENGLESV2

    glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
    glPopMatrix();										// Restore The Old Projection Matrix
    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    glPopMatrix();										// Restore The Old Projection Matrix

#endif
    EndDraw();
}

MOboolean moEffectIcon::Finish()
{
    return PreFinish();
}


void moEffectIcon::Interaction( moIODeviceManager *IODeviceManager ) {

	moDeviceCode *temp;
	MOint did,cid,state,valor;

	moEffect::Interaction( IODeviceManager );

    inc_iTx = 0;
    inc_iTy = 0;

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
				case MO_ICON_TRANSLATE_X:
					Tx = ((float) (valor- m_pResourceManager->GetRenderMan()->RenderWidth()/2) / (float) m_pResourceManager->GetRenderMan()->RenderWidth());
					iTx = 0;//resetear incremento
					MODebug2->Push(IntToStr(valor));
					break;
				case MO_ICON_TRANSLATE_Y:
                    Ty = ( m_pResourceManager->GetRenderMan()->RenderHeight() - valor) - m_pResourceManager->GetRenderMan()->RenderHeight()/2;
					Ty = ((float) Ty / (float) m_pResourceManager->GetRenderMan()->RenderHeight());
					iTy = 0;//resetear incremento
					MODebug2->Push(IntToStr(valor));
					break;

				case MO_ICON_SCALE_X:
					Sx+=((float) valor / (float) m_pResourceManager->GetRenderMan()->RenderWidth()/2);
					MODebug2->Push(IntToStr(valor));
					break;
				case MO_ICON_SCALE_Y:
                    //float _sy = m_pResourceManager->GetRenderMan()->RenderHeight() - valor;
					Sy-=((float) valor / (float) m_pResourceManager->GetRenderMan()->RenderHeight()/2);
					MODebug2->Push(IntToStr(valor));
					break;

				case MO_ICON_INC_TRANSLATE_X:
                    if (valor>0) inc_iTx = 1;
                    else if (valor<0) inc_iTx = -1;
                    if (inc_iTx!=inc_iTx_ant) {
                        iTx = 0;
                    }
					iTx =((float) valor / (float) m_pResourceManager->GetRenderMan()->RenderWidth());
					MODebug2->Push(IntToStr(valor));
					Tx+= iTx / 8.0;
					break;
				case MO_ICON_INC_TRANSLATE_Y:
                    //float _sy = m_pResourceManager->GetRenderMan()->RenderHeight() - valor;
                    if (valor>0) inc_iTy = 1;
                    else if (valor<0) inc_iTy = -1;
                    if (inc_iTy!=inc_iTy_ant) {
                        iTy = 0;
                    }
					iTy = - ((float) valor / (float) m_pResourceManager->GetRenderMan()->RenderHeight() );
					MODebug2->Push(IntToStr(valor));
					Ty+= iTy/8.0;
					break;
			}
		temp = temp->next;
		}
	}

    inc_iTx_ant = inc_iTx;
    inc_iTy_ant = inc_iTy;

}


void
moEffectIcon::Update( moEventList *Events ) {

	//get the pointer from the Moldeo Object sending it...
	moMoldeoObject::Update(Events);


}
