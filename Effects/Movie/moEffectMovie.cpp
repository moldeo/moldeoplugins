/*******************************************************************************

                                moEffectMovie.cpp

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
  Andres Colubri

*******************************************************************************/

#include "moEffectMovie.h"
#ifdef MO_WIN32
    #include "SDL.h"
#else
    #include "SDL/SDL.h"
#endif

//========================
//  Factory
//========================

moEffectMovieFactory *m_EffectMovieFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectMovieFactory==NULL)
		m_EffectMovieFactory = new moEffectMovieFactory();
	return(moEffectFactory*) m_EffectMovieFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectMovieFactory;
	m_EffectMovieFactory = NULL;
}

moEffect*  moEffectMovieFactory::Create() {
	return new moEffectMovie();
}

void moEffectMovieFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectMovie::moEffectMovie() {
	SetName("movie");
	movielaunched = false;

  m_pOutletFramePosition = NULL;
  m_pInletFramePosition = NULL;
}

moEffectMovie::~moEffectMovie() {
	Finish();
}


moConfigDefinition *
moEffectMovie::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("movies"), MO_PARAM_TEXTURE, MOVIE_MOVIES );
	p_configdefinition->Add( moText("sounds"), MO_PARAM_SOUND, MOVIE_SOUNDS );
	p_configdefinition->Add( moText("position"), MO_PARAM_FUNCTION, MOVIE_POSITION, moValue( "0.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("speed"), MO_PARAM_FUNCTION, MOVIE_SPEED, moValue( "1.0","FUNCTION").Ref() );
  p_configdefinition->Add( moText("volume"), MO_PARAM_FUNCTION, MOVIE_VOLUME, moValue( "1.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("balance"), MO_PARAM_FUNCTION, MOVIE_BALANCE, moValue( "0.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("brightness"), MO_PARAM_FUNCTION, MOVIE_BRIGHTNESS, moValue( "0.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("contrast"), MO_PARAM_FUNCTION, MOVIE_CONTRAST, moValue( "1.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("saturation"), MO_PARAM_FUNCTION, MOVIE_SATURATION, moValue( "1.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("hue"), MO_PARAM_FUNCTION, MOVIE_HUE, moValue( "0.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("moviemode"), MO_PARAM_NUMERIC, MOVIE_MODE, moValue( "0","NUM").Ref(), moText("VCR,SCRIPT,CYCLE,PLAYLIST,LAUNCH") );
	p_configdefinition->Add( moText("movielaunch"), MO_PARAM_NUMERIC, MOVIE_LAUNCH, moValue( "0","NUM").Ref(), moText("OFF,ON") );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, MOVIE_BLENDING);
	p_configdefinition->Add( moText("startplaying"), MO_PARAM_NUMERIC, MOVIE_STARTPLAYING, moValue( "0","NUM").Ref(), moText("OFF,ON"));
	p_configdefinition->Add( moText("loop"), MO_PARAM_NUMERIC, MOVIE_LOOP, moValue( "0","NUM").Ref() );
	p_configdefinition->Add( moText("showtrackdata"), MO_PARAM_NUMERIC, MOVIE_SHOWTRACKDATA, moValue( "0","NUM").Ref() );
	p_configdefinition->Add( moText("interpolation"), MO_PARAM_NUMERIC, MOVIE_INTERPOLATION, moValue( "0","NUM").Ref() );
	p_configdefinition->Add( moText("pos_text_x"), MO_PARAM_FUNCTION, MOVIE_POSTEXX, moValue( "0.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("pos_text_y"), MO_PARAM_FUNCTION, MOVIE_POSTEXY, moValue( "0.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("anc_text_x"), MO_PARAM_FUNCTION, MOVIE_ANCTEXX, moValue( "1.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("alt_text_y"), MO_PARAM_FUNCTION, MOVIE_ALTTEXY, moValue( "1.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("pos_cuad_x"), MO_PARAM_FUNCTION, MOVIE_POSCUADX, moValue( "0.0","FUNCTION").Ref()  );
	p_configdefinition->Add( moText("pos_cuad_y"), MO_PARAM_FUNCTION, MOVIE_POSCUADY, moValue( "0.0","FUNCTION").Ref()  );
	p_configdefinition->Add( moText("anc_cuad_x"), MO_PARAM_FUNCTION, MOVIE_ANCCUADX, moValue( "1.0","FUNCTION").Ref()  );
	p_configdefinition->Add( moText("alt_cuad_y"), MO_PARAM_FUNCTION, MOVIE_ALTCUADY, moValue( "1.0","FUNCTION").Ref()  );

	p_configdefinition->Add( moText("showmoviedata"), MO_PARAM_NUMERIC, MOVIE_SHOWMOVIEDATA, moValue( "0","NUM").Ref() );
	p_configdefinition->Add( moText("display_x"), MO_PARAM_FUNCTION, MOVIE_DISPLAY_X, moValue( "-0.5","FUNCTION").Ref()  );
	p_configdefinition->Add( moText("display_y"), MO_PARAM_FUNCTION, MOVIE_DISPLAY_Y, moValue( "-0.35","FUNCTION").Ref()  );
	p_configdefinition->Add( moText("display_width"), MO_PARAM_FUNCTION, MOVIE_DISPLAY_WIDTH, moValue( "1.0","FUNCTION").Ref()  );
	p_configdefinition->Add( moText("display_height"), MO_PARAM_FUNCTION, MOVIE_DISPLAY_HEIGHT, moValue( "0.05","FUNCTION").Ref()  );
  p_configdefinition->Add( moText("no_audio_output"), MO_PARAM_NUMERIC, MOVIE_NO_AUDIO_OUTPUT, moValue( "0","NUM").Ref() );
  p_configdefinition->Add( moText("no_video_output"), MO_PARAM_NUMERIC, MOVIE_NO_VIDEO_OUTPUT, moValue( "0","NUM").Ref() );
  p_configdefinition->Add( moText("no_text_output"), MO_PARAM_NUMERIC, MOVIE_NO_TEXT_OUTPUT, moValue( "0","NUM").Ref() );

	return p_configdefinition;
}


MOboolean
moEffectMovie::Init() {

  if (!m_pInletFramePosition) m_pInletFramePosition = new moInlet();

  if (m_pInletFramePosition) {
    MODebug2->Message("Inlet Frame Position Initializing");
    ((moConnector*)m_pInletFramePosition)->Init( moText("FRAMEPOSITION"), m_Inlets.Count(), MO_DATA_NUMBER_DOUBLE );
    m_Inlets.Add(m_pInletFramePosition);
  }

  if (!PreInit()) return false;

  // Hacer la inicializacion de este efecto en particular.
  moDefineParamIndex( MOVIE_ALPHA, moText("alpha") );
  moDefineParamIndex( MOVIE_COLOR, moText("color") );
  moDefineParamIndex( MOVIE_SYNC, moText("syncro") );
  //moDefineParamIndex( MOVIE_SCRIPT, moText("script") );
  moDefineParamIndex( MOVIE_MOVIES, moText("movies") );
  moDefineParamIndex( MOVIE_SOUNDS, moText("sounds") );
  moDefineParamIndex( MOVIE_MODE, moText("moviemode") );
  moDefineParamIndex( MOVIE_LAUNCH, moText("movielaunch") );
  moDefineParamIndex( MOVIE_BLENDING, moText("blending") );
  moDefineParamIndex( MOVIE_STARTPLAYING, moText("startplaying") );
  moDefineParamIndex( MOVIE_LOOP, moText("loop") );

  moDefineParamIndex( MOVIE_POSITION, moText("position") );
  moDefineParamIndex( MOVIE_SPEED, moText("speed") );
  moDefineParamIndex( MOVIE_VOLUME, moText("volume") );
  moDefineParamIndex( MOVIE_BALANCE, moText("balance") );
  moDefineParamIndex( MOVIE_BRIGHTNESS, moText("brightness") );
  moDefineParamIndex( MOVIE_CONTRAST, moText("contrast") );
  moDefineParamIndex( MOVIE_SATURATION, moText("saturation") );
  moDefineParamIndex( MOVIE_HUE, moText("hue") );


  moDefineParamIndex( MOVIE_SHOWMOVIEDATA, moText("showmoviedata") );
  moDefineParamIndex( MOVIE_INTERPOLATION, moText("interpolation") );
  moDefineParamIndex( MOVIE_POSTEXX, moText("pos_text_x") );
  moDefineParamIndex( MOVIE_POSTEXY, moText("pos_text_y") );
  moDefineParamIndex( MOVIE_ANCTEXX, moText("anc_text_x") );
  moDefineParamIndex( MOVIE_ALTTEXY, moText("alt_text_y") );
  moDefineParamIndex( MOVIE_POSCUADX, moText("pos_cuad_x") );
  moDefineParamIndex( MOVIE_POSCUADY, moText("pos_cuad_y") );
  moDefineParamIndex( MOVIE_ANCCUADX, moText("anc_cuad_x") );
  moDefineParamIndex( MOVIE_ALTCUADY, moText("alt_cuad_y") );

  moDefineParamIndex( MOVIE_SHOWTRACKDATA, moText("showtrackdata") );
  moDefineParamIndex( MOVIE_DISPLAY_X, moText("display_x") );
  moDefineParamIndex( MOVIE_DISPLAY_Y, moText("display_y") );
  moDefineParamIndex( MOVIE_DISPLAY_WIDTH, moText("display_width") );
  moDefineParamIndex( MOVIE_DISPLAY_HEIGHT, moText("display_height") );

  moDefineParamIndex( MOVIE_NO_AUDIO_OUTPUT, moText("no_audio_output") );
  moDefineParamIndex( MOVIE_NO_VIDEO_OUTPUT, moText("no_video_output") );
  moDefineParamIndex( MOVIE_NO_TEXT_OUTPUT, moText("no_text_output") );

  moDefineParamIndex( MOVIE_INLET, moText("inlet") );
  moDefineParamIndex( MOVIE_OUTLET, moText("outlet") );

  m_PlaySpeed = 0.0;
  m_PlayState = MO_MOVIE_PLAYSTATE_STOPPED;//speed 0 0.0
  m_SeekState = MO_MOVIE_SEEKSTATE_REACHED;//frame 0

  m_bDisplayOn = false;
  m_bPlayingSound = false;

  m_FramesLength = 0;
  m_FramePosition = 0;
  m_FramePositionAux = 0;
  m_FramePositionF = 0.0;
  m_FramePositionFAux = 0.0;
  m_DiffFrame = 0.0;
  m_FramePositionInc = 0.0;

  m_pAnim = NULL;
  m_pMovie = NULL;
  m_pTexture = NULL;
  m_pSound = NULL;

  m_pNextMovie = NULL;
  m_pNextTexture = NULL;

  showmoviedata = true;/** Start timer to shut down display info*/
  movielaunchon = MO_MOVIE_LAUNCH_OFF;

  /*
  m_pSound = m_pResourceManager->GetSoundMan()->GetSound(m_Config[moParamReference(MOVIE_SOUNDS)][MO_SELECTED][0].Text());
  if (m_pSound) {
  m_pSound->SetPosition( m_Config[moParamReference(MOVIE_SOUNDS)][MO_SELECTED][1].Float(),
  		m_Config[moParamReference(MOVIE_SOUNDS)][MO_SELECTED][2].Float(),
  		m_Config[moParamReference(MOVIE_SOUNDS)][MO_SELECTED][3].Float());
  }

  {
  moText wavfilename = m_Config[moParamReference(MOVIE_SOUNDS)][MO_SELECTED][0].Text();
  wavfilename = (moText)m_pResourceManager->GetDataMan()->GetDataPath() + (moText)moSlash + (moText)wavfilename;

  //m_SoundSystem.InitGraph();


  //if (m_SoundSystem.BuildLiveSound( wavfilename )) {

  //}

  }
  */
  /*
  m_MovieScriptFN = m_Config[moParamReference(MOVIE_SCRIPT)][MO_SELECTED][0].Text();
  if ((moText)m_MovieScriptFN!=moText("")) {

  moScript::InitScript();
  RegisterFunctions();

  m_MovieScriptFN = m_pResourceManager->GetDataMan()->GetDataPath()+ moText("/") + (moText)m_MovieScriptFN;

  if ( CompileFile(m_MovieScriptFN) ) {

    SelectScriptFunction( "Init" );
    AddFunctionParam( m_FramesPerSecond );
    AddFunctionParam( (int)m_FramesLength );
    AddFunctionParam( m_pResourceManager->GetRenderMan()->RenderWidth() );
    AddFunctionParam( m_pResourceManager->GetRenderMan()->RenderHeight() );
    RunSelectedFunction();

  } else {
    MODebug2->Error("couldnt compile script");
  }
  }
  */
  m_bTrackerInit = false;
  m_pTrackerData = NULL;
  //m_pTrackerGpuData = NULL;

  m_TicksAux = m_EffectState.tempo.ticks;
  m_Ticks = m_TicksAux;

  return true;
}

void moEffectMovie::UpdateParameters() {

  moTexture *pTexture;
  bool bReBalance = false;

  m_bStartPlayingOn = m_Config.Int(moR(MOVIE_STARTPLAYING));
	m_bLoop = m_Config.Int(moR(MOVIE_LOOP));
	m_bInterpolation = m_Config.Int(moR(MOVIE_INTERPOLATION));

	//registro de los dos ticks correspondientes a un Frame
	m_TicksAux = m_Ticks;
	m_Ticks = m_EffectState.tempo.ticks;

	pTexture = m_Config[moR(MOVIE_MOVIES)][MO_SELECTED][0].Texture();
	m_pSound = m_Config[moR(MOVIE_SOUNDS)][MO_SELECTED][0].Sound();

  /** TRATAMIENTO DE LOS CUADROS DE LA TEXTURA
  * TODO:
  *
  */
  bReBalance = UpdateMovieTexture( pTexture );
  UpdateMoviePosition();
  UpdateFilterParameters(bReBalance);
  UpdateInlets();
  UpdateOutlets();
}

MOboolean moEffectMovie::UpdateMovieTexture(moTexture* p_Texture) {

  MOboolean bReBalance = false;

  /**
  *    ASIGNACION DE OBJETOS
  */
  if (!p_Texture) return bReBalance;

  /// CAMBIO O SALTO DE PELICULA....
  if (m_pTexture) {
    if (p_Texture->GetName()!=m_pTexture->GetName()) {

      ///si se cambió de película
      if (m_pMovie) {
        ///parar o pausar...
        ///TODO: faltaria un parametro: STOP on CHANGE, o PAUSE ON CHANGE
        m_pMovie->Pause();
        //m_pMovie->Stop();
        if (m_pMovie->IsPaused()) {
          m_pMovie->Seek(0);
        }
        //bReBalance = true;
        bReBalance = false;

        if (m_pMovie->IsEOS()) {
          //m_pMovie->Stop();
        }
      }
    }
  }

  ///Asignamos la textura nueva...
  m_pTexture = p_Texture;

  if (!m_pTexture) return bReBalance;

  switch(m_pTexture->GetType()) {
    case MO_TYPE_MOVIE:
      ///REASIGNAMOS
      m_pMovie = dynamic_cast<moMovie*>(m_pTexture);
      m_pAnim = dynamic_cast<moTextureAnimated*>(m_pTexture);
      break;

    case MO_TYPE_CIRCULARVIDEOBUFFER:
    case MO_TYPE_VIDEOBUFFER:
    case MO_TYPE_TEXTURE_MULTIPLE:
    case MO_TYPE_TEXTUREBUFFER:
      m_pAnim = dynamic_cast<moTextureAnimated*>(m_pTexture);
      break;
    default:
      break;
  }

  return bReBalance;

}

void moEffectMovie::UpdateMoviePosition() {
  long  UserPosition;

  ///user position, convert from function to long...
  UserPosition = (long)m_Config.Eval(moR(MOVIE_POSITION));

  if (!m_pTexture) return;

  MOboolean is_texture_animated = m_pTexture->GetType()!=MO_TYPE_TEXTURE && m_pTexture->GetType()!=MO_TYPE_TEXTUREMEMORY;

  if (!is_texture_animated) return;

  if (UserPosition!=m_UserPosition) {
    m_UserPosition=UserPosition;
    //MODebug2->Push("changed user position:" + IntToStr(UserPosition) );
    if (m_pAnim) {
      m_FramePosition = m_UserPosition;
      ///MovieGLID() se ocupa del resto
      //m_pAnim->GetGLId( (MOuint) m_UserPosition );
    }
    if (m_pMovie) {

      //if (m_pMovie->IsPaused()) {
      //  m_pMovie->Seek( m_UserPosition );
      //}
      m_pMovie->Seek( m_UserPosition );
    }
  }

  /**
  *     SI DERIVA DE moTextureAnimated
  */
  if (m_pAnim) {
    m_FramesLength = m_pAnim->GetFrameCount();
    m_Config[moR(MOVIE_POSITION)][MO_SELECTED][0].SetRange( 0, m_FramesLength);
    m_FramesPerSecond = m_pAnim->GetFramesPerSecond();
    //m_FramePosition = m_pAnim->GetFrame();
    //m_FramePositionF = (double)m_FramePosition;
  }

  /**
  *   TRATAMIENTO DE VIDEOBUFFER
  */
  if ( m_pAnim->GetType()==MO_TYPE_VIDEOBUFFER ) {

    //m_FramePosition = m_pAnim->GetPosition();
    //m_FramePositionF = (double)m_FramePosition;

  }

  /**
  *     PELICULA (tratamiento)
  *     salto de posicion por el usuario
  *     2 modos:
  *             MO_PLAYMODE_TIMEBASE
  *             MO_PLAYMODE_FRAMEBASE
  */
  if (!m_pMovie) return;

  ///TODO: calculo de frame (solo si esta en modo VCR y en MO_PLAYMODE_TIMEBASE??)
  if (m_pMovie->GetPlayMode()==moMovie::MO_PLAYMODE_TIMEBASE) {

    //MODebug2->Message(moText("moMovie::Update > MO_PLAYMODE_TIMEBASE start check."));

    m_FramePosition = m_pMovie->GetPosition();
    m_FramePositionF = (double)m_FramePosition;

    ///estamos en syncro de todas maneras con el clock de la Consola...
    if ( (m_Ticks-m_TicksAux)<=0 ) { ///no esta avanzando el clock
      /// caso por Timer PAUSADO

      if ( ( moIsTimerPaused() || m_EffectState.tempo.Paused()) && m_pMovie->IsPlaying() ) {
        MODebug2->Message(moText("Timer is paused, should pause movie!!"));
        m_pMovie->Pause();
      }

      /// caso por Timer PARADO (NOT STARTED)
      if (moIsTimerStopped()  || m_EffectState.tempo.State()==MO_TIMERSTATE_STOPPED ) {
        //MODebug2->Message(moText("Timer is stopped, stop movie!!"));

        if (m_pMovie->IsPaused()) {
          m_pMovie->Seek(0);
        }

        if (m_pMovie->IsPlaying() || m_pMovie->IsEOS()) {
          MODebug2->Message(moText("Timer is stopped, should stop movie!!"));
          if ( m_pMovie->IsPlaying() ) {
            MODebug2->Message(moText("Timer is stopped, pausing and seek to 0"));
            m_pMovie->Pause();
            m_pMovie->Seek(0);
          }
          if (m_pMovie->IsEOS()) {
            MODebug2->Message(moText("DANGER. Timer is stopped and EOS reached. Do nothing!"));
            /*m_pMovie->Stop();*/
          }

          //m_pMovie->Stop();
        }
      }
    } else {
      ///TIMER CORRIENDO
      if (!m_pMovie->IsPlaying() && ( moIsTimerPlaying() && m_EffectState.tempo.State()==MO_TIMERSTATE_PLAYING ) ) {
        /// PELI NO CORRE

        if (m_pMovie->IsEOS() /*|| (m_FramePosition+5) >= m_FramesLength*/ ) {
          //MODebug2->Message(moText("Timer is playing, movie ended!!, do nothing...."));
          //m_pMovie->Play();
          //this->Disable();
          //this->TurnOff();
        } else if ( m_FramePosition>=0 && (m_FramePosition+MO_MOVIE_EOF_TOLERANCE)<=m_FramesLength ) {
        //MODebug2->Message(moText("Timer is playing, movie is not, try to play movie!!"));
        if (moviemode==MO_MOVIE_MODE_LAUNCH_AND_WAIT ) {
          if (movielaunchon == MO_MOVIE_LAUNCH_ON) {
            MODebug2->Message(moText("Launch ON"));
            m_pMovie->Play();
          }
        } else if (moviemode==MO_MOVIE_MODE_LAUNCH_NEXT_AND_WAIT ) {
          if (movielaunchon == MO_MOVIE_LAUNCH_ON) {
            MODebug2->Message(moText("Launch ON"));
            m_pMovie->Play();
          }
        } else m_pMovie->Play();
        //MODebug2->Message(moText("Timer is started, movie not: Play() was called..."));
        }


      } else if(m_pMovie->IsPlaying() && (moIsTimerPlaying() && m_EffectState.tempo.State()==MO_TIMERSTATE_PLAYING  ) ) {

        /// PELI SI CORRE
        //MODebug2->Message(moText("Timer is playing, movie is playing too, if position is 0, then play!!"));

        if (m_pMovie->GetPosition()==0) {
          //MODebug2->Message(moText("position is 0, Play()!!"));
          if (moviemode==MO_MOVIE_MODE_LAUNCH_AND_WAIT ) {
            if (movielaunchon == MO_MOVIE_LAUNCH_ON) {
                MODebug2->Message(moText("Movie Running (but at position 0). Launch ON"));
                m_pMovie->Play();
            }
          }
        }
      }
      //MODebug2->Message(moText("moMovie::Update > MO_PLAYMODE_TIMEBASE end check."));
    }

    /*
    MODebug2->Push(moText("TIMEBASE:") + IntToStr(m_Ticks-m_TicksAux)
         + moText(" IsPlaying:")
         + IntToStr(m_pMovie->IsPlaying())
         + moText(" MoldeoTimerStarted:")
         + IntToStr( moTimeManager::MoldeoTimer->Started() )
         + moText(" Position:")
         + IntToStr( m_pMovie->GetPosition() )
         );*/

  }

  ///fin PELICULA (tratamiento)
}

void moEffectMovie::UpdateFilterParameters(MOboolean p_ReBalance) {

  float Volume,Balance,Brightness,Contrast,Saturation,Hue;

  ///sound balance
  Volume = m_Config.Eval(moR(MOVIE_VOLUME));
  Balance = m_Config.Eval(moR(MOVIE_BALANCE));

  ///video balance
  Brightness = m_Config.Eval(moR(MOVIE_BRIGHTNESS));
  Contrast = m_Config.Eval(moR(MOVIE_CONTRAST));
  Saturation = m_Config.Eval(moR(MOVIE_SATURATION));
  Hue = m_Config.Eval(moR(MOVIE_HUE));

  /**
  *
  *
  *     BALANCES DE VOLUMEN, VOLUMEN, SATURACION, CONTRASTE, LUMINOSIDAD
  *
  */
  if (m_pMovie) {

    if (Volume!=m_Volume || p_ReBalance) { m_Volume=Volume; m_pMovie->SetVolume(m_Volume); }
    if (Balance!=m_Balance || p_ReBalance) { m_Balance=Balance; m_pMovie->SetBalance(m_Balance);}

    if (Brightness!=m_Brightness || p_ReBalance) { m_Brightness=Brightness; m_pMovie->SetBrightness(m_Brightness);}
    if (Contrast!=m_Contrast || p_ReBalance) { m_Contrast=Contrast; m_pMovie->SetContrast(m_Contrast);}
    if (Saturation!=m_Saturation || p_ReBalance) { m_Saturation=Saturation; m_pMovie->SetSaturation(m_Saturation);}
    if (Hue!=m_Hue || p_ReBalance) { m_Hue=Hue; m_pMovie->SetHue(m_Hue);}
    p_ReBalance = false;
  }

  if (m_pSound) {
    if (Volume!=m_Volume || p_ReBalance) { m_Volume=Volume; m_pSound->SetVolume(m_Volume); }
    if (Balance!=m_Balance || p_ReBalance) { m_Balance=Balance; m_pSound->SetBalance(m_Balance);}

    m_pSound->SetVolume(m_Volume);
    m_pSound->SetBalance(m_Balance);
  }

}

void moEffectMovie::UpdateInlets() {

  if (m_pInletFramePosition) {
    if (m_pInletFramePosition->GetData()) {
        m_pInletFramePosition->GetData()->SetDouble( m_FramePositionF );
        m_pInletFramePosition->Update(true);
    }
  }

  for( int i=0; i<m_Inlets.Count(); i++ ) {
    moInlet* pInlet = m_Inlets[i];

    if (pInlet->Updated() && pInlet->GetConnectorLabelName()==moText("TRACKERKLT")) {

      m_pTrackerData = (moTrackerSystemData *)pInlet->GetData()->Pointer();
      if (m_pTrackerData && !m_bTrackerInit) {
        m_bTrackerInit = true;
      }
    }
  }

}

void moEffectMovie::UpdateOutlets() {

  if (!m_pOutletFramePosition) {
    m_pOutletFramePosition = m_Outlets.GetRef( this->GetOutletIndex( moText("FRAMEPOSITION") ) );
  }

  if (m_pOutletFramePosition) {
    m_pOutletFramePosition->GetData()->SetFloat( m_FramePositionF );
    m_pOutletFramePosition->Update(true);
    //MODebug2->Message(FloatToStr(m_FramePositionF));
  }

}

void moEffectMovie::Draw( moTempo* tempogral, moEffectState* parentstate )
{

 	moviemode = (moEffectMovieMode) m_Config.Int(moR(MOVIE_MODE));
	movielaunchon = (moEffectMovieLaunch) m_Config.Int(moR(MOVIE_LAUNCH));

  UpdateParameters();

  MOuint FrameGLid;
  glEnable(GL_TEXTURE_2D);
  FrameGLid = MovieGLId();

  BeginDraw( tempogral, parentstate);

  moRenderManager* RENDER = m_pResourceManager->GetRenderMan();
  moGLManager* OPENGL = m_pResourceManager->GetGLMan();

  if ( RENDER==NULL || OPENGL==NULL) return;

  int w = RENDER->ScreenWidth();
  int h = RENDER->ScreenHeight();

  PosTextX = m_Config.Eval(moR(MOVIE_POSTEXX));
  AncTextX = m_Config.Eval(moR(MOVIE_ANCTEXX));
  PosTextY = m_Config.Eval(moR(MOVIE_POSTEXY));
  AltTextY = m_Config.Eval(moR(MOVIE_ALTTEXY));

  PosCuadX = m_Config.Eval(moR(MOVIE_POSCUADX));
  AncCuadX = m_Config.Eval(moR(MOVIE_ANCCUADX));
  PosCuadY = m_Config.Eval(moR(MOVIE_POSCUADY));
  AltCuadY = m_Config.Eval(moR(MOVIE_ALTCUADY));

  if (m_pTexture) {
      PosTextX0 = PosTextX * m_pTexture->GetMaxCoordS();
      PosTextX1 =(PosTextX + AncTextX)* m_pTexture->GetMaxCoordS();
      PosTextY0 =(1 - PosTextY)*m_pTexture->GetMaxCoordT();
      PosTextY1 =(1 - PosTextY - AltTextY)*m_pTexture->GetMaxCoordT();
  } else {
      PosTextX0 = PosTextX;
      PosTextX1 =(PosTextX + AncTextX);
      PosTextY0 =(1 - PosTextY);
      PosTextY1 =(1 - PosTextY - AltTextY);
  }
  PosCuadX0 = PosCuadX - AncCuadX/2;
  PosCuadX1 = PosCuadX + AncCuadX/2;
  PosCuadY1 = PosCuadY -  AltCuadY/2;
  PosCuadY0 = PosCuadY + AltCuadY/2;


  glDisable(GL_DEPTH_TEST);       // Disables Depth Testing
  glEnable(GL_ALPHA);

  moCamera3D Camera3D;
  //mGL->SetDefaultOrthographicView( w, h );
  //OPENGL->SetDefaultOrthographicView( w, h  );
  OPENGL->SetDefaultOrthographicView( w, h );
  Camera3D = OPENGL->GetProjectionMatrix();

  moPlaneGeometry ImageQuad( AncCuadX, AltCuadY, 1, 1 );
  /*
      m_VerticesUvs.Add( moVector2f( 0.0, 1.0) );
    m_VerticesUvs.Add( moVector2f( 0.0, 0.0) );
    m_VerticesUvs.Add( moVector2f( 1.0, 1.0) );
    m_VerticesUvs.Add( moVector2f( 1.0, 0.0) );
  */
  ImageQuad.m_VerticesUvs[ 0 ] = moTCoord( PosTextX0, PosTextY0 );
  ImageQuad.m_VerticesUvs[ 1 ] = moTCoord( PosTextX0, PosTextY1 );
  ImageQuad.m_VerticesUvs[ 2 ] = moTCoord( PosTextX1, PosTextY0 );
  ImageQuad.m_VerticesUvs[ 3 ] = moTCoord( PosTextX1, PosTextY1 );

  moMaterial Material;
  Material.m_Map = m_pTexture;
  Material.m_MapGLId = FrameGLid;
  moVector4d color = m_Config.EvalColor( moR(MOVIE_COLOR) );
  Material.m_Color = moColor( color.X(), color.Y(), color.Z() );

  moGLMatrixf Model;
  Model.MakeIdentity();
  Model.Translate( PosCuadX, PosCuadY, 0.0 );

  moMesh Mesh( ImageQuad, Material );
  Mesh.SetModelMatrix( Model );

  SetColor( m_Config[moR(MOVIE_COLOR)], m_Config[moR(MOVIE_ALPHA)], m_EffectState );
  SetBlending( (moBlendingModes) m_Config.Int( moR(MOVIE_BLENDING) ) );

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, FrameGLid );

#ifndef OPENGLESV2
  //RENDER->Render( &Mesh, &Camera3D );

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

#else
  RENDER->Render( &Mesh, &Camera3D );
#endif


  bool showfeat = m_Config.Int( moR(MOVIE_SHOWTRACKDATA) );
  if (m_bTrackerInit && m_pTrackerData && showfeat) {
    moGLViewport vport = m_pResourceManager->GetGLMan()->GetViewport();
    float aspect = vport.GetProportion();
    if (aspect==0.0) aspect = 1.0;
    m_pTrackerData->DrawFeatures( 1.0, 1.0 / aspect, 0.5, 0.5 );
  }

  showmoviedata = m_Config.Int( moR(MOVIE_SHOWMOVIEDATA) );

  m_DisplayX = m_Config.Eval( moR(MOVIE_DISPLAY_X) );
  m_DisplayY = m_Config.Eval( moR(MOVIE_DISPLAY_Y) );
  m_DisplayW = m_Config.Eval( moR(MOVIE_DISPLAY_WIDTH) );
  m_DisplayH = m_Config.Eval( moR(MOVIE_DISPLAY_HEIGHT) );

	if (showmoviedata) {
		DrawDisplayInfo( PosCuadX0, PosCuadY0 );
	}

  glEnable(GL_TEXTURE_2D);

  EndDraw();
}

MOuint moEffectMovie::MovieGLId() {

  MOuint glid = 0;

  //MODebug2->Push("m_FramePosition:"+IntToStr(m_FramePosition));

  switch(moviemode) {
    case MO_MOVIE_MODE_LAUNCH_AND_WAIT:
		case MO_MOVIE_MODE_VCR:
		  {
            if (m_pAnim) {
              ///Internally if FramePosition doesnt change GetFrame is not called... (for optimization)
              ///glid = m_pAnim->GetGLId( (MOuint)m_FramePosition );
              glid = m_Config.GetGLId( moR(MOVIE_MOVIES), (MOuint)m_FramePosition );
              //MODebug2->Message("glid VCR:"+IntToStr(glid)+" pos:"+IntToStr(m_FramePosition) );
            }

          if (m_pMovie) {

            if ( (m_FramePosition+MO_MOVIE_EOF_TOLERANCE) >= m_FramesLength || m_pMovie->IsEOS() ) {

            ///stop current
              if (m_pMovie->IsEOS()) {
                MODebug2->Message( "Reached End");
                MODebug2->Message( "Reached EOS!! > STOPPING stream (NULL), must reload...");
                m_pMovie->Stop();
                m_pMovie->Pause();
                m_pMovie->Seek( 0 );

              } else if (!m_pMovie->IsPaused()) {
                MODebug2->Message( "Reached End at " + IntToStr(m_FramePosition));
                MODebug2->Message( "Pausing. Virtual Stop at " + IntToStr(m_FramePosition) );
                m_pMovie->Pause();
                //m_pMovie->Seek( 0 );
              }

              if (m_bLoop) {
                    MODebug2->Message( "Looping video.");
                    if (m_pMovie->IsPaused()) {
                      m_pMovie->Seek(0);
                    }

              }
            }

          }
		  }
		break;
    case MO_MOVIE_MODE_SCRIPT:
      //if (!moTimeManager::MoldeoTimer->Paused() && moTimeManager::MoldeoTimer->Started()) {
        if (m_pAnim) {
          ///Internally if FramePosition doesnt change GetFrame is not called... (for optimization)
          ///glid = m_pAnim->GetGLId( (MOuint)m_FramePosition );
          glid = m_Config.GetGLId( moR(MOVIE_MOVIES), (MOuint)m_FramePosition );
        }
      //}
      break;

    case MO_MOVIE_MODE_CYCLE:
      //if (m_pAnim) {
        //glid = m_pAnim->GetGLId( &m_EffectState.tempo );
        glid = m_Config.GetGLId( moR(MOVIE_MOVIES), &m_EffectState.tempo );
        //MODebug2->Message("glid CYCLE:"+IntToStr(glid)+" tempo:"+FloatToStr(m_EffectState.tempo.ang));
      //}
      break;

    case MO_MOVIE_MODE_VCR_PLAYLIST:
      {
        if (m_pAnim) {
          ///Internally if FramePosition doesnt change GetFrame is not called... (for optimization)
          ///glid = m_pAnim->GetGLId( (MOuint)m_FramePosition );
          glid = m_Config.GetGLId( moR(MOVIE_MOVIES), (MOuint)m_FramePosition );
        }
        if (m_pMovie) {
          if ( (m_FramePosition+MO_MOVIE_EOF_TOLERANCE) >= m_FramesLength || m_pMovie->IsEOS()) {

            int actual_index = m_Config.GetParam(moR(MOVIE_MOVIES)).GetIndexValue();
            ///stop current
            if (m_pMovie->IsEOS()) {
                MODebug2->Push( "Reached EOS!! > STOPPING stream (NULL), must reload...");
                //m_pMovie->Stop();

            } else {
              MODebug2->Push( "Pausing. Virtual Stop!");
              m_pMovie->Pause();
              //m_pMovie->Seek( 0 );
            }

            ///next clip...
            if (m_bLoop) {
                if ( m_Config.GetParam(moR(MOVIE_MOVIES)).GetValuesCount()<=1) {
                    MODebug2->Push( "Looping video (playlist has one video).");
                    if (m_pMovie->IsPaused()) {
                      m_pMovie->Seek(0);
                    }
                } else {
                  if ( m_Config.GetParam(moR(MOVIE_MOVIES)).GetIndexValue() == (m_Config.GetParam(moR(MOVIE_MOVIES)).GetValuesCount() -1) ) {
                      m_Config.GetParam(moR(MOVIE_MOVIES)).FirstValue();
                       MODebug2->Push( "Looping playlist");
                  } else {
                      m_Config.GetParam(moR(MOVIE_MOVIES)).NextValue();
                  }
                }
            } else {
              m_Config.GetParam(moR(MOVIE_MOVIES)).NextValue();
            }

            if ( actual_index != m_Config.GetParam(moR(MOVIE_MOVIES)).GetIndexValue() ) {
              MODebug2->Push( "Next movie");
              m_pNextTexture = m_Config[moR(MOVIE_MOVIES)][MO_SELECTED][0].Texture();
              if (m_pNextTexture) {
                if ( m_pNextTexture->GetType()==MO_TYPE_MOVIE ) {
                  m_pNextMovie = (moMovie*)m_pNextTexture;
                  m_pNextMovie->Seek(0);
                  m_UserPosition = 1;
                }
              }
            }

            MODebug2->Push( "EOS:" + IntToStr((int)m_pMovie->IsEOS()) );
            MODebug2->Push("glid VCRPLAYLIST:" + IntToStr(m_Config.GetParam(moR(MOVIE_MOVIES)).GetIndexValue()) + " FramePosition:" + IntToStr(m_FramePosition) );

          }
        }
      }
      break;

  };

  if ( glid<=0 ) {
    //glid = m_pTexture->GetGLId();
    glid = m_Config.GetGLId( moR(MOVIE_MOVIES), &m_EffectState.tempo );
  }
  return glid;
}

void
moEffectMovie::DrawDisplayInfo( MOfloat x, MOfloat y ) {

	moTextArray pInfo;
/*
    pInfo.Add( moText("TicksAux:") + IntToStr((int)m_TicksAux) );
    pInfo.Add( moText("Ticks:") + IntToStr((int)m_Ticks) );
    pInfo.Add( moText(" "));
	pInfo.Add( moText("Mode:") + IntToStr((int)moviemode) );
	pInfo.Add( moText("Loop:") + IntToStr((int)m_bLoop) );
    pInfo.Add( moText("State:") + IntToStr((int)m_PlayState) );
	pInfo.Add( moText("Speed:") + FloatToStr(m_PlaySpeed) );
	pInfo.Add( moText(" "));
	pInfo.Add( moText("Len:") + IntToStr(m_FramesLength) );
	pInfo.Add( moText("Fps:") + FloatToStr(m_FramesPerSecond) );
	pInfo.Add( moText(" "));

	*/
	moText moviestate = "";
	if (m_pMovie) {
    switch (m_pMovie->State()) {
      case MO_STREAMSTATE_PAUSED:
        moviestate = "paused";
        break;
      case MO_STREAMSTATE_PLAYING:
        moviestate = "playing";
        break;
      case MO_STREAMSTATE_STOPPED:
        moviestate = "stopped";
        break;
      case MO_STREAMSTATE_READY:
        moviestate = "ready";
        break;
      case MO_STREAMSTATE_UNKNOWN:
      default:
        moviestate = "none";
        break;
    }
}

	pInfo.Add( moText("Frame:") + IntToStr( m_FramePosition ) + moText("/") + IntToStr( m_FramesLength )
           + moText(" FPS:") + FloatToStr( m_FramesPerSecond )
           + moText(" State:") + moviestate);

  pInfo.Add( moVideoManager::FramesToTimecode( (MOulonglong)m_FramePosition, m_FramesPerSecond )
                    + moText("/")
                    + moVideoManager::FramesToTimecode( (MOulonglong)m_FramesLength, m_FramesPerSecond ) );
    /*if (Sound)
		if (m_FramesLength>0 && Sound->GetBufferSize()>0 )
			pInfo.Add( moText("FP S:") + IntToStr(Sound->GetActualSample()/ ( Sound->GetBufferSize() / m_FramesLength )) );
        */

        /*
	pInfo.Add( moText("P.F.:") + FloatToStr(m_FramePositionF));
	pInfo.Add( moText("P.Inc.:") + FloatToStr(m_FramePositionInc));
	pInfo.Add( moText(" "));
	pInfo.Add( moText("P.Aux.:") + IntToStr(m_FramePositionAux));
	pInfo.Add( moText("P.Aux.F.:") + FloatToStr(m_FramePositionFAux));
*/

	/*
	pInfo.Add( moText("shottime: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("endshottime: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("empT: ")+ m_tScriptDebug.Pop());
	pInfo.Add( moText("aftT: ")+ m_tScriptDebug.Pop());
	pInfo.Add( moText("nobs: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("ttime: ")+m_tScriptDebug.Pop());

	pInfo.Add( moText("RFIRST: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("PFIRST: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("RSHOT: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("PSHOT: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("RLAST: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("PLAST: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("GFINISH: ")+m_tScriptDebug.Pop());
	pInfo.Add( moText("LFINISH: ")+m_tScriptDebug.Pop());
*/
	m_pResourceManager->GetGuiMan()->DisplayInfoWindow( m_DisplayX , m_DisplayY, m_DisplayW, m_DisplayH, pInfo );

}

MOboolean moEffectMovie::Finish()
{
/*
    if ((moText)m_MovieScriptFN!=moText("")) {
      SelectScriptFunction("Finish");
      RunSelectedFunction();
      moScript::FinishScript();
    }
*/
    if (m_pMovie) {
        m_pMovie->Stop();
        m_pMovie = NULL;
    }

    m_pTrackerData = NULL;

    //Images.Finish();
    PreFinish();
	return true;
}

void moEffectMovie::Interaction( moIODeviceManager *IODeviceManager ) {

	moDeviceCode *temp;//,*aux;
	MOint did,cid,state,valor;//e,estaux,elaux;
	moText txt;
	moSoundManager* SM = m_pResourceManager->GetSoundMan();
	moSoundEffect*	EQ = SM->GetEffects()->Get(1);
	float lowgain;
	float lowcutoff;
	float fxvalue;

	int TrackerId = -1;

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
				//VCR COMMANDS
				case MO_MOVIE_STOP:
					VCRCommand( MO_MOVIE_VCR_STOP );
					MODebug2->Push("STOP");
					break;
				case MO_MOVIE_PLAY:
					VCRCommand( MO_MOVIE_VCR_PLAY );
					MODebug2->Push("PLAY");
					break;
				case MO_MOVIE_REVERSE:
					VCRCommand( MO_MOVIE_VCR_REVERSE );
					MODebug2->Push("REVERSE");
					break;
				case MO_MOVIE_FW:
					VCRCommand( MO_MOVIE_VCR_FW );
					MODebug2->Push("FORWARD");
					break;
				case MO_MOVIE_RW:
					VCRCommand( MO_MOVIE_VCR_RW );
					MODebug2->Push("REWIND");
					break;
				case MO_MOVIE_FF:
					VCRCommand( MO_MOVIE_VCR_FF );
					MODebug2->Push("FAST FORWARD");
					break;
				case MO_MOVIE_FR:
					VCRCommand( MO_MOVIE_VCR_FR );
					MODebug2->Push("FAST REWIND");
					break;
				case MO_MOVIE_SPEED:
					VCRCommand( MO_MOVIE_VCR_SPEED );
					MODebug2->Push("PLAY AT SPEED");
					break;
				case MO_MOVIE_PAUSE:
					VCRCommand( MO_MOVIE_VCR_PAUSE );
					MODebug2->Push("PAUSE");
					break;
				case MO_MOVIE_SEEK:
					VCRCommand( MO_MOVIE_VCR_SEEK );
					MODebug2->Push("SEEK");
					break;
				case MO_MOVIE_PREVFRAME:
					VCRCommand( MO_MOVIE_VCR_PREVFRAME );
					MODebug2->Push("PREVFRAME");
					break;
				case MO_MOVIE_NEXTFRAME:
					VCRCommand( MO_MOVIE_VCR_NEXTFRAME );
					MODebug2->Push("NEXTFRAME");
					break;
				case MO_MOVIE_LOOP:
					this->m_bLoop = !this->m_bLoop;
					if (m_bLoop) MODebug2->Push(moText("Looped"));
					else MODebug2->Push(moText("Unlooped"));
					break;
				case MO_MOVIE_GAIN_UP:
					if (m_pSound) {
						m_pSound->SetVolume(+0.1);
						MODebug2->Push(moText("GainUp: ")+FloatToStr(m_pSound->GetVolume()));
					}
					break;
				case MO_MOVIE_GAIN_DOWN:
					if (m_pSound) {
						m_pSound->SetVolume(-0.1);
						MODebug2->Push(moText("GainDown: ")+FloatToStr(m_pSound->GetVolume()));
					}
					break;
//1.0 AL_EQUALIZER_LOW_GAIN [0.126, 7.943] 1.0

                #ifdef MO_WIN32

                //no hay ecualizacion en linux????

				case MO_MOVIE_EQ_LOW_GAIN_UP:
					/*
					EQ->GetParameterF( AL_EQUALIZER_LOW_GAIN , &lowgain );
					lowgain+= 0.1;
					if (0.126>lowgain) lowgain = 0.126;
					else if (lowgain>7.9) lowgain = 7.9;
					EQ->SetParameterF( AL_EQUALIZER_LOW_GAIN, lowgain );
					MODebug2->Push( moText("LowGainUp: ")+FloatToStr(lowgain) );
					*/
					break;
				case MO_MOVIE_EQ_LOW_GAIN_DOWN:
                    /*
					EQ->GetParameterF( AL_EQUALIZER_LOW_GAIN , &lowgain );
					lowgain-= 0.1;
					if (0.126>lowgain) lowgain = 0.126;
					else if (lowgain>7.9) lowgain = 7.9;
					EQ->SetParameterF( AL_EQUALIZER_LOW_GAIN, lowgain );
					MODebug2->Push( moText("LowGainDown: ")+FloatToStr(lowgain) );
					*/
					break;
//1.0 AL_EQUALIZER_LOW_CUTOFF Hz [50.0, 800.0] 200.0
				case MO_MOVIE_EQ_LOW_CUTOFF_UP:
                    /*
					EQ->GetParameterF( AL_EQUALIZER_LOW_CUTOFF , &lowcutoff );
					lowcutoff+= 10.0;
					if (50.0>lowcutoff) lowcutoff = 50.0;
					else if (lowcutoff>800.0) lowcutoff = 800.0;
					EQ->SetParameterF( AL_EQUALIZER_LOW_CUTOFF, lowcutoff );
					MODebug2->Push( moText("LowCutoffUp: ")+FloatToStr(lowcutoff) );
					*/
					break;
				case MO_MOVIE_EQ_LOW_CUTOFF_DOWN:
                    /*
					EQ->GetParameterF( AL_EQUALIZER_LOW_CUTOFF , &lowcutoff );
					lowcutoff-= 10.0;
					if (50.0>lowcutoff) lowcutoff = 50.0;
					else if (lowcutoff>800.0) lowcutoff = 800.0;
					EQ->SetParameterF( AL_EQUALIZER_LOW_CUTOFF, lowgain );
					MODebug2->Push( moText("LowCutoffDown: ")+FloatToStr(lowcutoff) );
					*/
					break;
//1.0 AL_EQUALIZER_MID1_GAIN [0.126, 7.943] 1.0
				case MO_MOVIE_EQ_MID1_GAIN_UP:
                    /*
					EQ->GetParameterF( AL_EQUALIZER_MID1_GAIN , &fxvalue );
					fxvalue+= 10;
					if (0.126>fxvalue) fxvalue = 0.126;
					else if (fxvalue>7.943) fxvalue = 7.943;
					EQ->SetParameterF( AL_EQUALIZER_MID1_GAIN, fxvalue );
					MODebug2->Push( moText("Mid1GainUp: ")+FloatToStr(fxvalue) );
					*/
					break;
				case MO_MOVIE_EQ_MID1_GAIN_DOWN:
                    /*
					EQ->GetParameterF( AL_EQUALIZER_MID1_GAIN , &fxvalue );
					fxvalue-= 0.1;
					if (0.126>fxvalue) fxvalue = 0.126;
					else if (fxvalue>7.943) fxvalue = 7.943;
					EQ->SetParameterF( AL_EQUALIZER_MID1_GAIN, fxvalue );
					MODebug2->Push(  moText("Mid1GainDown: ")+ FloatToStr(fxvalue) );
					*/
					break;
//1.0 AL_EQUALIZER_MID1_CENTER Hz [200.0, 3000.0] 500.0
				case MO_MOVIE_EQ_MID1_CENTER_UP:
                    /*
					EQ->GetParameterF( AL_EQUALIZER_MID1_CENTER , &fxvalue );
					fxvalue+= 20;
					if (200>fxvalue) fxvalue = 200;
					else if (fxvalue>3000) fxvalue = 3000;
					EQ->SetParameterF( AL_EQUALIZER_MID1_CENTER, fxvalue );
					MODebug2->Push( moText("Mid1CenterUp: ")+ FloatToStr(fxvalue) );
					*/
					break;
				case MO_MOVIE_EQ_MID1_CENTER_DOWN:
                    /*
					EQ->GetParameterF( AL_EQUALIZER_MID1_CENTER , &fxvalue );
					fxvalue-= 20;
					if (200>fxvalue) fxvalue = 200;
					else if (fxvalue>3000) fxvalue = 3000;
					EQ->SetParameterF( AL_EQUALIZER_MID1_CENTER, fxvalue );
					MODebug2->Push( moText("Mid1CenterDown: ")+ FloatToStr(fxvalue) );
					*/
					break;
//1.0 AL_EQUALIZER_MID1_WIDTH [0.01, 1.0] 1.0
				case MO_MOVIE_EQ_MID1_WIDTH_UP:
                    /*
					EQ->GetParameterF( AL_EQUALIZER_MID1_WIDTH , &fxvalue );
					fxvalue+= 0.1;
					if (0.01>fxvalue) fxvalue = 0.01;
					else if (fxvalue>1.0) fxvalue = 1.0;
					EQ->SetParameterF( AL_EQUALIZER_MID1_WIDTH, fxvalue );
					MODebug2->Push( moText("Mid1WidthUp: ")+FloatToStr(fxvalue) );
					*/
					break;
				case MO_MOVIE_EQ_MID1_WIDTH_DOWN:
                    /*
					EQ->GetParameterF( AL_EQUALIZER_MID1_WIDTH , &fxvalue );
					fxvalue-= 0.1;
					if (0.01>fxvalue) fxvalue = 0.01;
					else if (fxvalue>1.0) fxvalue = 1.0;
					EQ->SetParameterF( AL_EQUALIZER_MID1_WIDTH, fxvalue );
					MODebug2->Push( moText("Mid1WidthDown: ")+FloatToStr(fxvalue) );
					*/
					break;
//1.0 AL_EQUALIZER_MID2_GAIN [0.126, 7.943] 1.0
				case MO_MOVIE_EQ_MID2_GAIN_UP:
                    /*
					EQ->GetParameterF( AL_EQUALIZER_MID2_GAIN , &fxvalue );
					fxvalue+= 0.1;
					if (0.126>fxvalue) fxvalue = 0.126;
					else if (fxvalue>7.943) fxvalue = 7.943;
					EQ->SetParameterF( AL_EQUALIZER_MID2_GAIN, fxvalue );
					MODebug2->Push( moText("Mid2GainUp: ")+FloatToStr(fxvalue) );
					*/
					break;
				case MO_MOVIE_EQ_MID2_GAIN_DOWN:
				/*
					EQ->GetParameterF( AL_EQUALIZER_MID2_GAIN , &fxvalue );
					fxvalue-= 0.1;
					if (0.126>fxvalue) fxvalue = 0.126;
					else if (fxvalue>7.943) fxvalue = 7.943;
					EQ->SetParameterF( AL_EQUALIZER_MID2_GAIN, fxvalue );
					MODebug2->Push( moText("Mid2GainDown: ")+FloatToStr(fxvalue) );
					*/
					break;
//1.0 AL_EQUALIZER_MID2_CENTER [1000.0, 8000.0] 3000.0
				case MO_MOVIE_EQ_MID2_CENTER_UP:
                    /*
					EQ->GetParameterF( AL_EQUALIZER_MID2_CENTER , &fxvalue );
					fxvalue+= 50;
					if (1000.0>fxvalue) fxvalue = 1000.0;
					else if (fxvalue>8000.0) fxvalue = 8000.0;
					EQ->SetParameterF( AL_EQUALIZER_MID2_CENTER, fxvalue );
					MODebug2->Push( moText("Mid2CenterUp: ")+FloatToStr(fxvalue) );
					*/
					break;
				case MO_MOVIE_EQ_MID2_CENTER_DOWN:
                    /*
					EQ->GetParameterF( AL_EQUALIZER_MID2_CENTER , &fxvalue );
					fxvalue-= 50;
					if (1000.0>fxvalue) fxvalue = 1000.0;
					else if (fxvalue>8000.0) fxvalue = 8000.0;
					EQ->SetParameterF( AL_EQUALIZER_MID2_CENTER, fxvalue );
					MODebug2->Push( moText("Mid2CenterDown: ")+FloatToStr(fxvalue) );
					*/
					break;
//1.0 AL_EQUALIZER_MID2_WIDTH [0.01, 1.0] 1.0
				case MO_MOVIE_EQ_MID2_WIDTH_UP:
                    /*
					EQ->GetParameterF( AL_EQUALIZER_MID2_WIDTH , &fxvalue );
					fxvalue+= 0.1;
					if (0.01>fxvalue) fxvalue = 0.01;
					else if (fxvalue>1.0) fxvalue = 1.0;
					EQ->SetParameterF( AL_EQUALIZER_MID2_WIDTH, fxvalue );
					MODebug2->Push( moText("Mid2WidthUp: ")+FloatToStr(fxvalue) );
					*/
					break;
				case MO_MOVIE_EQ_MID2_WIDTH_DOWN:
                    /*
					EQ->GetParameterF( AL_EQUALIZER_MID2_WIDTH , &fxvalue );
					fxvalue-= 0.1;
					if (0.01>fxvalue) fxvalue = 0.01;
					else if (fxvalue>1.0) fxvalue = 1.0;
					EQ->SetParameterF( AL_EQUALIZER_MID2_WIDTH, fxvalue );
					MODebug2->Push( moText("Mid2WidthDown: ")+FloatToStr(fxvalue) );
					*/
					break;
//1.0 AL_EQUALIZER_HIGH_GAIN [0.126, 7.943] 1.0
				case MO_MOVIE_EQ_HIGH_GAIN_UP:
                    /*
					EQ->GetParameterF( AL_EQUALIZER_HIGH_GAIN , &fxvalue );
					fxvalue+= 0.1;
					if (0.126>fxvalue) fxvalue = 0.126;
					else if (fxvalue>7.943) fxvalue = 7.943;
					EQ->SetParameterF( AL_EQUALIZER_HIGH_GAIN, fxvalue );
					MODebug2->Push( moText("HighGainUp: ")+FloatToStr(fxvalue) );
					*/
					break;
				case MO_MOVIE_EQ_HIGH_GAIN_DOWN:
                    /*
					EQ->GetParameterF( AL_EQUALIZER_HIGH_GAIN , &fxvalue );
					fxvalue-= 0.1;
					if (0.126>fxvalue) fxvalue = 0.126;
					else if (fxvalue>7.943) fxvalue = 7.943;
					EQ->SetParameterF( AL_EQUALIZER_HIGH_GAIN, fxvalue );
					MODebug2->Push(  moText("HighGainDown: ")+FloatToStr(fxvalue) );
					*/
					break;
//1.0 AL_EQUALIZER_HIGH_CUTOFF Hz [4000.0, 16000.0] 6000.0
				case MO_MOVIE_EQ_HIGH_CUTOFF_UP:
                    /*
					EQ->GetParameterF( AL_EQUALIZER_HIGH_CUTOFF , &fxvalue );
					fxvalue+= 100;
					if (4000.0>fxvalue) fxvalue = 4000.0;
					else if (fxvalue>16000.0) fxvalue = 16000.0;
					EQ->SetParameterF( AL_EQUALIZER_HIGH_CUTOFF, fxvalue );
					MODebug2->Push( moText("HighCutoffUp: ")+FloatToStr(fxvalue) );
					*/
					break;
				case MO_MOVIE_EQ_HIGH_CUTOFF_DOWN:
                    /*
					EQ->GetParameterF( AL_EQUALIZER_HIGH_CUTOFF , &fxvalue );
					fxvalue-= 100;
					if (4000.0>fxvalue) fxvalue = 4000.0;
					else if (fxvalue>16000.0) fxvalue = 16000.0;
					EQ->SetParameterF( AL_EQUALIZER_HIGH_CUTOFF, fxvalue );
					MODebug2->Push( moText("HighCutoffDown: ")+FloatToStr(fxvalue) );
					*/
					break;

                #endif

			}
		temp = temp->next;
		}
	}

}


void
moEffectMovie::Activate() {
  MODebug2->Message("moEffectMovie::Activate");

  moMoldeoObject::Activate();

  if ( Activated() ) {
      if (m_pMovie && m_pMovie->IsPaused()) {
          m_pMovie->Play();
      }
  }

}

void
moEffectMovie::Deactivate() {
  MODebug2->Message("moEffectMovie::Deactivate");

  moMoldeoObject::Deactivate();

  if ( !Activated() ) {
      if (m_pMovie) {
          m_pMovie->Pause();
      }
  }

}

void
moEffectMovie::Update( moEventList *Events ) {

    moEvent *tmp;
    moEvent *actual;

    actual = Events->First;

 while(actual!=NULL) {

    float pr;
    float pos;
    int posi;

      switch(actual->deviceid) {
        case MO_IODEVICE_MOUSE:

          switch(actual->devicecode) {
            case SDL_MOUSEMOTION:
              cout << "moMovie receiving mouse motion: x: " << actual->reservedvalue2 << " y:" << actual->reservedvalue3 << endl;
              /** MAKE THE CONTROLS APPEAR*/
              m_CursorX = actual->reservedvalue2;
              m_CursorY = actual->reservedvalue3;

              break;
            case SDL_MOUSEBUTTONDOWN:
              cout << "moMovie receiving mouse button down: " << actual->reservedvalue0 << " x:" << actual->reservedvalue1 << " y:" << actual->reservedvalue2 << endl;
              //m_pWebView->InjectMouseMove( actual->reservedvalue1, actual->reservedvalue2 );
              /** CHECK WHERE IT HITS > SEEEK*/
              m_CursorX = actual->reservedvalue1;
              m_CursorY = actual->reservedvalue2;


              switch(actual->reservedvalue0) {
                case SDL_BUTTON_LEFT:
                  //m_pWebView->InjectMouseDown(Awesomium::kMouseButton_Left);
                  //this->Disable();
                  //this->TurnOff();

                  break;
                case SDL_BUTTON_MIDDLE:
                  //m_pWebView->InjectMouseDown(Awesomium::kMouseButton_Middle);
                  break;
                case SDL_BUTTON_RIGHT:
                  //m_pWebView->InjectMouseDown(Awesomium::kMouseButton_Right);
                  break;
              }
              break;
            case SDL_MOUSEBUTTONUP:
              cout << "moMovie receiving mouse button up: " << actual->reservedvalue0 << " x:" << actual->reservedvalue1 << " y:" << actual->reservedvalue2 << endl;
              //m_pWebView->InjectMouseMove( actual->reservedvalue1, actual->reservedvalue2 );

              m_CursorX = actual->reservedvalue1;
              m_CursorY = actual->reservedvalue2;


              switch(actual->reservedvalue0) {
                case SDL_BUTTON_LEFT:
                 // m_pWebView->InjectMouseUp(Awesomium::kMouseButton_Left);

                   /** SHOW CONTROLS*/
                   if ( m_CursorX>0 && m_CursorX<1280
                       && m_CursorY<720) {
                        showmoviedata = !showmoviedata;

                    }

                   /** PLAY/PAUSE*/
                   if ( m_CursorX>0 && m_CursorX<80
                       && m_CursorY>720 && m_CursorY<800) {
                      if (m_pMovie) {
                          if (m_pMovie->State()!=MO_STREAMSTATE_PAUSED)
                            this->Pause();
                          else
                            this->Play();
                      }
                  }

                   /** FULLSCREEN BUTTON TOGGLE*/
                   if ( m_CursorX>1200 && m_CursorX<1280
                       && m_CursorY>720 && m_CursorY<800 ) {
                        this->Disable();
                        this->TurnOff();
                    }

                    /** SEEK BAR*/
                    if ( m_CursorX>100 && m_CursorX<1180
                       && m_CursorY>720 && m_CursorY<800 ) {
                      pr = 1.0*(double)m_CursorX / 1280.0;
                      pos = pr*(double)m_FramesLength;
                      posi = (int) fabs(pos);
                      if (m_pMovie) m_pMovie->Seek( posi );
                  }
                  break;
                case SDL_BUTTON_MIDDLE:
                  //m_pWebView->InjectMouseUp(Awesomium::kMouseButton_Middle);
                  break;
                case SDL_BUTTON_RIGHT:
                  //m_pWebView->InjectMouseUp(Awesomium::kMouseButton_Right);
                  break;
              }
              break;
          }
          break;

        case MO_IODEVICE_KEYBOARD:

          break;
      }

      tmp = actual;
      actual = tmp->next;

    }



	//get the pointer from the Moldeo Object sending it...
	moMoldeoObject::Update(Events);

}

//CUSTOM FUNCTIONS


//play en funcion de play speed:
void moEffectMovie::VCRPlaySpeed() {

	//if (m_pAnim)
	//if (!m_pAnim->IsInterpolating()) {


		m_FramePositionAux = m_FramePosition;
		m_FramePositionInc = (double)( m_Ticks - m_TicksAux ) * m_PlaySpeed * m_FramesPerSecond / ((double)1000.0) ;
		m_FramePositionF+= m_FramePositionInc;
		if ( m_FramePositionF < 0.0 ) m_FramePositionF = 0.0;
		m_FramePosition = (long) m_FramePositionF;
		if ( m_FramePosition >= m_FramesLength ) m_FramePosition = (m_FramesLength-1);
	//}

	if ( m_pSound && m_FramesLength>0 ) {
		MOlong bytesperframe = m_pSound->GetBufferSize() / m_FramesLength;
		/*
   MODebug2->Message( "moEffectMovie::VCRPlaySpeed > "
                      + moText("playsound sample > m_bPlayingSound:") + IntToStr((int)m_bPlayingSound)
                      + " m_PlaySpeed: " + FloatToStr(m_PlaySpeed)
                      + " Name: " + m_pSound->GetName()
                      + " bytesperframe:" + IntToStr(bytesperframe)
                      + " m_FramePosition:" + IntToStr(m_FramePosition)
                      + " buffersize:" + IntToStr(m_pSound->GetBufferSize()));*/

		if ( m_PlaySpeed == 1.0 && !m_bPlayingSound ) {
        /*MODebug2->Message(
                          "moEffectMovie::VCRPlaySpeed > m_PlaySpeed: " + FloatToStr(m_PlaySpeed)+
                          " bufferposition:" + IntToStr(m_FramePosition * bytesperframe)+
                          " GetBufferSize:" + IntToStr( m_pSound->GetBufferSize() )
                          );*/
			if ((m_FramePosition * bytesperframe) < m_pSound->GetBufferSize() ) {
			  //MODebug2->Message( "moEffectMovie::VCRPlaySpeed > m_pSound->PlaySample");
				m_pSound->PlaySample( (MOuint) m_FramePosition * bytesperframe );
				m_bPlayingSound = true;
			}
		} else if (m_PlaySpeed==0.0 || m_FramePosition==0) {
      //MODebug2->Message( "moEffectMovie::VCRPlaySpeed > m_pSound->Stop");
			//m_pSound->Stop();
			m_bPlayingSound = false;
		}

		//RESYNC SOUND WITH VIDEO
		if (m_bPlayingSound) {

			if (bytesperframe>0 && m_FramesLength>0) {
				MOint async = (m_pSound->GetActualSample() / bytesperframe )  - m_FramePosition;

				if ( fabs((double)async) > 10.0 ) {

					//m_pSound->PlaySample( m_FramePosition * bytesperframe );

				}
			}
		}

	}

}

void moEffectMovie::SpeedRegulation( MOfloat p_fMinSpeed, MOfloat p_fMaxSpeed ) {

	MOfloat speedF;

	MOlong deltaticks;
	MOfloat deltaticksF;

	deltaticks = m_TicksAux - m_Ticks;
	deltaticksF = deltaticks;

	speedF = (double) 1000.0 * (double) (m_FramePositionFAux - m_FramePositionF) / (deltaticksF * m_FramesPerSecond);

	//MODebug2->Push(FloatToStr(speedF));

	if ( speedF > p_fMaxSpeed ) {
		m_PlaySpeed = p_fMaxSpeed;
		VCRPlaySpeed();
	}

	if ( speedF < p_fMinSpeed ) {
		m_PlaySpeed = p_fMinSpeed;
		VCRPlaySpeed();
	}

}

void moEffectMovie::VCRCommand( moEffectMovieVCRCommand p_Command, MOint p_iValue, MOfloat p_fValue ) {
	switch(p_Command) {
		case MO_MOVIE_VCR_STOP:
			m_PlayState = MO_MOVIE_PLAYSTATE_STOPPED;
			m_PlaySpeed = 0.0;
			if (m_pSound) m_pSound->Stop();
			if (m_pSound) m_pSound->Stop();
			//m_SoundSystem.Stop();
			m_bPlayingSound = false;
			break;

		case MO_MOVIE_VCR_PLAY:
			m_PlayState = MO_MOVIE_PLAYSTATE_PLAYING;
			m_PlaySpeed = 1.0;
			//m_SoundSystem.Play();
			break;

		case MO_MOVIE_VCR_PAUSE:
			m_PlaySpeed = 0.0;
			m_PlayState = MO_MOVIE_PLAYSTATE_PAUSED;
			if (m_pSound) m_pSound->Pause();
			//m_SoundSystem.Pause();
			m_bPlayingSound = false;
			break;

		case MO_MOVIE_VCR_REVERSE:
			m_PlaySpeed = -1.0;
			m_PlayState = MO_MOVIE_PLAYSTATE_REVERSE;
			break;

		case MO_MOVIE_VCR_FW:
			m_PlaySpeed = 2.0;
			m_PlayState = MO_MOVIE_PLAYSTATE_SPEED;
			break;

		case MO_MOVIE_VCR_RW:
			m_PlaySpeed = -2.0;
			m_PlayState = MO_MOVIE_PLAYSTATE_SPEED;
			break;

		case MO_MOVIE_VCR_FF:
			m_PlaySpeed = 4.0;
			m_PlayState = MO_MOVIE_PLAYSTATE_SPEED;
			break;
		case MO_MOVIE_VCR_FR:
			m_PlaySpeed = -4.0;
			m_PlayState = MO_MOVIE_PLAYSTATE_SPEED;
			break;
		case MO_MOVIE_VCR_SEEK:
			//nada por ahora
			m_PlayState = MO_MOVIE_PLAYSTATE_SEEKING;
			m_FramePosition = p_iValue;
			break;
		case MO_MOVIE_VCR_SPEED:
			m_PlaySpeed = p_fValue;
			m_PlayState = MO_MOVIE_PLAYSTATE_SPEED;
			break;
		case MO_MOVIE_VCR_PREVFRAME:
			if ( (m_FramePosition > 0) && (m_FramePosition < m_FramesLength) ) {
					m_FramePosition = m_FramePosition - 1;
					m_FramePositionF = m_FramePosition;
			}
			m_PlayState = MO_MOVIE_PLAYSTATE_PAUSED;
			break;
		case MO_MOVIE_VCR_NEXTFRAME:
			if ( (m_FramePosition >= 0) && (m_FramePosition < (m_FramesLength-1)) ) {
					m_FramePosition = m_FramePosition + 1;
					m_FramePositionF = m_FramePosition;
			}
			m_PlayState = MO_MOVIE_PLAYSTATE_PAUSED;
			break;
		case MO_MOVIE_VCR_LOOP:
			m_bLoop = !m_bLoop;
			break;
	}

}


int moEffectMovie::ScriptCalling(moLuaVirtualMachine& vm, int iFunctionNumber)
{
/*
    for(int i=0; i<m_iMethodBaseAncestors; i++ ) {
       MODebug2->Message( moText("moEffectMovie::ScriptCalling > baseancestor: ")+IntToStr(i)+moText(" base: ") + IntToStr(m_MethodBases[i]));
    }
*/
/*    MODebug2->Message( moText("Called moEffectMovie::ScriptCalling, iFunctionNumber") + IntToStr(iFunctionNumber)
                      +moText(" m_iMethodBase: ") + IntToStr(m_iMethodBase) );

   m_iMethodBase = m_MethodBases[m_iMethodBaseAncestors-1];
   m_iMethodBaseIterator = m_iMethodBaseAncestors-1;

    MODebug2->Message( moText("m_iMethodBaseAncestors: ") + IntToStr( m_iMethodBaseAncestors ) );
    MODebug2->Message( moText("m_iMethodBaseIterator: ") + IntToStr( m_iMethodBaseIterator ) );
    MODebug2->Message( moText("m_iMethodBase: ") + IntToStr( m_iMethodBase ) );
*/
    //MODebug2->Message( moText("m_iMethodBase: ") + IntToStr( m_iMethodBase ) );
    //MODebug2->Message( moText("iFunctionNumber: ") + IntToStr( iFunctionNumber ) );
    m_iMethodBase = 47;
    switch (iFunctionNumber - m_iMethodBase)
    {

        case 0:
            ResetScriptCalling();
            return GetFeature(vm); //0

        case 1:
            ResetScriptCalling();
            return SpeedRegulation(vm);//1
        case 2:
            ResetScriptCalling();
            return VCRStop(vm);//2
        case 3:
            ResetScriptCalling();
            return VCRPlay(vm);//3
        case 4:
            ResetScriptCalling();
            return VCRPause(vm);//4
        case 5:
            ResetScriptCalling();
            return VCRReverse(vm);//5
        case 6:
            ResetScriptCalling();
            return VCRForward(vm);//6
        case 7:
            ResetScriptCalling();
            return VCRRewind(vm);//7
        case 8:
            ResetScriptCalling();
            return VCRFForward(vm);//8
        case 9:
            ResetScriptCalling();
            return VCRFRewind(vm);//9
        case 10:
            ResetScriptCalling();
            return VCRSeek(vm);//10
        case 11:
            ResetScriptCalling();
            return VCRSpeed(vm);//11
        case 12:
            ResetScriptCalling();
            return VCRPrevFrame(vm);//12
        case 13:
            ResetScriptCalling();
            return VCRNextFrame(vm);//13
        case 14:
            ResetScriptCalling();
            return VCRLoop(vm);//14
        case 15:
            ResetScriptCalling();
            return VCRPlaySpeed(vm);//15
        case 16:
            ResetScriptCalling();
            return GetFramePosition(vm);//16
        case 17:
            ResetScriptCalling();
            return GetFramePositionF(vm);//17
        case 18:
            ResetScriptCalling();
            return GetPlaySpeed(vm);//18
        case 19:
            ResetScriptCalling();
            return SetFramePosition(vm);//19
        case 20:
            ResetScriptCalling();
            return SetFramePositionF(vm);//20
        case 21:
            ResetScriptCalling();
            return SetPlaySpeed(vm);//21
        case 22:
            ResetScriptCalling();
            return DrawLine(vm);//22
        default:
            //MODebug2->Message( moText("Called moEffectMovie::ScriptCalling, NextScriptCalling, from m_iMethodBase: ") + IntToStr(m_iMethodBase) );
            NextScriptCalling();
            //m_iMethodBase = m_MethodBases[m_iMethodBaseIterator-1];
            //m_iMethodBaseIterator = m_iMethodBaseIterator - 1;
            //MODebug2->Message( moText("Called moEffectMovie::ScriptCalling, NextScriptCalling, to m_iMethodBase: ") + IntToStr(m_iMethodBase) );
            return moEffect::ScriptCalling( vm, iFunctionNumber );
            //return moMoldeoObject::ScriptCalling( vm, iFunctionNumber );

	}
	return 0;

}

void moEffectMovie::HandleReturns(moLuaVirtualMachine& vm, const char *strFunc)
{
    if (strcmp (strFunc, "Run") == 0)
    {
        lua_State *state = (lua_State *) vm;
        MOint script_result = (MOint) lua_tonumber (state, 1);
		if (script_result != 0)
			MODebug2->Push(moText("Script returned error code: ") + (moText)IntToStr(script_result));
    }
}

void moEffectMovie::RegisterFunctions()
{
    moEffect::RegisterFunctions();
    //moMoldeoObject::RegisterFunctions();

    RegisterBaseFunction("GetFeature"); //0

    RegisterFunction("SpeedRegulation"); //1

    RegisterFunction("VCRStop"); //2
    RegisterFunction("VCRPlay"); //3
    RegisterFunction("VCRPause"); //4
    RegisterFunction("VCRReverse"); //5
    RegisterFunction("VCRForward"); //6
    RegisterFunction("VCRRewind"); //7
    RegisterFunction("VCRFForward"); //8
    RegisterFunction("VCRFRewind"); //9
    RegisterFunction("VCRSeek"); //10
    RegisterFunction("VCRSpeed"); //11
    RegisterFunction("VCRPrevFrame"); //12
    RegisterFunction("VCRNextFrame"); //13
    RegisterFunction("VCRLoop"); //14
    RegisterFunction("VCRPlaySpeed"); //15

    RegisterFunction("GetFramePosition"); //16
    RegisterFunction("GetFramePositionF"); //17
    RegisterFunction("GetPlaySpeed"); //18

    RegisterFunction("SetFramePosition"); //19
    RegisterFunction("SetFramePositionF"); //20
    RegisterFunction("SetPlaySpeed"); //21

    RegisterFunction("DrawLine"); //22

    ResetScriptCalling();

}

void moEffectMovie::ScriptExeRun() {


  moText cs;
  cs = m_Config.Text( __iscript );

  ///Reinicializamos el script en caso de haber cambiado
	if ((moText)m_Script!=cs && IsInitialized()) {

        m_Script = cs;
        moText fullscript = DataMan()->GetDataPath()+ moSlash + (moText)m_Script;

        if (moFileManager::FileExists(fullscript)) {

          MODebug2->Message( GetLabelName() +  moText(" script loading : ") + (moText)fullscript );

          if ( CompileFile(fullscript) ) {

              MODebug2->Message( GetLabelName() + moText(" script loaded and compiled!! for ") + (moText)fullscript );

              ///Reinicializamos el script
              ScriptExeInit();
              //SelectScriptFunction( "Init" );
              /**TODO: revisar uso de offset, para multipantallas
              moText toffset=moText("");

              toffset = m_Config[moR(CONSOLE_SCRIPT)][MO_SELECTED][1].Text();
              if (toffset!=moText("")) {
                  m_ScriptTimecodeOffset = atoi( toffset );
              } else {
                  m_ScriptTimecodeOffset = 0;
              }
              AddFunctionParam( (int)m_ScriptTimecodeOffset );
              */
              //RunSelectedFunction();

          } else MODebug2->Error( moText("Couldn't compile lua script ") + (moText)fullscript + " config:"+GetConfigName()+" label: "+GetLabelName() );
        } else MODebug2->Message("Script file not present. " + (moText)fullscript + " config: "+GetConfigName()+" label:"+GetLabelName() );
	}


  ///Si tenemos un script inicializado... corremos la funcion Run()

    if (moScript::IsInitialized()) {
        if (ScriptHasFunction("Run")) {
            SelectScriptFunction("Run");
            int nfeat = 0;
            int tw = 0;
            int th = 0;
            if ( m_pTrackerData ) {
             if (m_pTrackerData) {
                nfeat = m_pTrackerData->GetFeaturesCount();
                tw = m_pTrackerData->GetVideoFormat().m_Width;
                th = m_pTrackerData->GetVideoFormat().m_Height;
             }
            }
            AddFunctionParam( (int) nfeat );
            AddFunctionParam( (int) tw );
            AddFunctionParam( (int) th );
            RunSelectedFunction();
            //MODebug2->Message( moText("moEffectMovie::ScriptExeRun called! for ") + m_Script);
        } else {
          if (m_Script.Trim().Length()>0) {
            MODebug2->Error( moText("moEffectMovie::ScriptExeRun > no function RUN! for ") + m_Script);
          }
        }
    }
}

void moEffectMovie::ScriptExeDraw() {
    if (moScript::IsInitialized()) {
        if (ScriptHasFunction("Draw")) {
            SelectScriptFunction("Draw");
            int nfeat = 0;
            int tw = 0;
            int th = 0;
            if ( m_pTrackerData ) {
             if (m_pTrackerData) {
                nfeat = m_pTrackerData->GetFeaturesCount();
                tw = m_pTrackerData->GetVideoFormat().m_Width;
                th = m_pTrackerData->GetVideoFormat().m_Height;
             }
            }
            AddFunctionParam( (int) nfeat );
            AddFunctionParam( (int) tw );
            AddFunctionParam( (int) th );
            RunSelectedFunction();
        }
    }
}

void moEffectMovie::ScriptExeInit() {
    MODebug2->Message("moEffectMovie::ScriptExeInit !");
    if (moScript::IsInitialized()) {
        if (ScriptHasFunction("Init")) {
            SelectScriptFunction("Init");
            int rw = 0;
            int rh = 0;
            m_FramesPerSecond = 25;
            m_FramesLength = 1059;
            AddFunctionParam( (int) m_FramesPerSecond );
            AddFunctionParam( (int) m_FramesLength );
            rw = m_pResourceManager->GetRenderMan()->RenderWidth();
            rh = m_pResourceManager->GetRenderMan()->RenderHeight();
            AddFunctionParam( (int) rw );
            AddFunctionParam( (int) rh );
            //MODebug2->Message("moEffectMovie::ScriptExeInit Running function!.");
            bool res = RunSelectedFunction();
            //MODebug2->Message( moText("RunSelectedFunction result(0:bad, 1:ok):")+ IntToStr((int)res));
        } else {
          if (m_Script!="")
            MODebug2->Error("moEffectMovie::ScriptExeInit FAILED! no Init function in script.");
        }
    }
}
/*
void moEffectMovie::ScriptExeFinish() {
    if (moScript::IsInitialized()) {
        if (ScriptHasFunction("Finish")) {
            SelectScriptFunction("Finish");
            RunSelectedFunction();
        }
    }
}*/

void moEffectMovie::ScriptExeUpdate() {
    if (moScript::IsInitialized()) {
        if (ScriptHasFunction("Update")) {
            SelectScriptFunction("Update");
            RunSelectedFunction();
        }
    }
}


int moEffectMovie::GetFeature(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    float x, y, v;

    if (m_pTrackerData) {
		x = m_pTrackerData->GetFeature(i)->x;
		y = m_pTrackerData->GetFeature(i)->y;
		v = m_pTrackerData->GetFeature(i)->val;
	}
    /*
	if (m_pTrackerData) {
		x = m_pTrackerData->m_FeatureList->feature[i]->x;
		y = m_pTrackerData->m_FeatureList->feature[i]->y;
		v = m_pTrackerData->m_FeatureList->feature[i]->val;
	} else if (m_pTrackerGpuData) {
		x = m_pTrackerGpuData->m_FeatureList->_list[i]->x;
		y = m_pTrackerGpuData->m_FeatureList->_list[i]->y;
		v = m_pTrackerGpuData->m_FeatureList->_list[i]->valid;
	}*/

	lua_pushnumber(state, (lua_Number)x);
	lua_pushnumber(state, (lua_Number)y);
	lua_pushnumber(state, (lua_Number)v);

    return 3;
}

int moEffectMovie::SpeedRegulation(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOfloat min_speed = (MOfloat) lua_tonumber (state, 1);
    MOfloat max_speed = (MOfloat) lua_tonumber (state, 2);
	SpeedRegulation(min_speed, max_speed);

    return 0;
}

int moEffectMovie::VCRStop(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_STOP);
    return 0;
}

