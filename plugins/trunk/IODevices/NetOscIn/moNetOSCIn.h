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
  Andr�s Colubri

*******************************************************************************/

#include "moTypes.h"

#ifndef __MO_NET_OSC_OUT_H__
#define __MO_NET_OSC_OUT_H__

#include "moConfig.h"
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
    NETOSCIN_HOSTS,
	NETOSCIN_PORT,
	NETOSCIN_RECEIVEEVENTS
};


//el listener tenga que funcionar dentro de un Thread...para que el run sea asincronico
//y ponerle un flag para que pueda ser bloqueado por la funcion de Update...

class moOscPacketListener : public osc::OscPacketListener, public moThread, public moAbstract {

	public:

        moOscPacketListener() {
            m_pUdpRcv = NULL;
            pEvents = NULL;
            pTracker = NULL;
        }

        virtual int ThreadUserFunction() {
            if (m_pUdpRcv) {
                moAbstract::MODebug2->Message(moText("Running listener..."));
                m_pUdpRcv->Run();
            }
        }

		void Set( UdpListeningReceiveSocket* pudprcv ) {
		    moAbstract::MODebug2->Message(moText("Set listener"));
		    m_pUdpRcv = pudprcv;
		}

        void Init( moOutlets* pOutlets ) {
            moOutlet* pOutlet = NULL;
            for( int i=0; i<pOutlets->Count(); i++) {
                    pOutlet = pOutlets->Get(i);
                    if (pOutlet) {
                        if (pOutlet->GetConnectorLabelName() == moText("EVENTS")) {
                            pEvents = pOutlet;
                        }
                        if (pOutlet->GetConnectorLabelName() == moText("TRACKERSYSTEM")) {
                            pTracker = pOutlet;
                        }
                    }

            }
        }

        void Update( moOutlets* pOutlets ) {
            //block message
            m_Semaphore.Lock();

            moOutlet* poutlet = NULL;

            if (pEvents==NULL) {
                Init(pOutlets);
            }

            for( int j=0; j<Messages.Count();j++) {

                moDataMessage& message( Messages.Get(j) );
                poutlet = NULL;

                //sumamos a los mensajes....
                ///primer dato debe contener el codigo interno del evento
                moData DataCode = message.Get(0);

                if ( DataCode.Text() == moText("EVENT") ) {
                    poutlet = pEvents;
                    poutlet->GetMessages().Add( message );
                    //MODebug2->Push( moText("receiving event:") +  message.Get(1).ToText() );
                } else if ( DataCode.Text() == moText("TRACKERSYSTEM") ) {
                    poutlet = pTracker;
                    poutlet->GetMessages().Add( message );
                }
                if (poutlet) {
                    poutlet->Update();
                    if (poutlet->GetType()==MO_DATA_MESSAGES)
                        poutlet->GetData()->SetMessages( &poutlet->GetMessages() );
                    if (poutlet->GetType()==MO_DATA_MESSAGE)
                        poutlet->GetData()->SetMessage( &poutlet->GetMessages().Get( poutlet->GetMessages().Count() - 1 ) );
                }

            }
            Messages.Empty();

            m_Semaphore.Unlock();
        }

        moDataMessages        Messages;
        UdpListeningReceiveSocket* m_pUdpRcv;

    protected:
        moLock  m_Semaphore;

        moOutlet*   pEvents;
        moOutlet*   pTracker;
        moOutlet*   pTuio;
        moOutlet*   pData;

