/*******************************************************************************

                              moOla.cpp

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

#include "moOla.h"
#include <errno.h>

#ifdef WIN32
#include <conio.h>
#endif

//========================
//  Factory
//========================

moOlaFactory *m_OlaFactory = NULL;

MO_PLG_API moIODeviceFactory* CreateIODeviceFactory(){
	if (m_OlaFactory==NULL)
		m_OlaFactory = new moOlaFactory();
	return (moIODeviceFactory*) m_OlaFactory;
}

MO_PLG_API void DestroyIODeviceFactory(){
	delete m_OlaFactory;
	m_OlaFactory = NULL;
}

moIODevice*  moOlaFactory::Create() {
	return new moOla();
}

void moOlaFactory::Destroy(moIODevice* fx) {
	delete fx;
}

//========================
//  class
//========================


		moOlaData::moOlaData() {
		    	m_Type = MOOLA_ROTARY;
                m_Channel = 0;
                m_CC = 0;
                m_Val = 0;

		}

		moOlaData::~moOlaData() {

        }

moOlaData& moOlaData::operator =( const moOlaData& oladata)
{
	m_Type = oladata.m_Type;
	m_Channel = oladata.m_Channel;
	m_CC = oladata.m_CC;
	m_Val = oladata.m_Val;
    return *this;
}

void
moOlaData::Copy( const moOlaData& oladata ) {
    (*this) = oladata;
}

moOlaDevice::moOlaDevice() {
	m_bInit = false;
	m_bActive = false;
}


moOlaDevice::~moOlaDevice() {
	m_OlaDatas.Empty();
}


MOboolean
moOlaDevice::Init() {
	return true;
}

MOboolean
moOlaDevice::Finish() {
	return true;
}

MOboolean
moOlaDevice::Init( moText devicetext ) {

	SetName(devicetext);
    m_bInit = false;

    /** devicetext IP:PORT */
    /** TODO: initialize */
    m_DeviceId = 0;

    return m_bInit;
}


void
moOlaDevice::NewData( moOlaData p_oladata ) {


	if (m_OlaDatas.Count()>127) m_OlaDatas.Empty();
	m_OlaDatas.Add(p_oladata);

}

void
moOlaDevice::Update(moEventList *Events) {

	MOint i;
	moOlaData	oladata;

/*
    for(int cc=0; cc<c;cc++) {
        PmMessage msg = buffer[cc].message;
        moDebugManager::Message("moOlaDevice::Update > Read " + IntToStr(cc)+"/"+IntToStr(c) +" message: status: " + IntToStr(Pm_MessageStatus(msg)) + " data1: "+ IntToStr(Pm_MessageData1(msg))+ " data2: " + IntToStr(Pm_MessageData2(msg)) );

        oladata.m_Channel = 0;
        oladata.m_Type = MOOLA_FADER;
        oladata.m_CC = Pm_MessageData1(msg);
        oladata.m_Val = Pm_MessageData2(msg);
        m_OlaDatas.Add( oladata );
    }
*/


	for( i=0; i < m_OlaDatas.Count(); i++ ) {

		oladata = m_OlaDatas.Get( i );

		MODebug2->Push(moText("OLA Data CC:") + IntToStr(oladata.m_CC) + moText(" val:") + IntToStr(oladata.m_Val) );

		Events->Add( MO_IODEVICE_OLA, (MOint)(oladata.m_Type), oladata.m_Channel, oladata.m_CC, oladata.m_Val );

	}
	m_OlaDatas.Empty();


	//oladata = m_OlaDatas.Get( m_OlaDatas.Count()-1 );
	//moData pData = m_DataMessage.Get(m_DataMessage.Count()-1);
	//MODebug2->Push(moText("OLA n:") + IntToStr(m_DataMessage.Count()) + moText(" code:") + IntToStr(pData.Int()) );
    //m_DataMessage.Empty();

	//m_lock.Unlock();

}

//=============================================================================



//=============================================================================

moOla::moOla() : ola_client((ola::client::StreamingClient::Options())) {
	SetName("ola");
  m_pPixelIndex = NULL;
  m_pPixelIndexX = NULL;
  m_pPixelIndexY = NULL;
  m_pData = NULL;
  m_pOlaTexture = NULL;

}

moOla::~moOla() {
	Finish();
}

