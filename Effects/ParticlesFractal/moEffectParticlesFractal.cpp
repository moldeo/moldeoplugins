/*******************************************************************************

                              moEffectParticlesFractal.cpp

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

#include "moEffectParticlesFractal.h"
#include "moTextureFilterIndex.h"

#include "moArray.h"
moDefineDynamicArray( moParticlesFractalArray )


moTextureFilterVelocity::moTextureFilterVelocity() : moTextureFilter() {
  m_gravity = -1;
}


moTextureFilterVelocity::~moTextureFilterVelocity() {
}

MOboolean
moTextureFilterVelocity::Init(moGLManager* p_glman, moRenderManager* p_renderman, moTextureArray &p_src_tex, moTextureArray &p_dest_tex, moShader *p_shader, moTextFilterParam *p_params) {

  MOboolean result = moTextureFilterVelocity::Init( p_glman, p_renderman, p_src_tex, p_dest_tex, p_shader, p_params);

  moShader* m_shader = p_shader;
	moShaderGLSL* pglsl = NULL;

#ifdef SHADER_CG
    moShaderCG* pcg = NULL;
#endif

	if (m_shader->GetType()==MO_SHADER_CG) {
#ifdef SHADER_CG
        pcg = (moShaderCG*)m_shader;
#endif
	} else {
        pglsl = (moShaderGLSL*)m_shader;
	}

  moText uname = moText("gravity");
  if (pglsl) m_gravity = pglsl->GetUniformID(uname);

  return result;
}


void
moTextureFilterVelocity::Apply( moMoldeoObject *p_src_mob, moTempo* p_tempo, MOfloat p_fade, moTextFilterParam *p_params ) {

  moTextureFilter::Apply( p_src_mob, p_tempo, p_fade, p_params );

}


void
moTextureFilterVelocity::SetupShader(MOint w, MOint h, moTempo *p_tempo, MOfloat p_fade, moTextFilterParam *p_params, moMoldeoObject* p_src_object) {

  moTextureFilter::SetupShader( w, h, p_tempo, p_fade, p_params, p_src_object );

	if (-1 < m_gravity && p_src_object) {
        if (p_src_object->GetConfig()) {
          double gravity = p_src_object->GetConfig()->Eval( moR(PARTICLES_GRAVITY) );
          glUniform1fARB( m_gravity, gravity );
        }
	    //(m_shader->GetType() == (MOuint)MO_SHADER_GLSL) ? glUniform1fARB(m_fade_const, p_fade) : m_fade_const=0;/*cgGLSetParameter1f( (CGparameter)m_fade_const, p_fade )*/
	}

}


//========================
//  Efecto
//========================

moEffectParticlesFractal::moEffectParticlesFractal() {
	SetName("particlesfractal");

	m_pTFilter_MediumTexture = NULL;
	m_pMediumTexture = NULL;
  m_pTFilter_MediumTextureSwap = NULL;
  m_pMediumTextureSwap = NULL;
  m_bMediumTextureSwapOn = false;


	m_MediumTextureLoadedName = "";

	m_pTFilter_StateTexture = NULL;
	m_pStateTexture = NULL;
  m_pTFilter_StateTextureSwap = NULL;
  m_pStateTextureSwap = NULL;
  m_pStateTextureFinal = NULL;
  m_bStateTextureSwapOn = false;

	m_pTFilter_PositionTexture = NULL;
	m_pPositionTexture = NULL;
  m_pTFilter_PositionTextureSwap = NULL;
  m_pPositionTextureSwap = NULL;
  m_pPositionTextureFinal = NULL;
  m_bPositionTextureSwapOn = false;

	m_pTFilter_VelocityTexture = NULL;
	m_pVelocityTexture = NULL;
  m_pTFilter_VelocityTextureSwap = NULL;
  m_pVelocityTextureSwap = NULL;
  m_pVelocityTextureFinal = NULL;
  m_bVelocityTextureSwapOn = false;

	m_pTFilter_ScaleTexture = NULL;
	m_pScaleTexture = NULL;
  m_pTFilter_ScaleTextureSwap = NULL;
  m_pScaleTextureSwap = NULL;
  m_pScaleTextureFinal = NULL;
  m_bScaleTextureSwapOn = false;

  m_pTFilter_GeneticTexture = NULL;
	m_pGeneticTexture = NULL;
  m_pTFilter_GeneticTextureSwap = NULL;
  m_pGeneticTextureSwap = NULL;
  m_pGeneticTextureFinal = NULL;
  m_bGeneticTextureSwapOn = false;

  posArray = NULL;
  scaleArray = NULL;
  stateArray = NULL;
  colArray = NULL;
  geneticArray = NULL;

  g_fVBOSupported = false;
}

moEffectParticlesFractal::~moEffectParticlesFractal() {
	Finish();
}

