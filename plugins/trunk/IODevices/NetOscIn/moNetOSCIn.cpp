/*******************************************************************************

                              moNetOSCIn.cpp

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

#include "moNetOSCIn.h"

#include "moArray.h"

moDefineDynamicArray( moOscPacketListeners )

//========================
//  Factory
//========================

moNetOSCInFactory *m_NetOSCInFactory = NULL;

MO_PLG_API moIODeviceFactory* CreateIODeviceFactory(){
	if (m_NetOSCInFactory==NULL)
		m_NetOSCInFactory = new moNetOSCInFactory();
	return (moIODeviceFactory*) m_NetOSCInFactory;
}

MO_PLG_API void DestroyIODeviceFactory(){
	delete m_NetOSCInFactory;
	m_NetOSCInFactory = NULL;
}

moIODevice*  moNetOSCInFactory::Create() {
	return new moNetOSCIn();
}

void moNetOSCInFactory::Destroy(moIODevice* fx) {
	delete fx;
}

//  Listener class *****************************************************

 moOscPacketListener::moOscPacketListener() {
    m_pUdpRcv = NULL;
    pEvents = NULL;
    pTracker = NULL;
    debug_is_on = false;
}

int moOscPacketListener::ThreadUserFunction() {
    if (m_pUdpRcv) {
        moAbstract::MODebug2->Message(moText("Running listener..."));
        m_pUdpRcv->Run();
    }
}

void moOscPacketListener::Set( UdpListeningReceiveSocket* pudprcv ) {
    moAbstract::MODebug2->Message(moText("Set listener"));
    m_pUdpRcv = pudprcv;
}

void
moOscPacketListener::Init( moOutlets* pOutlets ) {
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

void
moOscPacketListener::Update( moOutlets* pOutlets, bool _debug_is_on ) {
    //block message
    m_Semaphore.Lock();

    debug_is_on = _debug_is_on;

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


void
moOscPacketListener::ProcessMessage( const osc::ReceivedMessage& m,
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
                    //el BLOB deberá ser pasado como buffer...
                    // habria que ver como reconocer si es Imagen o si es simplemente data
                    // eso hay que tratarlo, ya sea con el FreeImage: que tratará de reconocer en memoria el tag
                    // si falla podríamos ir pensando en una variante para esto...
                    MOpointer   pointer;
                    MOulong size;
                    moDataType dtype = MO_DATA_POINTER;
                    //dtype = MO_DATA_IMAGESAMPLE;
                    //dtype = MO_DATA_SOUNDSAMPLE;
                    rec.AsBlob( (const void*&)pointer, size );
                    data = moData( pointer, size, dtype );
                    message.Add( data );
                }
                if (debug_is_on) moAbstract::MODebug2->Push( data.TypeToText()+ moText(": ") + data.ToText() );
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

// moNetOSCIn class **************************************************

moNetOSCIn::moNetOSCIn()
{
    SetName("netoscin");
    m_pEvents = NULL;
}

moNetOSCIn::~moNetOSCIn()
{
    Finish();
}

MOboolean moNetOSCIn::Init()
{
    int dev;
    MOuint i, n, n_dev, n_hosts;
    moText conf, dev_name;

    //==========================
    // CORRESPONDE A UN PREINIT
    //==========================

    // Loading config file.
    if (moMoldeoObject::Init()) {
      moMoldeoObject::CreateConnectors();
    } else return false;

    //==========================
    // INIT
    //==========================

	moDefineParamIndex( NETOSCIN_INLET, moText("inlet") );
	moDefineParamIndex( NETOSCIN_OUTLET, moText("outlet") );
	moDefineParamIndex( NETOSCIN_SCRIPT , moText("script") );
	moDefineParamIndex( NETOSCIN_HOSTS , moText("hosts") );
	moDefineParamIndex( NETOSCIN_PORT , moText("port") );
	moDefineParamIndex( NETOSCIN_RECEIVEEVENTS, moText("receive_events") );
	moDefineParamIndex( NETOSCIN_DEBUG, moText("debug") );

    bool events_present;
    bool trackersystem_present;

    events_present = false;
    trackersystem_present = false;

    for( int i=0; i<m_Config.GetParam( moR(NETOSCIN_OUTLET) ).GetValuesCount(); i++) {
        moValue val( m_Config.GetParam( moR(NETOSCIN_OUTLET) ).GetValue(i));
        if ( val.GetSubValue().Text()==moText("EVENTS")) {
            events_present = true;
        }
        if ( val.GetSubValue().Text()==moText("TRACKERSYSTEM")) {
            trackersystem_present = true;
        }
    }
    if (!events_present) {
        m_Config.GetParam( moR(NETOSCIN_OUTLET) ).AddValue( moValue( "EVENTS", "TXT", "DATA", "TXT" ).Ref() );
    }
    if (!trackersystem_present) {
        m_Config.GetParam( moR(NETOSCIN_OUTLET) ).AddValue( moValue( "EVENTS", "TXT", "DATA", "TXT" ).Ref() );
    }

    moMoldeoObject::Init();


    for(int a=0; a <GetOutlets()->Count(); a++) {
        if ( m_Outlets[a]->GetConnectorLabelName() == moText("EVENTS") ) {
            m_pEvents = m_Outlets[a];
        }
    }

    // Reading hosts names and ports.
    n = m_Config.GetParamIndex("hosts");
	n_hosts = m_Config.GetValuesCount(n);
	host_name.Init(n_hosts, moText(""));
    host_port.Init(n_hosts, 0);
    for(i = 0; i < n_hosts; i++)
    {
		host_name.Set(i, m_Config.GetParam(n).GetValue(i).GetSubValue(0).Text());
		int nport;
		nport = m_Config.GetParam(n).GetValue(i).GetSubValue(1).Int();

		MODebug2->Push(moText(" NETOSCIN hosts: HOST NAME:")
                                            + (moText)host_name[i]
                                            + moText(" PORT:")
                                            + IntToStr(nport)
                 );

		if (nport==0) {
		    //nport = m_Config.Int( moText("port") );
		    nport = m_Config.Int( moR(NETOSCIN_PORT) );
            MODebug2->Push(moText(" NETOSCIN port 0: trying default port parameter")
                                            + moText(" PORT:")
                                            + IntToStr(nport)
                 );

		}
		host_port.Set(i, nport);
	}

	for(i = 0; i < n_hosts; i++)
	{
		moOscPacketListener*    pListener = NULL;

		pListener = new moOscPacketListener();
        if (pListener) {
            /*
            if ( host_name[i] == moText("all") ) {
                if (host_port[i]>0)
                    pListener->Set( new UdpListeningReceiveSocket( IpEndpointName( IpEndpointName::ANY_ADDRESS,
                                                                host_port[i] ),
                                                               pListener ) );
                else
                    pListener->Set( new UdpListeningReceiveSocket( IpEndpointName( IpEndpointName::ANY_ADDRESS,
                                                                IpEndpointName::ANY_PORT ),
                                                               pListener ) );
            } else if ( host_name[i] != moText("") ) {
                moTextArray ipNumbers;
                unsigned long ipaddress = 0;
                unsigned long i1=0, i2=0, i3=0, i4=0;
                ipNumbers = host_name[i].Explode(".");
                if (ipNumbers.Count()==4) {
                    i1 = atoi(ipNumbers[0]);
                    i2 = atoi(ipNumbers[1]);
                    i3 = atoi(ipNumbers[2]);
                    i4 = atoi(ipNumbers[3]);
                    ipaddress = (i1 << 24) & (i2<<16) & (i3<<8) & i4;
                } else {
                    ipaddress = IpEndpointName::ANY_ADDRESS;
                }
                if (host_port[i]>0)
                    pListener->Set( new UdpListeningReceiveSocket( IpEndpointName( ipaddress,
                                                                host_port[i] ),
                                                               pListener ) );
                else
                    pListener->Set( new UdpListeningReceiveSocket( IpEndpointName( ipaddress,
                                                                IpEndpointName::ANY_PORT ),
                                                               pListener ) );
            } else {
                pListener->Set( new UdpListeningReceiveSocket( IpEndpointName( IpEndpointName::ANY_ADDRESS,
                                                                IpEndpointName::ANY_PORT ),
                                                               pListener ) );
            }
*/
            UdpListeningReceiveSocket   *socket = NULL;

            try {
                socket = new UdpListeningReceiveSocket(
                IpEndpointName( IpEndpointName::ANY_ADDRESS, host_port[i] ),
                pListener );
            } catch (std::exception &e) {
                MODebug2->Error(moText("could not bind to UDP port "));
                socket = NULL;
            }

            if (socket!=NULL) {
                if (!socket->IsBound()) {
                    delete socket;
                    socket = NULL;
                    MODebug2->Error( moText("NETOSCIN UDP socket not connected:") +
                                    + (moText)host_name[i]
                                    + moText(" PORT:")
                                    + IntToStr(host_port[i])
                                    );
                } else {
                   MODebug2->Message( moText("NetOSCIn listening to OSC messages on UDP port "));
                   pListener->Set(socket);
                }
            }
            if (socket) {
                m_OscPacketListeners.Add( pListener );
                if (pListener->CreateThread()) {
                    MODebug2->Message( moText(" NETOSCIN OK: HOST NAME:")
                                            + (moText)host_name[i]
                                            + moText(" PORT:")
                                            + IntToStr(host_port[i])

                                            );
                } else {
                    MODebug2->Error( moText("NETOSCIN:") +
                                    + (moText)host_name[i]
                                    + moText(" PORT:")
                                    + IntToStr(host_port[i])
                                    );
                }
            }
       }
	}


	return true;
}