void
moOla::TestLeds( int ciclos, int steps, bool debug_on ) {

    int colorv1 = 0;
    int colorv2 = 25;

    for( int ciclo = 0; ciclo<ciclos; ciclo++) {



        for (unsigned int uni = 1; uni < 3; uni++) {
            for (unsigned int cha = 1; cha < 170; cha++) {

                for ( colorv1 = 0; colorv1 < steps; colorv1++) {
                    int red = 255*colorv1/steps;
                    int green = red;
                    int blue = red;
                    buffer.SetChannel( cha*3, red  );
                    buffer.SetChannel( cha*3+1, green  );
                    buffer.SetChannel( cha*3+2, blue );
                    if (!ola_client.SendDmx( uni, buffer ) ) {
                        //cout << "Send DMX failed" << endl;
                        return;
                    }// else cout << "Send DMX ok" << i << endl;
                    if (debug_on) {
                        cout << "Send DMX ok. Cha: " << cha << " val1:" << colorv1 << endl;
                    }
                    usleep(3); // sleep for 25ms between frames.
                }
                for ( colorv2 = steps-1; colorv2 > 0 ; colorv2--) {
                    int red = 255*colorv2/steps;
                    int green = red;
                    int blue = red;
                    buffer.SetChannel( cha*3, red  );
                    buffer.SetChannel( cha*3+1, green  );
                    buffer.SetChannel( cha*3+2, blue  );
                    if (!ola_client.SendDmx( uni, buffer ) ) {
                        //cout << "Send DMX failed" << endl;
                        return;
                    }// else cout << "Send DMX ok" << i << endl;
                    if (debug_on) {
                        cout << "Send DMX ok. Cha: " << cha << " val2:" << colorv2 << endl;
                    }
                    usleep(3); // sleep for 25ms between frames.
                }
                buffer.SetChannel( cha*3, 0  );
                buffer.SetChannel( cha*3+1, 0  );
                buffer.SetChannel( cha*3+2, 0  );
                usleep(2);
            }
        }

    }

}

int nrows = 4;
int ncols = 30;

typedef moInlet* inptr;

