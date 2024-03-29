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
  m_Name = "";
	SysXFlag = 0;
	m_DeviceId = -1;
	m_bInit = false;
	m_bActive = false;
	m_bAllControlsToZero = false;
	m_AllControlsToZeroChannel = 0;
	m_bAllNotesOff = false;
	for(int i=0;i<=16;i++) {
    m_Notes[i].Init( 127, moMidiNote() );
	}
	stream = NULL;
	channel = 0;
	m_bDebugIsOn = false;
}


moMidiDevice::~moMidiDevice() {
	m_MidiDatas.Empty();
}

void moMidiDevice::PrintMidiInErrorMsg(unsigned long err)
{

}

MOboolean
moMidiDevice::Init() {
  stream = NULL;
	return true;
}

MOboolean
moMidiDevice::Finish() {
  if (stream) {
    Pm_Close(stream);
    stream = NULL;
  }
	return true;
}

bool
moMidiDevice::Reinit() {
  Finish();
  return Init(m_Name);
}

MOboolean
moMidiDevice::Init( moText devicetext ) {

	SetName(devicetext);
  m_bInit = false;

#ifdef WIN32

#else

#endif
    if (stream) {
      Pm_Close(stream);
    }
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
                try {
                  Pm_OpenInput(&stream, pid, NULL, 128, NULL, NULL);

                } catch(...) {
                  MODebug2->Error("moMidi::Init(devicetext="+devicetext+") > Pm_OpenInput(pid="+IntToStr(pid)+")");
                }
                m_bInit =  true;
                return m_bInit;
            }
        }

        pid++;
        try {
            pinfo = Pm_GetDeviceInfo(pid);
        } catch(...) {
            MODebug2->Error("moMidi::Init(devicetext="+devicetext+") > Pm_GetDeviceInfo(pid="+IntToStr(pid)+")");
        }

        //pid++;
        //pinfo = *Pm_GetDeviceInfo(pid);
    }

    if (m_DeviceId>0)
      MODebug2->Message("moMidi::Init(devicetext="+devicetext+") > OK.");

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
	try {
	m_MidiDatas.Empty();
    if (this->channel>0) Pm_SetChannelMask( this->stream, Pm_Channel(this->channel-1) );
    int c = Pm_Read( this->stream, &buffer[0], 100 );
    if (c>0) {

        for(int cc=0; cc<c;cc++) {
            PmMessage msg = buffer[cc].message;
            //mididata.m_Channel = this->channel;
            //Pm_Mess
            mididata.m_Status = Pm_MessageStatus(msg);
            int ch = mididata.m_Status & 0x0000000F;
            mididata.m_Channel = ch+1;
            int st = mididata.m_Status;
            st = st >> 4;
            mididata.m_Type = (moEncoderType)st;
            mididata.m_CC = Pm_MessageData1(msg);
            mididata.m_Val = Pm_MessageData2(msg);
            moData oscdata;

            //MODebug2->Message("moMidiDevice::Update > Read " + IntToStr(cc)+"/"+IntToStr(c) +" message: channel: " + IntToStr(mididata.m_Channel) + " type: " + moEncoderTypeStr[mididata.m_Type] + " data1(note/cc): "+ IntToStr(Pm_MessageData1(msg))+ " data2(vel): " + IntToStr(Pm_MessageData2(msg)) );

            m_MidiDatas.Add( mididata );

            TBN.Start();

            /*** CONVERSIONS!!!! TODO: PUT A FLAG SOMEWHERE IN MIDI configuration*/
            /*if (  mididata.m_Type==MOMIDI_CC
                  && (
                    mididata.m_CC==30 ||
                    mididata.m_CC==31 ||
                    mididata.m_CC==40 ||
                    mididata.m_CC==41
                  )
                  && mididata.m_Val==127 ) {
              mididata.m_Type=MOMIDI_NOTEON;
              mididata.m_Val = 64;
            } else
            if ( mididata.m_Type==MOMIDI_CC && (
                    mididata.m_CC==30 ||
                    mididata.m_CC==31 ||
                    mididata.m_CC==40 ||
                    mididata.m_CC==41
                  )
                  && mididata.m_Val==0 ) {
              mididata.m_Type=MOMIDI_NOTEOFF;
              mididata.m_Val = 0;
            }*/
            /***/

            if ( mididata.m_Type==MOMIDI_NOTEON && mididata.m_Val>0 ) {
            //if ( mididata.m_Type==MOMIDI_NOTEOFF && mididata.m_Val>0 ) {
              if (mididata.m_Channel<=16) {
                m_Notes[mididata.m_Channel][mididata.m_CC].tone = mididata.m_CC;
                m_Notes[mididata.m_Channel][mididata.m_CC].velocity = mididata.m_Val;
                m_Notes[mididata.m_Channel][mididata.m_CC].sustain.Start();
                m_Notes[mididata.m_Channel][mididata.m_CC].release.Stop();
              }
              SUS.Start();
              REL.Stop();

            } else if ( mididata.m_Type==MOMIDI_NOTEOFF ||
              ( mididata.m_Type==MOMIDI_NOTEON && mididata.m_Val==0)
              || (mididata.m_Type==MOMIDI_CC && mididata.m_CC==123 && mididata.m_Val==0) )  {
            //} else if ( mididata.m_Type==MOMIDI_NOTEON || ( mididata.m_Type==MOMIDI_NOTEOFF && mididata.m_Val==0) )  {
              if (mididata.m_Channel<=16) {

                if ( mididata.m_Type==MOMIDI_CC && mididata.m_CC==123 ) {

                  for(int n=0; n<m_Notes[mididata.m_Channel].Count(); n++) {
                    if (m_Notes[mididata.m_Channel][n].sustain.Started()) {
                      m_Notes[mididata.m_Channel][n].sustain.Stop();
                      m_Notes[mididata.m_Channel][n].release.Start();
                    }
                  }
                } else {
                  m_Notes[mididata.m_Channel][mididata.m_CC].tone = mididata.m_CC;
                  m_Notes[mididata.m_Channel][mididata.m_CC].velocity = mididata.m_Val;
                  m_Notes[mididata.m_Channel][mididata.m_CC].sustain.Stop();
                  m_Notes[mididata.m_Channel][mididata.m_CC].release.Start();
                }
              }
              SUS.Stop();
              REL.Start();
            }

        }

    }

  if (m_bAllControlsToZero && m_AllControlsToZeroChannel) {
    for (i=0;i<128;i++) {
      if (i!=123) {
        mididata.m_Channel = m_AllControlsToZeroChannel;
        mididata.m_CC = i;
        mididata.m_Type = MOMIDI_CC;
        mididata.m_Val = 0;
        m_MidiDatas.Add(mididata);
      }
    }
    m_bAllControlsToZero = false;
  }

  if (m_bAllNotesOff) {
    if (0<=this->channel && this->channel<=16) {
      for(int n=0; n<m_Notes[this->channel].Count(); n++) {
        if (m_Notes[this->channel][n].sustain.Started()) {
          m_Notes[this->channel][n].sustain.Stop();
          m_Notes[this->channel][n].release.Start();
        }
      }
    }
    m_bAllNotesOff = false;
  }

	for( i=0; i < m_MidiDatas.Count(); i++ ) {

		mididata = m_MidiDatas.Get( i );

		if (m_bDebugIsOn) MODebug2->Push(
		moText("MIDI Data CC:") + IntToStr(mididata.m_CC)
		+ moText(" val:") + IntToStr(mididata.m_Val) );

		Events->Add( MO_IODEVICE_MIDI, (MOint)(mididata.m_Type), mididata.m_Channel, mididata.m_CC, mididata.m_Val );

	}

	} catch(...) {
    MODebug2->Error("Read error");
	}
	//m_MidiDatas.Empty();


	//mididata = m_MidiDatas.Get( m_MidiDatas.Count()-1 );
	//moData pData = m_DataMessage.Get(m_DataMessage.Count()-1);
	//MODebug2->Push(moText("MIDI n:") + IntToStr(m_DataMessage.Count()) + moText(" code:") + IntToStr(pData.Int()) );
    //m_DataMessage.Empty();

	//m_lock.Unlock();

}

