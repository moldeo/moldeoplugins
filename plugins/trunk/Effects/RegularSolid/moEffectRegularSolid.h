/*******************************************************************************

                              moEffectRegularSolid.h

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

#ifndef __MO_EFFECT_REGULARSOLID_H__
#define __MO_EFFECT_REGULARSOLID_H__

#include "moPlugin.h"

#ifndef MO_RED
#define MO_RED      0
#define MO_GREEN    1
#define MO_BLUE     2
#define MO_ALPHA    3
#endif

class moEffectRegularSolid : public moEffect
{
public:
    //config
    MOint solid, axisangle;
    MOint color, texture, size;
    MOint transx, transy, transz;
    MOint rotatx, rotaty, rotatz;
    MOint scalex, scaley, scalez;

    moTextureIndex Images;

    //planeta	
    GLUquadricObj *qobj;
	
	//reflejo
	//MOtexture Reflejo;
	
	//cintas
	/*
	MOtexture *Cintas;
	MOtexture Cinta;
	MOint Solido;
	MOuint cintas;
	MOuint ncintas;
	*/

    moEffectRegularSolid();
    virtual ~moEffectRegularSolid();

    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
};

class moEffectRegularSolidFactory : public moEffectFactory 
{
public:
    moEffectRegularSolidFactory() {}
    virtual ~moEffectRegularSolidFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
