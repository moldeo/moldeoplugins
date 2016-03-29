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

#ifdef MO_MACOSX
#include "sndfile.h"
#endif

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

bool moEffectSound3D::m_bAlutInit = false;

moSound3DAL::moSound3DAL() {
	m_pData = NULL;
    m_BufferId = -1;
    m_SourceId = -1;
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
    moEffectSound3D::al_check_error( moText("BuildEmpty Buffer for  "+GetName()) );


  p_size = 0;
	return true;
}

MOboolean
moSound3DAL::BuildFromBuffer( MOuint p_size, GLvoid* p_buffer ) {
	//BuildEmpty(p_size);
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
#ifdef MO_FREEALUT
    #ifndef MO_MACOSX
    //m_BufferId = alutCreateBufferFromFile( p_filename  );

    m_pData = (void*)alutLoadMemoryFromFile( p_filename, &m_eBufferFormat, &m_ulDataSize, &m_fFrequency );
    if (m_pData) {
      alBufferData( m_BufferId, m_eBufferFormat, m_pData, m_ulDataSize, (int)m_fFrequency );
        DMessage( " frames (m_ulDataSize): " + IntToStr(m_ulDataSize) );
        DMessage( " samplerate (m_fFrequency): " + IntToStr( (int)m_fFrequency) );
        //DMessage( " channels: " + IntToStr(wsndinfo.channels) );
        DMessage( " format: " + IntToStr( m_eBufferFormat ) );
        switch(m_eBufferFormat) {
          case AL_FORMAT_MONO8:
            DMessage( " AL_FORMAT_MONO8" );
            break;
          case AL_FORMAT_MONO16:
            DMessage( " AL_FORMAT_MONO16" );
            break;
          case AL_FORMAT_STEREO8:
            DMessage( " AL_FORMAT_STEREO8" );
            break;
          case AL_FORMAT_STEREO16:
            DMessage( " AL_FORMAT_STEREO16" );
            break;
          default:
            break;
        }
        //DMessage( " sections: " + IntToStr(wsndinfo.sections) );
        //DMessage( " seekable: " + IntToStr(wsndinfo.seekable) );
    }
    if ((error = alutGetError()) != ALUT_ERROR_NO_ERROR)
    {
        MODebug2->Error( "moSound3DAL::BuildFromFile > alutCreateBufferFromFile failed: " + IntToStr( error ) );
        // Delete Buffers
        //alDeleteBuffers(NUM_BUFFERS, buffers);
        return false;
    }
    #else
    SF_INFO wsndinfo;
    SNDFILE* wsndfile = sf_open( p_filename, SFM_READ, &wsndinfo );
    if (wsndfile) {
        DMessage( " frames: " + IntToStr(wsndinfo.frames) );
        DMessage( " samplerate: " + IntToStr(wsndinfo.samplerate) );
        DMessage( " channels: " + IntToStr(wsndinfo.channels) );
        DMessage( " format: " + IntToStr(wsndinfo.format) );
        DMessage( " sections: " + IntToStr(wsndinfo.sections) );
        DMessage( " seekable: " + IntToStr(wsndinfo.seekable) );
        if (wsndinfo.samplerate)
            DMessage( " seconds: " + FloatToStr( (float)wsndinfo.frames/(float)wsndinfo.samplerate) );
        long num_items = wsndinfo.frames * wsndinfo.channels;
        long numread = 0;
        short* sdata = new short [num_items*2];
        int i=0;
        DMessage( " num_items: " + IntToStr(num_items) );
        if ( sdata ) {
            int readcount;
            numread = sf_read_short( wsndfile, sdata, num_items );
            short min = 65535;
            short max = 0;
            for(; i<num_items; ++i) {
                if (sdata[i]>max) max = sdata[i];
                if (sdata[i]<min) min = sdata[i];
            }
            DMessage( " numread: " + IntToStr(numread) + " min:" + IntToStr(min)+ " max:" + IntToStr(max) );
            if (numread==num_items) {
                DMessage( " Loaded OK! " );
                //alBufferData( helloBuffer, AL_FORMAT_MONO16, samples, buf_size, sample_rate);
                alBufferData( m_BufferId, AL_FORMAT_STEREO16, sdata, num_items*2, wsndinfo.samplerate );
                moEffectSound3D::al_check_error("alBufferData");

            }
        }
        if (sdata) delete [] sdata;
    } else {
        DError( moText(sf_strerror(wsndfile)));
    }
    #endif
#else
  m_BufferId = 0;
#endif


  MODebug2->Message("moSound3DAL::BuildFromFile > Loaded Buffer from file: " + p_filename );

  alGetBufferi( m_BufferId, AL_SIZE, &m_BufferSize );
  moEffectSound3D::al_check_error("alGetBufferi AL_SIZE");
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

    // Generate a Source to playback the Buffer
    alGenSources( 1, &m_SourceId );
    moEffectSound3D::al_check_error("Generating source");
    alSourcei( m_SourceId, AL_BUFFER, m_BufferId );
    moEffectSound3D::al_check_error("Attaching buffer to source");
    MODebug2->Message("moSound3DAL::BuildFromFile > buffersize: " + IntToStr(m_BufferSize) + " bufferid:" + IntToStr(m_BufferId) + " Attached to source:" + IntToStr(m_SourceId) );
/*
    alSourcePlay( m_SourceId );
    //alSourceStop( m_SourceId );
    moEffectSound3D::al_check_error("Playing source");

    ALenum current_playing_state;

    alGetSourcei( m_SourceId, AL_SOURCE_STATE, & current_playing_state);
    moEffectSound3D::al_check_error("alGetSourcei AL_SOURCE_STATE");

    if (AL_PLAYING == current_playing_state) {
        DMessage("Playing!!!!");

    }
    float spos;
    float fr = m_AudioFormat.m_SampleRate*m_AudioFormat.m_Channels*2;

    Update();
    if (m_ActualSample<3000000) {
        alSourcei(m_SourceId, AL_BYTE_OFFSET, 3000000+1);
        DMessage("Goto:" + IntToStr(3000000) );
    }

    while (AL_PLAYING == current_playing_state) {

        //printf("still playing ... so sleep\n");

        //alutSleep(1);   // should use a thread sleep NOT sleep() for a more responsive finish

        //alutSleep(1);
        alGetSourcei( m_SourceId , AL_BYTE_OFFSET , &m_ActualSample);

        spos = m_ActualSample;

        if (m_ActualSample!=m_OldSample)
            DMessage( "Playing at: " + IntToStr( m_ActualSample ) + " pos (s):" + FloatToStr(spos/fr,2,2) );

        m_OldSample = m_ActualSample;

        if (m_ActualSample>6000000) {
            DMessage("Goto:" + IntToStr(3000000 + 1) );
            //PlaySample( 3000000 + 1 );
            alSourcei(m_SourceId, AL_BYTE_OFFSET, 3000000+1);
        }

        alGetSourcei( m_SourceId, AL_SOURCE_STATE, & current_playing_state);
        //alGetSourcei( m_SourceId, AL_SOURCE_, & current_playing_state);
        moEffectSound3D::al_check_error("alGetSourcei AL_SOURCE_STATE");
    }
 */
    alSourceStop( m_SourceId );
    return true;

}

