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
  Andr�s Colubri

*******************************************************************************/

#ifndef __MO_EFFECT_SOUND_H__
#define __MO_EFFECT_SOUND_H__

#include "moPlugin.h"

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
	SOUND_TEXTURE,
	SOUND_BLENDING,
	SOUND_WIDTH,
	SOUND_HEIGHT,
	SOUND_TRANSLATEX,
	SOUND_TRANSLATEY,
	SOUND_TRANSLATEZ,
	SOUND_ROTATEX,
	SOUND_ROTATEY,
	SOUND_ROTATEZ,
	SOUND_SCALEX,
	SOUND_SCALEY,
	SOUND_SCALEZ,
	SOUND_INLET,
	SOUND_OUTLET
};

class moEffectSound: public moEffect {

public:
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

    moGsGraph   m_Audio;
    moText      m_SoundFilename;
    moText      m_SoundFilenameFull;

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
