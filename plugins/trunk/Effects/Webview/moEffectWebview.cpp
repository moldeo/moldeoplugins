/*******************************************************************************

                              moEffectWebview.cpp

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

#include "GL/glew.h"
//#include "GL/glx.h"
#include "moEffectWebview.h"
#include <Awesomium/STLHelpers.h>
//========================
//  Factory
//========================

moEffectWebviewFactory *m_EffectTextFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectTextFactory==NULL)
		m_EffectTextFactory = new moEffectWebviewFactory();
	return(moEffectFactory*) m_EffectTextFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectTextFactory;
	m_EffectTextFactory = NULL;
}

moEffect*  moEffectWebviewFactory::Create() {
	return new moEffectWebview();
}

void moEffectWebviewFactory::Destroy(moEffect* fx) {
	delete fx;
}

//========================
//  Efecto
//========================

moEffectWebview::moEffectWebview() {
	SetName("webview");
	m_idsurface = -1;
	m_glidsurface = -1;
	m_pMoldeoSurface = NULL;
	m_pWebCore = NULL;
	m_Url = "";
	m_CursorX = 0.0;
	m_CursorY = 0.0;
}

moEffectWebview::~moEffectWebview() {
	Finish();
}

MOboolean
moEffectWebview::Init()
{
    if (!PreInit()) return false;

  moDefineParamIndex( WEBVIEW_INLET, moText("inlet") );
	moDefineParamIndex( WEBVIEW_OUTLET, moText("outlet") );
	moDefineParamIndex( WEBVIEW_ALPHA, moText("alpha") );
	moDefineParamIndex( WEBVIEW_COLOR, moText("color") );
	moDefineParamIndex( WEBVIEW_SYNC, moText("syncro") );
	moDefineParamIndex( WEBVIEW_PHASE, moText("phase") );
	moDefineParamIndex( WEBVIEW_URL, moText("url") );
	moDefineParamIndex( WEBVIEW_BLENDING, moText("blending") );
	moDefineParamIndex( WEBVIEW_WIDTH, moText("width") );
	moDefineParamIndex( WEBVIEW_HEIGHT, moText("height") );
	moDefineParamIndex( WEBVIEW_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( WEBVIEW_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( WEBVIEW_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( WEBVIEW_SCALEX, moText("scalex") );
	moDefineParamIndex( WEBVIEW_SCALEY, moText("scaley") );
	moDefineParamIndex( WEBVIEW_SCALEZ, moText("scalez") );
	moDefineParamIndex( WEBVIEW_ROTATEX, moText("rotatex") );
	moDefineParamIndex( WEBVIEW_ROTATEY, moText("rotatey") );
	moDefineParamIndex( WEBVIEW_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( WEBVIEW_EYEX, moText("eyex") );
	moDefineParamIndex( WEBVIEW_EYEY, moText("eyey") );
	moDefineParamIndex( WEBVIEW_EYEZ, moText("eyez") );
	moDefineParamIndex( WEBVIEW_VIEWX, moText("viewx") );
	moDefineParamIndex( WEBVIEW_VIEWY, moText("viewy") );
	moDefineParamIndex( WEBVIEW_VIEWZ, moText("viewz") );
	moDefineParamIndex( WEBVIEW_SHOWCURSOR, moText("showcursor") );

  int w = RenderMan()->ScreenWidth();
  int h = RenderMan()->ScreenHeight();

  m_idsurface = TextureMan()->AddTexture( "webview_"+this->GetLabelName(), w, h );

  m_pMoldeoSurface = TextureMan()->GetTexture( m_idsurface );
  if (m_pMoldeoSurface) {
    m_pMoldeoSurface->BuildEmpty(1280,800);
    m_glidsurface = m_pMoldeoSurface->GetGLId();
  }
  else { MODebug2->Error("moEffectWebview::Init() Surface do not exists"); return false; }

  Awesomium::WebConfig conf;
  conf.log_level = kLogLevel_Verbose;

  m_pWebCore = WebCore::Initialize(conf);

  if(m_pWebCore) {
    m_pWebCore->set_surface_factory(new GLTextureSurfaceFactory());
    m_pWebView = WebCore::instance()->CreateWebView(1280, 800);

    if (m_pWebView) {
      Load( m_Config.Text(moR(WEBVIEW_URL)) );
      MODebug2->Push("moEffectWebview::Init() > m_pWebView created with w = " + IntToStr(1280) + " h:" + IntToStr(800) );

    } else MODebug2->Error("moEffectWebview::Init() > NO WEBVIEW!");
  } else MODebug2->Error("moEffectWebview::Init() > NO WEBCORE!");
	return true;

}

void moEffectWebview::Load(moText url) {

//    DataSource* data_source = new MyDataSource();
//    web_session->AddDataSource(WSLit("MyApplication"), data_source);

    m_Url = url;
    WebURL webUrl(WSLit(url));
    if (m_pWebView) {
      MODebug2->Push("moEffectWebview::Load() > Loading url" + url );
      m_pWebView->LoadURL( webUrl );
      m_pWebView->set_view_listener(this);
      m_pWebView->set_load_listener(this);
      m_pWebView->set_process_listener(this);
      BindMethods( m_pWebView );

    }
}

void moEffectWebview::Draw( moTempo* tempogral, moEffectState* parentstate)
{

    double ancho,alto;
    int w = RenderMan()->ScreenWidth();
    int h = RenderMan()->ScreenHeight();
    float prop = RenderMan()->ScreenProportion();

    PreDraw( tempogral, parentstate);

    moText newUrl = m_Config.Text(moR(WEBVIEW_URL));
//    WebURL newWebUrl( WSLit(newUrl) ) ;

    if ( newUrl != m_Url ) {
      MODebug2->Push("moEffectWebview::Draw > Loading newurl" + newUrl );
      Load( newUrl );
    }


/*
    if ( m_pWebView->url() != newWebUrl ) {
        MODebug2->Push("moEffectWebview::Draw > Loading newurl" + newUrl );
        //Load( newUrl );
    }
*/

    // Cambiar la proyeccion para una vista ortogonal //
    glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    glLoadIdentity();									// Reset The Projection Matrix
    //glOrtho(0,w,0,h,-1,1);                              // Set Up An Ortho Screen
    GLMan()->SetOrthographicView( w, h );





