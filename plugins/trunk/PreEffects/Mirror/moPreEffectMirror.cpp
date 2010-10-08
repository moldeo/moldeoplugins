/*******************************************************************************

                            moPreEffectMirror.cpp

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

#include "moPreEffectMirror.h"

//========================
//  Factory
//========================

moPreEffectMirrorFactory *m_PreEffectMirrorFactory = NULL;

MO_PLG_API moPreEffectFactory* CreatePreEffectFactory(){
	if(m_PreEffectMirrorFactory==NULL)
		m_PreEffectMirrorFactory = new moPreEffectMirrorFactory();
	return(moPreEffectFactory*) m_PreEffectMirrorFactory;
}

MO_PLG_API void DestroyPreEffectFactory(){
	delete m_PreEffectMirrorFactory;
	m_PreEffectMirrorFactory = NULL;
}

moPreEffect*  moPreEffectMirrorFactory::Create() {
	return new moPreEffectMirror();
}

void moPreEffectMirrorFactory::Destroy(moPreEffect* fx) {
	delete fx;
}

//========================
//  PreEfecto
//========================
moPreEffectMirror::moPreEffectMirror() {
	SetName("mirror");
}

moPreEffectMirror::~moPreEffectMirror() {
	Finish();
}

MOboolean moPreEffectMirror::Init()
{
    if (!PreInit()) return false;

    moDefineParamIndex( MIRROR_ALPHA, moText("alpha") );
	moDefineParamIndex( MIRROR_SYNCRO, moText("syncro") );
	moDefineParamIndex( MIRROR_PHASE, moText("phase") );
	moDefineParamIndex( MIRROR_COLOR, moText("color") );

	moDefineParamIndex( MIRROR_BLENDING, moText("blending") );
	moDefineParamIndex( MIRROR_RADIUS, moText("radius") );
	moDefineParamIndex( MIRROR_POLYGONMODE, moText("polygonmode") );
	moDefineParamIndex( MIRROR_ABERRATION, moText("aberration") );
	moDefineParamIndex( MIRROR_CONFIGURATION, moText("configuration") );


    MESHAG = NULL;
	MO_MESHA_TEX = 16;
	MIRRORRENDERHEIGHT = 0;
	MIRRORRENDERWIDTH = 0;

    ReInit();

	return true;
}

void moPreEffectMirror::ReInit() {

	int i, j, it, jt;

	MOint w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    MOint h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    if ( w == MIRRORRENDERWIDTH && h == MIRRORRENDERHEIGHT )
        return;

    if(MESHAG!=NULL) {
		for(int i=0;i<(MO_MESHA_WIDTH+1);i++) {
			if(MESHAG[i]!=NULL) {
				delete [] MESHAG[i];
				MESHAG[i] = NULL;
			}
		}
		delete [] MESHAG;
		MESHAG = NULL;
	}

    MIRRORRENDERWIDTH = (int)w;
	MIRRORRENDERHEIGHT = (int)h;
	MO_MESHA_WIDTH = MIRRORRENDERWIDTH / MO_MESHA_TEX;
	MO_MESHA_HEIGHT = MIRRORRENDERHEIGHT / MO_MESHA_TEX;


    MESHAG = new ajCoord* [(MO_MESHA_WIDTH+1)];
	for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
		MESHAG[i] = new ajCoord [(MO_MESHA_HEIGHT+1)];
	}

	GLfloat deltai,deltaj,tdeltai,tdeltaj;
	GLfloat minora,minorb,majora,majorb;
	GLfloat wpoint2 = MO_MESHA_WIDTH+1;
	GLfloat hpoint2 = MO_MESHA_HEIGHT+1;


	minora = -0.4*1.0;//(w/1024.0f);//0.8 800 pix
	minorb = 0.4*1.0;//*(w/1024.0f);
	majora = -0.3;//0.6 600pix
	majorb = 0.3;
	deltai =(minorb-minora) /(float)(wpoint2-1);
	deltaj =(majorb-majora) /(float)(hpoint2-1);
	tdeltai = (w/1024.0f)/(float)(wpoint2-1);
	tdeltaj = (h/1024.0f)/(float)(hpoint2-1);


	for(i=0,it=((MIRRORRENDERWIDTH/2)-(MO_MESHA_WIDTH/2)*MO_MESHA_TEX);i<(MO_MESHA_WIDTH+1);i++,it+=MO_MESHA_TEX) {
		for(j=0,jt=((MIRRORRENDERHEIGHT/2)-(MO_MESHA_HEIGHT/2)*MO_MESHA_TEX);j<(MO_MESHA_HEIGHT+1);j++,jt+=MO_MESHA_TEX) {
			MESHAG[i][j].I = it;
			MESHAG[i][j].J = jt;
			MESHAG[i][j].U = 0.0f+((float)i)*(tdeltai);
			MESHAG[i][j].V = 0.0f+((float)j)*(tdeltaj);
			MESHAG[i][j].X =  minora+((float)i)*(deltai); //(float)(MESHAG[i][j].I-(MIRRORRENDERWIDTH/2));
			MESHAG[i][j].Y =  majora+((float)j)*(deltaj); //(float) -((MIRRORRENDERHEIGHT/2)-MESHAG[i][j].J);
			MESHAG[i][j].Z = -0.5185f;
			MESHAG[i][j].NX = 0.0;
			MESHAG[i][j].NY = 0.0;
			MESHAG[i][j].NZ = 0.1;
		}
	}

}

void moPreEffectMirror::Draw( moTempo* tempogral,moEffectState* parentstate)
{
    PreDraw( tempogral, parentstate);

    int it,jt;
	int i,j,bl,cf;
	float dist;
	float ra,ab;
	float s0, s1, t0, t1;
	MOint pmode;

    ReInit();

	MOint w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    MOint h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

	ra = m_Config[moR(MIRROR_RADIUS)][MO_SELECTED][0].Fun()->Eval(state.tempo.ang);
	ab = m_Config[moR(MIRROR_ABERRATION)][MO_SELECTED][0].Fun()->Eval(state.tempo.ang);

	bl = m_Config.GetParam(moR(MIRROR_BLENDING)).GetValue().GetSubValue(0).Int();
	cf = m_Config[moR(MIRROR_CONFIGURATION)][MO_SELECTED][0].Int();
	pmode = m_Config[moR(MIRROR_POLYGONMODE)].GetValue().GetSubValue(0).Int();

    // Aca van los comandos OpenGL del efecto.
	glDisable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	SetBlending( (moBlendingModes) bl );

	//ACA APLICO UN DESPLAZAMIENTO
	for(i=0;i<(MO_MESHA_WIDTH+1);i++) {
		for(j=0;j<(MO_MESHA_HEIGHT+1);j++) {

			//aqui tomo la distancia del centro al punto
			dist = MESHAG[i][j].X*MESHAG[i][j].X + MESHAG[i][j].Y*MESHAG[i][j].Y;
			dist = sqrt(dist);

			if(i>1 && i<(MO_MESHA_WIDTH-1)) {
				switch(cf) {
					case 0:	MESHAG[i][j].XN = MESHAG[i][j].X*ab;//*(ab+0.3*(dist/ra)*cos(state.tempo.ang*0.5));
						break;
					case 1: MESHAG[i][j].XN = MESHAG[i][j].X*(ab+0.3*(dist/ra)*sin(state.tempo.ang*0.5));
						break;
					case 2: MESHAG[i][j].XN = MESHAG[i][j].X*(ab+0.3*(dist/ra)*sin(state.tempo.ang*0.5));
						break;
					case 3: MESHAG[i][j].XN = MESHAG[i][j].X*(ab+0.3*(dist/ra)*cos(state.tempo.ang*0.5));
						break;
					case 4: MESHAG[i][j].XN = MESHAG[i][j].X*(ab+0.9*(dist/ra)*cos(state.tempo.ang*0.5));
						break;
					case 5: MESHAG[i][j].XN = MESHAG[i][j].X*(ab+0.4*(dist/ra)*cos(state.tempo.ang*0.5));
						break;
					case 6: MESHAG[i][j].XN = MESHAG[i][j].X*(ab+0.1*(dist/ra)*cos(state.tempo.ang*0.5));
						break;
					case 7: MESHAG[i][j].XN = MESHAG[i][j].X*(ab+0.2*(dist/ra)*cos(state.tempo.ang*0.5));
						break;
					case 8: MESHAG[i][j].XN = MESHAG[i][j].X*(ab+0.7*(dist/ra)*cos(state.tempo.ang*0.5));
						break;
				}
			}
			else {
				MESHAG[i][j].XN = MESHAG[i][j].X*ab;
			}


			if(j>1 && j<(MO_MESHA_HEIGHT-1)) {
				switch(cf) {
					case 0:	MESHAG[i][j].YN = MESHAG[i][j].Y*ab;//*(ab+0.3*(dist/ra)*cos(state.tempo.ang*0.5));
						break;
					case 1: MESHAG[i][j].YN = MESHAG[i][j].Y*(ab+0.3*(dist/ra)*cos(state.tempo.ang*0.5));
						break;
					case 2: MESHAG[i][j].YN = MESHAG[i][j].Y*(ab+0.3*(dist/ra)*sin(state.tempo.ang*0.5));
						break;
					case 3: MESHAG[i][j].YN = MESHAG[i][j].Y*(ab+0.35*(dist/ra)*sin(state.tempo.ang*0.1));
						break;
					case 4: MESHAG[i][j].YN = MESHAG[i][j].Y*(ab+0.2*(dist/ra)*sin(state.tempo.ang*2.5));
						break;
					case 5: MESHAG[i][j].YN = MESHAG[i][j].Y*(ab+0.12*(dist/ra)*sin(state.tempo.ang*4.5));
						break;
					case 6: MESHAG[i][j].YN = MESHAG[i][j].Y*(ab+0.18*(dist/ra)*sin(state.tempo.ang*10.5));
						break;
					case 7: MESHAG[i][j].YN = MESHAG[i][j].Y*(ab+0.6*(dist/ra)*cos(state.tempo.ang*0.5));
						break;
					case 8: MESHAG[i][j].YN = MESHAG[i][j].Y*(ab+0.1*(dist/ra)*sin(state.tempo.ang*0.1));
						break;
				}
			}
			else {
				MESHAG[i][j].YN = MESHAG[i][j].Y*ab;
			}

			//para los Z no se implemento nada todavia
			MESHAG[i][j].ZN = MESHAG[i][j].Z;

		}
	}

    SetColor( m_Config[moR(MIRROR_COLOR)][MO_SELECTED], m_Config[moR(MIRROR_ALPHA)][MO_SELECTED], state );

    if ( pmode == MO_POLYGONMODE_FILL ) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        glBindTexture(GL_TEXTURE_2D, m_pResourceManager->GetRenderMan()->RenderTexGLId(MO_EFFECTS_TEX));
    } else if ( pmode == MO_POLYGONMODE_LINE ) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        glBindTexture(GL_TEXTURE_2D, 0);
    } else if ( pmode == MO_POLYGONMODE_POINT ) {
        glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
        glBindTexture(GL_TEXTURE_2D, 0);
    }

  float fs = 1024.0f / w;
  float ft = 1024.0f / h;

    for(i=0; i<MO_MESHA_WIDTH; i++) {
	    for(j=0; j<MO_MESHA_HEIGHT; j++) {

            // Calculating texture coordinates.
    	    /*
    	    s0 = (float)MESHAG[i][j].I / (float)MIRRORRENDERWIDTH;
			t0 = (float)MESHAG[i][j].J / (float)MIRRORRENDERHEIGHT;
		 	s1 = (float)(MESHAG[i][j].I + MO_MESHA_TEX) / (float)MIRRORRENDERWIDTH;
			t1 = (float)(MESHAG[i][j].J + MO_MESHA_TEX) / (float)MIRRORRENDERHEIGHT;
			*/
			s0 = MESHAG[i][j].U * fs;
			t0 = MESHAG[i][j].V * ft;
			s1 = MESHAG[i+1][j].U * fs;
			t1 = MESHAG[i][j+1].V * ft;

			glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f( s0,  t0);
				glVertex3f(MESHAG[i][j].XN,MESHAG[i][j].YN,MESHAG[i][j].ZN);
				glTexCoord2f( s0,  t1);
				glVertex3f(MESHAG[i][j+1].XN,MESHAG[i][j+1].YN,MESHAG[i][j+1].ZN);
				glTexCoord2f( s1,  t0);
				glVertex3f(MESHAG[i+1][j].XN,MESHAG[i+1][j].YN,MESHAG[i+1][j].ZN);
				glTexCoord2f( s1,  t1);
				glVertex3f(MESHAG[i+1][j+1].XN,MESHAG[i+1][j+1].YN,MESHAG[i+1][j+1].ZN);
			glEnd();
		}
	}

    //default
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

