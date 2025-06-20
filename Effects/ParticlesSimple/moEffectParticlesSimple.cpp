/*******************************************************************************

                              moEffectParticlesSimple.cpp

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

#include "moEffectParticlesSimple.h"


#include "moArray.h"
moDefineDynamicArray( moParticlesSimpleArray )

//========================
//  Factory
//========================

moEffectParticlesSimpleFactory *m_EffectParticlesSimpleFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectParticlesSimpleFactory==NULL)
		m_EffectParticlesSimpleFactory = new moEffectParticlesSimpleFactory();
	return(moEffectFactory*) m_EffectParticlesSimpleFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectParticlesSimpleFactory;
	m_EffectParticlesSimpleFactory = NULL;
}

moEffect*  moEffectParticlesSimpleFactory::Create() {
	return new moEffectParticlesSimple();
}

void moEffectParticlesSimpleFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectParticlesSimple::moEffectParticlesSimple() {
	SetName("particlessimple");

	///stereo for this effect is activated by default
	//m_EffectState.stereo = MO_ACTIVATED;
}

moEffectParticlesSimple::~moEffectParticlesSimple() {
	Finish();
}

moConfigDefinition *
moEffectParticlesSimple::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );

	p_configdefinition->Add( moText("particlecolor"), MO_PARAM_COLOR, PARTICLES_PARTICLECOLOR );

	p_configdefinition->Add( moText("font"), MO_PARAM_FONT, PARTICLES_FONT, moValue( "Default", "TXT", "0", "NUM", "32.0", "NUM") );
	p_configdefinition->Add( moText("text"), MO_PARAM_TEXT, PARTICLES_TEXT, moValue( "Insert text in text parameter", "TXT") );
	p_configdefinition->Add( moText("ortho"), MO_PARAM_NUMERIC, PARTICLES_ORTHO, moValue( "0", "NUM").Ref() );


	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, PARTICLES_TEXTURE, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("folders"), MO_PARAM_TEXTUREFOLDER, PARTICLES_FOLDERS, moValue( "", "TXT") );

	p_configdefinition->Add( moText("texture_mode"), MO_PARAM_NUMERIC, PARTICLES_TEXTUREMODE, moValue( "0", "NUM"), moText("UNIT,PATCH,MANY,MANY2PATCH,MANYBYORDER"));

	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, PARTICLES_BLENDING, moValue( "0", "NUM") );
	p_configdefinition->Add( moText("width"), MO_PARAM_NUMERIC, PARTICLES_WIDTH, moValue( "9", "NUM") );
	p_configdefinition->Add( moText("height"), MO_PARAM_NUMERIC, PARTICLES_HEIGHT, moValue( "9", "NUM") );

    p_configdefinition->Add( moText("sizex"), MO_PARAM_FUNCTION, PARTICLES_SIZEX, moValue( "10.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("sizey"), MO_PARAM_FUNCTION, PARTICLES_SIZEY, moValue( "10.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("sizez"), MO_PARAM_FUNCTION, PARTICLES_SIZEY, moValue( "10.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("gravity"), MO_PARAM_FUNCTION, PARTICLES_GRAVITY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("viscosity"), MO_PARAM_FUNCTION, PARTICLES_VISCOSITY, moValue( "0.8", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("maxage"), MO_PARAM_FUNCTION, PARTICLES_MAXAGE, moValue( "3000", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("emitionperiod"), MO_PARAM_FUNCTION, PARTICLES_EMITIONPERIOD, moValue( "10", "FUNCTION").Ref() );
	//p_configdefinition->Add( moText("emitionrate"), MO_PARAM_NUMERIC, PARTICLES_EMITIONRATE, moValue( "1", "NUM").Ref() );
	p_configdefinition->Add( moText("emitionrate"), MO_PARAM_FUNCTION, PARTICLES_EMITIONRATE, moValue( "1", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("deathperiod"), MO_PARAM_NUMERIC, PARTICLES_DEATHPERIOD, moValue( "1", "NUM").Ref() );
	p_configdefinition->Add( moText("particlescript"), MO_PARAM_SCRIPT, PARTICLES_SCRIPT2 );

	p_configdefinition->Add( moText("randommethod"), MO_PARAM_NUMERIC, PARTICLES_RANDOMMETHOD, moValue( "0", "NUM").Ref(), moText("NOISY,COLINEAR,PERPENDICULAR") );
	p_configdefinition->Add( moText("creationmethod"), MO_PARAM_NUMERIC, PARTICLES_CREATIONMETHOD, moValue( "0", "NUM").Ref(), moText("LINEAR,PLANAR,VOLUMETRIC,CENTER,PLANAR_SNAP,VOLUMETRIC_SNAP") );
	p_configdefinition->Add( moText("orientationmode"), MO_PARAM_NUMERIC, PARTICLES_ORIENTATIONMODE, moValue( "0", "NUM").Ref(), moText("FIXED,CAMERA,MOTION,ACCELERATION,NORMAL") );

	p_configdefinition->Add( moText("fadein"), MO_PARAM_FUNCTION, PARTICLES_FADEIN, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("fadeout"), MO_PARAM_FUNCTION, PARTICLES_FADEOUT, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("sizein"), MO_PARAM_FUNCTION, PARTICLES_SIZEIN, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("sizeout"), MO_PARAM_FUNCTION, PARTICLES_SIZEOUT, moValue( "0", "FUNCTION").Ref() );



	p_configdefinition->Add( moText("attractortype"), MO_PARAM_NUMERIC, PARTICLES_ATTRACTORTYPE, moValue( "0", "NUM").Ref(), moText("POINT,GRID,SPHERE,TUBE,JET,TRACKER,VERTEX") );
	p_configdefinition->Add( moText("attractormode"), MO_PARAM_NUMERIC, PARTICLES_ATTRACTORMODE, moValue( "0", "NUM").Ref(), moText("ACCELERATION,STICK,BOUNCE,BREAKS,BRAKE,LINEAR") );
	p_configdefinition->Add( moText("attractorvectorx"), MO_PARAM_FUNCTION, PARTICLES_ATTRACTORVECTOR_X, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("attractorvectory"), MO_PARAM_FUNCTION, PARTICLES_ATTRACTORVECTOR_Y, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("attractorvectorz"), MO_PARAM_FUNCTION, PARTICLES_ATTRACTORVECTOR_Z, moValue( "0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("emittertype"), MO_PARAM_NUMERIC, PARTICLES_EMITTERTYPE, moValue( "0", "NUM").Ref(), moText("GRID,SPHERE,TUBE,JET,POINT,TRACKER,TRACKER2,SPIRAL,CIRCLE") );
	p_configdefinition->Add( moText("emittervectorx"), MO_PARAM_FUNCTION, PARTICLES_EMITTERVECTOR_X, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("emittervectory"), MO_PARAM_FUNCTION, PARTICLES_EMITTERVECTOR_Y, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("emittervectorz"), MO_PARAM_FUNCTION, PARTICLES_EMITTERVECTOR_Z, moValue( "0", "FUNCTION").Ref() );


	p_configdefinition->Add( moText("randomposition"), MO_PARAM_FUNCTION, PARTICLES_RANDOMPOSITION, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("randompositionx"), MO_PARAM_FUNCTION, PARTICLES_RANDOMPOSITION_X, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("randompositiony"), MO_PARAM_FUNCTION, PARTICLES_RANDOMPOSITION_Y, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("randompositionz"), MO_PARAM_FUNCTION, PARTICLES_RANDOMPOSITION_Y, moValue( "0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("randomvelocity"), MO_PARAM_FUNCTION, PARTICLES_RANDOMVELOCITY, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("randomvelocityx"), MO_PARAM_FUNCTION, PARTICLES_RANDOMVELOCITY_X, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("randomvelocityy"), MO_PARAM_FUNCTION, PARTICLES_RANDOMVELOCITY_Y, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("randomvelocityz"), MO_PARAM_FUNCTION, PARTICLES_RANDOMVELOCITY_Z, moValue( "1.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("randommotion"), MO_PARAM_FUNCTION, PARTICLES_RANDOMMOTION, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("randommotionx"), MO_PARAM_FUNCTION, PARTICLES_RANDOMMOTION_X, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("randommotiony"), MO_PARAM_FUNCTION, PARTICLES_RANDOMMOTION_Y, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("randommotionz"), MO_PARAM_FUNCTION, PARTICLES_RANDOMMOTION_Z, moValue( "0.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("rotatex_particle"), MO_PARAM_ROTATEX, PARTICLES_ROTATEX_PARTICLE, moValue( "0.0", "FUNCTION").Ref() );
  p_configdefinition->Add( moText("rotatey_particle"), MO_PARAM_ROTATEY, PARTICLES_ROTATEY_PARTICLE, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatez_particle"), MO_PARAM_ROTATEZ, PARTICLES_ROTATEZ_PARTICLE, moValue( "0.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("scalex_particle"), MO_PARAM_SCALEX, PARTICLES_SCALEX_PARTICLE, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scaley_particle"), MO_PARAM_SCALEY, PARTICLES_SCALEY_PARTICLE, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scalez_particle"), MO_PARAM_SCALEZ, PARTICLES_SCALEZ_PARTICLE, moValue( "1.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("time_to_revelation"), MO_PARAM_NUMERIC, PARTICLES_TIMETOREVELATION, moValue( "2000", "NUM") );
	p_configdefinition->Add( moText("time_of_revelation"), MO_PARAM_NUMERIC, PARTICLES_TIMEOFREVELATION, moValue( "3000", "NUM") );
	p_configdefinition->Add( moText("time_to_restoration"), MO_PARAM_NUMERIC, PARTICLES_TIMETORESTORATION, moValue( "2000", "NUM") );
	p_configdefinition->Add( moText("time_of_restoration"), MO_PARAM_NUMERIC, PARTICLES_TIMEOFRESTORATION, moValue( "3000", "NUM") );
	p_configdefinition->Add( moText("drawing_features"), MO_PARAM_NUMERIC, PARTICLES_DRAWINGFEATURES, moValue( "0", "NUM"), moText("NO,YES") );

	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, PARTICLES_TRANSLATEX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, PARTICLES_TRANSLATEY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, PARTICLES_TRANSLATEZ, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, PARTICLES_ROTATEX, moValue( "0.0", "FUNCTION").Ref() );
  p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, PARTICLES_ROTATEY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, PARTICLES_ROTATEZ, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, PARTICLES_SCALEX, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, PARTICLES_SCALEY, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, PARTICLES_SCALEZ, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("eyex"), MO_PARAM_FUNCTION, PARTICLES_EYEX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("eyey"), MO_PARAM_FUNCTION, PARTICLES_EYEY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("eyez"), MO_PARAM_FUNCTION, PARTICLES_EYEZ, moValue( "10.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("viewx"), MO_PARAM_FUNCTION, PARTICLES_VIEWX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("viewy"), MO_PARAM_FUNCTION, PARTICLES_VIEWY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("viewz"), MO_PARAM_FUNCTION, PARTICLES_VIEWZ, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("upviewx"), MO_PARAM_FUNCTION, PARTICLES_UPVIEWX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("upviewy"), MO_PARAM_FUNCTION, PARTICLES_UPVIEWY, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("upviewz"), MO_PARAM_FUNCTION, PARTICLES_UPVIEWZ, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("orderingmode"), MO_PARAM_NUMERIC, PARTICLES_ORDERING_MODE, moValue( "0", "NUM"), moText("NONE,ZDEPTHTEST,ZPOSITION,COMPLETE") );
	p_configdefinition->Add( moText("lightmode"), MO_PARAM_NUMERIC, PARTICLES_LIGHTMODE, moValue( "0", "NUM") );
	p_configdefinition->Add( moText("lightx"), MO_PARAM_FUNCTION, PARTICLES_LIGHTX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("lighty"), MO_PARAM_FUNCTION, PARTICLES_LIGHTY, moValue( "4.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("lightz"), MO_PARAM_FUNCTION, PARTICLES_LIGHTZ, moValue( "0.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("deltax_particle"), MO_PARAM_TRANSLATEX, PARTICLES_DELTAX_PARTICLE, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("deltay_particle"), MO_PARAM_TRANSLATEY, PARTICLES_DELTAY_PARTICLE, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("deltaz_particle"), MO_PARAM_TRANSLATEZ, PARTICLES_DELTAZ_PARTICLE, moValue( "0.0", "FUNCTION").Ref() );
return p_configdefinition;
}

MOboolean
moEffectParticlesSimple::Init()
{

  ///IMPORTANT: add inlets before PreInit so inlets name are availables for function variables!!
  m_pParticleTime = new moInlet();

  if (m_pParticleTime) {
    ((moConnector*)m_pParticleTime)->Init( moText("particletime"), m_Inlets.Count(), MO_DATA_NUMBER_DOUBLE );
    m_Inlets.Add(m_pParticleTime);
  }

  m_pParticleIndex = new moInlet();

  if (m_pParticleIndex) {
    ((moConnector*)m_pParticleIndex)->Init( moText("particleindex"), m_Inlets.Count(), MO_DATA_NUMBER_LONG );
    m_Inlets.Add(m_pParticleIndex);
  }

	m_pParticleIndexNormal = new moInlet();

  if (m_pParticleIndexNormal) {
    ((moConnector*)m_pParticleIndexNormal)->Init( moText("particleindexnormal"), m_Inlets.Count(), MO_DATA_NUMBER_DOUBLE );
    m_Inlets.Add(m_pParticleIndexNormal);
  }

  m_pParticleIndexCol = new moInlet();

  if (m_pParticleIndexCol) {
    ((moConnector*)m_pParticleIndexCol)->Init( moText("particlei"), m_Inlets.Count(), MO_DATA_NUMBER_LONG );
    m_Inlets.Add(m_pParticleIndexCol);
  }

  m_pParticleIndexRow = new moInlet();

  if (m_pParticleIndexRow) {
    ((moConnector*)m_pParticleIndexRow)->Init( moText("particlej"), m_Inlets.Count(), MO_DATA_NUMBER_LONG );
    m_Inlets.Add(m_pParticleIndexRow);
  }

	m_pParticleIndexX = new moInlet();

  if (m_pParticleIndexX) {
    ((moConnector*)m_pParticleIndexX)->Init( moText("particlex"), m_Inlets.Count(), MO_DATA_NUMBER_DOUBLE );
    m_Inlets.Add(m_pParticleIndexX);
  }

  m_pParticleIndexY = new moInlet();

  if (m_pParticleIndexY) {
    ((moConnector*)m_pParticleIndexY)->Init( moText("particley"), m_Inlets.Count(), MO_DATA_NUMBER_DOUBLE );
    m_Inlets.Add(m_pParticleIndexY);
  }

  if (!PreInit()) return false;

  if (glActiveTextureARB) {
  } else {
    glewInit();
    //MODebug2->Message( moText("moEffectParticlesSimple::Init >       glActiveTextureARB: ") + moText(IntToStr((int)glActiveTextureARB)) );
    //MODebug2->Message( moText("moEffectParticlesSimple::Init >       glMultiTexCoord2fARB: ") + moText(IntToStr((int)glMultiTexCoord2fARB)) );
  }

	moDefineParamIndex( PARTICLES_INLET, moText("inlet") );
	moDefineParamIndex( PARTICLES_OUTLET, moText("outlet") );
  moDefineParamIndex( PARTICLES_SCRIPT, moText("script") );

	moDefineParamIndex( PARTICLES_ALPHA, moText("alpha") );
	moDefineParamIndex( PARTICLES_COLOR, moText("color") );
	moDefineParamIndex( PARTICLES_SYNC, moText("syncro") );
	moDefineParamIndex( PARTICLES_PHASE, moText("phase") );
	moDefineParamIndex( PARTICLES_PARTICLECOLOR, moText("particlecolor") );
	moDefineParamIndex( PARTICLES_FONT, moText("font") );
	moDefineParamIndex( PARTICLES_TEXT, moText("text") );
	moDefineParamIndex( PARTICLES_ORTHO, moText("ortho") );


	moDefineParamIndex( PARTICLES_TEXTURE, moText("texture") );
	moDefineParamIndex( PARTICLES_FOLDERS, moText("folders") );
	moDefineParamIndex( PARTICLES_TEXTUREMODE, moText("texture_mode") );
	moDefineParamIndex( PARTICLES_BLENDING, moText("blending") );

	moDefineParamIndex( PARTICLES_WIDTH, moText("width") );
	moDefineParamIndex( PARTICLES_HEIGHT, moText("height") );
	moDefineParamIndex( PARTICLES_SIZEX, moText("sizex") );
	moDefineParamIndex( PARTICLES_SIZEY, moText("sizey") );
	moDefineParamIndex( PARTICLES_SIZEZ, moText("sizez") );

	moDefineParamIndex( PARTICLES_GRAVITY, moText("gravity") );
	moDefineParamIndex( PARTICLES_VISCOSITY, moText("viscosity") );

	moDefineParamIndex( PARTICLES_MAXAGE, moText("maxage") );
	moDefineParamIndex( PARTICLES_EMITIONPERIOD, moText("emitionperiod") );
	moDefineParamIndex( PARTICLES_EMITIONRATE, moText("emitionrate") );
	moDefineParamIndex( PARTICLES_DEATHPERIOD, moText("deathperiod") );
	moDefineParamIndex( PARTICLES_SCRIPT2, moText("particlescript") );

	moDefineParamIndex( PARTICLES_FADEIN, moText("fadein") );
	moDefineParamIndex( PARTICLES_FADEOUT, moText("fadeout") );
	moDefineParamIndex( PARTICLES_SIZEIN, moText("sizein") );
	moDefineParamIndex( PARTICLES_SIZEOUT, moText("sizeout") );

	moDefineParamIndex( PARTICLES_RANDOMMETHOD, moText("randommethod") );
	moDefineParamIndex( PARTICLES_CREATIONMETHOD, moText("creationmethod") );
	moDefineParamIndex( PARTICLES_ORIENTATIONMODE, moText("orientationmode") );

	moDefineParamIndex( PARTICLES_RANDOMPOSITION, moText("randomposition") );
	moDefineParamIndex( PARTICLES_RANDOMPOSITION_X, moText("randompositionx") );
	moDefineParamIndex( PARTICLES_RANDOMPOSITION_Y, moText("randompositiony") );
	moDefineParamIndex( PARTICLES_RANDOMPOSITION_Z, moText("randompositionz") );

	moDefineParamIndex( PARTICLES_RANDOMVELOCITY, moText("randomvelocity") );
	moDefineParamIndex( PARTICLES_RANDOMVELOCITY_X, moText("randomvelocityx") );
	moDefineParamIndex( PARTICLES_RANDOMVELOCITY_Y, moText("randomvelocityy") );
	moDefineParamIndex( PARTICLES_RANDOMVELOCITY_Z, moText("randomvelocityz") );

	moDefineParamIndex( PARTICLES_RANDOMMOTION, moText("randommotion") );
	moDefineParamIndex( PARTICLES_RANDOMMOTION_X, moText("randommotionx") );
	moDefineParamIndex( PARTICLES_RANDOMMOTION_Y, moText("randommotiony") );
	moDefineParamIndex( PARTICLES_RANDOMMOTION_Z, moText("randommotionz") );

	moDefineParamIndex( PARTICLES_EMITTERTYPE, moText("emittertype") );
	moDefineParamIndex( PARTICLES_EMITTERVECTOR_X, moText("emittervectorx") );
	moDefineParamIndex( PARTICLES_EMITTERVECTOR_Y, moText("emittervectory") );
	moDefineParamIndex( PARTICLES_EMITTERVECTOR_Z, moText("emittervectorz") );

	moDefineParamIndex( PARTICLES_ATTRACTORTYPE, moText("attractortype") );
	moDefineParamIndex( PARTICLES_ATTRACTORMODE, moText("attractormode") );
	moDefineParamIndex( PARTICLES_ATTRACTORVECTOR_X, moText("attractorvectorx") );
	moDefineParamIndex( PARTICLES_ATTRACTORVECTOR_Y, moText("attractorvectory") );
	moDefineParamIndex( PARTICLES_ATTRACTORVECTOR_Z, moText("attractorvectorz") );


	moDefineParamIndex( PARTICLES_SCALEX_PARTICLE, moText("scalex_particle") );
	moDefineParamIndex( PARTICLES_SCALEY_PARTICLE, moText("scaley_particle") );
	moDefineParamIndex( PARTICLES_SCALEZ_PARTICLE, moText("scalez_particle") );
	moDefineParamIndex( PARTICLES_ROTATEX_PARTICLE, moText("rotatex_particle") );
	moDefineParamIndex( PARTICLES_ROTATEY_PARTICLE, moText("rotatey_particle") );
	moDefineParamIndex( PARTICLES_ROTATEZ_PARTICLE, moText("rotatez_particle") );

	moDefineParamIndex( PARTICLES_TIMETOREVELATION, moText("time_to_revelation") );
	moDefineParamIndex( PARTICLES_TIMEOFREVELATION, moText("time_of_revelation") );
	moDefineParamIndex( PARTICLES_TIMETORESTORATION, moText("time_to_restoration") );
	moDefineParamIndex( PARTICLES_TIMEOFRESTORATION, moText("time_of_restoration") );
	moDefineParamIndex( PARTICLES_DRAWINGFEATURES, moText("drawing_features") );

	moDefineParamIndex( PARTICLES_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( PARTICLES_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( PARTICLES_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( PARTICLES_SCALEX, moText("scalex") );
	moDefineParamIndex( PARTICLES_SCALEY, moText("scaley") );
	moDefineParamIndex( PARTICLES_SCALEZ, moText("scalez") );
	moDefineParamIndex( PARTICLES_ROTATEX, moText("rotatex") );
	moDefineParamIndex( PARTICLES_ROTATEY, moText("rotatey") );
	moDefineParamIndex( PARTICLES_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( PARTICLES_EYEX, moText("eyex") );
	moDefineParamIndex( PARTICLES_EYEY, moText("eyey") );
	moDefineParamIndex( PARTICLES_EYEZ, moText("eyez") );
	moDefineParamIndex( PARTICLES_VIEWX, moText("viewx") );
	moDefineParamIndex( PARTICLES_VIEWY, moText("viewy") );
	moDefineParamIndex( PARTICLES_VIEWZ, moText("viewz") );
	moDefineParamIndex( PARTICLES_UPVIEWX, moText("upviewx") );
	moDefineParamIndex( PARTICLES_UPVIEWY, moText("upviewy") );
	moDefineParamIndex( PARTICLES_UPVIEWZ, moText("upviewz") );
	moDefineParamIndex( PARTICLES_ORDERING_MODE, moText("orderingmode") );
	moDefineParamIndex( PARTICLES_LIGHTMODE, moText("lightmode") );
	moDefineParamIndex( PARTICLES_LIGHTX, moText("lightx") );
	moDefineParamIndex( PARTICLES_LIGHTY, moText("lighty") );
	moDefineParamIndex( PARTICLES_LIGHTZ, moText("lightz") );

	moDefineParamIndex( PARTICLES_DELTAX_PARTICLE, moText("deltax_particle") );
	moDefineParamIndex( PARTICLES_DELTAY_PARTICLE, moText("deltay_particle") );
	moDefineParamIndex( PARTICLES_DELTAZ_PARTICLE, moText("deltaz_particle") );

    m_Physics.m_ParticleScript = moText("");

    m_Rate = 0;
    last_tick = 0;
    frame = 0;

    ortho = false;

    m_bTrackerInit = false;
    m_pTrackerData = NULL;

    UpdateParameters();

    ResetTimers();

    InitParticlesSimple(  m_Config.Int( moR(PARTICLES_WIDTH) ),
                          m_Config.Int( moR(PARTICLES_HEIGHT)) );


    pTextureDest = NULL;
    pSubSample = NULL;
    samplebuffer = NULL;

    glidori  = 0;
    glid = 0;
    original_width = 0;
    original_height = 0;
    original_proportion = 1.0;

    midi_red = midi_green = midi_blue = 1.0;
    midi_maxage = 1.0; //in millis
    midi_emitionperiod = 1.0;//in millisec
    midi_emitionrate = 1.0; // n per emitionperiod
    midi_randomvelocity = 1.0; //inicial vel
    midi_randommotion = 1.0; //motion dynamic
    #ifdef USE_TUIO
    m_InletTuioSystemIndex = GetInletIndex("TUIOSYSTEM");
    #endif
    m_InletTrackerSystemIndex = GetInletIndex("TRACKERKLT");

	return true;

}


void moEffectParticlesSimple::ResetTimers() {

    TimerFullRevelation.Stop();
    TimerFullRestoration.Stop();
    TimerOfRevelation.Stop();
    TimerOfRestoration.Stop();

    FeatureActivity.Stop();
    MotionActivity.Stop();
    NoMotionActivity.Stop();
    m_Physics.EmitionTimer.Stop();

      for ( int i=0; i < m_ParticlesSimpleArray.Count(); i++ ) {
            moParticlesSimple* pPar = m_ParticlesSimpleArray[i];
            pPar->Age.Stop();
            pPar->Age.SetRelativeTimer( (moTimerAbsolute*)&m_EffectState.tempo );
            pPar->Visible = false;
      }

  m_Physics.m_pLastBordParticle = NULL;
}

void moEffectParticlesSimple::Shot( moText source, int shot_type, int shot_file ) {

    MODebug2->Push(moText("moEffectParticlesSimple::Shot: Shot activated!!!!")+ (moText)source);

    moTextureBuffer* pTexBuf = NULL;

    pTextureDest = NULL;

    if (pSubSample && samplebuffer) {
        delete [] samplebuffer;
        pSubSample = NULL;
        samplebuffer = NULL;
    }

    glid = 0;
    glidori = 0;

    if (texture_mode==PARTICLES_TEXTUREMODE_MANY2PATCH) {



        ///primero toma la imagen que esta cargada: hay que generar una mas chica!!!!
        ///......

        if ( source != moText("")) {
            //agregado:
            //version banco de datos
            moTextureBuffer* pTexBufAltas = NULL;
            int ichosen;

            int idid = m_pResourceManager->GetTextureMan()->GetTextureBuffer(moText( source ),true);
            if (idid>-1) {
                pTexBufAltas = m_pResourceManager->GetTextureMan()->GetTextureBuffer(idid);
                pTexBufAltas->UpdateImages(1);
            }
            if (pTexBufAltas) {
                pTexBufAltas->UpdateImages(1);
                int np = pTexBufAltas->GetImagesProcessed();
                if (np>0) {
                    if (shot_type==0) {
                        /** Aleatorio*/
                        ichosen = (int)( moMathf::UnitRandom() * np );
                    } else {
                        /** Secuencial (ultimo ingresado)*/
                        if (shot_file<0) {
                            ichosen = np - 1;
                        } else {
                            ichosen = shot_file;
                        }
                    }

                    glidori = pTexBufAltas->GetFrame(ichosen);
                    glid = glidori;
                    if (glidori>0) {
                        pTextureDest = (moTexture*) pTexBufAltas->GetTexture(ichosen);
                    }
                } else MODebug2->Error(  moText("no images processed yet"));
            }

        } else {
            glid = m_Config.GetGLId( moR(PARTICLES_TEXTURE), &m_EffectState.tempo );
            glidori = glid;
            //MODebug2->Error(  moText(" no texture buffer"));
            //version self texture (LIVEIN0 o imagen)
            if (glid>0) pTextureDest = m_Config[moR(PARTICLES_TEXTURE)][MO_SELECTED][0].Texture();
        }


        ///m_pTextureManager->GetBitmap( glid );
        if (pTextureDest) {


            ///levantar la textura!!! del thumbnail: y su buffer!!!
            time_t rawtime;
            char strbuffer[0x100];
            struct tm * timeinfo;

            srand(2);
            int randcode = rand();

            time(&rawtime);
            timeinfo = localtime ( &rawtime );
            strftime (strbuffer,80,"%Y-%m-%d-%H-%M-",timeinfo);


            moText datetime = strbuffer;
            datetime+= IntToStr(randcode);

            moText alta;
            moText baja;
            moText extension;

            original_width = pTextureDest->GetWidth();
            original_height = pTextureDest->GetHeight();
            if (original_height>0) {
                original_proportion = (float)original_width / (float) original_height;
            } else original_proportion = 1.0f;

            alta = (this->GetLabelName()) + moText("ALTA");
            baja = (this->GetLabelName()) + moText("BAJA");
            extension = moText(".png");

            moText destalta;
            moText destbaja;
            moText copyalta;
            destalta = m_pResourceManager->GetDataMan()->GetDataPath() + (moText)alta;
            destbaja = m_pResourceManager->GetDataMan()->GetDataPath() + (moText)baja;
            copyalta = m_pResourceManager->GetDataMan()->GetDataPath() + moText("cams/") + (moText)datetime + (moText)alta;

            pTextureDest->CreateThumbnail( "PNG", pTextureDest->GetWidth(), pTextureDest->GetHeight(), destalta  );
            pTextureDest->CreateThumbnail( "PNG", m_cols, m_rows, destbaja  );

            if (source==moText("")) m_pResourceManager->GetFileMan()->CopyFile( destalta+(moText)extension , copyalta+(moText)extension );

            int idori = m_pResourceManager->GetTextureMan()->GetTextureMOId( alta+(moText)extension, false );
            if (idori>-1) {
                m_pResourceManager->GetTextureMan()->GetTexture(idori)->BuildFromFile( destalta+(moText)extension);
            } else idori = m_pResourceManager->GetTextureMan()->AddTexture( alta+(moText)extension );

            int idti = m_pResourceManager->GetTextureMan()->GetTextureMOId( baja+(moText)extension, false );
            if (idti>-1) {
                m_pResourceManager->GetTextureMan()->GetTexture(idti)->BuildFromFile( destbaja+(moText)extension );
            } else idti = m_pResourceManager->GetTextureMan()->AddTexture( baja+(moText)extension );

            if (idori>-1) glidori = m_pResourceManager->GetTextureMan()->GetTexture(idori)->GetGLId();

            //BAJA FOR SUBSAMPLING
            if (idti>-1) {
                    pSubSample = m_pResourceManager->GetTextureMan()->GetTexture(idti);
                    if (pSubSample) {
                        glid = pSubSample->GetGLId();
                        samplebuffer = new MObyte [ pSubSample->GetWidth() * pSubSample->GetHeight() * 3];

                        pSubSample->GetBuffer( samplebuffer, GL_RGB, GL_UNSIGNED_BYTE );

                    }
            }
        }
    }


}

