/*******************************************************************************

                              moMidi.cpp

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

#include "moMidi.h"
#include "moDebugManager.h"
#include <errno.h>

#ifdef WIN32
#include <conio.h>
#endif

//========================
//  Factory
//========================

moMidiFactory *m_MidiFactory = NULL;

MO_PLG_API moIODeviceFactory* CreateIODeviceFactory(){
	if (m_MidiFactory==NULL)
		m_MidiFactory = new moMidiFactory();
	return (moIODeviceFactory*) m_MidiFactory;
}

MO_PLG_API void DestroyIODeviceFactory(){
	delete m_MidiFactory;
	m_MidiFactory = NULL;
}

moIODevice*  moMidiFactory::Create() {
	return new moMidi();
}

void moMidiFactory::Destroy(moIODevice* fx) {
	delete fx;
}

//========================
//  class
//========================


		moMidiData::moMidiData() {
		    	m_Type = MOMIDI_ROTARY;
                m_Channel = 0;
                m_CC = 0;
                m_Val = 0;
		}

		moMidiData::~moMidiData() {

        }

moMidiData& moMidiData::operator =( const moMidiData& mididata)
{
	m_Type = mididata.m_Type;
	m_Channel = mididata.m_Channel;
	m_CC = mididata.m_CC;
	m_Val = mididata.m_Val;
    return *this;
}

void
moMidiData::Copy( const moMidiData& mididata ) {
    (*this) = mididata;
}

moMidiDevice::moMidiDevice() {
	SysXFlag = 0;
	m_DeviceId = -1;
	m_bInit = false;
	m_bActive = false;
}


moMidiDevice::~moMidiDevice() {
	m_MidiDatas.Empty();
}

void moMidiDevice::PrintMidiInErrorMsg(unsigned long err)
{

}

MOboolean
moMidiDevice::Init() {
	return true;
}

MOboolean
moMidiDevice::Finish() {
	return true;
}

MOboolean
moMidiDevice::Init( moText devicetext ) {

	SetName(devicetext);
    m_bInit = false;

#ifdef WIN32

/**
	MIDIINCAPS     moc;
	unsigned long   iNumDevs, i;

	iNumDevs = midiInGetNumDevs();

	if (iNumDevs==0) {
	    MODebug2->Message( moText("ERROR! NO MIDI DEVICES FOUND"));
    }

	// Go through all of those devices, displaying their names
	for (i = 0; i < iNumDevs; i++)
	{
		// Get info about the next device
		if (!midiInGetDevCaps(i, &moc, sizeof(MIDIINCAPS)))
		{

            //CString mycstring(moc.szPname);
            //char* nam = mycstringa.GetBuffer( mycstring.GetLength());
			// Display its Device ID and name

			char* nam = "xxsss";
			MODebug2->Message( moText("Device ID #")
                     + IntToStr(i) + moText(":")
                     + moText(nam) );
            #ifdef UNICODE
                std::wstring wc( 1024, L'#' );
                mbstowcs( &wc[0], devicetext, 1024 );
                if ( !m_stricmp( moc.szPname, (wchar_t*)wc.c_str() ) ) {
                    m_DeviceId = i;
                    break;
                }
            #else
                if ( !m_stricmp( moc.szPname, devicetext) ) {
                    m_DeviceId = i;
                    break;
                }
            #endif
		}
	}


	HMIDIIN			handle;
	MIDIHDR			midiHdr;
	unsigned long	err;

	if (m_DeviceId!=-1) {
		if (!(err = midiInOpen(&handle, m_DeviceId, (DWORD)midiCallback, (DWORD)this, CALLBACK_FUNCTION)))
		{
			// Store pointer to our input buffer for System Exclusive messages in MIDIHDR
			midiHdr.lpData = (LPSTR)&SysXBuffer[0];

			// Store its size in the MIDIHDR
			midiHdr.dwBufferLength = sizeof(SysXBuffer);

			// Flags must be set to 0
			midiHdr.dwFlags = 0;

			// Prepare the buffer and MIDIHDR
			err = midiInPrepareHeader(handle, &midiHdr, sizeof(MIDIHDR));
			if (!err)
			{
				// Queue MIDI input buffer
				err = midiInAddBuffer(handle, &midiHdr, sizeof(MIDIHDR));
				if (!err)
				{
					// Start recording Midi
					err = midiInStart(handle);
					m_bInit = true;
					return true;

				}
			}

		}
		else
		{
			printf("Error opening the default MIDI In Device!\r\n");
			PrintMidiInErrorMsg(err);
			return false;
		}
	}
*/
#else

