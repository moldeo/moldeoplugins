/*******************************************************************************

                              moNetOSCOut.cpp

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

#include "moNetOSCOut.h"

#include "moArray.h"

moDefineDynamicArray( moUdpTransmitSocketArray )

//========================
//  Factory
//========================

moNetOscOutFactory *m_NetOscOutFactory = NULL;

MO_PLG_API moIODeviceFactory* CreateIODeviceFactory(){
	if (m_NetOscOutFactory==NULL)
		m_NetOscOutFactory = new moNetOscOutFactory();
	return (moIODeviceFactory*) m_NetOscOutFactory;
}

MO_PLG_API void DestroyIODeviceFactory(){
	delete m_NetOscOutFactory;
	m_NetOscOutFactory = NULL;
}

moIODevice*  moNetOscOutFactory::Create() {
	return new moNetOSCOut();
}

void moNetOscOutFactory::Destroy(moIODevice* fx) {
	delete fx;
}



// moNetOSCOut class **************************************************

moNetOSCOut::moNetOSCOut()
{
	packetBuffer = NULL;
#ifdef OSCPACK
	packetStream = NULL;
#endif
    SetName("netoscout");

    m_Port = 7400;
    m_SendEvents = 0;
    m_SendDevices = moText("");
    for(int dev = 0; dev <= MO_IODEVICE_CONSOLE; dev++) recog_devices[dev] = false;
    map_devices["keyboard"] = MO_IODEVICE_KEYBOARD;
    map_devices["mouse"] = MO_IODEVICE_MOUSE;
    map_devices["midi"] = MO_IODEVICE_MIDI;
    map_devices["mixer"] = MO_IODEVICE_MIXER;
    map_devices["joystick"] = MO_IODEVICE_JOYSTICK;
    map_devices["net_tcp_in"] = MO_IODEVICE_NET_TCP_IN;
    map_devices["net_tcp_out"] = MO_IODEVICE_NET_TCP_OUT;
    map_devices["net_udp_in"] = MO_IODEVICE_NET_UDP_IN;
    map_devices["net_udp_out"] = MO_IODEVICE_NET_UDP_OUT;
    map_devices["live"] = MO_IODEVICE_LIVE;
    map_devices["tracker"] = MO_IODEVICE_TRACKER;
    map_devices["tablet"] = MO_IODEVICE_TABLET;
    map_devices["touch"] = MO_IODEVICE_TOUCH;
    map_devices["console"] = MO_IODEVICE_CONSOLE;
    sendInterval = 100;
    debug_is_on = false;
}

moNetOSCOut::~moNetOSCOut()
{
    Finish();
}

MOboolean moNetOSCOut::Init()
{
    int dev;
    MOuint i, n, n_dev, n_hosts;
    moText conf, dev_name;

    // Loading config file.
    if (moMoldeoObject::Init()) {
      moMoldeoObject::CreateConnectors();
    } else return false;


    moDefineParamIndex( NETOSCOUT_HOSTS, moText("hosts") );
    moDefineParamIndex( NETOSCOUT_PORT, moText("port") );
    moDefineParamIndex( NETOSCOUT_DEVICES, moText("devices") );
    moDefineParamIndex( NETOSCOUT_LATENCY, moText("latency") );
    moDefineParamIndex( NETOSCOUT_MAXEVENTS, moText("max_events") );
    moDefineParamIndex( NETOSCOUT_SENDEVENTS, moText("send_events") );
    moDefineParamIndex( NETOSCOUT_DELETEEVENTS, moText("delete_events") );
    moDefineParamIndex( NETOSCOUT_SENDMOLDEOAPI, moText("send_moldeo_api") );
    moDefineParamIndex( NETOSCOUT_DEBUG, moText("debug") );

    // Reading hosts names and ports.
    n = m_Config.GetParamIndex("hosts");
	n_hosts = m_Config.GetValuesCount(n);

	host_name.Init(n_hosts, moText(""));

    host_port.Init(n_hosts, 0);

    transmitSockets.Init(n_hosts,NULL);

    for(i = 0; i < n_hosts; i++)
    {
		host_name.Set(i, m_Config.GetParam(n).GetValue(i).GetSubValue(0).Text());
		host_port.Set(i, m_Config.GetParam(n).GetValue(i).GetSubValue(1).Int());
	}

    i = 0;

    UpdateParameters();

	for(i = 0; i < n_hosts; i++)
	{
#ifdef OSCPACK
	    unsigned long ii = GetHostByName(host_name[i]);
        char buffer[100];
        snprintf(buffer, 100, "%lu", ii); // Memory-safe version of sprintf.
        moText str = buffer;
	    //MODebug2->Message( moText("moNetOscOut:: host: ") + moText(host_name[i]) + moText(" ip int:") + (moText)str );
	    IpEndpointName ipendpointname( ii, host_port[i] ) ;
		transmitSockets[i] = new UdpTransmitSocket( ipendpointname );
#else
    //transmitSockets[i] = new lo::Address( host_name[i], IntToStr(host_port[i]) );
    transmitSockets[i] = lo_address_new( host_name[i], IntToStr(host_port[i]) );
#endif

		if (transmitSockets[i]) {
        MODebug2->Message(moText("NetOSCOut UdptransmitSocket Created at")+host_name[i]+":"+ IntToStr(host_port[i]) );
    }


	    //eventPacket[i] = new moEventPacket(sendInterval, maxEventNum);
	}

    //OUTPUT_BUFFER_SIZE = 1024; // 10 = maximum length of a 32 bit int in decimal rep.
    OUTPUT_BUFFER_SIZE = 60000;
    packetBuffer = new char[OUTPUT_BUFFER_SIZE];
    #ifdef OSCPACK
    packetStream = new osc::OutboundPacketStream( packetBuffer, OUTPUT_BUFFER_SIZE );
    #endif
	return true;
}

void
moNetOSCOut::UpdateParameters() {

    sendInterval = m_Config.Double( moR( NETOSCOUT_LATENCY ) );
    m_Port = m_Config.Int( moR( NETOSCOUT_PORT ) );
    maxEventNum = m_Config.Int( moR( NETOSCOUT_MAXEVENTS ) );
    m_SendEvents  = m_Config.Int( moR( NETOSCOUT_SENDEVENTS ) );
    moText send_devices = m_Config.Text( moR ( NETOSCOUT_DEVICES) );
    if ( send_devices.Length()>0 && m_SendDevices!=send_devices) {
      m_SendDevices = send_devices;
      m_SendDevicesArray = m_SendDevices.Explode( moText(",") );
      for(int dev = 0; dev <= MO_IODEVICE_CONSOLE; dev++) recog_devices[dev] = false;
      for( int d=0; d<m_SendDevicesArray.Count(); d++) {
          string devname = (char*) m_SendDevicesArray[d];
          int devid = -1;
          try {
            devid = map_devices[ devname ];
            if (devid>-1) recog_devices[ devid ] = true;
            MODebug2->Message(m_SendDevicesArray[d]);
          } catch(...) {
            MODebug2->Error("Bad device name: "+m_SendDevicesArray[d]);
          }
      }
    }
    delete_events = m_Config.Int( moR( NETOSCOUT_DELETEEVENTS ) );
    m_sendMoldeoApi = m_Config.Int( moR( NETOSCOUT_SENDMOLDEOAPI )  );
    debug_is_on = ( 0 != m_Config.Int( moR(NETOSCOUT_DEBUG) ) );
}


MOswitch moNetOSCOut::SetStatus(MOdevcode codisp, MOswitch state)
{
    return true;
}

MOswitch moNetOSCOut::GetStatus(MOdevcode codisp)
{
    return(-1);
}

MOint moNetOSCOut::GetValue(MOdevcode codisp)
{
    return(-1);
}

MOdevcode moNetOSCOut::GetCode(moText strcod)
{
    return(-1);
}

moConfigDefinition *
moNetOSCOut::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moMoldeoObject::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("hosts"), MO_PARAM_TEXT, NETOSCOUT_HOSTS, moValue( "127.0.0.1", "TXT", "3335", "INT") );
	p_configdefinition->Add( moText("port"), MO_PARAM_NUMERIC, NETOSCOUT_PORT, moValue( "3335", "INT") );

    p_configdefinition->Add( moText("devices"), MO_PARAM_TEXT, NETOSCOUT_DEVICES, moValue( "keyboard", "TXT") );
	p_configdefinition->Add( moText("latency"), MO_PARAM_NUMERIC, NETOSCOUT_LATENCY, moValue( "30", "FLOAT") );
	p_configdefinition->Add( moText("max_events"), MO_PARAM_NUMERIC, NETOSCOUT_MAXEVENTS, moValue( "10", "INT") );

	p_configdefinition->Add( moText("send_events"), MO_PARAM_NUMERIC, NETOSCOUT_SENDEVENTS, moValue( "0", "INT") );
	p_configdefinition->Add( moText("delete_events"), MO_PARAM_NUMERIC, NETOSCOUT_DELETEEVENTS, moValue( "0", "INT") );

	p_configdefinition->Add( moText("send_moldeo_api"), MO_PARAM_NUMERIC, NETOSCOUT_SENDMOLDEOAPI, moValue("1","INT") );
	p_configdefinition->Add( moText("debug"), MO_PARAM_NUMERIC, NETOSCOUT_DEBUG, moValue("0","INT") );

	return p_configdefinition;
}

void moNetOSCOut::Update(moEventList *Eventos)
{
    MOuint i;

    bool res;
    moEvent *tmp;
    moEvent *actual;

    UpdateParameters();
    // Sending over the network the events that correspond to recognized devices.
    //Eventos->Add( MO_IODEVICE_TRACKER, moGetTicks(), 112, 113, 114, 115 );


    if (m_SendEvents) {
        actual = Eventos->First;
        while(actual != NULL)
        {
            if ( actual->deviceid<=MO_IODEVICE_CONSOLE &&
                 recog_devices[actual->deviceid] ) {

   			    moDataMessage event_data_message;
			    moData pData;

                pData.SetText( moText("EVENT") );
                event_data_message.Add(pData);

                pData.SetInt( actual->deviceid );
                event_data_message.Add(pData);

                pData.SetInt( actual->devicecode );
                event_data_message.Add(pData);

                pData.SetInt( actual->reservedvalue0 );
                event_data_message.Add(pData);

                pData.SetInt( actual->reservedvalue1 );
                event_data_message.Add(pData);

                pData.SetInt( actual->reservedvalue2 );
                event_data_message.Add(pData);

                pData.SetInt( actual->reservedvalue3 );
                event_data_message.Add(pData);

                for (i = 0; i < (int)host_name.Count(); i++)
                    {
                        /*
                        res = eventPacket[i]->AddEvent(actual);
                        if (eventPacket[i]->ReadyToSend())
                        {
                            SendEvent(i);
                            eventPacket[i]->ClearPacket();
                            if (!res) eventPacket[i]->AddEvent(actual);
                        }
                        */
                        SendDataMessage( i, event_data_message );
                    }
            }


            if (delete_events)
            {
                tmp = actual->next;
                Eventos->Delete(actual);
                actual = tmp;
            }
            else actual = actual->next;
        }

    }



    if (m_sendMoldeoApi) {
        actual = Eventos->First;
        while(actual != NULL)
        {
            if ( actual->deviceid == MO_IODEVICE_CONSOLE
                && actual->devicecode == MO_ACTION_MOLDEOAPI_EVENT_SEND
                && actual->reservedvalue3 == MO_DATAMESSAGE
                && actual->pointer ) {

              /// moldeoapi_data_message
              if (debug_is_on) MODebug2->Message( moText("moNetOSCOut::Update "+GetLabelName()+" > Sending Moldeo API Message!") );
              moDataMessage* MoldeoAPIMessage = (moDataMessage*)actual->pointer;

              for (i = 0; i < host_name.Count(); i++)
                {
                    //res = eventPacket[i]->AddEvent(actual);
                    //if (eventPacket[i]->ReadyToSend())
                    {
                        if (debug_is_on) MODebug2->Message( moText("moNetOSCOut::Update "+GetLabelName()+" > SendDataMessage to host: I:") + (moText)IntToStr( i )+host_name[i] );
                        SendDataMessage( i, *MoldeoAPIMessage );
                        //eventPacket[i]->ClearPacket();
                        //if (!res) eventPacket[i]->AddEvent(actual);
                    }
                }

            }

            actual = actual->next;
        }
    }