MOboolean moPreEffectMirror::Finish()
{
    if(MESHAG!=NULL) {
		for(int i=0;i<(MO_MESHA_WIDTH+1);i++) {
			if(MESHAG[i]!=NULL) {
				delete [] MESHAG[i];
				MESHAG[i] = NULL;
			}
		}
		delete [] MESHAG;
		MESHAG = NULL;
	}

	return PreFinish();
}

moConfigDefinition *
moPreEffectMirror::GetDefinition( moConfigDefinition *p_configdefinition ) {
	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, MIRROR_BLENDING );
	/*
	moValue colordefault( "0.91", "FUNCTION" );
	colordefault.AddSubValue( "0.71", "FUNCTION" );
	colordefault.AddSubValue( "0.71", "FUNCTION" );
	colordefault.AddSubValue( "1.0", "FUNCTION" );
	p_configdefinition->Add( moText("color"), MO_PARAM_COLOR, MIRROR_COLOR, colordefault );//setear en 0.91,0.71,0.71,1
	*/
	p_configdefinition->Add( moText("radius"), MO_PARAM_FUNCTION, MIRROR_RADIUS, moValue( "0.5", "FUNCTION" ).Ref() );
	p_configdefinition->Add( moText("polygonmode"), MO_PARAM_POLYGONMODE, MIRROR_POLYGONMODE, moValue( "0", "NUM" ).Ref() );
	p_configdefinition->Add( moText("aberration"), MO_PARAM_FUNCTION, MIRROR_ABERRATION, moValue( "1.0", "FUNCTION" ).Ref() );
	p_configdefinition->Add( moText("configuration"), MO_PARAM_NUMERIC, MIRROR_CONFIGURATION, moValue("1","NUM").Ref() );

	return p_configdefinition;
}
















