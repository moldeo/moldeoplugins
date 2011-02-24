/*******************************************************************************

                               moEffectSound.cpp

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

#include "moEffectSound.h"

//========================
//  Factory
//========================

moEffectSoundFactory *m_EffectSoundoFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if (m_EffectSoundoFactory==NULL)
		m_EffectSoundoFactory = new moEffectSoundFactory();
	return (moEffectFactory*) m_EffectSoundoFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectSoundoFactory;
	m_EffectSoundoFactory = NULL;
}

moEffect*  moEffectSoundFactory::Create() {
	return new moEffectSound();
}

void moEffectSoundFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================
moEffectSound::moEffectSound() {

	SetName("sound");
	Sound = NULL;
}

moEffectSound::~moEffectSound() {
	Finish();
}


moConfigDefinition *
moEffectSound::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("sound"), MO_PARAM_SOUND, SOUND_SOUND, moValue( "default", MO_VALUE_TXT ) );
	p_configdefinition->Add( moText("mode"), MO_PARAM_NUMERIC, SOUND_MODE, moValue( "0", MO_VALUE_NUM_INT ) );
	p_configdefinition->Add( moText("loop"), MO_PARAM_NUMERIC, SOUND_LOOP, moValue( "1", MO_VALUE_NUM_INT ) );
	p_configdefinition->Add( moText("position"), MO_PARAM_FUNCTION, SOUND_POSITION, moValue( "0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("pitch"), MO_PARAM_FUNCTION, SOUND_PITCH, moValue( "1.0", MO_VALUE_FUNCTION).Ref() );

	//p_configdefinition->Add( moText("sound"), MO_PARAM_TEXT, SOUND_SOUND, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("volume"), MO_PARAM_FUNCTION, SOUND_VOLUME, moValue( "1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("balance"), MO_PARAM_TRANSLATEX, SOUND_BALANCE, moValue( "0.0", MO_VALUE_FUNCTION).Ref() );

	///ECHO - audioecho - gstreamer -
	p_configdefinition->Add( moText("echo_delay"), MO_PARAM_FUNCTION, SOUND_ECHO_DELAY, moValue( "0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("echo_intensity"), MO_PARAM_FUNCTION, SOUND_ECHO_INTENSITY, moValue( "0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("echo_feedback"), MO_PARAM_FUNCTION, SOUND_ECHO_FEEDBACK, moValue( "0", MO_VALUE_FUNCTION).Ref() );

	p_configdefinition->Add( moText("position_in"), MO_PARAM_FUNCTION, SOUND_POSITION_IN, moValue( "0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("position_out"), MO_PARAM_FUNCTION, SOUND_POSITION_OUT, moValue( "1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("fade_in"), MO_PARAM_FUNCTION, SOUND_FADE_IN, moValue( "0.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("fade_out"), MO_PARAM_FUNCTION, SOUND_FADE_OUT, moValue( "0.0", MO_VALUE_FUNCTION).Ref() );


	return p_configdefinition;
}

MOboolean moEffectSound::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( SOUND_ALPHA, moText("alpha") );
	moDefineParamIndex( SOUND_COLOR, moText("color") );
	moDefineParamIndex( SOUND_SYNC, moText("syncro") );
	moDefineParamIndex( SOUND_PHASE, moText("phase") );

	moDefineParamIndex( SOUND_SOUND, moText("sound") );
	moDefineParamIndex( SOUND_MODE, moText("mode") );
	moDefineParamIndex( SOUND_LOOP, moText("loop") );

	moDefineParamIndex( SOUND_POSITION, moText("position") );
	moDefineParamIndex( SOUND_PITCH, moText("pitch") );
	moDefineParamIndex( SOUND_VOLUME, moText("volume") );
  moDefineParamIndex( SOUND_BALANCE, moText("balance") );

  moDefineParamIndex( SOUND_ECHO_DELAY, moText("echo_delay") );
  moDefineParamIndex( SOUND_ECHO_INTENSITY, moText("echo_intensity") );
  moDefineParamIndex( SOUND_ECHO_FEEDBACK, moText("echo_feedback") );

  moDefineParamIndex( SOUND_POSITION_IN, moText("position_in") );
  moDefineParamIndex( SOUND_POSITION_OUT, moText("position_out") );
  moDefineParamIndex( SOUND_FADE_IN, moText("fade_in") );
  moDefineParamIndex( SOUND_FADE_OUT, moText("fade_out") );

	moDefineParamIndex( SOUND_INLET, moText("inlet") );
	moDefineParamIndex( SOUND_OUTLET, moText("outlet") );
/*
    Tx = Ty = Tz = Rx = Ry = Rz = 0.0;
	Sx = Sy = Sz = 1.0;

    m_SoundFilename = moText("");

    m_Audio.InitGraph();
    m_bAudioStarted = false;

    if (m_SoundFilename!=m_Config[moParamReference(SOUND_SOUND)][MO_SELECTED][0].Text() ) {

        m_SoundFilename = m_Config[moParamReference(SOUND_SOUND)][MO_SELECTED][0].Text();
        m_SoundFilenameFull = (moText)m_pResourceManager->GetDataMan()->GetDataPath() + (moText)moSlash + (moText)m_SoundFilename;

        m_Audio.BuildLiveSound( m_SoundFilenameFull );

    }

    last_ticks = 0;
*/
  m_UserPosition = 0;
	return true;
}