void moEffectParticlesSimple::ReInit() {

    MODebug2->Push(moText("moEffectParticlesSimple::ReInit Face construction activated!!!!"));

    int i;
    int j;
    int lum = 0;
    int lumindex = 0;
    int contrast = 0;

    UpdateParameters();
    //ResetTimers();

    m_pResourceManager->GetTimeMan()->ClearByObjectId(  this->GetId() );


    //m_ParticlesSimpleArray.Init( p_cols*p_rows, NULL );
    //m_ParticlesSimpleArrayTmp.Init( p_cols*p_rows, NULL );

    for( i=0; i<m_cols ; i++) {
        for( j=0; j<m_rows ; j++) {
            moParticlesSimple* pPar = m_ParticlesSimpleArray[i+j*m_cols];

            if (pPar) {

                pPar->Pos = moVector2f( (float) i, (float) j);
                pPar->ImageProportion = 1.0;
                pPar->Color = moVector3f(1.0,1.0,1.0);
                pPar->GLId2 = 0;

                if (texture_mode==PARTICLES_TEXTUREMODE_MANY2PATCH) {

                    ///take the texture preselected
                    moTextureBuffer* pTexBuf = m_Config[moR(PARTICLES_FOLDERS)][MO_SELECTED][0].TextureBuffer();

                    pPar->GLId = glidori;
                    pPar->GLId2 = glid;
                    //pPar->GLId2 = 0;

                    pPar->TCoord2 = moVector2f( 0.0, 0.0 );
                    pPar->TSize2 = moVector2f( 1.0f, 1.0f );

                    pPar->TCoord = moVector2f( (float) (i ) / (float) m_cols, (float) (j) / (float) m_rows );
                    pPar->TSize = moVector2f( 1.0f / (float) m_cols, 1.0f / (float) m_rows );

                    ///calculamos la luminancia del cuadro correspondiente
                    //int x0, y0, x1, y1;
                    float lumf = 0.0;

                    if (pSubSample && samplebuffer) {

                        if (pSubSample->GetWidth()!=m_cols) MODebug2->Error(moText("pSubSample width doesnt match m_cols"));
                        if (pSubSample->GetHeight()!=m_rows) MODebug2->Error(moText("pSubSample height doesnt match m_rows"));

                        int r = samplebuffer[ (i + j*pSubSample->GetWidth() ) *3 ];
                        int g = samplebuffer[ (i+1 + j*pSubSample->GetWidth() ) *3 ];
                        int b = samplebuffer[ (i+2 + j*pSubSample->GetWidth() ) *3 ];
                        //MODebug2->Message(moText("r:")+IntToStr(r)+moText(" g:")+IntToStr(g)+moText(" b:")+IntToStr(b));

                        lum = (r+g+b)/3;

                        //lum = ( + samplebuffer[ (i+1 + j*pSubSample->GetWidth() ) *3 ] + samplebuffer[ (i+2 + j*pSubSample->GetWidth() ) *3 ] ) / 3;
                        lum = ((lum & 1) << 7) | ((lum & 2) << 5) | ((lum & 4) << 3) | ((lum & 8) << 1) | ((lum & 16) >> 1) | ((lum & 32) >> 3) | ((lum & 64) >> 5) | ((lum & 128) >> 7);
                        if (lum<0) lum = 0;
                        //MODebug2->Message(moText("lum:")+IntToStr(lum));

                        if (lum>=0) {
                            lum = lum*1.2;

                            lumf = ( 100.0 * (float)lum ) / (float)255.0;
                            lumindex = (int) lumf;
                            if (lumindex>99) lumindex = 99;
                            //MODebug2->Push( moText("## Lum Index R G B ##") +IntToStr(lum)+IntToStr(r)+IntToStr(g)+IntToStr(b));

                        } else {
                            MODebug2->Message(moText("ReInit error:## lum is negative!!! ##")+IntToStr(lum)
                                    +moText("subs: w:") + IntToStr(pSubSample->GetWidth())
                                    +moText("subs: h:") + IntToStr(pSubSample->GetHeight())
                            );
                            lumindex = 99;
                        }

                    } else MODebug2->Push(moText("ReInit error: no texture nor samplebuffer"));


                     if (pTexBuf && pTextureDest && samplebuffer) {

                         int nim = pTexBuf->GetImagesProcessed();

                         pPar->ImageProportion = 1.0;

                         if (nim>0) {

                             moTextureFrames& pTextFrames(pTexBuf->GetBufferLevels( lumindex, 0 ) );

                             int nc = pTextFrames.Count();
                             int irandom = -1;

                             irandom = (int)( moMathf::UnitRandom() * (double)nc );
                             //irandom = 0;

                            moTextureMemory* pTexMem = pTextFrames.GetRef( irandom );

                            if (pTexMem) {
                                pPar->GLId = glidori;
                                pTexMem->GetReference();
                                pPar->GLId2 = pTexMem->GetGLId();
                                pPar->pTextureMemory = pTexMem;
                                if (pTexMem->GetHeight()>0) pPar->ImageProportion = (float) pTexMem->GetWidth() / (float) pTexMem->GetHeight();
                            } else {
                                #ifdef _DEBUG
                                MODebug2->Message(moText("Sample not founded: lumindex:")+IntToStr(lumindex)+moText(" irandom:")+IntToStr(irandom));
                                #endif
                                pPar->GLId = glidori;
                                pPar->GLId2 = pPar->GLId;
                                pPar->Color.X() = ((float)lum )/ 255.0f;
                                pPar->Color.Y() = ((float)lum )/ 255.0f;
                                pPar->Color.Z() = ((float)lum )/ 255.0f;
                                pPar->Color.X()*= pPar->Color.X();
                                pPar->Color.Y()*= pPar->Color.Y();
                                pPar->Color.Z()*= pPar->Color.Z();
                            }

                            //MODebug2->Push( moText("creating particle: irandom:") + IntToStr(irandom) + moText(" nc:") + IntToStr(nc) + moText(" count:") + IntToStr(pTexBuf->GetImagesProcessed()) + moText(" glid:") + IntToStr(pPar->GLId) + moText(" glid2:") + IntToStr(pPar->GLId2) );

                         }

                     } else {
                         MODebug2->Error( moText("particles error creating texture") );
                     }


                     //MODebug2->Log( moText("i:") + IntToStr(i) + moText(" J:") + IntToStr(j) + moText(" lum:") + IntToStr(lum) + moText(" lumindex:") + IntToStr(lumindex) + moText(" glid:") + IntToStr(pPar->GLId) + moText(" glid2:") + IntToStr(pPar->GLId2));

                }


                pPar->Size = moVector2f( m_Physics.m_EmitterSize.X() / (float) m_cols, m_Physics.m_EmitterSize.Y() / (float) m_rows );
                pPar->Force = moVector3f( 0.0f, 0.0f, 0.0f );

                SetParticlePosition( pPar );

                if (m_Physics.m_EmitionPeriod>0) {
                    pPar->Age.Stop();
                    pPar->Visible = false;
                } else {
                    pPar->Age.Stop();
                    pPar->Age.Start();
                    pPar->Visible = true;
                }

                ///Set Timer management
                pPar->Age.SetObjectId( this->GetId() );
                pPar->Age.SetTimerId( i + j*m_cols );
                m_pResourceManager->GetTimeMan()->AddTimer( &pPar->Age );

                m_ParticlesSimpleArray.Set( i + j*m_cols, pPar );

                moParticlesSimple* pParTmp = m_ParticlesSimpleArrayTmp[i + j*m_cols];
                pParTmp->Pos3d = pPar->Pos3d;
                pParTmp->Velocity = pPar->Velocity;
                pParTmp->Mass = pPar->Mass;
                pParTmp->Force = pPar->Force;
                pParTmp->Fixed = pPar->Fixed;
                m_ParticlesSimpleArrayTmp.Set( i + j*m_cols, pParTmp );
/*
                MODebug2->Log( moText("Pos:") + IntToStr(i) + moText(" J:") + IntToStr(j) + moText(" lum:") + IntToStr(lum) + moText(" lumindex:") + IntToStr(lumindex) + moText(" glid:") + IntToStr(pPar->GLId) + moText(" glid2:") + IntToStr(pPar->GLId2));
                MODebug2->Log( moText("X:") + FloatToStr(pPar->Pos3d.X()) + moText(" Y:") + FloatToStr(pPar->Pos3d.Y()) + moText(" Z:") + FloatToStr(pPar->Pos3d.Z()) );
                MODebug2->Log( moText("VX:") + FloatToStr(pPar->Velocity.X()) + moText(" VY:") + FloatToStr(pPar->Velocity.Y()) + moText(" VZ:") + FloatToStr(pPar->Velocity.Z()) );
                MODebug2->Log( moText("FX:") + FloatToStr(pPar->Force.X()) + moText(" FY:") + FloatToStr(pPar->Force.Y()) + moText(" FZ:") + FloatToStr(pPar->Force.Z()) );
                MODebug2->Log( moText("Size X:") + FloatToStr(pPar->Size.X()) + moText(" Size Y:") + FloatToStr(pPar->Size.Y()) );
                MODebug2->Log( moText("TCoord X:") + FloatToStr(pPar->TCoord.X()) + moText(" TCoord Y:") + FloatToStr(pPar->TCoord.Y()) );
                MODebug2->Log( moText("TCoord2 X:") + FloatToStr(pPar->TCoord2.X()) + moText(" TCoord2 Y:") + FloatToStr(pPar->TCoord2.Y()) );
                MODebug2->Log( moText("Visible:") + IntToStr((int)pPar->Visible) );
                MODebug2->Log( moText("Age:") + IntToStr((int)pPar->Age.Duration()) );
                */

            } else MODebug2->Error(moText("ParticleSimple::ReInit::no particle pointer"));
        }
    }

    //TrackParticle(1);

}

void moEffectParticlesSimple::UpdateDt() {

    ///CONSTANTE
    ///version dt = constante por cuadro
    //double dt = m_Config.Eval( moR(PARTICLES_SYNC),m_EffectState.tempo.ang) * (double)(m_EffectState.tempo.delta) /  (double)100.0;


    ///VARIABLE
    ///relativo al timecode...
    ///dt relativo al tempo (variable)
    /// entre 0 y inf.
    /// 0 significa que casi no hubo lapso de tiempo medible entre el cuadro anterior y este, por lo tanto no hay animacion
    /// 1 significa que el lapso de tiempo entre el evento anterior y el actual coincide con 1 cuadro cada 16.6 ms ( 60 fps )
    /// > 1 significa que el tiempo entre 2 cuadros supero el correspondiente a 60 fps
    /// > 4 significa que estamos por debajo de los 15 fps, pasada esta brecha deberiamos iterar N veces para obtener resultados correctos.
    //
/*
    dtrel = (double) ( gral_ticks - last_tick ) / (double)16.666666;
            //if ( ( (last_tick/100) % 50 ) == 0 ) MODebug2->Push("dtrel:"+FloatToStr(dtrel));
    dt = m_Config.Eval( moR(PARTICLES_SYNC),m_EffectState.tempo.ang) * dtrel * (double)(m_EffectState.tempo.delta) /  (double)100.0;

    if ( ( (last_tick/100) % 50 ) == 0 ) MODebug2->Push("dt:"+FloatToStr(dt));

    last_tick = gral_ticks;
    */
    /// 60 FPS = 16.666 milliseconds
    /// dtrel is frame relative where if dtrel = 1 = 1 frame (60fps)
    dtrel = (double) ( m_EffectState.tempo.ticks - last_tick ) / (double)16.666666;

    //if ( ( (last_tick/100) % 50 ) == 0 ) MODebug2->Push("dtrel:"+FloatToStr(dtrel));

    dt = m_Config.Eval( moR(PARTICLES_SYNC)) * dtrel * (double)(m_EffectState.tempo.delta) /  (double)100.0;

    last_tick = m_EffectState.tempo.ticks;

}

