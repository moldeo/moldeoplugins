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

	return true;
}

void moPreEffectErase::Draw( moTempo* tempogral, moEffectState* parentstate )
{

	BeginDraw(tempogral, parentstate);
	
#ifdef OPENGLESV2

	moVector4d color4D = m_Config.EvalColor( moR(ERASE_COLOR) );
	glClearColor(  color4D.X() * m_EffectState.tintr,
                color4D.Y() * m_EffectState.tintg,
                color4D.Z() * m_EffectState.tintb,
                color4D.W() *
                m_Config.Eval( moR( ERASE_ALPHA)) * m_EffectState.alpha);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	MODebug2->Message("moEffectImage::Draw(...) > Applying basic shader!");

	if (!m_BasicShader.Initialized()) {
	    MODebug2->Message("moEffectImage::Draw(...) > Creating basic shader!");
	    
	    m_BasicShader.Init();
	    m_BasicShader.CreateShader(                        
	                         moText("attribute vec4 position;")+moText("\n")
	                        +moText("attribute vec3 color;")+moText("\n")
				+moText("uniform mat4 model;")+moText("\n")
	                        +moText("varying lowp vec3 colorVarying;")+moText("\n")
	                        +moText("void main() {")+moText("\n")
	                        +moText("colorVarying = color;")+moText("\n")
	                        +moText("gl_Position = position;")+moText("\n")
	                        +moText("}"),
	
	                         moText("varying lowp vec3 colorVarying;")+moText("\n")
	                        +moText("void main() {")+moText("\n")
	                        +moText("gl_FragColor = vec4(colorVarying, 1.0);")+moText("\n")
	                        +moText("}")
    			);
    
    	   m_BasicShader.PrintVertShaderLog();
    	   m_BasicShader.PrintFragShaderLog();

    	   vertices_index = m_BasicShader.GetAttribID(moText("position"));
    	   color_index = m_BasicShader.GetAttribID(moText("color"));

    	   MODebug2->Message( moText("Shader Attrib IDs, position:")+IntToStr(vertices_index)+moText(" color:")+IntToStr(color_index) );
        }

  	if (m_BasicShader.Initialized())
     		m_BasicShader.StartShader();

  	float coords[6] = { -0.9,-0.9,  0.9,-0.9,  0,0.7 }; // two coords per vertex.
  	float colors[9] = { 1,0,0,  0,1,0,  1,0,0 };  // three RGB values per vertex.
	
  	glEnableVertexAttribArray( vertices_index );
  	glVertexAttribPointer( vertices_index, 2, GL_FLOAT, false, 0, coords );  // Set data type and location.

  	glEnableVertexAttribArray( color_index );
  	glVertexAttribPointer( color_index, 3, GL_FLOAT, false, 0, colors );

  	//glEnableClientState( GL_VERTEX_ARRAY );  // Enable use of arrays.
  	//glEnableClientState( GL_COLOR_ARRAY );

  	glDrawArrays( GL_TRIANGLES, 0, 3 ); // Use 3 vertices, starting with vertex 0.

  	glDisableVertexAttribArray( vertices_index );
  	glDisableVertexAttribArray( color_index );

  	if (m_BasicShader.Initialized())
  	   m_BasicShader.StopShader();
#endif
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