/*
    GLMan()->SetPerspectiveView( w, h );

  glMatrixMode(GL_PROJECTION);
	gluLookAt(		m_Config.Eval(moR(WEBVIEW_EYEX)),
					m_Config.Eval(moR(WEBVIEW_EYEY)),
					m_Config.Eval(moR(WEBVIEW_EYEZ)),
					m_Config.Eval(moR(WEBVIEW_VIEWX)),
					m_Config.Eval(moR(WEBVIEW_VIEWY)),
					m_Config.Eval(moR(WEBVIEW_VIEWZ)),
					0, 1, 0);
*/

  glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
  glLoadIdentity();									// Reset The View


glTranslatef(   m_Config.Eval( moR(WEBVIEW_TRANSLATEX) ),
                  m_Config.Eval( moR(WEBVIEW_TRANSLATEY)),
                  m_Config.Eval( moR(WEBVIEW_TRANSLATEZ))
              );

	glRotatef(  m_Config.Eval( moR(WEBVIEW_ROTATEZ) ), 0.0, 0.0, 1.0 );
    glRotatef(  m_Config.Eval( moR(WEBVIEW_ROTATEY) ), 0.0, 1.0, 0.0 );
    glRotatef(  m_Config.Eval( moR(WEBVIEW_ROTATEX) ), 1.0, 0.0, 0.0 );

	glScalef(   m_Config.Eval( moR(WEBVIEW_SCALEX)),
              m_Config.Eval( moR(WEBVIEW_SCALEY)),
              m_Config.Eval( moR(WEBVIEW_SCALEZ))
            );



  const GLTextureSurface* surf = this->surface();
  if (surf) {
      m_glidsurface = surf->GetTexture();
      //MODebug2->Push("moEffectWebview::Init() > surface gl id = " + IntToStr(m_glidsurface) );
  }
  glBindTexture( GL_TEXTURE_2D, m_glidsurface );

  SetBlending( (moBlendingModes) m_Config.Int( moR(WEBVIEW_BLENDING) ) );

	ancho = (int)m_Config.Eval( moR(WEBVIEW_WIDTH) );
	alto = (int)m_Config.Eval( moR(WEBVIEW_HEIGHT) );


	glBegin(GL_QUADS);
		glTexCoord2f( 0.0, 0.0);
		glVertex2f( -ancho*0.5, -alto*0.5);

		glTexCoord2f( 1.0, 0.0);
		glVertex2f(  ancho*0.5, -alto*0.5);

		glTexCoord2f( 1.0, 1.0);
		glVertex2f(  ancho*0.5,  alto*0.5);

		glTexCoord2f( 0.0, 1.0);
		glVertex2f( -ancho*0.5,  alto*0.5);
	glEnd();


	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix


  glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
  glLoadIdentity();									// Reset The View

    ancho = 2;
    alto = 2;
	
    
	SetColor( m_Config[moR(WEBVIEW_COLOR)], m_Config[moR(WEBVIEW_ALPHA)], m_EffectState );
	
	glBindTexture( GL_TEXTURE_2D, -1 );
    glBegin(GL_QUADS);
		glTexCoord2f( 0.0, 0.0);
		glVertex2f( -ancho*0.5+m_CursorX, -alto*0.5+h-m_CursorY);

		glTexCoord2f( 1.0, 0.0);
		glVertex2f(  ancho*0.5+m_CursorX, -alto*0.5+h-m_CursorY);

		glTexCoord2f( 1.0, 1.0);
		glVertex2f(  ancho*0.5+m_CursorX,  alto*0.5+h-m_CursorY);

		glTexCoord2f( 0.0, 1.0);
		glVertex2f( -ancho*0.5+m_CursorX,  alto*0.5+h-m_CursorY);
	glEnd();
	
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix
    glLoadIdentity();									// Reset The View


	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix


}