    virtual void ProcessMessage( const osc::ReceivedMessage& m,
				const IpEndpointName& remoteEndpoint )
    {
        m_Semaphore.Lock();
        moDataMessage message;
        try{
            //moAbstract::MODebug2->Push(moText("N: ")+IntToStr(m.ArgumentCount()));

            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();

            for(int i = 0; i<m.ArgumentCount(); i++) {

                const osc::ReceivedMessageArgument& rec((*arg));

                moData  data;

                //base.Copy( moData( (int)rec.AsBool() ) );
                if (rec.TypeTag()==osc::TRUE_TYPE_TAG) {
                    data = moData( (int)rec.AsBool() );
                    message.Add( data );

                } else if (rec.TypeTag()==osc::FALSE_TYPE_TAG) {
                    data = moData( (int)rec.AsBool() );
                    message.Add( data );

                } else if (rec.TypeTag()==osc::NIL_TYPE_TAG) {
                    data.Copy( moData( (int)rec.AsInt32() ) );
                    message.Add( data );

                } else if (rec.TypeTag()==osc::INFINITUM_TYPE_TAG) {
                    data.Copy( moData( (int)rec.AsInt32() ) );
                    message.Add( data );

                } else if (rec.TypeTag()==osc::INT32_TYPE_TAG) {
                    data.Copy( moData( (int)rec.AsInt32() ) );
                    message.Add( data );

                } else if (rec.TypeTag()==osc::CHAR_TYPE_TAG) {
                    data.Copy( moData( rec.AsChar() ) );
                    message.Add( data );

                } else if (rec.TypeTag()==osc::RGBA_COLOR_TYPE_TAG) {
                    int rgba = rec.AsRgbaColor();
                    int r = rgba >> 24;
                    int g = (rgba & 0x00FFFFFF) >> 16;
                    int b = (rgba & 0x0000FFFF) >> 8;
                    int a = (rgba & 0x000000FF);
                    data = moData(rgba);
                    message.Add(data);

                } else if (rec.TypeTag()==osc::MIDI_MESSAGE_TYPE_TAG) {//uint32
                    data = moData( (int)rec.AsMidiMessage() );
                    message.Add( data );

                } else if (rec.TypeTag()==osc::INT64_TYPE_TAG) {//uint64
                    data = moData( rec.AsInt64());
                    message.Add( data );

                } else if (rec.TypeTag()==osc::TIME_TAG_TYPE_TAG) {//uint64
                    data = moData( (MOlonglong)rec.AsTimeTag());
                    message.Add( data );

                } else if (rec.TypeTag()==osc::FLOAT_TYPE_TAG) {
                    data = moData( rec.AsFloat() );
                    message.Add( data );
                } else if (rec.TypeTag()==osc::DOUBLE_TYPE_TAG) {
                    data = moData( rec.AsDouble() );
                    message.Add( data );
                } else if (rec.TypeTag()==osc::STRING_TYPE_TAG) {
                    data = moData( moText( rec.AsString()) );
                    message.Add( data );

                } else if (rec.TypeTag()==osc::SYMBOL_TYPE_TAG) {
                    data = moData( moText(rec.AsSymbol()) );
                    message.Add( data );

                } else if (rec.TypeTag()==osc::BLOB_TYPE_TAG) {
                    //el BLOB deber� ser pasado como buffer...
                    // habria que ver como reconocer si es Imagen o si es simplemente data
                    // eso hay que tratarlo, ya sea con el FreeImage: que tratar� de reconocer en memoria el tag
                    // si falla podr�amos ir pensando en una variante para esto...
                    MOpointer   pointer;
                    MOulong size;
                    moDataType dtype = MO_DATA_POINTER;
                    //dtype = MO_DATA_IMAGESAMPLE;
                    //dtype = MO_DATA_SOUNDSAMPLE;
                    rec.AsBlob( (const void*&)pointer, size );
                    data = moData( pointer, size, dtype );
                    message.Add( data );
                }
                //moAbstract::MODebug->Push( data.TypeToText()+ moText(": ") + data.ToText() );
                (arg++);
            }

        }catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: "
                << m.AddressPattern() << ": " << e.what() << "\n";
        }
        Messages.Add(message);
        m_Semaphore.Unlock();
    }
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

	float sendInterval;
	int maxEventNum;
	float minReconnecTime;

    void SendEvent(int i);

    int m_ReceiveEvents;
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
