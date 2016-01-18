/*******************************************************************************

                               moEffectSound3D.cpp

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

#include "moEffectSound3D.h"

#define DMessage(X) MODebug2->Message(X)
#define DError(X) MODebug2->Error(X)
#define DPush(X) MODebug2->Push(X)
#define DLog(X) MODebug2->Log(X)


//========================
//  Factory
//========================

moEffectSound3DFactory *m_EffectSound3DoFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if (m_EffectSound3DoFactory==NULL)
		m_EffectSound3DoFactory = new moEffectSound3DFactory();
	return (moEffectFactory*) m_EffectSound3DoFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectSound3DoFactory;
	m_EffectSound3DoFactory = NULL;
}

moEffect*  moEffectSound3DFactory::Create() {
	return new moEffectSound3D();
}

void moEffectSound3DFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================
moEffectSound3D::moEffectSound3D() {

	SetName("sound");

}

moEffectSound3D::~moEffectSound3D() {
	Finish();
}

MOboolean moEffectSound3D::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( SOUND3D_ALPHA, moText("alpha") );
	moDefineParamIndex( SOUND3D_COLOR, moText("color") );
	moDefineParamIndex( SOUND3D_SYNC, moText("syncro") );
	moDefineParamIndex( SOUND3D_PHASE, moText("phase") );
	moDefineParamIndex( SOUND3D_SOUND, moText("sound") );
	moDefineParamIndex( SOUND3D_TEXTURE, moText("texture") );
	moDefineParamIndex( SOUND3D_BLENDING, moText("blending") );
	moDefineParamIndex( SOUND3D_WIDTH, moText("width") );
	moDefineParamIndex( SOUND3D_HEIGHT, moText("height") );
	moDefineParamIndex( SOUND3D_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( SOUND3D_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( SOUND3D_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( SOUND3D_ROTATEX, moText("rotatex") );
	moDefineParamIndex( SOUND3D_ROTATEY, moText("rotatey") );
	moDefineParamIndex( SOUND3D_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( SOUND3D_SCALEX, moText("scalex") );
	moDefineParamIndex( SOUND3D_SCALEY, moText("scaley") );
	moDefineParamIndex( SOUND3D_SCALEZ, moText("scalez") );
	moDefineParamIndex( SOUND3D_INLET, moText("inlet") );
	moDefineParamIndex( SOUND3D_OUTLET, moText("outlet") );

  if (alutInit(NULL,NULL)) {
    DMessage("ALUT Initialized!");
  } else {
    DError("ALUT not initialized. Check if ALUT is installed correctly.");
    return false;
  }

  helloBuffer = alutCreateBufferHelloWorld ();
  alGenSources (1, &helloSource);
  alSourcei (helloSource, AL_BUFFER, helloBuffer);
  alSourcePlay (helloSource);
  alutSleep (1);

    m_Sound3DFilename = moText("");

    m_Audio.InitGraph();
    m_bAudioStarted = false;

    if (m_Sound3DFilename!=m_Config[moParamReference(SOUND3D_SOUND)][MO_SELECTED][0].Text() ) {

        m_Sound3DFilename = m_Config[moParamReference(SOUND3D_SOUND)][MO_SELECTED][0].Text();
        m_Sound3DFilenameFull = (moText)m_pResourceManager->GetDataMan()->GetDataPath() + (moText)moSlash + (moText)m_Sound3DFilename;

        m_Audio.BuildLiveSound( m_Sound3DFilenameFull );

    }

    last_ticks = 0;

	return true;
}

#define KLT_TRACKED           0
#define KLT_NOT_FOUND        -1
#define KLT_SMALL_DET        -2
#define KLT_MAX_ITERATIONS   -3
#define KLT_OOB              -4
#define KLT_LARGE_RESIDUE    -5

void moEffectSound3D::Draw( moTempo* tempogral, moEffectState* parentstate )
{
    MOint indeximage;
	MOdouble PosTextX0, PosTextX1, PosTextY0, PosTextY1;
    int ancho,alto;
    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    PreDraw( tempogral, parentstate);

    if (m_Sound3DFilename!=m_Config[moParamReference(SOUND3D_SOUND)][MO_SELECTED][0].Text() ) {

        m_Sound3DFilename = m_Config[moParamReference(SOUND3D_SOUND)][MO_SELECTED][0].Text();
        m_Sound3DFilenameFull = (moText)m_pResourceManager->GetDataMan()->GetDataPath() + (moText)moSlash + (moText)m_Sound3DFilename;

        m_Audio.BuildLiveSound( m_Sound3DFilenameFull );

    }



    /*
    if ( tempogral->ticks>1 && !moTimeManager::MoldeoTimer->Paused() && !m_bAudioStarted ) {
        m_Audio.Seek( tempogral->ticks );
        m_Audio.Play();
        m_bAudioStarted = true;
    } else if ( tempogral->ticks==0 && !moTimeManager::MoldeoTimer->Started() ) {
        m_Audio.Stop();
        m_bAudioStarted = false;
    }

    if (  last_ticks == tempogral->ticks ) {
        m_Audio.Pause();
        m_Audio.Seek( tempogral->ticks );
        m_bAudioStarted = false;
    }
    */
    moStreamState audiostate = m_Audio.GetState();
    moText  textstate = m_Audio.StateToText( audiostate );
    long    audioposition;

    last_ticks = moGetTicks();

    audioposition = m_Audio.GetPosition();



    if ( (long) abs(audioposition - last_ticks) > 8000 ) {
       if (audiostate!=MO_STREAMSTATE_STOPPED)
           m_Audio.Seek( last_ticks );
           audioposition = last_ticks;

    }

    moSetDuration( audioposition );

    if ( moIsTimerPaused() ) {

        if (audiostate!=MO_STREAMSTATE_PAUSED)
            m_Audio.Pause();

    } else {

        if ( moIsTimerPlaying() ) {
            if (audiostate!=MO_STREAMSTATE_PLAYING)
                m_Audio.Play();
        } else {
            if (audiostate!=MO_STREAMSTATE_STOPPED)
                m_Audio.Stop();
        }

    }

    // Guardar y resetar la matriz de vista del modelo //
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
	glLoadIdentity();									// Reset The View

    // Cambiar la proyeccion para una vista ortogonal //
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0,w,0,h,-1,1);                              // Set Up An Ortho Screen


    // Funcion de blending y de alpha channel //
    glEnable(GL_BLEND);

	glDisable(GL_ALPHA);

    // Draw //
	glTranslatef(  ( m_Config[moR(SOUND3D_TRANSLATEX)].GetData()->Fun()->Eval(m_EffectState.tempo.ang)+Tx )*w,
                   ( m_Config[moR(SOUND3D_TRANSLATEY)].GetData()->Fun()->Eval(m_EffectState.tempo.ang)+Ty )*h,
					m_Config[moR(SOUND3D_TRANSLATEZ)].GetData()->Fun()->Eval(m_EffectState.tempo.ang)+Tz);

	glRotatef(  m_Config[moR(SOUND3D_ROTATEX)].GetData()->Fun()->Eval(m_EffectState.tempo.ang), 1.0, 0.0, 0.0 );
    glRotatef(  m_Config[moR(SOUND3D_ROTATEY)].GetData()->Fun()->Eval(m_EffectState.tempo.ang), 0.0, 1.0, 0.0 );
    glRotatef(  m_Config[moR(SOUND3D_ROTATEZ)].GetData()->Fun()->Eval(m_EffectState.tempo.ang), 0.0, 0.0, 1.0 );
	glScalef(   m_Config[moR(SOUND3D_SCALEX)].GetData()->Fun()->Eval(m_EffectState.tempo.ang)*Sx,
                m_Config[moR(SOUND3D_SCALEY)].GetData()->Fun()->Eval(m_EffectState.tempo.ang)*Sy,
                m_Config[moR(SOUND3D_SCALEZ)].GetData()->Fun()->Eval(m_EffectState.tempo.ang)*Sz);

    SetColor( m_Config[moR(SOUND3D_COLOR)][MO_SELECTED], m_Config[moR(SOUND3D_ALPHA)][MO_SELECTED], m_EffectState );

    SetBlending( (moBlendingModes) m_Config[moR(SOUND3D_BLENDING)][MO_SELECTED][0].Int() );

    moTexture* pImage = (moTexture*) m_Config[moR(SOUND3D_TEXTURE)].GetData()->Pointer();

    glBindTexture( GL_TEXTURE_2D, m_Config[moR(SOUND3D_TEXTURE)].GetData()->GetGLId(&m_EffectState.tempo) );

    PosTextX0 = 0.0;
	PosTextX1 = 1.0 * ( pImage!=NULL ? pImage->GetMaxCoordS() :  1.0 );
    PosTextY0 = 0.0;
    PosTextY1 = 1.0 * ( pImage!=NULL ? pImage->GetMaxCoordT() :  1.0 );

	//ancho = (int)m_Config[ moR(SOUND3D_WIDTH) ].GetData()->Fun()->Eval(m_EffectState.tempo.ang)* (float)(w/800.0);
	//alto = (int)m_Config[ moR(SOUND3D_HEIGHT) ].GetData()->Fun()->Eval(m_EffectState.tempo.ang)* (float)(h/600.0);

	/*
	glBegin(GL_QUADS);
		glTexCoord2f( PosTextX0, PosTextY1);
		glVertex2f( -0.5*w, -0.5*h);

		glTexCoord2f( PosTextX1, PosTextY1);
		glVertex2f(  0.5*w, -0.5*h);

		glTexCoord2f( PosTextX1, PosTextY0);
		glVertex2f(  0.5*w,  0.5*h);

		glTexCoord2f( PosTextX0, PosTextY0);
		glVertex2f( -0.5*w,  0.5*h);
	glEnd();
*/
    moFont* pFont = m_pResourceManager->GetFontMan()->GetFont(0);

    textstate+= moText(" position") + IntToStr(audioposition);
    textstate+= moText(" diff:") + (moText)IntToStr( abs(audioposition - last_ticks) );

    if (pFont) {
        pFont->Draw(    0.0,
                        0.0,
                        textstate );
    }

}

