/*******************************************************************************

                               moEffectLightDrawing.cpp

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *   (at your option) any later version.                                    *
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

  Copyright (C) 2011 Fabricio Costa

  Authors:
  Fabricio Costa


*******************************************************************************/

#include "moEffectLightDrawing.h"
#include "moFilterManager.h"
#include "moDebugManager.h"


//========================
//  Factory
//========================

moEffectLightDrawingFactory *m_EffectLightDrawingoFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if (m_EffectLightDrawingoFactory==NULL)
		m_EffectLightDrawingoFactory = new moEffectLightDrawingFactory();
	return (moEffectFactory*) m_EffectLightDrawingoFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectLightDrawingoFactory;
	m_EffectLightDrawingoFactory = NULL;
}

moEffect*  moEffectLightDrawingFactory::Create() {
	return new moEffectLightDrawing();
}

void moEffectLightDrawingFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================
moEffectLightDrawing::moEffectLightDrawing() {

	SetName("lightdrawing");

}

moEffectLightDrawing::~moEffectLightDrawing() {
	Finish();
}

moConfigDefinition *
moEffectLightDrawing::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );

	p_configdefinition->SetIndex( "inlet", LIGHTDRAWING_INLET );
	p_configdefinition->SetIndex( "outlet", LIGHTDRAWING_OUTLET );
	p_configdefinition->SetIndex( "alpha", LIGHTDRAWING_ALPHA );
	p_configdefinition->SetIndex( "color", LIGHTDRAWING_COLOR );
	p_configdefinition->SetIndex( "syncro", LIGHTDRAWING_SYNC );
	p_configdefinition->SetIndex( "phase", LIGHTDRAWING_PHASE );


/**
    LIGHTDRAWING_CANVAS_WIDTH, /// result canvas width
    LIGHTDRAWING_CANVAS_HEIGHT, /// result canvas height
*/
    p_configdefinition->Add( moText("canvas_width"), MO_PARAM_NUMERIC, LIGHTDRAWING_CANVAS_WIDTH, moValue( "320", "INT" ) );
    p_configdefinition->Add( moText("canvas_height"), MO_PARAM_NUMERIC, LIGHTDRAWING_CANVAS_HEIGHT, moValue( "240", "INT" ) );

