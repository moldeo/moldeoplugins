/*******************************************************************************

                        moMasterEffectPanel.cpp

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

#include "moMasterEffectPanel.h"

//========================
//  Factory
//========================

moMasterEffectPanelFactory *m_MasterEffectPanelFactory = NULL;

MO_PLG_API moMasterEffectFactory* CreateMasterEffectFactory(){
	if(m_MasterEffectPanelFactory==NULL)
		m_MasterEffectPanelFactory = new moMasterEffectPanelFactory();
	return(moMasterEffectFactory*) m_MasterEffectPanelFactory;
}

MO_PLG_API void DestroyMasterEffectFactory(){
	delete m_MasterEffectPanelFactory;
	m_MasterEffectPanelFactory = NULL;
}

moMasterEffect*  moMasterEffectPanelFactory::Create() {
	return new moMasterEffectPanel();
}

void moMasterEffectPanelFactory::Destroy(moMasterEffect* fx) {
	delete fx;
}

//========================
//  EfectoMaestro
//========================
moMasterEffectPanel::moMasterEffectPanel() {
	m_pEffectManager = NULL;
	consolestate = NULL;
	SetName("panel");
}

moMasterEffectPanel::~moMasterEffectPanel() {
	Finish();
}

moConfigDefinition *
moMasterEffectPanel::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );

	/** transformation 3d */

	/**
        p_configdefinition->AddClipViewport();

	*/

    p_configdefinition->Add( moText("viewport_left"), MO_PARAM_FUNCTION, PANEL_VIEWPORT_LEFT, moValue("0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("viewport_top"), MO_PARAM_FUNCTION, PANEL_VIEWPORT_TOP, moValue("0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("viewport_width"), MO_PARAM_FUNCTION, PANEL_VIEWPORT_WIDTH, moValue("1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("viewport_height"), MO_PARAM_FUNCTION, PANEL_VIEWPORT_HEIGHT, moValue("1.0", MO_VALUE_FUNCTION).Ref() );

    /**
        p_configdefinition->Add2dTransform( 0, 4 );
        p_configdefinition->Add3dTransform( 0, 9 );

    */
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, PANEL_TRANSLATEX, moValue("0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, PANEL_TRANSLATEY, moValue("0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, PANEL_TRANSLATEZ, moValue("-2.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, PANEL_ROTATEX, moValue("0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, PANEL_ROTATEY, moValue("0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, PANEL_ROTATEZ, moValue("0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, PANEL_SCALEX, moValue("1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, PANEL_SCALEY, moValue("1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, PANEL_SCALEZ, moValue("1.0", MO_VALUE_FUNCTION).Ref() );


    /**
        p_configdefinition->AddTexturing("background_texture", PANEL_TEXTURE, PANEL_BLENDING );
    */

	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, PANEL_TEXTURE, moValue("default", MO_VALUE_TXT) );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, PANEL_BLENDING, moValue( "0", MO_VALUE_NUM ).Ref() );


    /**
    tabla de panel:
        pre: 4
        post: 4
        columnas: 10
        filas: 4
    */

     p_configdefinition->Add( moText("rows"), MO_PARAM_NUMERIC, PANEL_ROWS, moValue( "4", MO_VALUE_NUM ).Ref() );
     p_configdefinition->Add( moText("columns"), MO_PARAM_NUMERIC, PANEL_COLUMNS, moValue( "4", MO_VALUE_NUM ).Ref() );


    /**
        icons for Effects State
    */
     //p_configdefinition->Add( moText("icon_color"), MO_PARAM_TEXTURE, PANEL_ICON_COLOR, moValue("default", MO_VALUE_TXT) );
     p_configdefinition->Add( moText("icon_onoff"), MO_PARAM_TEXTURE, PANEL_ICON_ONOFF, moValue("default", MO_VALUE_TXT) );
     p_configdefinition->Add( moText("icon_alpha"), MO_PARAM_TEXTURE, PANEL_ICON_ALPHA, moValue("default", MO_VALUE_TXT) );
     p_configdefinition->Add( moText("icon_selected"), MO_PARAM_TEXTURE, PANEL_ICON_SELECTED, moValue("default", MO_VALUE_TXT) );
     p_configdefinition->Add( moText("icon_tempo"), MO_PARAM_TEXTURE, PANEL_ICON_TEMPO, moValue("default", MO_VALUE_TXT) );

     /** BACKGROUNDS */
     p_configdefinition->Add( moText("icon_background"), MO_PARAM_TEXTURE, PANEL_ICON_BACKGROUND, moValue("default", MO_VALUE_TXT) );
     p_configdefinition->Add( moText("icon_class_background"), MO_PARAM_TEXTURE, PANEL_ICON_CLASS_BACKGROUND, moValue("default", MO_VALUE_TXT) );
     p_configdefinition->Add( moText("icon_separation"), MO_PARAM_TEXTURE, PANEL_ICON_SEPARATION, moValue("default", MO_VALUE_TXT) );
     p_configdefinition->Add( moText("icon_class_separation"), MO_PARAM_TEXTURE, PANEL_ICON_CLASS_SEPARATION, moValue("default", MO_VALUE_TXT) );

     p_configdefinition->Add( moText("codes"), MO_PARAM_TEXT );

	return p_configdefinition;
}


MOboolean moMasterEffectPanel::Init()
{
    if (!PreInit()) return false;

	moDefineParamIndex( PANEL_INLET, moText("inlet") );
	moDefineParamIndex( PANEL_OUTLET, moText("outlet") );
	moDefineParamIndex( PANEL_SCRIPT, moText("script") );

	moDefineParamIndex( PANEL_ALPHA, moText("alpha") );
	moDefineParamIndex( PANEL_COLOR, moText("color") );
	moDefineParamIndex( PANEL_SYNC, moText("syncro") );
	moDefineParamIndex( PANEL_PHASE, moText("phase") );

	moDefineParamIndex( PANEL_TEXTURE, moText("texture") );
	moDefineParamIndex( PANEL_BLENDING, moText("blending") );

	moDefineParamIndex( PANEL_VIEWPORT_LEFT, moText("viewport_left") );
	moDefineParamIndex( PANEL_VIEWPORT_TOP, moText("viewport_top") );
	moDefineParamIndex( PANEL_VIEWPORT_WIDTH, moText("viewport_width") );
	moDefineParamIndex( PANEL_VIEWPORT_HEIGHT, moText("viewport_height") );


	moDefineParamIndex( PANEL_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( PANEL_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( PANEL_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( PANEL_ROTATEX, moText("rotatex") );
	moDefineParamIndex( PANEL_ROTATEY, moText("rotatey") );
	moDefineParamIndex( PANEL_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( PANEL_SCALEX, moText("scalex") );
	moDefineParamIndex( PANEL_SCALEY, moText("scaley") );
	moDefineParamIndex( PANEL_SCALEZ, moText("scalez") );


    moDefineParamIndex( PANEL_ROWS, moText("rows") );
	moDefineParamIndex( PANEL_COLUMNS, moText("columns") );

	moDefineParamIndex( PANEL_ICON_ONOFF, moText("icon_onoff") );
    moDefineParamIndex( PANEL_ICON_ALPHA, moText("icon_alpha") );
    moDefineParamIndex( PANEL_ICON_SELECTED, moText("icon_selected") );
    moDefineParamIndex( PANEL_ICON_TEMPO, moText("icon_tempo") );

	moDefineParamIndex( PANEL_ICON_BACKGROUND, moText("icon_background") );
	moDefineParamIndex( PANEL_ICON_CLASS_BACKGROUND, moText("icon_class_background") );

	moDefineParamIndex( PANEL_ICON_SEPARATION, moText("icon_separation") );
	moDefineParamIndex( PANEL_ICON_CLASS_SEPARATION, moText("icon_class_separation") );


	return true;
}


void moMasterEffectPanel::DrawObject( float x, float y, float w, float h, moEffect* p_Fx, float r, float g, float b, float alpha ) {


    if (!p_Fx)
        return;


        glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(PANEL_ICON_BACKGROUND), &(m_EffectState.tempo) ) );

        //if ( p_Fx->Activated() ) {
            glColor4f( 1.0, 1.0, 1.0,  alpha );
        //} else {
        //    glColor4f( 0.0, 0.0, 0.0,alpha );
        //}

        glBegin(GL_QUADS);
            glTexCoord2f( 0.0, 0.0);
            glVertex2f( x-w/2.0, y+h/2 );

            glTexCoord2f( 1.0, 0.0);
            glVertex2f(  x+w/2.0, y+h/2 );

            glTexCoord2f( 1.0, 1.0);
            glVertex2f(  x+w/2.0, y-h/2 );

            glTexCoord2f( 0.0, 1.0);
            glVertex2f( x-w/2.0, y-h/2 );
        glEnd();


        glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(PANEL_ICON_ONOFF), &(m_EffectState.tempo) ) );

        if ( p_Fx->Activated() ) {
            glColor4f( 1.0, 0.0, 0.0,alpha );
        } else {
            glColor4f( 0.0, 0.0, 0.0,alpha );
        }

        glBegin(GL_QUADS);
            glTexCoord2f( 0.0, 0.0);
            glVertex2f( x-w/2.0, y+h/2 );

            glTexCoord2f( 1.0, 0.0);
            glVertex2f(  x+w/2.0, y+h/2 );

            glTexCoord2f( 1.0, 1.0);
            glVertex2f(  x+w/2.0, y-h/2 );

            glTexCoord2f( 0.0, 1.0);
            glVertex2f( x-w/2.0, y-h/2 );
        glEnd();


        glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(PANEL_ICON_ALPHA), &(this->m_EffectState.tempo) ) );

        glColor4f( p_Fx->GetEffectState().tintr, p_Fx->GetEffectState().tintg, p_Fx->GetEffectState().tintb, p_Fx->GetEffectState().alpha*alpha );

        glBegin(GL_QUADS);
            glTexCoord2f( 0.0, 0.0);
            glVertex2f( x-w/2.0, y+h/2 );

            glTexCoord2f( 1.0, 0.0);
            glVertex2f(  x+w/2.0, y+h/2 );

            glTexCoord2f( 1.0, 1.0);
            glVertex2f(  x+w/2.0, y-h/2 );

            glTexCoord2f( 0.0, 1.0);
            glVertex2f( x-w/2.0, y-h/2 );
        glEnd();



        glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(PANEL_ICON_SELECTED), &(this->m_EffectState.tempo) ) );

        if ( p_Fx->Selected() ) {
            glColor4f( 1.0f, 1.0f, 0.0f, alpha );
        } else {
            glColor4f( 0.0f, 0.0f, 0.0f, alpha );
        }

        glBegin(GL_QUADS);
            glTexCoord2f( 0.0, 0.0);
            glVertex2f( x-w/2.0, y+h/2 );

            glTexCoord2f( 1.0, 0.0);
            glVertex2f(  x+w/2.0, y+h/2 );

            glTexCoord2f( 1.0, 1.0);
            glVertex2f(  x+w/2.0, y-h/2 );

            glTexCoord2f( 0.0, 1.0);
            glVertex2f( x-w/2.0, y-h/2 );
        glEnd();

