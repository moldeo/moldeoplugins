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
    pOutEvents = NULL;
    pOutTracker = NULL;
    pOutData = NULL;
    pOutDataMessages = NULL;

    pOutAndOsc = NULL;

    pOutPositionX = NULL;
    pOutPositionY = NULL;
    pOutPositionZ = NULL;

    pOutAccelerationX = NULL;
    pOutAccelerationY = NULL;
    pOutAccelerationZ = NULL;

    pOutOrientationX = NULL;
    pOutOrientationY = NULL;
    pOutOrientationZ = NULL;


    pOutBeatFreq = NULL;
    pOutBeatValue = NULL;

    pOutBeatHighFreq = NULL;
    pOutBeatHighValue = NULL;

    pOutBeatLowFreq = NULL;
    pOutBeatLowValue = NULL;

    pOutBeatMediumFreq = NULL;
    pOutBeatMediumValue = NULL;


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
                    pOutEvents = pOutlet;
                }
                if (pOutlet->GetConnectorLabelName() == moText("NOTE")) {
                    pOutNote = pOutlet;
                }
                if (pOutlet->GetConnectorLabelName() == moText("NOTEVEL")) {
                    pOutNoteVel = pOutlet;
                }
                if (pOutlet->GetConnectorLabelName() == moText("DATA")) {
                    pOutData = pOutlet;
                }
                if (pOutlet->GetConnectorLabelName() == moText("DATAMESSAGES")) {
                    pOutDataMessages = pOutlet;
                }
                if (pOutlet->GetConnectorLabelName() == moText("TRACKERSYSTEM")) {
                    pOutTracker = pOutlet;
                }
                if (pOutlet->GetConnectorLabelName() == moText("ANDOSC")) {
                    pOutAndOsc = pOutlet;
                }

                if (pOutlet->GetConnectorLabelName() == moText("POSITIONX")) {
                    pOutPositionX = pOutlet;
                }
                if (pOutlet->GetConnectorLabelName() == moText("POSITIONY")) {
                    pOutPositionY = pOutlet;
                }
                if (pOutlet->GetConnectorLabelName() == moText("POSITIONZ")) {
                    pOutPositionZ = pOutlet;
                }

                if (pOutlet->GetConnectorLabelName() == moText("ORIENTATIONX")) {
                    pOutOrientationX = pOutlet;
                }
                if (pOutlet->GetConnectorLabelName() == moText("ORIENTATIONY")) {
                    pOutOrientationY = pOutlet;
                }
                if (pOutlet->GetConnectorLabelName() == moText("ORIENTATIONZ")) {
                    pOutOrientationZ = pOutlet;
                }

                if (pOutlet->GetConnectorLabelName() == moText("ACCELERATIONX")) {
                    pOutAccelerationX = pOutlet;
                }
                if (pOutlet->GetConnectorLabelName() == moText("ACCELERATIONY")) {
                    pOutAccelerationY = pOutlet;
                }
                if (pOutlet->GetConnectorLabelName() == moText("ACCELERATIONZ")) {
                    pOutAccelerationZ = pOutlet;
                }

                /*BEAT*/
                if (pOutlet->GetConnectorLabelName() == moText("BEATFREQ")) {
                    pOutBeatFreq = pOutlet;
                }

                if (pOutlet->GetConnectorLabelName() == moText("BEATVAL")) {
                    pOutBeatValue = pOutlet;
                }

                if (pOutlet->GetConnectorLabelName() == moText("BEATHIGHFREQ")) {
                    pOutBeatHighFreq = pOutlet;
                }

                if (pOutlet->GetConnectorLabelName() == moText("BEATHIGHVAL")) {
                    pOutBeatHighValue = pOutlet;
                }

                if (pOutlet->GetConnectorLabelName() == moText("BEATLOWFREQ")) {
                    pOutBeatLowFreq = pOutlet;
                }

                if (pOutlet->GetConnectorLabelName() == moText("BEATLOWVAL")) {
                    pOutBeatLowValue = pOutlet;
                }

                if (pOutlet->GetConnectorLabelName() == moText("BEATMEDIUMFREQ")) {
                    pOutBeatMediumFreq = pOutlet;
                }

                if (pOutlet->GetConnectorLabelName() == moText("BEATMEDIUMVAL")) {
                    pOutBeatMediumValue = pOutlet;
                }


            }

    }
}