void moEffectParticlesSimple::UpdateParameters() {


    this->UpdateDt();  // now in ::Update()

    time_tofull_restoration = m_Config.Int( moR(PARTICLES_TIMETORESTORATION) );
    time_of_restoration = m_Config.Int( moR(PARTICLES_TIMEOFRESTORATION) );

    time_tofull_revelation = m_Config.Int( moR(PARTICLES_TIMETOREVELATION));
    time_of_revelation = m_Config.Int( moR(PARTICLES_TIMEOFREVELATION) );

    ortho = (bool) m_Config.Int( moR(PARTICLES_ORTHO) );

    if ( moIsTimerStopped() || !m_EffectState.tempo.Started() ) {
        ResetTimers();
        //MODebug2->Message("moEffectParticlesSimple::UpdateParameters  > ResetTimers!!!");
    }

    //if script is modified... recompile
	if ((moText)m_Physics.m_ParticleScript!=m_Config.Text( moR(PARTICLES_SCRIPT2) ) ) {

        m_Physics.m_ParticleScript = m_Config.Text( moR(PARTICLES_SCRIPT2) );
        moText fullscript = m_pResourceManager->GetDataMan()->GetDataPath()+ moSlash + (moText)m_Physics.m_ParticleScript;

        if ( CompileFile(fullscript) ) {

            MODebug2->Message(moText("ParticlesSimple script loaded ") + (moText)fullscript );

            SelectScriptFunction( "Init" );
            //AddFunctionParam( m_FramesPerSecond );
            RunSelectedFunction();

        } else MODebug2->Error(moText("ParticlesSimple couldnt compile lua script ") + (moText)fullscript );
	}

    if (moScript::IsInitialized()) {
        if (ScriptHasFunction("RunSystem")) {
            SelectScriptFunction("RunSystem");
            //passing number of particles
            AddFunctionParam( (int) ( m_rows*m_cols ) );
            AddFunctionParam( (float) dt );
            RunSelectedFunction(1);
        }
    }

    drawing_features = m_Config.Int( moR(PARTICLES_DRAWINGFEATURES));
    texture_mode = m_Config.Int( moR(PARTICLES_TEXTUREMODE));

    m_Physics.m_EyeVector = moVector3f(
                                        m_Config.Eval( moR(PARTICLES_EYEX)),
                                        m_Config.Eval( moR(PARTICLES_EYEY)),
                                        m_Config.Eval( moR(PARTICLES_EYEZ))
                                       );

    m_Physics.m_TargetViewVector = moVector3f(
                                        m_Config.Eval( moR(PARTICLES_VIEWX)),
                                        m_Config.Eval( moR(PARTICLES_VIEWY)),
                                        m_Config.Eval( moR(PARTICLES_VIEWZ))
                                       );

    m_Physics.m_UpViewVector = moVector3f(
                                        m_Config.Eval( moR(PARTICLES_UPVIEWX)),
                                        m_Config.Eval( moR(PARTICLES_UPVIEWY)),
                                        m_Config.Eval( moR(PARTICLES_UPVIEWZ))
                                       );

    m_Physics.m_SourceLighMode = (moParticlesSimpleLightMode) m_Config.Int( moR(PARTICLES_LIGHTMODE));
    m_Physics.m_SourceLightVector = moVector3f(
                                        m_Config.Eval( moR(PARTICLES_LIGHTX)),
                                        m_Config.Eval( moR(PARTICLES_LIGHTY)),
                                        m_Config.Eval( moR(PARTICLES_LIGHTZ))
                                       );

    m_Physics.gravitational = m_Config.Eval( moR(PARTICLES_GRAVITY));
    m_Physics.viscousdrag = m_Config.Eval( moR(PARTICLES_VISCOSITY));


    /*
    m_Physics.m_MaxAge = (int)((float)(m_Config[moR(PARTICLES_MAXAGE)][MO_SELECTED][0].Int()) * midi_maxage);
    m_Physics.m_EmitionPeriod = (int)((float)(m_Config[moR(PARTICLES_EMITIONPERIOD)][MO_SELECTED][0].Int()) * midi_emitionperiod);
    m_Physics.m_EmitionRate = (int)((float)(m_Config[moR(PARTICLES_EMITIONRATE)][MO_SELECTED][0].Int()) * midi_emitionrate);
    m_Physics.m_DeathPeriod = m_Config[moR(PARTICLES_DEATHPERIOD)][MO_SELECTED][0].Float();
*/

    //emiper = (float)m_Config[moR(PARTICLES_EMITIONPERIOD)][MO_SELECTED][0].Int();
    //emiper = emiper * midi_emitionperiod;
    //emiperi = (long) emiper;
    //MODebug2->Message(moText("Emiper:")+IntToStr(emiperi));

    //m_Physics.m_MaxAge = m_Config.Int( moR(PARTICLES_MAXAGE) );
    m_Physics.m_MaxAge = (long) m_Config.Eval( moR(PARTICLES_MAXAGE) );
    //m_Physics.m_EmitionPeriod = emiperi;
    //emiperi = m_Config[moR(PARTICLES_EMITIONPERIOD)][MO_SELECTED][0].Int() * midi_emitionperiod;
    //m_Physics.m_EmitionPeriod = emiperi;
    m_Physics.m_EmitionPeriod = (long) m_Config.Eval( moR(PARTICLES_EMITIONPERIOD) );
    //m_Physics.m_EmitionPeriod = m_Config[moR(PARTICLES_EMITIONPERIOD)][MO_SELECTED][0].Int();
    //MODebug2->Message(moText("Emiperiod:")+IntToStr(m_Physics.m_EmitionPeriod));

    //m_Physics.m_EmitionRate = m_Config.Int( moR(PARTICLES_EMITIONRATE) );
    m_Physics.m_EmitionRate = (long) m_Config.Eval( moR(PARTICLES_EMITIONRATE) );
    m_Physics.m_DeathPeriod = m_Config.Int( moR(PARTICLES_DEATHPERIOD) );


    m_Physics.m_RandomMethod = (moParticlesRandomMethod) m_Config.Int( moR(PARTICLES_RANDOMMETHOD) );
    m_Physics.m_CreationMethod = (moParticlesCreationMethod) m_Config.Int( moR(PARTICLES_CREATIONMETHOD) );

    m_Physics.m_OrientationMode = (moParticlesOrientationMode) m_Config.Int( moR(PARTICLES_ORIENTATIONMODE) );

    m_Physics.m_FadeIn = m_Config.Eval( moR(PARTICLES_FADEIN));
    m_Physics.m_FadeOut = m_Config.Eval( moR(PARTICLES_FADEOUT));
    m_Physics.m_SizeIn = m_Config.Eval( moR(PARTICLES_SIZEIN));
    m_Physics.m_SizeOut = m_Config.Eval( moR(PARTICLES_SIZEOUT));


    /*
    m_Physics.m_RandomPosition = m_Config[moR(PARTICLES_RANDOMPOSITION)].GetData()->Fun()->Eval(m_EffectState.tempo.ang);
    m_Physics.m_RandomVelocity = m_Config[moR(PARTICLES_RANDOMVELOCITY)].GetData()->Fun()->Eval(m_EffectState.tempo.ang) * midi_randomvelocity;
    m_Physics.m_RandomMotion = m_Config[moR(PARTICLES_RANDOMMOTION)].GetData()->Fun()->Eval(m_EffectState.tempo.ang) * midi_randommotion;
    */
    m_Physics.m_RandomPosition = m_Config.Eval( moR(PARTICLES_RANDOMPOSITION));
    m_Physics.m_RandomVelocity = m_Config.Eval( moR(PARTICLES_RANDOMVELOCITY));
    m_Physics.m_RandomMotion = m_Config.Eval( moR(PARTICLES_RANDOMMOTION));


    m_Physics.m_EmitterType = (moParticlesSimpleEmitterType) m_Config.Int( moR(PARTICLES_EMITTERTYPE));
    m_Physics.m_AttractorType = (moParticlesSimpleAttractorType) m_Config.Int( moR(PARTICLES_ATTRACTORTYPE));

    m_Physics.m_PositionVector = moVector3f(m_Config.Eval( moR(PARTICLES_RANDOMPOSITION_X)),
                                            m_Config.Eval( moR(PARTICLES_RANDOMPOSITION_Y)),
                                            m_Config.Eval( moR(PARTICLES_RANDOMPOSITION_Z)) );

    m_Physics.m_EmitterSize = moVector3f(   m_Config.Eval( moR(PARTICLES_SIZEX)),
                                            m_Config.Eval( moR(PARTICLES_SIZEY)),
                                            m_Config.Eval( moR(PARTICLES_SIZEZ)));

    m_Physics.m_VelocityVector =  moVector3f( m_Config.Eval( moR(PARTICLES_RANDOMVELOCITY_X)),
                                            m_Config.Eval( moR(PARTICLES_RANDOMVELOCITY_Y)),
                                            m_Config.Eval( moR(PARTICLES_RANDOMVELOCITY_Z)));

    m_Physics.m_MotionVector =  moVector3f( m_Config.Eval( moR(PARTICLES_RANDOMMOTION_X)),
                                            m_Config.Eval( moR(PARTICLES_RANDOMMOTION_Y)),
                                            m_Config.Eval( moR(PARTICLES_RANDOMMOTION_Z)));

    m_Physics.m_EmitterVector = moVector3f( m_Config.Eval( moR(PARTICLES_EMITTERVECTOR_X)),
                                            m_Config.Eval( moR(PARTICLES_EMITTERVECTOR_Y)),
                                            m_Config.Eval( moR(PARTICLES_EMITTERVECTOR_Z)));

    if (m_bTrackerInit && m_Physics.m_EmitterType==PARTICLES_EMITTERTYPE_TRACKER2) {
        m_Physics.m_EmitterVector = moVector3f( m_TrackerBarycenter.X()*normalf, m_TrackerBarycenter.Y()*normalf, 0.0f );
    }

    m_Physics.m_AttractorMode = (moParticlesSimpleAttractorMode) m_Config.Int( moR(PARTICLES_ATTRACTORMODE));
    m_Physics.m_AttractorVector = moVector3f( m_Config.Eval( moR(PARTICLES_ATTRACTORVECTOR_X)),
                                            m_Config.Eval( moR(PARTICLES_ATTRACTORVECTOR_Y)),
                                            m_Config.Eval( moR(PARTICLES_ATTRACTORVECTOR_Z)));

    if (original_proportion!=1.0f) {
            if (original_proportion>1.0f) {
                m_Physics.m_AttractorVector.Y() = m_Physics.m_AttractorVector.Y() / original_proportion;
            } else {
                m_Physics.m_AttractorVector.X() = m_Physics.m_AttractorVector.X() * original_proportion;
            }
    }

    normalf = m_Physics.m_EmitterSize.X();

    m_OrderingMode = (moParticlesOrderingMode) m_Config.Int( moR(PARTICLES_ORDERING_MODE) );

    float ralpha,rbeta,rgama;

    ralpha = moMathf::DegToRad( m_Config.Eval( moR(PARTICLES_ROTATEX) ) );
    rbeta = moMathf::DegToRad( m_Config.Eval( moR(PARTICLES_ROTATEY) ) );
    rgama = moMathf::DegToRad( m_Config.Eval( moR(PARTICLES_ROTATEZ) ) );

    float r01 = cos(rbeta)*cos(rgama);
    float r02 = cos(rgama)*sin(ralpha)*sin(rbeta) - cos(ralpha)*sin(rgama);
    float r03 = cos(ralpha)*cos(rgama)*sin(rbeta)+sin(ralpha)*sin(rgama);
    float r04 = 0;

    float r11 = cos(rbeta)*sin(rgama);
    float r12 = cos(ralpha)*cos(rgama) + sin(ralpha)*sin(rbeta)*sin(rgama);
    float r13 = -cos(rgama)*sin(ralpha) + cos(ralpha)*sin(rbeta)*sin(rgama);
    float r14 = 0;

    float r21 = -sin(rbeta);
    float r22 = cos(rbeta)*sin(ralpha);
    float r23 = cos(ralpha)*cos(rbeta);
    float r24 = 0;

    m_Rot[0] = moVector4f(  r01, r02, r03, r04 );
    m_Rot[1] = moVector4f(  r11, r12, r13, r14 );
    m_Rot[2] = moVector4f(  r21, r22, r23, r24 );
    m_Rot[3] = moVector4f(  0, 0, 0, 1 );


    m_TS[0] = moVector4f(  m_Config.Eval( moR(PARTICLES_SCALEX) ), 0, 0, m_Config.Eval( moR(PARTICLES_TRANSLATEX) ) );
    m_TS[1] = moVector4f(  0, m_Config.Eval( moR(PARTICLES_SCALEY) ), 0, m_Config.Eval( moR(PARTICLES_TRANSLATEY) ) );
    m_TS[2] = moVector4f(  0, 0, m_Config.Eval( moR(PARTICLES_SCALEZ) ), m_Config.Eval( moR(PARTICLES_TRANSLATEZ) ) );
    m_TS[3] = moVector4f(  0, 0, 0, 1 );
/*
    MODebug2->Message(
" maxage: " + IntToStr(m_Physics.m_MaxAge) + "\n"+
" emitionperiod: " + IntToStr(m_Physics.m_EmitionPeriod) + "\n"+
" emitionrate: " + IntToStr(m_Physics.m_EmitionRate) + "\n"+
" emittervector x: " + FloatToStr(m_Physics.m_EmitterVector.X())
                      + " y: " + FloatToStr(m_Physics.m_EmitterVector.Y(),2,2) +
                      + " z: " + FloatToStr(m_Physics.m_EmitterVector.Z(),2,2) + "\n"+
" emittersize x: " + FloatToStr(m_Physics.m_EmitterSize.X(),2,2)
                      + " y: " + FloatToStr(m_Physics.m_EmitterSize.Y(),2,2) +
                      + " z: " + FloatToStr(m_Physics.m_EmitterSize.Z(),2,2) + "\n"+
" randommethod: " + IntToStr(m_Physics.m_RandomMethod) + "\n"+
" creationmethod: " + IntToStr(m_Physics.m_CreationMethod) + "\n"

                                           );
                                          */
}

void moEffectParticlesSimple::SetInletParticles( moParticlesSimple* pParticle ) {
	if (m_pParticleIndex) {
		if (m_pParticleIndex->GetData()) {
				m_pParticleIndex->GetData()->SetLong( ((long)pParticle->Pos.X()) + ((long)pParticle->Pos.Y())*m_cols );
				m_pParticleIndex->Update(true);
		}
	}

	if (m_pParticleIndexNormal) {
		if (m_pParticleIndexNormal->GetData()) {
				m_pParticleIndexNormal->GetData()->SetDouble( ( pParticle->Pos.X() + pParticle->Pos.Y()*(double)m_cols ) / (double)m_total );
				m_pParticleIndexNormal->Update(true);
		}
	}

	if (m_pParticleIndexCol) {
		if (m_pParticleIndexCol->GetData()) {
				m_pParticleIndexCol->GetData()->SetLong( (long)pParticle->Pos.X() );
				m_pParticleIndexCol->Update(true);
		}
	}

	if (m_pParticleIndexRow) {
		if (m_pParticleIndexRow->GetData()) {
				m_pParticleIndexRow->GetData()->SetLong( (long)pParticle->Pos.Y() );
				m_pParticleIndexRow->Update(true);
		}
	}

	if (m_pParticleIndexX) {
		if (m_pParticleIndexX->GetData()) {
				m_pParticleIndexX->GetData()->SetDouble( (double)pParticle->Pos3d.X() );
				m_pParticleIndexX->Update(true);
		}
	}

	if (m_pParticleIndexY) {
		if (m_pParticleIndexY->GetData()) {
				m_pParticleIndexY->GetData()->SetDouble( (long)pParticle->Pos3d.Y() );
				m_pParticleIndexY->Update(true);
		}
	}
}

void moEffectParticlesSimple::SetParticlePosition( moParticlesSimple* pParticle ) {

    float left =  - (m_Physics.m_EmitterSize.X()) / 2.0;
    float top =  m_Physics.m_EmitterSize.Y() / 2.0;
    float randomvelx = 0;
    float randomvely = 0;
    float randomvelz = 0;
    float randomposx = 0;
    float randomposy = 0;
    float randomposz = 0;
    double alpha;
    double phi;
    double radius;
    double z;
    double radius1;
    double radius2;

    double len=0,index=0,index_normal=0;

		index = pParticle->Pos.X() + pParticle->Pos.Y()*(double)m_cols;
		index_normal = 0.0; ///si no hay particulas siempre en 0
		if (m_total>0) {
				index_normal = index / (double)(m_total);
		}
		pParticle->ParticleIndex = index;
		pParticle->ParticleIndexNormal = index_normal;
		SetInletParticles(pParticle);

    randomposx = ( fabs(m_Physics.m_RandomPosition) >0.0)? (0.5-moMathf::UnitRandom())*m_Physics.m_RandomPosition*m_Physics.m_PositionVector.X() : m_Physics.m_PositionVector.X();
    randomposy = ( fabs(m_Physics.m_RandomPosition) >0.0)? (0.5-moMathf::UnitRandom())*m_Physics.m_RandomPosition*m_Physics.m_PositionVector.Y() : m_Physics.m_PositionVector.Y();
    randomposz = ( fabs(m_Physics.m_RandomPosition) >0.0)? (0.5-moMathf::UnitRandom())*m_Physics.m_RandomPosition*m_Physics.m_PositionVector.Z() : m_Physics.m_PositionVector.Z();

    randomvelx = ( fabs(m_Physics.m_RandomVelocity) >0.0)? (moMathf::UnitRandom())*m_Physics.m_RandomVelocity*m_Physics.m_VelocityVector.X() : m_Physics.m_VelocityVector.X();
    randomvely = ( fabs(m_Physics.m_RandomVelocity) >0.0)? (moMathf::UnitRandom())*m_Physics.m_RandomVelocity*m_Physics.m_VelocityVector.Y() : m_Physics.m_VelocityVector.Y();
    randomvelz = ( fabs(m_Physics.m_RandomVelocity) >0.0)? (moMathf::UnitRandom())*m_Physics.m_RandomVelocity*m_Physics.m_VelocityVector.Z() : m_Physics.m_VelocityVector.Z();

    moVector4d fullcolor;
    fullcolor = m_Config.EvalColor( moR(PARTICLES_PARTICLECOLOR));
    pParticle->Color = moVector3f(
                              fullcolor.X(),
                              fullcolor.Y(),
                              fullcolor.Z() );

    pParticle->Mass = 10.0f;
    pParticle->Fixed = false;


    pParticle->U = moVector3f( 1.0, 0.0, 0.0 );
    pParticle->V = moVector3f( 0.0, 1.0, 0.0 );
    pParticle->W = moVector3f( 0.0, 0.0, 1.0 );

    pParticle->dpdt = moVector3f( 0.0f, 0.0f, 0.0f );
    pParticle->dvdt = moVector3f( 0.0f, 0.0f, 0.0f );

    if (m_Physics.m_FadeIn>0.0) pParticle->Alpha = 0.0;///fade in ? to middle age?
    else pParticle->Alpha = fullcolor.W();

    if (m_Physics.m_SizeIn>0.0) pParticle->Scale = 0.0;///fade in ? to middle age?
    else pParticle->Scale = 1.0;

    switch(m_Physics.m_EmitterType) {

        case PARTICLES_EMITTERTYPE_GRID:
            //GRID POSITION
           switch(m_Physics.m_CreationMethod) {
                case PARTICLES_CREATIONMETHOD_LINEAR:
                    pParticle->Pos3d = moVector3f(   ( left + pParticle->Pos.X()*pParticle->Size.X() + pParticle->Size.X()*randomposx/2.0 )*m_Physics.m_EmitterVector.X() ,
                                                     ( top - pParticle->Pos.Y()*pParticle->Size.Y() - pParticle->Size.Y()*randomposy/2.0 )*m_Physics.m_EmitterVector.Y(),
                                                    randomposz*m_Physics.m_EmitterVector.Z() );
                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;

                case PARTICLES_CREATIONMETHOD_PLANAR:
                case PARTICLES_CREATIONMETHOD_VOLUMETRIC:
                    pParticle->Pos3d = moVector3f(   ( left + moMathf::UnitRandom()*m_Physics.m_EmitterSize.X() + pParticle->Size.X()*randomposx/2.0 )*m_Physics.m_EmitterVector.X() ,
                                                     ( top - moMathf::UnitRandom()*m_Physics.m_EmitterSize.Y() - pParticle->Size.Y()*randomposy/2.0 )*m_Physics.m_EmitterVector.Y(),
                                                    randomposz*m_Physics.m_EmitterVector.Z() );
                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;
            }

            break;

        case PARTICLES_EMITTERTYPE_SPHERE:
            //SPHERE POSITION
            switch(m_Physics.m_CreationMethod) {
                case PARTICLES_CREATIONMETHOD_LINEAR:
                    alpha = 2 * moMathf::PI * pParticle->Pos.X() / (double)m_cols;
                    phi = moMathf::PI * pParticle->Pos.Y() / (double)m_rows;
                    radius = moMathf::Sqrt( m_Physics.m_EmitterSize.X()*m_Physics.m_EmitterSize.X()+m_Physics.m_EmitterSize.Y()*m_Physics.m_EmitterSize.Y()) / 2.0;

                    pParticle->Pos3d = moVector3f(  (radius*moMathf::Cos(alpha)*moMathf::Sin(phi) + randomposx ) * m_Physics.m_EmitterVector.X(),
                                                    (radius*moMathf::Sin(alpha)*moMathf::Sin(phi) + randomposy ) * m_Physics.m_EmitterVector.Y(),
                                                    (radius*moMathf::Cos(phi) + randomposz ) * m_Physics.m_EmitterVector.Z() );

                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;

                case  PARTICLES_CREATIONMETHOD_PLANAR:
                    alpha = 2 * (moMathf::PI) * moMathf::UnitRandom();
                    phi = moMathf::PI * moMathf::UnitRandom();
                    radius = moMathf::Sqrt( m_Physics.m_EmitterSize.X()*m_Physics.m_EmitterSize.X()+m_Physics.m_EmitterSize.Y()*m_Physics.m_EmitterSize.Y()) / 2.0;
                    pParticle->Pos3d = moVector3f(
                                        (radius*moMathf::Cos(alpha)*moMathf::Sin(phi) + randomposx)* m_Physics.m_EmitterVector.X(),
                                        (radius*moMathf::Sin(alpha)*moMathf::Sin(phi) + randomposy)* m_Physics.m_EmitterVector.Y(),
                                        (radius*moMathf::Cos(phi) + randomposz)* m_Physics.m_EmitterVector.Z()
                                        );
                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;

                case  PARTICLES_CREATIONMETHOD_VOLUMETRIC:
                    alpha = 2 * moMathf::PI * moMathf::UnitRandom();
                    phi = moMathf::PI * moMathf::UnitRandom();
                    radius = moMathf::Sqrt( m_Physics.m_EmitterSize.X()*m_Physics.m_EmitterSize.X()+m_Physics.m_EmitterSize.Y()*m_Physics.m_EmitterSize.Y())*moMathf::UnitRandom() / 2.0;

                    pParticle->Pos3d = moVector3f(  (radius*moMathf::Cos(alpha)*moMathf::Sin(phi) + randomposx ) * m_Physics.m_EmitterVector.X(),
                                                    (radius*moMathf::Sin(alpha)*moMathf::Sin(phi) + randomposy ) * m_Physics.m_EmitterVector.Y(),
                                                    (radius*moMathf::Cos(phi) + randomposz ) * m_Physics.m_EmitterVector.Z() );

                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;
            }
            break;

        case PARTICLES_EMITTERTYPE_TUBE:
            //SPHERE POSITION
            switch(m_Physics.m_CreationMethod) {
                case PARTICLES_CREATIONMETHOD_LINEAR:
                    alpha = 2 * moMathf::PI * pParticle->Pos.X() / (double)m_cols;
                    radius1 = m_Physics.m_EmitterSize.X() / 2.0;
                    radius2 = m_Physics.m_EmitterSize.Y() / 2.0;
                    z = m_Physics.m_EmitterSize.Z() * ( 0.5f - ( pParticle->Pos.Y() / (double)m_rows ) );

                    pParticle->Pos3d = moVector3f(  ( radius1*moMathf::Cos(alpha) + randomposx ) * m_Physics.m_EmitterVector.X(),
                                                    ( radius1*moMathf::Sin(alpha) + randomposy ) * m_Physics.m_EmitterVector.Y(),
                                                    ( z + randomposz ) * m_Physics.m_EmitterVector.Z() );

                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;

                case  PARTICLES_CREATIONMETHOD_PLANAR:
                    alpha = 2 * moMathf::PI * moMathf::UnitRandom();
                    radius1 = m_Physics.m_EmitterSize.X() / 2.0;
                    radius2 = m_Physics.m_EmitterSize.Y() / 2.0;
                    z = m_Physics.m_EmitterSize.Z() * ( 0.5f - moMathf::UnitRandom());

                    pParticle->Pos3d = moVector3f(  ( radius1*moMathf::Cos(alpha) + randomposx ) * m_Physics.m_EmitterVector.X(),
                                                    ( radius1*moMathf::Sin(alpha) + randomposy ) * m_Physics.m_EmitterVector.Y(),
                                                    ( z + randomposz ) * m_Physics.m_EmitterVector.Z() );

                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;

                case  PARTICLES_CREATIONMETHOD_VOLUMETRIC:
                    alpha = 2 * moMathf::PI * moMathf::UnitRandom();
                    radius1 = m_Physics.m_EmitterSize.X() / 2.0;
                    radius2 = m_Physics.m_EmitterSize.Y() / 2.0;
                    radius = radius1 + moMathf::UnitRandom()*(radius2-radius1)*moMathf::UnitRandom();
                    z = m_Physics.m_EmitterSize.Z() * ( 0.5f - moMathf::UnitRandom());

                    pParticle->Pos3d = moVector3f(  ( radius*moMathf::Cos(alpha) + randomposx ) * m_Physics.m_EmitterVector.X(),
                                                    ( radius*moMathf::Sin(alpha) + randomposy ) * m_Physics.m_EmitterVector.Y(),
                                                    ( z + randomposz ) * m_Physics.m_EmitterVector.Z() );

                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;
            }
            break;

        case PARTICLES_EMITTERTYPE_JET:
            //SPHERE POSITION
            switch(m_Physics.m_CreationMethod) {
                case PARTICLES_CREATIONMETHOD_LINEAR:

                    //z = m_Physics.m_EmitterSize.Z() * moMathf::UnitRandom();
                    len = m_Physics.m_EmitterVector.Length();
                    z = index_normal;

                    pParticle->Pos3d = moVector3f(  m_Physics.m_EmitterVector.X()*( z + randomposx ),
                                                    m_Physics.m_EmitterVector.Y()*( z + randomposy ),
                                                    m_Physics.m_EmitterVector.Z()*( z + randomposz) );

                    pParticle->Velocity = moVector3f(   randomvelx,
                                                        randomvely,
                                                        randomvelz);
                    break;
                case PARTICLES_CREATIONMETHOD_PLANAR:
                case PARTICLES_CREATIONMETHOD_VOLUMETRIC:
                    z = m_Physics.m_EmitterSize.Z() * moMathf::UnitRandom();

                    pParticle->Pos3d = moVector3f(  m_Physics.m_EmitterVector.X()*( z + randomposx ),
                                                    m_Physics.m_EmitterVector.Y()*( z + randomposy ),
                                                    m_Physics.m_EmitterVector.Z()*( z + randomposz) );

                    pParticle->Velocity = moVector3f(   randomvelx,
                                                        randomvely,
                                                        randomvelz);
                    break;

            }
            break;

        case PARTICLES_EMITTERTYPE_POINT:
            //SPHERE POSITION
            pParticle->Pos3d = moVector3f(  randomposx+m_Physics.m_EmitterVector.X(),
                                            randomposy+m_Physics.m_EmitterVector.Y(),
                                            randomposz+m_Physics.m_EmitterVector.Z() );

            pParticle->Velocity = moVector3f(   randomvelx,
                                                randomvely,
                                                randomvelz);

            break;
        case PARTICLES_EMITTERTYPE_SPIRAL:
            //SPIRAL POSITION
            switch(m_Physics.m_CreationMethod) {
                case PARTICLES_CREATIONMETHOD_LINEAR:
                case  PARTICLES_CREATIONMETHOD_PLANAR:
                case  PARTICLES_CREATIONMETHOD_VOLUMETRIC:
                    alpha = 2 * moMathf::PI * pParticle->Pos.X() / (double)m_cols;
                    radius1 = m_Physics.m_EmitterSize.X() / 2.0;
                    radius2 = m_Physics.m_EmitterSize.Y() / 2.0;
                    z = m_Physics.m_EmitterSize.Z() * ( 0.5f - ( pParticle->Pos.Y() / (double)m_rows ) - (pParticle->Pos.X() / (double)(m_cols*m_rows)) );

                    pParticle->Pos3d = moVector3f(  ( radius1*moMathf::Cos(alpha) + randomposx ) * m_Physics.m_EmitterVector.X(),
                                                    ( radius1*moMathf::Sin(alpha) + randomposy ) * m_Physics.m_EmitterVector.Y(),
                                                    ( z + randomposz ) * m_Physics.m_EmitterVector.Z() );

                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;
            }
            break;
        case PARTICLES_EMITTERTYPE_CIRCLE:
            //CIRCLE POSITION
            switch(m_Physics.m_CreationMethod) {
                case PARTICLES_CREATIONMETHOD_LINEAR:
                    alpha = 2 * moMathf::PI * ( pParticle->Pos.X() + pParticle->Pos.Y()*m_cols ) / ((double)m_cols*(double)m_rows );
                    radius1 = m_Physics.m_EmitterSize.X() / 2.0;
                    radius2 = m_Physics.m_EmitterSize.Y() / 2.0;
                    z = 0.0;
                    //z = m_Physics.m_EmitterSize.Z() * ( 0.5f - ( pParticle->Pos.Y() / (double)m_rows ) - (pParticle->Pos.X() / (double)(m_cols*m_rows)) );

                    pParticle->Pos3d = moVector3f(  ( radius1*moMathf::Cos(alpha) + randomposx ) * m_Physics.m_EmitterVector.X(),
                                                    ( radius1*moMathf::Sin(alpha) + randomposy ) * m_Physics.m_EmitterVector.Y(),
                                                    ( z + randomposz ) );

                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;
                case  PARTICLES_CREATIONMETHOD_PLANAR:
                case  PARTICLES_CREATIONMETHOD_VOLUMETRIC:
                    alpha = 2 * moMathf::PI *  ( pParticle->Pos.X()*m_rows + pParticle->Pos.Y()) / ((double)m_cols*(double)m_rows );
                    radius1 = m_Physics.m_EmitterSize.X() / 2.0;
                    radius2 = m_Physics.m_EmitterSize.Y() / 2.0;
                    z = 0.0;
                    //z = m_Physics.m_EmitterSize.Z() * ( 0.5f - ( pParticle->Pos.Y() / (double)m_rows ) - (pParticle->Pos.X() / (double)(m_cols*m_rows)) );
                    randomposx = randomposx + (radius1-radius2)*moMathf::Cos(alpha);
                    randomposy = randomposy + (radius1-radius2)*moMathf::Sin(alpha);
                    pParticle->Pos3d = moVector3f(  ( radius1*moMathf::Cos(alpha) + randomposx ) * m_Physics.m_EmitterVector.X(),
                                                    ( radius1*moMathf::Sin(alpha) + randomposy ) * m_Physics.m_EmitterVector.Y(),
                                                    ( z + randomposz ) );

                    pParticle->Velocity = moVector3f( randomvelx,
                                                      randomvely,
                                                      randomvelz );
                    break;
            }
            break;

        case PARTICLES_EMITTERTYPE_TRACKER:
            switch(m_Physics.m_CreationMethod) {
                case PARTICLES_CREATIONMETHOD_CENTER:
                    if (m_pTrackerData) {
                        pParticle->Pos3d = moVector3f( (m_pTrackerData->GetBarycenter().X() - 0.5)*normalf, (-m_pTrackerData->GetBarycenter().Y()+0.5)*normalf, 0.0 );
                        pParticle->Pos3d+= moVector3f( randomposx, randomposy, randomposz );
                        pParticle->Velocity = moVector3f( randomvelx, randomvely, randomvelz );
                    }
                    break;

                case PARTICLES_CREATIONMETHOD_LINEAR:
                case PARTICLES_CREATIONMETHOD_PLANAR:
                case PARTICLES_CREATIONMETHOD_VOLUMETRIC:



                    if (m_pTrackerData) {
                        bool bfounded = false;
                        int np =  (int) ( moMathf::UnitRandom() * m_pTrackerData->GetFeaturesCount() );

                        moTrackerFeature *pTF = NULL;

                        pTF = m_pTrackerData->GetFeature( np );
                        if (pTF->valid) {
                            pParticle->Pos3d = moVector3f( (pTF->x - 0.5)*normalf, (-pTF->y+0.5)*normalf, 0.0 );
                            bfounded = true;
                        }

                        np = 0;
                        //como no encontro un feature valido para usar de generador arranca desde el primero....
                        //error, deberia tomar el baricentro.... o tomar al azar otro...
                        /*
                        if (!bfounded) {
                            pParticle->Pos3d = moVector3f( (m_pTrackerData->GetBarycenter().X() - 0.5)*normalf, (-m_pTrackerData->GetBarycenter().Y()+0.5)*normalf, 0.0 );
                        }*/
                        int cn=0;
                        if (!bfounded) {
                            do {
                                pTF = m_pTrackerData->GetFeature( np );
                                if (pTF->valid) {
                                    pParticle->Pos3d = moVector3f( (pTF->x - 0.5)*normalf, (-pTF->y+0.5)*normalf, 0.0 );
                                    bfounded = true;
                                }
                                np =  (int) ( moMathf::UnitRandom() * m_pTrackerData->GetFeaturesCount() );
                                cn++;
                            } while (!pTF->valid && np < m_pTrackerData->GetFeaturesCount() && cn<5 );
                            if (!bfounded) pParticle->Pos3d = moVector3f( (m_pTrackerData->GetBarycenter().X() - 0.5)*normalf, (-m_pTrackerData->GetBarycenter().Y()+0.5)*normalf, 0.0 );
                        }


                    } else {
                        pParticle->Pos3d = moVector3f( 0, 0, 0 );
                    }

                    pParticle->Pos3d+= moVector3f( randomposx, randomposy, randomposz );

                    pParticle->Velocity = moVector3f(   randomvelx,
                                                        randomvely,
                                                        randomvelz);
                    break;

            }
            break;
    };


}