/**
    LIGHTDRAWING_INTERVAL, /// time of each drawing, default = 3000 ms
    LIGHTDRAWING_FADE, /// fading of each new frame, default = 0.0
*/
    p_configdefinition->Add( moText("interval"), MO_PARAM_NUMERIC, LIGHTDRAWING_INTERVAL, moValue( "3000", "INT" ) );
    p_configdefinition->Add( moText("fade"), MO_PARAM_FUNCTION, LIGHTDRAWING_FADE, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
/**
    LIGHTDRAWING_CROP_LEFT, /// crop left, default = 0
    LIGHTDRAWING_CROP_RIGHT, /// crop right, default = 0
    LIGHTDRAWING_CROP_TOP, /// crop top, default = 0
    LIGHTDRAWING_CROP_BOTTOM, /// crop bottom, default = 0
*/
    p_configdefinition->Add( moText("crop_left"), MO_PARAM_NUMERIC, LIGHTDRAWING_CROP_LEFT, moValue( "0", "INT" ) );
    p_configdefinition->Add( moText("crop_right"), MO_PARAM_NUMERIC, LIGHTDRAWING_CROP_RIGHT, moValue( "0", "INT" ) );
    p_configdefinition->Add( moText("crop_top"), MO_PARAM_NUMERIC, LIGHTDRAWING_CROP_TOP, moValue( "0", "INT" ) );
    p_configdefinition->Add( moText("crop_bottom"), MO_PARAM_NUMERIC, LIGHTDRAWING_CROP_BOTTOM, moValue( "0", "INT" ) );

/**
	LIGHTDRAWING_INTENSITY_MIN, /// light intensity minimun to draw pixel, default = 0.95
	LIGHTDRAWING_INTENSITY_MAX, /// light intensity maximum to draw pixel, default = 1.0
	LIGHTDRAWING_CROMINANCE_MIN, /// light hue minimum to draw pixel, default = 0.0
	LIGHTDRAWING_CROMINANCE_MAX, /// light hue maximum to draw pixel, default = 360.0
	LIGHTDRAWING_SATURATION_MIN, /// light saturation minimum, default = 0.0
	LIGHTDRAWING_SATURATION_MAX, /// light saturation maximum, default = 1.0
*/
    p_configdefinition->Add( moText("intensity_min"), MO_PARAM_FUNCTION, LIGHTDRAWING_INTENSITY_MIN, moValue( "0.95", MO_VALUE_FUNCTION ).Ref() );
    p_configdefinition->Add( moText("intensity_max"), MO_PARAM_FUNCTION, LIGHTDRAWING_INTENSITY_MAX, moValue( "1.0", MO_VALUE_FUNCTION ).Ref() );

    p_configdefinition->Add( moText("crominance_min"), MO_PARAM_FUNCTION, LIGHTDRAWING_CROMINANCE_MIN, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
    p_configdefinition->Add( moText("crominance_max"), MO_PARAM_FUNCTION, LIGHTDRAWING_CROMINANCE_MAX, moValue( "1.0", MO_VALUE_FUNCTION ).Ref() );

    p_configdefinition->Add( moText("saturation_min"), MO_PARAM_FUNCTION, LIGHTDRAWING_SATURATION_MIN, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
    p_configdefinition->Add( moText("saturation_max"), MO_PARAM_FUNCTION, LIGHTDRAWING_SATURATION_MAX, moValue( "1.0", MO_VALUE_FUNCTION ).Ref() );

/**
    LIGHTDRAWING_SCREENSHOT_MODE, /// default = 0 = no screenshot, 1 = auto screenshot at the end of drawing
    LIGHTDRAWING_SCREENSHOT_INTERVAL, /// in milis. default = 0 = auto after drawing finished, see LIVEDRAWING_INTERVAL
    LIGHTDRAWING_SCREENSHOT_WIDTH, /// 0 = auto
    LIGHTDRAWING_SCREENSHOT_HEIGHT,/// 0 = auto
*/
    p_configdefinition->Add( moText("screenshot_mode"), MO_PARAM_NUMERIC, LIGHTDRAWING_SCREENSHOT_MODE, moValue( "0", "INT") );
    p_configdefinition->Add( moText("screenshot_interval"), MO_PARAM_NUMERIC, LIGHTDRAWING_SCREENSHOT_INTERVAL, moValue( "0","INT") );

    p_configdefinition->Add( moText("screenshot_width"), MO_PARAM_NUMERIC, LIGHTDRAWING_SCREENSHOT_WIDTH, moValue( "0", "INT") );
    p_configdefinition->Add( moText("screenshot_height"), MO_PARAM_NUMERIC, LIGHTDRAWING_SCREENSHOT_HEIGHT, moValue( "0","INT") );

	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, LIGHTDRAWING_TEXTURE, moValue( "default", MO_VALUE_TXT) );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, LIGHTDRAWING_BLENDING, moValue( "0", MO_VALUE_NUM ).Ref() );
	p_configdefinition->Add( moText("width"), MO_PARAM_FUNCTION, LIGHTDRAWING_WIDTH, moValue( "1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("height"), MO_PARAM_FUNCTION, LIGHTDRAWING_HEIGHT, moValue( "1.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, LIGHTDRAWING_TRANSLATEX, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, LIGHTDRAWING_TRANSLATEY, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("rotate"), MO_PARAM_ROTATEZ, LIGHTDRAWING_ROTATE );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, LIGHTDRAWING_SCALEX );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, LIGHTDRAWING_SCALEY );

	return p_configdefinition;
}



void moEffectLightDrawing::UpdateParameters() {

    m_intensity_min = m_Config.Eval( moR(LIGHTDRAWING_INTENSITY_MIN) );
    m_intensity_max = m_Config.Eval( moR(LIGHTDRAWING_INTENSITY_MAX) );

    fade = m_Config.Eval( moR(LIGHTDRAWING_FADE) );
    interval = m_Config.Int( moR(LIGHTDRAWING_INTERVAL) );
    //MODebug2->Push("fade" + FloatToStr(fade));
    //MODebug2->Push("m_TimerInterval.Duration"+IntToStr( m_TimerInterval.Duration() ) );
}