moConfigDefinition *
moEffectParticlesFractal::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );

  p_configdefinition->Add( moText("incremento_madurez"), MO_PARAM_NUMERIC, PARTICLES_MATURITY, moValue( "0.03", "FUNCTION").Ref() );

  p_configdefinition->Add( moText("left_rep_angle_min"), MO_PARAM_NUMERIC, PARTICLES_LEFT_REP_ANGLE_MIN, moValue( "0.0", "FUNCTION").Ref() );
  p_configdefinition->Add( moText("left_rep_angle_max"), MO_PARAM_NUMERIC, PARTICLES_LEFT_REP_ANGLE_MAX, moValue( "0.0", "FUNCTION").Ref() );
  p_configdefinition->Add( moText("right_rep_angle_min"), MO_PARAM_NUMERIC, PARTICLES_RIGHT_REP_ANGLE_MIN, moValue( "0.0", "FUNCTION").Ref() );
  p_configdefinition->Add( moText("right_rep_angle_max"), MO_PARAM_NUMERIC, PARTICLES_RIGHT_REP_ANGLE_MAX, moValue( "0.0", "FUNCTION").Ref() );

  p_configdefinition->Add( moText("left_mov_angle_min"), MO_PARAM_NUMERIC, PARTICLES_LEFT_MOV_ANGLE_MIN, moValue( "0.0", "FUNCTION").Ref() );
  p_configdefinition->Add( moText("left_mov_angle_max"), MO_PARAM_NUMERIC, PARTICLES_LEFT_MOV_ANGLE_MIN, moValue( "0.0", "FUNCTION").Ref() );
  p_configdefinition->Add( moText("right_mov_angle_min"), MO_PARAM_NUMERIC, PARTICLES_RIGHT_MOV_ANGLE_MIN, moValue( "0.0", "FUNCTION").Ref() );
  p_configdefinition->Add( moText("right_mov_angle_max"), MO_PARAM_NUMERIC, PARTICLES_RIGHT_MOV_ANGLE_MAX, moValue( "0.0", "FUNCTION").Ref() );


	p_configdefinition->Add( moText("particlecolor"), MO_PARAM_COLOR, PARTICLES_PARTICLECOLOR );

	p_configdefinition->Add( moText("font"), MO_PARAM_FONT, PARTICLES_FONT, moValue( "Default", "TXT", "0", "NUM", "32.0", "NUM") );
	p_configdefinition->Add( moText("text"), MO_PARAM_TEXT, PARTICLES_TEXT, moValue( "Insert text in text parameter", "TXT") );
	p_configdefinition->Add( moText("ortho"), MO_PARAM_NUMERIC, PARTICLES_ORTHO, moValue( "0", "NUM").Ref() );


	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, PARTICLES_TEXTURE, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("folders"), MO_PARAM_TEXTUREFOLDER, PARTICLES_FOLDERS, moValue( "", "TXT") );

	p_configdefinition->Add( moText("texture_medium"), MO_PARAM_TEXTURE, PARTICLES_TEXTURE_MEDIUM, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("texture_mode"), MO_PARAM_NUMERIC, PARTICLES_TEXTUREMODE, moValue( "0", "NUM"), moText("UNIT,PATCH,MANY,MANY2PATCH,MANYBYORDER"));

	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, PARTICLES_BLENDING, moValue( "0", "NUM") );
	p_configdefinition->Add( moText("width"), MO_PARAM_NUMERIC, PARTICLES_WIDTH, moValue( "16", "NUM") );
	p_configdefinition->Add( moText("height"), MO_PARAM_NUMERIC, PARTICLES_HEIGHT, moValue( "16", "NUM") );

  p_configdefinition->Add( moText("sizex"), MO_PARAM_FUNCTION, PARTICLES_SIZEX, moValue( "10.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("sizey"), MO_PARAM_FUNCTION, PARTICLES_SIZEY, moValue( "10.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("sizez"), MO_PARAM_FUNCTION, PARTICLES_SIZEY, moValue( "10.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("gravity"), MO_PARAM_FUNCTION, PARTICLES_GRAVITY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("viscosity"), MO_PARAM_FUNCTION, PARTICLES_VISCOSITY, moValue( "0.8", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("maxage"), MO_PARAM_NUMERIC, PARTICLES_MAXAGE, moValue( "3000", "NUM").Ref() );
	p_configdefinition->Add( moText("emitionperiod"), MO_PARAM_NUMERIC, PARTICLES_EMITIONPERIOD, moValue( "10", "NUM").Ref() );
	p_configdefinition->Add( moText("emitionrate"), MO_PARAM_NUMERIC, PARTICLES_EMITIONRATE, moValue( "1", "NUM").Ref() );
	p_configdefinition->Add( moText("deathperiod"), MO_PARAM_NUMERIC, PARTICLES_DEATHPERIOD, moValue( "1", "NUM").Ref() );
	p_configdefinition->Add( moText("particlescript"), MO_PARAM_SCRIPT, PARTICLES_SCRIPT );

	p_configdefinition->Add( moText("randommethod"), MO_PARAM_NUMERIC, PARTICLES_RANDOMMETHOD, moValue( "0", "NUM").Ref(), moText("NOISY,COLINEAR,PERPENDICULAR") );
	p_configdefinition->Add( moText("creationmethod"), MO_PARAM_NUMERIC, PARTICLES_CREATIONMETHOD, moValue( "0", "NUM").Ref(), moText("LINEAR,PLANAR,VOLUMETRIC,CENTER") );
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
	p_configdefinition->Add( moText("drawing_features"), MO_PARAM_NUMERIC, PARTICLES_DRAWINGFEATURES, moValue( "0", "NUM") );

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

		p_configdefinition->Add( moText("random_color_function"), MO_PARAM_NUMERIC, PARTICLES_RANDOMCOLORFUNCTION, moValue( "0", "NUM").Ref(), moText("NONE,OSCILLATE,RED,GREEN,BLUE,YELLOW") );

	/*
		p_configdefinition->Add( moText("orderingmode"), MO_PARAM_NUMERIC, PARTICLES_ORDERING_MODE, moValue( "0", "NUM"), moText("NONE,ZDEPTHTEST,ZPOSITION,COMPLETE") );
	p_configdefinition->Add( moText("lightmode"), MO_PARAM_NUMERIC, PARTICLES_LIGHTMODE, moValue( "0", "NUM") );
	p_configdefinition->Add( moText("lightx"), MO_PARAM_FUNCTION, PARTICLES_LIGHTX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("lighty"), MO_PARAM_FUNCTION, PARTICLES_LIGHTY, moValue( "4.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("lightz"), MO_PARAM_FUNCTION, PARTICLES_LIGHTZ, moValue( "0.0", "FUNCTION").Ref() );
	*/
	return p_configdefinition;
}

MOboolean
moEffectParticlesFractal::Init()
{
    if (!PreInit()) return false;

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
    moDefineParamIndex( PARTICLES_TEXTURE_MEDIUM, moText("texture_medium") );
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
    /*
    moDefineParamIndex( PARTICLES_ORDERING_MODE, moText("orderingmode") );
    moDefineParamIndex( PARTICLES_LIGHTMODE, moText("lightmode") );
    moDefineParamIndex( PARTICLES_LIGHTX, moText("lightx") );
    moDefineParamIndex( PARTICLES_LIGHTY, moText("lighty") );
    moDefineParamIndex( PARTICLES_LIGHTZ, moText("lightz") );*/

    m_Physics.m_ParticleScript = moText("");

    m_Rate = 0;
    last_tick = 0;
    frame = 0;

    ortho = false;

    m_bTrackerInit = false;
    m_pTrackerData = NULL;


    ResetTimers();
    UpdateParameters();


/*
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

    //m_InletTuioSystemIndex = GetInletIndex("TUIOSYSTEM");
    //m_InletTrackerSystemIndex = GetInletIndex("TRACKERKLT");
*/



    return true;

}


void moEffectParticlesFractal::ResetTimers() {
/*
    TimerFullRevelation.Stop();
    TimerFullRestoration.Stop();
    TimerOfRevelation.Stop();
    TimerOfRestoration.Stop();

    FeatureActivity.Stop();
    MotionActivity.Stop();
    NoMotionActivity.Stop();
    m_Physics.EmitionTimer.Stop();

    for ( int i=0; i < m_ParticlesFractalArray.Count(); i++ ) {
          moParticlesFractal* pPar = m_ParticlesFractalArray[i];
          pPar->Age.Stop();
          pPar->Visible = false;
    }

    m_Physics.m_pLastBordParticle = NULL;
    */
}

void moEffectParticlesFractal::Shot( moText source, int shot_type, int shot_file ) {


}

void moEffectParticlesFractal::ReInit() {

    MODebug2->Push( moText("moEffectParticlesFractal::ReInit Face construction activated!!!!"));
/*
    int i;
    int j;
    int lum = 0;
    int lumindex = 0;
    int contrast = 0;

    UpdateParameters();
    //ResetTimers();

    m_pResourceManager->GetTimeMan()->ClearByObjectId(  this->GetId() );


    //m_ParticlesFractalArray.Init( p_cols*p_rows, NULL );
    //m_ParticlesFractalArrayTmp.Init( p_cols*p_rows, NULL );

    for( i=0; i<m_cols ; i++) {
        for( j=0; j<m_rows ; j++) {
            moParticlesFractal* pPar = m_ParticlesFractalArray[i+j*m_cols];

            if (pPar) {

                pPar->Pos = moVector2f( (float) i, (float) j);
                pPar->ImageProportion = 1.0;
                pPar->Color = moVector3f(1.0,1.0,1.0);
                pPar->GLId2 = 0;

                if (texture_mode==PARTICLES_TEXTUREMODE_MANY2PATCH) {

                    ///take the texture preselected
                    moTextureBuffer* pTexBuf = m_Config[moR(PARTICLES_FOLDERS)].GetData()->TextureBuffer();

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

                m_ParticlesFractalArray.Set( i + j*m_cols, pPar );

                moParticlesFractal* pParTmp = m_ParticlesFractalArrayTmp[i + j*m_cols];
                pParTmp->Pos3d = pPar->Pos3d;
                pParTmp->Velocity = pPar->Velocity;
                pParTmp->Mass = pPar->Mass;
                pParTmp->Force = pPar->Force;
                pParTmp->Fixed = pPar->Fixed;
                m_ParticlesFractalArrayTmp.Set( i + j*m_cols, pParTmp );

            } else MODebug2->Error(moText("ParticleSimple::ReInit::no particle pointer"));
        }
    }
*/
    //TrackParticle(1);

}

void moEffectParticlesFractal::UpdateDt() {

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
    m_EffectState.tempo.dt = dt;

    last_tick = m_EffectState.tempo.ticks;

}

void moEffectParticlesFractal::UpdateParameters() {


    m_rows = m_Config.Int( moR(PARTICLES_WIDTH)  );
    m_cols = m_Config.Int( moR(PARTICLES_HEIGHT)  );

    m_MediumTextureLoadedName = m_Config.Texture( moR(PARTICLES_TEXTURE_MEDIUM)).GetName();
    //MODebug2->Message("m_MediumTextureLoadedName:"+m_MediumTextureLoadedName);

    ///create shader and destination texture
    if ( !m_pStateTexture ) {

      InitParticlesFractal(   m_rows,
                              m_cols );

    }

    /// WE HAVE TO SWAP ON EVERY ITERATION....
    /// BETWEEN m_pTFilter_StateTextureSwap FILTER AND m_pTFilter_StateTexture FILTER
    ///

    moShader* pShader = NULL;
    moTextureIndex* pDestTex = NULL;
/*
    if ( m_bMediumTextureSwapOn && m_pTFilter_MediumTextureSwap ) {
        m_bMediumTextureSwapOn = false;
        m_pTFilter_MediumTextureSwap->Apply( &m_EffectState.tempo, 1.0f, m_pStateFilterParams );

    } else if ( m_pTFilter_MediumTexture ) {
        m_bMediumTextureSwapOn = true;
        m_pTFilter_MediumTexture->Apply( &m_EffectState.tempo, 1.0f, m_pStateFilterParams );
    }
*/
    if ( m_bStateTextureSwapOn && m_pTFilter_StateTextureSwap ) {
        m_bStateTextureSwapOn = false;
        //m_pTFilter_StateTextureSwap->Apply( &m_EffectState.tempo, 1.0f, m_pStateFilterParams );
        m_pTFilter_StateTextureSwap->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        m_pStateTextureFinal = m_pStateTextureSwap;

    } else if ( m_pTFilter_StateTexture ) {
        m_bStateTextureSwapOn = true;
        //m_pTFilter_StateTexture->Apply( &m_EffectState.tempo );
        m_pTFilter_StateTexture->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        m_pStateTextureFinal = m_pStateTexture;
    }


     if ( m_bGeneticTextureSwapOn && m_pTFilter_GeneticTextureSwap ) {
        m_bGeneticTextureSwapOn = false;
        //m_pTFilter_PositionTextureSwap->Apply( &m_EffectState.tempo );
        m_pTFilter_GeneticTextureSwap->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        m_pGeneticTextureFinal = m_pGeneticTextureSwap;
    } else if ( m_pTFilter_GeneticTexture ) {
        m_bGeneticTextureSwapOn = true;
        //m_pTFilter_PositionTexture->Apply( &m_EffectState.tempo );
        m_pTFilter_GeneticTexture->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        m_pGeneticTextureFinal = m_pGeneticTexture;
    }

    if ( m_bVelocityTextureSwapOn && m_pTFilter_VelocityTextureSwap ) {
        m_bVelocityTextureSwapOn = false;
        m_pTFilter_VelocityTextureSwap->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        //m_pTFilter_VelocityTextureSwap->Apply( &m_EffectState.tempo );
    } else if ( m_pTFilter_VelocityTexture ) {
        m_bVelocityTextureSwapOn = true;
        m_pTFilter_VelocityTexture->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        //m_pTFilter_VelocityTexture->Apply( &m_EffectState.tempo );
    }

    if ( m_bPositionTextureSwapOn && m_pTFilter_PositionTextureSwap ) {
        m_bPositionTextureSwapOn = false;
        //m_pTFilter_PositionTextureSwap->Apply( &m_EffectState.tempo );
        m_pTFilter_PositionTextureSwap->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        m_pPositionTextureFinal = m_pPositionTextureSwap;
    } else if ( m_pTFilter_PositionTexture ) {
        m_bPositionTextureSwapOn = true;
        //m_pTFilter_PositionTexture->Apply( &m_EffectState.tempo );
        m_pTFilter_PositionTexture->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        m_pPositionTextureFinal = m_pPositionTexture;
    }

    if ( m_bScaleTextureSwapOn && m_pTFilter_ScaleTextureSwap ) {
        m_bScaleTextureSwapOn = false;
        //m_pTFilter_PositionTextureSwap->Apply( &m_EffectState.tempo );
        m_pTFilter_ScaleTextureSwap->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        m_pScaleTextureFinal = m_pScaleTextureSwap;
    } else if ( m_pTFilter_PositionTexture ) {
        m_bScaleTextureSwapOn = true;
        //m_pTFilter_PositionTexture->Apply( &m_EffectState.tempo );
        m_pTFilter_ScaleTexture->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        m_pScaleTextureFinal = m_pScaleTexture;
    }

    if (stateArray==NULL) {
      numParticles = m_rows * m_cols;
      stateArray = new GLfloat[4 * numParticles]();
      MODebug2->Push("Reading pixels: " +  IntToStr(numParticles) );
    }

    if (stateArray && m_pStateTextureFinal) {
      //MODebug2->Push("Reading pixels: " +  IntToStr(numParticles) );
      //m_pPositionTextureFinal->GetFBO()->Bind();

      //m_pPositionTextureFinal->GetFBO()->SetReadTexture( m_pPositionTextureFinal->GetGLId() );
      //glReadPixels(0, 0, m_rows, m_cols, GL_RGBA, GL_FLOAT, posArray);
      //m_pPositionTextureFinal->GetFBO()->Unbind();

      glBindTexture( GL_TEXTURE_2D, m_pStateTextureFinal->GetGLId() );
      glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, stateArray );
    }

    if (posArray==NULL) {
      numParticles = m_rows * m_cols;
      posArray = new GLfloat[4 * numParticles]();
      MODebug2->Push("Reading pixels: " +  IntToStr(numParticles) );
    }

    if (posArray && m_pPositionTextureFinal) {
      //MODebug2->Push("Reading pixels: " +  IntToStr(numParticles) );
      //m_pPositionTextureFinal->GetFBO()->Bind();

      //m_pPositionTextureFinal->GetFBO()->SetReadTexture( m_pPositionTextureFinal->GetGLId() );
      //glReadPixels(0, 0, m_rows, m_cols, GL_RGBA, GL_FLOAT, posArray);
      //m_pPositionTextureFinal->GetFBO()->Unbind();

      glBindTexture( GL_TEXTURE_2D, m_pPositionTextureFinal->GetGLId() );
      glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, posArray );
    }

    if (scaleArray==NULL) {
      numParticles = m_rows * m_cols;
      scaleArray = new GLfloat[4 * numParticles]();
      MODebug2->Push("Reading pixels: " +  IntToStr(numParticles) );
    }

    if (scaleArray && m_pScaleTextureFinal) {
      //MODebug2->Push("Reading pixels: " +  IntToStr(numParticles) );
      //m_pPositionTextureFinal->GetFBO()->Bind();

      //m_pPositionTextureFinal->GetFBO()->SetReadTexture( m_pPositionTextureFinal->GetGLId() );
      //glReadPixels(0, 0, m_rows, m_cols, GL_RGBA, GL_FLOAT, posArray);
      //m_pPositionTextureFinal->GetFBO()->Unbind();

      glBindTexture( GL_TEXTURE_2D, m_pScaleTextureFinal->GetGLId() );
      glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, scaleArray );
    }

    if (geneticArray==NULL) {
      numParticles = m_rows * m_cols;
      geneticArray = new GLfloat[4 * numParticles]();
      MODebug2->Push("Reading pixels: " +  IntToStr(numParticles) );
    }

    if (geneticArray && m_pGeneticTextureFinal) {
      //MODebug2->Push("Reading pixels: " +  IntToStr(numParticles) );
      //m_pPositionTextureFinal->GetFBO()->Bind();

      //m_pPositionTextureFinal->GetFBO()->SetReadTexture( m_pPositionTextureFinal->GetGLId() );
      //glReadPixels(0, 0, m_rows, m_cols, GL_RGBA, GL_FLOAT, posArray);
      //m_pPositionTextureFinal->GetFBO()->Unbind();

      glBindTexture( GL_TEXTURE_2D, m_pGeneticTextureFinal->GetGLId() );
      glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, geneticArray );
    }