void moEffectParticlesSimple::InitParticlesSimple( int p_cols, int p_rows, bool p_forced ) {

    int i,j;

    bool m_bNewImage = false;

    //Texture Mode MANY2PATCH takes a Shot "texture capture of actual camera"
    if (texture_mode==PARTICLES_TEXTUREMODE_MANY2PATCH) {
        Shot();
    }

    //Reset timers related to this object: one for each particle.
    if (m_pResourceManager){
        if (m_pResourceManager->GetTimeMan()) {
          m_pResourceManager->GetTimeMan()->ClearByObjectId(  this->GetId() );
        }
    }


        if (m_ParticlesSimpleArray.Count()>0) {
            /*for(i=0;i<m_ParticlesSimpleArray.Count();i++) {
                if (m_ParticlesSimpleArray[i]!=NULL) {
                    delete m_ParticlesSimpleArray[i];
                    //m_ParticlesSimple.Set(i, NULL);
                }
            }
            */
            m_ParticlesSimpleArray.Empty();
        }

        if (m_ParticlesSimpleArrayTmp.Count()>0) {
            /*for(i=0;i<m_ParticlesSimpleArrayTmp.Count();i++) {
                if (m_ParticlesSimpleArrayTmp[i]!=NULL) {
                    delete m_ParticlesSimpleArrayTmp[i];
                    //m_ParticlesSimple.Set(i, NULL);
                }
            }
            */
            m_ParticlesSimpleArrayTmp.Empty();
        }
        if (m_ParticlesSimpleArrayOrdered.Count()>0) {
            m_ParticlesSimpleArrayOrdered.Empty();
        }

        if ( !m_ParticlesSimpleVector.empty() ) {
          m_ParticlesSimpleVector.clear();
        }

    m_ParticlesSimpleVector.resize( p_cols*p_rows, NULL );
    m_ParticlesSimpleArrayOrdered.Init( p_cols*p_rows, NULL );
    m_ParticlesSimpleArray.Init( p_cols*p_rows, NULL );
    m_ParticlesSimpleArrayTmp.Init( p_cols*p_rows, NULL );

    int orderindex = 0;
    for( j=0; j<p_rows ; j++) {
            for( i=0; i<p_cols ; i++) {

            moParticlesSimple* pPar = new moParticlesSimple();
            m_ParticlesSimpleVector[orderindex] = pPar;
            orderindex++;

            pPar->ViewDepth = 0.0;
            pPar->Pos = moVector2f( (float) i, (float) j);
            pPar->ImageProportion = 1.0;
            //pPar->Color = moVector3f(1.0,1.0,1.0);
            moVector4d fullcolor;
            fullcolor = m_Config.EvalColor( moR(PARTICLES_PARTICLECOLOR));
            pPar->Color = moVector3f(
                                      fullcolor.X(),
                                      fullcolor.Y(),
                                      fullcolor.Z() );
            pPar->GLId2 = 0;

            if (texture_mode==PARTICLES_TEXTUREMODE_UNIT) {

                pPar->TCoord = moVector2f( 0.0, 0.0 );
                pPar->TSize = moVector2f( 1.0f, 1.0f );

            }
            else if (texture_mode==PARTICLES_TEXTUREMODE_PATCH) {

                pPar->TCoord = moVector2f( (float) (i) / (float) p_cols, (float) (j) / (float) p_rows );
                pPar->TSize = moVector2f( 1.0f / (float) p_cols, 1.0f / (float) p_rows );

            }
            else if (texture_mode==PARTICLES_TEXTUREMODE_MANY || texture_mode==PARTICLES_TEXTUREMODE_MANYBYORDER ) {

                pPar->TCoord = moVector2f( 0.0, 0.0 );
                pPar->TSize = moVector2f( 1.0f, 1.0f );

            }
            else if (texture_mode==PARTICLES_TEXTUREMODE_MANY2PATCH) {

                ///take the texture preselected
                moTextureBuffer* pTexBuf = m_Config[moR(PARTICLES_FOLDERS)][MO_SELECTED][0].TextureBuffer();

                pPar->GLId = glidori; //first id for Image reference to patch (and blend after)
                pPar->GLId2 = glid; //second id for each patch image

                pPar->TCoord2 = moVector2f( 0.0, 0.0 ); //coords for patch
                pPar->TSize2 = moVector2f( 1.0f, 1.0f );

                pPar->TCoord = moVector2f( (float) (i ) / (float) p_cols, (float) (j) / (float) p_rows );//grid
                pPar->TSize = moVector2f( 1.0f / (float) p_cols, 1.0f / (float) p_rows );//sizes

                ///calculamos la luminancia del cuadro correspondiente
                //int x0, y0, x1, y1;
                int lum = 0;
                int lumindex = 0;
                int contrast = 0;

                ///samplebuffer en la posicion i,j ?
                if (pSubSample && samplebuffer) {

                    lum = (samplebuffer[ (i + j*pSubSample->GetWidth() ) *3 ]
                           + samplebuffer[ (i+1 + j*pSubSample->GetWidth() ) *3 ]
                           + samplebuffer[ (i+2 + j*pSubSample->GetWidth() ) *3 ]) / 3;

                    if (lum<0) lum = -lum;
                    ///pasamos de color a porcentaje 0..255 a 0..99
                    if (lum>=0) {
                        lumindex = (int)( 100.0 * (float)lum / (float)256)  - 1;
                        if (lumindex>99) lumindex = 99;
                    }
                } else {
                    MODebug2->Message(moText("pSubSample: ")+IntToStr((long)pSubSample) +
                                    moText("samplebuffer: ")+IntToStr((long)samplebuffer));
                }

                 if (pTexBuf) {

                     ///cantidad de imagenes en el buffer
                     int nim = pTexBuf->GetImagesProcessed();

                     pPar->ImageProportion = 1.0;


                     if (nim>0) {

                        ///Tomamos de GetBufferLevels...

                         moTextureFrames& pTextFrames(pTexBuf->GetBufferLevels( lumindex, 0 ) );

                         int nc = pTextFrames.Count();
                         int irandom = -1;

                         irandom = (int)(  moMathf::UnitRandom() * (double)nc );

                        moTextureMemory* pTexMem = pTextFrames.GetRef( irandom );

                        if (pTexMem) {
                            pPar->GLId = glidori;
                            pTexMem->GetReference();
                            pPar->GLId2 = pTexMem->GetGLId();
                            pPar->pTextureMemory = pTexMem;
                            if (pTexMem->GetHeight()>0) pPar->ImageProportion = (float) pTexMem->GetWidth() / (float) pTexMem->GetHeight();
                        } else {
                            pPar->GLId = glidori;
                            pPar->GLId2 = pPar->GLId;
                            pPar->Color.X() = ((float)lum )/ 255.0f;
                            pPar->Color.Y() = ((float)lum )/ 255.0f;
                            pPar->Color.Z() = ((float)lum )/ 255.0f;
                            pPar->Color.X()*= pPar->Color.X();
                            pPar->Color.Y()*= pPar->Color.Y();
                            pPar->Color.Z()*= pPar->Color.Z();
                        }
                         //MODebug2->Push( moText("creating particle: irandom:") + IntToStr(irandom) + moText(" count:") + IntToStr(pTexBuf->GetImagesProcessed()) + moText(" glid:") + IntToStr(pPar->GLId) );

                     }

                 } else MODebug2->Error( moText("particles error creating texture") );

            }

            pPar->Size = moVector2f( m_Physics.m_EmitterSize.X() / (float) p_cols, m_Physics.m_EmitterSize.Y() / (float) p_rows );
            pPar->Force = moVector3f( 0.0f, 0.0f, 0.0f );

            SetParticlePosition( pPar );

            /*
            MODebug2->Message("i:"+IntToStr(i) );
            MODebug2->Message("j:"+IntToStr(j) );
            MODebug2->Message("px:"+FloatToStr(pPar->Pos3d.X()) );
            MODebug2->Message("py:"+FloatToStr(pPar->Pos3d.Y()) );
            */

            if (m_Physics.m_EmitionPeriod>0) {
                pPar->Age.Stop();
                pPar->Visible = false;
            } else {
                pPar->Age.Start();
                pPar->Visible = true;
            }

            ///Set Timer management
            pPar->Age.SetObjectId( this->GetId() );
            pPar->Age.SetTimerId( i + j*p_cols );
            pPar->Age.SetRelativeTimer( (moTimerAbsolute*)&m_EffectState.tempo );
            m_pResourceManager->GetTimeMan()->AddTimer( &pPar->Age );

            m_ParticlesSimpleArray.Set( i + j*p_cols, pPar );

            moParticlesSimple* pParTmp = new moParticlesSimple();
            pParTmp->Pos3d = pPar->Pos3d;
            pParTmp->Velocity = pPar->Velocity;
            pParTmp->Mass = pPar->Mass;
            pParTmp->Force = pPar->Force;
            pParTmp->Fixed = pPar->Fixed;
            m_ParticlesSimpleArrayTmp.Set( i + j*p_cols, pParTmp );

        }
    }

    m_rows = p_rows;
    m_cols = p_cols;
		m_total = m_cols*m_rows;

}

/// Mata y regenera particulas, tambien las actualiza....
void moEffectParticlesSimple::Regenerate() {

    int i,j;
    float randommotionx,randommotiony,randommotionz;
    long LastImageIndex = -1;

    long emitiontimer_duration = m_Physics.EmitionTimer.Duration();
    //MODebug2->Message("dur:"+IntToStr(emitiontimer_duration));

    /**Reset EmitionTimer if out-of-timeline*/
    if (emitiontimer_duration<0)
        m_Physics.EmitionTimer.Start();


    for( j=0; j<m_rows ; j++) {
      for( i=0; i<m_cols ; i++) {


            moParticlesSimple* pPar = m_ParticlesSimpleArray[i+j*m_cols];

            pPar->pTextureMemory = NULL;

            /** Reset/Kill out-of-timeline particle.... */
            /** Make particle die if particle Age is out of sync with moGetTicks absolute time (kind of reset the particles) */
            if (pPar->Age.Duration() > moGetTicks() ) {
                pPar->Age.Stop();
                pPar->Visible = false;
                pPar->MOId = -1; //reseteamos la textura asociada
                if (pPar->pTextureMemory) {
                    pPar->pTextureMemory->ReleaseReference();
                    pPar->pTextureMemory = NULL;
                    pPar->GLId = 0;
                }
            }


            /**KILL PARTICLE*/

            //if (i==3 && j==3)
              //MODebug2->Message("visible: " + IntToStr((int)pPar->Visible) + "on:" + IntToStr((int)pPar->Age.Started()) + " agedur:" + IntToStr(pPar->Age.Duration()));
            /** Reset/Kill particle if Age > MaxAge , if MaxAge is 0, never die!!!*/
            if ( pPar->Visible) {
                if (
                        (
                                (m_Physics.m_MaxAge>0) &&
                                (pPar->Age.Duration() > m_Physics.m_MaxAge)
                         )
                        ||
                        (
                                (pPar->MaxAge>0) &&
                               (pPar->Age.Duration() > pPar->MaxAge)
                        )
                    ) {

                    pPar->Age.Stop();
                    pPar->Visible = false;
                    pPar->MOId = -1; //reseteamos la textura asociada

                    /** Update rate ??... */
                    if (m_Rate>0) m_Rate--;

                    if (pPar->pTextureMemory) {
                        pPar->pTextureMemory->ReleaseReference();
                        pPar->pTextureMemory = NULL;
                        pPar->GLId = 0;
                    }

                }
            }

            /**REBORN PARTICLE*/
            //m_Physics.EmitionTimer.Duration()
            //MODebug2->Message("dur:"+IntToStr(emitiontimer_duration)+" vs:"+IntToStr(m_Physics.m_EmitionPeriod) );
            /** use m_Physics.EmitionTimer  , create EmitionRate # of particles every EmitionPeriod milliseconds */

            /** Rate is actual particle# emitted in this EmitionPeriod*/
            if ( m_Rate<m_Physics.m_EmitionRate &&
                (m_Physics.EmitionTimer.Duration() > m_Physics.m_EmitionPeriod)
                && pPar->Visible==false ) {

                bool letsborn = true;
                int id_last_particle = 0;
                int this_id_particle = 0;

                if (m_Physics.m_CreationMethod==PARTICLES_CREATIONMETHOD_LINEAR ) {
                  /**in linear creation method, need to track the last particle born, just to maintain linearity*/
                  if (m_Physics.m_pLastBordParticle)
                    id_last_particle = m_Physics.m_pLastBordParticle->Pos.X() + m_Physics.m_pLastBordParticle->Pos.Y()*m_cols;
                  else id_last_particle = -1;

                  this_id_particle = i+j*m_cols;

                  if ( /**last born was last in array */ id_last_particle==(m_rows*m_cols-1)
                       && /**this is first one in array*/ this_id_particle == 0 ) {
                      ///OK
                      letsborn = true;
                  } else if ( this_id_particle == (id_last_particle+1) ) {
                      /** this one is just the next one to the last born, just what we wanted!*/
                      ///OK
                      letsborn = true;
                  } else {
                    /** no linearity, yet */
                    ///wait for the other cycle
                    letsborn = false;
                  }

                }

                //m_Physics.EmitionTimer.Start();
                if (letsborn) {
                  pPar->Visible = true;
                  pPar->Age.Start();

                  //guardamos la referencia a esta particula, que servira para la proxima
                  if (m_Physics.m_pLastBordParticle) {
                    //guardamos la referencia del imageindex de la ultima particula (para mantener el orden en MANYBYORDER)
                    LastImageIndex = m_Physics.m_pLastBordParticle->ImageIndex;
                  }
                  m_Physics.m_pLastBordParticle = pPar;

                  /**Update the rate counter*/
                  m_Rate++;

                  /**Sets the particle position*/
                  SetParticlePosition( pPar );
                  /*
                  MODebug2->Message("frame:"+IntToStr(frame) );
                  MODebug2->Message("i:"+IntToStr(i) );
                  MODebug2->Message("j:"+IntToStr(j) );
                  MODebug2->Message("px:"+FloatToStr(pPar->Pos3d.X()) );
                  MODebug2->Message("py:"+FloatToStr(pPar->Pos3d.Y()) );
                  */

                  /*
                  if ((i+j*m_cols)%1000 == 0) {
                      MODebug2->Push(moText("part�cula en proceso - regenerando GLID ")+IntToStr(pPar->GLId)
                      + moText(" GLID2: ") + IntToStr(pPar->GLId2) );
                  }*/

                  //pPar->Pos3d = moVector3f( 0, 0, 0);

                  //regenerate

                   //moTexture* pTex = ; //m_pResourceManager->GetTextureMan()->GetTexture();

                   /**SPECIAL CASE for texture folders*/
                   ///asigna un id al azar!!!! de todos los que componen el moTextureBuffer
                   ///hay q pedir el moTextureBuffer
                   if ( texture_mode==PARTICLES_TEXTUREMODE_MANY || texture_mode==PARTICLES_TEXTUREMODE_MANYBYORDER ) {
                       moTextureBuffer* pTexBuf = m_Config[ moR(PARTICLES_FOLDERS) ][MO_SELECTED][0].TextureBuffer();
                       //m_Config[moR(PARTICLES_TEXTURE)].GetData()->GetGLId(&m_EffectState.tempo, 1, NULL );
                       if (pTexBuf) {
                           int nim = pTexBuf->GetImagesProcessed();
                           //MODebug2->Push( "nim: " + IntToStr(nim) );

                           pPar->ImageProportion = 1.0;

                           if (nim>0) {

                               float frandom = moMathf::UnitRandom() * nim;

                               //MODebug2->Push( "frandom: " + FloatToStr(frandom) );

                               //int irandom = ( ::rand() * nim )/ RAND_MAX;
                               int irandom = (int)frandom;
                               if (irandom>=nim) irandom = nim - 1;/** last one repeated if out of bound*/

                               if (texture_mode==PARTICLES_TEXTUREMODE_MANYBYORDER) {
                                irandom = LastImageIndex+1;
                                if (irandom>=nim) irandom = 0;
                               }

                               LastImageIndex = irandom;
                               pPar->ImageIndex = LastImageIndex;

                               //MODebug2->Push( "irandom: " + IntToStr(irandom) + " rand: " + IntToStr(::rand()) );

                               pPar->GLId = pTexBuf->GetFrame( irandom );

                               moTextureMemory* pTexMem = pTexBuf->GetTexture( irandom );
                               if (pTexMem) {
                                  pPar->pTextureMemory = pTexMem;
                                  if (pTexMem->GetHeight()>0) pPar->ImageProportion = (float) pTexMem->GetWidth() / (float) pTexMem->GetHeight();
                                }

                               ///MODebug2->Push( moText("creating particle: irandom:") + IntToStr(irandom) + moText(" count:") + IntToStr(pTexBuf->GetImagesProcessed()) + moText(" glid:") + IntToStr(pPar->GLId) );


                           } else {
                               ///pPar->GLId = 0;
                           }
                           pPar->TCoord = moVector2f( 0.0, 0.0 );
                           pPar->TSize = moVector2f( 1.0f, 1.0f );

                       } else MODebug2->Error( moText("PARTICLES_TEXTUREMODE_MANY particles error creating texture") );
                   }

                } ///fin letsborn

            }

            /** Ok, check if EmitionRate is reached*/
            if (m_Rate>=m_Physics.m_EmitionRate) {
                /**reset timer*/
                m_Physics.EmitionTimer.Start();
                m_Rate = 0;
            }

            /**FADEIN*/
            if ( pPar->Visible && m_Physics.m_FadeIn>0.0) {
                /**must be in lifetime range*/
                if ( m_Physics.m_MaxAge>0 &&  pPar->Age.Duration() < m_Physics.m_MaxAge  ) {
                    /**only apply in the first half-lifetime lapsus*/
                    if ( pPar->Age.Duration() < ( m_Physics.m_FadeIn * m_Physics.m_MaxAge / 2.0 ) ) {

                        pPar->Alpha = ( 2.0 * pPar->Age.Duration() / ( m_Physics.m_FadeIn * m_Physics.m_MaxAge ) );

                    } else pPar->Alpha = 1.0 ;
                } else if ( m_Physics.m_MaxAge==0) {
                    /**proportional to particle Age (in seconds) 1 second life = 100% opacity */
                    pPar->Alpha = m_Physics.m_FadeIn * pPar->Age.Duration() / 1000.0;
                }
            }

            /**FADEOUT*/
            if ( pPar->Visible && m_Physics.m_FadeOut>0.0) {
                /**must be in lifetime range*/
                if ( m_Physics.m_MaxAge>0 && (pPar->Age.Duration() < m_Physics.m_MaxAge) ) {
                    /**only apply in the last half-lifetime lapsus*/
                    if ( (m_Physics.m_MaxAge/2.0) < pPar->Age.Duration() ) {
                        if (  (m_Physics.m_FadeOut*m_Physics.m_MaxAge / 2.0) > (m_Physics.m_MaxAge - pPar->Age.Duration()) ) {

                            pPar->Alpha = ( m_Physics.m_MaxAge - pPar->Age.Duration() ) / (m_Physics.m_FadeOut *m_Physics.m_MaxAge / 2.0);

                        }
                    }
                }

            }

            /**SIZEIN*/
            if ( pPar->Visible && m_Physics.m_SizeIn>0.0
                    && (m_Physics.m_MaxAge>0) &&  (pPar->Age.Duration() < m_Physics.m_MaxAge) ) {

                if ( pPar->Age.Duration() < ( m_Physics.m_SizeIn * m_Physics.m_MaxAge / 2.0 )) {

                    pPar->Scale = ( 2.0 * pPar->Age.Duration() / ( m_Physics.m_SizeIn * m_Physics.m_MaxAge ) );

                } else pPar->Scale = 1.0 ;


            }


            /**SIZEOUT*/
            if ( pPar->Visible && m_Physics.m_SizeOut>0.0 && (m_Physics.m_MaxAge>0)
                && ( (m_Physics.m_MaxAge/2.0) < pPar->Age.Duration() ) && (pPar->Age.Duration() < m_Physics.m_MaxAge) ) {

                if (  (m_Physics.m_SizeOut*m_Physics.m_MaxAge / 2.0) > (m_Physics.m_MaxAge - pPar->Age.Duration()) && pPar->Age.Duration() < m_Physics.m_MaxAge ) {

                    pPar->Scale = ( m_Physics.m_MaxAge - pPar->Age.Duration() ) / (m_Physics.m_SizeOut *m_Physics.m_MaxAge / 2.0);

                }


            }

            if ( pPar->Visible ) {

                randommotionx = (m_Physics.m_RandomMotion>0)? (0.5-moMathf::UnitRandom())*m_Physics.m_MotionVector.X() : m_Physics.m_MotionVector.X();
                randommotiony = (m_Physics.m_RandomMotion>0)? (0.5-moMathf::UnitRandom())*m_Physics.m_MotionVector.Y() : m_Physics.m_MotionVector.Y();
                randommotionz = (m_Physics.m_RandomMotion>0)? (0.5-moMathf::UnitRandom())*m_Physics.m_MotionVector.Z() : m_Physics.m_MotionVector.Z();

                m_Physics.m_MotionVector.Normalize();
                if ( m_Physics.m_MotionVector.Length() > 0.0 ) {
                    if (m_Physics.m_RandomMotion>0.0) {
                        pPar->Velocity+= moVector3f( randommotionx, randommotiony, randommotionz ) * m_Physics.m_RandomMotion;
                    }
                }

            }




        }
    }
}