int moEffectMovie::VCRPlay(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_PLAY);
    return 0;
}

int moEffectMovie::VCRPause(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_PAUSE);
    return 0;
}

int moEffectMovie::VCRReverse(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_REVERSE);
    return 0;
}

int moEffectMovie::VCRForward(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_FW);
    return 0;
}

int moEffectMovie::VCRRewind(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_RW);
    return 0;
}

int moEffectMovie::VCRFForward(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_FF);
    return 0;
}

int moEffectMovie::VCRFRewind(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_FR);
    return 0;
}

int moEffectMovie::VCRSeek(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_SEEK);
    return 0;
}

int moEffectMovie::VCRSpeed(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_SPEED);
    return 0;
}

int moEffectMovie::VCRPrevFrame(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_PREVFRAME);
    return 0;
}

int moEffectMovie::VCRNextFrame(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_NEXTFRAME);
    return 0;
}

int moEffectMovie::VCRLoop(moLuaVirtualMachine& vm)
{
	VCRCommand(MO_MOVIE_VCR_LOOP);
    return 0;
}

int moEffectMovie::VCRPlaySpeed(moLuaVirtualMachine& vm)
{
	VCRPlaySpeed();
    return 0;
}

int moEffectMovie::GetFramePosition(moLuaVirtualMachine& vm)
{
	lua_State *state = (lua_State *) vm;
	lua_pushnumber(state, (lua_Number)m_FramePosition);
    return 1;
}