/**

m_pResourceManager->GetFBMan()->GetFBO(FBO[2])->SetReadTexture(m_pResourceManager->GetTextureMan()->GetFBOAttachPoint(texfpPos[readPosTex]));
  glReadPixels( 0, 0, , , GL_RGBA, GL_FLOAT, posArray );

*/

//m_pTFilter_PositionTexture


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
/*
    m_Physics.m_SourceLighMode = (moParticlesSimpleLightMode) m_Config.Int( moR(PARTICLES_LIGHTMODE));
    m_Physics.m_SourceLightVector = moVector3f(
                                        m_Config.Eval( moR(PARTICLES_LIGHTX)),
                                        m_Config.Eval( moR(PARTICLES_LIGHTY)),
                                        m_Config.Eval( moR(PARTICLES_LIGHTZ))
                                       );
*/
    m_Physics.gravitational = m_Config.Eval( moR(PARTICLES_GRAVITY));
    m_Physics.viscousdrag = m_Config.Eval( moR(PARTICLES_VISCOSITY));



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


}

void moEffectParticlesFractal::SetParticlePosition( moParticlesFractal* pParticle ) {
/**
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

    randomposx = (m_Physics.m_RandomPosition>0.0)? (0.5-moMathf::UnitRandom())*m_Physics.m_RandomPosition*m_Physics.m_PositionVector.X() : m_Physics.m_PositionVector.X();
    randomposy = (m_Physics.m_RandomPosition>0.0)? (0.5-moMathf::UnitRandom())*m_Physics.m_RandomPosition*m_Physics.m_PositionVector.Y() : m_Physics.m_PositionVector.Y();
    randomposz = (m_Physics.m_RandomPosition>0.0)? (0.5-moMathf::UnitRandom())*m_Physics.m_RandomPosition*m_Physics.m_PositionVector.Z() : m_Physics.m_PositionVector.Z();

    randomvelx = (m_Physics.m_RandomVelocity>0.0)? (0.5-moMathf::UnitRandom())*m_Physics.m_RandomVelocity*m_Physics.m_VelocityVector.X() : m_Physics.m_VelocityVector.X();
    randomvely = (m_Physics.m_RandomVelocity>0.0)? (0.5-moMathf::UnitRandom())*m_Physics.m_RandomVelocity*m_Physics.m_VelocityVector.Y() : m_Physics.m_VelocityVector.Y();
    randomvelz = (m_Physics.m_RandomVelocity>0.0)? (0.5-moMathf::UnitRandom())*m_Physics.m_RandomVelocity*m_Physics.m_VelocityVector.Z() : m_Physics.m_VelocityVector.Z();

    moVector4d fullcolor;
    fullcolor = m_Config.EvalColor( moR(PARTICLES_PARTICLECOLOR) , m_EffectState.tempo.ang );
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
                case  PARTICLES_CREATIONMETHOD_PLANAR:
                case  PARTICLES_CREATIONMETHOD_VOLUMETRIC:
                    alpha = 2 * moMathf::PI *  ( pParticle->Pos.X()*m_rows + pParticle->Pos.Y()) / ((double)m_cols*(double)m_rows );
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
*/

}