void moEffectParticlesSimple::ParticlesSimpleInfluence( float posx, float posy, float velx, float vely, float veln ) {

/*
    float left =  - normalf / 2.0;
    float top =  normalf / 2.0;
    float sizex = normalf / (float) m_cols;
    float sizey = normalf / (float) m_rows;

    float If,Jf;
    int I,J;
    int II,JJ;

    If = (posx - left - sizex/2.0 ) / sizex;
    Jf = -(posy - top + sizey/2.0 ) / sizey;

    II = (int) If;
    JJ = (int) Jf;


    int Ileft, Iright, Jtop, Jbottom;

    int INFLUENZA = 2;

    if (0<=II && II<m_cols && 0<=JJ && JJ<m_rows) {

        moParticlesSimple* pPar = m_ParticlesSimpleArray.GetRef( II + JJ*m_cols );

        //pBal->Size[0] = pBal->Size[0] / 1.1;
        //pBal->Size[1] = pBal->Size[1] / 1.1;

        pPar->Force = moVector3f( 0.0, 0.0, 1.01f );

        if (INFLUENZA>0) {

            //influence neigbours!!! on a force radius
            Ileft = II - INFLUENZA;
            Iright = II + INFLUENZA;
            Jbottom = JJ - INFLUENZA;
            Jtop = JJ + INFLUENZA;

            ( Ileft > 0 ) ? Ileft = Ileft : Ileft = 0;
            ( Iright < m_cols ) ? Iright = Iright : Iright = m_cols;
            ( Jbottom > 0 ) ? Jbottom = Jbottom : Jbottom = 0;
            ( Jtop < m_rows ) ? Jtop = Jtop : Jtop = m_rows;

            for( I=Ileft; I<Iright; I++) {
                for( J=Jbottom; J<Jtop; J++) {
                    m_ParticlesSimpleArray.GetRef( I + J*m_cols )->Force = moVector3f( 0.0, 0.0, 1.01f );
                }
            }

        }


    }
*/

}

void moEffectParticlesSimple::RevealingAll() {

    float scale = 1.0;
    moVector2f BSize = moVector2f( normalf / (float) m_cols, normalf / (float) m_rows );

    if (!TimerOfRevelation.Started() ) {
        TimerOfRevelation.Start();
    }

    if (TimerOfRevelation.Started() ) {

        if (TimerOfRevelation.Duration() < time_of_revelation ) {
            scale =  (float)( time_of_revelation - TimerOfRevelation.Duration() ) / (float)time_of_revelation;
        } else {
            scale = 0.0;
            TimerOfRevelation.Stop();
        }

        for( int i=0; i<m_cols ; i++) {
            for(int j=0; j<m_rows ; j++) {
                if (m_ParticlesSimpleArray[i+j*m_cols]->Size.X() > 0.1 )
                    m_ParticlesSimpleArray[i+j*m_cols]->Size = BSize * scale;
            }
        }

    }

    ///if full revealed reached:
    if (!TimerOfRevelation.Started() ) {
        revelation_status = PARTICLES_FULLREVEALED;
    }

}

void moEffectParticlesSimple::RestoringAll() {

    float scale = 1.0;
    float left =  - normalf / 2.0;
    float top =  normalf / 2.0;
    moVector2f BSize = moVector2f( normalf / (float) m_cols, normalf / (float) m_rows );

    if (!TimerOfRestoration.Started() ) {
        TimerOfRestoration.Start();
    }

    if (TimerOfRestoration.Started() ) {

        if (TimerOfRestoration.Duration() < time_of_restoration ) {
            scale = 1.0f -  (float)( time_of_restoration - TimerOfRestoration.Duration() ) / (float)time_of_restoration;
        } else {
            scale = 1.0f;
            TimerOfRestoration.Stop();
        }

        for( int i=0; i<m_cols ; i++) {
            for(int j=0; j<m_rows ; j++) {
                m_ParticlesSimpleArray[i+j*m_cols]->Size = BSize * scale;
                m_ParticlesSimpleArray[i+j*m_cols]->Pos3d = moVector3f(   left + (float)(i)*BSize.X() + BSize.X()/2.0,
                                        top - (float)(j)*BSize.Y() - BSize.Y()/2.0,
                                        0.0f );
            }
        }

    }

    ///if full revealed reached:
    if (!TimerOfRestoration.Started() ) {
        revelation_status = PARTICLES_FULLRESTORED;
    }
}

double moEffectParticlesSimple::CalculateViewDepth( moParticlesSimple* pPar ) {

  double viewdepth = 0.0;
  /// TODO: pPar->Pos3d must be transformed to

  moVector3f pos3d_tr;
  moVector4f pos3d_rot;
  moVector4f pos4d;

  //pos3d_trans =
  pos4d = moVector4f( pPar->Pos3d.X(), pPar->Pos3d.Y(), pPar->Pos3d.Z(), 1 );
  pos3d_rot.X() = m_Rot[0].Dot( pos4d );
  pos3d_rot.Y() = m_Rot[1].Dot( pos4d );
  pos3d_rot.Z() = m_Rot[2].Dot( pos4d );
  pos3d_rot.W() = 1.0;

  pos3d_tr.X() = m_TS[0].Dot( pos3d_rot );
  pos3d_tr.Y() = m_TS[1].Dot( pos3d_rot );
  pos3d_tr.Z() = m_TS[2].Dot( pos3d_rot );

  viewdepth = ( m_Physics.m_EyeVector - m_Physics.m_TargetViewVector ).Dot( pos3d_tr );

  return viewdepth;
}


void moEffectParticlesSimple::CalculateForces(bool tmparray)
{
   int i,p1,p2;
   //moVector3f down(1.0,1.0,-1.0);
   moVector3f zero(0.0,0.0,0.0);
   moVector3f f;
   moVector3f atdis;
   double len,dx,dy,dz;


   float left =  - (m_Physics.m_EmitterSize.X()) / 2.0;
    float top =  m_Physics.m_EmitterSize.Y() / 2.0;

   for ( i=0; i < m_ParticlesSimpleArray.Count(); i++ ) {
       moParticlesSimple* pPar = m_ParticlesSimpleArray[i];
      pPar->Force = zero;

      /*if (pPar->Fixed)
         continue;*/

      /* Gravitation */
      switch(m_Physics.m_AttractorType) {
        case PARTICLES_ATTRACTORTYPE_POINT:
            pPar->Force = ( m_Physics.m_AttractorVector - pPar->Pos3d )*(m_Physics.gravitational * pPar->Mass);
            break;

        case PARTICLES_ATTRACTORTYPE_JET:
            {
              //pPar->Force = ( m_Physics.m_AttractorVector - pPar->Pos3d )*(m_Physics.gravitational * pPar->Mass);
              double dot1 = m_Physics.m_AttractorVector.Dot( pPar->Pos3d );
              double det = m_Physics.m_AttractorVector.Length();
              double mu = 0.0;
              if (det>0) {
                  mu = dot1 / det;
              }
              pPar->Force = (m_Physics.m_AttractorVector * mu- pPar->Pos3d )*  (m_Physics.gravitational * pPar->Mass );
            }
            break;

        case PARTICLES_ATTRACTORTYPE_TRACKER:
            if (m_pTrackerData) {
                pPar->Force = ( moVector3f( 0.5f - m_pTrackerData->GetBarycenter().X(), 0.5f - m_pTrackerData->GetBarycenter().Y(), 0.0 ) - pPar->Pos3d )*(m_Physics.gravitational * pPar->Mass);
            }
            break;
        case PARTICLES_ATTRACTORTYPE_GRID:
            switch( m_Physics.m_AttractorMode ) {
                case PARTICLES_ATTRACTORMODE_STICK:
                case PARTICLES_ATTRACTORMODE_ACCELERATION:

                    pPar->Destination = moVector3f(   ( left + pPar->Pos.X()*pPar->Size.X() + pPar->Size.X()/2.0 )*m_Physics.m_AttractorVector.X() ,
                                                     ( top - pPar->Pos.Y()*pPar->Size.Y() - pPar->Size.Y()/2.0 )*m_Physics.m_AttractorVector.Y(),
                                                        m_Physics.m_AttractorVector.Z() );

                    if (m_Physics.m_AttractorMode==PARTICLES_ATTRACTORMODE_STICK && moVector3f( pPar->Destination - pPar->Pos3d ).Length() < 0.1 ) {
                        pPar->Pos3d = pPar->Destination;
                        pPar->Velocity = zero;
                        pPar->Force = zero;
                    } else pPar->Force = ( pPar->Destination - pPar->Pos3d )*(m_Physics.gravitational * pPar->Mass);
                    break;
                case PARTICLES_ATTRACTORMODE_LINEAR:
                    pPar->Destination = moVector3f(   ( left + pPar->Pos.X()*pPar->Size.X() + pPar->Size.X()/2.0 )*m_Physics.m_AttractorVector.X() ,
                                                     ( top - pPar->Pos.Y()*pPar->Size.Y() - pPar->Size.Y()/2.0 )*m_Physics.m_AttractorVector.Y(),
                                                        m_Physics.m_AttractorVector.Z() );

                    pPar->Pos3d = pPar->Pos3d + ( pPar->Destination - pPar->Pos3d) * m_Physics.gravitational;

                    //atdis =( pPar->Destination - pPar->Pos3d);
                    //if ( 0.04 < atdis.Length()  && atdis.Length() < 0.05 )  {
                        //MODebug2->Message( moText("Position reached : X:") + FloatToStr(pPar->Pos3d.X()) + moText(" Y:") + FloatToStr(pPar->Pos3d.Y()) );
                   // }
                    break;
                default:
                    break;
            }
            break;

        }


      /* Viscous drag */
      pPar->Force-= pPar->Velocity*m_Physics.viscousdrag;
   }

   // Handle the spring interaction
   /*
   for (i=0;i<ns;i++) {
      p1 = s[i].from;
      p2 = s[i].to;
      dx = p[p1].p.x - p[p2].p.x;
      dy = p[p1].p.y - p[p2].p.y;
      dz = p[p1].p.z - p[p2].p.z;
      len = sqrt(dx*dx + dy*dy + dz*dz);
      f.x  = s[i].springconstant  * (len - s[i].restlength);
      f.x += s[i].dampingconstant * (p[p1].v.x - p[p2].v.x) * dx / len;
      f.x *= - dx / len;
      f.y  = s[i].springconstant  * (len - s[i].restlength);
      f.y += s[i].dampingconstant * (p[p1].v.y - p[p2].v.y) * dy / len;
      f.y *= - dy / len;
      f.z  = s[i].springconstant  * (len - s[i].restlength);
      f.z += s[i].dampingconstant * (p[p1].v.z - p[p2].v.z) * dz / len;
      f.z *= - dz / len;
      if (!p[p1].fixed) {
         p[p1].f.x += f.x;
         p[p1].f.y += f.y;
         p[p1].f.z += f.z;
      }
      if (!p[p2].fixed) {
         p[p2].f.x -= f.x;
         p[p2].f.y -= f.y;
         p[p2].f.z -= f.z;
      }
   }
 */
}

void moEffectParticlesSimple::UpdateParticles( double dt,int method )
{
   int i;

   switch (method) {
   case 0:
                                      /* Euler */
      Regenerate();
      CalculateForces();
      CalculateDerivatives(false,dt);
      if (dt!=0.0)
      for ( i=0; i<m_ParticlesSimpleArray.Count(); i++ ) {
        moParticlesSimple* pPar = m_ParticlesSimpleArray[i];

        if (pPar && dt!=0.0) {
            pPar->Pos3d+= pPar->dpdt * dt;
            pPar->Velocity+= pPar->dvdt * dt;
        }
      }
      break;
   case 1:                                   /* Midpoint */
      Regenerate();
      CalculateForces();
      CalculateDerivatives(false,dt);
      if (dt!=0.0)
      for (i=0;i<m_ParticlesSimpleArray.Count();i++) {
            moParticlesSimple* pPar = m_ParticlesSimpleArray[i];
            moParticlesSimple* ptmpPar = m_ParticlesSimpleArrayTmp[i];
            if (pPar && ptmpPar &&  dt!=0.0) {
                ptmpPar->Pos3d = pPar->Pos3d;
                ptmpPar->Velocity = pPar->Velocity;
                ptmpPar->Force = pPar->Force;
                ptmpPar->Pos3d = pPar->dpdt * dt / 2;
                ptmpPar->Pos3d+= pPar->dvdt * dt / 2;
            }
      }
      CalculateForces(true);
      CalculateDerivatives(true,dt);
      if (dt!=0.0)
      for ( i=0; i < m_ParticlesSimpleArray.Count(); i++ ) {
            moParticlesSimple* pPar = m_ParticlesSimpleArray[i];
            moParticlesSimple* ptmpPar = m_ParticlesSimpleArrayTmp[i];
            if (pPar && ptmpPar &&  dt!=0.0) {
                pPar->Pos3d+= ptmpPar->dpdt * dt;
                pPar->Velocity += ptmpPar->dvdt * dt;
            }
      }
      break;
   }
}

/*
   Calculate the derivatives
   dp/dt = v
   dv/dt = f / m
*/
void moEffectParticlesSimple::CalculateDerivatives(bool tmparray, double dt)
{
   int i;
   if (tmparray) {
       for ( i=0; i<m_ParticlesSimpleArrayTmp.Count(); i++) {
          if (dt>0) m_ParticlesSimpleArrayTmp[i]->dpdt = m_ParticlesSimpleArrayTmp[i]->Velocity;
          if (dt>0) m_ParticlesSimpleArrayTmp[i]->dvdt = m_ParticlesSimpleArrayTmp[i]->Force * 1.0f / m_ParticlesSimpleArrayTmp[i]->Mass;

       }
   } else {
       for ( i=0; i<m_ParticlesSimpleArray.Count(); i++) {
         if (dt>0) m_ParticlesSimpleArray[i]->dpdt = m_ParticlesSimpleArray[i]->Velocity;

         if (dt>0) m_ParticlesSimpleArray[i]->dvdt = m_ParticlesSimpleArray[i]->Force *  1.0f / m_ParticlesSimpleArray[i]->Mass;

       }
   }
}

/*
   A 1st order 1D DE solver.
   Assumes the function is not time dependent.
   Parameters
      h      - step size
      y0     - last value
      method - algorithm to use [0,5]
      fcn    - evaluate the derivative of the field
*/
double moEffectParticlesSimple::Solver1D(double h,double y0,int method,double (*fcn)(double))
{
   double ynew;
   double k1,k2,k3,k4,k5,k6;

   switch (method) {
   case 0:                          /* Euler method */
      k1 = h * (*fcn)(y0);
      ynew = y0 + k1;
      break;
   case 1:                          /* Modified Euler */
      k1 = h * (*fcn)(y0);
      k2 = h * (*fcn)(y0 + k1);
      ynew = y0 + (k1 + k2) / 2;
      break;
   case 2:                          /* Heuns method */
      k1 = h * (*fcn)(y0);
      k2 = h * (*fcn)(y0 + 2 * k1 / 3);
      ynew = y0 + k1 / 4 + 3 * k2 / 4;
      break;
   case 3:                          /* Midpoint */
      k1 = h * (*fcn)(y0);
      k2 = h * (*fcn)(y0 + k1 / 2);
      ynew = y0 + k2;
      break;
   case 4:                          /* 4'th order Runge-kutta */
      k1 = h * (*fcn)(y0);
      k2 = h * (*fcn)(y0 + k1/2);
      k3 = h * (*fcn)(y0 + k2/2);
      k4 = h * (*fcn)(y0 + k3);
      ynew = y0 + k1 / 6 + k2 / 3 + k3 / 3 + k4 / 6;
      break;
   case 5:                          /* England 4'th order, six stage */
      k1 = h * (*fcn)(y0);
      k2 = h * (*fcn)(y0 + k1 / 2);
      k3 = h * (*fcn)(y0 + (k1 + k2) / 4);
      k4 = h * (*fcn)(y0 - k2 + 2 * k3);
      k5 = h * (*fcn)(y0 + (7 * k1 + 10 * k2 + k4) / 27);
      k6 = h * (*fcn)(y0 + (28*k1 - 125*k2 + 546*k3 + 54*k4 - 378*k5) / 625);
      ynew = y0 + k1 / 6 + 4 * k3 / 6 + k4 / 6;
      break;
   }

   return(ynew);
}


void moEffectParticlesSimple::ParticlesSimpleAnimation( moTempo* tempogral, moEffectState* parentstate ) {

    int i,j;
    int I,J;
    int Ileft, Iright, Jtop, Jbottom;

    switch(revelation_status) {
        case PARTICLES_FULLRESTORED:
            /*
            if (MotionActivity.Started()) {
                if (!TimerFullRevelation.Started()) {
                    TimerFullRevelation.Start();
                    revelation_status = PARTICLES_REVEALING;
                }
            }*/
            break;
        case PARTICLES_FULLREVEALED:
            /*
            if (!TimerFullRestoration.Started()) {
                TimerFullRestoration.Start();
            }

            if (TimerFullRestoration.Duration() > time_tofull_restoration ) {
                TimerFullRestoration.Stop();
                revelation_status = PARTICLES_RESTORINGALL;
            }*/
            break;
        case PARTICLES_REVEALINGALL:
            //RevealingAll();
            break;

        case PARTICLES_RESTORINGALL:
            //RestoringAll();
            break;

        case PARTICLES_REVEALING:
            /*
            if (TimerFullRevelation.Started()) {
                if (TimerFullRevelation.Duration() > time_tofull_revelation ) {
                    TimerFullRevelation.Stop();
                    revelation_status = PARTICLES_REVEALINGALL;
                }
            }
*/
            for( i = 0; i<m_cols ; i++) {
                for( j = 0; j<m_rows ; j++) {

                    moParticlesSimple* pPar = m_ParticlesSimpleArray.GetRef( i + j*m_cols );



                }

            }
            break;

    }


}

void moEffectParticlesSimple::TrackParticle( int partid ) {
    moParticlesSimple* pPar = NULL;
    pPar =  m_ParticlesSimpleArray.GetRef( partid );
    int i = partid;
    int j = 0;
    if (pPar) {
        //MODebug2->Push( moText("Pos:") + IntToStr(i) + moText(" J:") + IntToStr(j) + moText(" lum:") + IntToStr(lum) + moText(" lumindex:") + IntToStr(lumindex) + moText(" glid:") + IntToStr(pPar->GLId) + moText(" glid2:") + IntToStr(pPar->GLId2));
        MODebug2->Push( moText("Pos X:") + FloatToStr(pPar->Pos.X()) + moText(" Pos Y:") + FloatToStr(pPar->Pos.Y()) + moText(" glid:") + IntToStr(pPar->GLId) + moText(" glid2:") + IntToStr(pPar->GLId2));
        MODebug2->Push( moText("X:") + FloatToStr(pPar->Pos3d.X()) + moText(" Y:") + FloatToStr(pPar->Pos3d.Y()) + moText(" Z:") + FloatToStr(pPar->Pos3d.Z()) );
        MODebug2->Push( moText("VX:") + FloatToStr(pPar->Velocity.X()) + moText(" VY:") + FloatToStr(pPar->Velocity.Y()) + moText(" VZ:") + FloatToStr(pPar->Velocity.Z()) );
        MODebug2->Push( moText("FX:") + FloatToStr(pPar->Force.X()) + moText(" FY:") + FloatToStr(pPar->Force.Y()) + moText(" FZ:") + FloatToStr(pPar->Force.Z()) );
        MODebug2->Push( moText("Size X:") + FloatToStr(pPar->Size.X()) + moText(" Size Y:") + FloatToStr(pPar->Size.Y()) );
        MODebug2->Push( moText("TCoord X:") + FloatToStr(pPar->TCoord.X()) + moText(" TCoord Y:") + FloatToStr(pPar->TCoord.Y()) );
        MODebug2->Push( moText("TCoord2 X:") + FloatToStr(pPar->TCoord2.X()) + moText(" TCoord2 Y:") + FloatToStr(pPar->TCoord2.Y()) );
        MODebug2->Push( moText("Visible:") + IntToStr((int)pPar->Visible) );
        MODebug2->Push( moText("Age:") + IntToStr((int)pPar->Age.Duration()) );
    }

}


bool sortParticlesByComplete( moParticlesSimple* a , moParticlesSimple* b ) {
  if (a==NULL && b==NULL) {
      return false;
  }
  if (a==NULL) {
    return false;
  }
  if (b==NULL) {
    return false;
  }
  return ( a->ViewDepth < b->ViewDepth );
}

bool sortParticlesByZCoord( moParticlesSimple* a , moParticlesSimple* b ) {
  if (a==NULL && b==NULL) {
      return false;
  }
  if (a==NULL) {
    return false;
  }
  if (b==NULL) {
    return false;
  }
  return ( a->Pos3d.Z() < b->Pos3d.Z() );
}


