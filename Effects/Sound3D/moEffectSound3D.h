/*******************************************************************************

                                moEffectSound3D.h

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

#ifndef __MO_EFFECT_SOUND3D_H__
#define __MO_EFFECT_SOUND3D_H__

#include "moPlugin.h"
#include "moTimeManager.h"
#include "moSoundManager.h"

#define MO_FREEALUT

#ifdef MO_FREEALUT
#include <AL/alut.h>
#endif


#define MO_SOUND3D_TRANSLATE_X 0
#define MO_SOUND3D_TRANSLATE_Y 1
#define MO_SOUND3D_SCALE_X 2
#define MO_SOUND3D_SCALE_Y 3

enum moSound3DParamIndex {
	SOUND3D_ALPHA,
	SOUND3D_COLOR,
	SOUND3D_SYNC,
	SOUND3D_PHASE,
	SOUND3D_SOUND,
	SOUND3D_SEEKPOSITION,
	SOUND3D_MODE,
	SOUND3D_LOOP,
	SOUND3D_LAUNCH,
	SOUND3D_SPEEDOFSOUND,
	SOUND3D_PITCH,
	SOUND3D_VOLUME,
	SOUND3D_BALANCE,

	SOUND3D_TEXTURE,
	SOUND3D_BLENDING,

	SOUND3D_TRANSLATEX,
	SOUND3D_TRANSLATEY,
	SOUND3D_TRANSLATEZ,

	SOUND3D_DIRECTIONX,
	SOUND3D_DIRECTIONY,
	SOUND3D_DIRECTIONZ,

	SOUND3D_SPEEDX,
	SOUND3D_SPEEDY,
	SOUND3D_SPEEDZ,

	SOUND3D_INLET,
	SOUND3D_OUTLET
};


class moSound3DAL : public moSound3D {

  public:

    moSound3DAL();
    virtual ~moSound3DAL();

    virtual MOboolean  Init();
    virtual MOboolean  Finish();

  /**
    OVERRIDE moSound functions
    OpenAL es diferente a GStreamer
  */
    virtual void Play();
    virtual void Stop();
    virtual void Pause();
    virtual void Rewind();
    virtual moStreamState State();
    virtual void Update();
    virtual void SetVolume( float gain );
    virtual void SetBalance( float gain );
    virtual float GetVolume();
    virtual float GetActualSampleVolume();
    virtual void SetPitch( float pitch );
    virtual float GetPitch();
    virtual void SetLoop( bool loop );
    virtual void SetSpeedOfSound( float speedofsound );
    virtual bool IsPlaying();

    MOboolean BuildEmpty( MOuint p_size );
    MOboolean BuildFromBuffer( MOuint p_size, GLvoid* p_buffer );
    MOboolean BuildFromFile( const moText& p_filename );

    virtual void Final();
    virtual void Frame(int frame);
    virtual void Repeat(int repeat);

    void PlaySample( MOint sampleid );

    void SetPosition( float x, float y, float z );
    void SetPosition( const moVector3f& v_pos ) { SetPosition(v_pos.X(), v_pos.Y(), v_pos.Z() );  }

    void SetVelocity( float x, float y, float z );
    void SetVelocity( const moVector3f& v_vel ) { SetVelocity(v_vel.X(), v_vel.Y(), v_vel.Z() );  }

    void SetDirection( float x, float y, float z );
    void SetDirection( const moVector3f& v_dir ) { SetDirection(v_dir.X(), v_dir.Y(), v_dir.Z() );  }


    int m_iAlState;

    MOuint		    m_Buffers[NUMBUFFERS];


    ALenum			m_eBufferFormat;
    ALsizei	m_ulDataSize;
    ALsizei	m_ulFrequency;
    ALsizei	m_ulFormat;
    ALsizei	m_ulBufferSize;
    ALsizei	m_ulBytesWritten;
    void *			m_pData;
    ALboolean	m_ulLoop;


    ALfloat	m_fFrequency;

    /**

      int m_iAlState;

      MOint			m_ActualSample;
      MOint			m_OldSample;

      moFile*			m_pFile;
      moDataManager*	m_pDataMan;
      moFileManager*	m_pFileMan;

      MOuint		    m_Buffers[NUMBUFFERS];
      MOuint		    m_SourceId;
      MOuint			  m_BufferId;

      MOint			m_eBufferFormat;


      unsigned long	m_ulDataSize;
      unsigned long	m_ulFrequency;
      unsigned long	m_ulFormat;
      unsigned long	m_ulBufferSize;
      unsigned long	m_ulBytesWritten;
      void *			m_pData;

      bool	m_ulLoop;

      MOint			m_BufferSize;
    */

};

class moEffectSound3D: public moEffect {

public:
  moEffectSound3D();
  virtual ~moEffectSound3D();

  virtual MOboolean Init();
  void Draw( moTempo* tempogral, moEffectState* parentstate = NULL);
  MOboolean Finish();
  void Interaction( moIODeviceManager * );
  moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

  virtual MOboolean ResolveValue( moParam& param, int value_index, bool p_refresh=false );

  void UpdateParameters();
  void UpdateSound( const moText& p_newfilename );
  void Update( moEventList *Events );

  void MM_render_one_buffer();
  static int al_check_error(const char * p_message);
  int alutCheckError(const char * p_message);
  void ShowBufferInfo( ALint p_BufferId );



private:

  moSoundManager* m_pSM;
  ALCdevice*  m_pALCDevice;
  ALCcontext* m_pALCContext;

  ALuint helloBuffer, helloSource;
  long    last_ticks;

  //moGsGraph   m_Audio;
  moSound3DAL*   m_pAudio;
  moText      m_Sound3DFilename;
  moText      m_Sound3DFilenameFull;

  float m_fSampleVolume;

  bool    m_bAudioStarted;
  bool    m_bLaunch;
  float    m_fLaunch;
  bool    m_bLoop;
  float   m_fPitch;
  float   m_fVolume;
  float   m_fSpeedOfSound;

  int i_PlayedTimes;

  MOint Tx, Ty, Tz;
  MOfloat Sx, Sy, Sz;
  MOint Rx, Ry, Rz;

  moVector3f m_vPosition;
  moVector3f m_vDirection;
  moVector3f m_vSpeed;

  static bool m_bAlutInit;

};

class moEffectSound3DFactory : public moEffectFactory {

public:
    moEffectSound3DFactory() {}
    virtual ~moEffectSound3DFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