void moEffectParticlesFractal :: BuildVBOs()
{
    // Generate And Bind The Vertex Buffer
    glGenBuffersARB( 1, &m_nVBOVertices );                  // Get A Valid Name
    glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_nVBOVertices );         // Bind The Buffer
    // Load The Data
    glBufferDataARB( GL_ARRAY_BUFFER_ARB, m_nVertexCount*3*sizeof(float), m_pVertices, GL_STATIC_DRAW_ARB );

    // Generate And Bind The Texture Coordinate Buffer
    glGenBuffersARB( 1, &m_nVBOTexCoords );                 // Get A Valid Name
    glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_nVBOTexCoords );        // Bind The Buffer
    // Load The Data
    glBufferDataARB( GL_ARRAY_BUFFER_ARB, m_nVertexCount*2*sizeof(float), m_pTexCoords, GL_STATIC_DRAW_ARB );

    // Our Copy Of The Data Is No Longer Necessary, It Is Safe In The Graphics Card
    delete [] m_pVertices; m_pVertices = NULL;
    delete [] m_pTexCoords; m_pTexCoords = NULL;
}

void moEffectParticlesFractal::InitParticlesFractal( int p_cols, int p_rows, bool p_forced ) {

  int Mid = -1;

  moTexParam tparam = MODefTex2DParams;

  tparam.internal_format = GL_RGBA32F;
  tparam.min_filter = GL_NEAREST;
  tparam.mag_filter = GL_NEAREST;
  tparam.target = GL_TEXTURE_2D;
  tparam.wrap_s = GL_REPEAT;
  tparam.wrap_t = GL_REPEAT;

  moTextureFilterIndex* pTextureFilterIndex = GetResourceManager()->GetShaderMan()->GetTextureFilterIndex();

  p_cols = (int)moTexture::NextPowerOf2(p_cols);
  p_rows = max( p_cols, (int)moTexture::NextPowerOf2(p_rows));
  p_cols = max( p_cols, p_rows );

  //pos father x, fathery, age, iteration
  moText strResolution = IntToStr(p_cols)+"x"+IntToStr(p_rows);

  moText tName;

/**

  MEDIUM
  x: mass [BODY]
  y: frequency [SOUND]
  z: color [LIGHT]
  w: orientation [VIEW]

*/


  tName = "pf_medium_swap_fx#"+this->GetLabelName()+"_";

  Mid = TextureMan()->AddTexture( tName, p_cols, p_rows, tparam );
  if (Mid>0) {
      m_pMediumTextureSwap = TextureMan()->GetTexture(Mid);
      m_pMediumTextureSwap->BuildEmpty( p_cols, p_rows );
      //TextureMan()->GetTexture(Mid)->BuildEmpty( p_cols, p_rows );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }


  tName = "pf_medium_fx#"+this->GetLabelName()+"_";
  Mid = TextureMan()->AddTexture( tName, p_cols, p_rows, tparam );
  if (Mid>0) {
      m_pMediumTexture = TextureMan()->GetTexture(Mid);
      m_pMediumTexture->BuildEmpty( p_cols, p_rows );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }

/**
  /// CREATE FILTER FOR MEDIUM ADAPTATION
  if ( !m_pTFilter_MediumTextureSwap && m_pMediumTexture && m_pMediumTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add( m_pMediumTexture->GetName()
                      +  moText(" ")+this->GetLabelName()+moText("/Medium.cfg " )
                      + m_pMediumTextureSwap->GetName() );
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {

        m_pTFilter_MediumTextureSwap = pTextureFilterIndex->Get(idx-1);
        m_pMediumFilterParams = new moStateFilterParams();
        MODebug2->Message( moText("filter loaded m_pTFilter_MediumTextureSwap: ") + m_pTFilter_MediumTextureSwap->GetTextureFilterLabelName() );
    }
  }
  */
/**
 /// CREATE FILTER FOR MEDIUM ADAPTATION SWAP
  if ( !m_pTFilter_MediumTexture && m_pMediumTexture && m_pMediumTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add(
                      m_pMediumTextureSwap->GetName()
                      //+ moText(" ") + m_MediumTextureLoadedName
                      //+  moText(" shaders/Birth.cfg res:64x64 " )
                      +  moText(" ")+this->GetLabelName()+moText("/Medium.cfg " )
                      + m_pMediumTexture->GetName() );
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_MediumTexture = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_MediumTexture: ") + m_pTFilter_MediumTexture->GetTextureFilterLabelName() );
    }
  }
*/

/**

  STATE
  x: generation or iteration ( number of reproduction cycles )
  y: maturity reproduction
  z: maturity cycle
  w: origin generation (when this particle is born, its assigned a generation ) or 1.0f if not used

*/


  tName = "pf_state_swap_fx#"+this->GetLabelName()+"_";

  Mid = TextureMan()->AddTexture( tName, p_cols, p_rows, tparam );
  if (Mid>0) {
      m_pStateTextureSwap = TextureMan()->GetTexture(Mid);
      m_pStateTextureSwap->BuildEmpty( p_cols, p_rows );
      //TextureMan()->GetTexture(Mid)->BuildEmpty( p_cols, p_rows );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }


  tName = "pf_state_fx#"+this->GetLabelName()+"_";
  Mid = TextureMan()->AddTexture( tName, p_cols, p_rows, tparam );
  if (Mid>0) {
      m_pStateTexture = TextureMan()->GetTexture(Mid);
      m_pStateTexture->BuildEmpty( p_cols, p_rows );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }

  /// CREATE FILTER FOR STATE CHANGES (BIRTH AND DEATH) !!!
  if ( !m_pTFilter_StateTextureSwap && m_pStateTexture && m_pStateTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add( m_pStateTexture->GetName()
                      //+  moText(" shaders/Birth.cfg res:64x64 " )
                      +  moText(" ")+this->GetLabelName()+moText("/Birth.cfg " )
                      + m_pStateTextureSwap->GetName() );
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {

        m_pTFilter_StateTextureSwap = pTextureFilterIndex->Get(idx-1);
        m_pStateFilterParams = new moStateFilterParams();
        MODebug2->Message( moText("filter loaded m_pTFilter_StateTextureSwap: ") + m_pTFilter_StateTextureSwap->GetTextureFilterLabelName() );
    }
  }

 /// CREATE FILTER FOR STATE CHANGES (BIRTH AND DEATH) !!!
  if ( !m_pTFilter_StateTexture && m_pStateTexture && m_pStateTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add( m_pStateTextureSwap->GetName()
                      //+  moText(" shaders/Birth.cfg res:64x64 " )
                      +  moText(" ")+this->GetLabelName()+moText("/Birth.cfg " )
                      + m_pStateTexture->GetName() );
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_StateTexture = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_StateTexture: ") + m_pTFilter_StateTexture->GetTextureFilterLabelName() );
    }
  }



/**

  GENETIC

*/

  tName = "pf_genetic_fx#"+this->GetLabelName()+"_";
  Mid = TextureMan()->AddTexture( tName, p_cols, p_rows, tparam );
  if (Mid>0) {
      m_pGeneticTexture = TextureMan()->GetTexture(Mid);
      m_pGeneticTexture->BuildEmpty( p_cols, p_rows );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }

  tName = "pf_genetic_swap_fx#"+this->GetLabelName()+"_";
  Mid = TextureMan()->AddTexture( tName, p_cols, p_rows, tparam );
  if (Mid>0) {
      m_pGeneticTextureSwap = TextureMan()->GetTexture(Mid);
      m_pGeneticTextureSwap->BuildEmpty( p_cols, p_rows );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }

  /// CREATE FILTER FOR GENETIC CHANGES (MUTATIONS!) !!!
  if ( !m_pTFilter_GeneticTexture
      && m_pGeneticTextureSwap && m_pGeneticTexture && m_pStateTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add( m_pStateTextureSwap->GetName()
                      + " " + m_pGeneticTexture->GetName()
                      //+  moText(" shaders/Birth.cfg res:64x64 " )
                      +  moText(" ")+this->GetLabelName()+moText("/Genetic.cfg " )
                      + m_pGeneticTextureSwap->GetName() );
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_GeneticTexture = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_GeneticTexture: ") + m_pTFilter_GeneticTexture->GetTextureFilterLabelName() );
    }
  }

  /// CREATE FILTER FOR GENETIC CHANGES (MUTATIONS!) !!!
  if ( !m_pTFilter_GeneticTextureSwap
      && m_pGeneticTextureSwap && m_pGeneticTexture && m_pStateTexture ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add( m_pStateTexture->GetName()
                      + " " + m_pGeneticTextureSwap->GetName()
                      //+  moText(" shaders/Birth.cfg res:64x64 " )
                      +  moText(" ")+this->GetLabelName()+moText("/Genetic.cfg " )
                      + m_pGeneticTexture->GetName() );
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_GeneticTextureSwap = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_GeneticTextureSwap: ") + m_pTFilter_GeneticTextureSwap->GetTextureFilterLabelName() );
    }
  }

