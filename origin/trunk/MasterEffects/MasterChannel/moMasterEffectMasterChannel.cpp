/*******************************************************************************

                      moMasterEffectMasterChannel.cpp

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

#include "moMasterEffectMasterChannel.h"

//========================
//  Factory
//========================

moMasterEffectMasterChannelFactory *m_MasterEffectMasterChannelFactory = NULL;

MO_PLG_API moMasterEffectFactory* CreateMasterEffectFactory(){
	if(m_MasterEffectMasterChannelFactory==NULL)
		m_MasterEffectMasterChannelFactory = new moMasterEffectMasterChannelFactory();
	return(moMasterEffectFactory*) m_MasterEffectMasterChannelFactory;
}

MO_PLG_API void DestroyMasterEffectFactory(){
	delete m_MasterEffectMasterChannelFactory;
	m_MasterEffectMasterChannelFactory = NULL;
}

moMasterEffect*  moMasterEffectMasterChannelFactory::Create() {
	return new moMasterEffectMasterChannel();
}

void moMasterEffectMasterChannelFactory::Destroy(moMasterEffect* fx) {
	delete fx;
}

//========================
//  EfectoMaestro
//========================
moMasterEffectMasterChannel::moMasterEffectMasterChannel() {
	SetName("masterchannel");
	m_pEffectManager = NULL;
	consolestate = NULL;
}

moMasterEffectMasterChannel::~moMasterEffectMasterChannel() {
	Finish();
}

MOboolean moMasterEffectMasterChannel::Init()
{
    if (!PreInit()) return false;

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

void moMasterEffectMasterChannel::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw( tempogral, parentstate);

    glMatrixMode( GL_MODELVIEW );
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

}


void
moMasterEffectMasterChannel::Synchronize() {
	MOuint i;
	for(i=0;i<m_pEffectManager->AllEffects().Count();i++) {

        moEffectState fxstate = m_pEffectManager->AllEffects().GetRef(i)->GetEffectState();

        fxstate.synchronized = MO_ACTIVATED;

        m_pEffectManager->AllEffects().GetRef(i)->SetEffectState(fxstate);

	}
}

MOboolean moMasterEffectMasterChannel::Finish()
{
	m_pEffectManager = NULL;
	consolestate = NULL;
    return PreFinish();
}

void
moMasterEffectMasterChannel::Interaction(moIODeviceManager *IODeviceManager) {
	//aqui se parsean los codisp
	moDeviceCode *temp;
	MOuint e;
	MOint did,cid,state,valor;

    moEffectsArray& AllFx( m_pEffectManager->AllEffects() );
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

			float wheel_valor = (float) valor /(float) 256.0;

			if(state)
			switch(i) {
				case MO_ACTION_SYNCHRONIZE_ALL:
					for(e=0;e<AllFx.Count() && (pFx=AllFx.GetRef(e)) && pFx;e++)
						pFx->Synchronize();
					break;
				case MO_ACTION_TEMPO_DELTA_SUB_MASTER:
					for(e=0;e<AllFx.Count() && (pFx=AllFx.GetRef(e)) && pFx;e++)
						pFx->TempoDelta( +0.005 );

					consolestate->tempo.delta+=0.005;
					if(consolestate->tempo.delta>2.0)
						consolestate->tempo.delta = 2.0;
					else
					if(consolestate->tempo.delta<0.005)
						consolestate->tempo.delta = 0.005;
					break;
				case MO_ACTION_TEMPO_DELTA_BMO_MASTER:
					for(e=0;e<AllFx.Count() && (pFx=AllFx.GetRef(e)) && pFx;e++)
						pFx->TempoDelta( -0.005 );

                    consolestate->tempo.delta-=0.005;
					if(consolestate->tempo.delta>2.0)
						consolestate->tempo.delta = 2.0;
					else
					if(consolestate->tempo.delta<0.005)
						consolestate->tempo.delta = 0.005;
					break;

				case MO_ACTION_TEMPO_PITCH_MASTER:

                    for(e=0;e<AllFx.Count() && (pFx=AllFx.GetRef(e)) && pFx;e++)
						pFx->TempoDelta( wheel_valor );

					consolestate->tempo.delta+=wheel_valor;
					if(consolestate->tempo.delta>2.0)
						consolestate->tempo.delta = 2.0;
					else
					if(consolestate->tempo.delta<0.005)
						consolestate->tempo.delta = 0.005;
					break;

				case MO_ACTION_TEMPO_FACTOR_SUB_MASTER:
					for(e=0;e<AllFx.Count() && (pFx=AllFx.GetRef(e)) && pFx;e++)
						pFx->TempoFactor( +1.0 );

                    consolestate->tempo.factor+=1.0;
					if(consolestate->tempo.factor>50.0)
						consolestate->tempo.factor = 50.0;
					else
					if(consolestate->tempo.factor<1.0)
						consolestate->tempo.factor = 1.0;
					break;
				case MO_ACTION_TEMPO_FACTOR_BMO_MASTER:
					for(e=0;e<AllFx.Count() && (pFx=AllFx.GetRef(e)) && pFx;e++)
						pFx->TempoFactor( -1.0 );

					consolestate->tempo.factor-=1.0;
					if(consolestate->tempo.factor>50.0)
						consolestate->tempo.factor = 50.0;
					else
					if(consolestate->tempo.factor<1.0)
						consolestate->tempo.factor = 1.0;
					break;
				case MO_ACTION_ALPHA_SUB_MASTER:
					for(e=0;e<AllFx.Count() && (pFx=AllFx.GetRef(e)) && pFx;e++)
						pFx->Alpha( 0.005 );
					break;
				case MO_ACTION_ALPHA_BMO_MASTER:
					for(e=0;e<AllFx.Count() && (pFx=AllFx.GetRef(e)) && pFx;e++)
						pFx->Alpha( -0.005 );
					break;
				case MO_ACTION_WHEEL_ALPHA_MASTER:
					for(e=0;e<AllFx.Count() && (pFx=AllFx.GetRef(e)) && pFx;e++)
						pFx->Alpha( wheel_valor );
					break;
				case MO_ACTION_AMPLITUDE_SUB_MASTER:
					//niente por ahora
					break;
				case MO_ACTION_AMPLITUDE_BMO_MASTER:
					//niente por ahora
					break;
				case MO_ACTION_WHEEL_AMPLITUDE_MASTER:
					for(e=0;e<AllFx.Count() && (pFx=AllFx.GetRef(e)) && pFx;e++)
						pFx->Amplitude( wheel_valor );
					break;
				case MO_ACTION_MAGNITUDE_SUB_MASTER:
					//niente por ahora
					break;
				case MO_ACTION_MAGNITUDE_BMO_MASTER:
					//niente por ahora
					break;
				case MO_ACTION_WHEEL_MAGNITUDE_MASTER:
					for(e=0;e<AllFx.Count() && (pFx=AllFx.GetRef(e)) && pFx;e++)
						pFx->Magnitude( wheel_valor );
					break;
				case MO_ACTION_TINT_SUB_MASTER:
					//niente por ahora
					break;
				case MO_ACTION_TINT_BMO_MASTER:
					//niente por ahora
					break;
				case MO_ACTION_WHEEL_TINT_MASTER:
					for(e=0;e<AllFx.Count() && (pFx=AllFx.GetRef(e)) && pFx;e++)
						pFx->TintCSV( 0.0, 0.0, wheel_valor );
					break;
				case MO_ACTION_WHEEL_CHROMANCY_MASTER:
					for(e=0;e<AllFx.Count() && (pFx=AllFx.GetRef(e)) && pFx;e++)
						pFx->TintCSV( wheel_valor, 0.0, 0.0  );
					break;
				case MO_ACTION_WHEEL_SATURATION_MASTER:
					for(e=0;e<AllFx.Count() && (pFx=AllFx.GetRef(e)) && pFx;e++)
						pFx->TintCSV( 0.0, wheel_valor, 0.0  );
					break;
				case MO_ACTION_BEAT_PULSE_MASTER:
					consolestate->tempo.BeatPulse(moGetTicksAbsolute());
					break;
				default:
					break;
			}
		temp = temp->next;
		}
	}
}
