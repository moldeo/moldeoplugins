/*******************************************************************************

                                moEffectImage.cpp

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

#include "moEffectImage.h"

//========================
//  Factory
//========================

moEffectImageFactory *m_EffectImageFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectImageFactory==NULL)
		m_EffectImageFactory = new moEffectImageFactory();
	return(moEffectFactory*) m_EffectImageFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectImageFactory;
	m_EffectImageFactory = NULL;
}

moEffect*  moEffectImageFactory::Create() {
	return new moEffectImage();
}

void moEffectImageFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectImage::moEffectImage() {
	SetName("image");
}

moEffectImage::~moEffectImage() {
	Finish();
}

moConfigDefinition *
moEffectImage::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, IMAGE_TEXTURE, moValue( "default", MO_VALUE_TXT ).Ref() );
	/*p_configdefinition->Add( moText("filters"), MO_PARAM_FILTER, IMAGE_FILTER );*/
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, IMAGE_BLENDING, moValue( "0", MO_VALUE_NUM).Ref() );
	p_configdefinition->Add( moText("pos_text_x"), MO_PARAM_FUNCTION, IMAGE_POSTEXX, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("pos_text_y"), MO_PARAM_FUNCTION, IMAGE_POSTEXY, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("anc_text_x"), MO_PARAM_FUNCTION, IMAGE_ANCTEXX, moValue( "2.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("alt_text_y"), MO_PARAM_FUNCTION, IMAGE_ALTTEXY, moValue( "2.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("pos_cuad_x"), MO_PARAM_FUNCTION, IMAGE_POSCUADX, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("pos_cuad_y"), MO_PARAM_FUNCTION, IMAGE_POSCUADY, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("anc_cuad_x"), MO_PARAM_FUNCTION, IMAGE_ANCCUADX, moValue( "1.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("alt_cuad_y"), MO_PARAM_FUNCTION, IMAGE_ALTCUADY, moValue( "1.0", MO_VALUE_FUNCTION ).Ref() );
	return p_configdefinition;
}

MOboolean
moEffectImage::Init() {

    if (!PreInit()) return false;

    // Hacer la inicializacion de este efecto en particular.
	moDefineParamIndex( IMAGE_ALPHA, moText("alpha") );
	moDefineParamIndex( IMAGE_COLOR, moText("color") );
	moDefineParamIndex( IMAGE_SYNC, moText("syncro") );
	moDefineParamIndex( IMAGE_PHASE, moText("phase") );
	moDefineParamIndex( IMAGE_BLENDING, moText("blending") );
	moDefineParamIndex( IMAGE_TEXTURE, moText("texture") );
	/*moDefineParamIndex( IMAGE_FILTER, moText("filters") );*/
	moDefineParamIndex( IMAGE_POSTEXX, moText("pos_text_x") );
	moDefineParamIndex( IMAGE_POSTEXY, moText("pos_text_y") );
	moDefineParamIndex( IMAGE_ANCTEXX, moText("anc_text_x") );
	moDefineParamIndex( IMAGE_ALTTEXY, moText("alt_text_y") );
	moDefineParamIndex( IMAGE_POSCUADX, moText("pos_cuad_x") );
	moDefineParamIndex( IMAGE_POSCUADY, moText("pos_cuad_y") );
	moDefineParamIndex( IMAGE_ANCCUADX, moText("anc_cuad_x") );
	moDefineParamIndex( IMAGE_ALTCUADY, moText("alt_cuad_y") );
	moDefineParamIndex( IMAGE_INLET, moText("inlet") );
	moDefineParamIndex( IMAGE_OUTLET, moText("outlet") );

	return true;
}

void moEffectImage::Draw( moTempo* tempogral,moEffectState* parentstate)
{

    PreDraw( tempogral, parentstate);

    MOint Blending;
	MOdouble PosTextX,  AncTextX,  PosTextY,  AltTextY;
    MOdouble PosTextX0, PosTextX1, PosTextY0, PosTextY1;
    MOdouble PosCuadX,  AncCuadX,  PosCuadY,  AltCuadY;
    MOdouble PosCuadX0, PosCuadX1, PosCuadY0, PosCuadY1;

    moRenderManager* mRender = m_pResourceManager->GetRenderMan();
    moGLManager* mGL = m_pResourceManager->GetGLMan();

    if (mRender==NULL || mGL==NULL) return;

    int w = mRender->ScreenWidth();
    int h = mRender->ScreenHeight();

    moTexture* pImage = (moTexture*) m_Config[moR(IMAGE_TEXTURE)].GetData()->Pointer();

#ifndef OPENGLESV2
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
#endif

	//DIBUJAR
	glEnable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing

	SetBlending( (moBlendingModes) m_Config[moR(IMAGE_BLENDING)][MO_SELECTED][0].Int() );

    SetColor( m_Config[moR(IMAGE_COLOR)], m_Config[moR(IMAGE_ALPHA)], m_EffectState );

	//source: GL_ZERO, GL_ONE, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA, and GL_SRC_ALPHA_SATURATE
	//destination: GL_ZERO, GL_ONE, GL_SCR_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR, GL_DST_ALPHA, and GL_ONE_MINUS_DST_ALPHA.

#ifndef OPENGLESV2
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();
	glOrtho(0,1.0,0,1.0,-1,1);
	// Reset The Projection Matrix
	// Set Up An Ortho Screen
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
#endif

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glEnable(GL_TEXTURE_2D);
    //glBindTexture( GL_TEXTURE_2D, m_Config[moR(IMAGE_TEXTURE)].GetData()->GetGLId(&m_EffectState.tempo, 1, &FilterParams ) );
    //MODebug2->Push( FloatToStr(m_EffectState.tempo.ang) );
    glBindTexture( GL_TEXTURE_2D,  m_Config.GetGLId( moR(IMAGE_TEXTURE), &m_EffectState.tempo ) );

    PosTextX = m_Config.Eval( moR(IMAGE_POSTEXX), m_EffectState.tempo.ang);
    AncTextX = m_Config.Eval( moR(IMAGE_ANCTEXX), m_EffectState.tempo.ang);
    PosTextY = m_Config.Eval( moR(IMAGE_POSTEXY), m_EffectState.tempo.ang);
    AltTextY = m_Config.Eval( moR(IMAGE_ALTTEXY), m_EffectState.tempo.ang);

    PosCuadX = m_Config.Eval( moR(IMAGE_POSCUADX), m_EffectState.tempo.ang);
    AncCuadX = m_Config.Eval( moR(IMAGE_ANCCUADX), m_EffectState.tempo.ang);
    PosCuadY = m_Config.Eval( moR(IMAGE_POSCUADY), m_EffectState.tempo.ang);
    AltCuadY = m_Config.Eval( moR(IMAGE_ALTCUADY), m_EffectState.tempo.ang);

    if (pImage) {
        PosTextX0 = PosTextX * pImage->GetMaxCoordS();
        PosTextX1 =(PosTextX + AncTextX)* pImage->GetMaxCoordS();
        PosTextY0 =(1 - PosTextY)*pImage->GetMaxCoordT();
        PosTextY1 =(1 - PosTextY - AltTextY)*pImage->GetMaxCoordT();
    } else {
        PosTextX0 = PosTextX;
        PosTextX1 =(PosTextX + AncTextX);
        PosTextY0 =(1 - PosTextY);
        PosTextY1 =(1 - PosTextY - AltTextY);
    }
    PosCuadX0 = PosCuadX;
    PosCuadX1 = PosCuadX + AncCuadX;
    PosCuadY1 = PosCuadY;
    PosCuadY0 = PosCuadY + AltCuadY;

#ifndef OPENGLESV2
/*
	glBegin(GL_QUADS);
		glTexCoord2f( PosTextX0, PosTextY1);
		glVertex2f ( PosCuadX0, PosCuadY0);

		glTexCoord2f( PosTextX1, PosTextY1);
		glVertex2f ( PosCuadX1, PosCuadY0);

        glTexCoord2f( PosTextX1, PosTextY0);
		glVertex2f ( PosCuadX1, PosCuadY1);

		glTexCoord2f( PosTextX0, PosTextY0);
		glVertex2f ( PosCuadX0, PosCuadY1);
	glEnd();
*/
#else
#endif

    moPlaneGeometry ImageQuad( AncCuadX, AltCuadY, 1, 1 );
    moMaterial Material;
    Material.m_Map = pImage;
    //Material.m_Color = moColor( 1.0, 1.0, 1.0 );
    moVector4d color = m_Config.EvalColor( moR(IMAGE_COLOR) );
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
    mRender->Render( &Mesh, &Camera3D );

#ifndef OPENGLESV2
    glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
#endif
}

void moEffectImage::Interaction( moIODeviceManager *IODeviceManager ) {
	moDeviceCode *temp;
	MOint did,cid,state,valor;

	moEffect::Interaction( IODeviceManager );
/*
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
				case MO_FILTER_PAR1:
					FilterParams.par_flt1 = ((float) valor / (float) 800.0);
					//MODebug2->Push("par1: " + FloatToStr(valor));
					break;

			}
		temp = temp->next;
		}
	}
	*/
}


MOboolean moEffectImage::Finish()
{
    return PreFinish();
}
