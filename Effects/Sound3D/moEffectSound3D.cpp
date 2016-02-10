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

#define DMessage(X) MODebug2->Message( moText("moEffectSound3D::") + X )
#define DError(X) MODebug2->Error(moText("moEffectSound3D::") + X )
#define DPush(X) MODebug2->Push(moText("moEffectSound3D::") + X )
#define DLog(X) MODebug2->Log(moText("moEffectSound3D::") + X )


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
// SOUND 3D
//========================

moSound3DAL::moSound3DAL() {
	m_pData = NULL;
}

moSound3DAL::~moSound3DAL() {
	Finish();
}

MOboolean moSound3DAL::Finish() {
	// Clean up by deleting Source(s) and Buffer(s)

	alSourceStop(m_SourceId);
	alSourcei(m_SourceId, AL_BUFFER, 0);
    alDeleteSources(1, &m_SourceId);
	alDeleteBuffers(1, &m_BufferId);

	return true;
}


MOboolean moSound3DAL::Init() {
	m_BufferSize = 0;
	m_ActualSample = 0;
	m_OldSample = 0;
	return true;
}


MOboolean
moSound3DAL::BuildEmpty( MOuint p_size ) {

	// Generate an AL Buffer

	alGenBuffers( 1, &m_BufferId );

	// Generate a Source to playback the Buffer
	alGenSources( 1, &m_SourceId );

  p_size = 0;
	return true;
}

MOboolean
moSound3DAL::BuildFromBuffer( MOuint p_size, GLvoid* p_buffer ) {
	BuildEmpty(p_size);
	p_buffer = NULL;
	return true;
}

MOboolean
moSound3DAL::BuildFromFile( const moText& p_filename ) {

	BuildEmpty(0);

  moFile Sfile( p_filename );
/*
	switch(m_SoundType) {
		case MO_SOUNDTYPE_WAV:

			break;
		case MO_SOUNDTYPE_MP3:
			break;
		case MO_SOUNDTYPE_M4A:
			break;
        case MO_SOUNDTYPE_OGG:
			break;
		case MO_SOUNDTYPE_UNDEFINED:
			break;
	}
*/

  ALenum error;
  m_BufferId = alutCreateBufferFromFile( p_filename  );
  if ((error = alutGetError()) != ALUT_ERROR_NO_ERROR)
  {
    MODebug2->Error( "moSound3DAL::BuildFromFile > alutCreateBufferFromFile failed: " + IntToStr( error ) );
    // Delete Buffers
    //alDeleteBuffers(NUM_BUFFERS, buffers);
    return false;
  }
  MODebug2->Message("moSound3DAL::BuildFromFile > Loaded Buffer from file: " + p_filename );

  alGetBufferi( m_BufferId, AL_SIZE, &m_BufferSize );
	// Attach Source to Buffer
	alSourcei( m_SourceId, AL_BUFFER, m_BufferId );

  MODebug2->Message("moSound3DAL::BuildFromFile > buffersize: " + IntToStr(m_BufferSize) + " bufferid:" + IntToStr(m_BufferId) );

  ALint bcount,channels, frequency;
  alGetBufferi(m_BufferId, AL_SIZE, &m_ulBufferSize);
  m_AudioFormat.m_BufferSize = m_ulBufferSize;
  alGetBufferi(m_BufferId, AL_BITS, &bcount);
  m_AudioFormat.m_BitCount = bcount;
  alGetBufferi(m_BufferId, AL_CHANNELS, &channels);
  m_AudioFormat.m_Channels = channels;
  alGetBufferi(m_BufferId, AL_FREQUENCY, &frequency);
  m_AudioFormat.m_SampleRate = frequency;

  MODebug2->Message("moSound3DAL::BuildFromFile > Audioformat.m_BufferSize: " + IntToStr(m_BufferSize)
                    + " Audioformat.m_BitCount:" + IntToStr(m_AudioFormat.m_BitCount)
                    + " Audioformat.m_Channels:" + IntToStr(m_AudioFormat.m_Channels)
                    + " Audioformat.m_SampleRate:" + IntToStr(m_AudioFormat.m_SampleRate) );

	return true;

}

void moSound3DAL::Play() {
	// Start playing source

	alSourcePlay(m_SourceId);

}