void moSound3DAL::Play() {
	// Start playing source

	alSourcePlay(m_SourceId);

}


void moSound3DAL::PlaySample( MOint sampleid ) {

    Update();//update actual state and sample

	//alSourceStop(m_SourceId);
	if (m_ActualSample!=sampleid && sampleid!=0 && (State() != MO_STREAMSTATE_PLAYING)  ) {
		alSourcei(m_SourceId, AL_BYTE_OFFSET, sampleid);
		alSourcePlay(m_SourceId);
		m_ActualSample = sampleid;
	}

	ALenum error = alGetError();
	if (error!=AL_NO_ERROR) {
    MODebug2->Error("moSound3DAL::PlaySample > playing sample id: " + IntToStr(sampleid) + " error:" + IntToStr(error) );
	}

}

void moSound3DAL::Update() {

    State();
    m_OldSample = m_ActualSample;
    alGetSourcei( m_SourceId , AL_BYTE_OFFSET , &m_ActualSample);

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
}

void moSound3DAL::Repeat(int repeat) {
    alSourcei(m_SourceId, AL_LOOPING, repeat==1 );
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


bool
moSound3DAL::IsPlaying() {
  return (State()== MO_STREAMSTATE_PLAYING);
}


void moSound3DAL::SetPosition( float x, float y, float z ) {

	alSource3f( m_SourceId, AL_POSITION, x, y, z );
  float listenerPos[ 3 ] = { 0.0, 0.0, 0.0 };
  float listenerOri[ 6 ] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };
  float listenerVel[ 3 ] = { 0.0, 0.0, 0.0  };
  alListenerfv( AL_POSITION, listenerPos );
  alListenerfv( AL_VELOCITY, listenerVel );
  alListenerfv( AL_ORIENTATION, listenerOri );
  //alSourcei( m_SourceId, AL_SOURCE_RELATIVE, true );
  //alSourcef( m_SourceId, AL_DOPPLER_FACTOR, 2.0 );
  //alSourcef( m_SourceId, AL_DOPPLER_VELOCITY, 1.0 );
}