MOint
moOscPacketListener::GetOutletIndex( moOutlets* pOutlets, moText p_connector_name ) const {
  if (pOutlets)
	for( MOuint i=0; i< pOutlets->Count(); i++ ) {
	    moOutlet* pOutlet = pOutlets->Get(i);
		if ( pOutlet )
            if ( pOutlet->GetConnectorLabelName() == p_connector_name )
                return i;
	}
	return (-1);

}

int
moOscPacketListener::Update( moOutlets* pOutlets,
                            bool p_debug_is_on,
                            moEventList* pEvents,
                            int p_ProcessMoldeoApi,
                            int p_MoldeoId  ) {
    //block message
    m_Semaphore.Lock();

    debug_is_on = p_debug_is_on;

    moOutlet* poutlet = NULL;

    if (pOutEvents==NULL) {
        Init(pOutlets);
    }

    if (pEvents) {
      	moEvent *actual=NULL,*tmp;
        moMessage *pmessage;

        if (pEvents) actual = pEvents->First;

        ///Procesamos los eventos recibidos de los MoldeoObject Outlets
        while(actual!=NULL) {
          tmp = actual->next;
          ///procesamos aquellos Outlet q estan dirigidos a este objeto
          if (actual->deviceid==MO_IODEVICE_CONSOLE
              && actual->devicecode == MO_ACTION_MOLDEOAPI_EVENT_RECEIVE
              && actual->reservedvalue0 == p_MoldeoId
              && actual->reservedvalue3 == MO_DATAMESSAGE) {
                  moDataMessage* mpDataMessage = (moDataMessage*) actual->pointer;
                  delete mpDataMessage;
                  actual->pointer = NULL;
                  pEvents->Delete(actual);
          }
          actual = tmp;
        }

    }


    //MODebug2->Push( moText( "Messages.Count():" ) + IntToStr( Messages.Count() )  ) ;

    for( int j=0; j<Messages.Count();j++) {

        moDataMessage message;
        poutlet = NULL;

        message = Messages.Get(j);


        //sumamos a los mensajes....
        ///primer dato debe contener el codigo interno del evento o palabra que lo represente
        ///por ejemplo TRACKERSYSTEM, o EVENT, o POSITION, o ORIENTATION, etc...
        ///luego el resto del mensaje contiene los datos adicionales X, Y, Z
        moData DataCode;

        if (message.Count()>0) {
             DataCode = message.Get(0);
             if (debug_is_on) {
                moText argumentsText;
                for( int cc = 1; cc < message.Count(); cc++ ) {
                  argumentsText+= moText(" Arg ") + IntToStr(cc) + moText(": ") + message.Get(cc).ToText();
                }

                MODebug2->Push( moText( "Message:" ) + DataCode.Text() + moText( " Arguments:" ) + IntToStr(message.Count()-1) + moText(" > ") + argumentsText );

             }
        }

        if (
            DataCode.Text() == moText("BEAT")
            && message.Count()>=2
            ) {
            moData Freq = message.Get(1);
            moData Value = message.Get(2);
            if (pOutBeatFreq) {
                pOutBeatFreq->GetData()->SetDouble(Freq.Double());
                pOutBeatFreq->Update();
            }
            if (pOutBeatValue) {
                pOutBeatValue->GetData()->SetDouble(Value.Double());
                pOutBeatValue->Update();
            }
            if (debug_is_on) MODebug2->Push( moText( "Message Beat Freq: " )
                                            + FloatToStr(Freq.Double())
                                            + moText(" Val:")
                                            + FloatToStr(Value.Double()) );
        }

        if (
            DataCode.Text() == moText("BEATHIGH")
            && message.Count()>=2
            ) {
            moData Freq = message.Get(1);
            moData Value = message.Get(2);
            if (pOutBeatHighFreq) {
                pOutBeatHighFreq->GetData()->SetDouble(Freq.Double());
                pOutBeatHighFreq->Update();
            }
            if (pOutBeatHighValue) {
                pOutBeatHighValue->GetData()->SetDouble(Value.Double());
                pOutBeatHighValue->Update();
            }
            if (debug_is_on) MODebug2->Push( moText( "Message Beat High Freq: " )
                                            + FloatToStr(Freq.Double())
                                            + moText(" Val:")
                                            + FloatToStr(Value.Double()) );
        }

        if (
            DataCode.Text() == moText("BEATMEDIUM")
            && message.Count()>=2
            ) {
            moData Freq = message.Get(1);
            moData Value = message.Get(2);
            if (pOutBeatMediumFreq) {
                pOutBeatMediumFreq->GetData()->SetDouble(Freq.Double());
                pOutBeatMediumFreq->Update();
            }
            if (pOutBeatMediumValue) {
                pOutBeatMediumValue->GetData()->SetDouble(Value.Double());
                pOutBeatMediumValue->Update();
            }
            if (debug_is_on) MODebug2->Push( moText( "Message Beat Medium Freq: " )
                                            + FloatToStr(Freq.Double())
                                            + moText(" Val:")
                                            + FloatToStr(Value.Double()) );
        }

        if (
            DataCode.Text() == moText("BEATLOW")
            && message.Count()>=2
            ) {
            moData Freq = message.Get(1);
            moData Value = message.Get(2);
            if (pOutBeatLowFreq) {
                pOutBeatLowFreq->GetData()->SetDouble(Freq.Double());
                pOutBeatLowFreq->Update();
            }
            if (pOutBeatLowValue) {
                pOutBeatLowValue->GetData()->SetDouble(Value.Double());
                pOutBeatLowValue->Update();
            }
            if (debug_is_on) MODebug2->Push( moText( "Message Beat Low Freq: " )
                                            + FloatToStr(Freq.Double())
                                            + moText(" Val:")
                                            + FloatToStr(Value.Double()) );
        }

        if ( DataCode.Text() == moText("EVENT") ) {
            /*
            poutlet = pOutEvents;
            poutlet->GetMessages().Add( message );
            */
            //MODebug2->Push( moText("receiving event:") +  message.Get(1).ToText() );
        } else if ( DataCode.Text() == moText("TRACKERSYSTEM") ) {
            /*
            poutlet = pOutTracker;
            poutlet->GetMessages().Add( message );
            */
        } else if ( DataCode.Text() == moText("ANDOSC") ) {
            /*
            poutlet = pOutAndOsc;
            poutlet->GetMessages().Add( message );
            */
        } else if ( DataCode.Text() == moText("POSITION") ) {
/*
            int iref = 1;
            bool multitouch = false;
            moData PosI = message.Get(iref);

            if (        PosI.Type()==MO_DATA_NUMBER_INT
                    || PosI.Type()==MO_DATA_NUMBER_LONG ) {
                iref+= 1;
                multitouch = true;
            }

            if (message.Count()>=3 && pOutPositionX && pOutPositionY ) {
                moData PosX = message.Get(iref++);
                moData PosY = message.Get(iref++);

                ///DEBDERIA CALIBRARSE.... o fijarse en el plugin
                pOutPositionX->GetData()->SetDouble(  ( PosX.Double() -240 ) / 480.0 );
                pOutPositionY->GetData()->SetDouble(  ( 400 - PosY.Double() ) / 800.0 );
                pOutPositionX->Update();
                pOutPositionY->Update();

                //MODebug2->Push(moText("Pos X:") + FloatToStr(pOutPositionX->GetData()->Double()) );
                //MODebug2->Push(moText("Pos Y:") + FloatToStr(pOutPositionY->GetData()->Double()) );
            }
            if (!multitouch) {
                if (message.Count()>=4 && pOutPositionZ) {
                    moData PosZ = message.Get(iref++);
                    pOutPositionZ->GetData()->SetDouble(  PosZ.Double() );
                    pOutPositionZ->Update();
                }
            } else {
                //multitouch: procesar varios touchs
                    //finger id:
                    //finger pos x:
                    //finger pos y:
                    //finger size:

            }
            */
        } else if ( DataCode.Text() == moText("ORIENTATION") ) {
/*
            if (message.Count()>=4 && pOutOrientationX && pOutOrientationY && pOutOrientationZ ) {

                moData OriX = message.Get(1);
                moData OriY = message.Get(2);
                moData OriZ = message.Get(3);

                pOutOrientationX->GetData()->SetDouble(  ( 45 - OriX.Double() ) );
                pOutOrientationY->GetData()->SetDouble(  OriY.Double() );
                pOutOrientationZ->GetData()->SetDouble(  OriZ.Double() );
                pOutOrientationX->Update();
                pOutOrientationY->Update();
                pOutOrientationZ->Update();

                //MODebug2->Push(moText("Ori X:") + FloatToStr(pOutOrientationX->GetData()->Double()) );
                //MODebug2->Push(moText("Ori Y:") + FloatToStr(pOutOrientationY->GetData()->Double()) );
                //MODebug2->Push(moText("Ori Z:") + FloatToStr(pOutOrientationZ->GetData()->Double()) );
            }
            */
        } else if ( DataCode.Text() == moText("ACCELERATION") ) {

            if (message.Count()>=4 && pOutAccelerationX && pOutAccelerationY && pOutAccelerationZ) {
                moData AccelX = message.Get(1);
                moData AccelY = message.Get(2);
                moData AccelZ = message.Get(3);

                pOutAccelerationX->GetData()->SetDouble(  AccelX.Double() );
                pOutAccelerationY->GetData()->SetDouble(  AccelY.Double() );
                pOutAccelerationZ->GetData()->SetDouble(  AccelZ.Double() );
                pOutAccelerationX->Update();
                pOutAccelerationY->Update();
                pOutAccelerationZ->Update();

            }

        }

        if (
            DataCode.Text() == moText("MOLDEO") ) {

            if (debug_is_on)
                MODebug2->Push( moText( "MOLDEO COMMAND received" ) );

            if (p_ProcessMoldeoApi!=0) {
              if (pEvents) {
                  //MODebug2->Push( moText( "Processing Moldeo API COMMAND" ) );
                  moText ApiMessage = message.Get(1).ToText();
                  moDataMessage* sendMessage = new moDataMessage();
                  sendMessage->Copy( message, 1, message.Count()-1);
                  //sendMessage->Add( moData( ApiMessage ) );
                  /*process messages....*/
                  pEvents->Add( MO_IODEVICE_CONSOLE,
                                MO_ACTION_MOLDEOAPI_EVENT_RECEIVE,
                                p_MoldeoId, 0, 0, MO_DATAMESSAGE, sendMessage );


              }
            }
        }

        if (message.Count()>=3) {
        if (
             message.Get(1).ToText() == moText("opencv")

            ) {
          //MOTION_DETECTION
          if (debug_is_on)
              MODebug2->Push("NetOscIn > OPENCV Received");

          if (message.Count()>=3) {
            moData outletName = message.Get(2);
            moData outletValue = message.Get(3);
            //check if this outlet exists and update it!!!
            int idx = GetOutletIndex( pOutlets, outletName.Text());
            if (idx>-1) {
              moOutlet* pOutlet = pOutlets->Get(idx);
              if (pOutlet) {
                moData &Data( *pOutlet->GetData() );
                Data = outletValue;
                //pOutlet->GetData()->SetInt()
                pOutlet->Update(true);
                if (debug_is_on) MODebug2->Push("NetOscIn > OPENCV, outlet for:" + outletName.Text()+" outletValue:" + outletValue.ToText() );
              }
            } else {
             if (debug_is_on) MODebug2->Push("NetOscIn > OPENCV, no outlet for:" + outletName.Text() );
            }
          }
        }
        }

        if (
            DataCode.Text() == moText("note") || DataCode.Text() == moText("NOTE") ) {
            if (message.Count()>=4 && pOutNote && pOutNoteVel ) {
                moData NoteValue = message.Get(1);
                moData VelMessage = message.Get(2);
                moData VelValue = message.Get(3);

                pOutNote->GetData()->SetInt(  NoteValue.Int() );
                pOutNoteVel->GetData()->SetInt(  VelValue.Int() );
                pOutNote->Update();
                pOutNoteVel->Update();
            }
        }



        if (poutlet) {

            poutlet->Update();

            if (poutlet->GetType()==MO_DATA_MESSAGES)
                poutlet->GetData()->SetMessages( &poutlet->GetMessages() );

            if (poutlet->GetType()==MO_DATA_MESSAGE) {
                moDataMessages& msgs( poutlet->GetMessages() );
                if (msgs.Count()>0) {
                    poutlet->GetData()->SetMessage( & msgs[ msgs.Count() - 1 ] );
                }
            }
        }


    }

    if (pOutDataMessages) {
      moDataMessages& msgs( pOutDataMessages->GetMessages() );
      msgs.Empty();
      msgs = Messages;
      if ( pOutDataMessages->GetType()==MO_DATA_MESSAGES ) {
        pOutDataMessages->GetData()->SetMessages( &msgs );
        pOutDataMessages->Update();
      }
    }

    int nmess = Messages.Count();
    Messages.Empty();

    m_Semaphore.Unlock();
    return nmess;
}