void moSound3DAL::PlaySample( MOint sampleid ) {

	moStreamState state = this->State();
  //MODebug2->Message( "PlaySample: actualsample:"+IntToStr(m_ActualSample) + " sampleid:" + IntToStr(sampleid) + " state: "+IntToStr(state) );

	m_OldSample = m_ActualSample;

	//alGetSourcei( m_SourceId , AL_BUFFER , &m_ActualSample);
	alGetSourcei( m_SourceId , AL_BYTE_OFFSET , &m_ActualSample);


	//alSourceStop(m_SourceId);

	if (m_ActualSample!=sampleid && (state != MO_STREAMSTATE_PLAYING)  ) {
		//alSourcei( m_SourceId, AL_BUFFER , sampleid );
		//alSourcei( m_SourceId, AL_BUFFER , m_BufferId );
		alSourcei(m_SourceId, AL_BYTE_OFFSET, sampleid);
		alSourcePlay(m_SourceId);
		m_ActualSample = sampleid;
	}
/*
// check for errors
while (source_state == AL_PLAYING) {
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
        // check for errors
}
*/
	ALenum error = alGetError();
	if (error!=AL_NO_ERROR) {
    MODebug2->Error("moSound3DAL::PlaySample > playing sample id: " + IntToStr(sampleid) + " error:" + IntToStr(error) );
	}


  sampleid = 0;
}

void moSound3DAL::Pause() {

	alSourcePause(m_SourceId);

}

void moSound3DAL::Stop() {
	// Stop the Source and clear the Queue

	alSourceStop(m_SourceId);


}

void moSound3DAL::Rewind() {

	alSourceRewind(m_SourceId);

}


void moSound3DAL::Final() {

	Update();
}

void moSound3DAL::Frame(int frame) {
	Update();
	frame = 0;
}

void moSound3DAL::Repeat(int repeat) {
  repeat = 0;
}

moStreamState moSound3DAL::State() {
	// Get Source State
	alGetSourcei( m_SourceId, AL_SOURCE_STATE, &m_iAlState);
	switch(m_iAlState) {
      case AL_INITIAL:
        return MO_STREAMSTATE_READY;
        break;

      case AL_PLAYING:
        return MO_STREAMSTATE_PLAYING;
        break;

      case AL_PAUSED:
        return MO_STREAMSTATE_PAUSED;
        break;

      case AL_STOPPED:
        return MO_STREAMSTATE_STOPPED;
        break;
	}

  return MO_STREAMSTATE_UNKNOWN;
}

void moSound3DAL::Update() {

	alGetSourcei( m_SourceId, AL_SOURCE_STATE, &m_iAlState );
	alGetSourcei( m_SourceId, AL_BUFFER , &m_ActualSample );

}

void moSound3DAL::SetPosition( float x, float y, float z ) {

	alSource3f( m_SourceId, AL_POSITION, x, y, z );

  x = 0;
  y = 0;
  z = 0;
}

void moSound3DAL::SetVelocity( float x, float y, float z ) {

	alSource3f( m_SourceId, AL_VELOCITY, x, y, z );

  x = 0;
  y = 0;
  z = 0;
}

void moSound3DAL::SetDirection( float x, float y, float z ) {

	alSource3f( m_SourceId, AL_DIRECTION, x, y, z );

  x = 0;
  y = 0;
  z = 0;
}

void moSound3DAL::SetVolume( float volume ) {

	alSourcef( m_SourceId, AL_GAIN, volume );

  volume = 0;
}

float moSound3DAL::GetVolume() {

	alGetSourcef( m_SourceId, AL_GAIN, &m_Volume );

	return m_Volume;

}

void moSound3DAL::SetPitch( float pitch )  {
    m_Pitch = pitch;

    alSourcef(m_SourceId, AL_PITCH, pitch);

    pitch = 0;
}


float moSound3DAL::GetPitch()  {

  alGetSourcef( m_SourceId, AL_PITCH, &m_Pitch );

  return m_Pitch;
}


//========================
//  Efecto
//========================
moEffectSound3D::moEffectSound3D() {

	SetName("sound3d");
	m_pSM = NULL;
  m_pAudio = NULL;

}

moEffectSound3D::~moEffectSound3D() {
	Finish();
}