void moEffectWebview::setUpLighting()
{

}


MOboolean moEffectWebview::Finish()
{
    if (m_pWebCore) {
      m_pWebCore->Shutdown();
    }
    return PreFinish();
}

//====================
//
//		CUSTOM
//
//===================


void moEffectWebview::Interaction( moIODeviceManager *IODeviceManager ) {

}


moConfigDefinition *
moEffectWebview::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("url"), MO_PARAM_TEXT, WEBVIEW_URL, moValue( "Url to complete", "TXT") );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, WEBVIEW_BLENDING, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("width"), MO_PARAM_FUNCTION, WEBVIEW_WIDTH, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("height"), MO_PARAM_FUNCTION, WEBVIEW_HEIGHT, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, WEBVIEW_TRANSLATEX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, WEBVIEW_TRANSLATEY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, WEBVIEW_TRANSLATEZ, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, WEBVIEW_ROTATEX, moValue( "0.0", "FUNCTION").Ref() );
    p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, WEBVIEW_ROTATEY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, WEBVIEW_ROTATEZ, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, WEBVIEW_SCALEX, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, WEBVIEW_SCALEY, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, WEBVIEW_SCALEZ, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("eyex"), MO_PARAM_FUNCTION, WEBVIEW_EYEX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("eyey"), MO_PARAM_FUNCTION, WEBVIEW_EYEY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("eyez"), MO_PARAM_FUNCTION, WEBVIEW_EYEZ, moValue( "100.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("viewx"), MO_PARAM_FUNCTION, WEBVIEW_VIEWX, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("viewy"), MO_PARAM_FUNCTION, WEBVIEW_VIEWY, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("viewz"), MO_PARAM_FUNCTION, WEBVIEW_VIEWZ, moValue( "0.0", "FUNCTION").Ref() );
    p_configdefinition->Add( moText("showcursor"), MO_PARAM_NUMERIC, WEBVIEW_SHOWCURSOR, moValue( "1", "NUM").Ref() );
	return p_configdefinition;
}