MOboolean moEffectSound3D::Finish()
{
    alutExit ();
    return PreFinish();
}

void moEffectSound3D::Interaction( moIODeviceManager *IODeviceManager ) {

	moDeviceCode *temp;
	MOint did,cid,state,valor;

	moEffect::Interaction( IODeviceManager );

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
				case MO_SOUND3D_TRANSLATE_X:
					Tx = valor;
					MODebug2->Push(IntToStr(valor));
					break;
				case MO_SOUND3D_TRANSLATE_Y:
					Ty = m_pResourceManager->GetRenderMan()->RenderHeight() - valor;
					MODebug2->Push(IntToStr(valor));
					break;
				case MO_SOUND3D_SCALE_X:
					Sx+=((float) valor / (float) 256.0);
					MODebug2->Push(IntToStr(valor));
					break;
				case MO_SOUND3D_SCALE_Y:
					Sy+=((float) valor / (float) 256.0);
					MODebug2->Push(IntToStr(valor));
					break;
			}
		temp = temp->next;
		}
	}

}

moConfigDefinition *
moEffectSound3D::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("sound"), MO_PARAM_TEXT, SOUND3D_SOUND, moValue( "test.wav", "TXT") );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, SOUND3D_TEXTURE, moValue( "soundos/estrellas/star.tga", "TXT") );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, SOUND3D_BLENDING, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("width"), MO_PARAM_FUNCTION, SOUND3D_WIDTH, moValue( "256", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("height"), MO_PARAM_FUNCTION, SOUND3D_HEIGHT, moValue( "256", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, SOUND3D_TRANSLATEX, moValue( "0.5", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, SOUND3D_TRANSLATEY, moValue( "0.5", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, SOUND3D_TRANSLATEZ );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, SOUND3D_ROTATEX );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, SOUND3D_ROTATEY );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, SOUND3D_ROTATEZ );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, SOUND3D_SCALEX );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, SOUND3D_SCALEY );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, SOUND3D_SCALEZ );
	return p_configdefinition;
}



void
moEffectSound3D::Update( moEventList *Events ) {

	//get the pointer from the Moldeo Object sending it...
	moMoldeoObject::Update(Events);


}
