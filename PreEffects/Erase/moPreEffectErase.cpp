/*******************************************************************************

                            moPreEffectErase.cpp

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

#include "moPreEffectErase.h"

//========================
//  Factory
//========================

moPreEffectEraseFactory *m_PreEffectEraseFactory = NULL;

MO_PLG_API moPreEffectFactory* CreatePreEffectFactory(){
	if(m_PreEffectEraseFactory==NULL)
		m_PreEffectEraseFactory = new moPreEffectEraseFactory();
	return(moPreEffectFactory*) m_PreEffectEraseFactory;
}

MO_PLG_API void DestroyPreEffectFactory(){
	delete m_PreEffectEraseFactory;
	m_PreEffectEraseFactory = NULL;
}

moPreEffect*  moPreEffectEraseFactory::Create() {
	return new moPreEffectErase();
}

void moPreEffectEraseFactory::Destroy(moPreEffect* fx) {
	delete fx;
}

//========================
//  PreEfecto
//========================


moPreEffectErase::moPreEffectErase() {
	SetName("erase");
}

moPreEffectErase::~moPreEffectErase() {
	Finish();
}

MOboolean moPreEffectErase::Init()
{
    if (!PreInit()) return false;

    moDefineParamIndex( ERASE_ALPHA, moText("alpha") );
    moDefineParamIndex( ERASE_COLOR, moText("color") );
		//moDefineParamIndex( ERASE_MODE, moText("mode") );

	return true;
}

void moPreEffectErase::Draw( moTempo* tempogral, moEffectState* parentstate )
{

	BeginDraw(tempogral, parentstate);

	moVector4d color4D = m_Config.EvalColor( moR(ERASE_COLOR) );
	float premultiplyalpha = color4D.W() * m_Config.Eval( moR( ERASE_ALPHA)) * m_EffectState.alpha;
	glClearColor(  color4D.X() * m_EffectState.tintr * premultiplyalpha,
                color4D.Y() * m_EffectState.tintg * premultiplyalpha,
                color4D.Z() * m_EffectState.tintb * premultiplyalpha,
                premultiplyalpha
                );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	EndDraw();
}

MOboolean moPreEffectErase::Finish()
{
    return PreFinish();
}

moConfigDefinition *
moPreEffectErase::GetDefinition( moConfigDefinition *p_configdefinition ) {

	p_configdefinition = moMoldeoObject::GetDefinition(p_configdefinition);
	p_configdefinition->Add( moText("alpha"), MO_PARAM_ALPHA );
	p_configdefinition->Add( moText("color"), MO_PARAM_COLOR, -1, moValue( "0.0", "FUNCTION", "0.0","FUNCTION","0.0","FUNCTION","0.0","FUNCTION") );
	p_configdefinition->Add( moText("syncro"), MO_PARAM_SYNC );
	p_configdefinition->Add( moText("phase"), MO_PARAM_PHASE );


	return p_configdefinition;
}