/**

  VELOCITY

*/

  //vx,vy,vz
  tName = "pf_velocity_fx#"+this->GetLabelName()+"_";
  Mid = TextureMan()->AddTexture( tName, p_cols, p_rows, tparam );
  if (Mid>0) {
      m_pVelocityTexture = TextureMan()->GetTexture(Mid);
      m_pVelocityTexture->BuildEmpty( p_cols, p_rows );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }

  //vx,vy,vz
  tName = "pf_velocity_swap_fx#"+this->GetLabelName()+"_";
  Mid = TextureMan()->AddTexture( tName, p_cols, p_rows, tparam );
  if (Mid>0) {
      m_pVelocityTextureSwap = TextureMan()->GetTexture(Mid);
      m_pVelocityTextureSwap->BuildEmpty( p_cols, p_rows );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }

//pos x, y, z, alive
  tName = "pf_position_fx#"+this->GetLabelName()+"_";
  Mid = TextureMan()->AddTexture( tName, p_cols, p_rows, tparam );
  if (Mid>0) {
      m_pPositionTexture = TextureMan()->GetTexture(Mid);
      m_pPositionTexture->BuildEmpty( p_cols, p_rows );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }

  //pos x, y, z, alive SWAP
  tName = "pf_position_swap_fx#"+this->GetLabelName()+"_";
  Mid = TextureMan()->AddTexture( tName, p_cols, p_rows, tparam );
  if (Mid>0) {
      m_pPositionTextureSwap = TextureMan()->GetTexture(Mid);
      m_pPositionTextureSwap->BuildEmpty( p_cols, p_rows );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }

  /// CREATE FILTER FOR VELOCITY CHANGES (BIRTH AND DEATH) !!!
  if ( !m_pTFilter_VelocityTexture && m_pStateTextureSwap
      && m_pVelocityTexture && m_pVelocityTextureSwap
      && m_pGeneticTextureSwap && m_pPositionTextureSwap) {
    moTextArray copy_filter_0;
    copy_filter_0.Add( m_pStateTextureSwap->GetName()
                      + " "
                      + m_pGeneticTextureSwap->GetName()
                      + " "
                      + m_pVelocityTexture->GetName()
                      + " " + m_pPositionTextureSwap->GetName()
                      + " " + m_MediumTextureLoadedName
                      //+  moText(" shaders/Birth.cfg res:64x64 " )
                      +  moText(" ")+this->GetLabelName()+moText("/Velocity.cfg " )
                      + m_pVelocityTextureSwap->GetName() );

    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_VelocityTexture = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_VelocityTexture: ") + m_pTFilter_VelocityTexture->GetTextureFilterLabelName() );
    }
  }

  /// CREATE FILTER FOR VELOCITY CHANGES (BIRTH AND DEATH) !!!
  if ( !m_pTFilter_VelocityTextureSwap && m_pStateTexture
      && m_pVelocityTexture && m_pVelocityTextureSwap && m_pPositionTexture ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add( m_pStateTexture->GetName()
                      + " "
                      + m_pGeneticTexture->GetName()
                      + " "
                      +  m_pVelocityTextureSwap->GetName()
                      + " " + m_pPositionTexture->GetName()
                      + " " + m_MediumTextureLoadedName
                      //+  moText(" shaders/Birth.cfg res:64x64 " )
                      +  moText(" ")+this->GetLabelName()+moText("/Velocity.cfg " )
                      + m_pVelocityTexture->GetName() );
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_VelocityTextureSwap = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_VelocityTextureSwap: ") + m_pTFilter_VelocityTextureSwap->GetTextureFilterLabelName() );
    }
  }

