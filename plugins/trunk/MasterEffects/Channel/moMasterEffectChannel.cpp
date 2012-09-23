/*******************************************************************************

                            moMasterEffectChannel.cpp

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

#include "moMasterEffectChannel.h"

//========================
//  Factory
//========================

moMasterEffectChannelFactory *m_MasterEffectChannelFactory = NULL;

MO_PLG_API moMasterEffectFactory* CreateMasterEffectFactory(){
	if(m_MasterEffectChannelFactory==NULL)
		m_MasterEffectChannelFactory = new moMasterEffectChannelFactory();
	return(moMasterEffectFactory*) m_MasterEffectChannelFactory;
}

MO_PLG_API void DestroyMasterEffectFactory(){
	delete m_MasterEffectChannelFactory;
	m_MasterEffectChannelFactory = NULL;
}

moMasterEffect*  moMasterEffectChannelFactory::Create() {
	return new moMasterEffectChannel();
}

void moMasterEffectChannelFactory::Destroy(moMasterEffect* fx) {
	delete fx;
}

//========================
//  EfectoMaestro
//========================
moMasterEffectChannel::moMasterEffectChannel() {
	m_SelectedArray = NULL;
	m_pEffectManager = NULL;
	consolestate = NULL;
	SetName("channel");
}

moMasterEffectChannel::moMasterEffectChannel(char *nom) {
	m_SelectedArray = NULL;
	m_pEffectManager = NULL;
	consolestate = NULL;
	SetName(nom);
}

moMasterEffectChannel::~moMasterEffectChannel() {
	Finish();
}

MOboolean moMasterEffectChannel::Init()
{
	MOuint i;

    if (!PreInit()) return false;

	//inicializacion de los m_SelectedArray!! y puesta a -1(INDEF o DESACT)
	m_SelectedArray = new MOswitch [ m_pEffectManager->AllEffects().Count() ];
	for(i=0; i < m_pEffectManager->AllEffects().Count() ; i++) {
	    m_SelectedArray[i] = MO_DEACTIVATED;
	}

	m_bFirstSelected = m_Config.GetParam( m_Config.GetParamIndex( moText("firstselected") )).GetValue().GetSubValue(0).Int();

	if ( m_bFirstSelected ) {
		//m_Selected = ;
		//m_SelectedArray[12] = MO_ACTIVATED;//?QUIEN ES, el Effect 1???
		m_Selected = -1;

	} else {

		m_Selected = -1;

	}

    moDefineParamIndex( CHANNEL_ALPHA, moText("alpha") );
	moDefineParamIndex( CHANNEL_COLOR, moText("color") );
	moDefineParamIndex( CHANNEL_SYNC, moText("syncro") );
	moDefineParamIndex( CHANNEL_PHASE, moText("phase") );


    /*
    color = m_Config.GetParamIndex("color");
    transx = m_Config.GetParamIndex("TranslateX");
    transy = m_Config.GetParamIndex("TranslateY");
    transz = m_Config.GetParamIndex("TranslateZ");
    rotatx = m_Config.GetParamIndex("RotateX");
    rotaty = m_Config.GetParamIndex("RotateY");
    rotatz = m_Config.GetParamIndex("RotateZ");
    scalex = m_Config.GetParamIndex("ScaleX");
    scaley = m_Config.GetParamIndex("ScaleY");
    scalez = m_Config.GetParamIndex("ScaleZ");
    */
    // Hacer la inicializacion de este efecto en particular.
    // Seteos de OpenGL.
    // Seteos de Texturas.
	return true;
}

void moMasterEffectChannel::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw( tempogral, parentstate);

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();                                     // Store The Modelview Matrix
	glLoadIdentity();
