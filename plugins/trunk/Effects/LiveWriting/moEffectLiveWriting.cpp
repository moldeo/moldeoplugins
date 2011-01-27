/*******************************************************************************

                              moEffectLiveWriting.cpp

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
  Andrés Colubri

*******************************************************************************/

#include "moEffectLiveWriting.h"
#ifdef MO_WIN32
  #include "SDL.h"
#else
  #include "SDL/SDL.h"
#endif

#include "moArray.cpp"

moDefineDynamicArray( moLiveChars );
moDefineDynamicArray( moLivePhrases );
moDefineDynamicArray( moLiveParagraphs );
moDefineDynamicArray( moLiveChapter );
moDefineDynamicArray( moLiveBook );

moDefineDynamicArray( moLiveFrameZones );
moDefineDynamicArray( moLiveLayouts );
moDefineDynamicArray( moLiveFrames );
moDefineDynamicArray( moLiveComicBook );




//========================
//  Factory
//========================

moEffectLiveWritingFactory *m_EffectTextFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectTextFactory==NULL)
		m_EffectTextFactory = new moEffectLiveWritingFactory();
	return(moEffectFactory*) m_EffectTextFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectTextFactory;
	m_EffectTextFactory = NULL;
}

moEffect*  moEffectLiveWritingFactory::Create() {
	return new moEffectLiveWriting();
}

void moEffectLiveWritingFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Clases
//========================

moLivePhrase::moLivePhrase() {

}

moLivePhrase::~moLivePhrase() {

}

moLivePhrase &
moLivePhrase::operator = (const moLivePhrase &src) {

  m_Chars = src.m_Chars;
  return (*this);
}


void
moLivePhrase::Add( moLiveChar c ) {

    m_Chars.Add( c );

}

void
moLivePhrase::Delete() {

    m_Chars.Remove( m_Chars.Count()-1 );

}

//==========================================================

moLiveParagraph::moLiveParagraph() {

}

moLiveParagraph::~moLiveParagraph() {

}

moLiveParagraph &
moLiveParagraph::operator = (const moLiveParagraph &src) {

  m_Phrases = src.m_Phrases;
  return (*this);

}


//==========================================================


moLivePage::moLivePage() {

}

moLivePage::~moLivePage() {

}

moLivePage &
moLivePage::operator = (const moLivePage &src) {

  m_Paragraphs = src.m_Paragraphs;
  return (*this);

}

//==========================================================


moLiveFrame::moLiveFrame() {

}

moLiveFrame::~moLiveFrame() {

}

moLiveFrame &
moLiveFrame::operator = (const moLiveFrame &src) {

  m_Phrases = src.m_Phrases;
  m_Zone = src.m_Zone;
  return (*this);

}

//==========================================================


moLiveFrameZone::moLiveFrameZone() {

}

moLiveFrameZone::~moLiveFrameZone() {

}

moLiveFrameZone &
moLiveFrameZone::operator = (const moLiveFrameZone &src) {

  width = src.width;
  height = src.height;

  for(int i=0; i<BORDERS; i++) {

      position[i] = src.position[i];
      padding[i] = src.padding[i];
      margin[i] = src.margin[i];
      border_type[i] = src.border_type[i];
      border_size[i] = src.border_size[i];

      m_Font = src.m_Font;
      m_FontColor = src.m_FontColor;
      m_BorderColor[i] = src.m_BorderColor[i];


  }


  return (*this);

}


//==========================================================


moLiveLayout::moLiveLayout() {

}

moLiveLayout::~moLiveLayout() {

}

moLiveLayout &
moLiveLayout::operator = (const moLiveLayout &src) {


  return (*this);

}

//==========================================================

moLiveComicPage::moLiveComicPage() {

}

moLiveComicPage::~moLiveComicPage() {

}

moLiveComicPage &
moLiveComicPage::operator = (const moLiveComicPage &src) {

  m_Frames = src.m_Frames;
  m_FramePosition = src.m_FramePosition;
  return (*this);

}


void
moLiveComicPage::NextPosition() { ///va hacia el proximo frame

    m_FramePosition++;

    if (m_Frames.Count()==0) {
        m_FramePosition = 0;
        return;
    }

    m_FramePosition = std::min( (int)m_FramePosition, (int)(m_Frames.Count()-1) );


}

