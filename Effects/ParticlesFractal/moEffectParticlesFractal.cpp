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

  m_pTFilter_ColorTexture = NULL;
	m_pColorTexture = NULL;
  m_pTFilter_ColorTextureSwap = NULL;
  m_pColorTextureSwap = NULL;
  m_pColorTextureFinal = NULL;
  m_bColorTextureSwapOn = false;

  m_pTFilter_OrientationTexture = NULL;
	m_pOrientationTexture = NULL;
  m_pTFilter_OrientationTextureSwap = NULL;
  m_pOrientationTextureSwap = NULL;
  m_pOrientationTextureFinal = NULL;
  m_bOrientationTextureSwapOn = false;

  m_pTFilter_CellCodeTexture = NULL;
	m_pCellCodeTexture = NULL;
  m_pTFilter_CellCodeTextureSwap = NULL;
  m_pCellCodeTextureSwap = NULL;
  m_pCellCodeTextureFinal = NULL;
  m_bCellCodeTextureSwapOn = false;

  m_pTFilter_CellMemoryTexture = NULL;
	m_pCellMemoryTexture = NULL;
  m_pTFilter_CellMemoryTextureSwap = NULL;
  m_pCellMemoryTextureSwap = NULL;
  m_pCellMemoryTextureFinal = NULL;
  m_bCellMemoryTextureSwapOn = false;

  posArray = NULL;
  scaleArray = NULL;
  orientationArray = NULL;
  stateArray = NULL;
  colorArray = NULL;

  cellcodeArray = NULL;
  cellmemoryArray = NULL;

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

	p_configdefinition->Add( moText("texture_code"), MO_PARAM_TEXTURE, PARTICLES_TEXTURE_CODE, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("texture_memory"), MO_PARAM_TEXTURE, PARTICLES_TEXTURE_MEMORY, moValue( "default", "TXT") );

	p_configdefinition->Add( moText("texture_medium"), MO_PARAM_TEXTURE, PARTICLES_TEXTURE_MEDIUM, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("texture_altitude"), MO_PARAM_TEXTURE, PARTICLES_TEXTURE_ALTITUDE, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("texture_variability"), MO_PARAM_TEXTURE, PARTICLES_TEXTURE_VARIABILITY, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("texture_confidence"), MO_PARAM_TEXTURE, PARTICLES_TEXTURE_CONFIDENCE, moValue( "default", "TXT") );

	p_configdefinition->Add( moText("texture_mode"), MO_PARAM_NUMERIC, PARTICLES_TEXTUREMODE, moValue( "0", "NUM"), moText("UNIT,PATCH,MANY,MANY2PATCH,MANYBYORDER"));

	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, PARTICLES_BLENDING, moValue( "0", "NUM") );
	p_configdefinition->Add( moText("width"), MO_PARAM_NUMERIC, PARTICLES_WIDTH, moValue( "16", "NUM") );
	p_configdefinition->Add( moText("height"), MO_PARAM_NUMERIC, PARTICLES_HEIGHT, moValue( "16", "NUM") );
	p_configdefinition->Add( moText("memory"), MO_PARAM_NUMERIC, PARTICLES_MEMORY, moValue( "4", "NUM") );
	p_configdefinition->Add( moText("code"), MO_PARAM_NUMERIC, PARTICLES_CODE, moValue( "4", "NUM") );

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

		p_configdefinition->Add( moText("random_color_function"), MO_PARAM_NUMERIC, PARTICLES_RANDOMCOLORFUNCTION, moValue( "0", "NUM").Ref(), moText("NONE,DEGRADE,OSCILLATE,RED,GREEN,BLUE,YELLOW") );
		p_configdefinition->Add( moText("particles_separation"), MO_PARAM_FUNCTION, PARTICLES_SEPARATION, moValue( "0.0", "FUNCTION").Ref() );

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
    moDefineParamIndex( PARTICLES_TEXTURE_ALTITUDE, moText("texture_altitude") );
    moDefineParamIndex( PARTICLES_TEXTURE_VARIABILITY, moText("texture_variability") );
    moDefineParamIndex( PARTICLES_TEXTURE_CONFIDENCE, moText("texture_confidence") );
    moDefineParamIndex( PARTICLES_TEXTUREMODE, moText("texture_mode") );
    moDefineParamIndex( PARTICLES_BLENDING, moText("blending") );

    moDefineParamIndex( PARTICLES_WIDTH, moText("width") );
    moDefineParamIndex( PARTICLES_HEIGHT, moText("height") );
    moDefineParamIndex( PARTICLES_MEMORY, moText("memory") );
    moDefineParamIndex( PARTICLES_CODE, moText("code") );
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

    moDefineParamIndex( PARTICLES_RANDOMCOLORFUNCTION, moText("random_color_function") );
    moDefineParamIndex( PARTICLES_SEPARATION, moText("particles_separation") );

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
    m_cellmem = m_Config.Int( moR(PARTICLES_MEMORY)  );
    m_cellcode = m_Config.Int( moR(PARTICLES_CODE)  );

    m_MediumTextureLoadedName = m_Config.Texture( moR(PARTICLES_TEXTURE_MEDIUM)).GetName();
    m_AltitudeTextureLoadedName = m_Config.Texture( moR(PARTICLES_TEXTURE_ALTITUDE)).GetName();
    m_VariabilityTextureLoadedName = m_Config.Texture( moR(PARTICLES_TEXTURE_VARIABILITY)).GetName();
    m_ConfidenceTextureLoadedName = m_Config.Texture( moR(PARTICLES_TEXTURE_CONFIDENCE)).GetName();
    //MODebug2->Message("m_MediumTextureLoadedName:"+m_MediumTextureLoadedName);

    ///create shader and destination texture
    if ( !m_pStateTexture ) {

      InitParticlesFractal(   m_rows,
                              m_cols );

      if (cellcodeArray==NULL) {
        long numCellCodeBox = m_rows * m_cols * m_cellcode * m_cellcode;
        cellcodeArray = new GLfloat[4 * numCellCodeBox]();
        MODebug2->Push("Reading pixels: " +  IntToStr(numParticles) );

        if (cellcodeArray && m_pCellCodeTextureFinal) {
          //MODebug2->Push("Reading pixels: " +  IntToStr(numParticles) );
          //m_pPositionTextureFinal->GetFBO()->Bind();

          //m_pPositionTextureFinal->GetFBO()->SetReadTexture( m_pPositionTextureFinal->GetGLId() );
          //glReadPixels(0, 0, m_rows, m_cols, GL_RGBA, GL_FLOAT, posArray);
          //m_pPositionTextureFinal->GetFBO()->Unbind();

          glBindTexture( GL_TEXTURE_2D, m_pCellCodeTextureFinal->GetGLId() );
          glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, cellcodeArray );
          for(long c=0; c<numCellCodeBox; c++) {
            //cellcodeArray[c*4+3] = 1.0;
            cellcodeArray[c*4] = 0.0;
            cellcodeArray[c*4+1] = 0.0;
            cellcodeArray[c*4+2] = 0.0;
            cellcodeArray[c*4+3] = 0.0;
          }
        }
      }


    }


    if (cellmemoryArray==NULL) {
        long numCellMemBox = m_rows * m_cols * m_cellmem * m_cellmem;
        cellmemoryArray = new GLfloat[4 * numCellMemBox]();
          for(long c=0; c<numCellMemBox; c++) {
            //cellcodeArray[c*4+3] = 1.0;
            cellmemoryArray[c*4] = 0.0;
            cellmemoryArray[c*4+1] = 0.0;
            cellmemoryArray[c*4+2] = 0.0;
            cellmemoryArray[c*4+3] = 0.0;
          }
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

    if ( m_bCellCodeTextureSwapOn && m_pTFilter_CellCodeTextureSwap ) {
        m_bCellCodeTextureSwapOn = false;
        //m_pTFilter_StateTextureSwap->Apply( &m_EffectState.tempo, 1.0f, m_pStateFilterParams );
        m_pTFilter_CellCodeTextureSwap->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        m_pCellCodeTextureFinal = m_pCellCodeTextureSwap;

    } else if ( m_pTFilter_CellCodeTexture ) {
        m_bCellCodeTextureSwapOn = true;
        //m_pTFilter_StateTexture->Apply( &m_EffectState.tempo );
        m_pTFilter_CellCodeTexture->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        m_pCellCodeTextureFinal = m_pCellCodeTexture;
    }

    if ( m_bCellMemoryTextureSwapOn && m_pTFilter_CellMemoryTextureSwap ) {
        m_bCellMemoryTextureSwapOn = false;
        //m_pTFilter_StateTextureSwap->Apply( &m_EffectState.tempo, 1.0f, m_pStateFilterParams );
        m_pTFilter_CellMemoryTextureSwap->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        m_pCellMemoryTextureFinal = m_pCellMemoryTextureSwap;

    } else if ( m_pTFilter_CellMemoryTexture ) {
        m_bCellMemoryTextureSwapOn = true;
        //m_pTFilter_StateTexture->Apply( &m_EffectState.tempo );
        m_pTFilter_CellMemoryTexture->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        m_pCellMemoryTextureFinal = m_pCellMemoryTexture;
    }


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


     if ( m_bColorTextureSwapOn && m_pTFilter_ColorTextureSwap ) {
        m_bColorTextureSwapOn = false;
        //m_pTFilter_PositionTextureSwap->Apply( &m_EffectState.tempo );
        m_pTFilter_ColorTextureSwap->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        m_pColorTextureFinal = m_pColorTextureSwap;
    } else if ( m_pTFilter_ColorTexture ) {
        m_bColorTextureSwapOn = true;
        //m_pTFilter_PositionTexture->Apply( &m_EffectState.tempo );
        m_pTFilter_ColorTexture->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        m_pColorTextureFinal = m_pColorTexture;
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
        m_pTFilter_ScaleTextureSwap->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        m_pScaleTextureFinal = m_pScaleTextureSwap;
    } else if ( 1==1 && m_pTFilter_ScaleTexture ) {
        m_bScaleTextureSwapOn = true;
        m_pTFilter_ScaleTexture->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        m_pScaleTextureFinal = m_pScaleTexture;
    }


    if ( m_bOrientationTextureSwapOn && m_pTFilter_OrientationTextureSwap ) {
        m_bOrientationTextureSwapOn = false;
        //m_pTFilter_PositionTextureSwap->Apply( &m_EffectState.tempo );
        m_pTFilter_OrientationTextureSwap->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        m_pOrientationTextureFinal = m_pOrientationTextureSwap;
    } else if ( m_pTFilter_OrientationTexture ) {
        m_bOrientationTextureSwapOn = true;
        //m_pTFilter_PositionTexture->Apply( &m_EffectState.tempo );
        m_pTFilter_OrientationTexture->Apply( (moMoldeoObject*)(this), &m_EffectState.tempo );
        m_pOrientationTextureFinal = m_pOrientationTexture;
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

    if (colorArray==NULL) {
      numParticles = m_rows * m_cols;
      colorArray = new GLfloat[4 * numParticles]();
      MODebug2->Push("Reading pixels: " +  IntToStr(numParticles) );
    }

    if (colorArray && m_pColorTextureFinal) {
      //MODebug2->Push("Reading pixels: " +  IntToStr(numParticles) );
      //m_pPositionTextureFinal->GetFBO()->Bind();

      //m_pPositionTextureFinal->GetFBO()->SetReadTexture( m_pPositionTextureFinal->GetGLId() );
      //glReadPixels(0, 0, m_rows, m_cols, GL_RGBA, GL_FLOAT, posArray);
      //m_pPositionTextureFinal->GetFBO()->Unbind();

      glBindTexture( GL_TEXTURE_2D, m_pColorTextureFinal->GetGLId() );
      glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, colorArray );
    }

    if (orientationArray==NULL) {
      numParticles = m_rows * m_cols;
      orientationArray = new GLfloat[4 * numParticles]();
      MODebug2->Push("Reading pixels: " +  IntToStr(numParticles) );
    }

     if (orientationArray && m_pOrientationTextureFinal) {
      //MODebug2->Push("Reading pixels: " +  IntToStr(numParticles) );
      //m_pPositionTextureFinal->GetFBO()->Bind();

      //m_pPositionTextureFinal->GetFBO()->SetReadTexture( m_pPositionTextureFinal->GetGLId() );
      //glReadPixels(0, 0, m_rows, m_cols, GL_RGBA, GL_FLOAT, posArray);
      //m_pPositionTextureFinal->GetFBO()->Unbind();

      glBindTexture( GL_TEXTURE_2D, m_pOrientationTextureFinal->GetGLId() );
      glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, orientationArray );
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

    random_color_function = m_Config.Eval(moR(PARTICLES_RANDOMCOLORFUNCTION));
    particles_separation = m_Config.Eval(moR(PARTICLES_SEPARATION));

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
  moDataManager *pDM = m_pResourceManager->GetDataMan();

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

  CELL CODE AND MEMORY

*/

  tName = "pf_cellcode_fx#"+this->GetLabelName()+"_";

  Mid = TextureMan()->AddTexture( tName, p_cols*m_cellcode, p_rows*m_cellcode, tparam );
  if (Mid>0) {
      m_pCellCodeTexture = TextureMan()->GetTexture(Mid);
      m_pCellCodeTexture->BuildEmpty( p_cols*m_cellcode, p_rows*m_cellcode );
      //m_pCellCodeTexture->CreateThumbnail( "PNGA", p_cols*m_cellcode, p_rows*m_cellcode );
      m_pCellCodeTextureFinal = m_pCellCodeTexture;
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }

  tName = "pf_cellcode_swap_fx#"+this->GetLabelName()+"_";

  Mid = TextureMan()->AddTexture( tName, p_cols*m_cellcode, p_rows*m_cellcode, tparam );
  if (Mid>0) {
      m_pCellCodeTextureSwap = TextureMan()->GetTexture(Mid);
      m_pCellCodeTextureSwap->BuildEmpty( p_cols*m_cellcode, p_rows*m_cellcode );
      m_pCellCodeTextureFinal = m_pCellCodeTextureSwap;
      //m_pCellCodeTexture->CreateThumbnail( "PNGA", p_cols*m_cellcode, p_rows*m_cellcode );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }


  tName = "pf_cellmem_fx#"+this->GetLabelName()+"_";

  Mid = TextureMan()->AddTexture( tName, p_cols*m_cellmem, p_rows*m_cellmem, tparam );
  if (Mid>0) {
      m_pCellMemoryTexture = TextureMan()->GetTexture(Mid);
      m_pCellMemoryTexture->BuildEmpty( p_cols*m_cellmem, p_rows*m_cellmem );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }

  tName = "pf_cellmem_swap_fx#"+this->GetLabelName()+"_";

  Mid = TextureMan()->AddTexture( tName, p_cols*m_cellmem, p_rows*m_cellmem, tparam );
  if (Mid>0) {
      m_pCellMemoryTextureSwap = TextureMan()->GetTexture(Mid);
      m_pCellMemoryTextureSwap->BuildEmpty( p_cols*m_cellmem, p_rows*m_cellmem );
      //TextureMan()->GetTexture(Mid)->BuildEmpty( p_cols, p_rows );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }

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

/// CREATE FILTER FOR CELL CODE CHANGES
  if ( !m_pTFilter_CellCodeTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add( m_pCellCodeTexture->GetName()
                      + moText(" ")+m_pCellMemoryTexture->GetName()
                      + " " + m_pStateTexture->GetName()
                      + " " + m_MediumTextureLoadedName/*Afecta el codigo ejecutado*/
                      + " " + m_AltitudeTextureLoadedName/*Afecta el codigo ejecutado*/
                      + " " + m_VariabilityTextureLoadedName/*Afecta el codigo ejecutado*/
                      + " " + m_ConfidenceTextureLoadedName/*Afecta el codigo ejecutado*/
                      + moText(" ")+this->GetLabelName()+moText("/CellCode.cfg " )
                      + m_pCellCodeTextureSwap->GetName() );
    MODebug2->Message(     moText("SHADER: CELL CODE SWAP   ======================"));
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_CellCodeTextureSwap = pTextureFilterIndex->Get(idx-1);
        m_pCellCodeFilterParams = new moStateFilterParams();
        MODebug2->Message( moText("filter loaded m_pTFilter_CellCodeTextureSwap: ") + m_pTFilter_CellCodeTextureSwap->GetTextureFilterLabelName() );
        MODebug2->Message( moText("-------------------------------------------"));
    }
  }

 /// CREATE FILTER FOR CELL CODE CHANGES (BIRTH AND DEATH) !!!
  if ( !m_pTFilter_CellCodeTexture ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add( m_pCellCodeTextureSwap->GetName()
                      + moText(" ")+m_pCellMemoryTextureSwap->GetName()
                      + " " + m_pStateTextureSwap->GetName()
                      + " " + m_MediumTextureLoadedName/*Afecta el codigo ejecutado*/
                      + " " + m_AltitudeTextureLoadedName/*Afecta el codigo ejecutado*/
                      + " " + m_VariabilityTextureLoadedName/*Afecta el codigo ejecutado*/
                      + " " + m_ConfidenceTextureLoadedName/*Afecta el codigo ejecutado*/
                      + moText(" ")+this->GetLabelName()+moText("/CellCode.cfg " )
                      + m_pCellCodeTexture->GetName() );
    MODebug2->Message(     moText("SHADER: CELL CODE        ======================"));
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_CellCodeTexture = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_CellCodeTexture: ") + m_pTFilter_CellCodeTexture->GetTextureFilterLabelName() );
        MODebug2->Message( moText("-------------------------------------------"));
    }
  }


/// CREATE FILTER FOR CELL MEMORY CHANGES
  if ( !m_pTFilter_CellMemoryTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add( m_pCellCodeTexture->GetName()
                      + moText(" ")+m_pCellMemoryTexture->GetName()
                      + " " + m_pStateTexture->GetName()
                      + " " + m_MediumTextureLoadedName/*Afecta la memoria*/
                      + " " + m_AltitudeTextureLoadedName/*Afecta la memoria*/
                      + " " + m_VariabilityTextureLoadedName/*Afecta la memoria*/
                      + " " + m_ConfidenceTextureLoadedName/*Afecta la memoria*/
                      + moText(" ")+this->GetLabelName()+moText("/CellMemory.cfg " )
                      + m_pCellMemoryTextureSwap->GetName() );
    MODebug2->Message(     moText("SHADER: CELL MEMORY SWAP   ======================"));
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_CellMemoryTextureSwap = pTextureFilterIndex->Get(idx-1);
        m_pCellMemoryFilterParams = new moStateFilterParams();
        MODebug2->Message( moText("filter loaded m_pTFilter_CellMemoryTextureSwap: ") + m_pTFilter_CellMemoryTextureSwap->GetTextureFilterLabelName() );
        MODebug2->Message( moText("-------------------------------------------"));
    }
  }

 /// CREATE FILTER FOR CELL MEMORY CHANGES (BIRTH AND DEATH) !!!
  if ( !m_pTFilter_CellMemoryTexture ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add( m_pCellCodeTextureSwap->GetName()
                      + moText(" ")+m_pCellMemoryTextureSwap->GetName()
                      + " " + m_pStateTextureSwap->GetName()
                      + " " + m_MediumTextureLoadedName/*Afecta la memoria*/
                      + " " + m_AltitudeTextureLoadedName/*Afecta la memoria */
                      + " " + m_VariabilityTextureLoadedName/*Afecta la memoria*/
                      + " " + m_ConfidenceTextureLoadedName/*Afecta la memoria*/
                      + moText(" ")+this->GetLabelName()+moText("/CellMemory.cfg " )
                      + m_pCellMemoryTexture->GetName() );
    MODebug2->Message(     moText("SHADER: CELL MEMORY        ======================"));
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_CellMemoryTexture = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_CellMemoryTexture: ") + m_pTFilter_CellMemoryTexture->GetTextureFilterLabelName() );
        MODebug2->Message( moText("-------------------------------------------"));
    }
  }