MOboolean
moOla::Init() {

	moText conf;
	MOint i;

    int Mid = -1;
    moTexParam tparam = MODefTex2DParams;
    tparam.internal_format = GL_RGB;


    /** OLA texture*/
    Mid = GetResourceManager()->GetTextureMan()->AddTexture( "OLA"+GetLabelName(), 300, 4, tparam );
    if (Mid>-1) {
      m_pOlaTexture = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
      m_pData = new unsigned char [ 300 * 4 * 3 ];///w:300 h:4 rgb:3
      for(int a=0;a<(300*4*3);a++) {
        m_pData[a] = 127;
      }
      if (m_pOlaTexture && m_pData) {
        m_pOlaTexture->BuildFromBuffer( 300, 4, m_pData, GL_RGB, GL_UNSIGNED_BYTE );
      }
    }

    m_pPixelIndex = new moInlet();
    if (m_pPixelIndex) {
      ((moConnector*)m_pPixelIndex)->Init( moText("pixelindex"), m_Inlets.Count(), MO_DATA_NUMBER_LONG );
      m_Inlets.Add(m_pPixelIndex);
    }

    m_pPixelIndexX = new moInlet();
    if (m_pPixelIndexX) {
      ((moConnector*)m_pPixelIndexX)->Init( moText("x"), m_Inlets.Count(), MO_DATA_NUMBER_FLOAT );
      m_Inlets.Add(m_pPixelIndexX);
    }

    m_pPixelIndexY = new moInlet();
    if (m_pPixelIndexY) {
      ((moConnector*)m_pPixelIndexY)->Init( moText("y"), m_Inlets.Count(), MO_DATA_NUMBER_FLOAT );
      m_Inlets.Add(m_pPixelIndexY);
    }

    m_pPixelRow = new inptr [nrows];
    m_pPixelCol = new inptr [ncols];

    for( int nr = 0; nr < nrows ; nr++ ) {
      m_pPixelRow[nr] = new moInlet();
      ((moConnector*)m_pPixelRow[nr])->Init( moText("row")+IntToStr(nr), m_Inlets.Count(), MO_DATA_NUMBER_LONG );
      m_Inlets.Add(m_pPixelRow[nr]);
    }
    for( int nc = 0; nc < ncols ; nc++ ) {
      m_pPixelCol[nc] = new moInlet();
      ((moConnector*)m_pPixelCol[nc])->Init( moText("col")+IntToStr(nc), m_Inlets.Count(), MO_DATA_NUMBER_LONG );
      m_Inlets.Add(m_pPixelCol[nc]);
    }
    //m_pPixelRow[0] = new moInlet();
    /**
    if (m_pPixelIndex) {
      ((moConnector*)m_pPixelIndex)->Init( moText("pixelindex"), m_Inlets.Count(), MO_DATA_NUMBER_LONG );
      m_Inlets.Add(m_pPixelIndex);
    }
    */


	// Loading config file.
	//levantamos el config del keyboard
  if (moMoldeoObject::Init()) {
    moMoldeoObject::CreateConnectors();
  } else return false;

	moDefineParamIndex( OLA_DEVICE, moText("oladevice") );
	moDefineParamIndex( OLA_TEXTURE, moText("texture") );
	moDefineParamIndex( OLA_RED, moText("red") );
	moDefineParamIndex( OLA_GREEN, moText("green") );
	moDefineParamIndex( OLA_BLUE, moText("blue") );
	moDefineParamIndex( OLA_ALPHA, moText("alpha") );
	moDefineParamIndex( OLA_STARTUNIVERSE, moText("startuniverse") );
	moDefineParamIndex( OLA_MODE, moText("mode") );

	oladevices = m_Config.GetParamIndex("oladevice");

	MOint nvalues = m_Config.GetValuesCount( oladevices );
	m_Config.SetCurrentParamIndex( oladevices );


    unsigned int universe = 1; // universe to use for sending data
    // turn on OLA logging
    ola::InitLogging(ola::OLA_LOG_WARN, ola::OLA_LOG_STDERR);

    buffer.Blackout(); // Set all channels to 0
    // Create a new client.

    // Setup the client, this connects to the server
    if (!ola_client.Setup()) {
        std::cerr << "Setup failed" << endl;
        MODebug2->Error("moOla::Init >> Setup failed!");
        return false;
    } else {
        //cout << "Send DMX failed" << endl;
        MODebug2->Message("moOla::Init >> Ola Client Setup.");
    }
    // Send 100 frames to the server. Increment slot (channel) 0 each time a
    // frame is sent.

	/*
        MO_OLA_SYTEM_LABELNAME	0
        MO_OLA_SYSTEM_ON 1
	*/
	/*
	for( int i = 0; i < nvalues; i++) {

		m_Config.SetCurrentValueIndex( oladevices, i );

		moOlaDevicePtr pDevice = NULL;
		pDevice = new moOlaDevice();

		if (pDevice!=NULL) {
			pDevice->MODebug = MODebug;
			moText OlaDeviceCode = m_Config.GetParam().GetValue().GetSubValue(MO_OLA_SYTEM_LABELNAME).Text();
			if ( pDevice->Init( OlaDeviceCode ) ) {
				pDevice->SetActive( m_Config.GetParam().GetValue().GetSubValue(MO_OLA_SYSTEM_ON).Int() );
				if (pDevice->IsActive()) {
                    MODebug2->Message( moText("Ola Device is ACTIVE!"));
				}
			} else {
				MODebug2->Error( moText("Ola Device not found: ") + (moText)OlaDeviceCode );
			}
		}

		m_OlaDevices.Add( pDevice );

	}
	*/



	//levantamos los codes definidos
	MOint coparam = m_Config.GetParamIndex("code");
	m_Config.SetCurrentParamIndex(coparam);
	MOint ncodes = m_Config.GetValuesCount(coparam);

	m_Config.FirstValue();

	printf("\nOLA: ncodes:%i\n",ncodes);

	for( i = 0; i < ncodes; i++) {
		moOlaDataCode oladatacode;
		oladatacode.strcode = m_Config.GetParam().GetValue().GetSubValue(0).Text();
		oladatacode.devcode = i;
		oladatacode.oladata.m_Type = (moEncoderType)m_Config.GetParam().GetValue().GetSubValue(1).Int();
		oladatacode.oladata.m_Channel = m_Config.GetParam().GetValue().GetSubValue(2).Int();
		oladatacode.oladata.m_CC = m_Config.GetParam().GetValue().GetSubValue(3).Int();
		oladatacode.oladata.m_Val = 0;
		oladatacode.state = MO_FALSE;
/*

		Codes[i].value = MO_FALSE;
		*/
		m_Codes.Add( oladatacode );
		m_Config.NextValue();
	}



	return true;
}


