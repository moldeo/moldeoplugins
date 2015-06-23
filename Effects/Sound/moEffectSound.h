/*******************************************************************************

                                moEffectSound.h

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

#ifndef __MO_EFFECT_SOUND_H__
#define __MO_EFFECT_SOUND_H__

#include "moPlugin.h"
#include "moTimeManager.h"
#include "moSoundManager.h"

#define MO_SOUND_TRANSLATE_X 0
#define MO_SOUND_TRANSLATE_Y 1
#define MO_SOUND_SCALE_X 2
#define MO_SOUND_SCALE_Y 3


enum moSoundParamIndex {
	SOUND_ALPHA,
	SOUND_COLOR,
	SOUND_SYNC,
	SOUND_PHASE,

	SOUND_SOUND,
	SOUND_MODE,
	SOUND_LOOP,
	SOUND_POSITION,
	SOUND_PITCH,
	SOUND_VOLUME,
	SOUND_BALANCE,

	SOUND_ECHO_DELAY,
	SOUND_ECHO_INTENSITY,
	SOUND_ECHO_FEEDBACK,

  SOUND_POSITION_IN,
  SOUND_POSITION_OUT,

  SOUND_FADE_IN,
  SOUND_FADE_OUT,

  SOUND_LAUNCH,

	SOUND_INLET,
	SOUND_OUTLET
};

class moEffectSound: public moEffect {

public:
    enum SoundMode {
      MO_SOUND_MODE_PLAY_ONCE = 0,
      MO_SOUND_MODE_SCRIPT = 1,
      MO_SOUND_MODE_CYCLE = 2,
      MO_SOUND_MODE_PLAYLIST = 3
    };

    moEffectSound();
    virtual ~moEffectSound();

    MOboolean Init();
    void Draw( moTempo* tempogral, moEffectState* parentstate = NULL);
    MOboolean Finish();
	void Interaction( moIODeviceManager * );
	moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );
    void Update( moEventList *Events );

private:

    long    last_ticks;
    moSound*   Sound;

    moGsGraph   m_Audio;
    moText      m_SoundFilename;
    moText      m_SoundFilenameFull;

    int         m_FramePosition;
    float     m_UserPosition;

    bool    m_bAudioStarted;

	MOint Tx, Ty, Tz;
	MOfloat Sx, Sy, Sz;
	MOint Rx, Ry, Rz;

};

class moEffectSoundFactory : public moEffectFactory {

public:
    moEffectSoundFactory() {}
    virtual ~moEffectSoundFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