/**

  POSITION

*/




  /// CREATE FILTER FOR POSITION CHANGES (BIRTH AND DEATH) !!!
  if ( !m_pTFilter_PositionTextureSwap && m_pPositionTexture && m_pPositionTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add(
                      //+  moText(" shaders/Birth.cfg res:64x64 " )
                      m_pStateTextureSwap->GetName()
                      + " " + m_pVelocityTextureSwap->GetName()
                      + " " + m_pPositionTexture->GetName()
                      + " " + m_MediumTextureLoadedName
                      + moText(" ")+this->GetLabelName()+moText("/Position.cfg" )
                      + " " + m_pPositionTextureSwap->GetName() );
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_PositionTextureSwap = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_PositionTextureSwap: ") + m_pTFilter_PositionTextureSwap->GetTextureFilterLabelName() );
    }
  }

  /// CREATE FILTER FOR POSITION CHANGES (BIRTH AND DEATH) !!!
  if ( !m_pTFilter_PositionTexture && m_pPositionTexture && m_pPositionTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add(//+  moText(" shaders/Birth.cfg res:64x64 " )
                      m_pStateTexture->GetName()
                      + " " + m_pVelocityTexture->GetName()
                      + " " + m_pPositionTextureSwap->GetName()
                      + " " + m_MediumTextureLoadedName
                      + moText(" ")+this->GetLabelName()+moText("/Position.cfg" )
                      + " " + m_pPositionTexture->GetName() );
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_PositionTexture = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_PositionTexture: ") + m_pTFilter_PositionTexture->GetTextureFilterLabelName() );
    }
  }


/**

  SCALE

*/

  //sx,sy,sz
  tName = "particlesfractal_scale_fx#"+this->GetLabelName()+"_";
  Mid = TextureMan()->AddTexture( tName, p_cols, p_rows, tparam );
  if (Mid>0) {
      m_pScaleTexture = TextureMan()->GetTexture(Mid);
      m_pScaleTexture->BuildEmpty( p_cols, p_rows );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }

  tName = "particlesfractal_scale_swap_fx#"+this->GetLabelName()+"_";
  Mid = TextureMan()->AddTexture( tName, p_cols, p_rows, tparam );
  if (Mid>0) {
      m_pScaleTextureSwap = TextureMan()->GetTexture(Mid);
      m_pScaleTextureSwap->BuildEmpty( p_cols, p_rows );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }

  //GL_ARB_vertex_buffer_object
  /// CREATE FILTER FOR SCALE !! (SWAP)
  if ( !m_pTFilter_ScaleTextureSwap && m_pScaleTexture && m_pScaleTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add(
                      //+  moText(" shaders/Birth.cfg res:64x64 " )
                      m_pStateTextureSwap->GetName()
                      + " " + m_pPositionTextureSwap->GetName()
                      + " " + m_pScaleTexture->GetName()
                      + " " + m_MediumTextureLoadedName
                      + moText(" ")+this->GetLabelName()+moText("/Scale.cfg" )
                      + " " + m_pScaleTextureSwap->GetName() );
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_ScaleTextureSwap = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_PositionTextureSwap: ") + m_pTFilter_ScaleTextureSwap->GetTextureFilterLabelName() );
    }
  }

  /// CREATE FILTER FOR SCALE !! ()
  if ( !m_pTFilter_ScaleTexture && m_pScaleTexture && m_pScaleTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add(//+  moText(" shaders/Birth.cfg res:64x64 " )
                      m_pStateTexture->GetName()
                      + " " + m_pPositionTexture->GetName()
                      + " " + m_pScaleTextureSwap->GetName()
                      + " " + m_MediumTextureLoadedName
                      + moText(" ")+this->GetLabelName()+moText("/Scale.cfg" )
                      + " " + m_pScaleTexture->GetName() );
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_ScaleTexture = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_ScaleTexture: ") + m_pTFilter_ScaleTexture->GetTextureFilterLabelName() );
    }
  }



}

void moEffectParticlesFractal::Regenerate() {

}

void moEffectParticlesFractal::ParticlesFractalInfluence( float posx, float posy, float velx, float vely, float veln ) {

}

void moEffectParticlesFractal::RevealingAll() {


}

void moEffectParticlesFractal::RestoringAll() {

}


void moEffectParticlesFractal::CalculateForces(bool tmparray)
{

}

void moEffectParticlesFractal::UpdateParticles( double dt,int method )
{

}