/*
        glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(PANEL_ICON_TEMPO), &(m_EffectState.tempo) ) );

        //if ( p_Fx->Activated() ) {
            glColor4f( 1.0, 1.0, 1.0,  p_Fx->GetEffectState().tempo.ang );
        //} else {
        //    glColor4f( 0.0, 0.0, 0.0,alpha );
        //}

        glBegin(GL_QUADS);
            glTexCoord2f( 0.0, 0.0);
            glVertex2f( x-w/2.0, y+h/2 );

            glTexCoord2f( 1.0, 0.0);
            glVertex2f(  x+w/2.0, y+h/2 );

            glTexCoord2f( 1.0, 1.0);
            glVertex2f(  x+w/2.0, y-h/2 );

            glTexCoord2f( 0.0, 1.0);
            glVertex2f( x-w/2.0, y-h/2 );
        glEnd();
*/

/*
    if ( p_Fx->state.on == MO_ON ) {
    }
*/

}

void moMasterEffectPanel::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw( tempogral, parentstate);

        // Guardar y resetar la matriz de vista del modelo //
        glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
        glPushMatrix();                                     // Store The Modelview Matrix
        glLoadIdentity();									// Reset The View



        glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
        glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
        glPushMatrix();										// Store The Projection Matrix
        glLoadIdentity();									// Reset The Projection Matrix

        //gluPerspective(45.0f, 1/prop, 0.1f ,4000.0f);



		//glBindTexture(GL_TEXTURE_2D, 0);
		//glDisable(GL_TEXTURE_2D);

        /** VIEWPORT */
        float w = m_pResourceManager->GetRenderMan()->ScreenWidth();
        float h = m_pResourceManager->GetRenderMan()->ScreenHeight();
        float screen_ratio = w / h;

        int viewport_left = (int)(m_Config.Eval( moR(PANEL_VIEWPORT_LEFT) ) * w);
        int viewport_top = (int)(m_Config.Eval( moR(PANEL_VIEWPORT_TOP) ) * h);
        int viewport_width = (int)(m_Config.Eval( moR(PANEL_VIEWPORT_WIDTH) ) * w);
        int viewport_height = (int)(m_Config.Eval( moR(PANEL_VIEWPORT_HEIGHT) ) * h);

        glViewport( viewport_left, viewport_top, viewport_width, viewport_height );
        gluPerspective( 45.0f, 1/screen_ratio, 0.1f ,4000.0f);



        /** FX 3D TRANSFORM*/

        glTranslatef(   m_Config.Eval( moR(PANEL_TRANSLATEX) ), m_Config.Eval( moR(PANEL_TRANSLATEY)), m_Config.Eval( moR(PANEL_TRANSLATEZ)));

        glRotatef(  m_Config.Eval( moR(PANEL_ROTATEX) ), 1.0, 0.0, 0.0 );
        glRotatef(  m_Config.Eval( moR(PANEL_ROTATEY) ), 0.0, 1.0, 0.0 );
        glRotatef(  m_Config.Eval( moR(PANEL_ROTATEZ) ), 0.0, 0.0, 1.0 );

        glScalef(   m_Config.Eval( moR(PANEL_SCALEX)), m_Config.Eval( moR(PANEL_SCALEY)), m_Config.Eval( moR(PANEL_SCALEZ)) );


        /** background texturing */
        glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(PANEL_TEXTURE), &m_EffectState.tempo ) );
        SetBlending( (moBlendingModes) m_Config.Int( moR(PANEL_BLENDING) ) );

        //glPointSize(1);

        /** FX */
		SetColor( m_Config[moR(PANEL_COLOR)], m_Config[moR(PANEL_ALPHA)], m_EffectState );

        glBegin(GL_QUADS);
            glTexCoord2f( 0.0, 0.0);
            glVertex2f( -0.5, -0.5);

            glTexCoord2f( 1.0, 0.0);
            glVertex2f(  0.5, -0.5);

            glTexCoord2f( 1.0, 1.0);
            glVertex2f(  0.5,  0.5);

            glTexCoord2f( 0.0, 1.0);
            glVertex2f( -0.5,  0.5);
        glEnd();


        /**
        DIBUJAR:
            circulo rojo pequeño (on/off)
            circulo amarillo mediano (selected/unselected)
            circulo verde  ( (alpha) / glowing (tempo))

        **/

        float alpha_v = m_Config.Eval(moR(PANEL_ALPHA));

        float left_most = -0.5;
        float width_object = 0.1f / 4.0f;
		for( int fx=0; fx<m_pEffectManager->PreEffects().Count(); fx++, left_most+= width_object) {
            moEffect* pfx = m_pEffectManager->PreEffects().GetRef(fx);
            if (pfx) {
                DrawObject( left_most, 0.0, width_object, 1.0, pfx, 0.0, 1.0, 0.0, alpha_v );
            }
		}

        /** prendido/apagado , seleccionado/no seleccionado,  */

        float top_most = 0.0;
        float height_object = 1.0;

        int rows = m_Config.Int( moR(PANEL_ROWS) );
        int cols = m_Config.Int( moR(PANEL_COLUMNS) );
        if ((rows*cols)<40) {
            rows = 4;
            cols = 10;
            MODebug2->Push("Must be at least 40, 4x10  4 rows by 10 columns, instead of rows: " + IntToStr(rows) + " cols: "  +  IntToStr(cols) );
        }

        width_object = 0.8f / (float)cols;
        height_object = 1.0f /  (float)rows;

        /*
        for(int r=0, top_most = 0.0; r<rows; r++, top_most+= height_object ) {
            for(int c=0, left_most = -0.4; c<cols; c++, left_most+= width_object) {

                int fx = r*cols + c;
                if ( fx<m_pEffectManager->Effects().Count() ) {
                    moEffect* pfx = m_pEffectManager->Effects().GetRef(fx);
                    if (pfx) {
                        DrawObject( left_most, top_most, width_object, height_object, pfx, 0.0, 1.0, 0.0, 1.0 );
                    } else MODebug2->Push("No fx in " + IntToStr(fx) + " row:" + IntToStr(r) + " col: "  +  IntToStr(c) );
                }

            }

        }
        */

        left_most = -0.4;
        width_object = 0.8f / 40.0;
        for( int fx=0; fx<m_pEffectManager->Effects().Count(); fx++, left_most+= width_object) {
            moEffect* pfx = m_pEffectManager->Effects().GetRef(fx);
            if (pfx) {
                DrawObject( left_most, 0.0, width_object, 0.5, pfx, 0.0, 1.0, 0.0, alpha_v );
            }

		}


        left_most = 0.4;
        width_object = 0.1 / 4.0;
        for( int fx=0; fx<m_pEffectManager->PostEffects().Count(); fx++, left_most+= width_object) {
            moEffect* pfx = m_pEffectManager->PostEffects().GetRef(fx);
            if (pfx) {
                DrawObject( left_most, 0.0, width_object, 1.0, pfx, 0.0, 1.0, 0.0, alpha_v );
            }

		}



        glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glPopMatrix();

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix


	glEnable(GL_TEXTURE_2D);

}