/**

  STATE
  x: generation or iteration ( number of reproduction cycles )
  y: maturity reproduction
  z: maturity cycle
  w: origin generation (when this particle is born, its assigned a generation ) or 1.0f if not used

*/


  /// CREATE FILTER FOR STATE CHANGES (BIRTH AND DEATH) !!!
  if ( !m_pTFilter_StateTextureSwap && m_pStateTexture && m_pStateTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add( m_pStateTexture->GetName()
                      + moText(" ")+m_pCellCodeTexture->GetName()
                      + moText(" ")+m_pCellMemoryTexture->GetName()
                      + moText(" ")+this->GetLabelName()+moText("/Birth.cfg " )
                      + m_pStateTextureSwap->GetName() );
    MODebug2->Message(     moText("SHADER: BIRTH SWAP   ======================"));
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_StateTextureSwap = pTextureFilterIndex->Get(idx-1);
        m_pStateFilterParams = new moStateFilterParams();
        MODebug2->Message( moText("filter loaded m_pTFilter_StateTextureSwap: ") + m_pTFilter_StateTextureSwap->GetTextureFilterLabelName() );
        MODebug2->Message( moText("-------------------------------------------"));
    }
  }

 /// CREATE FILTER FOR STATE CHANGES (BIRTH AND DEATH) !!!
  if ( !m_pTFilter_StateTexture && m_pStateTexture && m_pStateTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add( m_pStateTextureSwap->GetName()
                      + moText(" ")+m_pCellCodeTextureSwap->GetName()
                      + moText(" ")+m_pCellMemoryTextureSwap->GetName()
                      + moText(" ")+this->GetLabelName()+moText("/Birth.cfg " )
                      + m_pStateTexture->GetName() );
    MODebug2->Message(     moText("SHADER: BIRTH        ======================"));
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_StateTexture = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_StateTexture: ") + m_pTFilter_StateTexture->GetTextureFilterLabelName() );
        MODebug2->Message( moText("-------------------------------------------"));
    }
  }