void moMidiDevice::AllControlsToZero( int p_channel ) {
  int i;
  moMidiData mididata;
  for (i=0;i<128;i++) {
    if (i!=123) {
      mididata.m_Channel = p_channel;
      mididata.m_CC = i;
      mididata.m_Type = MOMIDI_CC;
      mididata.m_Val = 0;
      m_MidiDatas.Add(mididata);
    }
  }
}

//=============================================================================



//=============================================================================

moMidi::moMidi() {
	SetName("midi");
	m_pMidiDevice = NULL;
	m_MidiDeviceChannel = 0;
	m_bAllControlsToZero = false;
	m_bAllNotesOff = false;
	mscript = "";
	pOutDataMessages = NULL;
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

  moOutlet* pOutlet = NULL;
  for( int i=0; i<m_Outlets.Count(); i++) {
    pOutlet = m_Outlets.Get(i);
    if (pOutlet->GetConnectorLabelName() == moText("DATAMESSAGES")) {
      pOutDataMessages = pOutlet;
      MODebug2->Message("moMidi::Init > Added Outlet DATAMESSAGES");
    }
  }

	moDefineParamIndex( MIDI_SCRIPT, moText("script") );
	moDefineParamIndex( MIDI_DEVICE, moText("mididevice") );
  moDefineParamIndex( MIDI_CHANNEL, moText("midichannel") );
  moDefineParamIndex( MIDI_NOTEFADEOUT, moText("notefadeout"));
  moDefineParamIndex( MIDI_DEBUG, moText("debug"));
  moDefineParamIndex( MIDI_REINIT, moText("reinit"));

	mididevices = m_Config.GetParamIndex("mididevice");
	midichannels = m_Config.GetParamIndex("midichannel");
	//debugison = m_Config.GetParamIndex("debug");


	InitDevices();

  MODebug2->Message("moMidi::Update > m_Inlets Count: " + IntToStr(m_Inlets.Count()) );
  MODebug2->Message("moMidi::Update > m_Outlets Count: " + IntToStr(m_Outlets.Count()) );


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

bool
moMidi::InitDevices() {

try {
  MOint nvalues = m_Config.GetValuesCount( mididevices );
	m_Config.SetCurrentParamIndex( mididevices );

/*
	for( int c=0; c < m_MidiDevices.Count(); c++ ) {

    moMidiDevicePtr pDevice = m_MidiDevices[c];
    if (pDevice) {
      pDevice->Finish();
      delete pDevice;
    }

	}

	m_MidiDevices.Empty();
*/
	/*
        MO_MIDI_SYTEM_LABELNAME	0
        MO_MIDI_SYSTEM_ON 1
	*/

	for( int i = 0; i < nvalues; i++) {

		m_Config.SetCurrentValueIndex( mididevices, i );
			moText MidiDeviceCode = m_Config.GetParam().GetValue().GetSubValue(MO_MIDI_SYTEM_LABELNAME).Text();
    int MidiIsOn = m_Config.GetParam().GetValue().GetSubValue(MO_MIDI_SYSTEM_ON).Int();
		moMidiDevicePtr pDevice = NULL;
    for( int d = 0; d < m_MidiDevices.Count() ; d++) {
      moMidiDevicePtr pdev = m_MidiDevices.Get(d);
      if (pdev) {
        MODebug2->Message("Check device:"+pdev->GetName());
        if (pdev->GetName()==MidiDeviceCode) {
          pDevice = pdev;
          MODebug2->Message( moText("moMidi::Init > Midi Device Founded [") + MidiDeviceCode + moText("] >>") );
        }
      }
    }
    if (pDevice==NULL) {
      pDevice = new moMidiDevice();
    } else pDevice = NULL;


		if (pDevice!=NULL) {
			pDevice->MODebug = MODebug;
			if ( pDevice->Init( MidiDeviceCode ) ) {
				pDevice->SetActive( MidiIsOn );
				//pDevice->channel = MidiDeviceChannel;
				if (pDevice->IsActive()) {
                    MODebug2->Message( moText("moMidi::Init > Midi Device [") + MidiDeviceCode + moText("] >> is ACTIVE !") );
				}
				m_pMidiDevice = pDevice;
				m_MidiDevices.Add( pDevice );
			} else {
				MODebug2->Error( moText("Midi Device not found: ") + (moText)MidiDeviceCode );
			}
		}



	}
}
  catch(...) {
    MODebug2->Error("InitDevices");
  }

  MODebug2->Message( moText("moMidi::Init > Midi Device Channel:") + GetLabelName() + " >> " + IntToStr(m_MidiDeviceChannel) );

}

bool
moMidi::ReinitDevice() {
  for( int c=0; c < m_MidiDevices.Count(); c++ ) {

    moMidiDevicePtr pDevice = m_MidiDevices[c];
    if (pDevice) {
      pDevice->Reinit();
    }

	}
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

void moMidi::UpdateParameters() {

  m_vMidiFadeout = m_Config.Eval( moR(MIDI_NOTEFADEOUT) );
  m_vGateVelocity = m_Config.Eval( moR(MIDI_NOTEGATEVELOCITY) );
  m_MidiDeviceChannel = m_Config.Int( moR(MIDI_CHANNEL) );

  bool reinit = m_Config.Eval( moR(MIDI_REINIT) );
  //reinitialize
  if ( m_reinit != reinit && reinit==true) {
    ReinitDevice();
    m_reinit = reinit;
  } else m_reinit = reinit;

  m_debugison = m_Config.Eval( moR(MIDI_DEBUG) );

}

void moMidi::RegisterFunctions()
{
    moMoldeoObject::RegisterFunctions();

    RegisterBaseFunction("AllControlsToZero"); //0
    RegisterFunction("AllNotesOff"); //1
    ResetScriptCalling();
}


int moMidi::ScriptCalling(moLuaVirtualMachine& vm, int iFunctionNumber)
{
    m_iMethodBase = 37;
    switch (iFunctionNumber - m_iMethodBase)
    {
        case 0:
            ResetScriptCalling();
            return luaAllControlsToZero(vm);
        case 1:
            ResetScriptCalling();
            return luaAllNotesOff(vm);

        default:
            NextScriptCalling();
            return moMoldeoObject::ScriptCalling( vm, iFunctionNumber );
	}
}

int moMidi::luaAllControlsToZero(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;
    m_bAllControlsToZero = true;
    MODebug2->Message("luaAllControlsToZero " + this->GetLabelName());
    return 0;
}

int moMidi::luaAllNotesOff(moLuaVirtualMachine& vm)
{
    lua_State *state = (lua_State *) vm;
    m_bAllNotesOff = true;
    return 0;
}


void
moMidi::Update(moEventList *Events) {

  moMoldeoObject::ScriptExeRun();

  int i;
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

  UpdateParameters();
  long release_max = 400; ///100ms

  m_DataMessages.Empty();


    ///ACTUALIZAMOS CADA DISPOSITIVO
    //MODebug2->Message("moMidi::Update > Update device: " + moMoldeoObject::GetLabelName()+" devices:" + IntToStr(m_MidiDevices.Count()) );
  for( i = -1; i < (int)m_MidiDevices.Count(); i++ ) {
        //MODebug2->Message("Update devices");
    moMidiDevicePtr MidiDevPtr = NULL;

    //MODebug2->Message("moMidi::Update > Update device i:" + IntToStr(i));

    if (i==-1) {
      MidiDevPtr = m_pMidiDevice;
      //if (m_pMidiDevice)
        //MODebug2->Message("moMidi::Update > Update device: " + moMoldeoObject::GetLabelName()+" m_pMidiDevice:" + m_pMidiDevice->GetName() );
    } else {
      MidiDevPtr = m_MidiDevices.Get(i);
      if (m_pMidiDevice==MidiDevPtr) MidiDevPtr = NULL;
    }


    int idx = -1;
    moDataMessage m_DataMessage;

    if (MidiDevPtr!=NULL) {
      if (MidiDevPtr->IsInit()) {
        if (MidiDevPtr==m_pMidiDevice) {
          m_pMidiDevice->Update( Events );
        }

        if (m_bAllControlsToZero && m_MidiDeviceChannel) {
          MidiDevPtr->AllControlsToZero(m_MidiDeviceChannel);
          m_bAllControlsToZero = false;
        }

        moText codetbn = "TIMEBN";
        moText codesus = "SUSTAIN";
        moText coderel = "RELEASE";
        moText codenote = "NOTE";

        const char* outs[] = { "ISON","SUS","REL","VEL" };

        //MODebug2->Message("Nnotes:"+IntToStr( MidiDevPtr->m_Notes.Count() ) );
        if (m_MidiDeviceChannel>0) {

          for( int iN=0; iN<MidiDevPtr->m_Notes[m_MidiDeviceChannel].Count(); iN++ ) {

            moMidiNote Note = MidiDevPtr->m_Notes[m_MidiDeviceChannel][iN];

            moOutlet* pOutNote = NULL;
            moText base_note_txt = codenote + IntToStr( iN );
            //MODebug2->Message( base_note_txt+ " > " );

            if ( (Note.sustain.Duration()>0) || (Note.release.Duration()>0) ) {
              ///SEND "NOTE42ISON" CC=42 > value = 0|1 if on or off
              ///SEND "NOTE42SUS" value = sustain.duration
              ///SEND "NOTE42REL" value = release.duration
              ///SEND "NOTE42VEL" value = velocity
              double releaseNormal =  ( (double)release_max - (double)Note.release.Duration() ) / (double)release_max;
              if (releaseNormal<=0.0)  { releaseNormal = 0.0; MidiDevPtr->m_Notes[m_MidiDeviceChannel][iN].release.Stop(); }
              if (releaseNormal>=1.0) { releaseNormal = 1.0; MidiDevPtr->m_Notes[m_MidiDeviceChannel][iN].release.Stop(); }

              if (m_debugison)
              MODebug2->Message( base_note_txt+ "[ISON] " + IntToStr(Note.sustain.Duration()>0)
                                              + "[ISON]:REL " + IntToStr(Note.release.Duration()>0));
              if (m_debugison)
              MODebug2->Message( " [SUS] " + IntToStr(Note.sustain.Duration())
                                              + " [REL] " + FloatToStr(releaseNormal)
                                              + " [VEL] " + FloatToStr(Note.velocity) );

              for( int outi=0; outi<4; outi++) {
                moText postext = (char*)outs[outi];
                moText note_txt = base_note_txt + postext;
                //MODebug2->Message( note_txt+ " > " );
                idx = GetOutletIndex( note_txt );
                if( idx>-1) {

                    moOutlet* pOutNote = m_Outlets[idx];

                    if (pOutNote) {
                        switch(outi) {
                          case 0:
                            pOutNote->GetData()->SetDouble( (double)(Note.sustain.Duration()>0) );
                            //MODebug2->Message( note_txt+ ": " + IntToStr(Note.sustain.Duration()>0) );
                            break;
                          case 1:
                            pOutNote->GetData()->SetDouble( (double)Note.sustain.Duration() );
                            //MODebug2->Message( note_txt+ ": " + FloatToStr(Note.sustain.Duration()) );
                            break;
                          case 2:
                            pOutNote->GetData()->SetDouble( (double) releaseNormal );
                            //MODebug2->Message( note_txt+ ": " + FloatToStr(releaseNormal) );
                            break;
                          case 3:
                            pOutNote->GetData()->SetDouble( (double)Note.velocity );
                            //MODebug2->Message( note_txt+ ": " + FloatToStr(Note.velocity) );
                            break;
                          default:
                            //pOutNote->GetData()->SetDouble(0);
                            break;
                        }
                        pOutNote->Update();

                        moData outletmidi;
                        //moData outletchannel;
                        moData outletcode;
                        moData outletvalue;
                        outletmidi.SetText("DATA");
                        //outletchannel.SetInt();
                        outletcode.SetText(note_txt);
                        outletvalue.SetDouble( (double)pOutNote->GetData()->Double() );
                        m_DataMessage.Empty();
                        m_DataMessage.Add(outletmidi);
                        m_DataMessage.Add(outletcode);
                        m_DataMessage.Add(outletvalue);
                        m_DataMessages.Add(m_DataMessage);
                    }
                }
              }//en all note[ison,sus,rel,vel]
            }///end sustain
          }///end notes
        }
        //moText tn = "TIMEN";
        int tbnidx = GetOutletIndex( codetbn );
        if( tbnidx>-1) {
            int dura = MidiDevPtr->TBN.Duration();
            //MODebug2->Message( codetbn+ " founded! Udpating value:" + IntToStr(dura)  );
            moOutlet* pOutTBN = m_Outlets[tbnidx];
            if (pOutTBN) {
                pOutTBN->GetData()->SetDouble( (double)dura );
                pOutTBN->Update();
            }
        }

        tbnidx = GetOutletIndex( codesus );
        if( tbnidx>-1) {
            int dura = MidiDevPtr->SUS.Duration();
            //MODebug2->Message( codetbn+ " founded! Udpating value:" + IntToStr(dura)  );
            moOutlet* pOutSUS = m_Outlets[tbnidx];
            if (pOutSUS) {
                pOutSUS->GetData()->SetDouble( (double)dura );
                pOutSUS->Update();
            }
        }

        tbnidx = GetOutletIndex( coderel );
        if( tbnidx>-1) {
            int dura = MidiDevPtr->REL.Duration();
            //MODebug2->Message( codetbn+ " founded! Udpating value:" + IntToStr(dura)  );
            moOutlet* pOutREL = m_Outlets[tbnidx];
            if (pOutREL) {
                pOutREL->GetData()->SetDouble( (double)dura );
                pOutREL->Update();
            }
        }


				const moMidiDatas& mdatas( MidiDevPtr->GetMidiDatas() );
				for(int md=0; md<mdatas.Count(); md++ ) {
                    m_DataMessage.Empty();
                    const moMidiData& mdata( mdatas.Get(md) );
                    int ccode = mdata.m_CC;
                    moOutlet* pOutCCode = NULL;
                    moText ccodetxt = "";
                    int idx = -1;
                    //MODebug2->Message( "moMidi::Update n outlets:" + IntToStr(m_Outlets.Count()) );
                    if (m_debugison)
                      MODebug2->Message( "moMidi::Update > "+GetLabelName()
                                        +" Channel: " + IntToStr(mdata.m_Channel)
                                        +" Type: " + IntToStr(mdata.m_Type)
                                        +" ccode: " + IntToStr(ccode)
                                        +" val: " + IntToStr(mdata.m_Val) );
                    if (m_MidiDeviceChannel==0 || m_MidiDeviceChannel==mdata.m_Channel) {
                      switch( mdata.m_Type ) {

                        case MOMIDI_PATCHCHANGE:
                          {
                          ccodetxt = moText("PC");/* + IntToStr( ccode, 2);*/
                          idx = GetOutletIndex( ccodetxt );

                          if( idx>-1) {
                              //MODebug2->Message( ccodetxt+ " founded! Udpating value:" + IntToStr(mdata.m_Val)+ "idx:" + IntToStr(idx)  );
                              pOutCCode = m_Outlets[idx];
                              int CC = mdata.m_CC;
                              if (pOutCCode) {
                                  //MODebug2->Message( "Updating code" );
                                  /*
                                  switch( mdata.m_CC ) {
                                    case MIDIBPM_92:
                                      CC = 92;
                                      break;
                                    case MIDIBPM_96:
                                      CC = 96;
                                      break;
                                    case MIDIBPM_104:
                                      CC = 104;
                                      break;
                                    case MIDIBPM_113:
                                      CC = 113;
                                      break;
                                    default:
                                      CC = 0;
                                      break;
                                  }
                                  */
                                  pOutCCode->GetData()->SetDouble( (double)CC );
                                  pOutCCode->Update();
                                  moData outletmidi;
                                  //moData outletchannel;
                                  moData outletcode;
                                  moData outletvalue;
                                  outletmidi.SetText("DATA");
                                  //outletchannel.SetInt();
                                  outletcode.SetText(ccodetxt);
                                  outletvalue.SetDouble( (double)mdata.m_CC );

                                  m_DataMessage.Add(outletmidi);
                                  m_DataMessage.Add(outletcode);
                                  m_DataMessage.Add(outletvalue);
                                  m_DataMessages.Add(m_DataMessage);
                                  if (m_debugison) MODebug2->Message("moMidi::Update "+GetLabelName()+" > Added data to messages:" + outletcode.ToText()+" Mess. Count:" + IntToStr(m_DataMessages.Count())  );

                              }
                          }
                          ccodetxt = moText("PC") + IntToStr( ccode, 2);
                          }
                          break;

                        case MOMIDI_CC:
                        {
                        ccodetxt = moText("C") + IntToStr( ccode, 2);
                        }
                        break;

                        case MOMIDI_NOTEON:
                        {
                          ccodetxt = moText("NOTEONFREQ");/* + IntToStr( ccode, 2);*/
                          idx = GetOutletIndex( ccodetxt );

                          if( idx>-1) {
                              //MODebug2->Message( ccodetxt+ " founded! Udpating value:" + IntToStr(mdata.m_Val)+ "idx:" + IntToStr(idx)  );
                              pOutCCode = m_Outlets[idx];
                              if (pOutCCode) {
                                  //MODebug2->Message( "Updating code" );
                                  pOutCCode->GetData()->SetDouble( (double)mdata.m_CC );
                                  pOutCCode->Update();
                              }
                          }
                          ccodetxt = moText("NOTEONVEL");/* + IntToStr( ccode, 2);*/
                        }
                        break;

                        case MOMIDI_NOTEOFF:
                        {
                        /*
                          ccodetxt = moText("NOTEONFREQ");
                          idx = GetOutletIndex( ccodetxt );

                          if( idx>-1) {
                              MODebug2->Message( ccodetxt+ " founded! Udpating value:" + IntToStr(mdata.m_Val)+ "idx:" + IntToStr(idx)  );
                              pOutCCode = m_Outlets[idx];
                              if (pOutCCode) {
                                  MODebug2->Message( "Updating code" );
                                  pOutCCode->GetData()->SetDouble( (double)mdata.m_CC );
                                  pOutCCode->Update();
                              }
                          }
                          ccodetxt = moText("NOTEONVEL");/* + IntToStr( ccode, 2);
                          */
                        }
                        break;

                        default:

                          break;
                      }
                      idx = GetOutletIndex( ccodetxt  );
                      //MODebug2->Message( "moMidi::Update > ccodetxt: " + ccodetxt + " val:"+ IntToStr(mdata.m_Val)+" idx:"+IntToStr(idx) );

                      if( idx>-1) {
                          //MODebug2->Message( ccodetxt+ " founded! Udpating value:" + IntToStr(mdata.m_Val)+ "idx:" + IntToStr(idx)  );
                          pOutCCode = m_Outlets[idx];
                          if (pOutCCode) {
                              //MODebug2->Message( "Updating code" );
                              pOutCCode->GetData()->SetDouble( (double)mdata.m_Val );
                              pOutCCode->Update();
                              moData outletmidi;
                              //moData outletchannel;
                              moData outletcode;
                              moData outletvalue;

                              outletmidi.SetText("DATA");
                              //outletchannel.SetInt();
                              outletcode.SetText(ccodetxt);
                              outletvalue.SetDouble( (double)mdata.m_Val );

                              m_DataMessage.Add(outletmidi);
                              m_DataMessage.Add(outletcode);
                              m_DataMessage.Add(outletvalue);

                              m_DataMessages.Add(m_DataMessage);
                              if (m_debugison) MODebug2->Message("moMidi::Update "+GetLabelName()+" > Added data to messages:" + outletcode.ToText()+" Mess. Count:" + IntToStr(m_DataMessages.Count())  );
                          }
                      }
                    }
                }
            }
		}


	}


if (pOutDataMessages) {
      moDataMessages& msgs( pOutDataMessages->GetMessages() );
      msgs.Empty();
      msgs = m_DataMessages;
      if ( pOutDataMessages->GetType()==MO_DATA_MESSAGES ) {
        //if (m_debugison) MODebug2->Message("moMidi::Update > Updated Outlet DATAMESSAGES: Count:" + IntToStr(m_DataMessages.Count()) );
        pOutDataMessages->GetData()->SetMessages( &msgs );
        pOutDataMessages->Update();
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
	p_configdefinition->Add( moText("midichannel"), MO_PARAM_NUMERIC, MIDI_CHANNEL, moValue( "0", "NUM"), moText("No channel,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16") );
	p_configdefinition->Add( moText("notefadeout"), MO_PARAM_FUNCTION, MIDI_NOTEFADEOUT, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("notegatevelocity"), MO_PARAM_FUNCTION, MIDI_NOTEGATEVELOCITY, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("debug"), MO_PARAM_NUMERIC, MIDI_DEBUG, moValue( "0", "NUM").Ref(), moText("NO,YES") );
	p_configdefinition->Add( moText("reinit"), MO_PARAM_NUMERIC, MIDI_REINIT, moValue( "0", "NUM").Ref(), moText("NO,YES") );
	return p_configdefinition;
}


MOboolean
moMidi::Finish() {
	return true;
}
