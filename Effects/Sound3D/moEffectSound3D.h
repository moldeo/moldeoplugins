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

#ifdef MO_FREEALUT
#include <AL/alut.h>
#endif // FREEALUT

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
	SOUND3D_TEXTURE,
	SOUND3D_BLENDING,
	SOUND3D_WIDTH,
	SOUND3D_HEIGHT,
	SOUND3D_TRANSLATEX,
	SOUND3D_TRANSLATEY,
	SOUND3D_TRANSLATEZ,
	SOUND3D_ROTATEX,
	SOUND3D_ROTATEY,
	SOUND3D_ROTATEZ,
	SOUND3D_SCALEX,
	SOUND3D_SCALEY,
	SOUND3D_SCALEZ,
	SOUND3D_INLET,
	SOUND3D_OUTLET
};

class moEffectSound3D: public moEffect {

public:
    moEffectSound3D();
    virtual ~moEffectSound3D();

    MOboolean Init();
    void Draw( moTempo* tempogral, moEffectState* parentstate = NULL);
    MOboolean Finish();
	void Interaction( moIODeviceManager * );
	moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );
    void Update( moEventList *Events );

private:

    ALuint helloBuffer, helloSource;
    long    last_ticks;

    moGsGraph   m_Audio;
    moText      m_Sound3DFilename;
    moText      m_Sound3DFilenameFull;

    bool    m_bAudioStarted;

	MOint Tx, Ty, Tz;
	MOfloat Sx, Sy, Sz;
	MOint Rx, Ry, Rz;

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