MOboolean moEffectSound3D::Init() {


  if ( alutInit( NULL, NULL )) {
    DMessage("Init > ALUT Initialized!");
    //generate al id
    alGenSources (1, &helloSource);
    //generate al buffer
    helloBuffer = alutCreateBufferHelloWorld ();
    //associate buffer to al id source
    alSourcei (helloSource, AL_BUFFER, helloBuffer);
    //play source
    alSourcePlay (helloSource);
    //wait
    alutSleep (1);

  } else {
    DError("Init > ALUT not initialized. Check if ALUT is installed correctly.");
    return false;
  }

  m_Sound3DFilename = moText("");

  m_pSM = m_pResourceManager->GetSoundMan();
  if (!m_pSM) {
    return false;
  }

  if (!PreInit()) return false;

  moDefineParamIndex( SOUND3D_ALPHA, moText("alpha") );
  moDefineParamIndex( SOUND3D_COLOR, moText("color") );
  moDefineParamIndex( SOUND3D_SYNC, moText("syncro") );
  moDefineParamIndex( SOUND3D_PHASE, moText("phase") );
  moDefineParamIndex( SOUND3D_SOUND, moText("sound") );

  moDefineParamIndex( SOUND3D_SEEKPOSITION, moText("seekposition") );

  moDefineParamIndex( SOUND3D_MODE, moText("mode") );
  moDefineParamIndex( SOUND3D_LOOP, moText("loop") );
  moDefineParamIndex( SOUND3D_LAUNCH, moText("launch") );
  moDefineParamIndex( SOUND3D_SPEEDOFSOUND, moText("speedofsound") );
  moDefineParamIndex( SOUND3D_PITCH, moText("pitch") );
  moDefineParamIndex( SOUND3D_VOLUME, moText("volume") );
  moDefineParamIndex( SOUND3D_BALANCE, moText("balance") );

  moDefineParamIndex( SOUND3D_TEXTURE, moText("texture") );
  moDefineParamIndex( SOUND3D_BLENDING, moText("blending") );

  moDefineParamIndex( SOUND3D_TRANSLATEX, moText("translatex") );
  moDefineParamIndex( SOUND3D_TRANSLATEY, moText("translatey") );
  moDefineParamIndex( SOUND3D_TRANSLATEZ, moText("translatez") );

  moDefineParamIndex( SOUND3D_TRANSLATEX, moText("translatex") );
  moDefineParamIndex( SOUND3D_TRANSLATEY, moText("translatey") );
  moDefineParamIndex( SOUND3D_TRANSLATEZ, moText("translatez") );

  moDefineParamIndex( SOUND3D_TRANSLATEX, moText("translatex") );
  moDefineParamIndex( SOUND3D_TRANSLATEY, moText("translatey") );
  moDefineParamIndex( SOUND3D_TRANSLATEZ, moText("translatez") );

  moDefineParamIndex( SOUND3D_INLET, moText("inlet") );
  moDefineParamIndex( SOUND3D_OUTLET, moText("outlet") );

  UpdateParameters();
  /*m_Audio.InitGraph();

  m_bAudioStarted = false;

  if (m_Sound3DFilename!=m_Config[moParamReference(SOUND3D_SOUND)][MO_SELECTED][0].Text() ) {

      m_Sound3DFilename = m_Config[moParamReference(SOUND3D_SOUND)][MO_SELECTED][0].Text();
      m_Sound3DFilenameFull = (moText)m_pResourceManager->GetDataMan()->GetDataPath() + (moText)moSlash + (moText)m_Sound3DFilename;

      m_Audio.BuildLiveSound( m_Sound3DFilenameFull );

  }
  */
  last_ticks = 0;

  return true;
}

#define KLT_TRACKED           0
#define KLT_NOT_FOUND        -1
#define KLT_SMALL_DET        -2
#define KLT_MAX_ITERATIONS   -3
#define KLT_OOB              -4
#define KLT_LARGE_RESIDUE    -5

MOboolean
moEffectSound3D::ResolveValue( moParam& param, int value_index, bool p_refresh ) {

  int idx = -1;
  moValue& value( param.GetValue(value_index) );
  moParamType param_type = param.GetParamDefinition().GetType();
  //MODebug2->Message( moText("+Init value #") + IntToStr(v) );

  if (value.GetSubValueCount()<=0) {
      return false;
  }

  moValueBase& valuebase0(value.GetSubValue(0));

  switch( param_type ) {

      case MO_PARAM_SOUND:
          if (value.GetSubValueCount()>0) {
            if (valuebase0.Text()!="") {
              UpdateSound( valuebase0.Text() );
              if (m_pAudio) {
                  valuebase0.SetSound( m_pAudio );
                  return true;
              }
              return false;
            }
          }
          break;
      default:
        break;

  }//fin siwtch


  return moMoldeoObject::ResolveValue( param, value_index, p_refresh );
}

void
moEffectSound3D::UpdateParameters() {
  UpdateSound( m_Config.Text( moR(SOUND3D_SOUND)) );
}