#endif
    stream = NULL;
    PmDeviceID pid = (int)Pm_GetDefaultInputDeviceID();
    const PmDeviceInfo *pinfo;
    pinfo = Pm_GetDeviceInfo(pid);

    while( pinfo!=NULL ) {

        MODebug2->Message(
                            moText("PmDeviceInfo: id:") + IntToStr(pid)
                            );
        MODebug2->Message( " name:" + moText( (char*)pinfo->name ) );
        //moDebugManager::Message( " devicetext:" + devicetext );
        MODebug2->Message( " interf:" + moText( (char*)pinfo->interf ) );
        MODebug2->Message( " input:" + IntToStr( pinfo->input ) );
        MODebug2->Message( " output:" + IntToStr( pinfo->output ) );

      if (pinfo->name) {
            if ( moText(pinfo->name)==devicetext && pinfo->input==1) {
                m_DeviceId = (int) pid;
                Pm_OpenInput(&stream, pid, NULL, 128, NULL, NULL);
                m_bInit =  true;
                return m_bInit;
            }
        }

        pid++;
        try {
            pinfo = Pm_GetDeviceInfo(pid);
        } catch(...) {

        }

        //pid++;
        //pinfo = *Pm_GetDeviceInfo(pid);
    }



		return m_bInit;
}


void
moMidiDevice::NewData( moMidiData p_mididata ) {

	//m_lock.Lock();
	/*if (m_DataMessage.Count()>127) m_DataMessage.Empty();
	//p_mididata.m_CC = 0;
	moData pData;
	pData.SetInt( p_mididata.m_Val );
	m_DataMessage.Add( pData );
	*/

	if (m_MidiDatas.Count()>127) m_MidiDatas.Empty();
	m_MidiDatas.Add(p_mididata);



	//m_lock.Unlock();


}

void
moMidiDevice::Update(moEventList *Events ) {

	MOint i;
	moMidiData	mididata;

	//m_lock.Lock();
	m_MidiDatas.Empty();

    int c = Pm_Read( this->stream, &buffer[0], 10 );
    if (c>0) {

        for(int cc=0; cc<c;cc++) {
            PmMessage msg = buffer[cc].message;
            MODebug2->Message("moMidiDevice::Update > Read " + IntToStr(cc)+"/"+IntToStr(c) +" message: status: " + IntToStr(Pm_MessageStatus(msg)) + " data1: "+ IntToStr(Pm_MessageData1(msg))+ " data2: " + IntToStr(Pm_MessageData2(msg)) );

            mididata.m_Channel = 0;
            mididata.m_Type = MOMIDI_FADER;
            mididata.m_CC = Pm_MessageData1(msg);
            mididata.m_Val = Pm_MessageData2(msg);
            m_MidiDatas.Add( mididata );
        }

    }



	for( i=0; i < m_MidiDatas.Count(); i++ ) {

		mididata = m_MidiDatas.Get( i );

		MODebug2->Push(moText("MIDI Data CC:") + IntToStr(mididata.m_CC) + moText(" val:") + IntToStr(mididata.m_Val) );

		Events->Add( MO_IODEVICE_MIDI, (MOint)(mididata.m_Type), mididata.m_Channel, mididata.m_CC, mididata.m_Val );

	}
	//m_MidiDatas.Empty();


	//mididata = m_MidiDatas.Get( m_MidiDatas.Count()-1 );
	//moData pData = m_DataMessage.Get(m_DataMessage.Count()-1);
	//MODebug2->Push(moText("MIDI n:") + IntToStr(m_DataMessage.Count()) + moText(" code:") + IntToStr(pData.Int()) );
    //m_DataMessage.Empty();

	//m_lock.Unlock();

}