MOboolean moEffectLightDrawing::Init() {

    if (!PreInit()) return false;

    //m_Config.Indexation();

	moDefineParamIndex( LIGHTDRAWING_ALPHA, moText("alpha") );
	moDefineParamIndex( LIGHTDRAWING_COLOR, moText("color") );
	moDefineParamIndex( LIGHTDRAWING_SYNC, moText("syncro") );
	moDefineParamIndex( LIGHTDRAWING_PHASE, moText("phase") );

	moDefineParamIndex( LIGHTDRAWING_INTENSITY_MIN, moText("intensity_min") );
	moDefineParamIndex( LIGHTDRAWING_INTENSITY_MAX, moText("intensity_max") );

	moDefineParamIndex( LIGHTDRAWING_INTERVAL, moText("interval") );
	moDefineParamIndex( LIGHTDRAWING_FADE, moText("fade") );

	moDefineParamIndex( LIGHTDRAWING_TEXTURE, moText("texture") );
	moDefineParamIndex( LIGHTDRAWING_BLENDING, moText("blending") );
	moDefineParamIndex( LIGHTDRAWING_WIDTH, moText("width") );
	moDefineParamIndex( LIGHTDRAWING_HEIGHT, moText("height") );
	moDefineParamIndex( LIGHTDRAWING_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( LIGHTDRAWING_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( LIGHTDRAWING_ROTATE, moText("rotate") );
	moDefineParamIndex( LIGHTDRAWING_SCALEX, moText("scalex") );
	moDefineParamIndex( LIGHTDRAWING_SCALEY, moText("scaley") );
	moDefineParamIndex( LIGHTDRAWING_INLET, moText("inlet") );
	moDefineParamIndex( LIGHTDRAWING_OUTLET, moText("outlet") );

    m_pLightTexture = NULL;
    m_pLiveTexture = NULL;
    m_buffer_data = NULL;
    m_buffer_light_data = NULL;



    if (m_pLiveTexture==NULL) {

        int tid = m_pResourceManager->GetTextureMan()->GetTextureMOId( moText("LIVEIN0"), false );
        if(tid>-1) {
            m_pLiveTexture = m_pResourceManager->GetTextureMan()->GetTexture(tid);
        }
    }

	return true;
}


void moEffectLightDrawing::Draw( moTempo* tempogral, moEffectState* parentstate )
{

    float ancho, alto;

    BeginDraw( tempogral, parentstate);

    if ( ! m_TimerInterval.Started() || m_TimerInterval.Duration()<0 )
        m_TimerInterval.Start();

    // Guardar y resetar la matriz de vista del modelo //
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glLoadIdentity();									// Reset The View
    // Cambiar la proyeccion para una vista ortogonal //
    glDisable(GL_DEPTH_TEST);       // Disables Depth Testing
    //glDepthMask(GL_FALSE);

    m_pResourceManager->GetGLMan()->SetOrthographicView();

    glEnable(GL_ALPHA);

    ancho = m_Config.Eval( moR(LIGHTDRAWING_WIDTH));
    alto = m_Config.Eval( moR(LIGHTDRAWING_HEIGHT));

    /// Draw //
    glTranslatef(  m_Config.Eval( moR(LIGHTDRAWING_TRANSLATEX)),
                   m_Config.Eval( moR(LIGHTDRAWING_TRANSLATEY)),
                   0.0);

    ///solo rotamos en el eje Z (0,0,1) ya que siempre estaremos perpedicular al plano (X,Y)
    glRotatef(  m_Config.Eval( moR(LIGHTDRAWING_ROTATE)), 0.0, 0.0, 1.0 );

    glScalef(   m_Config.Eval( moR(LIGHTDRAWING_SCALEX)),
                m_Config.Eval( moR(LIGHTDRAWING_SCALEY)),
                  1.0);

    //SetColor( m_Config[moR(LIGHTDRAWING_COLOR)][MO_SELECTED], m_Config[moR(LIGHTDRAWING_ALPHA)][MO_SELECTED], state );
    SetColor( m_Config[moR(LIGHTDRAWING_COLOR)], m_Config[moR(LIGHTDRAWING_ALPHA)], m_EffectState );

    SetBlending( (moBlendingModes) m_Config.Int( moR(LIGHTDRAWING_BLENDING) ) );

    //glBindTexture( GL_TEXTURE_2D, m_Config.GetGLId( moR(LIGHTDRAWING_TEXTURE), &m_EffectState.tempo) );
    if (m_pLightTexture) {
        glBindTexture( GL_TEXTURE_2D, m_pLightTexture->GetGLId() );
    }


    glBegin(GL_QUADS);
      glTexCoord2f( 0.0, 1.0 );
      glVertex2f( -0.5*ancho, -0.5*alto);

      glTexCoord2f( 1.0, 1.0 );
      glVertex2f(  0.5*ancho, -0.5*alto);

      glTexCoord2f( 1.0, 0.0 );
      glVertex2f(  0.5*ancho,  0.5*alto);

      glTexCoord2f( 0.0, 0.0 );
      glVertex2f( -0.5*ancho,  0.5*alto);
    glEnd();



    EndDraw();
}

MOboolean moEffectLightDrawing::Finish()
{
    return PreFinish();
}



void
moEffectLightDrawing::Update( moEventList *Events ) {

    moBucket *pbucket = NULL;
	moEvent *actual,*tmp;
	moVideoSample* pSample = NULL;
	moBucket*   pBucket = NULL;
	MOubyte* pbuffer = NULL;

    actual = Events->First;

    UpdateParameters();

    while(actual!=NULL) {
		//if(actual->deviceid == GetId() ) {



        if (actual->reservedvalue0 == -1 && actual->pointer!=NULL ) {

			pSample = (moVideoSample*)actual->pointer;
            pBucket = (moBucket*)pSample->m_pSampleBuffer;
/*
MODebug2->Push(
                            moText(" did:") + IntToStr ( actual->deviceid )
                            + moText(" dcode:") + IntToStr ( actual->devicecode )
                            + moText(" val0:") + IntToStr ( actual->reservedvalue0 )
                            + moText(" ptr:") + IntToStr ( (long)actual->pointer )
                            + moText(" videosample:") + IntToStr ( (long)pSample )
                            + moText(" pBucket:") + IntToStr ( (long)pBucket )
                       );
*/
            if (  pSample && pBucket ) {

                int m_sizebuffer = pBucket->GetSize();
                MOubyte * m_buffer = pBucket->GetBuffer();

                //procesar bucket...
                if ( m_pLightTexture==NULL) {

                    MODebug2->Push(
                            moText(" Adding LIGHTTEXTURE to manager")
                                   );

                    int tid = m_pResourceManager->GetTextureMan()->AddTexture( moText("LIGHTTEXTURE"), pSample->m_VideoFormat.m_Width,pSample->m_VideoFormat.m_Height );
                    if(tid>-1) {
                        m_pLightTexture = m_pResourceManager->GetTextureMan()->GetTexture(tid);
                        MODebug2->Push(
                            moText(" Added! id:")
                            + IntToStr(tid)
                                   );
                        m_pLightTexture->BuildEmpty(pSample->m_VideoFormat.m_Width,  pSample->m_VideoFormat.m_Height);
                        MODebug2->Push(
                            moText(" glid:")
                            + IntToStr(m_pLightTexture->GetGLId())
                                   );
                    }


                }


                if (m_pLightTexture && m_buffer) {
                    if ((m_pLightTexture->GetGLId() == 0) || (m_pLightTexture->GetWidth() != pSample->m_VideoFormat.m_Width))
                    {
                      if (m_pLightTexture->GetGLId() != 0) m_pLightTexture->Finish();
                      m_pLightTexture->BuildEmpty(pSample->m_VideoFormat.m_Width,  pSample->m_VideoFormat.m_Height);
                      MODebug2->Push(
                            moText(" Building LIGHTTEXTURE:")
                            + IntToStr(pSample->m_VideoFormat.m_Width)
                            + moText("X")
                            + IntToStr(pSample->m_VideoFormat.m_Height)
                                   );

                    }
/*
                  if (m_buffer_light_data) {
                    delete [] m_buffer_light_data;
                    m_buffer_light_data = NULL;
                  }
                  */
                  if (m_buffer_light_data==NULL) {
                      int dsize = pSample->m_VideoFormat.m_Width*pSample->m_VideoFormat.m_Height*3;
                        m_buffer_light_data = new MOubyte [dsize ];
                        memset((void*)m_buffer_light_data, 0,  dsize );
                        MODebug2->Push(
                            moText(" Creating buffer for LIGHTTEXTURE:")
                            + IntToStr(pSample->m_VideoFormat.m_Width)
                            + moText("X")
                            + IntToStr(pSample->m_VideoFormat.m_Height)
                                   );
                  }

                    /*
                    if (m_pLightTexture) {
                        //m_pLiveTexture->GetBuffer();
                        if (pTex) {

                            pTexSample = new moVideoSample();
                            if (pTexSample) {
                                pTexSample->m_VideoFormat.m_Width = pTex->GetWidth();
                                pTexSample->m_VideoFormat.m_Height = pTex->GetHeight();
                                pTexSample->m_VideoFormat.m_BufferSize = pTex->GetWidth()*pTex->GetHeight()*3;

                                pBucket = new moBucket();
                                if (pBucket) {
                                    ///ATENCION!!! al hacer un BuildBucket, hay que hacer un EmptyBucket!!!
                                    pBucket->BuildBucket( pTexSample->m_VideoFormat.m_BufferSize, 0);
                                    pTex->GetBuffer( (void*) pBucket->GetBuffer(), GL_RGB, GL_UNSIGNED_BYTE );
                                    ///asignamos el bucket al videosample
                                    pTexSample->m_pSampleBuffer = pBucket;

                                }
                            }

                        }

                    }
                    */

                    int index_t;

                    if (m_TimerInterval.Duration()>interval) {
                        fade = 1.0;
                        m_TimerInterval.Stop();
                        m_TimerInterval.Start();
                    }

                    float fade2 = momax(0.0, 1.0 - fade);

                    if (m_buffer_light_data) {
                        for( int j=0; j<m_pLightTexture->GetHeight(); j++ ) {
                            for( int i=0; i<m_pLightTexture->GetWidth(); i++ ) {
                                index_t = i*3 + j*m_pLightTexture->GetWidth()*3;
                                //index_t = index_t*3;
                                //m_buffer[index_t] = 0;
                                //m_buffer[index_t] = m_buffer[index_t];
                                //m_buffer[index_t+1] = 0;
                                //m_buffer[index_t+2] = 0;
                                //float Lum = (float)m_buffer[index_t]*0.11/255.0 + (float)m_buffer[index_t+1]*0.59/255.0 + (float)m_buffer[index_t+2]*0.3/255.0;
                                float Lum = 0,r=0,g=0,b=0;
                                b = momax( b, (float)m_buffer[index_t]/255.0 );
                                g = momax( g, (float)m_buffer[index_t+1]/255.0 );
                                r = momax( r, (float)m_buffer[index_t+2]/255.0 );
                                Lum = momax( Lum , r);
                                Lum = momax( Lum , g);
                                Lum = momax( Lum , b);


                                float fr,fg,fb;
                                int ir,ig,ib;
                                fr = (float) fade2 * (float)m_buffer_light_data[index_t+2];
                                fg = (float) fade2 * (float)m_buffer_light_data[index_t+1];
                                fb = (float) fade2 * (float)m_buffer_light_data[index_t];

                                if ( Lum >= m_intensity_min ) {



                                    fr = (fr + (float)m_buffer[index_t+2])*0.5;
                                    fg = (fg + (float)m_buffer[index_t+1])*0.5;
                                    fb = (fb + (float)m_buffer[index_t])*0.5;
                                    ib = (int) momax( 0, (int)fb);
                                    ig = (int) momax( 0, (int)fg);
                                    ir = (int) momax( 0, (int)fr);

                                    ib = (int) momin( 255, ib);
                                    ig = (int) momin( 255, ig);
                                    ir = (int) momin( 255, ir);
/*
                                    MODebug2->Push( "ir"+IntToStr( ir )
                                                   +"ig"+IntToStr( ig )
                                                   +"ib"+IntToStr( ib ) );
*/
                                    m_buffer_light_data[index_t] =  ib;
                                    m_buffer_light_data[index_t+1] = ig;
                                    m_buffer_light_data[index_t+2] = ir;
                                } else {
                                    /** deja como esta, aplica fade */
                                    /** ANDA OK*/
                                    m_buffer_light_data[index_t] = momax( 0, (int)fb);
                                    m_buffer_light_data[index_t+1] = momax( 0, (int)fg);
                                    m_buffer_light_data[index_t+2] = momax( 0, (int)fr);
                                }

                            }
                        }

                        m_pLightTexture->SetBuffer( m_buffer_light_data, GL_BGR_EXT);
                    }
                } else {
                    if (!m_pLightTexture) {
                        MODebug2->Push(
                            moText(" Texture not created"));
                    }
                }


            }

			tmp = actual->next;
			//Events->Delete(actual);
			actual = tmp;
		} else actual = actual->next;//no es el que necesitamos...
	}

	//get the pointer from the Moldeo Object sending it...
	moMoldeoObject::Update(Events);
}
