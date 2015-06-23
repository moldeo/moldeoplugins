/*******************************************************************************

                              moNetOSCIn.h

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

#ifndef __MO_NET_OSC_OUT_H__
#define __MO_NET_OSC_OUT_H__

#include "moConfig.h"
#include "moActions.h"
#include "moDeviceCode.h"
#include "moEventList.h"
#include "moIODeviceManager.h"
#include "moTypes.h"

#include "OscReceivedElements.h"
#include "OscPacketListener.h"
#include "OscOutboundPacketStream.h"
#include "UdpSocket.h"

#include "moConnectors.h"

enum moNetOSCInParamIndex {
    NETOSCIN_INLET,
    NETOSCIN_OUTLET,
    NETOSCIN_SCRIPT,
    NETOSCIN_HOSTS,
    NETOSCIN_PORT,
    NETOSCIN_RECEIVEEVENTS,
    NETOSCIN_DEBUG,
    NETOSCIN_PROCESSMOLDEOAPI
};


//el listener tenga que funcionar dentro de un Thread...para que el run sea asincronico
//y ponerle un flag para que pueda ser bloqueado por la funcion de Update...

class moOscPacketListener : public osc::OscPacketListener, public moThread, public moAbstract {

	public:

        moOscPacketListener();

        virtual int ThreadUserFunction();

		void Set( UdpListeningReceiveSocket* pudprcv );

        void Init( moOutlets* pOutlets );
        MOint GetOutletIndex( moOutlets* pOutlets, moText p_connector_name ) const;
        int Update( moOutlets* pOutlets,
                   bool p_debug_is_on,
                   moEventList* pEvents,
                   int p_ProcessMoldeoApi = false,
                   int p_MoldeoId = -1 );

        moDataMessages        Messages;
        UdpListeningReceiveSocket* m_pUdpRcv;

    protected:
        moLock  m_Semaphore;

         moOutlet*   pOutBeatFreq;
         moOutlet*   pOutBeatValue;

         moOutlet*   pOutBeatHighFreq;
         moOutlet*   pOutBeatHighValue;

         moOutlet*   pOutBeatLowFreq;
         moOutlet*   pOutBeatLowValue;

         moOutlet*   pOutBeatMediumFreq;
         moOutlet*   pOutBeatMediumValue;

        moOutlet*   pOutEvents;
        moOutlet*   pOutTracker;
        moOutlet*   pOutTuio;

        moOutlet*   pOutData;
        moOutlet*   pOutDataMessages;

        moOutlet*   pOutNote;
        moOutlet*   pOutNoteVel;

        moOutlet*   pOutAndOsc;

        moOutlet*   pOutPositionX;
        moOutlet*   pOutPositionY;
        moOutlet*   pOutPositionZ;

        moOutlet*   pOutAccelerationX;
        moOutlet*   pOutAccelerationY;
        moOutlet*   pOutAccelerationZ;

        moOutlet*   pOutOrientationX; //pitch
        moOutlet*   pOutOrientationY; //roll
        moOutlet*   pOutOrientationZ; //azimuth

        bool debug_is_on;

    virtual void ProcessMessage( const osc::ReceivedMessage& m,
				const IpEndpointName& remoteEndpoint );
};

moDeclareDynamicArray( moOscPacketListener*, moOscPacketListeners )

// moNetOSCIn class **************************************************

class moNetOSCIn : public moIODevice
{
public:
    moNetOSCIn();
    virtual ~moNetOSCIn();

    virtual void Update(moEventList*);
    MOboolean Init();
    MOswitch GetStatus(MOdevcode);
    MOswitch SetStatus( MOdevcode,MOswitch);
    MOint GetValue(MOdevcode);
    MOdevcode GetCode( moText);
    MOboolean Finish();
    moConfigDefinition *GetDefinition( moConfigDefinition *p_configdefinition );

    void UpdateParameters();

private:

    moOscPacketListeners    m_OscPacketListeners;

    moOutlet*   m_pEvents;

    // Parameters.
    moTextArray host_name;
    moIntArray host_port;
    bool debug_is_on;

    MOint   gen_port;

	float sendInterval;
	int maxEventNum;
	float minReconnecTime;

    void SendEvent(int i);

    int m_ReceiveEvents;
    int m_ProcessMoldeoApi;
};

class moNetOSCInFactory : public moIODeviceFactory {

public:
    moNetOSCInFactory() {}
    virtual ~moNetOSCInFactory() {}
    moIODevice* Create();
    void Destroy(moIODevice* fx);
};

extern "C"
{
MO_PLG_API moIODeviceFactory* CreateIODeviceFactory();
MO_PLG_API void DestroyIODeviceFactory();
}


#endif