void moEffectParticlesFractal::CalculateDerivatives(bool tmparray, double dt)
{
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
double moEffectParticlesFractal::Solver1D(double h,double y0,int method,double (*fcn)(double))
{
   return(0);
}


void moEffectParticlesFractal::ParticlesFractalAnimation( moTempo* tempogral, moEffectState* parentstate ) {


}

void moEffectParticlesFractal::TrackParticle( int partid ) {

}

void moEffectParticlesFractal::DrawParticlesFractal( moTempo* tempogral, moEffectState* parentstate ) {

  //glDisable(GL_TEXTURE_2D);
  glPointSize(4.0f);
  moText Tpositions;
  int max_scale_iterations = int( log2(m_rows) );
  int max_generations = max_scale_iterations * 2;

//  SetColor( m_Config[moR(PARTICLES_COLOR)][MO_SELECTED], m_Config[moR(PARTICLES_ALPHA)][MO_SELECTED], m_EffectState );

  float scalex = m_Config.Eval( moR(PARTICLES_SCALEX_PARTICLE) );
  float scaley = m_Config.Eval( moR(PARTICLES_SCALEY_PARTICLE) );
  float scalez = m_Config.Eval( moR(PARTICLES_SCALEZ_PARTICLE) );
  float sizex = 1.0 * m_Physics.m_EmitterSize.X() / (1.0+abs(m_rows));
  float sizey = 1.0 * m_Physics.m_EmitterSize.Y() / (1.0+abs(m_cols));
  float sizez = 0.08f;

  float tcoordx = 0.0f;
  float tcoordy = 0.0f;
  float tsizex = 1.0f;
  float tsizey = 1.0f;
  float zoff = 0.001f/(1+m_cols)*(1+m_rows);

  float alpha = m_EffectState.alpha*m_Config.Eval( moR(PARTICLES_ALPHA) );
  int ioff,joff,ijoff;

  if (posArray && stateArray && geneticArray)
  for (int i = 0; i < m_cols; i++) {
      ioff = i * 4;
      for (int j = 0; j < m_rows; j++)
      {
        joff = j * m_cols * 4;
        ijoff = ioff+joff;
        float w = posArray[ijoff + 3];

        if (w>0.0) {
          float x = posArray[ijoff];
          float y = posArray[ijoff + 1];
          float z = posArray[ijoff + 2];

          //z = z + ijoff*zoff*0.0f;

          float generation = stateArray[ijoff];
          float maturity = stateArray[ijoff+1];
          float age = stateArray[ijoff+2];
          float origin = stateArray[ijoff+3];

          float r = geneticArray[ijoff]*m_Color.X();
          float g = geneticArray[ijoff+1]*m_Color.Y();
          float b = geneticArray[ijoff+2]*m_Color.Z();
          float al = geneticArray[ijoff+3]*m_Color.W();
          glColor4f( r,g,b,al*m_Alpha );

          moVector3f U,V,W;
          moVector3f A,B,C,D;

          moVector3f CENTRO;

          U = moVector3f( 0.0f, 0.0f, 1.0f );
          V = moVector3f( 1.0f, 0.0f, 0.0f );
          W = moVector3f( 0.0f, 1.0f, 0.0f );

          //U = CO;
          U.Normalize();

          ///glPointSize(1.0f + 0.5f*age + 0.5f*max_generations*generation );
  /**
            glPointSize( 1.0f + origin );
            glBegin(GL_POINTS);
              if (w>0.0f) {

              //if (w==0.3f) glColor4f( 1.0, 0.0, 0.0, 1.0f );
                //else if (w==0.9f) glColor4f( 0.0, 0.0, 1.0, 1.0f );
                //glColor4f( age+ 0.5f*max_generations*generation, age, age, 1.0f );
                glColor4f( 0.5f+generation*0.2f, 0.5f+generation*0.2f, 0.5f+generation*0.2f, 0.5f );

              } else glColor4f( 1.0, 1.0, 1.0, 0.5f );
              glVertex3f(x, y, z);
            glEnd();
  */
  /**
                if (j < m_rows/4 ) {
                  glColor4f(  1.0, 0.0, 0.0, alpha  );
                } else
                if (j < 2*m_rows/4 ) {
                  glColor4f(  0.0, 1.0, 0.0, alpha  );
                } else
                if (j < 3*m_rows/4 ) {
                  glColor4f(  0.0, 0.0, 1.0, alpha  );
                } else
                if (j < 4*m_rows/4 ) {
                  glColor4f(  0.5, 0.5, 0.5, alpha  );
                }
  */
            /**
            if (w==0.83f) glColor4f( 1.0, 1.0, 1.0, 1.0f );
            else if (w==0.9f) glColor4f( 0.0, 0.0, 1.0, 0.25f );
            else glColor4f( 1.0, 0.0, 0.0, 0.5f );
            */
  //glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
  /*
  x = 0.0;
  y = 0.0;
  z = 0.0;
  */

            glBegin(GL_QUADS);
              glTexCoord2f( tcoordx, tcoordy );
              glVertex3f( x-sizex*scalex, y-sizey*scaley, z);

              glTexCoord2f( tcoordx+tsizex, tcoordy );
              glVertex3f( x+sizex*scalex, y-sizey*scaley, z);

              glTexCoord2f( tcoordx+tsizex, tcoordy+tsizey );
              glVertex3f( x+sizex*scalex, y+sizey*scaley, z);

              glTexCoord2f( tcoordx, tcoordy+tsizey );
              glVertex3f( x-sizex*scalex, y+sizey*scaley, z);
            glEnd();
        }

      }
  }

  //MODebug2->Push( "positions:" + Tpositions );

  glEnable(GL_TEXTURE_2D);
}
#ifdef USE_TUIO
using namespace TUIO;
#endif
void moEffectParticlesFractal::DrawTracker() {

}


void moEffectParticlesFractal::Draw( moTempo* tempogral, moEffectState* parentstate)
{

    int ancho,alto;
    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();
    frame++;
    moFont* pFont = m_Config[moR(PARTICLES_FONT)].GetData()->Font();

    UpdateParameters();

    PreDraw( tempogral, parentstate);

    if (ortho) {
        glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
        //glDepthMask(GL_FALSE);
        glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
        glLoadIdentity();									// Reset The Projection Matrix
        glOrtho(-0.5,0.5,-0.5*h/w,0.5*h/w,-1,1);                              // Set Up An Ortho Screen
    } else {
        glDisable(GL_DEPTH_TEST);
        //glDepthMask(GL_FALSE);
        glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
        glLoadIdentity();									// Reset The Projection Matrix
        m_pResourceManager->GetGLMan()->SetPerspectiveView( w, h );
    }

    glMatrixMode(GL_PROJECTION);

    if (!ortho) {
        if ( m_EffectState.stereoside == MO_STEREO_NONE ) {

            gluLookAt(		m_Physics.m_EyeVector.X(),
                            m_Physics.m_EyeVector.Y(),
                            m_Physics.m_EyeVector.Z(),
                            m_Physics.m_TargetViewVector.X(),
                            m_Physics.m_TargetViewVector.Y(),
                            m_Physics.m_TargetViewVector.Z(),
                            m_Physics.m_UpViewVector.X(), m_Physics.m_UpViewVector.Y(), m_Physics.m_UpViewVector.Z());

        }
    }

    if (texture_mode==PARTICLES_TEXTUREMODE_UNIT || texture_mode==PARTICLES_TEXTUREMODE_PATCH) {
      glid = m_Config.GetGLId( moR(PARTICLES_TEXTURE), &m_EffectState.tempo);
    }

    glid_medium = m_Config.GetGLId( moR(PARTICLES_TEXTURE_MEDIUM), &m_EffectState.tempo);


    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glLoadIdentity();									// Reset The View

    glEnable(GL_ALPHA);

    glShadeModel(GL_SMOOTH);

    tx = m_Config.Eval( moR(PARTICLES_TRANSLATEX));
    ty = m_Config.Eval( moR(PARTICLES_TRANSLATEY));
    tz = m_Config.Eval( moR(PARTICLES_TRANSLATEZ));


    rz = m_Config.Eval( moR(PARTICLES_ROTATEZ));

    sx = m_Config.Eval( moR(PARTICLES_SCALEX));
    sy = m_Config.Eval( moR(PARTICLES_SCALEY));
    sz = m_Config.Eval( moR(PARTICLES_SCALEZ));


//scale
	glScalef(   sx,
              sy,
              sz);

    glTranslatef(   tx,
                    ty,
                    tz );

    //rotation
    glRotatef(  rz, 0.0, 0.0, 1.0 );
    glRotatef(  m_Config.Eval( moR(PARTICLES_ROTATEY)), 0.0, 1.0, 0.0 );
    glRotatef(  m_Config.Eval( moR(PARTICLES_ROTATEX)), 1.0, 0.0, 0.0 );



    //blending
    SetBlending( (moBlendingModes) m_Config.Int( moR(PARTICLES_BLENDING) ) );
/*
    //set image
    moTexture* pImage = (moTexture*) m_Config[moR(PARTICLES_TEXTURE)].GetData()->Pointer();
*/
    //color
    SetColor( m_Config[moR(PARTICLES_COLOR)][MO_SELECTED], m_Config[moR(PARTICLES_ALPHA)][MO_SELECTED], m_EffectState );

    m_Color = m_Config.EvalColor( moR(PARTICLES_COLOR) );
    m_Alpha = m_Config.Eval(moR(PARTICLES_ALPHA));


	moText Texto = m_Config.Text( moR(PARTICLES_TEXT) );
    if (glid>=0) glBindTexture( GL_TEXTURE_2D, glid );
        else glBindTexture( GL_TEXTURE_2D, 0);

/*
    glColor4f(0.0, 1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
      glTexCoord2f( 0.0, 1.0 );
      glVertex2f( -0.5, -0.5);

      glTexCoord2f( 1.0, 1.0 );
      glVertex2f(  0.5, -0.5);

      glTexCoord2f( 1.0, 0.0 );
      glVertex2f(  0.5,  0.5);

      glTexCoord2f( 0.0, 0.0 );
      glVertex2f( -0.5,  0.5);
    glEnd();*/

    DrawParticlesFractal( tempogral, parentstate );
/*
  // Set Pointers To Our Data
  if( g_fVBOSupported )
  {
      glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_nVBOVertices );
      glVertexPointer( 3, GL_FLOAT, 0, (char *) NULL );       // Set The Vertex Pointer To The Vertex Buffer
      glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_nVBOTexCoords );
      glTexCoordPointer( 2, GL_FLOAT, 0, (char *) NULL );     // Set The TexCoord Pointer To The TexCoord Buffer
  } else
  {
      glVertexPointer( 3, GL_FLOAT, 0, m_pVertices ); // Set The Vertex Pointer To Our Vertex Data
      glTexCoordPointer( 2, GL_FLOAT, 0, m_pTexCoords );  // Set The Vertex Pointer To Our TexCoord Data
  }

  glDrawArrays( GL_TRIANGLES, 0, m_nVertexCount );
  glDisableClientState( GL_VERTEX_ARRAY );
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
*/

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix

    EndDraw();
}

void moEffectParticlesFractal::setUpLighting()
{
  /**
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
//  glEnable(GL_LIGHT2);

   float front_emission[4] = { 1, 1, 1, 0.0 };
   float front_ambient[4]  = { 0.2, 0.2, 0.2, 0.0 };
   float front_diffuse[4]  = { 0.95, 0.95, 0.8, 0.0 };
   float front_specular[4] = { 0.6, 0.6, 0.6, 0.0 };
   glMaterialfv(GL_FRONT, GL_EMISSION, front_emission);
   glMaterialfv(GL_FRONT, GL_AMBIENT, front_ambient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, front_diffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, front_specular);
   glMaterialf(GL_FRONT, GL_SHININESS, 16.0);
   glColor4fv(front_diffuse);

   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
   glColorMaterial(GL_FRONT, GL_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);

   glEnable(GL_LIGHTING);
   */
}


MOboolean moEffectParticlesFractal::Finish()
{
    return PreFinish();
}

//====================
//
//		CUSTOM
//
//===================


void moEffectParticlesFractal::Interaction( moIODeviceManager *IODeviceManager ) {
  /*
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
*/

}

void moEffectParticlesFractal::Update( moEventList *p_eventlist ) {

    moMoldeoObject::Update( p_eventlist );
/*
    if (moScript::IsInitialized()) {
        if (ScriptHasFunction("Update")) {
            SelectScriptFunction("Update");
            //passing number of particles
            AddFunctionParam( (int) ( m_rows*m_cols ) );
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
                MODebug2->Error( GetLabelName() + moText(" moEffectParticlesFractal::  script > ") + moText("Update function not executed") );
            }
        }
    }
*/

}

void moEffectParticlesFractal::RegisterFunctions()
{

}

int moEffectParticlesFractal::ScriptCalling(moLuaVirtualMachine& vm, int iFunctionNumber)
{

}



int moEffectParticlesFractal::luaDrawPoint(moLuaVirtualMachine& vm)
{
    return 0;
}

int moEffectParticlesFractal::luaGetParticle(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesFractal* Par;

    Par = m_ParticlesFractalArray[i];

    if (Par) {

        lua_pushnumber(state, (lua_Number) Par->Age.Duration() );
        lua_pushnumber(state, (lua_Number) Par->Visible );
        lua_pushnumber(state, (lua_Number) Par->Mass );

    }

    return 3;
}


int moEffectParticlesFractal::luaGetParticleRotation(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesFractal* Par;

    Par = m_ParticlesFractalArray[i];

    if (Par) {

        lua_pushnumber(state, (lua_Number) Par->Age.Duration() );
        lua_pushnumber(state, (lua_Number) Par->Visible );
        lua_pushnumber(state, (lua_Number) Par->Mass );

    }

    return 3;
}

int moEffectParticlesFractal::luaGetParticlePosition(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesFractal* Par;

    moVector3f Position;

    Par = m_ParticlesFractalArray[i];

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

int moEffectParticlesFractal::luaGetParticleSize(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesFractal* Par;

    moVector2f Size;

    Par = m_ParticlesFractalArray[i];

    if (Par) {
        Size = Par->Size;
        lua_pushnumber(state, (lua_Number) Size.X() );
        lua_pushnumber(state, (lua_Number) Size.Y() );

    } else {
        lua_pushnumber(state, (lua_Number) 0 );
        lua_pushnumber(state, (lua_Number) 0 );
    }

    return 2;
}

int moEffectParticlesFractal::luaGetParticleScale(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesFractal* Par;

    double Scale;

    Par = m_ParticlesFractalArray[i];

    if (Par) {
        Scale = Par->Scale;
        lua_pushnumber(state, (lua_Number) Scale );
    } else {
        lua_pushnumber(state, (lua_Number) 0 );
    }

    return 1;
}

int moEffectParticlesFractal::luaGetParticleVelocity(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    moParticlesFractal* Par = m_ParticlesFractalArray[i];

    moVector3f Velocity = Par->Velocity;

    if (Par) {

        lua_pushnumber(state, (lua_Number) Velocity.X() );
        lua_pushnumber(state, (lua_Number) Velocity.Y() );
        lua_pushnumber(state, (lua_Number) Velocity.Z() );

    }

    return 3;
}


int moEffectParticlesFractal::luaGetParticleIntersection(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;

    MOfloat x = (MOfloat) lua_tonumber (state, 1);
    MOfloat y = (MOfloat) lua_tonumber (state, 2);

    moParticlesFractal* Par;

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

    for( int i=0; i<m_ParticlesFractalArray.Count(); i++ ) {

        Par = m_ParticlesFractalArray[i];

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


int moEffectParticlesFractal::luaUpdateParticle( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOint age = (MOint) lua_tonumber (state, 2);
    MOint visible = (MOint) lua_tonumber (state, 3);
    MOint mass = (MOint) lua_tonumber (state, 4);

    moParticlesFractal* Par = m_ParticlesFractalArray[i];

    if (Par) {
        Par->Age.SetDuration( age);
        Par->Visible = visible;
        Par->Mass = mass;
    }

    return 0;

}

int moEffectParticlesFractal::luaUpdateParticlePosition( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOfloat x = (MOfloat) lua_tonumber (state, 2);
    MOfloat y = (MOfloat) lua_tonumber (state, 3);
    MOfloat z = (MOfloat) lua_tonumber (state, 4);

    moParticlesFractal* Par = m_ParticlesFractalArray[i];

    if (Par) {
        Par->Pos3d = moVector3f( x, y, z );
    }

    return 0;

}

int moEffectParticlesFractal::luaUpdateParticleSize( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOfloat x = (MOfloat) lua_tonumber (state, 2);
    MOfloat y = (MOfloat) lua_tonumber (state, 3);


    moParticlesFractal* Par = m_ParticlesFractalArray[i];

    if (Par) {
        Par->Size = moVector2f( x, y );
    }

    return 0;

}

int moEffectParticlesFractal::luaUpdateParticleScale( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOfloat scale = (MOfloat) lua_tonumber (state, 2);

    moParticlesFractal* Par = m_ParticlesFractalArray[i];

    if (Par) {
        Par->Scale = scale;
    }

    return 0;

}

int moEffectParticlesFractal::luaUpdateParticleVelocity( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOfloat x = (MOfloat) lua_tonumber (state, 2);
    MOfloat y = (MOfloat) lua_tonumber (state, 3);
    MOfloat z = (MOfloat) lua_tonumber (state, 4);

    moParticlesFractal* Par = m_ParticlesFractalArray[i];

    if (Par) {
        Par->Velocity = moVector3f( x, y, z );
    }

    return 0;

}

int moEffectParticlesFractal::luaUpdateParticleRotation( moLuaVirtualMachine& vm ) {
    lua_State *state = (lua_State *) vm;

    MOint i = (MOint) lua_tonumber (state, 1);

    MOfloat rx = (MOfloat) lua_tonumber (state, 2);
    MOfloat ry = (MOfloat) lua_tonumber (state, 3);
    MOfloat rz = (MOfloat) lua_tonumber (state, 4);

    moParticlesFractal* Par = m_ParticlesFractalArray[i];

    if (Par) {
        Par->Rotation = moVector3f( rx, ry, rz );
    }

    return 0;

}


int moEffectParticlesFractal::luaUpdateForce( moLuaVirtualMachine& vm ) {


    return 0;

}



int moEffectParticlesFractal::luaShot( moLuaVirtualMachine& vm) {

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

int moEffectParticlesFractal::luaReInit(moLuaVirtualMachine& vm ) {

    lua_State *state = (lua_State *) vm;

    this->ReInit();

    return 0;
}