/*
    glTranslatef(   movx(m_Config.GetParam(transx), tempo),
                    movy(m_Config.GetParam(transy), tempo),
                    movz(m_Config.GetParam(transz), tempo));
    glRotatef(  movx(m_Config.GetParam(rotatx), tempo), 1.0, 0.0, 0.0 );
    glRotatef(  movy(m_Config.GetParam(rotaty), tempo), 0.0, 1.0, 0.0 );
    glRotatef(  movz(m_Config.GetParam(rotatz), tempo), 0.0, 0.0, 1.0 );
    glScale(    movx(m_Config.GetParam(scalex), tempo),
                movy(m_Config.GetParam(scaley), tempo),
                movz(m_Config.GetParam(scalez), tempo));
*/
	/*
    glColor4f(  m_Config.GetParam(color).GetValueFlt(MO_RED)*state.tint,
                m_Config.GetParam(color).GetValueFlt(MO_GREEN*state.tint),
                m_Config.GetParam(color).GetValueFlt(MO_BLUE)*state.tint,
                m_Config.GetParam(color).GetValueFlt(MO_ALPHA)*state.alpha);
    */
    // Aca van los comandos OpenGL del efecto.
/*
	MOint m_Selected,ne,i;
	m_Selected = -1;
	ne = 0;

	for(i=0;i<consolestate->m_nAllEffects;i++) {
		if( m_SelectedArray[i] == MO_ACTIVATED ) {
			ne++;
			m_Selected = i;
		}
	}
 */

	MOint cantparam,paramselected,j,ancho;
	ancho = 1;

	if ( m_Selected > -1 ) {
		//Como solo hay un elegido, dibujo sus debug

		// Cambiar la proyeccion para una vista ortogonal //

		glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
		glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glPushMatrix();										// Store The Projection Matrix
		glLoadIdentity();									// Reset The Projection Matrix
		glOrtho(0,800,0,600,-1,1);							// Set Up An Ortho Screen

		glPointSize(1);
		//glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);

		SetColor( m_Config[moR(CHANNEL_COLOR)], m_Config[moR(CHANNEL_ALPHA)], m_EffectState );

		moEffect*	peffect = NULL;

		if ( m_pEffectManager != NULL ) {

            peffect = m_pEffectManager->AllEffects().GetRef( m_Selected );

			if (peffect!=NULL) {

				cantparam = peffect->GetConfig()->GetParams().Count();
				paramselected = peffect->GetConfig()->GetCurrentParamIndex();

				//aquí inmprimir el parametro seleccionado...
				moTextArray Txts(1);
				moText ParamTxt;
				ParamTxt = peffect->GetConfig()->GetParam(paramselected).GetParamDefinition().GetName()+moText(":");
				moParamType PType = peffect->GetConfig()->GetParam(paramselected).GetParamDefinition().GetType();
				ParamTxt+= peffect->GetConfig()->GetParam(paramselected).GetValue().GetSubValue().Text();
				//Txts.Add( ParamTxt );
				//m_pResourceManager->GetGuiMan()->DisplayInfoWindow(600,30, 200,40, Txts);
				//Txts.Finish();

				for(j=0; j<=paramselected; j++) {
			/*
					glBegin(GL_POINTS);
						glColor4f(1.0,1.0,1.0,1.0);
						glVertex2i(400-j*2,300);
						glColor4f(0.0,0.0,0.0,1.0);
						glVertex2i(400-j*2-1,300);
					glEnd();
			*/

					glBegin(GL_QUADS);
						glTexCoord2f(1.0,1.0);
						glVertex2i(-ancho+800-j*5,0-ancho);

						glTexCoord2f(0.0,1.0);
						glVertex2i(ancho+800-j*5,0-ancho);

						glTexCoord2f(0.0,0.0);
						glVertex2i(ancho+800-j*5,0+ancho);

						glTexCoord2f(1.0,0.0);
						glVertex2i(-ancho+800-j*5,0+ancho);
					glEnd();

				}
			}//fin peffect
		}//end m_Selected
		// Dejamos todo como lo encontramos //
		glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
		glPopMatrix();										// Restore The Old Projection Matrix
	}
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix


	glEnable(GL_TEXTURE_2D);
}