void
moOscPacketListener::ProcessMessage( const osc::ReceivedMessage& m,
				const IpEndpointName& remoteEndpoint ) {



        m_Semaphore.Lock();
        moDataMessage message;

        moData  data0;


        try {
            //moAbstract::MODebug2->Push(moText("N: ")+IntToStr(m.ArgumentCount()));

            /*AND OSC*/
            /// El primer dato corresponderá a una palabra que represente la secuencia de datos.

            if (
                    moText( m.AddressPattern() ) == moText("/1/xy") //andOSC
                || moText( m.AddressPattern() ) == moText("/touch") //OSCTouch i = finger_id, f: x f: y f:size

                ) {

                data0 = moData( moText( "POSITION" ) );
                message.Add( data0 );
            } else if ( moText( m.AddressPattern() ) == moText("/ori") ) {
                data0 = moData( moText( "ORIENTATION" ) );
                message.Add( data0 );
            } else if ( moText( m.AddressPattern() ) == moText("/acc") ) {
                data0 = moData( moText( "ACCELERATION" ) );
                message.Add( data0 );
            } else/*AnalyseSOundMonar*/
            if ( moText( m.AddressPattern() ) == moText("/beat") ) {
                data0 = moData( moText( "BEAT" ) );
                message.Add( data0 );
            } else if ( moText( m.AddressPattern() ) == moText("/beathigh") ) {
                data0 = moData( moText( "BEATHIGH" ) );
                message.Add( data0 );
            } else if ( moText( m.AddressPattern() ) == moText("/beatmedium") ) {
                data0 = moData( moText( "BEATMEDIUM" ) );
                message.Add( data0 );
            } else if ( moText( m.AddressPattern() ) == moText("/beatlow") ) {
                data0 = moData( moText( "BEATLOW" ) );
                message.Add( data0 );
            } else if ( moText( m.AddressPattern() ).SubText(0,6) == moText("/moldeo") ) {
                data0 = moData( moText( "MOLDEO" ) );
                message.Add( data0 );
            } else if ( moText( m.AddressPattern() ) == moText("/opencv") ) {
                data0 = moData( moText( "OPENCV" ) );
                message.Add( data0 );
            } else {
                data0 = moData( moText(  m.AddressPattern() ) );
                message.Add( data0 );
            }


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
                //if (debug_is_on) MODebug2->Push( moText(" Data type:") + data.TypeToText()+ moText(": ") + data.ToText() );
                (arg++);
            }
        }
        catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or
            // missing arguments get thrown as exceptions.

            std::cout << "error while parsing message: "
                << m.AddressPattern() << ": " << e.what() << "\n";

        }
        Messages.Add(message);
        m_Semaphore.Unlock();
    }