void
moEffectSound3D::UpdateSound( const moText& p_newfilename ) {

    moDataManager* pDataMan = m_pResourceManager->GetDataMan();
    moFile SoundFile;
    if (m_Sound3DFilename!=p_newfilename ) {

        m_Sound3DFilename = p_newfilename;

        m_pAudio = (moSound3DAL*)m_pSM->GetSound( m_Sound3DFilename, false );

        if (m_pAudio) {
        } else {

          SoundFile = pDataMan->GetDataFile( m_Sound3DFilename );
          m_Sound3DFilenameFull = SoundFile.GetCompletePath();
          m_pAudio = new moSound3DAL();
          if (m_pAudio) {
            m_pAudio->Init();
            //m_pAudio->Init( m_Sound3DFilename, m_Sound3DFilenameFull );
            m_pAudio->SetName( m_Sound3DFilename );
            m_pAudio->SetFileName( m_Sound3DFilenameFull );

            MODebug2->Message("moEffectSound3D::UpdateSound > New audio building from file: " + m_pAudio->GetName() );
            m_pAudio->BuildFromFile( m_pAudio->GetFileName() );

            m_pSM->AddSound( (moSound*) m_pAudio );
            //m_pAudio->Play();
          }

        }

    }

    if (m_bLaunch != m_Config.Int(moR(SOUND3D_LAUNCH)) ) {

     // if (m_pAudio->State()!=MO_STREAMSTATE_PLAYING) {
        m_pAudio->Play();
      //}

      m_bLaunch = m_Config.Int(moR(SOUND3D_LAUNCH));
    }

}

void moEffectSound3D::Draw( moTempo* tempogral, moEffectState* parentstate )
{
    MOint indeximage;
    MOdouble PosTextX0, PosTextX1, PosTextY0, PosTextY1;
    int ancho,alto;
    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    PreDraw( tempogral, parentstate);

    UpdateParameters();

    /*

    */


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
    /*
    moStreamState audiostate = m_Audio.GetState();
    moText  textstate = m_Audio.StateToText( audiostate );
    long    audioposition;

    last_ticks = moGetTicks();
*/
    /*audioposition = m_Audio.GetPosition();



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
    */
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
    /*
    moFont* pFont = m_pResourceManager->GetFontMan()->GetFont(0);

    textstate+= moText(" position") + IntToStr(audioposition);
    textstate+= moText(" diff:") + (moText)IntToStr( abs(audioposition - last_ticks) );

    if (pFont) {
        pFont->Draw(    0.0,
                        0.0,
                        textstate );
    }
    */
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
	p_configdefinition->Add( moText("sound"), MO_PARAM_SOUND, SOUND3D_SOUND, moValue( "default", "TXT") );

  p_configdefinition->Add( moText("seekposition"), MO_PARAM_FUNCTION, SOUND3D_SEEKPOSITION, moValue( "0", MO_VALUE_FUNCTION ).Ref() );
  p_configdefinition->Add( moText("mode"), MO_PARAM_NUMERIC, SOUND3D_MODE, moValue( "0", MO_VALUE_NUM_INT ) );
	p_configdefinition->Add( moText("loop"), MO_PARAM_NUMERIC, SOUND3D_LOOP, moValue( "0", MO_VALUE_NUM_INT ) );
	p_configdefinition->Add( moText("launch"), MO_PARAM_NUMERIC, SOUND3D_LAUNCH, moValue( "0", MO_VALUE_NUM_INT ) );
	p_configdefinition->Add( moText("speedofsound"), MO_PARAM_FUNCTION, SOUND3D_SPEEDOFSOUND, moValue( "0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("pitch"), MO_PARAM_FUNCTION, SOUND3D_PITCH, moValue( "1.0", MO_VALUE_FUNCTION).Ref() );

	//p_configdefinition->Add( moText("sound"), MO_PARAM_TEXT, SOUND_SOUND, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("volume"), MO_PARAM_FUNCTION, SOUND3D_VOLUME, moValue( "1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("balance"), MO_PARAM_TRANSLATEX, SOUND3D_BALANCE, moValue( "0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, SOUND3D_TEXTURE, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, SOUND3D_BLENDING, moValue( "0", "NUM").Ref() );


	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, SOUND3D_TRANSLATEX, moValue( "0.5", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, SOUND3D_TRANSLATEY, moValue( "0.5", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, SOUND3D_TRANSLATEZ );
	p_configdefinition->Add( moText("directionx"), MO_PARAM_FUNCTION, SOUND3D_DIRECTIONX );
	p_configdefinition->Add( moText("directiony"), MO_PARAM_FUNCTION, SOUND3D_DIRECTIONY );
	p_configdefinition->Add( moText("directionz"), MO_PARAM_FUNCTION, SOUND3D_DIRECTIONZ );
	p_configdefinition->Add( moText("speedx"), MO_PARAM_FUNCTION, SOUND3D_SPEEDX );
	p_configdefinition->Add( moText("speedy"), MO_PARAM_FUNCTION, SOUND3D_SPEEDY );
	p_configdefinition->Add( moText("speedz"), MO_PARAM_FUNCTION, SOUND3D_SPEEDZ );
	return p_configdefinition;
}



void
moEffectSound3D::Update( moEventList *Events ) {

	//get the pointer from the Moldeo Object sending it...
	moMoldeoObject::Update(Events);


}