/**

  COLOR

*/

  tName = "pf_color_fx#"+this->GetLabelName()+"_";
  Mid = TextureMan()->AddTexture( tName, p_cols, p_rows, tparam );
  if (Mid>0) {
      m_pColorTexture = TextureMan()->GetTexture(Mid);
      m_pColorTexture->BuildEmpty( p_cols, p_rows );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }

  tName = "pf_color_swap_fx#"+this->GetLabelName()+"_";
  Mid = TextureMan()->AddTexture( tName, p_cols, p_rows, tparam );
  if (Mid>0) {
      m_pColorTextureSwap = TextureMan()->GetTexture(Mid);
      m_pColorTextureSwap->BuildEmpty( p_cols, p_rows );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }

  /// CREATE FILTER FOR COLOR CHANGES (MUTATIONS!) !!!
  if ( !m_pTFilter_ColorTexture
      && m_pColorTextureSwap && m_pColorTexture && m_pStateTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add( m_pStateTextureSwap->GetName()
                      + " " + m_pColorTexture->GetName()
                      + " " + m_MediumTextureLoadedName/*Afecta el color de la particula: (r,g,b) || (h,s,v)*/
                      + " " + m_AltitudeTextureLoadedName/*Afecta el brillo de la particula: (r*0.15+g*0.7+b*0.2) */
                      + " " + m_VariabilityTextureLoadedName/*Afecta la variación del color: entre que colores oscila*/
                      + " " + m_ConfidenceTextureLoadedName/*Afecta la confianza del dato: su opacidad: alpha channel (rgb,A)*/
                      + moText(" ")+m_pCellCodeTexture->GetName()
                      + moText(" ")+m_pCellMemoryTexture->GetName()
                      + moText(" ")+this->GetLabelName()+moText("/Color.cfg " )
                      + m_pColorTextureSwap->GetName() );
    MODebug2->Message(     moText("SHADER: COLOR SWAP ======================"));
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_ColorTexture = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_ColorTexture: ") + m_pTFilter_ColorTexture->GetTextureFilterLabelName() );
        MODebug2->Message( moText("-------------------------------------------"));
    }
  }

  /// CREATE FILTER FOR COLOR CHANGES (MUTATIONS!) !!!
  if ( !m_pTFilter_ColorTextureSwap
      && m_pColorTextureSwap && m_pColorTexture && m_pStateTexture ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add( m_pStateTexture->GetName()
                      + " " + m_pColorTextureSwap->GetName()
                       + " " + m_MediumTextureLoadedName/**/
                      + " " + m_AltitudeTextureLoadedName/**/
                      + " " + m_VariabilityTextureLoadedName/**/
                      + " " + m_ConfidenceTextureLoadedName/**/
                      + moText(" ")+m_pCellCodeTexture->GetName()
                      + moText(" ")+m_pCellMemoryTexture->GetName()
                      + moText(" ")+this->GetLabelName()+moText("/Color.cfg " )
                      + m_pColorTexture->GetName() );
    MODebug2->Message(     moText("SHADER: COLOR ==========================="));
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_ColorTextureSwap = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_ColorTextureSwap: ") + m_pTFilter_ColorTextureSwap->GetTextureFilterLabelName() );
        MODebug2->Message( moText("-------------------------------------------"));

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
      && m_pColorTextureSwap && m_pPositionTextureSwap) {
    moTextArray copy_filter_0;
    copy_filter_0.Add( m_pStateTextureSwap->GetName()
                      + " "
                      + m_pColorTextureSwap->GetName()
                      + " "
                      + m_pVelocityTexture->GetName()
                      + " " + m_pPositionTextureSwap->GetName()
                      + " " + m_MediumTextureLoadedName/*el color determina el tipo de movimiento: 0 fijo, 1 lineal, [0<>0.5] fractal dimension N [N es 2 si el espacio es 2d. N es 3 si este es 3d, etc...], [0.5<>1.0] fractal hacia dimension 1 (dimension, hacia atras o hacia adelante)  */
                      + " " + m_AltitudeTextureLoadedName/*la altitud se puede interpretar como gravedad o como altura: mayor gravedad=>menor velocidad, mayor altitud=>mayor velocidad*/
                      + " " + m_VariabilityTextureLoadedName/*la variabilidad: orienta la tendencia hacia donde se mueve este valor, el movimiento estará orientado hacia el centro de gravedad de la tendencia*/
                      + " " + m_ConfidenceTextureLoadedName/*la confianza reduce o aumenta el nivel de ruido dentro del movimiento*/
                      + moText(" ")+m_pCellCodeTexture->GetName()
                      + moText(" ")+this->GetLabelName()+moText("/Velocity.cfg " )
                      + m_pVelocityTextureSwap->GetName() );

    MODebug2->Message(     moText("SHADER: VELOCITY SWAP ===================="));
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_VelocityTexture = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_VelocityTexture: ") + m_pTFilter_VelocityTexture->GetTextureFilterLabelName() );
        MODebug2->Message( moText("-------------------------------------------"));
    }
  }

  /// CREATE FILTER FOR VELOCITY CHANGES (BIRTH AND DEATH) !!!
  if ( !m_pTFilter_VelocityTextureSwap && m_pStateTexture
      && m_pVelocityTexture && m_pVelocityTextureSwap && m_pPositionTexture ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add( m_pStateTexture->GetName()
                      + " "
                      + m_pColorTexture->GetName()
                      + " "
                      +  m_pVelocityTextureSwap->GetName()
                      + " " + m_pPositionTexture->GetName()
                      + " " + m_MediumTextureLoadedName
                      + " " + m_AltitudeTextureLoadedName
                      + " " + m_VariabilityTextureLoadedName
                      + " " + m_ConfidenceTextureLoadedName
                      + moText(" ")+m_pCellCodeTexture->GetName()
                      + moText(" ")+this->GetLabelName()+moText("/Velocity.cfg " )
                      + m_pVelocityTexture->GetName() );
    MODebug2->Message(     moText("SHADER: VELOCITY =========================="));
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_VelocityTextureSwap = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_VelocityTextureSwap: ") + m_pTFilter_VelocityTextureSwap->GetTextureFilterLabelName() );
        MODebug2->Message( moText("-------------------------------------------"));
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
                      + " " + m_pColorTexture->GetName()
                      + " " + m_MediumTextureLoadedName/*el color se puede interpretar para zonificar las particulas: rojo a la izquierda de la pantalla, azul a la derecha*/
                      + " " + m_AltitudeTextureLoadedName/*la altidud a su vez puede servir para dar una profundidad al valor...*/
                      + " " + m_VariabilityTextureLoadedName/*la variabilidad del dato hace que permita nivelar la posicion con mayor precisión: hacia donde se dirige*/
                      + " " + m_ConfidenceTextureLoadedName/*la confianza del dato hace que tenga una posición menos precisa*/
                      + moText(" ")+m_pCellCodeTexture->GetName()
                      + moText(" ")+this->GetLabelName()+moText("/Position.cfg" )
                      + " " + m_pPositionTextureSwap->GetName() );
    MODebug2->Message(     moText("SHADER: POSITION SWAP ====================="));
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_PositionTextureSwap = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_PositionTextureSwap: ") + m_pTFilter_PositionTextureSwap->GetTextureFilterLabelName() );
        MODebug2->Message( moText("-------------------------------------------"));
    }
  }

  /// CREATE FILTER FOR POSITION CHANGES (BIRTH AND DEATH) !!!
  if ( !m_pTFilter_PositionTexture && m_pPositionTexture && m_pPositionTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add(//+  moText(" shaders/Birth.cfg res:64x64 " )
                      m_pStateTexture->GetName()
                      + " " + m_pVelocityTexture->GetName()
                      + " " + m_pPositionTextureSwap->GetName()
                      + " " + m_pColorTexture->GetName()
                      + " " + m_MediumTextureLoadedName
                      + " " + m_AltitudeTextureLoadedName
                      + " " + m_VariabilityTextureLoadedName
                      + " " + m_ConfidenceTextureLoadedName
                      + moText(" ")+m_pCellCodeTexture->GetName()
                      + moText(" ")+this->GetLabelName()+moText("/Position.cfg" )
                      + " " + m_pPositionTexture->GetName() );
    MODebug2->Message(     moText("SHADER: POSITION ========================="));
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_PositionTexture = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_PositionTexture: ") + m_pTFilter_PositionTexture->GetTextureFilterLabelName() );
        MODebug2->Message( moText("-------------------------------------------"));
    }
  }