// moNetOSCIn class **************************************************

moNetOSCIn::moNetOSCIn() {
    SetName("netoscin");
    m_pEvents = NULL;
}

moNetOSCIn::~moNetOSCIn() {
    Finish();
}

MOboolean moNetOSCIn::Init() {
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
	moDefineParamIndex( NETOSCIN_PROCESSMOLDEOAPI, moText("process_moldeo_api") );

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
    m_ProcessMoldeoApi = m_Config.Int( moR(NETOSCIN_PROCESSMOLDEOAPI) );

}

moConfigDefinition *
moNetOSCIn::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moMoldeoObject::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("hosts"), MO_PARAM_TEXT, NETOSCIN_HOSTS, moValue("127.0.0.1","TXT") );
	p_configdefinition->Add( moText("port"), MO_PARAM_NUMERIC, NETOSCIN_PORT, moValue("3334","INT") );
	p_configdefinition->Add( moText("receive_events"), MO_PARAM_NUMERIC, NETOSCIN_RECEIVEEVENTS, moValue("0","INT") );
	p_configdefinition->Add( moText("debug"), MO_PARAM_NUMERIC, NETOSCIN_DEBUG, moValue("0","INT") );
	p_configdefinition->Add( moText("process_moldeo_api"), MO_PARAM_NUMERIC, NETOSCIN_PROCESSMOLDEOAPI, moValue("1","INT") );


	return p_configdefinition;
}

