/*******************************************************************************

                              moNetOSCOut.h

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

#include "moTypes.h"
#include "moActions.h"

#ifndef __MO_NET_OSC_OUT_H__
#define __MO_NET_OSC_OUT_H__

#include "moConfig.h"
#include "moDeviceCode.h"
#include "moEventList.h"
#include "moIODeviceManager.h"
#include "moFilterManager.h"

#include "OscOutboundPacketStream.h"
#include "UdpSocket.h"

//template class LIBMOLDEO_API moDynamicArray<UdpTransmitSocket*>;
//typedef moDynamicArray<UdpTransmitSocket*> moUdpTransmitSocketArray;
moDeclareDynamicArray( UdpTransmitSocket*, moUdpTransmitSocketArray )

enum moNetOSCOutParamIndex {
    NETOSCOUT_INLET,
    NETOSCOUT_OUTLET,
    NETOSCOUT_SCRIPT,
    NETOSCOUT_HOSTS,
    NETOSCOUT_PORT,
    NETOSCOUT_DEVICES,
    NETOSCOUT_LATENCY,
    NETOSCOUT_MAXEVENTS,
    NETOSCOUT_SENDEVENTS,
    NETOSCOUT_DELETEEVENTS,
    NETOSCOUT_SENDMOLDEOAPI
};


// moNetOSCOut class **************************************************

class moNetOSCOut : public moIODevice
{
public:
    moNetOSCOut();
    virtual ~moNetOSCOut();

    void Update(moEventList*);
    MOboolean Init();
    MOswitch GetStatus(MOdevcode);
    MOswitch SetStatus( MOdevcode,MOswitch);
    MOint GetValue(MOdevcode);
    MOdevcode GetCode( moText);
    MOboolean Finish();

    virtual moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

    void UpdateParameters();

private:
	moUdpTransmitSocketArray transmitSockets;
	osc::OutboundPacketStream* packetStream;
	char* packetBuffer;
	moEventPacketArray eventPacket;
	int OUTPUT_BUFFER_SIZE;

  int m_sendMoldeoApi;
	int m_SendEvents;
    int m_Port;

    // Parameters.
    moTextArray host_name;
    moIntArray host_port;

    bool recog_devices[MO_IODEVICE_TABLET + 1];
    bool delete_events;

	float sendInterval;
	int maxEventNum;
	float minReconnecTime;

    void SendEvent(int i);
    void SendDataMessage( int i, moDataMessage &datamessage );

};

class moNetOscOutFactory : public moIODeviceFactory {

public:
    moNetOscOutFactory() {}
    virtual ~moNetOscOutFactory() {}
    moIODevice* Create();
    void Destroy(moIODevice* fx);
};

extern "C"
{
MO_PLG_API moIODeviceFactory* CreateIODeviceFactory();
MO_PLG_API void DestroyIODeviceFactory();
}


#endif