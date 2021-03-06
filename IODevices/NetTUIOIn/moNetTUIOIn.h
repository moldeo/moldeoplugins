/*******************************************************************************

                              moNetTUIOIn.h

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

#include "moTypes.h"

#ifndef __MO_NET_TUIO_IN_H__
#define __MO_NET_TUIO_IN_H__

#include "moConfig.h"
#include "moDeviceCode.h"
#include "moEventList.h"
#include "moIODeviceManager.h"
#include "moTypes.h"
#include "moFilterManager.h"

#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"

#include "tuio/TuioClient.h"

enum tuioDef {
  MO_TOUCH_START,
  MO_TOUCH_END,
  MO_TOUCH_MOVE
};

#include "moConnectors.h"

enum moNetTUIOInParamIndex {
	NETTUIOIN_INLET,
	NETTUIOIN_OUTLET,
	NETTUIOIN_SCRIPT,
  NETTUIOIN_PORT,
  NETTUIOIN_HOST,
  NETTUIOIN_VERBOSE
};

using namespace TUIO;

class moNetTUIOListener : public TuioListener, public moAbstract {

public:

	moNetTUIOListener( moOutlets* p_Outlets, int port );
	virtual bool Init( moOutlets* p_Outlets, int port );

	virtual ~moNetTUIOListener() {
		tuioClient->disconnect();
		delete tuioClient;
		tuioClient = NULL;
	}

	virtual void addTuioObject(TuioObject *tobj);
	virtual void updateTuioObject(TuioObject *tobj);
	virtual void removeTuioObject(TuioObject *tobj);

	virtual void addTuioCursor(TuioCursor *tcur);
	virtual void updateTuioCursor(TuioCursor *tcur);
	virtual void removeTuioCursor(TuioCursor *tcur);

	virtual void refresh(TuioTime frameTime);

	TuioClient *tuioClient;
  moOutlet*   m_pTrackerNetTUIOIn;

  void drawObjects();
  void SetVerbose ( bool p_verbose = true );
  void updateOutlets();
  void updateEvents( moEventList* Events );

private:

  moOutlets*  m_pOutlets;

  moOutlet*   m_pOutletCursors; //vectors

  moOutlet*   m_pOutletCursor1;
  moOutlet*   m_pOutletCursor1TouchStart;
  moOutlet*   m_pOutletCursor1TouchOn;
  moOutlet*   m_pOutletCursor1TouchEnd;
  moOutlet*   m_pOutletCursor1TouchOff;
  moOutlet*   m_pOutletCursor1TouchMove;
  moOutlet*   m_pOutletCursor1X;
  moOutlet*   m_pOutletCursor1Y;
  moOutlet*   m_pOutletCursor1VX;
  moOutlet*   m_pOutletCursor1VY;

  moOutlet*   m_pOutletCursor2;
  moOutlet*   m_pOutletCursor2TouchStart;
  moOutlet*   m_pOutletCursor2TouchOn;
  moOutlet*   m_pOutletCursor2TouchEnd;
  moOutlet*   m_pOutletCursor2TouchOff;
  moOutlet*   m_pOutletCursor2TouchMove;
  moOutlet*   m_pOutletCursor2X;
  moOutlet*   m_pOutletCursor2Y;
  moOutlet*   m_pOutletCursor2VX;
  moOutlet*   m_pOutletCursor2VY;

  moOutlet*   m_pOutletCursor3;
  moOutlet*   m_pOutletCursor3TouchStart;
  moOutlet*   m_pOutletCursor3TouchOn;
  moOutlet*   m_pOutletCursor3TouchEnd;
  moOutlet*   m_pOutletCursor3TouchOff;
  moOutlet*   m_pOutletCursor3TouchMove;
  moOutlet*   m_pOutletCursor3X;
  moOutlet*   m_pOutletCursor3Y;
  moOutlet*   m_pOutletCursor3VX;
  moOutlet*   m_pOutletCursor3VY;

  moOutlet*   m_pOutletCursor4;
  moOutlet*   m_pOutletCursor4TouchStart;
  moOutlet*   m_pOutletCursor4TouchOn;
  moOutlet*   m_pOutletCursor4TouchEnd;
  moOutlet*   m_pOutletCursor4TouchOff;
  moOutlet*   m_pOutletCursor4TouchMove;
  moOutlet*   m_pOutletCursor4X;
  moOutlet*   m_pOutletCursor4Y;
  moOutlet*   m_pOutletCursor4VX;
  moOutlet*   m_pOutletCursor4VY;

  bool verbose;
	bool fullscreen, running;

	int width, height;
	int screen_width, screen_height;
	int window_width, window_height;
	moEventList m_CachedEvents;
};

// moNetTUIOIn class **************************************************

class moNetTUIOIn : public moIODevice
{
public:
    moNetTUIOIn();
    virtual ~moNetTUIOIn();

    virtual void Update(moEventList*);
    MOboolean Init();
    MOswitch GetStatus(MOdevcode);
    MOswitch SetStatus( MOdevcode,MOswitch);
    MOint GetValue(MOdevcode);
    MOdevcode GetCode( moText);
    MOboolean Finish();
    moConfigDefinition *GetDefinition( moConfigDefinition *p_configdefinition );

private:

    moNetTUIOListener*   m_pListener;

    // Parameters.
    moTextArray host_name;
    moIntArray host_port;

    moTrackerSystemData*  m_pTrackerSystemData;


	float sendInterval;
	int maxEventNum;
	float minReconnecTime;

    void SendEvent(int i);
};

class moNetTUIOInFactory : public moIODeviceFactory {

public:
    moNetTUIOInFactory() {}
    virtual ~moNetTUIOInFactory() {}
    moIODevice* Create();
    void Destroy(moIODevice* fx);
};

extern "C"
{
MO_PLG_API moIODeviceFactory* CreateIODeviceFactory();
MO_PLG_API void DestroyIODeviceFactory();
}


#endif