MOboolean moMasterEffectChannel::Finish()
{
	if(m_SelectedArray!=NULL)
	 delete[] m_SelectedArray;
	m_SelectedArray = NULL;
	m_pEffectManager = NULL;
	consolestate = NULL;
    return PreFinish();
}
/*
void moMasterEffectChannel::Update( moEventList *p_eventlist ) {

    moMoldeoObject::Update( p_eventlist );

    ///MIDI CONTROLS !!!!!
    moEvent* actual = p_eventlist->First;
	//recorremos todos los events y parseamos el resultado
	//borrando aquellos que ya usamos
	MOint tempval;
	while(actual!=NULL) {
		//solo nos interesan los del midi q nosotros mismos generamos, para destruirlos
		if(actual->deviceid == MO_IODEVICE_MIDI) {
		    ///actual->reservedvalue1 corresponds to CC midi code : it works as a n index in m_Codes (has to be defined in param "code" in the config file...
		    ///actual->reservedvalue2 corresponds to VAL

			int CC = actual->reservedvalue1;
			int VAL = actual->reservedvalue2;

            switch (CC) {
                case 45:
                    MODebug2->Push(moText("## Midi Button ##"));
                    break;
                default:
                    MODebug2->Push("device in channel!!!");
                    MODebug2->Push(IntToStr(VAL));
                    break;
            }
		}
	}
}
*/


void moMasterEffectChannel::UnselectAll() {

        moEffectsArray& AllFx( m_pEffectManager->AllEffects() );
        moEffect* pFx = NULL;

        for( MOuint e=0; e < AllFx.Count() && ( pFx = AllFx.GetRef(e) ) && pFx; e++) {
                m_SelectedArray[e] = MO_OFF;
                m_Selected = -1;
                pFx->Unselect();
        }

}


void moMasterEffectChannel::UpdateMultipleSelection() {

    moEffectsArray& AllFx( m_pEffectManager->AllEffects() );
	moEffect* pFx = NULL;

    for(MOuint e=0; e < AllFx.Count() && (pFx = AllFx.GetRef(e) ); e++ ) {
        if( pFx )
            if ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e )
                pFx->Select();
            else
                pFx->Unselect();

    }


}




void
moMasterEffectChannel::Interaction( moIODeviceManager *IODeviceManager ) {
	//aqui se parsean los codisp
	moDeviceCode *temp,*aux,*aux2;
	MOuint e,sel;
	MOint did,cid,status,valor,estaux,estaux2,elaux;

    if (!m_pEffectManager)
        return;

    moEffectsArray& AllFx( m_pEffectManager->AllEffects() );
	moEffect* pFx = NULL;
	moEffectState fxstate;


	if( !Activated()  && (m_Selected>=0) ) {
		//BYPASSING INTERACTIONS TO SELECTED EFFECT

		/// TODO: la seleccion de efectos basado en el array general ya no es conveniente!!!!
		/// TODO: esto ya se volvio obsoleto, hay que pasar a eventos por outlets-inlets....
        pFx = (moEffect*)AllFx.GetRef(m_Selected);

		if (pFx && pFx!=this) {
            pFx->Interaction(IODeviceManager);
		}
		return;
	}



	if(devicecode!=NULL)
	for(int i=0; i<ncodes;i++) {
		temp = devicecode[i].First;
		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			status = IODeviceManager->IODevices().GetRef(did)->GetStatus(cid);
			valor = IODeviceManager->IODevices().GetRef(did)->GetValue(cid);

            float wheel_valor = (float) valor /(float) 256.0;

			if(status)
			switch(i) {
				case MO_ACTION_SYNCHRO_EFFECT:
					for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            pFx->Synchronize();
					break;

/**BEAT PULSE*/
				case MO_ACTION_BEAT_PULSE_EFFECT:
					for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            pFx->BeatPulse();
                    break;

/**TEMPO DELTA*/
				case MO_ACTION_TEMPO_DELTA_SUB_EFFECT:
					for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            pFx->TempoDelta(+0.005);
					break;
				case MO_ACTION_TEMPO_DELTA_BMO_EFFECT:
					for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            pFx->TempoDelta(-0.005);
					break;
				case MO_ACTION_TEMPO_PITCH_EFFECT:
					for( e=0; e<AllFx.Count() && (pFx = AllFx.GetRef(e)); e++ ) {
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) ) {
						    pFx->TempoDelta( wheel_valor );
						    /*
                            MODebug2->Push( "MO_ACTION_TEMPO_PITCH_EFFECT  " + pFx->GetLabelName()
                                           + "valor anterior:" + FloatToStr(pFx->GetEffectState().tempo.delta )
                                           + " valor agregado:" + FloatToStr(wheel_valor)
                                           + " valor actual:" + FloatToStr(pFx->TempoDelta( wheel_valor )) );
                            */
						}
					}
					break;

/**TEMPO FACTOR*/
				case MO_ACTION_TEMPO_FACTOR_SUB_EFFECT:
					for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            pFx->TempoFactor(1.0);
					break;
				case MO_ACTION_TEMPO_FACTOR_BMO_EFFECT:
					for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            pFx->TempoFactor(-1.0);
					break;

/**ALPHA*/
				case MO_ACTION_ALPHA_SUB_EFFECT:
                    for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            pFx->Alpha(0.05);
					break;
				case MO_ACTION_ALPHA_BMO_EFFECT:
                    for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            pFx->Alpha(-0.05);
					break;
				case MO_ACTION_WHEEL_ALPHA:
                    for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) ) {
						    pFx->Alpha( wheel_valor );
						    /*
                            MODebug2->Push( "MO_ACTION_WHEEL_ALPHA  " + pFx->GetLabelName()
                                           + "valor anterior:" + FloatToStr(pFx->GetEffectState().alpha)
                                           + " valor agregado:" + FloatToStr(wheel_valor)
                                           + " valor actual:" + FloatToStr(pFx->Alpha( wheel_valor )) );
                                           */
                        }
					break;

