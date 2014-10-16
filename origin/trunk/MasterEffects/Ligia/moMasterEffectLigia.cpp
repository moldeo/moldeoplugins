/*******************************************************************************

                            moMasterEffectLigia.cpp

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

#include "moMasterEffectLigia.h"

//========================
//  Factory
//========================

moMasterEffectLigiaFactory *m_MasterEffectLigiaFactory = NULL;

MO_PLG_API moMasterEffectFactory* CreateMasterEffectFactory(){
	if(m_MasterEffectLigiaFactory==NULL)
		m_MasterEffectLigiaFactory = new moMasterEffectLigiaFactory();
	return(moMasterEffectFactory*) m_MasterEffectLigiaFactory;
}

MO_PLG_API void DestroyMasterEffectFactory(){
	delete m_MasterEffectLigiaFactory;
	m_MasterEffectLigiaFactory = NULL;
}

moMasterEffect*  moMasterEffectLigiaFactory::Create() {
	return new moMasterEffectLigia();
}

void moMasterEffectLigiaFactory::Destroy(moMasterEffect* fx) {
	delete fx;
}

//========================
//  EfectoMaestro Ligia
//========================


moMasterEffectLigia::moMasterEffectLigia() {

	SetName("ligia");

	preview = stream = -1;

    m_capture_mode = false;

	done = false;

}

moMasterEffectLigia::~moMasterEffectLigia() {
	Finish();
}

MOboolean moMasterEffectLigia::Init()
{
    if (!PreInit()) return false;

	Images.MODebug = MODebug;
	return true;
}

void moMasterEffectLigia::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw( tempogral, parentstate);

    int ancho = 5;

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // Aca van los comandos OpenGL del efecto.
	//if(welcome==MO_FALSE) Welcome();

		glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
		glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glPushMatrix();										// Store The Projection Matrix
		glLoadIdentity();									// Reset The Projection Matrix

		float w = m_pResourceManager->GetRenderMan()->ScreenWidth();
        float h = m_pResourceManager->GetRenderMan()->ScreenHeight();
		glViewport( 0.0, 0.0, 1.0*w, 0.1*h );
		glOrtho(0,800,0,600,-1,1);							// Set Up An Ortho Screen

		glPointSize(4);
		//glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
		glColor4f(1.0,0.0,0.5,1.0);
    /**
        Dibujar el control del F11
        un punto rojo a la izquierda
    */
        if (m_capture_mode) {
					glBegin(GL_QUADS);
						glTexCoord2f(1.0,1.0);
						glVertex2i(-ancho,0-ancho);

						glTexCoord2f(0.0,1.0);
						glVertex2i(ancho,0-ancho);

						glTexCoord2f(0.0,0.0);
						glVertex2i(ancho,0+ancho);

						glTexCoord2f(1.0,0.0);
						glVertex2i(-ancho,0+ancho);
					glEnd();
        }


	/**
        Dibujar el canal 0: el efecto seleccionado: LABEL > parametros + valor seleccionado...

	*/

        glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glPopMatrix();

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix


	glEnable(GL_TEXTURE_2D);
}


MOboolean moMasterEffectLigia::Finish()
{
    return PreFinish();
}

void
moMasterEffectLigia::Interaction(moIODeviceManager *IODeviceManager) {
	//aqui se parsean los codisp
	//printf("IODeviceManager:%i",IODeviceManager);

	moDeviceCode *temp;
	MOint did,cid,state,valor;

	MOuint j,e;

    if (!m_pEffectManager) return;

    moEffectsArray& AllFx( m_pEffectManager->AllEffects());

    moEffectsArray& Fx( m_pEffectManager->Effects());
    moPreEffectsArray& PreFx( m_pEffectManager->PreEffects());
    moPostEffectsArray& PostFx( m_pEffectManager->PostEffects());
    moMasterEffectsArray& MastFx( m_pEffectManager->MasterEffects());

	moEffect* pFx = NULL;
	moEffectState fxstate;


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
				case MO_ACTION_SHUTDOWN_LIGIA_CONSOLE:
					consolestate->finish = MO_TRUE;
					break;
				case MO_ACTION_PAUSE_LIGIA_CONSOLE:
					consolestate->pause*=-1;
					break;
				case MO_ACTION_LIGIA_AUTOMATIC_MODE:
					consolestate->automatic*=-1;
					break;
				case MO_ACTION_REINIT_LIGIA:
					//consolestate->reinit = MO_TRUE;
					break;
				case MO_ACTION_SAVE_SCREEN:
					consolestate->savescreen*=-1;
					break;
				case MO_ACTION_SET_FPS:
					consolestate->setfps*=-1;
					break;
				case MO_ACTION_SHUTDOWN_EFFECTS:
                    for( e=0; e<Fx.Count() && (pFx=Fx.GetRef(e))  && pFx;e++)
                        pFx->Deactivate();
					break;
				case MO_ACTION_SHUTDOWN_PREPOST_EFFECTS:

                    for( e=0; e<PreFx.Count() && (pFx=PreFx.GetRef(e)) && pFx;e++)
                        pFx->Deactivate();

                    for( e=0; e<PostFx.Count() && (pFx=PostFx.GetRef(e)) && pFx;e++)
                        pFx->Deactivate();

                    for( e=0; e<PreFx.Count() && (pFx=PreFx.GetRef(e)) && pFx;e++)
                        if (pFx->GetName()=="erase") pFx->Activate();

					break;
				case MO_ACTION_SHUTDOWN_MASTER_EFFECTS:

                    for( e=0; e<MastFx.Count() && (pFx=MastFx.GetRef(e))  && pFx;e++) {
                        if (
                                !(pFx->GetName()=="ligia") &&
                                !(pFx->GetName()=="masterchannel") &&
                                !(pFx->GetName()=="presetpanel")
                            )
                            (pFx->Activated())? pFx->Deactivate() : pFx->Activate();
                            m_capture_mode = (!pFx->Activated());
                    }
					break;
				default:
					break;
			}
		temp = temp->next;
		}
	}

}

moConfigDefinition *
moMasterEffectLigia::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("codes"), MO_PARAM_TEXT );
	return p_configdefinition;
}