void
moEffectWebview::Update( moEventList *Events ) {

	//get the pointer from the Moldeo Object sending it...
	moMoldeoObject::Update(Events);

  if (m_pWebView && m_pWebCore) {
    //MODebug2->Push("moEffectWebview::Update() > Webcore Updating...");

    /*Handle events*/
    moEvent *tmp;
    moEvent *actual;

    actual = Events->First;

    while(actual!=NULL) {

      if (m_pWebView)
      switch(actual->deviceid) {
		case MO_IODEVICE_TOUCH:
			switch(actual->devicecode) {

				case MO_TOUCH_START:
					m_WebTouchEvent.type = kWebTouchEventType_Start;
					m_WebTouchEvent.touches[0].id = 0;//actual->reservedvalue0;
					m_WebTouchEvent.touches[0].position_x = actual->reservedvalue1;
					m_WebTouchEvent.touches[0].position_y = actual->reservedvalue2;
					m_WebTouchEvent.touches[0].force = 0;
					m_WebTouchEvent.touches[0].state = kWebTouchPointState_Pressed;
					m_WebTouchEvent.touches[0].screen_position_x = actual->reservedvalue1;
					m_WebTouchEvent.touches[0].screen_position_y = actual->reservedvalue2;
					m_WebTouchEvent.touches_length = 1;
					m_WebTouchEvent.target_touches[0] = m_WebTouchEvent.touches[0];
					m_WebTouchEvent.target_touches_length = 1;
					m_WebTouchEvent.changed_touches[0] = m_WebTouchEvent.touches[0];
					m_WebTouchEvent.changed_touches_length = 1;
					m_pWebView->InjectTouchEvent( m_WebTouchEvent );

					m_CursorX = actual->reservedvalue1;
					m_CursorY = actual->reservedvalue2;
					//MODebug2->Message("moEffectWebview::InjectTouchEvent() > MO_TOUCH_START at ");
					break;

				case MO_TOUCH_END:
					m_WebTouchEvent.type = kWebTouchEventType_End;
					m_WebTouchEvent.touches[0].id = 0; //actual->reservedvalue0;
					m_WebTouchEvent.touches[0].position_x = actual->reservedvalue1;
					m_WebTouchEvent.touches[0].position_y = actual->reservedvalue2;
					m_WebTouchEvent.touches[0].force = 0;
					m_WebTouchEvent.touches[0].state = kWebTouchPointState_Released;
					m_WebTouchEvent.touches[0].screen_position_x = actual->reservedvalue1;
					m_WebTouchEvent.touches[0].screen_position_y = actual->reservedvalue2;
					m_WebTouchEvent.touches_length = 1;


					m_WebTouchEvent.target_touches[0] = m_WebTouchEvent.touches[0];
					m_WebTouchEvent.target_touches_length = 1;

					m_WebTouchEvent.changed_touches[0] = m_WebTouchEvent.touches[0];
					m_WebTouchEvent.changed_touches_length = 1;
					m_pWebView->InjectTouchEvent( m_WebTouchEvent );

					m_CursorX = actual->reservedvalue1;
					m_CursorY = actual->reservedvalue2;
					//MODebug2->Message("moEffectWebview::InjectTouchEvent() > MO_TOUCH_END at ");
					break;

				case MO_TOUCH_MOVE:
					m_WebTouchEvent.type = kWebTouchEventType_Move;
					m_WebTouchEvent.touches[0].id = 0; //actual->reservedvalue0;
					m_WebTouchEvent.touches[0].position_x = actual->reservedvalue1;
					m_WebTouchEvent.touches[0].position_y = actual->reservedvalue2;
					m_WebTouchEvent.touches[0].force = 0;
					m_WebTouchEvent.touches[0].state = kWebTouchPointState_Moved;
					m_WebTouchEvent.touches[0].screen_position_x = actual->reservedvalue1;
					m_WebTouchEvent.touches[0].screen_position_y = actual->reservedvalue2;
					m_WebTouchEvent.touches_length = 1;

					m_WebTouchEvent.target_touches[0] = m_WebTouchEvent.touches[0];
					m_WebTouchEvent.target_touches_length = 1;

					m_WebTouchEvent.changed_touches[0] = m_WebTouchEvent.touches[0];
					m_WebTouchEvent.changed_touches_length = 1;
					m_pWebView->InjectTouchEvent( m_WebTouchEvent );

					m_CursorX = actual->reservedvalue1;
					m_CursorY = actual->reservedvalue2;
					//MODebug2->Message("moEffectWebview::InjectTouchEvent() > MO_TOUCH_MOVE at ");
					break;
			}
			break;
			
        case MO_IODEVICE_MOUSE:
			
          switch(actual->devicecode) {
            case SDL_MOUSEMOTION:
              //cout << "Webview receiving mouse motion: x: " << actual->reservedvalue2 << " y:" << actual->reservedvalue3 << endl;
              m_pWebView->InjectMouseMove( actual->reservedvalue2, actual->reservedvalue3 );
              m_CursorX = actual->reservedvalue2;
              m_CursorY = actual->reservedvalue3;
              break;
            case SDL_MOUSEBUTTONDOWN:
              //cout << "Webview receiving mouse button down: " << actual->reservedvalue0 << " x:" << actual->reservedvalue1 << " y:" << actual->reservedvalue2 << endl;
              //m_pWebView->InjectMouseMove( actual->reservedvalue1, actual->reservedvalue2 );

              m_CursorX = actual->reservedvalue1;
              m_CursorY = actual->reservedvalue2;

              switch(actual->reservedvalue0) {
                case SDL_BUTTON_LEFT:
                  m_pWebView->InjectMouseDown(Awesomium::kMouseButton_Left);
                  break;
                case SDL_BUTTON_MIDDLE:
                  m_pWebView->InjectMouseDown(Awesomium::kMouseButton_Middle);
                  break;
                case SDL_BUTTON_RIGHT:
                  m_pWebView->InjectMouseDown(Awesomium::kMouseButton_Right);
                  break;
              }
              break;
            case SDL_MOUSEBUTTONUP:
              //cout << "Webview receiving mouse button up: " << actual->reservedvalue0 << " x:" << actual->reservedvalue1 << " y:" << actual->reservedvalue2 << endl;
              //m_pWebView->InjectMouseMove( actual->reservedvalue1, actual->reservedvalue2 );

              m_CursorX = actual->reservedvalue1;
              m_CursorY = actual->reservedvalue2;


              switch(actual->reservedvalue0) {
                case SDL_BUTTON_LEFT:
                  m_pWebView->InjectMouseUp(Awesomium::kMouseButton_Left);
                  break;
                case SDL_BUTTON_MIDDLE:
                  m_pWebView->InjectMouseUp(Awesomium::kMouseButton_Middle);
                  break;
                case SDL_BUTTON_RIGHT:
                  m_pWebView->InjectMouseUp(Awesomium::kMouseButton_Right);
                  break;
              }
              break;
          }
          break;
		  
        case MO_IODEVICE_KEYBOARD:

          break;
      }

      tmp = actual;
      actual = tmp->next;

    }


    //m_JSEvents, send all events cached in JSEvents to GetEvents() IODeviceManager device

    actual = m_JSEvents.First;
    while(actual!=NULL) {

        if ( actual->deviceid==-1 && actual->reservedvalue3 == MO_MESSAGE ) {
            cout << "moEffectWebview::Update >> Adding message to Events " << endl;
            moMessage *message = (moMessage*) actual;
            moMessage *newmessage = new moMessage();
            if (newmessage) {
                *newmessage = *message;

                Events->Add( (moEvent*)newmessage );
                cout << "moEffectWebview::Update >> Adding message to Events: ADDED with data: " << newmessage->m_Data.ToText() << endl;
            }
        }

        tmp = actual;
        actual = tmp->next;
    }
    m_JSEvents.Finish();


    m_pWebCore->Update();
  }

}