void moEffectParticlesSimple::OrderParticles() {

  /// order here or elsewhere
  switch( m_OrderingMode ) {

      case PARTICLES_ORDERING_MODE_COMPLETE:
        sort( m_ParticlesSimpleVector.begin(), m_ParticlesSimpleVector.end(), sortParticlesByComplete );
        break;

      case PARTICLES_ORDERING_MODE_ZDEPTHTEST:
        break;

      case PARTICLES_ORDERING_MODE_ZPOSITION:
        sort( m_ParticlesSimpleVector.begin(), m_ParticlesSimpleVector.end(), sortParticlesByZCoord );
        break;

      case PARTICLES_ORDERING_MODE_NONE:
        break;
      default:
        break;
  }

}

void moEffectParticlesSimple::DrawParticlesSimple( moTempo* tempogral, moEffectState* parentstate ) {

    int i,j;
    int cols2,rows2;




    //if ((moGetTicks() % 1000) == 0) TrackParticle(1);

    cols2 = m_Config.Int( moR(PARTICLES_WIDTH));
    rows2 = m_Config.Int( moR(PARTICLES_HEIGHT) );

    if (cols2!=m_cols || rows2!=m_rows) {
        InitParticlesSimple(cols2,rows2);
    }

    /// TODO: what is this???? gross bug
    /*
    if ( last_tick > tempogral->ticks || tempogral->ticks==0 ) {
        m_Physics.EmitionTimer.Start();
    }
    */

/*
    dtrel = (double) ( tempogral->ticks - last_tick ) / (double)16.666666;
            //if ( ( (last_tick/100) % 50 ) == 0 ) MODebug2->Push("dtrel:"+FloatToStr(dtrel));
    dt = m_Config.Eval( moR(PARTICLES_SYNC),m_EffectState.tempo.ang) * dtrel * (double)(m_EffectState.tempo.delta) /  (double)100.0;

    last_tick = tempogral->ticks;
    */
    gral_ticks  = tempogral->ticks;



    if (!m_Physics.EmitionTimer.Started())
        m_Physics.EmitionTimer.Start();



    //glBindTexture( GL_TEXTURE_2D, 0 );
    if ( texture_mode!=PARTICLES_TEXTUREMODE_MANY && texture_mode!=PARTICLES_TEXTUREMODE_MANY2PATCH ) {
        //glBindTexture( GL_TEXTURE_2D, /*m_Config[moR(PARTICLES_TEXTURE)].GetData()->GetGLId(&m_EffectState.tempo, 1, NULL )*/0 );
        if (glid>=0) glBindTexture( GL_TEXTURE_2D, glid );
        else glBindTexture( GL_TEXTURE_2D, 0);
    }
    //glColor4f(1.0,1.0,1.0,1.0);
    //glDisable( GL_CULL_FACE);
    //glDisable( GL_DEPTH_TEST);
    //glFrontFace( GL_CW);
    //glPolygonMode( GL_LINES, GL_FRONT_AND_BACK);

    //SetBlending( (moBlendingModes) m_Config[moR(PARTICLES_BLENDING)][MO_SELECTED][0].Int() );


    float sizexd2,sizeyd2;
    float tsizex,tsizey;

    moFont* pFont = m_Config[moR(PARTICLES_FONT) ][MO_SELECTED][0].Font();
    moText Texto;

    //Update particle position, velocity, aging, death and rebirth.
    UpdateParticles( dt, 0 ); //Euler mode
    OrderParticles();
    ParticlesSimpleAnimation( tempogral, parentstate );

    float idxt = 0.0;

    //Now really draw each particle
    int orderedindex = 0;
    for( j = 0; j<m_rows ; j++) {
      for( i = 0; i<m_cols ; i++) {

            idxt = 0.5 + (float)( i + j * m_cols ) / (float)( m_cols * m_rows * 2 );

            moParticlesSimple* pPar = m_ParticlesSimpleArray.GetRef( i + j*m_cols );

            switch(m_OrderingMode) {
              case PARTICLES_ORDERING_MODE_NONE:
                break;
              case PARTICLES_ORDERING_MODE_COMPLETE:
                {
                  pPar = m_ParticlesSimpleVector[ orderedindex ];
                  pPar->ViewDepth = CalculateViewDepth( pPar );
                }
                break;
              case PARTICLES_ORDERING_MODE_ZPOSITION:
                pPar = m_ParticlesSimpleVector[ orderedindex ];
                break;
              default:

                break;
            }

            orderedindex+= 1;
            double part_timer;
            if (pPar) {
              if (pPar->Visible) {


                  if (texture_mode==PARTICLES_TEXTUREMODE_MANY || texture_mode==PARTICLES_TEXTUREMODE_MANYBYORDER || texture_mode==PARTICLES_TEXTUREMODE_MANY2PATCH ) {
                      //pPar->GLId = 22;
                      if (pPar->GLId>0) {
                          glActiveTextureARB( GL_TEXTURE0_ARB );
                          glEnable(GL_TEXTURE_2D);
                          //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
                          glBindTexture( GL_TEXTURE_2D, pPar->GLId );
                      }
                      if (pPar->GLId2>0) {
                          glActiveTextureARB( GL_TEXTURE1_ARB );
                          glEnable(GL_TEXTURE_2D);
                          //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                          glBindTexture( GL_TEXTURE_2D, pPar->GLId2 );
                      }
                  }

                  if (moScript::IsInitialized()) {
                      if (ScriptHasFunction("RunParticle")) {
                          SelectScriptFunction("RunParticle");
                          AddFunctionParam( (int) ( i + j*m_cols ) );
                          AddFunctionParam( (float)dt );
                          if (!RunSelectedFunction(1)) {
                              MODebug2->Error( moText("RunParticle function not executed") );
                          }
                      }
                  }

                  sizexd2 = (pPar->Size.X()* pPar->ImageProportion )/2.0;
                  sizeyd2 = pPar->Size.Y()/2.0;
                  tsizex = pPar->TSize.X();
                  tsizey = pPar->TSize.Y();
                  part_timer = 0.001f * (double)(pPar->Age.Duration()); // particule ang = durationinmilis / 1000 ...

                  if (m_pParticleTime) {
                    if (m_pParticleTime->GetData()) {
                        m_pParticleTime->GetData()->SetDouble(part_timer);
                        m_pParticleTime->Update(true);
                    }
                  }

									SetInletParticles(pPar);

                  glPushMatrix();

                  moVector3f CO(m_Physics.m_EyeVector - pPar->Pos3d);
                  moVector3f U,V,W;
                  moVector3f CPU,CPW;
                  moVector3f A,B,C,D;

                  moVector3f CENTRO;

                  U = moVector3f( 0.0f, 0.0f, 1.0f );
                  V = moVector3f( 1.0f, 0.0f, 0.0f );
                  W = moVector3f( 0.0f, 1.0f, 0.0f );

                  U = CO;
                  U.Normalize();

                  //orientation always perpendicular to plane (X,Y)
                  switch(m_Physics.m_OrientationMode) {

                          case PARTICLES_ORIENTATIONMODE_FIXED:
                              //cuadrado centrado en Pos3d....
                              U = moVector3f( 0.0f, 0.0f, 1.0f );
                              V = moVector3f( 1.0f, 0.0f, 0.0f );
                              W = moVector3f( 0.0f, 1.0f, 0.0f );
                              break;

                          case PARTICLES_ORIENTATIONMODE_CAMERA:
                          /** TODO: fix this */
                              V = moVector3f( -CO.Y(), CO.X(), 0.0f );
                              V.Normalize();

                              CPU = moVector3f( U.X(), U.Y(), 0.0f );
                              W = moVector3f( 0.0f, 0.0f, CPU.Length() );
                              CPU.Normalize();
                              CPW = CPU * -U.Z();
                              W+= CPW;
                              break;

                          case PARTICLES_ORIENTATIONMODE_MOTION:
                          /** TODO: fix this */
                              if (pPar->Velocity.Length()>0) U = pPar->Velocity;
                              U.Normalize();
                              if (U.Length() < 0.5) {
                                  U = moVector3f( 0.0, 0.0, 1.0 );
                                  U.Normalize();
                              }
                              V = moVector3f( -U.Y(), U.X(), 0.0f );
                              V.Normalize();
                              CPU = moVector3f( U.X(), U.Y(), 0.0f );
                              W = moVector3f( 0.0f, 0.0f, CPU.Length() );
                              CPU.Normalize();
                              CPW = CPU * -U.Z();
                              W+= CPW;
                              break;

													case PARTICLES_ORIENTATIONMODE_NORMAL:
															switch(m_Physics.m_EmitterType) {
																case PARTICLES_EMITTERTYPE_GRID:
																case PARTICLES_EMITTERTYPE_POINT:
																case PARTICLES_EMITTERTYPE_JET:
																	U = moVector3f( 0.0f, 0.0f, 1.0f );
		                              V = moVector3f( 1.0f, 0.0f, 0.0f );
		                              W = moVector3f( 0.0f, 1.0f, 0.0f );
																	break;

																case PARTICLES_EMITTERTYPE_SPIRAL:
																case PARTICLES_EMITTERTYPE_TUBE:
																  //no Z Coordinates
																	U = moVector3f( pPar->Pos3d.X(), pPar->Pos3d.Y(), 0.0 );
																	U.Normalize();
																	if (U.Length() < 0.5) {
																			U = moVector3f( 0.0, 0.0, 1.0 );
																			U.Normalize();
																	}
																	V = moVector3f( -U.Y(), U.X(), 0.0f );
																	V.Normalize();
																	CPU = moVector3f( U.X(), U.Y(), 0.0f );
																	W = moVector3f( 0.0f, 0.0f, CPU.Length() );
																	CPU.Normalize();
																	CPW = CPU * -U.Z();
																	W+= CPW;
																	break;

																case PARTICLES_EMITTERTYPE_SPHERE:
																  //All Coordinates
																	U = moVector3f( pPar->Pos3d.X(), pPar->Pos3d.Y(), pPar->Pos3d.Z());
																	U.Normalize();
																	if (U.Length() < 0.5) {
																			U = moVector3f( 0.0, 0.0, 1.0 );
																			U.Normalize();
																	}
																	V = moVector3f( -U.Y(), U.X(), 0.0f );
																	V.Normalize();
																	CPU = moVector3f( U.X(), U.Y(), 0.0f );
																	W = moVector3f( 0.0f, 0.0f, CPU.Length() );
																	CPU.Normalize();
																	CPW = CPU * -U.Z();
																	W+= CPW;
																	break;

																default:
																	U = moVector3f( 0.0f, 0.0f, 1.0f );
																	V = moVector3f( 1.0f, 0.0f, 0.0f );
																	W = moVector3f( 0.0f, 1.0f, 0.0f );
																	break;

															}
															break;
                  }

                  A = V * -sizexd2 + W * sizeyd2;
                  B = V *sizexd2 +  W * sizeyd2;
                  C = V *sizexd2 + W * -sizeyd2;
                  D = V * -sizexd2 + W * -sizeyd2;


                  //cuadrado centrado en Pos3d....

                  //TODO: dirty code here!!!
                  if (texture_mode==PARTICLES_TEXTUREMODE_UNIT || texture_mode==PARTICLES_TEXTUREMODE_PATCH) {

                      MOfloat cycleage = m_EffectState.tempo.ang*m_EffectState.tempo.syncro;

                      //if (m_Physics.m_MaxAge>0) cycleage = (float) ((double)pPar->Age.Duration() /  (double)m_Physics.m_MaxAge );
                      cycleage = part_timer*m_EffectState.tempo.syncro;

                      int glid = pPar->GLId;

                      if ( pPar->MOId==-1 ) {
                        moTextFilterParam DefParam;
                        //glid = m_Config.GetGLId( moR(PARTICLES_TEXTURE), (float)cycleage*(m_EffectState.tempo.syncro), (float)1.0, &DefParam );
                        //glid = m_Config.GetGLId( moR(PARTICLES_TEXTURE), (float)cycleage, 1.0, &DefParam );
												glid = m_Config.GetGLId( moR(PARTICLES_TEXTURE), (float)cycleage, 1.0, DefParam );
                        //MODebug2->Message( moText("Unit GLID: ") + IntToStr(glid)
                        //+ moText(" ang: ") + FloatToStr(cycleage)
                        //+ moText(" syncro: ") + FloatToStr(m_EffectState.tempo.syncro) );
                      } else {


                          if ( pPar->MOId>-1 ) {

                              moTexture* pTex = m_pResourceManager->GetTextureMan()->GetTexture(pPar->MOId);

                              if (pTex) {

                                  if (
                                      pTex->GetType()==MO_TYPE_VIDEOBUFFER
                                      || pTex->GetType()==MO_TYPE_CIRCULARVIDEOBUFFER
                                      || pTex->GetType()==MO_TYPE_MOVIE
                                      || pTex->GetType()==MO_TYPE_TEXTURE_MULTIPLE
                                       ) {
                                      moTextureAnimated *pTA = (moTextureAnimated*)pTex;

                                      if (pPar->FrameForced) {
                                          glid = pTA->GetGLId( pPar->ActualFrame );
                                      } else {
                                          glid = pTA->GetGLId((MOfloat)cycleage);
                                          pPar->ActualFrame = pTA->GetActualFrame();

                                          pPar->FramePS = pTA->GetFramesPerSecond();
                                          pPar->FrameCount = pTA->GetFrameCount();
                                      }

                                  } else {
                                      glid = pTex->GetGLId();
                                  }

                              }
                          }
                      }

                      glBindTexture( GL_TEXTURE_2D , glid );
                  }

                  glTranslatef( pPar->Pos3d.X()+m_Config.Eval( moR(PARTICLES_DELTAX_PARTICLE) ),
                                pPar->Pos3d.Y()+m_Config.Eval( moR(PARTICLES_DELTAY_PARTICLE) ),
                                pPar->Pos3d.Z()+m_Config.Eval( moR(PARTICLES_DELTAZ_PARTICLE) ));

                  glRotatef(  m_Config.Eval( moR(PARTICLES_ROTATEZ_PARTICLE) ) + pPar->Rotation.Z(), U.X(), U.Y(), U.Z() );
                  glRotatef(  m_Config.Eval( moR(PARTICLES_ROTATEY_PARTICLE) ) + pPar->Rotation.Y(), W.X(), W.Y(), W.Z() );
                  glRotatef(  m_Config.Eval( moR(PARTICLES_ROTATEX_PARTICLE) ) + pPar->Rotation.X(), V.X(), V.Y(), V.Z() );

                  //scale
                  glScalef(   m_Config.Eval( moR(PARTICLES_SCALEX_PARTICLE) )*pPar->Scale,
                              m_Config.Eval( moR(PARTICLES_SCALEY_PARTICLE) )*pPar->Scale,
                              m_Config.Eval( moR(PARTICLES_SCALEZ_PARTICLE) )*pPar->Scale);


                  glColor4f(  m_Config[moR(PARTICLES_COLOR)][MO_SELECTED][MO_RED].Eval() * pPar->Color.X() * m_EffectState.tintr,
                              m_Config[moR(PARTICLES_COLOR)][MO_SELECTED][MO_GREEN].Eval() * pPar->Color.Y() * m_EffectState.tintg,
                              m_Config[moR(PARTICLES_COLOR)][MO_SELECTED][MO_BLUE].Eval() * pPar->Color.Z() * m_EffectState.tintb,
                              m_Config[moR(PARTICLES_COLOR)][MO_SELECTED][MO_ALPHA].Eval()
                              * m_Config.Eval( moR(PARTICLES_ALPHA))
                              * m_EffectState.alpha * pPar->Alpha );

                  glBegin(GL_QUADS);
                      //glColor4f( 1.0, 0.5, 0.5, idxt );

                      if (pPar->GLId2>0) {
                          //glColor4f( 1.0, 0.5, 0.5, idxt );
                          glMultiTexCoord2fARB( GL_TEXTURE0_ARB, pPar->TCoord.X(), pPar->TCoord.Y() );
                          glMultiTexCoord2fARB( GL_TEXTURE1_ARB, pPar->TCoord2.X(), pPar->TCoord2.Y());
                      } else glTexCoord2f( pPar->TCoord.X(), pPar->TCoord.Y() );
                      glNormal3f( -U.X(), -U.Y(), -U.Z() );
                      glVertex3f( A.X(), A.Y(), A.Z());

                      //glColor4f( 0.5, 1.0, 0.5, idxt );

                      if (pPar->GLId2>0) {
                          glMultiTexCoord2fARB( GL_TEXTURE0_ARB, pPar->TCoord.X()+tsizex, pPar->TCoord.Y() );
                          glMultiTexCoord2fARB( GL_TEXTURE1_ARB, pPar->TCoord2.X()+pPar->TSize2.X(), pPar->TCoord2.Y());
                      } else glTexCoord2f( pPar->TCoord.X()+tsizex, pPar->TCoord.Y() );
                      glNormal3f( -U.X(), -U.Y(), -U.Z() );
                      glVertex3f( B.X(), B.Y(), B.Z());

                      //glColor4f( 0.5, 0.5, 1.0, idxt );
                      if (pPar->GLId2>0) {
                          glMultiTexCoord2fARB( GL_TEXTURE0_ARB, pPar->TCoord.X()+tsizex, pPar->TCoord.Y()+tsizey );
                          glMultiTexCoord2fARB( GL_TEXTURE1_ARB, pPar->TCoord2.X()+pPar->TSize2.X(), pPar->TCoord2.Y()+pPar->TSize2.Y());
                      } else glTexCoord2f( pPar->TCoord.X()+tsizex, pPar->TCoord.Y()+tsizey );
                      glNormal3f( -U.X(), -U.Y(), -U.Z() );
                      glVertex3f( C.X(), C.Y(), C.Z());

                      //glColor4f( 1.0, 1.0, 1.0, idxt );
                      if (pPar->GLId2>0) {
                          glMultiTexCoord2fARB( GL_TEXTURE0_ARB, pPar->TCoord.X(), pPar->TCoord.Y()+pPar->TSize.Y());
                          glMultiTexCoord2fARB( GL_TEXTURE1_ARB, pPar->TCoord2.X(), pPar->TCoord2.Y()+pPar->TSize2.Y());
                      } else glTexCoord2f( pPar->TCoord.X(), pPar->TCoord.Y()+tsizey );
                      glNormal3f( -U.X(), -U.Y(), -U.Z() );
                      glVertex3f( D.X(), D.Y(), D.Z());
                  glEnd();

                  //draw vectors associated...
                  if ( drawing_features>2 ) {
                      CENTRO = moVector3f( 0.0 , 0.0, 0.0 );

                      glDisable( GL_TEXTURE_2D );
                      glLineWidth( 8.0 );
                      glBegin(GL_LINES);
                          ///draw U
                          glColor4f( 0.0, 1.0, 1.0, 1.0);
                          glVertex3f( CENTRO.X(), CENTRO.Y(), 0.0001);

                          glColor4f( 0.0, 1.0, 1.0, 1.0);
                          glVertex3f( CENTRO.X() + U.X(), CENTRO.Y() + U.Y(), 0.0001);

                      glEnd();

                      glBegin(GL_LINES);
                          ///draw V
                          glColor4f( 1.0, 0.0, 1.0, 1.0);
                          glVertex3f( CENTRO.X(), CENTRO.Y(), 0.0001);

                          glColor4f( 1.0, 0.0, 1.0, 1.0);
                          glVertex3f( CENTRO.X() + V.X(), CENTRO.Y() + V.Y(), 0.0001);

                      glEnd();

                      glBegin(GL_LINES);
                          ///draw W
                          glColor4f( 0.0, 0.0, 1.0, 1.0);
                          glVertex3f( CENTRO.X(), CENTRO.Y(), 0.0001);

                          glColor4f( 0.0, 0.0, 1.0, 1.0);
                          glVertex3f( CENTRO.X() + W.X(), CENTRO.Y() + W.Y(), 0.0001);

                      glEnd();
                      glEnable( GL_TEXTURE_2D );
                  }


                  glPopMatrix();
              }

/*
              if (i==0 && j==0 ) {
                MODebug2->Message(
                                  moText(" pPar 0,0:")+
                                  moText(" visible:") + IntToStr((int)pPar->Visible)+ moText("\n")+
                                  " part_timer:" + FloatToStr(part_timer)+"\n"
                                  +" Pos X:" + FloatToStr(pPar->Pos.X())+ " Y:" + FloatToStr(pPar->Pos.Y())+"\n"
                                  +" Pos3d X:" + FloatToStr(pPar->Pos3d.X())+ " Y:" + FloatToStr(pPar->Pos3d.Y())+ " Z:" + FloatToStr(pPar->Pos3d.Z())+"\n"
                                  );

              }
*/
            }
        }
    }

    if (pFont && drawing_features>2) {
        for( i = 0; i<m_cols ; i++) {
            for( j = 0; j<m_rows ; j++) {

                moParticlesSimple* pPar = m_ParticlesSimpleArray.GetRef( i + j*m_cols );
                if ((i + j*m_cols) % 10 == 0 ) {
                    Texto = moText( IntToStr(i + j*m_cols));
                    Texto.Left(5);
                    Texto+= moText("F:")+moText( (moText)FloatToStr( pPar->Force.X() ).Left(5) + moText(",")
                                + (moText)FloatToStr( pPar->Force.Y() ).Left(5)
                                + moText(",") + (moText)FloatToStr( pPar->Force.Z() ).Left(5) );

                    Texto+= moText("V:")+ moText( (moText)FloatToStr( pPar->Velocity.X() ).Left(5) + moText(",")
                                + (moText)FloatToStr( pPar->Velocity.Y() ).Left(5)
                                + moText(",") + (moText)FloatToStr( pPar->Velocity.Z() ).Left(5) );

                    pFont->Draw( pPar->Pos3d.X(),
                                 pPar->Pos3d.Y(),
                                 Texto );


                    Texto = moText( moText("(") + (moText)FloatToStr(pPar->TCoord.X()).Left(4) + moText(",") + (moText)FloatToStr(pPar->TCoord.Y()).Left(4) + moText(")") );

                    pFont->Draw( pPar->Pos3d.X()-sizexd2,
                                 pPar->Pos3d.Y()+sizeyd2-2,
                                 Texto );

                    Texto = moText( moText("(") + (moText)FloatToStr(pPar->TCoord.X()+tsizex).Left(4) + moText(",") + (moText)FloatToStr(pPar->TCoord.Y()).Left(4) + moText(")"));

                    pFont->Draw( pPar->Pos3d.X()+sizexd2-12,
                                 pPar->Pos3d.Y()+sizeyd2-5,
                                 Texto );

                    Texto = moText( moText("(") + (moText)FloatToStr(pPar->TCoord.X()+tsizex).Left(4) + moText(",") + (moText)FloatToStr(pPar->TCoord.Y()+tsizey).Left(4) + moText(")"));

                    pFont->Draw( pPar->Pos3d.X()+sizexd2-12,
                                 pPar->Pos3d.Y()-sizeyd2+2,
                                 Texto );

                    Texto = moText( moText("(") + (moText)FloatToStr(pPar->TCoord.X()).Left(4) + moText(",") + (moText)FloatToStr(pPar->TCoord.Y()+tsizey).Left(4) + moText(")"));

                    pFont->Draw( pPar->Pos3d.X()-sizexd2,
                                 pPar->Pos3d.Y()-sizeyd2+5,
                                 Texto );
                }

            }
        }
    }

    if (pFont && drawing_features>2) {

                Texto = moText( moText("T2 Rest.:") + IntToStr(TimerFullRestoration.Duration()));
                pFont->Draw( -10.0f,
                             0.0f,
                             Texto );

                Texto = moText( moText("T2 Revel.:") + IntToStr(TimerFullRevelation.Duration()));
                pFont->Draw( -10.0f,
                             3.0f,
                             Texto );

                Texto = moText( moText("T Revel.:") + IntToStr(TimerOfRevelation.Duration()) + moText(" rev: ") + IntToStr(time_of_revelation));
                pFont->Draw( -10.0f,
                             6.0f,
                             Texto );

                Texto = moText( moText("T Rest.:") + IntToStr(TimerOfRestoration.Duration())  + moText(" res: ") + IntToStr(time_of_restoration) );
                pFont->Draw( -10.0f,
                             9.0f,
                             Texto );

                Texto = moText("Status: ");

                switch(revelation_status) {
                    case PARTICLES_FULLRESTORED:
                        Texto+= moText("Full Restored");
                        break;
                    case PARTICLES_REVEALING:
                        Texto+= moText("Revealing");
                        break;
                    case PARTICLES_REVEALINGALL:
                        Texto+= moText("Revealing");
                        break;
                    case PARTICLES_FULLREVEALED:
                        Texto+= moText("Full Revealed");
                        break;
                    case PARTICLES_RESTORING:
                        Texto+= moText("Restoring");
                        break;
                    case PARTICLES_RESTORINGALL:
                        Texto+= moText("Restoring All");
                        break;
                }
                pFont->Draw( -10.0f,
                             13.0f,
                             Texto );
    }

}
#ifdef USE_TUIO
using namespace TUIO;
#endif
void moEffectParticlesSimple::DrawTracker() {

    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    m_pTrackerData = NULL;

    bool has_motion = false;
    bool has_features = false;

    if (m_InletTrackerSystemIndex>-1) {
        moInlet* pInlet = m_Inlets.GetRef(m_InletTrackerSystemIndex);
        if (pInlet)
            if (pInlet->Updated()) {
                m_pTrackerData = (moTrackerSystemData *)pInlet->GetData()->Pointer();
                // chequeando info

                /*MODebug2->Push( moText("ParticlesSimple varX: ") + FloatToStr( m_pTrackerData->GetVariance().X())
                       + moText(" varY: ") + FloatToStr(m_pTrackerData->GetVariance().Y()) );
                */
            }
    }
#ifdef USE_TUIO
    m_pTUIOData = NULL;
    if (m_InletTuioSystemIndex>-1) {
        moInlet* pInlet = m_Inlets.GetRef(m_InletTuioSystemIndex);
        if (pInlet)
            if (pInlet->Updated()) {
                m_pTUIOData = (moTUIOSystemData *)pInlet->GetData()->Pointer();

                if (m_pTUIOData) {

                    TuioObject* pObject;

if (drawing_features > 0  ) {

                    std::list<TuioObject*> objects = m_pTUIOData->getTuioObjects();

                    for (std::list<TuioObject*>::iterator iter=objects.begin(); iter != objects.end(); iter++) {

                        pObject = (*iter);
                        if (pObject) {
                            moVector2f position( pObject->getX(), pObject->getY() * h / w);

                            glBindTexture(GL_TEXTURE_2D,0);
                            glColor4f(0.0, 1.0, 1.0, 1.0);

                            float angle = pObject->getAngle();

                            //int u = pObject->getSymbolID();

                            //moTrackerFeature* NF = NULL;

                            //if (m_pTrackerData) {
                            //    NF = m_pTrackerData->GetFeature(u);
                            //}


                            ///DRAW ANGLE CENTER
                            //glRotatef(  angle*moMathf::RAD_TO_DEG , 0.0, 0.0, 1.0 );

                            glBegin(GL_QUADS);
                                glVertex2f((position.X() - 0.02)*normalf, (position.Y() - 0.02)*normalf);
                                glVertex2f((position.X() - 0.02)*normalf, (position.Y() + 0.02)*normalf);
                                glVertex2f((position.X() + 0.02)*normalf, (position.Y() + 0.02)*normalf);
                                glVertex2f((position.X() + 0.02)*normalf, (position.Y() - 0.02)*normalf);
                            glEnd();


                            ///DRAW angle vector
                            glColor4f( 0.0,1.0,1.0,1.0);
                            glLineWidth(8.0);
                            glBegin(GL_LINES);
                                glVertex2f( position.X(), position.Y());
                                glVertex2f( position.X() + 0.04*cos(angle), position.Y() + 0.04*sin(angle) );
                            glEnd();

                            ///speed

                            glColor4f( 0.4,0.6,0.2,1.0);
                            glLineWidth(4.0);
                            glBegin(GL_LINES);
                                glVertex2f( position.X(), position.Y());
                                glVertex2f( position.X() + pObject->getXSpeed(), position.Y() + pObject->getYSpeed() );
                            glEnd();

                            ///rotation speed

                            //glColor4f( 1.0,0.0,0.0,1.0);
                            //glLineWidth(8.0);
                            //glBegin(GL_LINES);
                            //    glVertex2f( position.X(), position.Y());
                            //    glVertex2f( position.X(), position.Y() + pObject->getRotationSpeed()*0.0001 );
                            //glEnd();

                            //MODebug2->Push(moText("rspeed:")+IntToStr( (int)(pObject->getRotationSpeed()/10.0))  );

                            //if (NF) {
                            //    moVector2f pu( NF->x - 0.5, (-NF->y + 0.5) * h / w);
                            //    glColor4f( 0.5,0.0,0.0,1.0);
                            //    glBegin(GL_QUADS);
                            //        glVertex2f( ( pu.X() - 0.01)*normalf, (pu.Y() - 0.01)*normalf);
                            //        glVertex2f((pu.X() - 0.01)*normalf, (pu.Y() + 0.01)*normalf);
                            //        glVertex2f((pu.X() + 0.01)*normalf, (pu.Y() + 0.01)*normalf);
                            //        glVertex2f((pu.X() + 0.01)*normalf, (pu.Y() - 0.01)*normalf);
                            //    glEnd();
                            //}



                        }

                    }

                    TuioCursor* pCursor;

                    std::list<TuioCursor*> cursors = m_pTUIOData->getTuioCursors();

                    for (std::list<TuioCursor*>::iterator iter=cursors.begin(); iter != cursors.end(); iter++) {

                        pCursor = (*iter);
                        if (pCursor) {
                            moVector2f position( pCursor->getX(), pCursor->getY() * h / w);

                            glBindTexture(GL_TEXTURE_2D,0);
                            glColor4f(0.0, 0.0, 1.0, 1.0);
                            ///try using moP5::ellipse(x,y,w,h)

                            float r = 0.02;
                            glLineWidth(4.0);
                            glBegin(GL_LINE_LOOP);
                                for(float t = 0; t <= moMathf::TWO_PI; t += moMathf::TWO_PI / 12 )
                                    glVertex2f( r* cos(t) + position.X(), r* sin(t) + position.Y());
                            glEnd();
                        }

                    }
}
                }

            }
    }
#endif

    //if (m_pTUIOData) {
    //    int nobjects = m_pTUIOData->getTuioObjects().size();
    //    //MODebug2->Push( moText(" NOBJECTS: ") + IntToStr(nobjects) );

    //        for (int f = 0; f < m_pTUIOData->getTuioObjects().size(); f++)
    //        {

    //        }

    //}


    if (m_pTrackerData ) {

        glDisable(GL_TEXTURE_2D);

        m_bTrackerInit = true;

        //SelectScriptFunction("Reset");
        //RunSelectedFunction();

        //MODebug2->Push(IntToStr(TrackerId));

        //MODebug2->Push(moText("Receiving:") + IntToStr(m_pTrackerData->GetFeaturesCount()) );
        if (m_pTrackerData->GetFeaturesCount()>0) {
            int tw = m_pTrackerData->GetVideoFormat().m_Width;
            int th = m_pTrackerData->GetVideoFormat().m_Height;
            //MODebug2->Push(moText("vformat:")+IntToStr(tw)+moText("th")+IntToStr(th));

            m_TrackerBarycenter = moVector2f( ( ( m_pTrackerData->GetBarycenter().X() ) - 0.5),
                                              ( -( m_pTrackerData->GetBarycenter().Y() ) + 0.5) * h / w );

            //MODebug2->Push(moText("Barycenter x:")+FloatToStr(m_TrackerBarycenter.X()) + moText(" y:")+FloatToStr(m_TrackerBarycenter.Y()) );

            if (drawing_features > 2 ) {

                glBindTexture(GL_TEXTURE_2D,0);
                glColor4f(0.3, 1.0, 1.0, 1.0);

                glBegin(GL_QUADS);
                    glVertex2f((m_TrackerBarycenter.X() - 0.02)*normalf, (m_TrackerBarycenter.Y() - 0.02)*normalf);
                    glVertex2f((m_TrackerBarycenter.X() - 0.02)*normalf, (m_TrackerBarycenter.Y() + 0.02)*normalf);
                    glVertex2f((m_TrackerBarycenter.X() + 0.02)*normalf, (m_TrackerBarycenter.Y() + 0.02)*normalf);
                    glVertex2f((m_TrackerBarycenter.X() + 0.02)*normalf, (m_TrackerBarycenter.Y() - 0.02)*normalf);
                glEnd();

            }


            for (int f = 0; f < m_pTrackerData->GetFeaturesCount(); f++)
            {

                moTrackerFeature* pF = m_pTrackerData->GetFeature(f);

            if (pF && pF->valid) {

                float x = (pF->x ) - 0.5;
                float y = ( -(pF->y ) + 0.5 ) * h / w;
                float tr_x = (pF->tr_x ) - 0.5 ;
                float tr_y = ( -(pF->tr_y ) + 0.5 ) * h / w;
                float v_x = (pF->v_x );
                float v_y = -(pF->v_y);
                float vel = sqrtf( v_x*v_x+v_y*v_y );
                int v = pF->val;

                //MODebug2->Log(moText("    x:")+FloatToStr(pF->x) + moText(" y:")+FloatToStr(pF->y) );

                glBindTexture(GL_TEXTURE_2D,0);
                glColor4f(1.0, 0.0, 0.0, 1.0);

                if (v >= 0) glColor4f(0.0, 1.0, 0.0, 1.0);
                else if (v == -1) glColor4f(1.0, 0.0, 1.0, 1.0);
                else if (v == -2) glColor4f(1.0, 0.0, 1.0, 1.0);
                else if (v == -3) glColor4f(1.0, 0.0, 1.0, 1.0);
                else if (v == -4) glColor4f(1.0, 0.0, 1.0, 1.0);
                else if (v == -5) glColor4f(1.0, 0.0, 1.0, 1.0);




                if ( v>=0) {
                    has_features = true;

                    if (drawing_features > 0 ) {

                        glPointSize((GLfloat)4);
                        glLineWidth((GLfloat)4.0);

                        if ( pF->is_object) {
                            glColor4f(1.0, 0.0, 0.0, 1.0);

                            //Uplas[f] >=3 !!!!!
                            int myuplas[3];
                            myuplas[0] = -1;
                            myuplas[1] = -1;
                            myuplas[2] = -1;
                            int k1,k2,o = 0;
                            for( int pp=0; pp<m_pTrackerData->nPares; pp++) {
                                k1 = m_pTrackerData->m_Pares[pp][0];
                                k2 = m_pTrackerData->m_Pares[pp][1];
                                if (o<3) if (k1==f) myuplas[o++] = k2;
                                if (o<3) if (k2==f) myuplas[o++] = k1;
                            }

                            int countv = 1;
                            moVector2f obj_v( x, y );
                            glColor4f(1.0, 0.1, 0.1, 1.0);
                            //glPointSiz((GLfloat)10);
                            glLineWidth((GLfloat)2.0);
                            for( int pp=0; pp<3; pp++ ) {
                                int e = myuplas[pp];
                                moTrackerFeature* NF = m_pTrackerData->GetFeature(e);
                                if (NF) {
                                    moVector2f av2( NF->x - 0.5, (-NF->y + 0.5 ) * h / w );
                                    //obj_v+=av2;
                                    glBegin(GL_LINES);
                                        glVertex2f(obj_v.X(),obj_v.Y());
                                        glVertex2f(av2.X(),av2.Y());
                                    glEnd();
                                    countv++;
                                }
                            }
                        }

                        glBegin(GL_QUADS);
                            glVertex2f((x - 0.008)*normalf, (y - 0.008)*normalf);
                            glVertex2f((x - 0.008)*normalf, (y + 0.008)*normalf);
                            glVertex2f((x + 0.008)*normalf, (y + 0.008)*normalf);
                            glVertex2f((x + 0.008)*normalf, (y - 0.008)*normalf);
                        glEnd();
                    }


                    if (drawing_features > 1 ) {
                        glBegin(GL_QUADS);
                            glVertex2f((tr_x - 0.008)*normalf, (tr_y - 0.008)*normalf);
                            glVertex2f((tr_x - 0.008)*normalf, (tr_y + 0.008)*normalf);
                            glVertex2f((tr_x + 0.008)*normalf, (tr_y + 0.008)*normalf);
                            glVertex2f((tr_x + 0.008)*normalf, (tr_y - 0.008)*normalf);
                        glEnd();


                        glColor4f(1.0, 1.0, 1.0, 1.0);
                        glBegin(GL_LINES);
                            glVertex2f( x*normalf, y*normalf);
                            glVertex2f( tr_x*normalf, tr_y*normalf);
                        glEnd();
                    }

                    if ( vel > 0.01 && vel < 0.1) {

                        has_motion = true;

                        if (drawing_features > 1 ) {

                            glColor4f(0.0, 0.0, 1.0, 1.0);
                            //glPointSize((GLfloat)10);
                            glLineWidth((GLfloat)2.0);

                            glBegin(GL_LINES);
                                glVertex2f( x*normalf, y*normalf);
                                glVertex2f( (x+v_x)*normalf, (y+v_y)*normalf);
                            glEnd();
                        }

                        ///ParticlesSimpleInfluence( x*normalf, y*normalf, (x+v_x)*normalf, (y+v_y)*normalf, vel*normalf );
                    }


                }

            }


            }

            ///post process:
            ///add threshold??? some number of frames for really check...
            if (has_motion) {

                if (!MotionActivity.Started()) {
                    MotionActivity.Start();
                }

            } else {
                MotionActivity.Stop();
            }

            if (has_features) {

                if (!FeatureActivity.Started()) {
                    FeatureActivity.Start();
                }

            } else {
                FeatureActivity.Stop();
            }


        }

        glEnable(GL_TEXTURE_2D);

    }

}