void
moOla::SetValue( MOdevcode cd, MOint vl ) {
    m_Codes[cd].oladata.m_Val = (int)vl;
}

void
moOla::SetValue( MOdevcode cd, MOfloat vl ) {
    m_Codes[cd].oladata.m_Val = (int)vl;
}


MOswitch
moOla::SetStatus(MOdevcode devcode, MOswitch state) {

    m_Codes[devcode].state = state;

	return true;
}


//notar que el devcode es a su vez el indice dentro del array de Codes
MOswitch
moOla::GetStatus(MOdevcode devcode) {
	return(m_Codes[devcode].state);
}

MOint
moOla::GetValue( MOdevcode devcode) {
	return(m_Codes[devcode].oladata.m_Val);
}


MOdevcode
moOla::GetCode(moText strcod) {

	MOint codi,i;

	codi = m_Config.GetParamIndex("code");
	m_Config.SetCurrentParamIndex(codi);
	m_Config.FirstValue();

	for( i = 0; i < m_Config.GetValuesCount(codi); i++) {
		if(!stricmp( strcod, m_Config.GetParam().GetValue().GetSubValue(MOOLA_CFG_STRCOD).Text() ))
			return(i);
		m_Config.NextValue();
	}

	return(-1);//error, no encontro
}

MOpointer
moOla::GetPointer(MOdevcode devcode ) {

	return NULL;
}

void
moOla::Update(moEventList *Events) {
	MOuint i;
	moEvent *actual,*tmp;



	actual = Events->First;
	//recorremos todos los events y parseamos el resultado
	//borrando aquellos que ya usamos
	while(actual!=NULL) {
		//solo nos interesan los del ola q nosotros mismos generamos, para destruirlos
		if(actual->deviceid == MO_IODEVICE_OLA) {
			//ya usado lo borramos de la lista
			tmp = actual->next;
			Events->Delete(actual);
			actual = tmp;
		} else actual = actual->next;//no es nuestro pasamos al next
	}


    ///ACTUALIZAMOS CADA DISPOSITIVO
	for( i = 0; i < m_OlaDevices.Count(); i++ ) {
        //MODebug2->Message("Update devices");
		moOlaDevicePtr OlaDevPtr;
		OlaDevPtr = m_OlaDevices.Get(i);
		if (OlaDevPtr!=NULL) {
			if (OlaDevPtr->IsInit()) {
				OlaDevPtr->Update( Events );
			}
		}


	}

	//TestLeds( 1, 100 );

    int colorv1 = 0;
    int colorv2 = 25;


    double red = m_Config.Eval(moR(OLA_RED));
    double green = m_Config.Eval(moR(OLA_GREEN));
    double blue = m_Config.Eval(moR(OLA_BLUE));
    double alpha = m_Config.Eval(moR(OLA_ALPHA));
    //int startuniverse = min( 1, max( 1, m_Config.Int(moR(OLA_STARTUNIVERSE)) ) );
    int startuniverse = m_Config.Int(moR(OLA_STARTUNIVERSE));
    int mode = m_Config.Int(moR(OLA_MODE));
    /**
    red = 0;
    green = 0;
    blue = 0;
    alpha = 0;
*/
    //MODebug2->Message("moOla::Update >" +GetLabelName()+ " StartUniverse:"+IntToStr(startuniverse) );

    long pindex = 0;
    long pindexrgb = 0;
    int pixelperstring = 300;
    unsigned int rgbindexmax = 170;///512 / 3 = 170 -> 170*3 = 510
    /// 170 = floor( 512 / 3 )
    /// last possible channel is: 169*3 = 507,508,509 (cos 510,511,512(doesnt exist!)

    if (mode==1) {
      int TGLId = m_Config.GetGLId( moR(OLA_TEXTURE), this );
      glBindTexture( GL_TEXTURE_2D, TGLId );
      glGetTexImage( GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pData );
    }

    for (unsigned int uni = startuniverse; uni < startuniverse+8; uni++) {
        for (unsigned int cha = 0; cha < rgbindexmax; cha++) {

          int x = (int) ( pindexrgb % pixelperstring );
          int y = (int) ( pindexrgb / pixelperstring );
          int rgbiidx = (y*pixelperstring+x)*3;

          if (mode==0) {

            if (m_pPixelIndex) {
              m_pPixelIndex->GetData()->SetLong(pindexrgb);
              m_pPixelIndex->Update(true);
            }
            int row = (int)  ( pindex / pixelperstring);
            int col = (int)  ( pindex % pixelperstring);
            if (ncols<pixelperstring) {
              col = (int) col / ( pixelperstring / ncols ) ;
            }


            //MODebug2->Message( "pindex:"+IntToStr(pindex)+" row:"+IntToStr(row)+" col:"+IntToStr(col) );

            if (m_pPixelIndexX) {
              m_pPixelIndexX->GetData()->SetDouble(x);
              m_pPixelIndexX->Update(true);
            }

            if (m_pPixelIndexY) {
              m_pPixelIndexY->GetData()->SetDouble(y);
              m_pPixelIndexY->Update(true);
            }


            red = m_Config.Eval(moR(OLA_RED));
            green = m_Config.Eval(moR(OLA_GREEN));
            blue = m_Config.Eval(moR(OLA_BLUE));
            alpha = m_Config.Eval(moR(OLA_ALPHA));

            unsigned char ired = 255*red*alpha;
            unsigned char igreen = 255*green*alpha;
            unsigned char iblue = 255*blue*alpha;

            if (m_pData && 0<=x && x<pixelperstring && 0<=y && y<4) {
              m_pData[ rgbiidx+2 ] = iblue;
              m_pData[ rgbiidx+1 ] = igreen;
              m_pData[ rgbiidx ] = ired;
            }
            if (m_pData) {
              buffer.SetChannel( cha*3, (unsigned char) m_pData[rgbiidx] );
              buffer.SetChannel( cha*3+1, (unsigned char) m_pData[rgbiidx+1] );
              buffer.SetChannel( cha*3+2, (unsigned char) m_pData[rgbiidx+2] );

            }
          } else if (mode==1) {
            if (m_pData) {
              buffer.SetChannel( cha*3, (unsigned char) m_pData[rgbiidx] );
              buffer.SetChannel( cha*3+1, (unsigned char) m_pData[rgbiidx+1] );
              buffer.SetChannel( cha*3+2, (unsigned char) m_pData[rgbiidx+2] );
            }

          }

          pindex++;
          pindexrgb++;

        }


      if (!ola_client.SendDmx( uni, buffer ) ) {
        //MODebug2->Error("Couldnt send buffer");
      }
    }


    if (m_pOlaTexture && m_pData) {
        m_pOlaTexture->BuildFromBuffer( 300, 4, m_pData, GL_RGB, GL_UNSIGNED_BYTE );
    }

    moMoldeoObject::Update(Events);
}


