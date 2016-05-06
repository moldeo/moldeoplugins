/*******************************************************************************

                                moMidi.h

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
  Andr� Colubri

  Class:
  moMidi

  Description:
  Motion Analyzer


*******************************************************************************/

#include "moConfig.h"
#include "moDeviceCode.h"
#include "moEventList.h"
#include "moIODeviceManager.h"
#include "moTypes.h"
#include "moVideoGraph.h"
#include "moArray.h"
//#include "moLive.h"
#include "moShaderGLSL.h"

#ifdef WIN32
	//#include "setupapi.h"
	//#include "hidsdi.h"
	#include "mmsystem.h"
	#include "tchar.h"
    #include "wchar.h"

    #ifdef UNICODE
        #define m_stricmp wcsicmp
    #else
        #define m_stricmp stricmp
    #endif
#else

#endif

#include "portmidi.h"

#ifndef __MO_MIDIDEVICES_H
#define __MO_MIDIDEVICES_H

#define	MOMIDI_CFG_STRCOD			0
#define	MOMIDI_CFG_ROTARYBUTTON		1
#define	MOMIDI_CFG_MIDICHANNEL		2
#define	MOMIDI_CFG_MIDICC			3

#define	MO_MIDI_SYTEM_LABELNAME		0
#define	MO_MIDI_SYSTEM_ON			1

enum moMidiParamIndex {

  MIDI_DEVICE,
  MIDI_CHANNEL,
  MIDI_CODES

};

/** MIDI commands
  0x80     Note Off
  0x90     Note On
  0xA0     Aftertouch
  0xB0     Continuous controller
  0xC0     Patch change
  0xD0     Channel Pressure
  0xE0     Pitch bend
  0xF0     (non-musical commands)
*/
enum moEncoderType {
	MOMIDI_ROTARY=0, /** custom: */
	MOMIDI_PUSHBUTTON=1, /** custom: */
	MOMIDI_FADER=2, /** custom: */
	MOMIDI_NOTEON=8, /** 0x80     Note Off */
	MOMIDI_NOTEOFF=9, /** 0x90     Note On */
	MOMIDI_AFTERTOUCH=10, /** 0xA0     Aftertouch */
	MOMIDI_CC=11, /** 0xB0     Continuous controller */
	MOMIDI_PATCHCHANGE=12, /** 0xC0     Patch change */
	MOMIDI_CHANNELPRESSURE=13, /** 0xD0     Channel Pressure */
	MOMIDI_PITCHBEND=14, /** 0xE0     Pitch bend */
	MOMIDI_SYSEX=15, /** 0xF0     (non-musical commands) */
};
/** FULL TABLES
Command   Meaning 	  # parameters 	param 1 	  param 2
0x80      Note-off 	  2             key         velocity
0x90      Note-on 	  2             key         velocity
0xA0      Aftertouch  2             key         touch
0xB0      CC          2             code#       value
0xC0      P. Change 	2             instrument#
0xD0      C. Pressure 1             pressure
0xE0      Pitch bend 	2             lsb(7bits)  msb(7bits)
0xF0      (non-musical commands)

command 	meaning 	# param
0xF0 	start of system exclusive message 	variable
0xF1 	MIDI Time Code Quarter Frame (Sys Common)
0xF2 	Song Position Pointer (Sys Common)
0xF3 	Song Select (Sys Common)
0xF4 	???
0xF5 	???
0xF6 	Tune Request (Sys Common)
0xF7 	end of system exclusive message 	0
0xF8 	Timing Clock (Sys Realtime)
0xFA 	Start (Sys Realtime)
0xFB 	Continue (Sys Realtime)
0xFC 	Stop (Sys Realtime)
0xFD 	???
0xFE 	Active Sensing (Sys Realtime)
0xFF 	System Reset (Sys Realtime)
*/

class moMidiData {
	public:
		moMidiData();
		virtual ~moMidiData();

		moMidiData& operator = ( const moMidiData& mididata);
		void Copy( const moMidiData& mididata );