/*

*/

    //inlets outlets
    moMoldeoObject::Update(Eventos);


    int MsgToSend = GetInletIndex("DATAMESSAGE" );
    if (MsgToSend > -1 && 1==1) {
      moInlet* pInlet = GetInlets()->Get(MsgToSend);
      if (pInlet ) {
          if (pInlet->Updated()) {
            moDataMessage data_message;
            moData mData;
            moData* pData = pInlet->GetData();
            if (pData) {
              if (pData->Type()==MO_DATA_MESSAGE) {

              //if (debug_is_on) MODebug2->Message(  + pData->ToText());

                moDataMessage* pMess = pData->Message();
                if (pMess) {
                  //MODebug2->Push( moText(" text: ") + pData->ToText());
                  for(int k=0; k<pMess->Count(); k++ ) {
                    mData = pMess->Get(k);
                    data_message.Add(mData);
                    //MODebug2->Push( moText(" text d: ") + mData.ToText());
                  }
                }

                //mData.SetText("test");
                //data_message.Add(mData);
                //moDataMessage DM = *pData->Message();
                for (i = 0; i < host_name.Count(); i++)
                {
                    {
                        //MODebug2->Push( moText("sending DATAMESSAGE: ") + ccc );
                        SendDataMessage( i, data_message );
                    }
                }
              }
            }
            //moDataMessage* pMoldeoDataMessage = (moDataMessage*)pInlet->GetData()->Message();
              /*
              moText ccc;
              for( int c=0; c<DM.Count(); c++) {
                ccc = ccc + DM.Get(c).ToText();
              }*/
/*
            if (pMoldeoDataMessage) {
              moDataMessage DM = (*pMoldeoDataMessage);
              for (i = 0; i < host_name.Count(); i++)
              {
                  {
                      //MODebug2->Push( moText("sending DATAMESSAGE: ") + ccc );
                      //SendDataMessage( i, DM );
                  }
              }

            }
            */
          }
      }
    }

    MsgToSend = GetInletIndex("DATAMESSAGES" );
    if (MsgToSend > -1 && 1==1) {
      //if (debug_is_on) MODebug2->Message( moText("moNetOscOut::Update > DATAMESSAGES inlet"));
      moInlet* pInlet = GetInlets()->Get(MsgToSend);
      if (pInlet ) {
          //if (debug_is_on) MODebug2->Message( moText("moNetOscOut::Update > pInlet"));
          if (pInlet->Updated()) {
            //if (debug_is_on) MODebug2->Message( moText("moNetOscOut::Update > pInlet updated!"));
            moData* pData = pInlet->GetData();
            if (pData) {
              //if (debug_is_on) MODebug2->Message( moText("moNetOscOut::Update > pInlet updated pData:") + pData->TypeToText());
              if (pData->Type()==MO_DATA_MESSAGES) {

                //if (debug_is_on) MODebug2->Message( moText("moNetOscOut::Update > Receiving DATAMESSAGES: ") + pData->ToText());
                //if (debug_is_on) MODebug2->Message( moText("moNetOscOut::Update > Receiving DATAMESSAGES: ") );

                moDataMessages* pMessX = pData->Messages();

                if (pMessX) {


                  for(int m=0; m<pMessX->Count(); m++ ) {

                    const moDataMessage& pMess(pMessX->Get(m));
                    moDataMessage data_message;
                    moData mData;

                    //MODebug2->Push( moText(" text: ") + pData->ToText());
                    for(int k=0; k<pMess.Count(); k++ ) {

                      mData = pMess.Get(k);
                      data_message.Add(mData);
                      if (debug_is_on) MODebug2->Message( moText("moNetOscOut::Update "+GetLabelName()+" > Receiving DATAMESSAGES Message") + IntToStr(m)+ moText(" Data: ") +IntToStr(k) + " Val: " + mData.ToText());
                    }

                    for (i = 0; i < host_name.Count(); i++)
                    {
                      {
                          //if (debug_is_on) MODebug2->Message( moText("moNetOscOut::Update > sending DATAMESSAGE: ") );
                          SendDataMessage( i, data_message );
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }

    moTrackerSystemData* m_pTrackerData = NULL;
    //bool m_bTrackerInit = false;
	//Procesar Inlets

	//MODebug2->Message("netoscout:  updating");
/*
	for(int i=0; i<m_Inlets.Count(); i++) {
		moInlet* pInlet = m_Inlets[i];
		if (pInlet->Updated() && ( pInlet->GetConnectorLabelName()==moText("TRACKERKLT")) ) {
*/
    int Tracker = GetInletIndex("TRACKERKLT" );
    if (Tracker > -1) {
      moInlet* pInlet = GetInlets()->Get(Tracker);
      if (pInlet ) {
          if (pInlet->Updated()) {
            //MODebug2->Message("netoscout: receiving tracker data");

			m_pTrackerData = (moTrackerSystemData *)pInlet->GetData()->Pointer();
			if (m_pTrackerData /*&& !m_bTrackerInit*/ ) {

			    moDataMessage tracker_data_message;

			    moData pData;
/*
                pData.SetText( moText("speak") );
                tracker_data_message.Add(pData);

                pData.SetText( moText("N") );
                tracker_data_message.Add(pData);
                */

                pData.SetInt( m_pTrackerData->GetValidFeatures() );
                tracker_data_message.Add(pData);


                //if (m_pTrackerData->GetValidFeatures()>0 && m_pTrackerData->GetValidFeatures()<80) {

                    pData.SetText( moText("MAX") );
                    tracker_data_message.Add(pData);

                    pData.SetFloat( m_pTrackerData->GetMin().X() );
                    tracker_data_message.Add(pData);
                    pData.SetFloat( m_pTrackerData->GetMin().Y() );
                    tracker_data_message.Add(pData);
                    pData.SetFloat( m_pTrackerData->GetMax().X() );
                    tracker_data_message.Add(pData);
                    pData.SetFloat( m_pTrackerData->GetMax().Y() );
                    tracker_data_message.Add(pData);

                    pData.SetText( moText("BAR") );
                    tracker_data_message.Add(pData);

                    pData.SetFloat( m_pTrackerData->GetBarycenter().X() );
                    tracker_data_message.Add(pData);
                    pData.SetFloat( m_pTrackerData->GetBarycenter().Y() );
                    tracker_data_message.Add(pData);

                    pData.SetText( moText("VAR") );
                    tracker_data_message.Add(pData);

                    pData.SetFloat( m_pTrackerData->GetVariance().X() );
                    tracker_data_message.Add(pData);

                    pData.SetFloat( m_pTrackerData->GetVariance().Y() );
                    tracker_data_message.Add(pData);

                    pData.SetFloat( m_pTrackerData->GetVariance().Length() );
                    tracker_data_message.Add(pData);

                    pData.SetText( moText("MP") );
                    tracker_data_message.Add(pData);
                    for(i=0;i<m_pTrackerData->m_Zones;i++) {
                        pData.SetInt( m_pTrackerData->GetPositionMatrix(m_pTrackerData->ZoneToPosition(i)) );
                        tracker_data_message.Add(pData);
                    }

                    pData.SetText( moText("MM") );
                    tracker_data_message.Add(pData);
                    for(i=0;i<m_pTrackerData->m_Zones;i++) {
                        pData.SetInt( m_pTrackerData->GetMotionMatrix(m_pTrackerData->ZoneToPosition(i)) );
                        tracker_data_message.Add(pData);
                    }

                    pData.SetText( moText("MA") );
                    tracker_data_message.Add(pData);
                    for(i=0;i<m_pTrackerData->m_Zones;i++) {
                        pData.SetInt( m_pTrackerData->GetAccelerationMatrix(m_pTrackerData->ZoneToPosition(i)) );
                        tracker_data_message.Add(pData);
                    }

                    pData.SetText( moText("VEL") );
                    tracker_data_message.Add(pData);

                    pData.SetFloat( m_pTrackerData->GetBarycenterMotion().Length() );
                    tracker_data_message.Add(pData);
                    pData.SetFloat( m_pTrackerData->GetBarycenterMotion().X() );
                    tracker_data_message.Add(pData);
                    pData.SetFloat( m_pTrackerData->GetBarycenterMotion().Y() );
                    tracker_data_message.Add(pData);

                    pData.SetText( moText("ACC") );
                    tracker_data_message.Add(pData);

                    pData.SetFloat( m_pTrackerData->GetBarycenterAcceleration().Length() );
                    tracker_data_message.Add(pData);
                    pData.SetFloat( m_pTrackerData->GetBarycenterAcceleration().X() );
                    tracker_data_message.Add(pData);
                    pData.SetFloat( m_pTrackerData->GetBarycenterAcceleration().Y() );
                    tracker_data_message.Add(pData);

                    //MODebug2->Message( moText("netoscout: receiving tracker data: bx:") + (moText)FloatToStr(m_pTrackerData->GetBarycenter().X()) );
                    //MODebug2->Push( moText("N:") + (moText)IntToStr( m_pTrackerData->GetValidFeatures() ) );
                    for (i = 0; i < host_name.Count(); i++)
                    {
                        //res = eventPacket[i]->AddEvent(actual);
                        //if (eventPacket[i]->ReadyToSend())
                        {
                            //MODebug2->Push( moText("sending I:") + (moText)IntToStr( i ) );
                            SendDataMessage( i, tracker_data_message );
                            //eventPacket[i]->ClearPacket();
                            //if (!res) eventPacket[i]->AddEvent(actual);
                        }
                    }
               // }

			}
          }
      }
	}

}

MOboolean moNetOSCOut::Finish()
{
#ifdef OSCPACK
	if (packetStream != NULL)
	{
		delete packetStream;
		packetStream = NULL;
	}
	#endif
	if (packetBuffer != NULL)
	{
		delete packetBuffer;
		packetBuffer = NULL;
	}
	/*
	for (int i = 0; i < eventPacket.Count(); i++)
	{
	    delete transmitSockets[i];
	    delete eventPacket[i];
	}
	*/
	return true;
}

void moNetOSCOut::SendEvent(int i)
{
	int n;
	moEventStruct ev;

#ifdef OSCPACK
	packetStream->Clear();
    (*packetStream) << osc::BeginBundleImmediate;
	for (n = 0; n < eventPacket[i]->GetNumEvents(); n++)
	{
	   ev = eventPacket[i]->GetEvent(n);
       (*packetStream) << osc::BeginMessage( moText("") + IntToStr(n) )
	                   << ev.valid << ev.devicecode << ev.deviceid
		               << ev.reservedvalue0 << ev.reservedvalue1
		               << ev.reservedvalue2 << ev.reservedvalue3
		               << osc::EndMessage;
    }
    (*packetStream) << osc::EndBundle;
    transmitSockets[i]->Send( packetStream->Data(), packetStream->Size() );
#endif // OSCPACK
}

void moNetOSCOut::SendDataMessage( int i, moDataMessage &datamessage ) {
#ifdef OSCPACK
    if (packetStream==NULL) {
        cout << "moNetOSCOut::SendDataMessage > error: packetStream is NULL" << endl;
        return;
    }
#endif

	//cout << "moNetOSCOut::SendDataMessage > start" << endl;
#ifdef OSCPACK
	packetStream->Clear();
	//cout << "moNetOSCOut::SendDataMessage > Clear() ok." << endl;

    (*packetStream) << osc::BeginBundleImmediate;
    cout << "moNetOSCOut::SendDataMessage > BeginBundleImmediate ok." << endl;

    (*packetStream) << osc::BeginMessage("/moldeo");
    //cout << "moNetOSCOut::SendDataMessage > data in messages:" << datamessage.Count() << endl;
#else
lo_timetag timetag;
lo_timetag_now(&timetag);
lo_bundle bundle = lo_bundle_new(timetag);
lo_message ms = lo_message_new();
moText oscpath = "";
#endif
    moData data;
    int nfields = 0;
    int error = 0;
    try {
      for(int j=0; j< datamessage.Count(); j++) {
          data = datamessage[j];
          if (debug_is_on) {
            MODebug2->Message( moText("moNetOSCOut::SendDataMessage "+GetLabelName()+" > data:") + IntToStr(j) + moText(":") + data.ToText() );
          }
          switch(data.Type()) {
          #ifdef OSCPACK
              case MO_DATA_NUMBER_FLOAT:
                  (*packetStream) << data.Float();
                  break;
              case MO_DATA_NUMBER_INT:
                  (*packetStream) << data.Int();
                  break;
              case MO_DATA_NUMBER_LONG:
                  (*packetStream) << data.Long();
                  break;
              case MO_DATA_NUMBER_DOUBLE:
                  (*packetStream) << data.Double();
                  break;
              case MO_DATA_TEXT:
                  (*packetStream) << (char*)data.Text();
                  break;
          #else
              case MO_DATA_NUMBER_FLOAT:
                  error = lo_message_add_float( ms , data.Float());
                  nfields++;
                  break;
              case MO_DATA_NUMBER_INT:
                  error = lo_message_add_int32( ms , data.Int());
                  nfields++;
                  break;
              case MO_DATA_NUMBER_LONG:
                  error = lo_message_add_int64( ms , data.Long());
                  nfields++;
                  break;
              case MO_DATA_NUMBER_DOUBLE:
                  error = lo_message_add_double( ms , data.Double());
                  nfields++;
                  break;
              case MO_DATA_TEXT:
                  if (oscpath=="") {
                    oscpath = data.Text();
                    moText mot = data.Text().Left(50000);
                    error = lo_message_add_string( ms , (char*)mot);
                    nfields++;
                  } else {
                    moText mot = data.Text().Left(50000);
                    error = lo_message_add_string( ms , (char*)mot);
                    nfields++;
                  }
                  break;

          #endif


          }
        if (error<0)
          MODebug2->Error(moText("moNetOSCOut > data error adding value: ") + data.ToText() );
      }
#ifdef OSCPACK
    } catch(osc::Exception E) {
      MODebug2->Error( moText("moNetOSCOut > Exception: ") + E.what()
                      + " packet actual size: (" + IntToStr( (*packetStream).Size() ) + ") "
                      + " Data too long for buffer: (max OUTPUT_BUFFER_SIZE:" + IntToStr(OUTPUT_BUFFER_SIZE) + ") "
                      + data.ToText()
                      + " (size:"+ IntToStr(data.ToText().Length())+" )"
                      + " (total size:" + IntToStr( data.ToText().Length() + (*packetStream).Size() ) + ")" );
    }
  (*packetStream) << osc::EndMessage;
    //cout << "moNetOSCOut::SendDataMessage > osc::EndMessage ok" << endl;
    //cout << "osc::EndBundle: " << endl;
    //cout << osc::EndBundle << endl;

    #ifdef MO_WIN32
        (*packetStream) << osc::EndBundle;
    #endif
    //cout << "moNetOSCOut::SendDataMessage > osc::EndBundle ok" << endl;

    //cout << "moNetOSCOut::SendDataMessage > sending." << endl;

    if (transmitSockets[i]) {
        //MODebug2->Message(moText("moNetOSCOut > sending ") + IntToStr(i) + " size:" + IntToStr(packetStream->Size()) );
        transmitSockets[i]->Send( packetStream->Data(), packetStream->Size() );

    }
#else
    } catch(...) {
        MODebug2->Error( moText("moNetOSCOut > Exception:"));
    }

    //char* bundlen = IntToStr(nfields);
/*
    char * fullt = data.Text();
    lo_blob bb = lo_blob_new(data.Text().Length(), (void*)fullt);
    lo_message_add_blob(ms, bb);*/

    lo_bundle_add_message( bundle, "/moldeo", ms);

    if (transmitSockets[i]) {
        //MODebug2->Message(moText("moNetOSCOut > sending ") + IntToStr(i) + " size:" + IntToStr(packetStream->Size()) );
        //transmitSockets[i]->Send( packetStream->Data(), packetStream->Size() );
        /*
        lo::Bundle myBundle = lo::Bundle({{"example", lo::Message("i", 1234321)},
                       {"example", lo::Message("i", 4321234)}});
        transmitSockets[i]->send(myBundle);
        *//*

        int rres = lo_send( transmitSockets[i], "/moldeo","sf","consoleget",1.618f);
        lo_send( transmitSockets[i], "/moldeo", "f", 3.1415f );


        moText pruebatexto = "";
        for(int t=0; t<3000; t++ ) {
            lo_message ms2 = lo_message_new();
            int err = lo_message_add_float( ms2 , 3.1415 );
            pruebatexto+= moText("á");
            err = lo_message_add_string( ms2 , pruebatexto );
            err = lo_message_add_int32( ms2 , pruebatexto.Length() );
            rres = lo_send_message( transmitSockets[i], "/moldeo", ms2 );
            if (rres<0) break;
        }*/

        //err = lo_message_add_string( ms2 , data.ToText() );
        int datalen = lo_bundle_length(bundle);
        //MODebug2->Message( IntToStr(pruebatexto.Length()) );
        int rres = lo_send_bundle( transmitSockets[i], bundle );
        //int rres =
        if (rres<=0) {
          MODebug2->Error("moNetOSCOut::SendDataMessage > Couldnt send osc rres:"+IntToStr(rres)+" datamessage datalen:" + IntToStr(datalen)+"/"+IntToStr(LO_MAX_MSG_SIZE));
        }

    }
#endif




    //MODebug2->Push(moText("sending"));

}

