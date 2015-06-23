/*******************************************************************************

                                moEffectGrid.h

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

#ifndef __MO_EFFECT_GRID_H__
#define __MO_EFFECT_GRID_H__

#include "moPlugin.h"
#include "moldeo.h"
#include "supereg/tmotor.h"

enum moGridKeys {
  GRID_TURN_LEFT = 0,
  GRID_TURN_RIGHT = 1,
  GRID_KEYUP = 2,
  GRID_KEYDOWN = 3,
  GRID_TILT_RIGHT = 4,
  GRID_TILT_LEFT = 5,
  GRID_FORWARD = 6,
  GRID_BACK = 7,
  GRID_PITCH = 8,
  GRID_TRIM = 9,
  GRID_RESET = 10

};

enum moGridCodes {
MO_GRID_UP 		= 0,
MO_GRID_DOWN	,
MO_GRID_TURN_LEFT,
MO_GRID_TURN_RIGHT,
MO_GRID_TILT_LEFT,
MO_GRID_TILT_RIGHT,
MO_GRID_FORWARD,
MO_GRID_BACK,
MO_GRID_PITCH,
MO_GRID_TRIM,
MO_GRID_WIREFRAME,
MO_GRID_RESETPOSITION
};

enum moGridParamIndex {
  GRID_INLET,
	GRID_OUTLET,
	GRID_SCRIPT,
	GRID_ALPHA,
	GRID_COLOR,
	GRID_SYNC,
	GRID_PHASE,
	GRID_SHADER,
	GRID_TEXTURE,
	GRID_TEXTUREB,
	GRID_TEXTUREC,
	GRID_TEXTURED,
	GRID_TEXTUREE,
	GRID_TEXTUREF,
	GRID_TEXTUREG,
	GRID_TEXTUREH,
  GRID_MAP,
  GRID_MAP_POSITION_X,
  GRID_MAP_POSITION_Y,
	GRID_SCALES,
	GRID_SIZE,
	GRID_WIREFRAME,
	GRID_WIREFRAME_WIDTH,
	GRID_WIREFRAME_CONFIGURATION,

	GRID_HEIGHT_MULTIPLY,
	GRID_HEIGHT_RANDOMIZER,
  GRID_PLANET_FACTOR,
  GRID_TORUS_FACTOR,
  GRID_WAVE_AMPLITUDE,

  GRID_CONTROL_ALTITUDE,
	GRID_CONTROL_ROLL_ANGLE,
	GRID_CONTROL_ROLL_FORCE,
	GRID_CONTROL_LIFT_ANGLE,
	GRID_CONTROL_LIFT_FORCE,

	GRID_CONTROL_SPEED,
	GRID_CONTROL_SPEED_ACCELERATION,
	GRID_CONTROL_MINIMUM_SPEED,
	GRID_CONTROL_MAXIMUM_SPEED,

	GRID_CONTROL_GRAVITY_FORCE,
	GRID_CONTROL_MINIMUM_ALTITUDE,
	GRID_CONTROL_MAXIMUM_ALTITUDE,
	GRID_CONTROL_MINIMUM_SURFACE_ALTITUDE,

	GRID_TRANSLATEX,
	GRID_TRANSLATEY,
	GRID_TRANSLATEZ,

	GRID_SCALEX,
	GRID_SCALEY,
	GRID_SCALEZ,

	GRID_ROTATEX,
	GRID_ROTATEY,
	GRID_ROTATEZ

};

class moEffectGrid: public moEffect
{
public:
    TEngine_Utility *Grid;
    moOutlet* m_pOutletCameraHeight;

    moEffectGrid();
    virtual ~moEffectGrid();

    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
    void UpdateParameters();

    moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );
    void Interaction(moIODeviceManager*);
    float turnAngle;
private:

    moVector2f  m_MapPosition;

	moTextureManager*	MOTextures;
    MOint color, texture, textureb;
    MOtexture   *textures;
    MOuint      ntextures;
    moInlet*    m_pGesture;

};

class moEffectGridFactory : public moEffectFactory
{
public:
    moEffectGridFactory() {}
    virtual ~moEffectGridFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