MOswitch moNetOSCIn::SetStatus(MOdevcode codisp, MOswitch state) {
    return true;
}

MOswitch moNetOSCIn::GetStatus(MOdevcode codisp) {
    return(-1);
}

MOint moNetOSCIn::GetValue(MOdevcode codisp) {
    return(-1);
}

MOdevcode moNetOSCIn::GetCode(moText strcod) {
    return(-1);
}

void moNetOSCIn::Update(moEventList *Events) {

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
            int nnn = pListener->Update( GetOutlets(),
                                        debug_is_on,
                                        Events,
                                        m_ProcessMoldeoApi,
                                        GetMobDefinition().GetMoldeoId()  );
            //if (nnn>0 && debug_is_on) MODebug2->Push( moText("Updating outlets with messages: ") + IntToStr(nnn) );
        }

	}

    if (m_ReceiveEvents) {
        if (m_pEvents) {

            //MODebug2->Push( moText("Checking events "));
/*
            for(int i = 0; i<m_pEvents->GetMessages().Count();  i++) {

                moDataMessage& mess( m_pEvents->GetMessages().GetRef(i) );

                moData data;
                ///atencion el primer dato es "EVENT"
                int did = mess[1].Int();
                int cid = mess[2].Int();
                int val0 = mess[3].Int();
                int val1 = mess[4].Int();
                int val2 = mess[5].Int();
                int val3 = mess[6].Int();


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

                Events->Add( did, cid, val0, val1, val2, val3 );

            }
            */
        }
    }

	moMoldeoObject::Update(Events);

    for(i=0; i<GetOutlets()->Count();i++) {
        moOutlet* pOutlet = GetOutlets()->Get(i);
        if (pOutlet) {
            if (pOutlet->Updated()) {
                /*
                MODebug2->Push(moText("NetOscIn messages: ")+IntToStr(pOutlet->GetMessages().Count() )
                             +moText(" [1s int]: ")+IntToStr(pOutlet->GetMessages().GetRef(0).GetRef(0).Int())
                             +moText(" [2d int]: ")+IntToStr(pOutlet->GetMessages().GetRef(0).GetRef(1).Int())
                             +moText(" [3rd int]: ")+IntToStr(pOutlet->GetMessages().GetRef(0).GetRef(2).Int())
                             );
                             */
            }
        }
    }

}

MOboolean moNetOSCIn::Finish() {

	return true;
}

void moNetOSCIn::SendEvent(int i) {

}