void
moNetOSCIn::UpdateParameters() {

    m_ReceiveEvents = m_Config.Int( moR(NETOSCIN_RECEIVEEVENTS) );
    debug_is_on = ( 0 != m_Config.Int( moR(NETOSCIN_DEBUG) ) );

}

moConfigDefinition *
moNetOSCIn::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moMoldeoObject::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("hosts"), MO_PARAM_TEXT, NETOSCIN_HOSTS, moValue("127.0.0.1","TXT") );
	p_configdefinition->Add( moText("port"), MO_PARAM_NUMERIC, NETOSCIN_PORT, moValue("7400","INT") );
	p_configdefinition->Add( moText("receive_events"), MO_PARAM_NUMERIC, NETOSCIN_RECEIVEEVENTS, moValue("0","INT") );
	p_configdefinition->Add( moText("debug"), MO_PARAM_NUMERIC, NETOSCIN_DEBUG, moValue("0","INT") );

	return p_configdefinition;
}

MOswitch moNetOSCIn::SetStatus(MOdevcode codisp, MOswitch state)
{
    return true;
}

MOswitch moNetOSCIn::GetStatus(MOdevcode codisp)
{
    return(-1);
}

MOint moNetOSCIn::GetValue(MOdevcode codisp)
{
    return(-1);
}