void moSound3DAL::SetVelocity( float x, float y, float z ) {

	alSource3f( m_SourceId, AL_VELOCITY, x, y, z );

}

void moSound3DAL::SetDirection( float x, float y, float z ) {

	alSource3f( m_SourceId, AL_DIRECTION, x, y, z );

}

void moSound3DAL::SetVolume( float volume ) {

	alSourcef( m_SourceId, AL_GAIN, volume );

}

float moSound3DAL::GetVolume() {

	alGetSourcef( m_SourceId, AL_GAIN, &m_Volume );

	return m_Volume;

}

float moSound3DAL::GetActualSampleVolume() {
  float avolume = 0;
  if (m_pData) {
    //MODebug2->Message("actualsample:"+IntToStr( m_ActualSample )+"/"+IntToStr(m_ulDataSize) );
    if (m_ActualSample < m_ulDataSize  ) {
      /*int indexp = m_ActualSample / (2/m_AudioFormat.m_Channels);
      avolume = (float) ((WORD*)m_pData)[ indexp ];
      avolume = (1.0f*fabs(avolume)) / (65535.0/2.0);
      */
    }

  }
  return avolume;
}

void moSound3DAL::SetPitch( float pitch )  {
    m_Pitch = pitch;

    alSourcef(m_SourceId, AL_PITCH, pitch);

}


float moSound3DAL::GetPitch()  {

  alGetSourcef( m_SourceId, AL_PITCH, &m_Pitch );

  return m_Pitch;
}

void moSound3DAL::SetLoop( bool loop ) {

  //alSource3f();
  alSourcei( m_SourceId, AL_LOOPING, loop);
}


void
moSound3DAL::SetSpeedOfSound( float speedofsound ) {
  //alSourcei( m_SourceId, AL_SPEED_OF_SOUND, speedofsound );
  alSpeedOfSound( speedofsound );
}



void moSound3DAL::SetBalance( float balance ) {
    //DMessage("SetBalance for "+GetName()+" balance:" + FloatToStr(balance) );
    //alSourcef( m_SourceId, AL_BALANCE, balance );
    float listenerPos[ 3 ] = { 0.0, 0.0, 0.0 };
    float listenerOri[ 6 ] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };
    float listenerVel[ 3 ] = { 0.0, 0.0, 0.0  };
    alListenerfv( AL_POSITION, listenerPos );
    alListenerfv( AL_VELOCITY, listenerVel );
    alListenerfv( AL_ORIENTATION, listenerOri );

    float sourcePos[ 3 ] = { balance*10.0, 0.0, 0.0 };
    alSourcefv(m_SourceId, AL_POSITION, sourcePos);

    m_Balance = balance;
}


//========================
//  Efecto
//========================
moEffectSound3D::moEffectSound3D() {

	SetName("sound3d");
	m_pSM = NULL;
  m_pAudio = NULL;
    m_pALCContext = NULL;
    m_pALCDevice = NULL;

}

moEffectSound3D::~moEffectSound3D() {
	Finish();
}

int moEffectSound3D::al_check_error(const char * p_message) { // generic OpenAL error checker

    ALenum al_error;
    al_error = alGetError();

    if(AL_NO_ERROR != al_error) {

        printf("OPENAL ERROR - %s  (%s)\n", alGetString(al_error), p_message);
        return al_error;
    }
    return 0;
}

int moEffectSound3D::alutCheckError(const char * p_message) { // generic OpenAL error checker

    ALenum alut_error;
    alut_error = alutGetError();

    if( ALUT_ERROR_NO_ERROR != alut_error) {

        printf("ALUT ERROR - %s  (%s)\n", alutGetErrorString(alut_error), p_message);
        return alut_error;
    }
    return 0;
}