/**

  SCALE

*/

  //sx,sy,sz
  if (1==1) {

      tName = "pf_scale_fx#"+this->GetLabelName()+"_";
      Mid = TextureMan()->AddTexture( tName, p_cols, p_rows, tparam );
      if (Mid>0) {
          m_pScaleTexture = TextureMan()->GetTexture(Mid);
          m_pScaleTexture->BuildEmpty( p_cols, p_rows );
          MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
      } else {
          MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
      }

      tName = "pf_scale_swap_fx#"+this->GetLabelName()+"_";
      Mid = TextureMan()->AddTexture( tName, p_cols, p_rows, tparam );
      if (Mid>0) {
          m_pScaleTextureSwap = TextureMan()->GetTexture(Mid);
          m_pScaleTextureSwap->BuildEmpty( p_cols, p_rows );
          MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
      } else {
          MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
      }
  }
  //GL_ARB_vertex_buffer_object
  /// CREATE FILTER FOR SCALE !! (SWAP)
  if (1==1 && !m_pTFilter_ScaleTextureSwap && m_pScaleTexture && m_pScaleTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add(
                      //+  moText(" shaders/Birth.cfg res:64x64 " )
                      m_pStateTextureSwap->GetName()
                      + " " + m_pPositionTextureSwap->GetName()
                      + " " + m_pScaleTexture->GetName()
                      + " " + m_MediumTextureLoadedName/**/
                      + " " + m_AltitudeTextureLoadedName/**/
                      + " " + m_VariabilityTextureLoadedName/**/
                      + " " + m_ConfidenceTextureLoadedName/**/
                      + moText(" ")+m_pCellCodeTexture->GetName()
                      + moText(" ")+this->GetLabelName()+moText("/Scale.cfg" )
                      + " " + m_pScaleTextureSwap->GetName() );
    MODebug2->Message(     moText("SHADER: SCALE SWAP ========================"));
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_ScaleTextureSwap = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_ScaleTextureSwap: ") + m_pTFilter_ScaleTextureSwap->GetTextureFilterLabelName() );
        MODebug2->Message( moText("-------------------------------------------"));
    }
  }

  /// CREATE FILTER FOR SCALE !! ()
  if (1==1 && !m_pTFilter_ScaleTexture && m_pScaleTexture && m_pScaleTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add(//+  moText(" shaders/Birth.cfg res:64x64 " )
                      m_pStateTexture->GetName()
                      + " " + m_pPositionTexture->GetName()
                      + " " + m_pScaleTextureSwap->GetName()
                      + " " + m_MediumTextureLoadedName/**/
                      + " " + m_AltitudeTextureLoadedName/**/
                      + " " + m_VariabilityTextureLoadedName/**/
                      + " " + m_ConfidenceTextureLoadedName/**/
                      + moText(" ")+m_pCellCodeTexture->GetName()
                      + moText(" ")+this->GetLabelName()+moText("/Scale.cfg" )
                      + " " + m_pScaleTexture->GetName() );
    MODebug2->Message(     moText("SHADER: SCALE ============================="));
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_ScaleTexture = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_ScaleTexture: ") + m_pTFilter_ScaleTexture->GetTextureFilterLabelName() );
        MODebug2->Message( moText("-------------------------------------------"));
    }
  }