/**AMPLITUDE*/
				case MO_ACTION_AMPLITUDE_SUB_EFFECT:
                    for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            pFx->Amplitude(0.05);
				case MO_ACTION_AMPLITUDE_BMO_EFFECT:
                    for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            pFx->Amplitude(-0.05);
					break;
				case MO_ACTION_WHEEL_AMPLITUDE:
                    for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            pFx->Amplitude( wheel_valor );
					break;

/**MAGNITUDE*/
				case MO_ACTION_MAGNITUDE_SUB_EFFECT:
                    for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            pFx->Magnitude(0.05);
					break;
				case MO_ACTION_MAGNITUDE_BMO_EFFECT:
                    for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            pFx->Magnitude(-0.05);
					break;
				case MO_ACTION_WHEEL_MAGNITUDE:
                    for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            pFx->Magnitude( wheel_valor );
					break;

/**TINT*/
				case MO_ACTION_TINT_SUB_EFFECT:
                    for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            pFx->TintCSV( 0.0, 0.0, 0.05 );
					break;
				case MO_ACTION_TINT_BMO_EFFECT:
                    for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            pFx->TintCSV( 0.0, 0.0, -0.05 );
					break;
				case MO_ACTION_WHEEL_TINT:
                    for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) ) {
                            pFx->TintCSV( 0.0, 0.0, -wheel_valor );
                            //MODebug2->Push( "MO_ACTION_WHEEL_TINT  " + pFx->GetLabelName() + " valor:" + FloatToStr(-wheel_valor) );
						}
					break;

/**TINT CHROMINANCE*/
				case MO_ACTION_WHEEL_CHROMANCY:
                    for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) ) {
                            pFx->TintCSV( wheel_valor*0.25, 0.0, 0.0 );
                            MODebug2->Push( "MO_ACTION_WHEEL_CHROMANCY  " + pFx->GetLabelName() + " valor:" + FloatToStr(wheel_valor) );
                        }
					break;


/**TINT SATURATION*/
				case MO_ACTION_WHEEL_SATURATION:
                    for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) ) {
                            pFx->TintCSV( 0.0, -wheel_valor*0.25, 0.0 );
                            //MODebug2->Push( "MO_ACTION_WHEEL_CHROMANCY  " + pFx->GetLabelName() + " valor:" + FloatToStr(wheel_valor) );
                        }
                    break;