void moEffectSound3D::ShowBufferInfo( ALint p_BufferId ) {

    ALint bcount,channels, frequency, bsize;
    alGetBufferi(p_BufferId, AL_SIZE, &bsize);
    al_check_error("failed call to alGetBufferi AL_SIZE");
    alGetBufferi(p_BufferId, AL_BITS, &bcount);
    al_check_error("failed call to alGetBufferi AL_BITS");
    alGetBufferi(p_BufferId, AL_CHANNELS, &channels);
    al_check_error("failed call to alGetBufferi AL_CHANNELS");
    alGetBufferi(p_BufferId, AL_FREQUENCY, &frequency);
    al_check_error("failed call to alGetBufferi AL_FREQUENCY");

    DMessage("ShowBufferInfo > size: " + IntToStr(bsize)
            + " bitcount:" + IntToStr(bcount)
            + " channels:" + IntToStr(channels)
            + " frequency:" + IntToStr(frequency) );

}

MOboolean moEffectSound3D::Init() {

#ifdef MAC_OSX
    m_pALCDevice = alcOpenDevice( alcGetString(NULL, ALC_DEVICE_SPECIFIER) );
    if (!m_pALCDevice) {
        DError("Init > alcOpenDevice no device created!");
        return false;
    } else {
        DMessage( moText("Init > alcOpenDevice device:") + IntToStr((long)m_pALCDevice) );
    }
#endif
    /*
    ALboolean enumeration;

    enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
    if (enumeration == AL_FALSE) {
        // enumeration not supported
        DError("Init > enumeration not supported.");
    } else {
        // enumeration supported
        DMessage("Init > enumerating devices:");
        const ALCchar *ldevices;
        ldevices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
        const ALCchar *ldevice = ldevices, *next = ldevices + 1;
        size_t len = 0;
        while (device && *ldevice != '\0' && next && *next != '\0') {
            DMessage( moText("device:") + moText((char*)ldevice) );
            len = strlen(ldevice);
            ldevice += (len + 1);
            next += (len + 2);
        }

    }
    */

#ifdef MAC_OSX
    m_pALCContext = alcCreateContext(m_pALCDevice, NULL);
    if (!alcMakeContextCurrent(m_pALCContext)) {
        DError("Init > alcMakeContextCurrent Error!");
        return false;
    } else {
        DMessage( moText("Init > context:") + IntToStr((long)m_pALCContext) );
    }
#endif

#ifdef MO_FREEALUT

   //if ( alutInitWithoutContext( NULL, NULL )) {

   if (m_bAlutInit==false) {
      m_bAlutInit = alutInit( NULL, NULL );
   }

    if ( m_bAlutInit ) {

      DMessage("Init > ALUT Initialized!");

      //alDistanceModel( AL_INVERSE_DISTANCE );
      //alDistanceModel( AL_INVERSE_DISTANCE );
      //MM_render_one_buffer();
      //generate al id
      //generate al buffer
      //MM_render_one_buffer();
  #ifndef MAC_OSX
  /*
      helloBuffer = alutCreateBufferHelloWorld ();
      alutCheckError( "alutCreateBufferHelloWorld" );
      DMessage( "helloBuffer:"+ IntToStr(helloBuffer) );

      //al_check_error("failed call to alutCreateBufferHelloWorld");
      //associate buffer to al id source
      if (helloBuffer) {
        ShowBufferInfo(helloBuffer);
        alGenSources (1, &helloSource);
        al_check_error("failed call to alGenSources");
        alSourcei (helloSource, AL_BUFFER, helloBuffer);
        alSourcePlay(helloSource);
      }
    */
  #endif

  } else {
      DError("Init > ALUT not initialized. Check if ALUT is installed correctly.");
      //return false;
  }
#endif


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

  moDefineParamIndex( SOUND3D_SPEEDX, moText("speedx") );
  moDefineParamIndex( SOUND3D_SPEEDY, moText("speedy") );
  moDefineParamIndex( SOUND3D_SPEEDZ, moText("speedz") );

  moDefineParamIndex( SOUND3D_DIRECTIONX, moText("directionx") );
  moDefineParamIndex( SOUND3D_DIRECTIONY, moText("directiony") );
  moDefineParamIndex( SOUND3D_DIRECTIONZ, moText("directionz") );

  moDefineParamIndex( SOUND3D_INLET, moText("inlet") );
  moDefineParamIndex( SOUND3D_OUTLET, moText("outlet") );

  UpdateParameters();

  last_ticks = 0;

  return true;
}

