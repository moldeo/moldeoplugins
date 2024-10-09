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

    pOutMagnetoX = NULL;
    pOutMagnetoY = NULL;
    pOutMagnetoZ = NULL;

    pOutCompass = NULL;


    pOutBeatFreq = NULL;
    pOutBeatValue = NULL;

    pOutBeatHighFreq = NULL;
    pOutBeatHighValue = NULL;

    pOutBeatLowFreq = NULL;
    pOutBeatLowValue = NULL;

    pOutBeatMediumFreq = NULL;
    pOutBeatMediumValue = NULL;

    pOutBlinkValue = NULL;
    pOutSurpriseValue = NULL;
    pOutFrownValue = NULL;
    pOutHoriValue = NULL;
    pOutVertiValue = NULL;

    pOutAndiamoX = NULL;
    pOutAndiamoY = NULL;

    for(int c=0;c<4;c++) {
      pOutEmgCh[c] = NULL;
    }

    debug_is_on = false;
}
#ifdef OSCPACK
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
#else
void moOscPacketListener::Set( lo_server_thread pudprcv ) {
    moAbstract::MODebug2->Message(moText("Set listener"));
    m_pUdpRcv = pudprcv;
}
#endif

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


                if (pOutlet->GetConnectorLabelName() == moText("BLINK")) {
                    pOutBlinkValue = pOutlet;
                }

                if (pOutlet->GetConnectorLabelName() == moText("EYEBROW")) {
                    pOutSurpriseValue = pOutlet;
                }

                if (pOutlet->GetConnectorLabelName() == moText("FURROW")) {
                    pOutFrownValue = pOutlet;
                }

                if (pOutlet->GetConnectorLabelName() == moText("HORIEYE")) {
                    pOutHoriValue = pOutlet;
                }

                if (pOutlet->GetConnectorLabelName() == moText("VERTIEYE")) {
                    pOutVertiValue = pOutlet;
                }

                if (pOutlet->GetConnectorLabelName() == moText("ANDIAMOX")) {
                    pOutAndiamoX = pOutlet;
                }

                if (pOutlet->GetConnectorLabelName() == moText("ANDIAMOY")) {
                    pOutAndiamoY = pOutlet;
                }


                if (pOutlet->GetConnectorLabelName() == moText("MAGNETOX")) {
                    pOutMagnetoX = pOutlet;
                }
                if (pOutlet->GetConnectorLabelName() == moText("MAGNETOY")) {
                    pOutMagnetoY = pOutlet;
                }
                if (pOutlet->GetConnectorLabelName() == moText("MAGNETOZ")) {
                    pOutMagnetoZ = pOutlet;
                }

                if (pOutlet->GetConnectorLabelName() == moText("COMPASS")) {
                    pOutCompass = pOutlet;
                }

                if (pOutlet->GetConnectorLabelName() == moText("EMGCH1")) {
                    pOutEmgCh[0] = pOutlet;
                }
                if (pOutlet->GetConnectorLabelName() == moText("EMGCH2")) {
                    pOutEmgCh[1] = pOutlet;
                }
                if (pOutlet->GetConnectorLabelName() == moText("EMGCH3")) {
                    pOutEmgCh[2] = pOutlet;
                }
                if (pOutlet->GetConnectorLabelName() == moText("EMGCH4")) {
                    pOutEmgCh[3] = pOutlet;
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
                            int p_MoldeoId,
                            bool receive_events  ) {
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


    //if (debug_is_on) MODebug2->Message( moText( "Messages.Count():" ) + IntToStr( Messages.Count() )  ) ;

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

                MODebug2->Push( moText( " > Message:" ) + DataCode.Text() + moText( " Arguments:" ) + IntToStr(message.Count()-1) + moText(" > ") + argumentsText );

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
            if (debug_is_on) MODebug2->Push( moText( " > Message Beat Freq: " )
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
            if (debug_is_on) MODebug2->Push( moText( " > Message Beat High Freq: " )
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
            if (debug_is_on) MODebug2->Push( moText( " > Message Beat Medium Freq: " )
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
            if (debug_is_on) MODebug2->Push( moText( " > Message Beat Low Freq: " )
                                            + FloatToStr(Freq.Double())
                                            + moText(" Val:")
                                            + FloatToStr(Value.Double()) );
        }

        if (
            DataCode.Text() == moText("ANDIAMO")
            && message.Count()>=2
            ) {
            moData Cx = message.Get(1);
            moData Cy = message.Get(2);
            if (pOutAndiamoX) {
                pOutAndiamoX->GetData()->SetDouble(Cx.Double());
                pOutAndiamoX->Update();
            }
            if (pOutAndiamoY) {
                pOutAndiamoY->GetData()->SetDouble(Cy.Double());
                pOutAndiamoY->Update();
            }
            if (debug_is_on) MODebug2->Push( moText( " > Message Andiamo X,Y, Cx: " )
                                            + FloatToStr(Cx.Double())
                                            + moText(", Cy: ")
                                            + FloatToStr(Cy.Double()) );
        }

        if ( DataCode.Text() == moText("MOLDEO") && message.Count()>=8 && receive_events  ) {
          ///atencion el primer dato es "EVENT"
          moText DataEvent = message.Get(1).ToText();
          if (DataEvent==moText("EVENT")) {
            DataCode = message.Get(1);
            int did = message.Get(2).Int();
            int cid = message.Get(3).Int();
            int val0 = message.Get(4).Int();
            int val1 = message.Get(5).Int();
            int val2 = message.Get(6).Int();
            int val3 = message.Get(7).Int();


            if (debug_is_on) MODebug2->Message( moText("Receiving events: ")
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

            if (pEvents) pEvents->Add( did, cid, val0, val1, val2, val3 );
          }
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

            if (message.Count()>=4 && pOutOrientationX && pOutOrientationY && pOutOrientationZ ) {

                moData OriX = message.Get(1);
                moData OriY = message.Get(2);
                moData OriZ = message.Get(3);

                pOutOrientationX->GetData()->SetDouble(  OriX.Double() );
                pOutOrientationY->GetData()->SetDouble(  OriY.Double() );
                pOutOrientationZ->GetData()->SetDouble(  OriZ.Double() );
                pOutOrientationX->Update();
                pOutOrientationY->Update();
                pOutOrientationZ->Update();

                //MODebug2->Push(moText("Ori X:") + FloatToStr(pOutOrientationX->GetData()->Double()) );
                //MODebug2->Push(moText("Ori Y:") + FloatToStr(pOutOrientationY->GetData()->Double()) );
                //MODebug2->Push(moText("Ori Z:") + FloatToStr(pOutOrientationZ->GetData()->Double()) );
            }

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

        } else if ( DataCode.Text() == moText("MAGNETO") ) {

            if (message.Count()>=4 && pOutMagnetoX && pOutMagnetoY && pOutMagnetoZ ) {

                moData MagX = message.Get(1);
                moData MagY = message.Get(2);
                moData MagZ = message.Get(3);

                pOutMagnetoX->GetData()->SetDouble(  MagX.Double() );
                pOutMagnetoY->GetData()->SetDouble(  MagY.Double() );
                pOutMagnetoZ->GetData()->SetDouble(  MagZ.Double() );
                pOutMagnetoX->Update();
                pOutMagnetoY->Update();
                pOutMagnetoZ->Update();

                //MODebug2->Push(moText("Ori X:") + FloatToStr(pOutOrientationX->GetData()->Double()) );
                //MODebug2->Push(moText("Ori Y:") + FloatToStr(pOutOrientationY->GetData()->Double()) );
                //MODebug2->Push(moText("Ori Z:") + FloatToStr(pOutOrientationZ->GetData()->Double()) );
            }

        } else if ( DataCode.Text() == moText("COMPASS") ) {
          if (message.Count()>=2 && pOutCompass ) {
            moData Compass = message.Get(1);
            pOutCompass->GetData()->SetDouble(  Compass.Double() );
            pOutCompass->Update();
          }
        }

        if (
            DataCode.Text() == moText("MOLDEO") ) {

            if (debug_is_on)
                MODebug2->Message( moText( "moNetOscIn::moOscPacketListener::Update >  MOLDEO COMMAND received Count:" )+IntToStr(message.Count()) );

            moData DataCode;
            int moldeoc = 0;
            moldeoc++;
            DataCode = message.Get(moldeoc);
            if (debug_is_on)
                MODebug2->Message( moText( "moNetOscIn::moOscPacketListener::Update >  MOLDEO COMMAND next code is:" )+DataCode.Text() );

            if (DataCode.Text()=="MOLDEO") {
                moldeoc++;
                DataCode = message.Get(moldeoc);
            }
            //CHECK IF IT IS JUST DATA (MIDI or OSC)
            if (DataCode.ToText()==moText("DATA")) {
                if (debug_is_on)
                    MODebug2->Message( moText( "moNetOscIn::moOscPacketListener::Update >  its DATA, checking outlets for next text (label)" ));
              //send to oulets every data
              moData DataLabel;
              moData DataValue;
              moldeoc++;
              if (moldeoc<message.Count()) DataLabel = message.Get(moldeoc);
              moldeoc++;
              if (moldeoc<message.Count()) DataValue = message.Get(moldeoc);
              if (debug_is_on)
                MODebug2->Message( moText( "moNetOscIn::moOscPacketListener::Update > " )+ " Label:" + DataLabel.Text() + " Val:" + DataValue.ToText()+" Type:" + DataValue.TypeToText() );
              if (pOutlets) {
                int poi = GetOutletIndex( pOutlets, DataLabel.Text()  );
                if (poi > -1) {
                  moOutlet* pOutlet = pOutlets->Get(poi);
                  if (pOutlet) {
                    if (debug_is_on)
                      MODebug2->Message( moText( "moNetOscIn::moOscPacketListener::Update Outlet > " ) + DataLabel.Text() );

                    (*pOutlet->GetData()) = DataValue;
                      //pOutlet->GetData()SetDouble( DataValue.Double() );

                    pOutlet->Update(true);

                  }
                } else {
                    if (debug_is_on)
                        MODebug2->Error( moText( "moNetOscIn::moOscPacketListener::Update Outlet > Label Not Found. " ) + DataLabel.Text() );
                }
              }
            } else
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


        if (
            DataCode.Text() == moText("EXP")
            ) {

            moText ApiMessage = message.Get(1).ToText();
            moData Value = message.Get(2);
            if (ApiMessage=="BLINK") {
                if (pOutBlinkValue) {

                    pOutBlinkValue->GetData()->SetDouble(Value.Double());
                    pOutBlinkValue->Update();
                }

            } else
            if (ApiMessage=="EYEBROW") {
                if (pOutSurpriseValue) {

                    pOutSurpriseValue->GetData()->SetDouble(Value.Double());
                    pOutSurpriseValue->Update();
                }

            } else
            if (ApiMessage=="FURROW") {
                if (pOutFrownValue) {

                    pOutFrownValue->GetData()->SetDouble(Value.Double());
                    pOutFrownValue->Update();
                }
            } else
            if (ApiMessage=="HORIEYE") {
                if (pOutHoriValue) {

                    pOutHoriValue->GetData()->SetDouble(Value.Double());
                    pOutHoriValue->Update();
                }

            } else
            if (ApiMessage=="VERTIEYE") {
                if (pOutVertiValue) {

                    pOutVertiValue->GetData()->SetDouble(Value.Double());
                    pOutVertiValue->Update();
                }

            }

            if (debug_is_on) MODebug2->Message( moText( " > Message EXP Value: " )
                                                + ApiMessage
                                                + moText(" Val:")
                                                + FloatToStr(Value.Double()) );
        }

        if (message.Count()>=3) {
        if (
             message.Get(1).ToText() == moText("opencv")

            ) {
          //MOTION_DETECTION
          if (debug_is_on)
              MODebug2->Push( " > NetOscIn > OPENCV Received");

          if (message.Count()>=3) {
              for(int m=2;m<message.Count()-1;m++) {
                moData outletName = message.Get(m);
                moData outletValue = message.Get(m+1);
                //check if this outlet exists and update it!!!
                int idx = GetOutletIndex( pOutlets, outletName.Text());
                if (idx>-1) {
                  moOutlet* pOutlet = pOutlets->Get(idx);
                  if (pOutlet) {
                    moData &Data( *pOutlet->GetData() );
                    Data = outletValue;
                    //pOutlet->GetData()->SetInt()
                    pOutlet->Update(true);
                    if (debug_is_on) MODebug2->Push("NetOscIn  > OPENCV, outlet for:" + outletName.Text()+" outletValue:" + outletValue.ToText() );
                  }
                } else {
                 if (debug_is_on) MODebug2->Push("NetOscIn > OPENCV, no outlet for:" + outletName.Text() );
                }
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


        if (
            DataCode.Text() == moText("EMGCH1")
            && message.Count()>=2
            ) {
            moData Value = message.Get(1);
            if (pOutEmgCh[0]) {
                pOutEmgCh[0]->GetData()->SetDouble(Value.Double());
                pOutEmgCh[0]->Update();
            }
            if (debug_is_on) MODebug2->Push( moText( " > Message Emg Ch1: " )
                                            + moText(" Val:")
                                            + FloatToStr(Value.Double()) );
        }
        if (
            DataCode.Text() == moText("EMGCH2")
            && message.Count()>=2
            ) {
            moData Value = message.Get(1);
            if (pOutEmgCh[1]) {
                pOutEmgCh[1]->GetData()->SetDouble(Value.Double());
                pOutEmgCh[1]->Update();
            }
            if (debug_is_on) MODebug2->Push( moText( " > Message Emg Ch2: " )
                                            + moText(" Val:")
                                            + FloatToStr(Value.Double()) );
        }
        if (
            DataCode.Text() == moText("EMGCH3")
            && message.Count()>=2
            ) {
            moData Value = message.Get(1);
            if (pOutEmgCh[2]) {
                pOutEmgCh[2]->GetData()->SetDouble(Value.Double());
                pOutEmgCh[2]->Update();
            }
            if (debug_is_on) MODebug2->Push( moText( " > Message Emg Ch3: " )
                                            + moText(" Val:")
                                            + FloatToStr(Value.Double()) );
        }
        if (
            DataCode.Text() == moText("EMGCH4")
            && message.Count()>=2
            ) {
            moData Value = message.Get(1);
            if (pOutEmgCh[3]) {
                pOutEmgCh[3]->GetData()->SetDouble(Value.Double());
                pOutEmgCh[3]->Update();
            }
            if (debug_is_on) MODebug2->Push( moText( " > Message Emg Ch4: " )
                                            + moText(" Val:")
                                            + FloatToStr(Value.Double()) );
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
      //MODebug2->Message("NetOscIN Messages: " + IntToStr(msgs.Count()));
      /*for(int c=0;c<msgs.Count();c++) {
        MODebug2->Message("NetOscIN Message n°" + IntToStr(c)
        + moText(" ") + msgs.Get(c).Get(0).ToText()
        + moText(" #") +IntToStr(msgs.Get(c).Count()-1));
      }*/
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

#ifdef OSCPACK
void
moOscPacketListener::ProcessMessage( const osc::ReceivedMessage& m,
				const IpEndpointName& remoteEndpoint ) {
#else
int
moOscPacketListener::ProcessMessage(const char *path, const char *types, lo_arg ** argv,
                    int argc, void *data, void *user_data) {
#endif

//cout << "moNetOSCIn >> moOscPacketListener::ProcessMessage receiving" << endl;

moOscPacketListener* self = NULL;
  #ifdef OSCPACK

  self = this;
  moText path = moText( m.AddressPattern() );

  #else
    
  if (user_data==NULL) { 
    cout << "no user data" << endl; 
    return -1;
  }
  
  self = (moOscPacketListener*) user_data;
  
  if (self == NULL) {
    return 0;
  }  

  self->MODebug2->Message("moNetOSCIn >> moOscPacketListener::ProcessMessage receiving");

  self->MODebug2->Message("moNetOSCIn >> moOscPacketListener::ProcessMessage Lock");
  self->m_Semaphore.Lock();

  self->MODebug2->Message("moNetOSCIn >> moOscPacketListener::ProcessMessage Unlock");
  self->m_Semaphore.Unlock();

  return 0;

  #endif

        //cout << "blocking" << endl;
        self->m_Semaphore.Lock();
        moDataMessage message;

        moData  data0;
        moData  data1;

        moText addresspath = path;
        moText moldeopath = path;
        moldeopath.SubText(0,6);
        if (self->debug_is_on) {
          self->MODebug2->Message(addresspath);
        }
        //cout << "addresspath:" << "[" << addresspath << "]" << endl;
#ifdef OSCPACK
         addresspath = moText( m.AddressPattern() );
#endif
        try {
            //moAbstract::MODebug2->Push(moText("N: ")+IntToStr(m.ArgumentCount()));

            /*AND OSC*/
            /// El primer dato corresponderá a una palabra que represente la secuencia de datos.

            if (
                    addresspath == moText("/1/xy") //andOSC
                || addresspath == moText("/touch") //OSCTouch i = finger_id, f: x f: y f:size

                ) {

                data0 = moData( moText( "POSITION" ) );
                message.Add( data0 );
            } else if (
                    addresspath == moText("/emg/emg/ch1") //LEICI
            ) {
                data0 = moData( moText( "EMGCH1" ) );
                message.Add( data0 );
            } else if (
                    addresspath == moText("/emg/emg/ch2") //LEICI
            ) {
                data0 = moData( moText( "EMGCH2" ) );
                message.Add( data0 );
            } else if (
                    addresspath == moText("/emg/emg/ch3") //LEICI
            ) {
                data0 = moData( moText( "EMGCH3" ) );
                message.Add( data0 );
            } else if (
                    addresspath == moText("/emg/emg/ch4") //LEICI
            ) {
                data0 = moData( moText( "EMGCH4" ) );
                message.Add( data0 );
            } else if (
                    addresspath == moText("/spec") ) {
                data0 = moData( moText( "SPEC" ) );
                message.Add( data0 );
            } else if ( addresspath == moText("/gyrosc/comp")) {
                data0 = moData( moText( "COMPASS" ) );
                message.Add( data0 );
            } else if ( addresspath == moText("/mag")) {
              data0 = moData( moText( "MAGNETO" ) );
              message.Add( data0 );
            } else if ( addresspath == moText("/gyrosc/mag") ) {
              data0 = moData( moText( "MAGNETO" ) );
              message.Add( data0 );
            } else if ( addresspath == moText("/ori")
            || addresspath == moText("/gir")
            || addresspath == moText("/gyrosc/gyro") ) {
                data0 = moData( moText( "ORIENTATION" ) );
                message.Add( data0 );
            } else if ( addresspath == moText("/acc")
            || addresspath == moText("/gyrosc/accel")
            || addresspath == moText("/accxyz") ) {
                data0 = moData( moText( "ACCELERATION" ) );
                message.Add( data0 );
            } else/*AnalyseSOundMonar*/
            if ( addresspath == moText("/beat") ) {
                data0 = moData( moText( "BEAT" ) );
                message.Add( data0 );
            } else if ( addresspath == moText("/beathigh") ) {
                data0 = moData( moText( "BEATHIGH" ) );
                message.Add( data0 );
            } else if ( addresspath == moText("/beatmedium") ) {
                data0 = moData( moText( "BEATMEDIUM" ) );
                message.Add( data0 );
            } else if ( addresspath == moText("/beatlow") ) {
                data0 = moData( moText( "BEATLOW" ) );
                message.Add( data0 );
            } else if ( addresspath == moText("/andiamo") ) {
                data0 = moData( moText( "ANDIAMO" ) );
                message.Add( data0 );
            } else if ( addresspath == moText("/opencv") ) {
                data0 = moData( moText( "OPENCV" ) );
                message.Add( data0 );
            } else if ( addresspath == moText("/EXP/BLINK") ) {
                if (self->debug_is_on) MODebug2->Message( moText(" ============/EXP/BLINK") );
                data0 = moData( moText( "EXP" ) );
                data1 =  moData( moText( "BLINK" ) );
                message.Add( data0 );
                message.Add( data1 );
            } else if ( addresspath == moText("/EXP/HORIEYE") ) {
                if (self->debug_is_on) MODebug2->Message( moText(" ============/EXP/HORIEYE") );
                data0 = moData( moText( "EXP" ) );
                data1 =  moData( moText( "HORIEYE" ) );
                message.Add( data0 );
                message.Add( data1 );
            } else if ( addresspath == moText("/EXP/VERTIEYE") ) {
                if (self->debug_is_on) MODebug2->Message( moText(" ============/EXP/VERTIEYE") );
                data0 = moData( moText( "EXP" ) );
                data1 =  moData( moText( "VERTIEYE" ) );
                message.Add( data0 );
                message.Add( data1 );
            } else if ( addresspath == moText("/EXP/EYEBROW") ) {
                if (self->debug_is_on) MODebug2->Message( moText(" ============/EXP/EYEBROW") );
                data0 = moData( moText( "EXP" ) );
                data1 =  moData( moText( "EYEBROW" ) );
                message.Add( data0 );
                message.Add( data1 );
            } else if ( addresspath == moText("/EXP/FURROW") ) {
                if (self->debug_is_on) MODebug2->Message( moText(" ============/EXP/FURROW") );
                data0 = moData( moText( "EXP" ) );
                data1 =  moData( moText( "FURROW" ) );
                message.Add( data0 );
                message.Add( data1 );
            } else if ( moldeopath == moText("/moldeo") ) {
                //cout << "addresspath MOLDEO:" << endl;
                data0 = moData( moText( "MOLDEO" ) );
                message.Add( data0 );
            } else {
                data0 = moData( addresspath );
                message.Add( data0 );
            }
#ifdef OSCPACK
            osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();

            for(int i = 0; i<m.ArgumentCount(); i++) {

                const osc::ReceivedMessageArgument& rec((*arg));
                char tt = rec.TypeTag();
#else

						/**/
            int i = 0;
						int imax = 0;
						imax = argc;
            cout << "check argc:" << argc << " imax: " << imax << endl;

            for ( i = 0; i < imax; i++) {
            	cout << "arg i:" << i << " imax: " << imax << " type: " << types[i] << " /argc:" << argc << endl;
							if (i>=imax) {
								cout << "continue i: " << i << " superior o igual a [imax]:" << imax << endl;
								continue;
							}
            //lo_arg_pp((lo_type)types[i], argv[i]);
            //cout << endl;
              lo_type tt = (lo_type)types[i];
              cout << "check lo_type tt:" << tt << endl;

            //cout << endl;

#endif
                moData  data;
                switch(tt) {
                #ifdef OSCPACK
                //base.Copy( moData( (int)rec.AsBool() ) );
                    case osc::TRUE_TYPE_TAG:
                    case osc::FALSE_TYPE_TAG:
                      data = moData( (int)rec.AsBool() );
                      break;

                    case osc::NIL_TYPE_TAG:
                    case osc::INFINITUM_TYPE_TAG:
                    case osc::INT32_TYPE_TAG:
                      data.Copy( moData( (int)rec.AsInt32() ) );
                      break;

                    case osc::CHAR_TYPE_TAG:
                      data.Copy( moData( rec.AsChar() ) );
                      break;

                    case osc::RGBA_COLOR_TYPE_TAG:
                      {
                      int rgba = rec.AsRgbaColor();
                      int r = rgba >> 24;
                      int g = (rgba & 0x00FFFFFF) >> 16;
                      int b = (rgba & 0x0000FFFF) >> 8;
                      int a = (rgba & 0x000000FF);
                      data = moData(rgba);
                      }
                      break;

                    case osc::MIDI_MESSAGE_TYPE_TAG:
                      data = moData( (int)rec.AsMidiMessage() );
                      break;

                    case osc::INT64_TYPE_TAG:
                      data = moData( rec.AsInt64());
                      break;

                    case osc::TIME_TAG_TYPE_TAG:
                      data = moData( (MOlonglong)rec.AsTimeTag());
                      break;

                    case osc::FLOAT_TYPE_TAG:
                      data = moData( rec.AsFloat() );
                      break;
                    case osc::DOUBLE_TYPE_TAG:
                      data = moData( rec.AsDouble() );
                      break;
                    case osc::STRING_TYPE_TAG:
                      data = moData( moText( rec.AsString()) );
                      break;

                    case osc::SYMBOL_TYPE_TAG:
                      data = moData( moText(rec.AsSymbol()) );
                      break;

                    case osc::BLOB_TYPE_TAG:
                    {
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
                    }
                      break;
                  #else
                   case LO_FALSE:
                   case LO_TRUE:
									 	 //cout << argv << endl;
										 if (argv && argv[i]) {
											 self->MODebug2->Message( moText("argv[i] ") + IntToStr((long)argv[i]) );
											 data = moData( (int)argv[i]->i );
										 } else {
											 self->MODebug2->Message( moText("argv[i] ") + IntToStr((long)argv[i]) );
										 }
                      break;
                   case LO_INT32:
									 		if (argv && argv[i]) {
												self->MODebug2->Message( moText("argv[i] ") + IntToStr((long)argv[i]) );
                      	data = moData( (int)argv[i]->i32 );
											}
                      break;

                   case LO_INT64:
									 		if (argv && argv[i]) {
												self->MODebug2->Message( moText("argv[i] ") + IntToStr((long)argv[i]) );
                      	data = moData( (int)argv[i]->i64 );
											}
                      break;

                   case LO_DOUBLE:
									 		//cout << argv << endl;
											if (argv && argv[i]) {
												self->MODebug2->Message( moText("argv[i] ") + IntToStr((long)argv[i]) );
												data = moData( (double)argv[i]->d );
											} else {
												self->MODebug2->Message( moText("argv[i] ") + IntToStr((long)argv[i]) );
											}
                      break;
                   case LO_FLOAT:
									 		if (argv && argv[i]) {
												self->MODebug2->Message( moText("argv[i] ") + IntToStr((long)argv[i]) );
                      	data = moData( (float)argv[i]->f );
											}
                      break;
                    case LO_STRING:
											if (argv && argv[i]) {
												self->MODebug2->Message( moText("argv[i] ") + IntToStr((long)argv[i]) );
												//cout << "string:" << argv[i]->s << endl;
                      	//data = moData( moText((char*)&argv[i]->s) );
											} else {
												self->MODebug2->Message( moText("argv[i] ") + IntToStr((long)argv[i]) );
											}
                      break;

                  #endif
                }
                if (self->debug_is_on) {
                  self->MODebug2->Message( moText(" > Data type:") + data.TypeToText()+ moText(": ") + data.ToText() );
                }
                #ifdef OSCPACK
                (arg++);
                #endif
								if (data.Type()!=MO_DATA_UNDEFINED) {
										message.Add( data );
								}


            }
        }
        #ifdef OSCPACK
        catch( osc::Exception& e ){
        std::cout << "error while parsing message: "
                << addresspath << ": " << e.what() << "\n";
        #else
        catch(...) {
        #endif
            // any parsing errors such as unexpected argument types, or
            // missing arguments get thrown as exceptions.


        }


        self->Messages.Add(message);
        self->m_Semaphore.Unlock();
}

#ifndef OSCPACK
void error(int num, const char *msg, const char *path)
{
    cout << "liblo server error " << num << " in path " << path << ": " << msg << endl;
    //fflush(stdout);
}

/* catch any incoming messages and display them. returning 1 means that the
 * message has not been fully handled and the server should try other methods */
 /**
int generic_handler(const char *path, const char *types, lo_arg ** argv,
                    int argc, void *data, void *user_data)
{
    int i;

    cout << "path: " << path << endl;
    for (i = 0; i < argc; i++) {
        cout << "arg i:" << i << " type: " << types[i] << endl;
        lo_arg_pp((lo_type)types[i], argv[i]);
        cout << endl;
    }
    cout << endl;
    //fflush(stdout);

    return 1;
}
*/
#endif

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
    int nport=0;

    for(i = 0; i < n_hosts; i++)
    {
		host_name.Set(i, m_Config.GetParam(n).GetValue(i).GetSubValue(0).Text());

		nport = m_Config.GetParam(n).GetValue(i).GetSubValue(1).Int();

		MODebug2->Message(moText(" NETOSCIN hosts: HOST NAME:")
                                            + (moText)host_name[i]
                                            + moText(" PORT:")
                                            + IntToStr(nport)
                 );

		if (nport==0) {
		    //nport = m_Config.Int( moText("port") );
		    nport = m_Config.Int( moR(NETOSCIN_PORT) );
            MODebug2->Message(moText(" NETOSCIN port 0: trying default port parameter")
                                            + moText(" PORT:")
                                            + IntToStr(nport)
                 );

		}
		host_port.Set(i, nport);
	}
	moText tport = IntToStr(nport);
/**

    try {
      MODebug2->Message("moNetOscIn::Init > creating srver at port: " + IntToStr(nport));
      //lo_server st = lo_server_new( tport, error);
      lo_server_thread st = lo_server_thread_new( tport, error);

      if (!st)
        MODebug2->Error("moNetOscIn::Init > error starting linux (liblo).");
      else
        MODebug2->Message("moNetOscIn::Init > server created at port: " +  IntToStr(nport));
      //lo_server_thread_add_method(st, NULL, NULL, generic_handler, NULL);
      lo_server_thread_add_method(st, NULL, NULL, generic_handler, NULL);

      lo_server_thread_start(st);
      //lo_server_wait();

    } catch(...) {
      MODebug2->Error("moNetOscIn::Init > error starting server (liblo).");
    }
*/

	for(i = 0; i < n_hosts; i++)
	{
		moOscPacketListener*    pListener = NULL;

		pListener = new moOscPacketListener();
        if (pListener) {

            #ifdef OSCPACK
            UdpListeningReceiveSocket*  socket = NULL;
            #else
            lo_server_thread socket = NULL;
            #endif


            try {
            #ifdef OSCPACK
                socket = new UdpListeningReceiveSocket(
                IpEndpointName( IpEndpointName::ANY_ADDRESS, host_port[i] ),
                pListener );
            #else
                socket = lo_server_thread_new( tport, error);
            #endif
            } catch (std::exception &e) {
                MODebug2->Error(moText("could not bind to UDP port "));
                socket = NULL;
            }

            if (socket!=NULL) {
                #ifdef OSCPACK
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
                #else
                pListener->Set(socket);
                lo_server_thread_add_method( socket, NULL, NULL, moOscPacketListener::ProcessMessage, pListener);
                lo_server_thread_start(socket);
                MODebug2->Message( moText("NetOSCIn listening to OSC messages on UDP port "));
                #endif
            }
            if (socket) {
                m_OscPacketListeners.Add( pListener );
                #ifdef OSCPACK
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
                #endif
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
                                        GetMobDefinition().GetMoldeoId(),
                                        m_ReceiveEvents  );
            //if (nnn>0 && debug_is_on) MODebug2->Push( moText("Updating outlets with messages: ") + IntToStr(nnn) );
        }

	}

    if (m_ReceiveEvents) {
        if (m_pEvents) {

            //MODebug2->Push( moText("Checking events "));
/*
            for(int i = 0; i<m_pEvents->GetMessages().Count();  i++) {



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