/**

  ORIENTATION

*/

//sx,sy,sz
  tName = "pf_orien_fx#"+this->GetLabelName()+"_";
  Mid = TextureMan()->AddTexture( tName, p_cols, p_rows, tparam );
  if (Mid>0) {
      m_pOrientationTexture = TextureMan()->GetTexture(Mid);
      m_pOrientationTexture->BuildEmpty( p_cols, p_rows );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }

  tName = "pf_orien_swap_fx#"+this->GetLabelName()+"_";
  Mid = TextureMan()->AddTexture( tName, p_cols, p_rows, tparam );
  if (Mid>0) {
      m_pOrientationTextureSwap = TextureMan()->GetTexture(Mid);
      m_pOrientationTextureSwap->BuildEmpty( p_cols, p_rows );
      MODebug2->Message("moEffectParticlesFractal::InitParticlesFractal > " + tName + " texture created!! " + strResolution);
  } else {
      MODebug2->Error("moEffectParticlesFractal::InitParticlesFractal > Couldn't create texture: " + tName + " " + strResolution);
  }

  /// CREATE FILTER FOR ORIENTATION CHANGES !!!
  if ( !m_pTFilter_OrientationTextureSwap && m_pOrientationTexture && m_pOrientationTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add(
                      //+  moText(" shaders/Birth.cfg res:64x64 " )
                      m_pStateTextureSwap->GetName()
                      + " " + m_pVelocityTextureSwap->GetName()
                      + " " + m_pPositionTextureSwap->GetName()
                      + " " + m_pOrientationTexture->GetName()
                      + " " + m_MediumTextureLoadedName/**/
                      + " " + m_AltitudeTextureLoadedName/**/
                      + " " + m_VariabilityTextureLoadedName/**/
                      + " " + m_ConfidenceTextureLoadedName/**/
                      + moText(" ")+m_pCellCodeTexture->GetName()
                      + moText(" ")+this->GetLabelName()+moText("/Orientation.cfg" )
                      + " " + m_pOrientationTextureSwap->GetName() );
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_OrientationTextureSwap = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_OrientationTextureSwap: ") + m_pTFilter_OrientationTextureSwap->GetTextureFilterLabelName() );
    }
  }

  /// CREATE FILTER FOR ORIENTATION CHANGES !!!
  if ( !m_pTFilter_OrientationTexture && m_pOrientationTexture && m_pOrientationTextureSwap ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add(//+  moText(" shaders/Birth.cfg res:64x64 " )
                      m_pStateTexture->GetName()
                      + " " + m_pVelocityTexture->GetName()
                      + " " + m_pPositionTexture->GetName()
                      + " " + m_pOrientationTextureSwap->GetName()
                      + " " + m_MediumTextureLoadedName/**/
                      + " " + m_AltitudeTextureLoadedName/**/
                      + " " + m_VariabilityTextureLoadedName/**/
                      + " " + m_ConfidenceTextureLoadedName/**/
                      + moText(" ")+m_pCellCodeTexture->GetName()
                      + moText(" ")+this->GetLabelName()+moText("/Orientation.cfg" )
                      + " " + m_pOrientationTexture->GetName() );
    int idx = pTextureFilterIndex->LoadFilters( &copy_filter_0 );
    if (idx>0) {
        m_pTFilter_OrientationTexture = pTextureFilterIndex->Get(idx-1);
        MODebug2->Message( moText("filter loaded m_pTFilter_OrientationTexture: ") + m_pTFilter_OrientationTexture->GetTextureFilterLabelName() );
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

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();


  if (texture_mode==PARTICLES_TEXTUREMODE_MANY2PATCH) {
    m_pTexBuf = m_Config[moR(PARTICLES_FOLDERS)][MO_SELECTED][0].TextureBuffer();
    if (m_pTexBuf) {
        m_nImages = m_pTexBuf->GetImagesProcessed();
    }
  } else {
    m_pTexBuf = NULL;
  }



  if (posArray && stateArray && colorArray && orientationArray)
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

          float sx = scaleArray[ijoff];
          float sy = scaleArray[ijoff + 1];
          float sz = scaleArray[ijoff + 2];

          float rx = orientationArray[ijoff];
          float ry = orientationArray[ijoff + 1];
          float rz = orientationArray[ijoff + 2];
            //float rz = 0.0;

          //z = z + ijoff*zoff*0.0f;

          float generation = stateArray[ijoff];
          float maturity = stateArray[ijoff+1];
          float age = stateArray[ijoff+2];
          float origin = stateArray[ijoff+3];

          float r = colorArray[ijoff]*m_Color.X();
          float g = colorArray[ijoff+1]*m_Color.Y();
          float b = colorArray[ijoff+2]*m_Color.Z();
          float al = colorArray[ijoff+3]*m_Color.W();

          long memcell_ix = 0;
          float fmaterial = 0;
          int imaterial = -1;
          if (cellmemoryArray) {
            memcell_ix = i * 4 * m_cellmem + j * 4 * m_cellmem * m_cellmem * m_cols;
            fmaterial = cellmemoryArray[ memcell_ix + 0  ];
            if (fmaterial>0) {
                //MODebug2->Message("fmaterial:"+FloatToStr(fmaterial));
            }
            if (m_pTexBuf && (fmaterial < m_nImages) ) {
                //fmaterial = ( fmaterial * m_nImages );
                imaterial = int(fmaterial);
                //MODebug2->Message("imaterial:"+IntToStr(imaterial));
            }
          }

          glColor4f( r,g,b,m_EffectState.alpha*al*m_Alpha );

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
  /*
            if ( m_Physics.m_EmitterType == PARTICLES_EMITTERTYPE_SPIRAL ) {
              float ag = float(ijoff) / float(m_cols*m_rows);
              float alx = float(i) / float(m_cols);
              float aly = float(j) / float(m_rows);
              rz = alx*(1.0+particles_separation)*360+90;

              glRotatef(  rz, U.X(), U.Y(), U.Z() );
            } else {


            }*/
/*            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
*/
            glPushMatrix();
            //glLoadIdentity();
            //glRotatef(  ry, W.X(), W.Y(), W.Z() );
            //glRotatef(  rx, V.X(), V.Y(), V.Z() );

            glTranslatef( x, y, z );

            glRotatef(  rz, U.X(), U.Y(), U.Z() );
            glRotatef(  rx, V.X(), V.Y(), V.Z() );
            glRotatef(  ry, W.X(), W.Y(), W.Z() );

            sx = sx*scalex;
            sy = sy*scaley;
            sz = sz*scalez;
            if (m_pTexBuf) {
                int irandom = int( float(m_nImages-1) * g );
                if (imaterial>=0) irandom = imaterial;
                //moTextureFrames& pTextFrames(m_pTexBuf->GetBufferLevels( 255*(r*0.2+g*0.7+b*0.1), 0 ) );
                //int iTex = pTextFrames.GetRef( 0 );

                int iTex = m_pTexBuf->GetFrame( irandom );
                glBindTexture( GL_TEXTURE_2D, iTex );
            }

            glBegin(GL_QUADS);
              glTexCoord2f( tcoordx, tcoordy );
              glVertex3f( 0-sizex*sx, 0-sizey*sy, z);

              glTexCoord2f( tcoordx+tsizex, tcoordy );
              glVertex3f( 0+sizex*sx, 0-sizey*sy, z);

              glTexCoord2f( tcoordx+tsizex, tcoordy+tsizey );
              glVertex3f( 0+sizex*sx, 0+sizey*sy, z);

              glTexCoord2f( tcoordx, tcoordy+tsizey );
              glVertex3f( 0-sizex*sx, 0+sizey*sy, z);
            glEnd();
            glPopMatrix();
        }

      }
  }

  //MODebug2->Push( "positions:" + Tpositions );
  glPopMatrix();
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

    PreDraw( tempogral, parentstate);

    UpdateParameters();

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
                    midi_frandomvelocity = (float)VAL / (float) 127.0;
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

    ////Estas funciones deben llamarse en la función Init

    // 00: white
    RegisterFunction("CellBeginProgram");//25 params: (id_cell)

    // 01: red 01
    RegisterFunction("CellAge");//26 change age parameter => params: (self, delta maturity time)
    // 02: red 02
    RegisterFunction("CellDuplicate");//27 duplicate cell program when maturity is reached, params: ( maturity_age  )
    // 03: red 03
    RegisterFunction("CellMutate");//28 mutate program: params: ( id_father, id_mother, [id_others] )
    // 04: red 04
    RegisterFunction("CellCrossover");//29 mutate program: params: ( id_father, id_mother, [id_others] )
    // 05: red 05
    RegisterFunction("CellDie");//30 change age parameter => params: (self, delta maturity time)

    // 06: black
    RegisterFunction("CellEndProgram");//31
    RegisterFunction("CellDumpProgram");//32

    RegisterFunction("WriteMemory");//33
    RegisterFunction("ReadMemory");//34
    RegisterFunction("DumpMemory");//35