void moEffectParticlesSimple::Draw( moTempo* tempogral, moEffectState* parentstate)
{

    PreDraw( tempogral, parentstate);
    moRenderManager* mRender = m_pResourceManager->GetRenderMan();
    moGLManager* mGL = m_pResourceManager->GetGLMan();

    double  prop = mRender->ScreenProportion();
    if (mRender==NULL || mGL==NULL) return;

    int w = mRender->ScreenWidth();
    int h = mRender->ScreenHeight();

    int ancho,alto;
    frame++;
    moFont* pFont = m_Config[ moR(PARTICLES_FONT) ][MO_SELECTED][0].Font();
    UpdateParameters();

    glDisable(GL_DEPTH_TEST);

    (ortho) ? mGL->SetDefaultOrthographicView( w, h ) : mGL->SetDefaultPerspectiveView( w, h );

    if (!ortho) {
        if ( m_EffectState.stereoside == MO_STEREO_NONE ) {

        mGL->LookAt(  m_Physics.m_EyeVector.X(),/*0*/
                      m_Physics.m_EyeVector.Y(),/*0*/
                      m_Physics.m_EyeVector.Z(),/*-10*/
                      m_Physics.m_TargetViewVector.X(),/*0*/
                      m_Physics.m_TargetViewVector.Y(),/*0*/
                      m_Physics.m_TargetViewVector.Z(),/*0*/
                      /** 0,  1,  0 **/
                      /** producto vectorial de eye x target */
                      m_Physics.m_UpViewVector.X(),
                      m_Physics.m_UpViewVector.Y(),
                      m_Physics.m_UpViewVector.Z());

        } else {
          /*
            if ( m_EffectState.stereoside == MO_STEREO_LEFT ) {
               gluLookAt(	m_Physics.m_EyeVector.X()-0.1,
                            m_Physics.m_EyeVector.Y(),
                            m_Physics.m_EyeVector.Z(),
                            m_Physics.m_TargetViewVector.X()-0.1,
                            m_Physics.m_TargetViewVector.Y(),
                            m_Physics.m_TargetViewVector.Z(),
                            0, 1, 0);
            } else if ( m_EffectState.stereoside == MO_STEREO_RIGHT ) {
                gluLookAt(	m_Physics.m_EyeVector.X()+0.1,
                            m_Physics.m_EyeVector.Y(),
                            m_Physics.m_EyeVector.Z(),
                           m_Physics.m_TargetViewVector.X()+0.1,
                            m_Physics.m_TargetViewVector.Y(),
                            m_Physics.m_TargetViewVector.Z(),
                            0, 1, 0);
            }
*/
        }
    }

/*
  glGetFloatv( GL_PROJECTION_MATRIX, &glM[0] );
   glGetIntegerv( GL_PROJECTION_STACK_DEPTH, &pdepth );
   //MODebug2->Message( "pdepth: " + IntToStr(pdepth) );
   moGLMatrixf PMGL = moMatrix4f( glM );
   //MODebug2->Message( "GL_PROJECTION_MATRIX:\n" + PMGL.ToJSON() );
*/

    if (texture_mode==PARTICLES_TEXTUREMODE_UNIT || texture_mode==PARTICLES_TEXTUREMODE_PATCH) {
      glid = m_Config.GetGLId( moR(PARTICLES_TEXTURE), &m_EffectState.tempo);
    }


    glEnable(GL_ALPHA);
    //glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    //glClear( GL_DEPTH_BUFFER_BIT );
    //glEnable(GL_ALPHA_TEST);


    //glDisable( GL_CULL_FACE);
    //glFrontFace( GL_CCW);

    //glDepthMask(true);

    //esto deberia ser parametrizable...
	//glEnable( GL_DEPTH_TEST);
	//glDisable( GL_DEPTH_TEST);
    setUpLighting();

    tx = m_Config.Eval( moR(PARTICLES_TRANSLATEX));
    ty = m_Config.Eval( moR(PARTICLES_TRANSLATEY));
    tz = m_Config.Eval( moR(PARTICLES_TRANSLATEZ));

    sx = m_Config.Eval( moR(PARTICLES_SCALEX));
    sy = m_Config.Eval( moR(PARTICLES_SCALEY));
    sz = m_Config.Eval( moR(PARTICLES_SCALEZ));

    //rotation
    float rz = m_Config.Eval( moR(PARTICLES_ROTATEZ))*moMathf::DEG_TO_RAD;
    float ry = m_Config.Eval( moR(PARTICLES_ROTATEY))*moMathf::DEG_TO_RAD;
    float rx = m_Config.Eval( moR(PARTICLES_ROTATEX))*moMathf::DEG_TO_RAD;

#ifndef OPENGLESV2
/*
    glTranslatef( tx, ty, tz);
    glRotatef(  rz, 0.0, 0.0, 1.0 );
    glRotatef(  ry, 0.0, 1.0, 0.0 );
    glRotatef(  rx, 1.0, 0.0, 0.0 );
    glScalef(   sx, sy, sz);
*/
#endif
  glMatrixMode( GL_MODELVIEW );                         // Select The Modelview Matrix
  moGLMatrixf& MM( mGL->GetModelMatrix() );
  MM.MakeIdentity();
  MM.Scale( sx, sy, sz );
  MM.Rotate( rx, 1.0, 0.0, 0.0 );
  MM.Rotate( ry, 0.0, 1.0, 0.0 );
  MM.Rotate( rz, 0.0, 0.0, 1.0 );
  MM.Translate( tx, ty, tz );

#ifndef OPENGLESV2
  glMatrixMode( GL_MODELVIEW );
  glLoadMatrixf( MM.GetPointer() );
#endif
    //blending
    SetBlending( (moBlendingModes) m_Config.Int( moR(PARTICLES_BLENDING) ) );
/*
    //set image
    moTexture* pImage = (moTexture*) m_Config[moR(PARTICLES_TEXTURE)].GetData()->Pointer();
*/
    //color
    //SetColor( m_Config[moR(PARTICLES_COLOR)][MO_SELECTED], m_Config[moR(PARTICLES_ALPHA)][MO_SELECTED], m_EffectState );
    SetColor( m_Config[moR(PARTICLES_COLOR)], m_Config[moR(PARTICLES_ALPHA)], m_EffectState );
/*
    moText Texto = m_Config.Text( moR(PARTICLES_TEXT) );

    moVector4d color = m_Config.EvalColor( moR(PARTICLES_COLOR) );
    moPlaneGeometry PlaneQuad( 1.0, 1.0, 1, 1 );
    moMaterial Material;
    moData* TData = m_Config[moR(PARTICLES_TEXTURE)][MO_SELECTED][0].GetData();
    if (TData) {
      glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(PARTICLES_TEXTURE), &m_EffectState.tempo ) );
      Material.m_Map = TData->Texture();
    }
        Material.m_Color = moColor( color.X(), color.Y(), color.Z() );

    moMesh Mesh( PlaneQuad, Material );
    Mesh.SetModelMatrix( MM );

    moCamera3D Camera3D;
    Camera3D = mGL->GetProjectionMatrix();
    mRender->Render( Mesh, Camera3D );
*/

//glutSolidTorus ( 1, 2, 13, 20);
/*
    glutSolidSphere ( 1, 13, 20 );
    glBegin(GL_QUADS );

      glNormal3f(0,0,-1);
      glVertex3f( -3, 3, 0);

      glNormal3f(0,0,-1);
      glVertex3f( 3, 3, 0);

      glNormal3f(0,0,-1);
      glVertex3f( 3, -3, 0);

      glNormal3f(0,0,-1);
      glVertex3f( -3, -3, 0);
    glEnd();
    */

    DrawParticlesSimple( tempogral, parentstate );

    //add conditions

    DrawTracker();


    if (pFont && drawing_features>2) {
        moText infod = moText("gravitational:")+FloatToStr(m_Physics.gravitational)+moText(" viscosity:")+FloatToStr(m_Physics.viscousdrag);
        pFont->Draw( 0.0, 0.0, infod );
    }

    EndDraw();

#ifndef OPENGLESV2
  //glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
#endif

}