MOboolean moMasterEffectPanel::Finish()
{
	m_pEffectManager = NULL;
	consolestate = NULL;
	return PreFinish();
}

void
moMasterEffectPanel::Interaction(moIODeviceManager *IODeviceManager) {
	//aqui se parsean los codisp
	moDeviceCode *temp;
	MOint did,cid,state;

	if(devicecode!=NULL)
	for(int i=0; i<ncodes;i++) {
		temp = devicecode[i].First;
		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			state = IODeviceManager->IODevices().GetRef(did)->GetStatus(cid);
			if(state) {
				moEffect* pEffect = NULL;
			//switch(i) {
			//	default:
					if(i<=MO_ACTION_SWITCH_EFFECT_39) {

/*
						// Sucio codigo agregado rapidamente para poder asignar los efectos a teclas arbitrarias de las 4 filas
						// del teclado
						int n = m_pEffectManager->Effects().Count();
						for(int j = 0; j < n; j++) {
							pEffect = m_pEffectManager->Effects().GetRef(j);
							if(pEffect!=NULL) {
								if(pEffect->keyidx == i) {
									pEffect->state.on*=-1;
									break;
								}
							}
						}
						// Fabri, despues implementa mejor esto... ;-)
*/


						// Codigo original:
						pEffect = m_pEffectManager->Effects().GetRef(i);
						if(pEffect)
							( pEffect->Activated() ) ? pEffect->Deactivate() : pEffect->Activate();


					}
					else
					if(i<=MO_ACTION_SWITCH_PRE_EFFECT_11) {
						pEffect =m_pEffectManager->PreEffects().GetRef(i-MO_ACTION_SWITCH_PRE_EFFECT_0);
						if(pEffect)
							( pEffect->Activated() ) ? pEffect->Deactivate() : pEffect->Activate();
					}
					else
					if(i<=MO_ACTION_SWITCH_POST_EFFECT_11) {
						pEffect = m_pEffectManager->PostEffects().GetRef(i-MO_ACTION_SWITCH_POST_EFFECT_0);
						if(pEffect)
							( pEffect->Activated() ) ? pEffect->Deactivate() : pEffect->Activate();
                    }
			//		break;
			//}
			}
		temp = temp->next;
		}
	}
}