void
moLiveComicPage::PrevPosition() { ///va hacia el frame anterior

    m_FramePosition--;

    m_FramePosition = std::max( 0, m_FramePosition );

}

void
moLiveComicPage::LeftPosition() { ///va hacia el frame mas surdo

}

void
moLiveComicPage::UpPosition() { ///va hacia el frame mas arriba

}

void
moLiveComicPage::DownPosition() { ///va hacia el frame mas abajo

}

void
moLiveComicPage::RightPosition() { ///va hacia el frame mas diestro

}


//========================
//  Efecto
//========================

moEffectLiveWriting::moEffectLiveWriting() {
	SetName("livewriting");
}

moEffectLiveWriting::~moEffectLiveWriting() {
	Finish();
}


moConfigDefinition *
moEffectLiveWriting::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("mode"), MO_PARAM_TEXT, LIVEWRITING_MODE, moValue( "direct", MO_VALUE_TXT ) );
	p_configdefinition->Add( moText("layouts"), MO_PARAM_TEXT, LIVEWRITING_LAYOUTS, moValue( "<div id=\"layout1\"></div><div id=\"layout2\"></div>", MO_VALUE_TXT ) );
	p_configdefinition->Add( moText("font"), MO_PARAM_FONT, LIVEWRITING_FONT, moValue( "Default", "TXT", "0", "NUM", "32.0", "NUM") );
	p_configdefinition->Add( moText("text"), MO_PARAM_TEXT, LIVEWRITING_TEXT, moValue( "Insert text in text parameter", MO_VALUE_TXT ) );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, LIVEWRITING_BLENDING, moValue( "0", MO_VALUE_NUM ).Ref() );
	p_configdefinition->Add( moText("width"), MO_PARAM_FUNCTION, LIVEWRITING_WIDTH, moValue( "1.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("height"), MO_PARAM_FUNCTION, LIVEWRITING_HEIGHT, moValue( "1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, LIVEWRITING_TRANSLATEX, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, LIVEWRITING_TRANSLATEY, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("rotate"), MO_PARAM_ROTATEZ, LIVEWRITING_ROTATE, moValue( "0.0", MO_VALUE_FUNCTION ).Ref() );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, LIVEWRITING_SCALEX, moValue( "1.0", MO_VALUE_FUNCTION).Ref() );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, LIVEWRITING_SCALEY, moValue( "1.0", MO_VALUE_FUNCTION).Ref() );
	return p_configdefinition;
}


