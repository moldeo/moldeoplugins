/*******************************************************************************

                                moEffectLightDrawing.h

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

#ifndef __MO_EFFECT_LIGHTDRAWING_H__
#define __MO_EFFECT_LIGHTDRAWING_H__

#include "moPlugin.h"

#define MO_LIGHTDRAWING_TRANSLATE_X 0
#define MO_LIGHTDRAWING_TRANSLATE_Y 1
#define MO_LIGHTDRAWING_SCALE_X 2
#define MO_LIGHTDRAWING_SCALE_Y 3


/**

    Define OUTLETs

        TRACKING !! X and Y
        TRACKER > DATA
        IN OUT...
        TRACKING


*/

enum moLightDrawingParamIndex {

	LIGHTDRAWING_ALPHA,
	LIGHTDRAWING_COLOR,
	LIGHTDRAWING_SYNC,
	LIGHTDRAWING_PHASE,

    LIGHTDRAWING_CANVAS_WIDTH, /// result canvas width
    LIGHTDRAWING_CANVAS_HEIGHT, /// result canvas height

    LIGHTDRAWING_INTERVAL, /// time of each drawing, default = 3000 ms, the canvas is cleared every 3000 ms, if 0 ms, then it's never cleared...
    LIGHTDRAWING_FADE, /// fading of each new frame, default = 0.0

    LIGHTDRAWING_CROP_LEFT, /// crop left
    LIGHTDRAWING_CROP_RIGHT, /// crop right
    LIGHTDRAWING_CROP_TOP, /// crop top
    LIGHTDRAWING_CROP_BOTTOM, /// crop bottom

	LIGHTDRAWING_INTENSITY_MIN, /// light intensity minimun to draw pixel, default = 0.95
	LIGHTDRAWING_INTENSITY_MAX, /// light intensity maximum to draw pixel, default = 1.0
	LIGHTDRAWING_CROMINANCE_MIN, /// light hue minimum to draw pixel, default = 0.0
	LIGHTDRAWING_CROMINANCE_MAX, /// light hue maximum to draw pixel, default = 360.0
	LIGHTDRAWING_SATURATION_MIN, /// light saturation minimum, default = 0.0
	LIGHTDRAWING_SATURATION_MAX, /// light saturation maximum, default = 1.0

    LIGHTDRAWING_SCREENSHOT_MODE, /// default = 0 = no screenshot, 1 = auto screenshot at the end of drawing, 2 = auto screenshot at the end of interval screenshot
    LIGHTDRAWING_SCREENSHOT_INTERVAL, /// in milis. default = 0 = auto after drawing finished, see LIVEDRAWING_INTERVAL
    LIGHTDRAWING_SCREENSHOT_WIDTH, /// 0 = auto
    LIGHTDRAWING_SCREENSHOT_HEIGHT,/// 0 = auto

	LIGHTDRAWING_TEXTURE,
	LIGHTDRAWING_BLENDING,
	LIGHTDRAWING_WIDTH,
	LIGHTDRAWING_HEIGHT,
	LIGHTDRAWING_TRANSLATEX,
	LIGHTDRAWING_TRANSLATEY,
	LIGHTDRAWING_ROTATE,
	LIGHTDRAWING_SCALEX,
	LIGHTDRAWING_SCALEY,
	LIGHTDRAWING_INLET,
	LIGHTDRAWING_OUTLET
};

class moEffectLightDrawing: public moEffect {

public:
    moEffectLightDrawing();
    virtual ~moEffectLightDrawing();

    MOboolean Init();
    void Draw( moTempo* tempogral, moEffectState* parentstate = NULL);
    MOboolean Finish();

    moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

    void UpdateParameters();

    void Update( moEventList *Events );

    moTexture* m_pLightTexture;
    moTexture* m_pLiveTexture;

    /** Buffer to load cam texture and modify */
    MOubyte * m_buffer_data;
    MOubyte * m_buffer_light_data;

    float m_intensity_min;
    float m_intensity_max;
    float fade;
    long interval;

    moTimer m_TimerInterval;

};

class moEffectLightDrawingFactory : public moEffectFactory {

public:
    moEffectLightDrawingFactory() {}
    virtual ~moEffectLightDrawingFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
