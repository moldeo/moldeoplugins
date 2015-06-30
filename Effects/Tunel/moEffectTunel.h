/*******************************************************************************

                              moEffectTunel.h

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

#ifndef __MO_EFFECT_TUNEL_H__
#define __MO_EFFECT_TUNEL_H__

#include "moPlugin.h"

/// Esto tiene que estar en el m_Config.
#define MAXTUNELSIDES     64
#define MAXTUNELSEGMENTS     64
#define TEXTURE_SPEED   0.5

class ajCoord {
public:
   MOfloat X, Y, Z;     //coord
   MOfloat XN, YN, ZN;  //coord news
   MOfloat INCX, INCY, INCZ;  //coord news
   MOfloat NX,NY,NZ;    //normal
   MOfloat U,V;         //text
   MOint I,J;           //raster
   MOtexture Texture;   //texture
   MOfloat Scalef;
};

class ajQuad {
public:
	ajCoord A;
	ajCoord B;
	ajCoord C;
	ajCoord D;
};


enum moTunelParamIndex {
	TUNEL_INLET,
	TUNEL_OUTLET,
	TUNEL_SCRIPT,
	TUNEL_ALPHA,
	TUNEL_COLOR,
	TUNEL_SYNC,
	TUNEL_PHASE,
	TUNEL_TEXTURE,
	TUNEL_BLENDING,
	TUNEL_TRANSLATEX,
	TUNEL_TRANSLATEY,
	TUNEL_TRANSLATEZ,
	TUNEL_ROTATE,
	TUNEL_TARGET_TRANSLATEX,
	TUNEL_TARGET_TRANSLATEY,
	TUNEL_TARGET_TRANSLATEZ,
	TUNEL_TARGET_ROTATE,
	TUNEL_SIDES,
	TUNEL_SEGMENTS,
	TUNEL_WIREFRAME

};

class moEffectTunel: public moEffect
{
public:
    moEffectTunel();
    virtual ~moEffectTunel();

    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
    moConfigDefinition *GetDefinition( moConfigDefinition *p_configdefinition );

    void UpdateParameters();

private:
    int sides;
    int segments;
    bool wireframe_mode;
    ajCoord Tunnels[MAXTUNELSIDES+1][MAXTUNELSEGMENTS];

};

class moEffectTunelFactory : public moEffectFactory
{
public:
    moEffectTunelFactory() {}
    virtual ~moEffectTunelFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