MOboolean
moEffectLiveWriting::Init()
{
    if (!PreInit()) return false;

	moDefineParamIndex( LIVEWRITING_ALPHA, moText("alpha") );
	moDefineParamIndex( LIVEWRITING_COLOR, moText("color") );
	moDefineParamIndex( LIVEWRITING_SYNC, moText("syncro") );
	moDefineParamIndex( LIVEWRITING_PHASE, moText("phase") );
	moDefineParamIndex( LIVEWRITING_MODE, moText("mode") );
	moDefineParamIndex( LIVEWRITING_LAYOUTS, moText("layouts") );
	moDefineParamIndex( LIVEWRITING_FONT, moText("font") );
	moDefineParamIndex( LIVEWRITING_TEXT, moText("text") );
	moDefineParamIndex( LIVEWRITING_BLENDING, moText("blending") );
	moDefineParamIndex( LIVEWRITING_WIDTH, moText("width") );
	moDefineParamIndex( LIVEWRITING_HEIGHT, moText("height") );
	moDefineParamIndex( LIVEWRITING_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( LIVEWRITING_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( LIVEWRITING_SCALEX, moText("scalex") );
	moDefineParamIndex( LIVEWRITING_SCALEY, moText("scaley") );
	moDefineParamIndex( LIVEWRITING_ROTATE, moText("rotate") );
	moDefineParamIndex( LIVEWRITING_INLET, moText("inlet") );
	moDefineParamIndex( LIVEWRITING_OUTLET, moText("outlet") );

	return true;

}

void moEffectLiveWriting::Draw( moTempo* tempogral, moEffectState* parentstate)
{

    int ancho,alto;
    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    moFont* pFont = m_Config[ moR(LIVEWRITING_FONT) ].GetData()->Font();

    PreDraw( tempogral, parentstate);

    // Cambiar la proyeccion para una vista ortogonal //
    glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    glLoadIdentity();									// Reset The Projection Matrix
    glOrtho( -0.5, 0.5, -0.5*(h/w), 0.5*(h/w), -1, 1);          // Set Up An Ortho Screen
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glLoadIdentity();									// Reset The View


    glEnable(GL_BLEND);


    /// Draw //
    glTranslatef(  m_Config.Eval( moR(LIVEWRITING_TRANSLATEX), state.tempo.ang),
                   m_Config.Eval( moR(LIVEWRITING_TRANSLATEY), state.tempo.ang),
                   0.0);

    ///solo rotamos en el eje Z (0,0,1) ya que siempre estaremos perpedicular al plano (X,Y)
    glRotatef(  m_Config.Eval( moR(LIVEWRITING_ROTATE), state.tempo.ang), 0.0, 0.0, 1.0 );

    glScalef(   m_Config.Eval( moR(LIVEWRITING_SCALEX), state.tempo.ang ),
                m_Config.Eval( moR(LIVEWRITING_SCALEY), state.tempo.ang ),
                  1.0);

    SetColor( m_Config[moR(LIVEWRITING_COLOR)][MO_SELECTED], m_Config[moR(LIVEWRITING_ALPHA)][MO_SELECTED], state );

    SetBlending( (moBlendingModes) m_Config.Int( moR(LIVEWRITING_BLENDING) ) );

    moText Texto = m_Config.Text( moR(LIVEWRITING_TEXT) );

    float r1;
    r1 = 2.0 *((double)rand() /(double)(RAND_MAX+1));


    if (pFont) {
        pFont->Draw(    0.0,
                        0.0,
                        Texto,
                        m_Config[moR(LIVEWRITING_FONT)][MO_SELECTED][2].Int(), ///inner font size
                        0,
                        m_Config.Eval( moR(LIVEWRITING_SCALEX), state.tempo.ang ),
                        m_Config.Eval( moR(LIVEWRITING_SCALEY), state.tempo.ang ),
                        m_Config.Eval( moR(LIVEWRITING_ROTATE), state.tempo.ang ) );

        //moText infod = moText("screen width:")+IntToStr(w)+moText(" screen height:")+IntToStr(h);
        //pFont->Draw( 0, 0, infod, m_Config[moR(LIVEWRITING_FONT)][MO_SELECTED][2].Int(), 0, 2.0, 2.0, 0.0);
    }

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix


}

MOboolean moEffectLiveWriting::Finish()
{
    return PreFinish();
}


void moEffectLiveWriting::Interaction( moIODeviceManager *IODeviceManager ) {


  moDeviceCode *temp;
	MOint did,cid,state,valor;

	MOint d,i2;

	if(devicecode!=NULL)
	for(int i=0; i<ncodes;i++) {
		temp = devicecode[i].First;
		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			state = IODeviceManager->IODevices().Get(did)->GetStatus(cid);
			valor = IODeviceManager->IODevices().Get(did)->GetValue(cid);
			if(state) {

			  /*

						d = i / 10;
						i2 = i - d*10;
						MESHALIGHTS[i2*(Mwidth/(10))][d*(Mheight/(4))].Pressed = MO_PRESSED;

          */
          /*

          any character:

          */

          if ( SDLK_SPACE<=cid && cid<=SDLK_z ) {

              m_ActualPhrase.Add( moLiveChar( cid ) );

          } else {
            switch(cid) {

              case SDLK_BACKSPACE:
                m_ActualPhrase.Delete();
                break;

              case SDLK_RETURN:
                ///cambiar viñeta... automaticamente
                ///el cambio de pagina es automático?? o deberia hacer un PAGE_DOWN, PAGE_UP....
                ///m_ActualFrame = m_ActualComicPage.GetActualFrame();
                ///m_ActualFrame.Add(  );
                break;

              case SDLK_TAB:
                ///cambiar de zona (internamente cambia de zona dentro del layout )
                ///m_ActualComicPage.NextPosition();
                ///m_ActualFrame = m_ActualComicPage.GetActualFrame();
                break;




            }

          }


          MODebug2->Push(moText("tecla presionada") + IntToStr( cid ) );


			}
		temp = temp->next;
		}
	}

}