void moEffectParticlesSimple::setUpLighting()
{

  /*
   // Set up lighting.
   float light1_ambient[4]  = { 1.0, 1.0, 1.0, 1.0 };
   float light1_diffuse[4]  = { 1.0, 0.9, 0.9, 1.0 };
   float light1_specular[4] = { 1.0, 0.7, 0.7, 1.0 };
   float light1_position[4] = { -1.0, 1.0, 1.0, 0.0 };
   glLightfv(GL_LIGHT1, GL_AMBIENT,  light1_ambient);
   glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_diffuse);
   glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
   glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
   glEnable(GL_LIGHT1);

   float light2_ambient[4]  = { 0.2, 0.2, 0.2, 1.0 };
   float light2_diffuse[4]  = { 0.9, 0.9, 0.9, 1.0 };
   float light2_specular[4] = { 0.7, 0.7, 0.7, 1.0 };
   float light2_position[4] = { 1.0, -1.0, -1.0, 0.0 };
   glLightfv(GL_LIGHT2, GL_AMBIENT,  light2_ambient);
   glLightfv(GL_LIGHT2, GL_DIFFUSE,  light2_diffuse);
   glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular);
   glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
//   glEnable(GL_LIGHT2);

*/
  if ( m_Physics.m_SourceLighMode>0 ) {

   //glShadeModel( GL_SMOOTH );
   glShadeModel( GL_FLAT );
   glEnable( GL_DEPTH_TEST);
   //glEnable( GL_DEPTH_BUFFER);
   glEnable(GL_NORMALIZE);
   //glEnable(GL_CULL_FACE);
  //glEnable(GL_AUTO_NORMAL );
   glEnable(GL_LIGHTING);

   float light0_ambient[4]  = { 0.1, 0.1, 0.1, 1.0 };
   float light0_diffuse[4]  = { 1, 1.0,1, 1.0 };
   float light0_specular[4] = { 1.0, 1.0, 1.0, 1.0 };
   float light0_position[4];
   light0_position[0] = m_Physics.m_SourceLightVector.X();
   light0_position[1] = m_Physics.m_SourceLightVector.Y();
   light0_position[2] = m_Physics.m_SourceLightVector.Z();
   light0_position[3] = 0.0;

   glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
   glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

   float light0_spot_direction[3];
   light0_spot_direction[0] = 0.0;
   light0_spot_direction[1] = 0.0;
   light0_spot_direction[2] = 0.0;

   if (m_Physics.m_SourceLighMode==PARTICLES_LIGHTMODE_SPOT) {
      glLightfv( GL_LIGHT0, GL_SPOT_DIRECTION, light0_spot_direction );

   }
   glEnable(GL_LIGHT0);

   float front_emission[4] = { 0.0, 0.0, 0.0, 1.0 };
   float front_ambient[4]  = { 0.1, 0.1, 0.1, 1.0 };
   float front_diffuse[4]  = { 1, 1, 1, 1.0 };
   float front_specular[4] = { 1, 1, 1, 1.0 };
   /*
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, front_emission);
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, front_ambient);
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, front_diffuse);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, front_specular);
   glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1.0);
   glColor4fv(front_diffuse);
   */

   glMaterialfv(GL_FRONT, GL_EMISSION, front_emission);
   glMaterialfv(GL_FRONT, GL_AMBIENT, front_ambient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, front_diffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, front_specular);
   glMaterialf(GL_FRONT, GL_SHININESS, 50.0);

  //glFrontFace(GL_CW);
  //glFrontFace(GL_CCW);

   //glLightModelfv( GL_LIGHT_MODEL_AMBIENT, LightModelAmbient );

   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
   //glColorMaterial(GL_FRONT, GL_DIFFUSE);
   glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
   glEnable(GL_COLOR_MATERIAL);

    glPushMatrix();
    glTranslatef(   m_Physics.m_SourceLightVector.X(),
                    m_Physics.m_SourceLightVector.Y(),
                    m_Physics.m_SourceLightVector.Z() );
    glColor4f( 1, 1, 0, 0.5 );
    //glutSolidSphere( 0.1, 4, 4);
    glPopMatrix();


  } else {
    glShadeModel(GL_SMOOTH);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
  }
}


MOboolean moEffectParticlesSimple::Finish()
{
    return PreFinish();
}

//====================
//
//		CUSTOM
//
//===================


void moEffectParticlesSimple::Interaction( moIODeviceManager *IODeviceManager ) {
	moDeviceCode *temp;
	MOint did,cid,state,valor;

	moEffect::Interaction( IODeviceManager );

	if (devicecode!=NULL)
	for(int i=0; i<ncodes; i++) {

		temp = devicecode[i].First;

		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			state = IODeviceManager->IODevices().GetRef(did)->GetStatus(cid);
			valor = IODeviceManager->IODevices().GetRef(did)->GetValue(cid);
			if (state)
			switch(i) {
				case 0:
					//Tx = valor;
					MODebug2->Push(IntToStr(valor));
					break;
				case 1:
					//Ty = m_pResourceManager->GetRenderMan()->RenderHeight() - valor;
					MODebug2->Push(IntToStr(valor));
					break;
				case 2:
					//Sx+=((float) valor / (float) 256.0);
					MODebug2->Push(IntToStr(valor));
					break;
				case 3:
					//Sy+=((float) valor / (float) 256.0);
					MODebug2->Push(IntToStr(valor));
					break;
			}
		temp = temp->next;
		}
	}

	///MIDI CONTROLS !!!!!
    moEvent* actual = IODeviceManager->GetEvents()->First;
	//recorremos todos los events y parseamos el resultado
	//borrando aquellos que ya usamos
	MOint tempval;
	while(actual!=NULL) {
		//solo nos interesan los del midi q nosotros mismos generamos, para destruirlos
		if(actual->deviceid == MO_IODEVICE_MIDI) {

		    ///actual->reservedvalue1 corresponds to CC midi code : it works as a n index in m_Codes (has to be defined in param "code" in the config file...
		    ///actual->reservedvalue2 corresponds to VAL

			int CC = actual->reservedvalue1;
			int VAL = actual->reservedvalue2;

            switch (CC) {
                case 45:
                    if (texture_mode==PARTICLES_TEXTUREMODE_MANY2PATCH) {
                        this->Shot( "FOTOS", 1 );
                        this->ReInit();
                        MODebug2->Message(moText("## Midi Shot Triggered ##"));
                    } else {
                        this->ReInit();
                    }
                    break;
                case 81:
                    midi_red = (float)VAL / (float) 127.0;
                    MODebug2->Message(moText("Red:")+FloatToStr(midi_red));
                    break;
                case 82:
                    midi_green = (float)VAL / (float) 127.0;
                    MODebug2->Message(moText("Green:")+FloatToStr(midi_green));
                    break;
                case 83:
                    midi_blue = (float)VAL / (float) 127.0;
                    MODebug2->Message(moText("Blue:")+FloatToStr(midi_blue));
                    break;

                case 84:
                    midi_maxage = (float)VAL / (float) 127.0;
                    MODebug2->Message(moText("Maxage:")+FloatToStr(midi_maxage));
                    break;
                case 85:
                    midi_emitionperiod = (float)VAL / (float) 127.0;
                    MODebug2->Message(moText("Emition Period:")+FloatToStr(midi_emitionperiod));
                    break;
                case 86:
                    midi_emitionrate = (float)VAL / (float) 127.0;
                    break;
                case 87:
                    midi_randomvelocity = (float)VAL / (float) 127.0;
                    break;

                case 88:
                    midi_randommotion = (float)VAL / (float) 127.0;
                    break;

                case 107:
                    if (VAL==0 || VAL==127) {
                        m_Config.PreConfPrev();
                    }
                    break;
                case 108:
                    if (VAL==0 || VAL==127) {
                         m_Config.PreConfNext();
                    }
                    break;


                default:
                    MODebug2->Push("device in psimple!!!");
                    MODebug2->Push(IntToStr(VAL));
                    break;
            }



			actual = actual->next;
		} else actual = actual->next;
	}

}

void moEffectParticlesSimple::Update( moEventList *p_eventlist ) {

    moMoldeoObject::Update( p_eventlist );



    /** TODO: Obsolete > use  luaGetTrackerFeaturesCount and luaGetTrackerValidFeatures */
    /*
    if (moScript::IsInitialized()) {
        if (ScriptHasFunction("Update")) {
            SelectScriptFunction("Update");
            //passing number of particles
            AddFunctionParam( (int) ( m_rows*m_cols ) );
            //passing dt
            AddFunctionParam( (float) dt );
            if (m_pTrackerData) {
                AddFunctionParam(m_pTrackerData->GetFeaturesCount());
                AddFunctionParam(m_pTrackerData->GetValidFeatures());
                //AddFunctionParam((int)m_pTrackerData->GetVideoFormat().m_Width);
                //AddFunctionParam((int)m_pTrackerData->GetVideoFormat().m_Height);
            } else {
                AddFunctionParam( -1 );
                AddFunctionParam( -1 );
            }

            if (!RunSelectedFunction(1)) {
                MODebug2->Error( GetLabelName() + moText(" moEffectParticlesSimple::  script > ") + moText("Update function not executed") );
            }
        }
    }
    */
}

void moEffectParticlesSimple::RegisterFunctions()
{
    moMoldeoObject::RegisterFunctions();

    RegisterBaseFunction("GetDelta"); //0
    RegisterFunction("GetParticleCount"); //1
    RegisterFunction("GetParticle"); //2
    RegisterFunction("GetParticlePosition");//3
    RegisterFunction("GetParticleSize");//4
    RegisterFunction("GetParticleScale");//5
    RegisterFunction("GetParticleVelocity");//6
    RegisterFunction("GetParticleRotation");//7
    RegisterFunction("GetParticleGraphics");//8
    RegisterFunction("GetParticleOpacity");//9
    RegisterFunction("GetParticleColor");//10

    RegisterFunction("UpdateParticle");//11
    RegisterFunction("UpdateParticlePosition");//12
    RegisterFunction("UpdateParticleSize");//13
    RegisterFunction("UpdateParticleScale");//14
    RegisterFunction("UpdateParticleVelocity");//15
    RegisterFunction("UpdateParticleRotation");//16
    RegisterFunction("UpdateParticleGraphics");//17
    RegisterFunction("UpdateParticleOpacity");//18
    RegisterFunction("UpdateParticleColor");//19

	RegisterFunction("UpdateForce");//20

	RegisterFunction("Shot");//21
	RegisterFunction("ReInit");//22

    RegisterFunction("DrawPoint");//23
    RegisterFunction("GetParticleIntersection");//24

    ResetScriptCalling();
}

int moEffectParticlesSimple::ScriptCalling(moLuaVirtualMachine& vm, int iFunctionNumber)
{
    switch (iFunctionNumber - m_iMethodBase)
    {
        case 0:
            ResetScriptCalling();
            return luaGetDelta(vm);
        case 1:
            ResetScriptCalling();
            return luaGetParticleCount(vm);
        case 2:
            ResetScriptCalling();
            return luaGetParticle(vm);
        case 3:
            ResetScriptCalling();
            return luaGetParticlePosition(vm);
        case 4:
            ResetScriptCalling();
            return luaGetParticleSize(vm);
        case 5:
            ResetScriptCalling();
            return luaGetParticleScale(vm);
        case 6:
            ResetScriptCalling();
            return luaGetParticleVelocity(vm);
        case 7:
            ResetScriptCalling();
            return luaGetParticleRotation(vm);
        case 8:
            ResetScriptCalling();
            return luaGetParticleGraphics(vm);
        case 9:
            ResetScriptCalling();
            return luaGetParticleOpacity(vm);
        case 10:
            ResetScriptCalling();
            return luaGetParticleColor(vm);


        case 11:
            ResetScriptCalling();
            return luaUpdateParticle(vm);
        case 12:
            ResetScriptCalling();
            return luaUpdateParticlePosition(vm);
        case 13:
            ResetScriptCalling();
            return luaUpdateParticleSize(vm);
        case 14:
            ResetScriptCalling();
            return luaUpdateParticleScale(vm);
        case 15:
            ResetScriptCalling();
            return luaUpdateParticleVelocity(vm);
        case 16:
            ResetScriptCalling();
            return luaUpdateParticleRotation(vm);
        case 17:
            ResetScriptCalling();
            return luaUpdateParticleGraphics(vm);
        case 18:
            ResetScriptCalling();
            return luaUpdateParticleOpacity(vm);
        case 19:
            ResetScriptCalling();
            return luaUpdateParticleColor(vm);

        case 20:
            ResetScriptCalling();
            return luaUpdateForce(vm);


        case 21:
            ResetScriptCalling();
            return luaShot(vm);

        case 22:
            ResetScriptCalling();
            return luaReInit(vm);

        case 23:
            ResetScriptCalling();
            return luaDrawPoint(vm);

        case 24:
            ResetScriptCalling();
            return luaGetParticleIntersection(vm);

        default:
            NextScriptCalling();
            return moMoldeoObject::ScriptCalling( vm, iFunctionNumber );
	}
}



int moEffectParticlesSimple::luaDrawPoint(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOfloat x = (MOfloat) lua_tonumber (state, 1);
    MOfloat y = (MOfloat) lua_tonumber (state, 2);

    glDisable(GL_TEXTURE_2D);

    glColor4f( 1.0, 1.0, 1.0, 1.0);
    glPointSize(4.0);

    glBegin(GL_POINTS);
        glVertex2f( x, y );
    glEnd();

    return 0;
}

int moEffectParticlesSimple::luaGetDelta(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    lua_pushnumber(state, (lua_Number) dt );

    return 1;
}

int moEffectParticlesSimple::luaGetParticleCount(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    lua_pushnumber(state, (lua_Number) m_ParticlesSimpleArray.Count() );

    return 1;
}



int moEffectParticlesSimple::luaGetParticle(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesSimple* Par;

    Par = m_ParticlesSimpleArray[i];

    if (Par) {

        lua_pushnumber(state, (lua_Number) Par->Age.Duration() );
        lua_pushnumber(state, (lua_Number) Par->Visible );
        lua_pushnumber(state, (lua_Number) Par->Mass );
        lua_pushnumber(state, (lua_Number) Par->MaxAge );

    }

    return 4;
}


int moEffectParticlesSimple::luaGetParticleRotation(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesSimple* Par;

    moVector3f Rotation;

    Par = m_ParticlesSimpleArray[i];

    if (Par) {

        Rotation = Par->Rotation;
        lua_pushnumber(state, (lua_Number) Rotation.X() );
        lua_pushnumber(state, (lua_Number) Rotation.Y() );
        lua_pushnumber(state, (lua_Number) Rotation.Z() );

    } else {
        lua_pushnumber(state, (lua_Number) 0 );
        lua_pushnumber(state, (lua_Number) 0 );
        lua_pushnumber(state, (lua_Number) 0 );
    }

    return 3;
}

int moEffectParticlesSimple::luaGetParticleOpacity(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesSimple* Par;

    float opacity;

    Par = m_ParticlesSimpleArray[i];

    if (Par) {

        opacity = Par->Alpha;
        lua_pushnumber(state, (lua_Number) opacity );

    } else {
        lua_pushnumber(state, (lua_Number) 0 );
    }

    return 1;
}

int moEffectParticlesSimple::luaGetParticleColor(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesSimple* Par;

    moVector3f Color;

    Par = m_ParticlesSimpleArray[i];

    if (Par) {

        Color = Par->Color;
        lua_pushnumber(state, (lua_Number) Color.X() );
        lua_pushnumber(state, (lua_Number) Color.Y() );
        lua_pushnumber(state, (lua_Number) Color.Z() );

    } else {
        lua_pushnumber(state, (lua_Number) 0 );
        lua_pushnumber(state, (lua_Number) 0 );
        lua_pushnumber(state, (lua_Number) 0 );
    }

    return 3;
}

/**

    0: Par->GLId
    1: si es animado, la cantidad de frames: 0 o count
    2: si es animado, el  cuadro que se esta mostrando...
    3: si es animado, la velocidad...

*/

int moEffectParticlesSimple::luaGetParticleGraphics(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesSimple* Par;

    moVector3f Rotation;

    Par = m_ParticlesSimpleArray[i];

    if (Par) {
        int frameactual = 0;
        int framecount = 0;
        int frameps = 0;
        int moid = -1;

        if (Par->MOId==-1) {
            //int moid = m_pResourceManager->GetTextureMan()->GetTextureMOId( (MOuint)Par->GLId );
        }

        moid = Par->MOId;
        frameactual = Par->ActualFrame;
        framecount = Par->FrameCount;
        frameps = Par->FramePS;

        //Rotation = Par->Rotation;
        lua_pushnumber(state, (lua_Number) moid );
        lua_pushnumber(state, (lua_Number) frameactual );
        lua_pushnumber(state, (lua_Number) framecount );
        lua_pushnumber(state, (lua_Number) frameps );


    } else {

        lua_pushnumber(state, (lua_Number) 0 );
        lua_pushnumber(state, (lua_Number) 0 );
        lua_pushnumber(state, (lua_Number) 0 );
        lua_pushnumber(state, (lua_Number) 0 );
    }

    return 4;
}

//TODO: do an absolute version, who calculates also the system translation and rotation....
//this is a relative version o particle position

int moEffectParticlesSimple::luaGetParticlePosition(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesSimple* Par;

    moVector3f Position;

    Par = m_ParticlesSimpleArray[i];

    if (Par) {

        Position = Par->Pos3d;
        lua_pushnumber(state, (lua_Number) Position.X() );
        lua_pushnumber(state, (lua_Number) Position.Y() );
        lua_pushnumber(state, (lua_Number) Position.Z() );

    } else {
        lua_pushnumber(state, (lua_Number) 0 );
        lua_pushnumber(state, (lua_Number) 0 );
        lua_pushnumber(state, (lua_Number) 0 );
    }

    return 3;
}

int moEffectParticlesSimple::luaGetParticleSize(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesSimple* Par;

    moVector2f Size;

    Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Size = Par->Size;
        double part_timer = 0.001f * (double)(Par->Age.Duration());

        if (m_pParticleTime) {
          m_pParticleTime->GetData()->SetDouble(part_timer);
          m_pParticleTime->Update(true);
        }

        double sx = m_Config.Eval( moR(PARTICLES_SCALEX_PARTICLE) ) * Par->Scale * Par->ImageProportion;
        double sy = m_Config.Eval( moR(PARTICLES_SCALEY_PARTICLE) ) * Par->Scale;

        lua_pushnumber(state, (lua_Number) Size.X()*sx );
        lua_pushnumber(state, (lua_Number) Size.Y()*sy );

    } else {
        lua_pushnumber(state, (lua_Number) 0 );
        lua_pushnumber(state, (lua_Number) 0 );
    }

    return 2;
}

int moEffectParticlesSimple::luaGetParticleScale(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesSimple* Par;

    double Scale;

    Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Scale = Par->Scale;
        lua_pushnumber(state, (lua_Number) Scale );
    } else {
        lua_pushnumber(state, (lua_Number) 0 );
    }

    return 1;
}

int moEffectParticlesSimple::luaGetParticleVelocity(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesSimple* Par = m_ParticlesSimpleArray[i];

    moVector3f Velocity = Par->Velocity;

    if (Par) {

        lua_pushnumber(state, (lua_Number) Velocity.X() );
        lua_pushnumber(state, (lua_Number) Velocity.Y() );
        lua_pushnumber(state, (lua_Number) Velocity.Z() );

    }

    return 3;
}


int moEffectParticlesSimple::luaGetParticleIntersection(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOfloat x = (MOfloat) lua_tonumber (state, 1);
    MOfloat y = (MOfloat) lua_tonumber (state, 2);

    moParticlesSimple* Par;

    moVector3f Position;
    int index;
    bool a = false;

    float x1,x2,y1,y2;
    float xc,yc;
    float ux,uy;
    float vx,vy;

    float sizeu,sizev;

    index = -1;

    /// slow system
    ///iterate and calculate

    ///particle system in ortho mode....

    for( int i=0; i<m_ParticlesSimpleArray.Count(); i++ ) {

        Par = m_ParticlesSimpleArray[i];

        if (Par->Visible) {
            Position = Par->Pos3d;

            ///check for each particle if it intersects with x,y on screen

            xc = Par->Pos3d.X()*sx*cos(rz/180) + tx;
            yc = Par->Pos3d.Y()*sy*sin(rz/180) + ty;
            x1 = ( Par->Pos3d.X() - Par->Size.X()/2 ) * sx + tx;
            x2 = ( Par->Pos3d.X() + Par->Size.X()/2 ) * sx + tx;
            y1 = ( Par->Pos3d.Y() - Par->Size.Y()/2 ) * sy + ty;
            y2 = ( Par->Pos3d.Y() + Par->Size.Y()/2 ) * sy + ty;

            ux = ( x1 - xc );
            uy = ( y1 - yc );
            vx = ( x2 - xc );
            vy = ( y2 - yc );
            sizeu = Par->Size.X()/2 * sx;
            sizev = Par->Size.Y()/2 * sx;

            moVector2f feat( x, y );
            moVector2f u( ux, uy );
            moVector2f v( vx, vy );
            moVector2f featuv;
            featuv = moVector2f ( fabs( feat.Dot(u)), fabs( feat.Dot(v) ) );
            if ( featuv.X() < sizeu && featuv.Y() < sizev  ) {
                index = i;
            }
        }
    }

    lua_pushnumber(state, (lua_Number) index );

    return 1;
}


int moEffectParticlesSimple::luaUpdateParticle( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOint age = (MOint) lua_tonumber (state, 2);
    MOint visible = (MOint) lua_tonumber (state, 3);
    MOint mass = (MOint) lua_tonumber (state, 4);
    MOint maxage = (MOint) lua_tonumber (state, 5);

    moParticlesSimple* Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Par->Age.SetDuration( age);
        Par->Visible = visible;
        Par->Mass = mass;
        Par->MaxAge = maxage;
    }

    return 0;

}

int moEffectParticlesSimple::luaUpdateParticlePosition( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOfloat x = (MOfloat) lua_tonumber (state, 2);
    MOfloat y = (MOfloat) lua_tonumber (state, 3);
    MOfloat z = (MOfloat) lua_tonumber (state, 4);

    moParticlesSimple* Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Par->Pos3d = moVector3f( x, y, z );
    }

    return 0;

}

int moEffectParticlesSimple::luaUpdateParticleSize( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOfloat x = (MOfloat) lua_tonumber (state, 2);
    MOfloat y = (MOfloat) lua_tonumber (state, 3);


    moParticlesSimple* Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Par->Size = moVector2f( x, y );
    }

    return 0;

}

int moEffectParticlesSimple::luaUpdateParticleScale( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOfloat scale = (MOfloat) lua_tonumber (state, 2);

    moParticlesSimple* Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Par->Scale = scale;
    }

    return 0;

}

int moEffectParticlesSimple::luaUpdateParticleVelocity( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOfloat x = (MOfloat) lua_tonumber (state, 2);
    MOfloat y = (MOfloat) lua_tonumber (state, 3);
    MOfloat z = (MOfloat) lua_tonumber (state, 4);

    moParticlesSimple* Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Par->Velocity = moVector3f( x, y, z );
    }

    return 0;

}

int moEffectParticlesSimple::luaUpdateParticleRotation( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOfloat rx = (MOfloat) lua_tonumber (state, 2);
    MOfloat ry = (MOfloat) lua_tonumber (state, 3);
    MOfloat rz = (MOfloat) lua_tonumber (state, 4);

    moParticlesSimple* Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Par->Rotation = moVector3f( rx, ry, rz );
    }

    return 0;

}

int moEffectParticlesSimple::luaUpdateParticleGraphics( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOuint moid = (MOuint) lua_tonumber (state, 2);
    MOuint actualframe = (MOuint) lua_tonumber (state, 3);
    MOuint frameps = (MOuint) lua_tonumber (state, 4);
    MOuint frameforced = (MOuint) lua_tonumber (state, 5);

    moParticlesSimple* Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Par->MOId = moid;
        Par->ActualFrame = actualframe;
        Par->FramePS = frameps;
        Par->FrameForced = (bool)frameforced;
    }

    return 0;

}


int moEffectParticlesSimple::luaUpdateForce( moLuaVirtualMachine& vm ) {


    return 0;

}

int moEffectParticlesSimple::luaUpdateParticleOpacity( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOfloat alpha = (MOfloat) lua_tonumber (state, 2);

    moParticlesSimple* Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Par->Alpha = alpha;
    }

    return 0;

}

int moEffectParticlesSimple::luaUpdateParticleColor( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOfloat r = (MOfloat) lua_tonumber (state, 2);
    MOfloat g = (MOfloat) lua_tonumber (state, 3);
    MOfloat b = (MOfloat) lua_tonumber (state, 4);

    moParticlesSimple* Par = m_ParticlesSimpleArray[i];

    if (Par) {
        Par->Color = moVector3f(r,g,b);
    }

    return 0;

}


int moEffectParticlesSimple::luaShot( moLuaVirtualMachine& vm) {

    lua_State *state = (lua_State *) vm;

	if (lua_isboolean(state,1)) {
		bool vb = lua_toboolean(state,1);
		vb ? MODebug2->Push(moText("true")) : MODebug2->Push(moText("false"));
	} else {
		char *text = (char *) lua_tostring (state, 1);
		int shot_type = lua_tonumber (state, 2);
		int shot_file = lua_tonumber (state, 3);

		moText source_to_shot = moText(text);
		this->Shot( source_to_shot, shot_type, shot_file );
	}

    return 0;
}

int moEffectParticlesSimple::luaReInit(moLuaVirtualMachine& vm ) {

    lua_State *state = (lua_State *) vm;

    this->ReInit();

    return 0;
}