MOdevcode moNetOSCIn::GetCode(moText strcod)
{
    return(-1);
}

void moNetOSCIn::Update(moEventList *Eventos)
{

    MOuint i;


    bool res;
    moEvent *tmp;
    moEvent *actual;

    //MODebug2->Push( moText("NETOSCIN: Update"));

    UpdateParameters();

    //clean every outlet!!! important
    for(i=0; i<GetOutlets()->Count();i++) {
        moOutlet* pOutlet = GetOutlets()->Get(i);
        if (pOutlet) {
            pOutlet->GetMessages().Empty();
            pOutlet->Update(false);
        }
    }

	for(i=0; i<m_OscPacketListeners.Count(); i++ ) {

        //pass outlets thru listeners to populate the outlets
        moOscPacketListener* pListener = NULL;
        pListener = m_OscPacketListeners[i];

        if (pListener) {
            //MODebug2->Push( moText("Updating outlets "));
            pListener->Update( GetOutlets(), debug_is_on );
        }

	}

    if (m_ReceiveEvents) {
        if (m_pEvents) {
            //MODebug2->Push( moText("Checking events "));
            for(int i = 0; i<m_pEvents->GetMessages().Count();  i++) {
                moDataMessage& mess(m_pEvents->GetMessages().Get(i) );

                moData data;
                ///atencion el primer dato es "EVENT"
                int did = mess.Get(1).Int();
                int cid = mess.Get(2).Int();
                int val0 = mess.Get(3).Int();
                int val1 = mess.Get(4).Int();
                int val2 = mess.Get(5).Int();
                int val3 = mess.Get(6).Int();


                MODebug2->Push( moText("Receiving events: ")
                                + moText(" did:")
                                + IntToStr(did)
                                + moText(" cid:")
                                + IntToStr(cid)
                                + moText(" val0:")
                                + IntToStr(val0)
                                + moText(" val1:")
                                + IntToStr(val1)
                                + moText(" val2:")
                                + IntToStr(val2)
                                + moText(" val3:")
                                + IntToStr(val3)
                );

                Eventos->Add( did, cid, val0, val1, val2, val3 );

            }
        }
    }

	moMoldeoObject::Update(Eventos);

    for(i=0; i<GetOutlets()->Count();i++) {
        moOutlet* pOutlet = GetOutlets()->Get(i);
        if (pOutlet) {
            if (pOutlet->Updated()) {
                /*
                MODebug2->Push(moText("NetOscIn messages: ")+IntToStr(pOutlet->GetMessages().Count() )
                             +moText(" [1s int]: ")+IntToStr(pOutlet->GetMessages().Get(0).Get(0).Int())
                             +moText(" [2d int]: ")+IntToStr(pOutlet->GetMessages().Get(0).Get(1).Int())
                             +moText(" [3rd int]: ")+IntToStr(pOutlet->GetMessages().Get(0).Get(2).Int())
                             );
                             */
            }
        }
    }

}

MOboolean moNetOSCIn::Finish()
{

	return true;
}

void moNetOSCIn::SendEvent(int i)
{

}