/*
    // 05: yellow 01
    RegisterFunction("cellGrow");//30 scale
    // 06: yellow 02
    RegisterFunction("cellRotate");//31 orientation
    // 07: yellow 03
    RegisterFunction("cellAcc");//32 force -> direction

    // 08: blue 01
    RegisterFunction("cellLink");//33 associate to others params, parent, none or other cell: (self, id_cell)
    // 09: blue 02
    RegisterFunction("cellEmit");//34 emite elementos dentro del medio (pinta por ejemplo): params: ( self, emit_code, emit_amount )
    // 10: blue 03
    RegisterFunction("cellSens");//35 sensa los elementos dentro del medio (lee): params ( self,  sens_code (una de las 4 texturas, posicion) )

    // 11:
    RegisterFunction("readCellAge");//36
    // 12:
    RegisterFunction("readCellScale");//37
    // 13:
    RegisterFunction("readCellOrientation");//38
    // 14:
    RegisterFunction("readCellSpeed");//39

    // 15: green 01
    RegisterFunction("readCellMemory");//40 read value from memory ( self, id_mem )
    // 16: green 02
    RegisterFunction("writeCellMemory");//41 write value from memory ( self, id_mem, values ) , can write sub-program
    // 17: green 03
    RegisterFunction("cmpCellMemory");//42 compare value between memory... (self, id_mem1, id_mem2, size )


    // 18 FUNCTION (DERIVATION/INTEGRATION)
    RegisterFunction("callCellMemory");//43 apply function: new code for operations depends on each implementation versions (this ensure fractality)
    // 19
    RegisterFunction("returnCellMemory");//44 return value -> end sub-program
*/


    ResetScriptCalling();

}

