/*******************************************************************************

                              moEffectWebview.h

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

#ifndef __MO_EFFECT_WEBVIEW_H__
#define __MO_EFFECT_WEBVIEW_H__


#include <Awesomium/WebCore.h>
#include <Awesomium/STLHelpers.h>
#include <Awesomium/DataSource.h>
#include <Awesomium/DataPak.h>

#include "moTypes.h"
#include "moPlugin.h"

#include "awesomium/common/method_dispatcher.h"
#include "awesomium/common/gl_texture_surface.h"

using namespace Awesomium;

enum moWebviewParamIndex {
  WEBVIEW_INLET,
	WEBVIEW_OUTLET,
	WEBVIEW_SCRIPT,
	WEBVIEW_ALPHA,
	WEBVIEW_COLOR,
	WEBVIEW_SYNC,
	WEBVIEW_PHASE,
	WEBVIEW_URL,
	WEBVIEW_BLENDING,
	WEBVIEW_WIDTH,
	WEBVIEW_HEIGHT,
	WEBVIEW_TRANSLATEX,
	WEBVIEW_TRANSLATEY,
	WEBVIEW_TRANSLATEZ,
  WEBVIEW_SCALEX,
	WEBVIEW_SCALEY,
	WEBVIEW_SCALEZ,
	WEBVIEW_ROTATEX,
	WEBVIEW_ROTATEY,
	WEBVIEW_ROTATEZ,
	WEBVIEW_EYEX,
	WEBVIEW_EYEY,
	WEBVIEW_EYEZ,
	WEBVIEW_VIEWX,
	WEBVIEW_VIEWY,
	WEBVIEW_VIEWZ,
	WEBVIEW_LIGHTX,
	WEBVIEW_LIGHTY,
	WEBVIEW_LIGHTZ,
	WEBVIEW_SHOWCURSOR
};

/*
const char* html_str = "<h1>Hello World</h1>";

class MyDataSource : public DataSource {
 public:
  MyDataSource() { }
  virtual ~MyDataSource { }

  virtual void OnRequest(int request_id,
                         const WebString& path) {
    if (path == WSLit("index.html"))
      SendResponse(request_id,
                   strlen(html_str),
                   html_str,
                   WSLit("text/html"));
  }
};
*/


class moEffectWebview : public moEffect,
  public WebViewListener::View,
  public WebViewListener::Load,
  public WebViewListener::Process
{
    public:


        moEffectWebview();
        virtual ~moEffectWebview();

        MOboolean Init();
        void Draw(moTempo*, moEffectState* parentstate = NULL);
        MOboolean Finish();

        void Interaction( moIODeviceManager * );
        moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );
        void Update( moEventList *Events );

        void Load(moText url);
        const GLTextureSurface* surface() {

                  if (!m_pWebView) return 0;

                  const Surface* surface = m_pWebView->surface();
                  if (surface)
                    return static_cast<const GLTextureSurface*>(surface);

                  return 0;
        }

/*VIEW SPECIFIC*/

virtual void OnChangeTitle(Awesomium::WebView* caller,
                             const Awesomium::WebString& title);

  virtual void OnChangeAddressBar(Awesomium::WebView* caller,
                                  const Awesomium::WebURL& url);

  virtual void OnChangeTooltip(Awesomium::WebView* caller,
                               const Awesomium::WebString& tooltip);

  virtual void OnChangeTargetURL(Awesomium::WebView* caller,
                                 const Awesomium::WebURL& url);

  virtual void OnChangeCursor(Awesomium::WebView* caller,
                              Awesomium::Cursor cursor);

  virtual void OnChangeFocus(Awesomium::WebView* caller,
                                Awesomium::FocusedElementType focus_type);

  virtual void OnAddConsoleMessage(Awesomium::WebView* caller,
                                   const Awesomium::WebString& message,
                                   int line_number,
                                   const Awesomium::WebString& source);

  virtual void OnShowCreatedWebView(Awesomium::WebView* caller,
                                    Awesomium::WebView* new_view,
                                    const Awesomium::WebURL& opener_url,
                                    const Awesomium::WebURL& target_url,
                                    const Awesomium::Rect& initial_pos,
                                    bool is_popup);

  virtual void OnBeginLoadingFrame(Awesomium::WebView* caller,
                                   int64 frame_id,
                                   bool is_main_frame,
                                   const Awesomium::WebURL& url,
                                   bool is_error_page);

  virtual void OnFailLoadingFrame(Awesomium::WebView* caller,
                                  int64 frame_id,
                                  bool is_main_frame,
                                  const Awesomium::WebURL& url,
                                  int error_code,
                                  const Awesomium::WebString& error_description);

  virtual void OnFinishLoadingFrame(Awesomium::WebView* caller,
                                    int64 frame_id,
                                    bool is_main_frame,
                                    const Awesomium::WebURL& url);

  virtual void OnDocumentReady(Awesomium::WebView* caller,
                                    const Awesomium::WebURL& url);

  virtual void OnUnresponsive(Awesomium::WebView* caller);

  virtual void OnResponsive(Awesomium::WebView* caller);

  virtual void OnCrashed(Awesomium::WebView* caller,
                         Awesomium::TerminationStatus status);

  virtual void BindMethods(WebView* web_view);

  virtual void OnJSMoldeoAction(WebView* caller,
                  const JSArray& args);

   private:

        moTexture*    m_pMoldeoSurface;
        int           m_idsurface;
        int           m_glidsurface;
        moText        m_Url;

        WebCore*      m_pWebCore;
        WebView*      m_pWebView;
        MethodDispatcher method_dispatcher_;

        double           m_CursorX;
        double           m_CursorY;
        moEventList     m_JSEvents;

        void setUpLighting();
};

class moEffectWebviewFactory : public moEffectFactory
{
public:
    moEffectWebviewFactory() {}
    virtual ~moEffectWebviewFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