MOboolean moEffectSound3D::Finish()
{
  #ifdef MAC_OSX

    if (m_pALCContext) {
        alcDestroyContext(m_pALCContext);
        m_pALCContext = NULL;
    }

    if (m_pALCDevice) {
        alcCloseDevice(m_pALCDevice);
        m_pALCDevice = NULL;
    }
  #endif

#ifdef MO_FREEALUT
    alutExit ();
#endif
    return PreFinish();
}


void moEffectSound3D::MM_render_one_buffer() {


    alGenBuffers( 1, &helloBuffer );
    al_check_error("failed call to alGenBuffers");

    // fill buffer with an audio curve
    // float freq = 440.f;
    float freq = 100.f;
    float incr_freq = 0.1f;

    int seconds = 4;	// this determines buffer size based on seconds of playback audio independent of sample rate
    // unsigned sample_rate = 22050;
    unsigned sample_rate = 44100;
    unsigned int channels = 2;
    double my_pi = 3.14159;
    size_t buf_size = seconds * sample_rate * channels;

    //short * samples = (short *)malloc(sizeof(short) * buf_size); // allocates audio buffer memory
    short * samples = new short[buf_size];

    printf("\nhere is freq %f\n", freq);

    int i=0;
    for(; i<buf_size; ++i) {



        samples[i] = 32760 * sin( (2.f * my_pi * freq)/sample_rate * i ); // populate audio curve data point

        //if (i%2==0) samples[i] = samples[i];

        freq += incr_freq; // jack around frequency ... just comment out this for sin curve

        if (10.0 > freq || freq > 5000.0) { // toggle frequency increment if we reach min or max freq

            incr_freq *= -1.0f;
        }
    }

    // upload buffer to OpenAL
    if (channels==1) alBufferData( helloBuffer, AL_FORMAT_MONO16, samples, buf_size, sample_rate);
    if (channels==2) alBufferData( helloBuffer, AL_FORMAT_STEREO16, samples, buf_size, sample_rate);
    al_check_error("populating alBufferData");

    free(samples); // release audio buffer memory after above upload into OpenAL internal buffer

    /// Set-up sound source and play buffer

    //alGenSources(1, & streaming_source[0]);
    //alSourcei(streaming_source[0], AL_BUFFER, helloBuffer);
    alGenSources (1, &helloSource);
    al_check_error("failed call to alGenSources");
    alSourcei (helloSource, AL_BUFFER, helloBuffer);
    alSourcePlay(helloSource);

    // ---------------------

    ALenum current_playing_state;
    alGetSourcei( helloSource, AL_SOURCE_STATE, & current_playing_state);
    al_check_error("alGetSourcei AL_SOURCE_STATE");

    while (AL_PLAYING == current_playing_state) {

        //printf("still playing ... so sleep\n");

        //alutSleep(1);   // should use a thread sleep NOT sleep() for a more responsive finish
        alGetSourcei( helloSource, AL_SOURCE_STATE, & current_playing_state);
        al_check_error("alGetSourcei AL_SOURCE_STATE");
    }

    printf("end of playing\n");


}


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

  m_bLoop = m_Config.Int( moR( SOUND3D_LOOP ) );
  m_fPitch = m_Config.Eval( moR( SOUND3D_PITCH ) );
  m_fVolume = m_Config.Eval( moR( SOUND3D_VOLUME ) );
  m_fSpeedOfSound = m_Config.Eval( moR( SOUND3D_SPEEDOFSOUND ) );
  //MODebug2->Message("Pitch:" + FloatToStr(m_fPitch) );
  m_vPosition = moVector3f( m_Config.Eval(moR(SOUND3D_TRANSLATEX)),
                            m_Config.Eval(moR(SOUND3D_TRANSLATEY)),
                            m_Config.Eval(moR(SOUND3D_TRANSLATEZ)) );

  m_vSpeed = moVector3f( m_Config.Eval(moR(SOUND3D_SPEEDX)),
                            m_Config.Eval(moR(SOUND3D_SPEEDY)),
                            m_Config.Eval(moR(SOUND3D_SPEEDZ)) );

  m_vDirection = moVector3f( m_Config.Eval(moR(SOUND3D_DIRECTIONX)),
                            m_Config.Eval(moR(SOUND3D_DIRECTIONY)),
                            m_Config.Eval(moR(SOUND3D_DIRECTIONZ)) );

  if (m_pAudio) {
    m_pAudio->SetLoop( m_bLoop );
    m_pAudio->SetPitch( m_fPitch );
    m_pAudio->SetVolume( m_fVolume );
    m_pAudio->SetSpeedOfSound( m_fSpeedOfSound );
    m_pAudio->SetDirection( m_vDirection );
    m_pAudio->SetVelocity( m_vSpeed );
    m_pAudio->SetPosition( m_vPosition );

    m_pAudio->Update();
    m_fSampleVolume = 0.0;
    //m_fSampleVolume = m_pAudio->GetActualSampleVolume();
    //MODebug2->Message("m_fSampleVolume:"+FloatToStr( m_fSampleVolume )  );
  }





}