moConfigDefinition *
moOla::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moIODevice::GetDefinition( p_configdefinition );

	p_configdefinition->Add( moText("oladevice"), MO_PARAM_TEXT, OLA_DEVICE, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, OLA_TEXTURE, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("red"), MO_PARAM_FUNCTION, OLA_RED, moValue("0.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("green"), MO_PARAM_FUNCTION, OLA_GREEN, moValue("0.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("blue"), MO_PARAM_FUNCTION, OLA_BLUE, moValue("0.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("alpha"), MO_PARAM_FUNCTION, OLA_ALPHA, moValue("1.0","FUNCTION").Ref() );
	p_configdefinition->Add( moText("startuniverse"), MO_PARAM_NUMERIC, OLA_STARTUNIVERSE, moValue( "1", "NUM").Ref() );
	p_configdefinition->Add( moText("leds"), MO_PARAM_NUMERIC, OLA_LEDS, moValue( "50", "NUM").Ref() );
	p_configdefinition->Add( moText("rgbtype"), MO_PARAM_NUMERIC, OLA_RGBTYPE, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("testmode"), MO_PARAM_NUMERIC, OLA_TESTMODE, moValue( "0", "NUM").Ref(), moText("None,") );
	p_configdefinition->Add( moText("testoffset"), MO_PARAM_NUMERIC, OLA_TESTOFFSET, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("checkserver"), MO_PARAM_NUMERIC, OLA_CHECKSERVER, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("debug"), MO_PARAM_NUMERIC, OLA_MDEBUG, moValue( "0", "NUM").Ref(), moText("YES,NO") );
	p_configdefinition->Add( moText("mode"), MO_PARAM_NUMERIC, OLA_MODE, moValue( "0", "NUM").Ref(), moText("RGBA,TEXTURE") );

	return p_configdefinition;
}


MOboolean
moOla::Finish() {
	return true;
}