int moEffectParticlesFractal::ScriptCalling(moLuaVirtualMachine& vm, int iFunctionNumber)
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



        case 25:
            ResetScriptCalling();
            return luaCellBeginProgram(vm);
        case 26:
            ResetScriptCalling();
            return luaCellAge(vm);
        case 27:
            ResetScriptCalling();
            return luaCellDuplicate(vm);
        case 28:
            ResetScriptCalling();
            return luaCellMutate(vm);
        case 29:
            ResetScriptCalling();
            return luaCellCrossover(vm);
        case 30:
            ResetScriptCalling();
            return luaCellDie(vm);
        case 31:
            ResetScriptCalling();
            return luaCellEndProgram(vm);
        case 32:
            ResetScriptCalling();
            return luaCellDumpProgram(vm);

        case 33:
            ResetScriptCalling();
            return luaWriteMemory(vm);
        case 34:
            ResetScriptCalling();
            return luaReadMemory(vm);
        case 35:
            ResetScriptCalling();
            return luaDumpMemory(vm);
        /*case 29:
            ResetScriptCalling();
            return luaCellRotate(vm);
        case 30:
            ResetScriptCalling();
            return luaCellGrow(vm);
        case 31:
            ResetScriptCalling();
            return luaCellAcc(vm);
*/
        default:
            NextScriptCalling();
            return moMoldeoObject::ScriptCalling( vm, iFunctionNumber );
    }
}

int moEffectParticlesFractal::luaCellBeginProgram(moLuaVirtualMachine& vm) {

    lua_State *state = (lua_State *) vm;

    lua_id_cell = (MOint) lua_tonumber (state, 1);
    MODebug2->Message( moText("CellBeginProgram: ") + IntToStr(lua_id_cell) );

    ///put one 0 pixel at id_cell position?

    if (cellcodeArray) {
        cell_position_j = lua_id_cell / m_cols;
        cell_position_i = lua_id_cell - cell_position_j*m_cols;
        cell_position = cell_position_i*4*m_cellcode + cell_position_j*4*m_cellcode*m_cellcode*m_cols;
        cellcodeArray[cell_position] = 1.0;
        m_pCellCodeTexture->BuildFromBuffer(m_pCellCodeTexture->GetWidth(),m_pCellCodeTexture->GetHeight(), cellcodeArray, GL_RGBA, GL_FLOAT);
        m_pCellCodeTextureSwap->BuildFromBuffer(m_pCellCodeTexture->GetWidth(),m_pCellCodeTexture->GetHeight(), cellcodeArray, GL_RGBA, GL_FLOAT);
    }

    lua_pushnumber(state, (lua_Number) 1 );

    return 1;
}

int moEffectParticlesFractal::luaCellAge(moLuaVirtualMachine& vm) {
    lua_State *state = (lua_State *) vm;

    float delta_age = (float) lua_tonumber (state, 1);
    MODebug2->Message( moText("CellAge: ") + IntToStr(lua_id_cell)+moText(" delta: ")+FloatToStr(delta_age) );

    if (cellcodeArray) {
        cellcodeArray[cell_position+1] = 0.11;
        cellcodeArray[cell_position+2] = delta_age;
        m_pCellCodeTexture->BuildFromBuffer(m_pCellCodeTexture->GetWidth(),m_pCellCodeTexture->GetHeight(), cellcodeArray, GL_RGBA, GL_FLOAT);
        m_pCellCodeTextureSwap->BuildFromBuffer(m_pCellCodeTexture->GetWidth(),m_pCellCodeTexture->GetHeight(), cellcodeArray, GL_RGBA, GL_FLOAT);
    }


    return 0;
}

int moEffectParticlesFractal::luaCellDuplicate(moLuaVirtualMachine& vm) {
    lua_State *state = (lua_State *) vm;

    float maturity = (float) lua_tonumber (state, 1);
    MODebug2->Message( moText("CellDuplicate: ") + IntToStr(lua_id_cell)
                       +moText(" maturity: ")+FloatToStr(maturity) );

    if (cellcodeArray) {
        cellcodeArray[cell_position+3] = 0.12;
        cellcodeArray[cell_position+4] = maturity;
        m_pCellCodeTexture->BuildFromBuffer(m_pCellCodeTexture->GetWidth(),m_pCellCodeTexture->GetHeight(), cellcodeArray, GL_RGBA, GL_FLOAT);
        m_pCellCodeTextureSwap->BuildFromBuffer(m_pCellCodeTexture->GetWidth(),m_pCellCodeTexture->GetHeight(), cellcodeArray, GL_RGBA, GL_FLOAT);
    }

    return 0;
}

int moEffectParticlesFractal::luaCellMutate(moLuaVirtualMachine& vm) {
    lua_State *state = (lua_State *) vm;

    float mutation_cell = (float) lua_tonumber (state, 1);
    float mutation_randomness = (float) lua_tonumber (state, 1);
    MODebug2->Message(  moText("CellMutate: ") + IntToStr(lua_id_cell)
                        +moText(" mutation id cell: ")+FloatToStr(mutation_cell)
                        +moText(" mutation randomness: ")+FloatToStr(mutation_randomness) );

    if (cellcodeArray) {
        cellcodeArray[cell_position+5] = 0.13;
        cellcodeArray[cell_position+6] = mutation_cell;
        cellcodeArray[cell_position+7] = mutation_randomness;
        m_pCellCodeTexture->BuildFromBuffer(m_pCellCodeTexture->GetWidth(),m_pCellCodeTexture->GetHeight(), cellcodeArray, GL_RGBA, GL_FLOAT);
        m_pCellCodeTextureSwap->BuildFromBuffer(m_pCellCodeTexture->GetWidth(),m_pCellCodeTexture->GetHeight(), cellcodeArray, GL_RGBA, GL_FLOAT);
    }

    return 0;
}

int moEffectParticlesFractal::luaCellCrossover(moLuaVirtualMachine& vm) {
    lua_State *state = (lua_State *) vm;

    float mutation_cell_one = (float) lua_tonumber (state, 1);
    float mutation_cell_two = (float) lua_tonumber (state, 1);
    MODebug2->Message(  moText("CellCrossover: ") + IntToStr(lua_id_cell)
                        +moText(" crossover id cells: ")+FloatToStr(mutation_cell_one)+moText("x")+FloatToStr(mutation_cell_two) );

    if (cellcodeArray) {
        cellcodeArray[cell_position+8] = 0.14;
        cellcodeArray[cell_position+9] = mutation_cell_one;
        cellcodeArray[cell_position+10] = mutation_cell_two;
        m_pCellCodeTexture->BuildFromBuffer(m_pCellCodeTexture->GetWidth(),m_pCellCodeTexture->GetHeight(), cellcodeArray, GL_RGBA, GL_FLOAT);
        m_pCellCodeTextureSwap->BuildFromBuffer(m_pCellCodeTexture->GetWidth(),m_pCellCodeTexture->GetHeight(), cellcodeArray, GL_RGBA, GL_FLOAT);
    }

    return 0;
}