void
moEffectSound3D::UpdateSound( const moText& p_newfilename ) {

    moDataManager* pDataMan = m_pResourceManager->GetDataMan();
    moFile SoundFile;
    moSound3DAL* prev_Audio  = NULL;
    if (m_Sound3DFilename!=p_newfilename ) {

        m_Sound3DFilename = p_newfilename;
        if (m_pAudio) {
          prev_Audio = m_pAudio;
        }
        m_pAudio = (moSound3DAL*)m_pSM->GetSound( m_Sound3DFilename, false );

        if (m_pAudio) {
            if (prev_Audio) {
              prev_Audio->Stop();
            }
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
              if (m_pAudio->BuildFromFile( m_pAudio->GetFileName() ) ) {
                  DMessage("Adding sound to SoundManager!");
                    m_pSM->AddSound( (moSound*) m_pAudio );
              }

          }

        }

    }

    if (m_bLaunch != m_Config.Int(moR(SOUND3D_LAUNCH)) ) {

      DMessage("Launching sound!" + m_pAudio->GetName());
      m_bLaunch = m_Config.Int(moR(SOUND3D_LAUNCH));
        if (m_bLaunch && moIsTimerPlaying()) {
          m_pAudio->Play();
        }
    }


    if ( moIsTimerStopped() && m_pAudio ) {
      if (m_pAudio->IsPlaying())
        m_pAudio->Stop();
    }
    if (m_bLaunch && m_pAudio) {
      if (!m_pAudio->IsPlaying() && moIsTimerPlaying() ) {
        m_pAudio->Play();
      }
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
    moShaderManager* pSMan;
    moGLManager* pGLMan;
    moRenderManager* pRMan;
    moTextureManager* pTMan;

    if (m_pResourceManager) {
      pSMan = m_pResourceManager->GetShaderMan();
      if (!pSMan) return;

      pGLMan = m_pResourceManager->GetGLMan();
      if (!pGLMan) return;

      pRMan = m_pResourceManager->GetRenderMan();
      if (!pRMan) return;

      pTMan = m_pResourceManager->GetTextureMan();
      if (!pTMan) return;

    } else return ;

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    ///MATERIAL
    moMaterial Mat;
      Mat.m_Map = pTMan->GetTexture(pTMan->GetTextureMOId( "default", false ));
      Mat.m_MapGLId = Mat.m_Map->GetGLId();
      //Mat.m_MapGLId = 1;
      //Mat.m_Color = moColor( 1.0, 1.0, 1.0 );
      //Mat.m_fTextWSegments = 13.0f;
      //Mat.m_fTextHSegments = 13.0f;
      //Mat.m_vLight = moVector3f( -1.0, -1.0, -1.0 );
      //Mat.m_vLight.Normalize();
      //Mat.m_PolygonMode = MO_POLYGONMODE_LINE;
      //Mat.m_PolygonMode = MO_POLYGONMODE_FILL;
      //Mat.m_fWireframeWidth = 0.0005f;

    ///GEOMETRY
    moSphereGeometry Sphere( 0.1+0.01*m_fSampleVolume, 8, 8 );

    ///MESH MODEL (aka SCENE NODE)
    float progress = 0.0;
    moGLMatrixf Model;
    Model.MakeIdentity()
         .Rotate(   360.0*progress*moMathf::DEG_TO_RAD, 0.0, 1.0, 0.0 )
         .Translate(    m_vPosition.X(), m_vPosition.Y(), m_vPosition.Z() );
    moMesh Mesh( Sphere, Mat );
    Mesh.SetModelMatrix(Model);

    ///CAMERA PERSPECTIVE
    moCamera3D Camera3D;
    pGLMan->SetDefaultPerspectiveView( pRMan->ScreenWidth(), pRMan->ScreenHeight() );
    //  Camera3D.MakePerspective(60.0f, p_display_info.Proportion(), 0.01f, 1000.0f );
    Camera3D = pGLMan->GetProjectionMatrix();

    ///RENDERING
    pRMan->Render( &Mesh, &Camera3D );
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