void
moEffectWebview::OnJSMoldeoAction(  WebView* caller,
                                    const JSArray& args)
{

    cout << "moEffectWebview::OnJSMoldeoAction" << args.size() <<  endl;

    moText action;
    moText object_label;
    moText position;
    moData actiondata;
    moText param_label;
    moText value_index;
    moMessage* pmessage = NULL;


  switch(args.size()) {
    case 0:
        cout << "moEffectWebview::OnJSMoldeoAction received with no arguments. DO NOTHING." << endl;
        break;
    case 1:
        cout << "moEffectWebview::OnJSMoldeoAction received with one argument. NOT ENOUGH ARGS: [" << ToString( args[0].ToString() ) << "]" << endl;
        break;
    case 2:
        action = ToString( args[0].ToString() ).c_str();
        object_label = ToString( args[1].ToString() ).c_str();

        actiondata.SetText(action+","+object_label);
        //moData objectdata(object_label);
        pmessage = new moMessage( -1, this->GetId(), actiondata );
        m_JSEvents.Add( (moEvent*) pmessage );

        break;

    case 3:
        action = ToString( args[0].ToString() ).c_str();
        object_label =  ToString( args[1].ToString() ).c_str();
        position = ToString( args[2].ToString() ).c_str();

        actiondata.SetText( action+","+object_label+","+position);

        pmessage = new moMessage( -1, this->GetId(), actiondata );
        m_JSEvents.Add( (moEvent*) pmessage );
        break;

    case 4:
        action = ToString( args[0].ToString() ).c_str();
        object_label = ToString( args[1].ToString() ).c_str();
        param_label = ToString( args[2].ToString() ).c_str();
        value_index = ToString( args[3].ToString() ).c_str();

        actiondata.SetText( action+","+object_label+","+param_label+","+value_index);

        pmessage = new moMessage( -1, this->GetId(), actiondata );
        m_JSEvents.Add( (moEvent*) pmessage );
        break;
  };

  cout << "NARGS: " << args.size() << endl;
  cout << "action: " << action << endl;
  cout << "object_label: " << object_label << endl;
  cout << "position: " << position << endl;
  cout << "param_label: " << param_label << endl;
  cout << "value_index: " << value_index << endl;
  cout << "action data:" << actiondata.ToText() << endl;
  //MODebug2->Message("moEffectWebview::OnJSMoldeoAction");



   //m_JSEvents.Add();

}



