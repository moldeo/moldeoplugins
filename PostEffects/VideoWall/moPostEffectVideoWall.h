/*******************************************************************************

                      moPostEffectVideoWall.h

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

#ifndef __MO_POST_EFFECT_VIDEOWALL_H__
#define __MO_POST_EFFECT_VIDEOWALL_H__

#include "moPostPlugin.h"

enum moVideoWallIndex {
    VIDEOWALL_INLET,
    VIDEOWALL_OUTLET,
    VIDEOWALL_SCRIPT,
    VIDEOWALL_ALPHA,
    VIDEOWALL_SYNCRO,
    VIDEOWALL_PHASE,
	VIDEOWALL_COLOR,
	VIDEOWALL_CONFIGURATION,
	VIDEOWALL_TRANSLATEZ
};

class moPostEffectVideoWall : public moPostEffect
{
public:
    //config

    moPostEffectVideoWall();
    virtual ~moPostEffectVideoWall();

    MOboolean Init();
    void Draw( moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
	virtual moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );
};

class moPostEffectVideoWallFactory : public moPostEffectFactory
{
public:
    moPostEffectVideoWallFactory() {}
    virtual ~moPostEffectVideoWallFactory() {}
    moPostEffect* Create();
    void Destroy(moPostEffect* fx);
};

extern "C"
{
MO_PLG_API moPostEffectFactory* CreatePostEffectFactory();
MO_PLG_API void DestroyPostEffectFactory();
}
#endif