int moEffectParticlesFractal::luaCellDie(moLuaVirtualMachine& vm) {
    lua_State *state = (lua_State *) vm;

    float die_age =  (float) lua_tonumber (state, 1);
    MODebug2->Message( moText("CellDie: ") + IntToStr(lua_id_cell)+moText(" die age: ")+FloatToStr(die_age) );
    if (cellcodeArray) {
        cellcodeArray[cell_position+11] = 0.15;
        cellcodeArray[cell_position+12] = die_age;
        m_pCellCodeTexture->BuildFromBuffer(m_pCellCodeTexture->GetWidth(),m_pCellCodeTexture->GetHeight(), cellcodeArray, GL_RGBA, GL_FLOAT);
        m_pCellCodeTextureSwap->BuildFromBuffer(m_pCellCodeTexture->GetWidth(),m_pCellCodeTexture->GetHeight(), cellcodeArray, GL_RGBA, GL_FLOAT);
    }

    return 0;
}

int moEffectParticlesFractal::luaCellEndProgram(moLuaVirtualMachine& vm) {
    lua_State *state = (lua_State *) vm;

    //lua_id_cell = (MOint) lua_tonumber (state, 1);
    MODebug2->Message( moText("CellEndProgram: ") + IntToStr(lua_id_cell) );

    if (cellcodeArray) {
        long cell_position_end = cell_position + (4*m_cellcode-1) + 4*(m_cellcode-1)*m_cellcode*m_cols ;
        cellcodeArray[cell_position_end] = -1.0;
        m_pCellCodeTexture->BuildFromBuffer(m_pCellCodeTexture->GetWidth(),m_pCellCodeTexture->GetHeight(), cellcodeArray, GL_RGBA, GL_FLOAT);
        m_pCellCodeTextureSwap->BuildFromBuffer(m_pCellCodeTexture->GetWidth(),m_pCellCodeTexture->GetHeight(), cellcodeArray, GL_RGBA, GL_FLOAT);
    }
    return 0;
}


int moEffectParticlesFractal::luaCellDumpProgram(moLuaVirtualMachine& vm) {
    lua_State *state = (lua_State *) vm;

    lua_id_cell = (MOint) lua_tonumber (state, 1);
    MODebug2->Message( moText("CellDumpProgram: ") + IntToStr(lua_id_cell) );

    if (cellcodeArray && m_pCellCodeTextureFinal) {

        cell_position_j = lua_id_cell / m_cols;
        cell_position_i = lua_id_cell - cell_position_j*m_cols;
        cell_position = cell_position_i*4*m_cellcode + cell_position_j*4*m_cellcode*m_cellcode*m_cols;

        moText fname = m_pResourceManager->GetDataMan()->GetDataPath()+moSlash+"cellcode";
        m_pCellCodeTextureFinal->CreateThumbnail("PNGA", m_pCellCodeTexture->GetWidth(), m_pCellCodeTexture->GetHeight(), fname);

        glBindTexture( GL_TEXTURE_2D, m_pCellCodeTextureFinal->GetGLId() );
        glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, cellcodeArray );

        moText fullcode = "";
        int cchange = 0;
        moText linechange = "";

        for(int linec = 0; linec<m_cellcode; linec++) {

            fullcode += linechange;
            long cpos = cell_position+linec*4*m_cellcode*m_cols;
            moText celdatab = "";

            for(int cc = cpos; cc<(cpos+4*m_cellcode); cc++) {
                //if ( cchange == m_cellcode*m_cellcode*4 ) { cellchange = "\n\n"; cchange=0; }
                fullcode += celdatab + FloatToStr( cellcodeArray[cc] );
                celdatab = "  ";
                //cchange+= 1;
            }
            linechange = "\n";
        }
        MODebug2->Message(fullcode);
    }

    return 0;
}


int moEffectParticlesFractal::luaWriteMemory(moLuaVirtualMachine& vm) {

    lua_State *state = (lua_State *) vm;

    lua_id_cell = (MOint) lua_tonumber (state, 1);
    long lua_id_cell_mem_pos = (MOint) lua_tonumber (state, 2);
    float lua_id_cell_mem_val = (MOfloat) lua_tonumber (state, 3);
    MODebug2->Message( moText("WriteMemory: ") + IntToStr(lua_id_cell)
                        + moText(" Mem Pos:") + IntToStr(lua_id_cell_mem_pos)
                        + moText(" Mem Val:") + FloatToStr(lua_id_cell_mem_val) );

    ///put one 0 pixel at id_cell position?
    if (cellmemoryArray && m_pCellMemoryTexture && m_pCellMemoryTextureSwap) {

        cell_position_j = lua_id_cell / m_cols;
        cell_position_i = lua_id_cell - cell_position_j*m_cols;
        //long memj = lua_id_cell_mem_pos / (4*m_cellmem);
        //long memi = lua_id_cell_mem_pos - memj* (4*m_cellmem);
        cell_position = cell_position_i*4*m_cellmem + cell_position_j*4*m_cellmem*m_cellmem*m_cols;

        cellmemoryArray[cell_position+lua_id_cell_mem_pos] = lua_id_cell_mem_val;

        m_pCellMemoryTexture->BuildFromBuffer(m_pCellMemoryTexture->GetWidth(),m_pCellMemoryTexture->GetHeight(), cellmemoryArray, GL_RGBA, GL_FLOAT);
        m_pCellMemoryTextureSwap->BuildFromBuffer(m_pCellMemoryTextureSwap->GetWidth(),m_pCellMemoryTextureSwap->GetHeight(), cellmemoryArray, GL_RGBA, GL_FLOAT);
    }

    lua_pushnumber(state, (lua_Number) 1 );

    return 1;
}

int moEffectParticlesFractal::luaDumpMemory(moLuaVirtualMachine& vm) {
    lua_State *state = (lua_State *) vm;

    lua_id_cell = (MOint) lua_tonumber (state, 1);
    MODebug2->Message( moText("DumpMemory: ") + IntToStr(lua_id_cell) );

    if (cellmemoryArray && m_pCellMemoryTextureFinal) {

        cell_position_j = lua_id_cell / m_cols;
        cell_position_i = lua_id_cell - cell_position_j*m_cols;
        cell_position = cell_position_i*4*m_cellmem + cell_position_j*4*m_cellmem*m_cellmem*m_cols;

        moText fname = m_pResourceManager->GetDataMan()->GetDataPath()+moSlash+"cellmem";
        m_pCellMemoryTextureFinal->CreateThumbnail("PNGA", m_pCellMemoryTexture->GetWidth(), m_pCellMemoryTexture->GetHeight(), fname);

        glBindTexture( GL_TEXTURE_2D, m_pCellMemoryTextureFinal->GetGLId() );
        glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, cellmemoryArray );

        moText fullcode = "";
        int cchange = 0;
        moText linechange = "";

        for(int linec = 0; linec<m_cellmem; linec++) {

            fullcode += linechange;
            long cpos = cell_position+linec*4*m_cellmem*m_cols;
            moText celdatab = "";

            for(int cc = cpos; cc<(cpos+4*m_cellmem); cc++) {
                //if ( cchange == m_cellcode*m_cellmem*4 ) { cellchange = "\n\n"; cchange=0; }
                fullcode += celdatab + FloatToStr( cellmemoryArray[cc] );
                celdatab = "  ";
                //cchange+= 1;
            }
            linechange = "\n";
        }
        MODebug2->Message(fullcode);
    } else {
        MODebug2->Warning("CellMemory array not ready!");
    }

    return 0;
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