//=============================================================================



//=============================================================================

moMidi::moMidi() {
	SetName("midi");
}

moMidi::~moMidi() {
	Finish();
}

MOboolean
moMidi::Init() {

	moText conf;
	MOint i;

	// Loading config file.
	//levantamos el config del keyboard
  if (moMoldeoObject::Init()) {
    moMoldeoObject::CreateConnectors();
  } else return false;

	moDefineParamIndex( MIDI_DEVICE, moText("mididevice") );


	mididevices = m_Config.GetParamIndex("mididevice");

	MOint nvalues = m_Config.GetValuesCount( mididevices );
	m_Config.SetCurrentParamIndex( mididevices );

	/*
        MO_MIDI_SYTEM_LABELNAME	0
        MO_MIDI_SYSTEM_ON 1
	*/
	for( int i = 0; i < nvalues; i++) {

		m_Config.SetCurrentValueIndex( mididevices, i );

		moMidiDevicePtr pDevice = NULL;
		pDevice = new moMidiDevice();

		if (pDevice!=NULL) {
			pDevice->MODebug = MODebug;
			moText MidiDeviceCode = m_Config.GetParam().GetValue().GetSubValue(MO_MIDI_SYTEM_LABELNAME).Text();
			if ( pDevice->Init( MidiDeviceCode ) ) {
				pDevice->SetActive( m_Config.GetParam().GetValue().GetSubValue(MO_MIDI_SYSTEM_ON).Int() );
				if (pDevice->IsActive()) {
                    MODebug2->Message( moText("Midi Device is ACTIVE!"));
				}
			} else {
				MODebug2->Error( moText("Midi Device not found: ") + (moText)MidiDeviceCode );
			}
		}

		m_MidiDevices.Add( pDevice );

	}



	//levantamos los codes definidos
	MOint coparam = m_Config.GetParamIndex("code");
	m_Config.SetCurrentParamIndex(coparam);
	MOint ncodes = m_Config.GetValuesCount(coparam);

	m_Config.FirstValue();

	printf("\nMIDI: ncodes:%i\n",ncodes);

	for( i = 0; i < ncodes; i++) {
		moMidiDataCode mididatacode;
		mididatacode.strcode = m_Config.GetParam().GetValue().GetSubValue(0).Text();
		mididatacode.devcode = i;
		mididatacode.mididata.m_Type = (moEncoderType)m_Config.GetParam().GetValue().GetSubValue(1).Int();
		mididatacode.mididata.m_Channel = m_Config.GetParam().GetValue().GetSubValue(2).Int();
		mididatacode.mididata.m_CC = m_Config.GetParam().GetValue().GetSubValue(3).Int();
		mididatacode.mididata.m_Val = 0;
		mididatacode.state = MO_FALSE;
/*

		Codes[i].value = MO_FALSE;
		*/
		m_Codes.Add( mididatacode );
		m_Config.NextValue();
	}
	return true;
}


void
moMidi::SetValue( MOdevcode cd, MOint vl ) {
    m_Codes[cd].mididata.m_Val = (int)vl;
}

void
moMidi::SetValue( MOdevcode cd, MOfloat vl ) {
    m_Codes[cd].mididata.m_Val = (int)vl;
}


MOswitch
moMidi::SetStatus(MOdevcode devcode, MOswitch state) {

    m_Codes[devcode].state = state;

	return true;
}


//notar que el devcode es a su vez el indice dentro del array de Codes
MOswitch
moMidi::GetStatus(MOdevcode devcode) {
	return(m_Codes[devcode].state);
}

MOint
moMidi::GetValue( MOdevcode devcode) {
	return(m_Codes[devcode].mididata.m_Val);
}


MOdevcode
moMidi::GetCode(moText strcod) {

	MOint codi,i;

	codi = m_Config.GetParamIndex("code");
	m_Config.SetCurrentParamIndex(codi);
	m_Config.FirstValue();

	for( i = 0; i < m_Config.GetValuesCount(codi); i++) {
		if(!stricmp( strcod, m_Config.GetParam().GetValue().GetSubValue(MOMIDI_CFG_STRCOD).Text() ))
			return(i);
		m_Config.NextValue();
	}

	return(-1);//error, no encontro
}