void
moEffectWebview::BindMethods(WebView* web_view) {

    // Create a global js object named 'app'
    JSValue result = web_view->CreateGlobalJavascriptObject(WSLit("app"));
    if (result.IsObject()) {
      // Bind our custom method to it.
      JSObject& app_object = result.ToObject();
      method_dispatcher_.Bind( app_object,
                              WSLit("JSMoldeoAction"),
                              JSDelegate(this, &moEffectWebview::OnJSMoldeoAction ));
    }

    // Bind our method dispatcher to the WebView
    web_view->set_js_method_handler(&method_dispatcher_);
  }



void moEffectWebview::OnChangeTitle(Awesomium::WebView* caller,
                                const Awesomium::WebString& title) {
}

void moEffectWebview::OnChangeAddressBar(Awesomium::WebView* caller,
                                     const Awesomium::WebURL& url) {
}

void moEffectWebview::OnChangeTooltip(Awesomium::WebView* caller,
                                  const Awesomium::WebString& tooltip) {
}

void moEffectWebview::OnChangeTargetURL(Awesomium::WebView* caller,
                                    const Awesomium::WebURL& url) {
}

void moEffectWebview::OnChangeCursor(Awesomium::WebView* caller,
                                 Awesomium::Cursor cursor) {
}

void moEffectWebview::OnChangeFocus(Awesomium::WebView* caller,
                                Awesomium::FocusedElementType focus_type) {
}

void moEffectWebview::OnAddConsoleMessage(Awesomium::WebView* caller,
                                   const Awesomium::WebString& message,
                                   int line_number,
                                   const Awesomium::WebString& source) {
}

void moEffectWebview::OnShowCreatedWebView( WebView* caller,
                                       WebView* new_view,
                                       const Awesomium::WebURL& opener_url,
                                       const Awesomium::WebURL& target_url,
                                       const Awesomium::Rect& initial_pos,
                                       bool is_popup) {
  /*
  new_view->Resize(WIDTH, HEIGHT);
  WebTile* new_tile = new WebTile(new_view, WIDTH, HEIGHT);
  new_view->set_view_listener(this);
  new_view->set_load_listener(this);
  new_view->set_process_listener(this);
  webTiles.push_back(new_tile);
  animateTo(webTiles.size() - 1);
  */
}

void moEffectWebview::OnBeginLoadingFrame(Awesomium::WebView* caller,
                                   int64 frame_id,
                                   bool is_main_frame,
                                   const Awesomium::WebURL& url,
                                   bool is_error_page) {
}

void moEffectWebview::OnFailLoadingFrame(Awesomium::WebView* caller,
                                  int64 frame_id,
                                  bool is_main_frame,
                                  const Awesomium::WebURL& url,
                                  int error_code,
                                  const Awesomium::WebString& error_description) {
}

void moEffectWebview::OnFinishLoadingFrame(Awesomium::WebView* caller,
                                   int64 frame_id,
                                   bool is_main_frame,
                                   const Awesomium::WebURL& url) {
  if (is_main_frame)
    MODebug2->Push("moEffectWebpageview > Page Loaded ["+ moText( (wchar_t*)caller->title().data() ) + "] url[" + moText((wchar_t*)url.spec().data()));

}

void moEffectWebview::OnDocumentReady(Awesomium::WebView* caller,
                                  const Awesomium::WebURL& url) {
}

void moEffectWebview::OnUnresponsive(Awesomium::WebView* caller) {
}

void moEffectWebview::OnResponsive(Awesomium::WebView* caller) {
}

void moEffectWebview::OnCrashed(Awesomium::WebView* caller,
                            Awesomium::TerminationStatus status) {
  std::cout << "WebView crashed with status: " << (int)status << std::endl;
}