int moEffectMovie::GetFramePositionF(moLuaVirtualMachine& vm)
{
	lua_State *state = (lua_State *) vm;
	lua_pushnumber(state, (lua_Number)m_FramePositionF);
    return 1;
}

int moEffectMovie::GetPlaySpeed(moLuaVirtualMachine& vm)
{
	lua_State *state = (lua_State *) vm;
	lua_pushnumber(state, (lua_Number)m_PlaySpeed);
    return 1;
}

int moEffectMovie::SetFramePosition(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;
    m_FramePosition = (MOlong) lua_tonumber (state, 1);
    return 0;
}

int moEffectMovie::SetFramePositionF(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;
    m_FramePositionF = (MOdouble) lua_tonumber (state, 1);
    return 0;
}

int moEffectMovie::SetPlaySpeed(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;
    m_PlaySpeed = (MOfloat) lua_tonumber (state, 1);
    return 0;
}

int moEffectMovie::GetTicks(moLuaVirtualMachine& vm)
{
	lua_State *state = (lua_State *) vm;
	lua_pushnumber(state, (lua_Number)m_Ticks);
    return 1;
}

int moEffectMovie::DrawLine(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOfloat x0 = (MOfloat) lua_tonumber (state, 1);
    MOfloat y0 = (MOfloat) lua_tonumber (state, 2);
    MOfloat x1 = (MOfloat) lua_tonumber (state, 3);
    MOfloat y1 = (MOfloat) lua_tonumber (state, 4);
    MOfloat w = (MOfloat) lua_tonumber (state, 5);
    MOfloat r = (MOfloat) lua_tonumber (state, 6);
    MOfloat g = (MOfloat) lua_tonumber (state, 7);
    MOfloat b = (MOfloat) lua_tonumber (state, 8);
    MOfloat a = (MOfloat) lua_tonumber (state, 9);

	glColor4f(r, g, b, a);
    glLineWidth(w);
    glBegin(GL_LINES);
        glVertex2f(x0, y0);
        glVertex2f(x1, y1);
    glEnd();

    return 0;
}