/** NEXT EFFECT*/
				case MO_ACTION_PARAM_NEXT_EFFECT:
					for(e=0; e<AllFx.Count(); e++) {
						//lo hace con el primer elegido
						if(m_SelectedArray[e]==MO_ON && m_Selected!=(MOint)e) {
							if(AllFx.GetRef(e)!=NULL) {
								AllFx.GetRef(e)->GetConfig()->NextParam();
								//OJO
								//aca deberiamos actualizar E/S(para que se vea el parametro elegido)
							}
							break;
						}
					}
					if(m_Selected>-1) {
						if(AllFx.GetRef(m_Selected)!=NULL) {
                            AllFx.GetRef(m_Selected)->GetConfig()->NextParam();
						}
					}
					break;
				case MO_ACTION_PARAM_PREV_EFFECT:
					for(e=0; e<AllFx.Count(); e++) {
						//lo hace con el primer elegido
						if(m_SelectedArray[e]==MO_ON && m_Selected!=(MOint)e) {
							if(AllFx.GetRef(e)!=NULL) {
								AllFx.GetRef(e)->GetConfig()->PrevParam();
								//OJO
								//aca deberiamos actualizar E/S(para que se vea el parametro elegido)
							}
							break;
						}
					}
					if(m_Selected>-1) {
						if(AllFx.GetRef(m_Selected)!=NULL) {
						AllFx.GetRef(m_Selected)->GetConfig()->PrevParam();
						}
					}
					break;
				case MO_ACTION_VALOR_NEXT_EFFECT:
					for(e=0; e<AllFx.Count(); e++) {
						//lo hace con el primer elegido
						if(m_SelectedArray[e]==MO_ON && m_Selected!=(MOint)e) {
							if(AllFx.GetRef(e)!=NULL) {
								AllFx.GetRef(e)->GetConfig()->NextValue();
								//OJO
								//aca deberiamos actualizar E/S(para que se vea el parametro elegido)
							}
							break;
						}
					}
					if(m_Selected>-1) {
						if(AllFx.GetRef(m_Selected)!=NULL) {
						AllFx.GetRef(m_Selected)->GetConfig()->NextValue();
						}
					}
					break;
				case MO_ACTION_VALOR_PREV_EFFECT:
					for(e=0; e<AllFx.Count(); e++) {
						//lo hace con el primer elegido
						if(m_SelectedArray[e]==MO_ON && m_Selected!=(MOint)e) {
							if(AllFx.GetRef(e)!=NULL) {
								AllFx.GetRef(e)->GetConfig()->PreviousValue();
								//OJO
								//aca deberiamos actualizar E/S(para que se vea el parametro elegido)
							}
							break;
						}
					}
					if(m_Selected>-1) {
						if(AllFx.GetRef(m_Selected)!=NULL) {
						AllFx.GetRef(m_Selected)->GetConfig()->PreviousValue();
						}
					}
					break;
				case MO_ACTION_PARAMSET_NEXT_EFFECT:
					for(e=0; e<AllFx.Count(); e++) {
						//lo hace con el primer elegido
						if(m_SelectedArray[e]==MO_ON && m_Selected!=(MOint)e) {
							if(AllFx.GetRef(e)!=NULL) {
								AllFx.GetRef(e)->GetConfig()->PreConfNext();
								//OJO
								//aca deberiamos actualizar E/S(para que se vea el parametro elegido)
							}
							break;
						}
					}
					if(m_Selected>-1) {
						if(AllFx.GetRef(m_Selected)!=NULL) {
						AllFx.GetRef(m_Selected)->GetConfig()->PreConfNext();
						}
					}
					break;
				case MO_ACTION_PARAMSET_PREV_EFFECT:
					for(e=0; e<AllFx.Count(); e++) {
						//lo hace con el primer elegido
						if(m_SelectedArray[e]==MO_ON && m_Selected!=(MOint)e) {
							if(AllFx.GetRef(e)!=NULL) {
								AllFx.GetRef(e)->GetConfig()->PreConfPrev();
								//OJO
								//aca deberiamos actualizar E/S(para que se vea el parametro elegido)
							}
							break;
						}
					}
					if(m_Selected>-1) {
						if(AllFx.GetRef(m_Selected)!=NULL) {
						AllFx.GetRef(m_Selected)->GetConfig()->PreConfPrev();
						}
					}
					break;
				case MO_ACTION_UNSELECT_EFFECTS:
					UnselectAll();
					break;

                case MO_ACTION_PLAY:
                    MODebug2->Push("MO_ACTION_PLAY");
                    for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            { pFx->Unsynchronize();pFx->Play();}
                    break;

                case MO_ACTION_PAUSE:
                    MODebug2->Push("MO_ACTION_PAUSE");
                    for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            { pFx->Unsynchronize();pFx->Pause();}
                    break;

                case MO_ACTION_STOP:
                    MODebug2->Push("MO_ACTION_STOP");
                    for(e=0;e<AllFx.Count() && (pFx = AllFx.GetRef(e));e++)
						if( pFx && ( m_SelectedArray[e]==MO_ON || m_Selected==(MOint)e ) )
                            { pFx->Unsynchronize();pFx->Stop();}
                    break;


				default:
					aux = devicecode[MO_ACTION_SEL_UNIQUE].First;
					aux2 = devicecode[MO_ACTION_SEL_MULTIPLE].First;
					estaux = IODeviceManager->IODevices().GetRef(aux->device)->GetStatus(aux->devicecode);
					estaux2 = IODeviceManager->IODevices().GetRef(aux2->device)->GetStatus(aux->devicecode);

					if(MO_ACTION_SEL_EFFECT_0<=i && i<=MO_ACTION_SEL_EFFECT_39) {

					    MODebug2->Push("sel fx:" + IntToStr(i)+ " estaux:"+IntToStr(estaux) + " estaux2:" + IntToStr(estaux2) ) ;

						// Sucio codigo agregado rapidamente para poder asignar los efectos a teclas arbitrarias de las 4 filas
						// del teclado:
						// Basicamente, remapeando el indice i al i0 que corresponde a la posicion del efecto seleccionado
						// en el array.
						/*
						moEffect* pEffect = NULL;
						int i0 = -1;
						int n = m_pEffectManager->Effects().Count();
						for(int j = 0; j < n; j++) {
							pEffect = m_pEffectManager->Effects().GetRef(j);
							if(pEffect!=NULL) {
								if(pEffect->keyidx == i) {
									i0=j;
									break;
								}
							}
						}
						if (i0 == -1) return;
						*/


					    sel = m_pEffectManager->PreEffects().Count() + m_pEffectManager->PostEffects().Count() +(i-MO_ACTION_SEL_EFFECT_0);
                        pFx = AllFx.GetRef(sel);
						if (estaux) { UnselectAll(); m_Selected = sel; } else { m_SelectedArray[sel]*=-1;}
                        if (!pFx) m_Selected = -1;
                        UpdateMultipleSelection();
					}
					else
					if(MO_ACTION_SEL_PRE_EFFECT_0<=i && i<=MO_ACTION_SEL_PRE_EFFECT_3) {

					    sel = (i-MO_ACTION_SEL_PRE_EFFECT_0);
                        pFx = AllFx.GetRef(sel);
						if (estaux) { UnselectAll(); m_Selected = sel; } else { m_SelectedArray[sel]*=-1;}
                        if (!pFx) m_Selected = -1;
                         UpdateMultipleSelection();
					}
					else
					if(MO_ACTION_SEL_POST_EFFECT_0<=i && i<=MO_ACTION_SEL_POST_EFFECT_3) {

					    sel = m_pEffectManager->PreEffects().Count()+(i-MO_ACTION_SEL_POST_EFFECT_0);
                        pFx = AllFx.GetRef(sel);
						if (estaux) { UnselectAll(); m_Selected = sel; } else { m_SelectedArray[sel]*=-1;}
                        if (!pFx) m_Selected = -1;
                        UpdateMultipleSelection();
					}
					break;
			}
		temp = temp->next;
		}
	}
}