#define KLT_TRACKED           0
#define KLT_NOT_FOUND        -1
#define KLT_SMALL_DET        -2
#define KLT_MAX_ITERATIONS   -3
#define KLT_OOB              -4
#define KLT_LARGE_RESIDUE    -5

void moEffectSound::Draw( moTempo* tempogral, moEffectState* parentstate )
{
    MOint indeximage;
	MOdouble PosTextX0, PosTextX1, PosTextY0, PosTextY1;
    int ancho,alto;
    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    bool bReBalance = false;

    PreDraw( tempogral, parentstate);

/*
    if (m_SoundFilename!=m_Config[moParamReference(SOUND_SOUND)][MO_SELECTED][0].Text() ) {

        m_SoundFilename = m_Config[moParamReference(SOUND_SOUND)][MO_SELECTED][0].Text();
        m_SoundFilenameFull = (moText)m_pResourceManager->GetDataMan()->GetDataPath() + (moText)moSlash + (moText)m_SoundFilename;

        m_Audio.BuildLiveSound( m_SoundFilenameFull );

    }
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

    audioposition = m_Audio.GetPosition();



    if ( (long) abs(audioposition - last_ticks) > 8000 ) {
       if (audiostate!=MO_STREAMSTATE_STOPPED)
           m_Audio.Seek( last_ticks );
           audioposition = last_ticks;

    }

    moSetDuration( audioposition );

    if (moTimeManager::MoldeoTimer->Paused()) {

        if (audiostate!=MO_STREAMSTATE_PAUSED)
            m_Audio.Pause();

    } else {

        if ( moTimeManager::MoldeoTimer->Started() ) {
            if (audiostate!=MO_STREAMSTATE_PLAYING)
                m_Audio.Play();
        } else {
            if (audiostate!=MO_STREAMSTATE_STOPPED)
                m_Audio.Stop();
        }

    }
*/
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

    SetColor( m_Config[moR(SOUND_COLOR)][MO_SELECTED], m_Config[moR(SOUND_ALPHA)][MO_SELECTED], state );

    moSound* NextSound = NULL;
    if (m_Config[ moR(SOUND_SOUND) ].GetData())
      NextSound = m_Config[ moR(SOUND_SOUND) ].GetData()->Sound();

    if (NextSound && Sound) {
      if ( Sound->GetName() != NextSound->GetName() ) {

        ///si se cambió de película
        if (Sound->Initialized()) {
          ///parar o pausar...
          ///faltaria un parametro: STOP on CHANGE, o PAUSE ON CHANGE
          Sound->Stop();
          bReBalance = true;
        }

      }
    }

    Sound = NextSound;

    int mLoop = m_Config.Int(moR(SOUND_LOOP));
    int mMode = m_Config.Int(moR(SOUND_MODE));

    MOulong AudioPosition,AudioDuration; //in ms
    moText tPosition,tDuration;

    if (Sound) {

      AudioPosition = Sound->GetPosition();
      AudioDuration = Sound->GetDuration();
      tPosition = moVideoManager::NanosecondsToTimecode((MOuint64)AudioPosition*(MOuint64)1000000);
      tDuration = moVideoManager::NanosecondsToTimecode((MOuint64)AudioDuration*(MOuint64)1000000);

      double mUserPosition = m_Config.Eval(moR(SOUND_POSITION));

      if (mUserPosition!=m_UserPosition) {
        //MODebug2->Push(" mUserPosition:" + FloatToStr(mUserPosition) + "/" + FloatToStr(m_UserPosition) );
        Sound->Seek( (MOulong)mUserPosition, 1.0 );
        m_UserPosition = mUserPosition;
      }

      ///estamos en syncro de todas maneras con el clock de la Consola...
      if (moTimeManager::MoldeoTimer->Paused()) {
        Sound->Pause();
      }

      /// caso por Timer PARADO (NOT STARTED)
      if (!moTimeManager::MoldeoTimer->Started()) {
        if (Sound->IsPlaying())
          Sound->Stop();
      }

      if (!Sound->IsPlaying() && moTimeManager::MoldeoTimer->Started()) {
         Sound->Play();
      }

      switch((SoundMode)mMode) {

          case MO_SOUND_MODE_PLAYLIST:
            if ( AudioPosition >= AudioDuration || Sound->IsEOS()) {

              if ( mLoop && m_Config.GetParam(moR(SOUND_SOUND)).GetIndexValue() == (m_Config.GetParam(moR(SOUND_SOUND)).GetValuesCount()-1) ) {
                  m_Config.GetParam(moR(SOUND_SOUND)).FirstValue();
              } else m_Config.GetParam(moR(SOUND_SOUND)).NextValue();

              if (Sound->State()!=MO_STREAMSTATE_STOPPED) {
                Sound->Stop();
              }
              MODebug2->Push( "EOS:" + IntToStr((int)Sound->IsEOS()) );
            }
            break;

          default:
              if (mLoop) {
                if ( AudioPosition >= AudioDuration || Sound->IsEOS()) {
                  Sound->Pause();
                  Sound->Seek( 0, 1.0 );
                  //if ( m_FramePosition >= (Sound->GetFrameCount()-1) || Sound->IsEOS() ) {
                    //Sound->Pause();
                    //
                    //Sound->Play();
                  //}
                }
              }
            break;

      }

      float mVolume = m_Config.Eval( moR(SOUND_VOLUME) );

      if ( mVolume !=Sound->GetVolume() || bReBalance ) {
        Sound->SetVolume( mVolume );
      }

      float mPitch = m_Config.Eval( moR(SOUND_PITCH) );

      long pitchi1 = ( mPitch * 100.0 );
      long pitchi2 = ( Sound->GetPitch() * 100.0 );

      if ( pitchi1 != pitchi2 ) {
        mPitch = (float)pitchi1 / 100.0;
        Sound->SetPitch( mPitch );
      }

      float mBalance = m_Config.Eval( moR(SOUND_BALANCE) );

      if ( mBalance != Sound->GetBalance() || bReBalance ) {
        Sound->SetBalance( mBalance );
      }

      float mEchoDelay = m_Config.Eval( moR(SOUND_ECHO_DELAY) );

      if ( mEchoDelay != Sound->GetEchoDelay() || bReBalance ) {
        Sound->SetEchoDelay( mEchoDelay );
      }

      float mEchoIntensity = m_Config.Eval( moR(SOUND_ECHO_INTENSITY) );

      if ( mEchoIntensity != Sound->GetEchoIntensity() || bReBalance ) {
        Sound->SetEchoIntensity( mEchoIntensity );
      }

      float mEchoFeedback = m_Config.Eval( moR(SOUND_ECHO_FEEDBACK) );

      if ( mEchoFeedback != Sound->GetEchoFeedback() || bReBalance ) {
        Sound->SetEchoFeedback( mEchoFeedback );
      }

    }

    moFont* pFont = m_pResourceManager->GetFontMan()->GetFont(0);
/*
    textstate+= moText(" position") + IntToStr(audioposition);
    textstate+= moText(" diff:") + (moText)IntToStr( abs(audioposition - last_ticks) );
*/
    moText textaudio = tPosition + " / " + tDuration;

    if (pFont) {
        pFont->Draw(    0.0,
                        0.0,
                        textaudio );
    }

}

MOboolean moEffectSound::Finish()
{
    return PreFinish();
}

void moEffectSound::Interaction( moIODeviceManager *IODeviceManager ) {

	moDeviceCode *temp;
	MOint did,cid,state,valor;

	moEffect::Interaction( IODeviceManager );

}

void
moEffectSound::Update( moEventList *Events ) {

	//get the pointer from the Moldeo Object sending it...
	moMoldeoObject::Update(Events);

  ///SE APAGA EL SONIDO SI EL EFECTO ESTA DESACTIVADO

  if ( state.on==MO_ON && m_Config.Sound( moR(SOUND_SOUND) ).State()!=MO_STREAMSTATE_PLAYING ) {
    m_Config.Sound( moR(SOUND_SOUND) ).Play();
  } else if ( state.on==MO_OFF  && m_Config.Sound( moR(SOUND_SOUND) ).State()==MO_STREAMSTATE_PLAYING ) {
    m_Config.Sound( moR(SOUND_SOUND) ).Pause();
  }

}
