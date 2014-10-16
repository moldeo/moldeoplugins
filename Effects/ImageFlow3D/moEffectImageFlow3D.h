/*******************************************************************************

                             moEffectImageFlow3D.h

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                     *
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

  Copyright(C) 2012 Fabricio Costa

  Authors:
  Fabricio Costa

*******************************************************************************/

#include "moPlugin.h"
#include "moTextureIndex.h"
#include "moMathFunction.h"

#ifndef __MO_EFFECT_IMAGE_FLOW_3D_H__
#define __MO_EFFECT_IMAGE_FLOW_3D_H__

enum moImageFlow3DParamIndex {
    IMAGEFLOW3D_ALPHA,
	IMAGEFLOW3D_COLOR,
	IMAGEFLOW3D_SYNCRO,
	IMAGEFLOW3D_PHASE,
	IMAGEFLOW3D_BLENDING,
	IMAGEFLOW3D_IMAGES,
	IMAGEFLOW3D_VELOCITY,
	IMAGEFLOW3D_FLOW_MODE,
	IMAGEFLOW3D_TEXTURE,
	IMAGEFLOW3D_TRANSLATEX,
	IMAGEFLOW3D_TRANSLATEY,
	IMAGEFLOW3D_TRANSLATEZ,
	IMAGEFLOW3D_ROTATEX,
	IMAGEFLOW3D_ROTATEY,
	IMAGEFLOW3D_ROTATEZ,
	IMAGEFLOW3D_SCALEX,
	IMAGEFLOW3D_SCALEY,
	IMAGEFLOW3D_SCALEZ
};

class moEffectImageFlow3D : public moEffect
{
public:
    moEffectImageFlow3D();
    virtual ~moEffectImageFlow3D();

    MOboolean Init();
    void Draw( moTempo*, moEffectState* parentstate = NULL);
    void Interaction( moIODeviceManager * );
    MOboolean Finish();
	virtual moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

protected:
	float color[4];
	int blending;
	int flow_mode;
    moTextureIndex Images;

	int idx0, idx1;
	int tex0, tex1;
    float flow_coord;
	float flow_velocity0, flow_velocity, flow_velocity_bak;
	float minx, maxx, miny, maxy;

	void setBlendMode();
	void updateParameters();
};

class moEffectImageFlow3DFactory : public moEffectFactory
{
public:
    moEffectImageFlow3DFactory() {}
    virtual ~moEffectImageFlow3DFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