		moEncoderType	m_Type;
    MOint m_Status;
		MOint	m_Channel;
		MOint	m_CC;
		MOint	m_Val;

};

template class moDynamicArray<moMidiData>;
typedef  moDynamicArray<moMidiData> moMidiDatas;

class moMidiDataCode {
	public:
		moText		strcode;
		MOint		devcode;
		MOint		state;
		moMidiData	mididata;

};
template class moDynamicArray<moMidiDataCode>;
typedef  moDynamicArray<moMidiDataCode> moMidiDataCodes;


class moMidiDevice : /*public moThread,*/ public moAbstract {

	public:

		moMidiDevice();
		virtual ~moMidiDevice();

		void SetName( moText p_name) {
			m_Name = p_name;
		}
		void SetActive( MOboolean p_active) {
			m_bActive = p_active;
		}

		MOboolean		Init();
		MOboolean		Init( moText devicetext );
		MOboolean		Finish();

		MOboolean			IsInit() {
			return m_bInit;
		}
		moText GetName() {
			return m_Name;
		}

		MOboolean IsActive() {
			return m_bActive;
		}

		void				NewData( moMidiData p_mididata );
		const moMidiDatas&			GetMidiDatas() {
            return m_MidiDatas;
		}
		void Update(moEventList *Events );

	//
	/*
	#ifdef WIN32
	static void CALLBACK midiCallback(HMIDIIN handle, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
	#else
	*/
	PortMidiStream *stream; /* A PortMidiStream pointer */
	char channel;
	PmEvent buffer[4000];
	/*
	#endif
	*/
	void PrintMidiInErrorMsg(unsigned long err);

	//=============================================================================
	/*	A buffer to hold incoming System Exclusive bytes. I arbitrarily make this 256 bytes. Note:
		For Win 3.1 16-bit apps, this buffer should be allocated using GlobalAlloc with the
		GMEM_MOVEABLE flag to get a handle of the memory object. Then pass this handle to the
		GlobalLock function to get a pointer to the memory object. To free a data block, use
		GlobalUnlock and GlobalFree. But Win32 doesn't appear to have this limitation.
	*/
	unsigned char SysXBuffer[256];

	/* A flag to indicate whether I'm currently receiving a SysX message */
	unsigned char SysXFlag;
  moTimer TBN;/**Time Between Notes*/

	protected:

		moLock			m_lock;
		moMidiDatas		m_MidiDatas;
		MOint			m_DeviceId;
		moText			m_Name;
		MOboolean		m_bActive;
		MOboolean		m_bInit;

		moDataMessage   m_DataMessage;

};


typedef moMidiDevice* moMidiDevicePtr;

template class moDynamicArray<moMidiDevicePtr>;
typedef  moDynamicArray<moMidiDevicePtr> moMidiDevicePtrs;

class moMidi : public moIODevice
{
public:
    moMidi();
    ~moMidi();

    void Update(moEventList*);
    MOboolean Init();
    MOswitch GetStatus(MOdevcode);
    MOswitch SetStatus( MOdevcode,MOswitch);
	void SetValue( MOdevcode cd, MOint vl );
	void SetValue( MOdevcode cd, MOfloat vl );

    moConfigDefinition *GetDefinition( moConfigDefinition *p_configdefinition );

    MOint GetValue(MOdevcode);
	MOpointer GetPointer(MOdevcode devcode );
    MOdevcode GetCode( moText);
    MOboolean Finish();



private:
    moConfig config;

    moEventList *events;

protected:

	MOint	mididevices;
	MOint midichannels;
   	moMidiDevicePtrs		m_MidiDevices;
	moMidiDataCodes			m_Codes;

};


class moMidiFactory : public moIODeviceFactory {

    public:
        moMidiFactory() {}
        virtual ~moMidiFactory() {}
        moIODevice* Create();
        void Destroy(moIODevice* fx);

};

extern "C"
{
MO_PLG_API moIODeviceFactory* CreateIODeviceFactory();
MO_PLG_API void DestroyIODeviceFactory();
}


#endif