MOpointer
moMidi::GetPointer(MOdevcode devcode ) {

	return NULL;
}

void
moMidi::Update(moEventList *Events) {
	MOuint i;
	moEvent *actual,*tmp;



	actual = Events->First;
	//recorremos todos los events y parseamos el resultado
	//borrando aquellos que ya usamos
	while(actual!=NULL) {
		//solo nos interesan los del midi q nosotros mismos generamos, para destruirlos
		if(actual->deviceid == MO_IODEVICE_MIDI) {
			//ya usado lo borramos de la lista
			tmp = actual->next;
			Events->Delete(actual);
			actual = tmp;
		} else actual = actual->next;//no es nuestro pasamos al next
	}


    ///ACTUALIZAMOS CADA DISPOSITIVO
	for( i = 0; i < m_MidiDevices.Count(); i++ ) {
        //MODebug2->Message("Update devices");
		moMidiDevicePtr MidiDevPtr;
		MidiDevPtr = m_MidiDevices.Get(i);

		if (MidiDevPtr!=NULL) {
			if (MidiDevPtr->IsInit()) {
				MidiDevPtr->Update( Events );

				const moMidiDatas& mdatas( MidiDevPtr->GetMidiDatas() );
				for(int md=0; md<mdatas.Count(); md++ ) {
                    const moMidiData& mdata( mdatas.Get(md) );
                    int ccode = mdata.m_CC;
                    moText ccodetxt = moText("C") + IntToStr( ccode, 2);
                    int idx = GetOutletIndex( ccodetxt );
                    moOutlet* pOutCCode = NULL;
                    if( idx>-1) {
                        MODebug2->Message( ccodetxt+ " founded! Udpating value:" + IntToStr(mdata.m_Val)+ "idx:" + IntToStr(idx)  );
                        pOutCCode = m_Outlets[idx];
                        if (pOutCCode) {
                            MODebug2->Message( "Updating code" );
                            pOutCCode->GetData()->SetDouble( (double)mdata.m_Val );
                            pOutCCode->Update();
                        }
                    } else {
                        // TODO: autocreate outlets!!
                        //pOutCCode =r new moOutlet();
                    }
                }
            }
		}


	}

	//m_Codes

	actual = Events->First;
	//recorremos todos los events y parseamos el resultado
	//borrando aquellos que ya usamos
	MOint tempval;
	while(actual!=NULL) {
		//solo nos interesan los del midi q nosotros mismos generamos, para destruirlos
		if(actual->deviceid == MO_IODEVICE_MIDI) {

		    ///actual->reservedvalue1 corresponds to CC midi code : it works as a n index in m_Codes (has to be defined in param "code" in the config file...
		    ///actual->reservedvalue2 corresponds to VAL

			moMidiDataCode pcode = m_Codes.Get( actual->reservedvalue1 );

			//tempval = actual->reservedvalue2;
			//calculamos la diferencia y modificamos el valor del evento...
			//actual->reservedvalue2 = (tempval - pcode.mididata.m_Val) * 8;
			//actual->reservedvalue2 = ( tempval - 64)*4;
			//guardamos el valor actual para calcular la proxima
			pcode.mididata.m_Val = actual->reservedvalue2;
			m_Codes.Set( actual->reservedvalue1, pcode );
			if (actual->reservedvalue2>0) SetStatus( actual->reservedvalue1, MO_ON );
			else  SetStatus( actual->reservedvalue1, MO_OFF );

			actual = actual->next;
		} else actual = actual->next;//no es nuestro pasamos al next
	}


    moMoldeoObject::Update(Events);


}

moConfigDefinition *
moMidi::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moIODevice::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("mididevice"), MO_PARAM_TEXT, MIDI_DEVICE, moValue( "nanoKONTROL MIDI 1", "TXT") );
	return p_configdefinition;
}


MOboolean
moMidi::Finish() {
	return true;
}
