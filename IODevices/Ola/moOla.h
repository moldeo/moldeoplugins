/*******************************************************************************

                                moOla.h

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

  Class:
  moOla

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
#include <stdlib.h>
#include <unistd.h>

#ifdef WIN32

    #ifdef UNICODE
        #define m_stricmp wcsicmp
    #else
        #define m_stricmp stricmp
    #endif
    #define USE_LIBARTNET
#else
  #define USE_LIBARTNET 0
#endif

#ifndef __MO_MIDIDEVICES_H
#define __MO_MIDIDEVICES_H

#include <stdint.h>


#ifndef WIN32
  #include <ola/DmxBuffer.h>
  //#include <ola/io/SelectServer.h>
  #include <ola/Logging.h>
  //#include <ola/client/ClientWrapper.h>
  #include <ola/Callback.h>
  #include <ola/client/StreamingClient.h>
#endif

#ifdef USE_LIBARTNET
#include <artnet/artnet.h>
#include <artnet/packets.h>
#endif

using std::cout;
using std::endl;

#define	MOOLA_CFG_STRCOD			0
#define	MOOLA_CFG_ROTARYBUTTON		1
#define	MOOLA_CFG_MIDICHANNEL		2
#define	MOOLA_CFG_MIDICC			3

#define	MO_OLA_SYTEM_LABELNAME		0
#define	MO_OLA_SYSTEM_ON			1

#define MO_IODEVICE_OLA 0x7001

enum moOlaParamIndex {

  OLA_DEVICE,
  OLA_RED,
  OLA_GREEN,
  OLA_BLUE,
  OLA_ALPHA,
  OLA_STARTUNIVERSE,
  OLA_LEDS,
  OLA_RGBTYPE, /** Rgb Type */
  OLA_TESTMODE,
  OLA_TESTOFFSET,
  OLA_MDEBUG,
  OLA_CHECKSERVER,
  OLA_CODES

};

enum moEncoderType {
	MOOLA_ROTARY=0,
	MOOLA_PUSHBUTTON=1,
	MOOLA_FADER=2
};

class moOlaData {
	public:
		moOlaData();
		virtual ~moOlaData();

		moOlaData& operator = ( const moOlaData& mididata);
		void Copy( const moOlaData& mididata );


		moEncoderType	m_Type;
		MOint	m_Channel;
		MOint	m_CC;
		MOint	m_Val;

};

template class moDynamicArray<moOlaData>;
typedef  moDynamicArray<moOlaData> moOlaDatas;

class moOlaDataCode {
	public:
		moText		strcode;
		MOint		devcode;
		MOint		state;
		moOlaData	oladata;

};
template class moDynamicArray<moOlaDataCode>;
typedef  moDynamicArray<moOlaDataCode> moOlaDataCodes;


class moOlaDevice : /*public moThread,*/ public moAbstract {

	public:

		moOlaDevice();
		virtual ~moOlaDevice();

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

		void				NewData( moOlaData p_oladata );
		moOlaData*			GetData();
		void Update(moEventList *Events);


	protected:

        moText          m_Ip;
        moText          m_Port;
        moText          m_Universe;
        int             m_iUniverse;

		moLock			m_lock;
		moOlaDatas		m_OlaDatas;
		MOint			m_DeviceId;
		moText			m_Name;
		MOboolean		m_bActive;
		MOboolean		m_bInit;

		moDataMessage   m_DataMessage;

};


typedef moOlaDevice* moOlaDevicePtr;

template class moDynamicArray<moOlaDevicePtr>;
typedef  moDynamicArray<moOlaDevicePtr> moOlaDevicePtrs;

class moOla : public moIODevice
{
public:
    moOla();
    virtual ~moOla();

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

    void TestLeds( int ciclos=1, int steps=20, bool debug_on=false );

private:
    moConfig config;

#ifndef WIN32
    ola::client::StreamingClient ola_client;
    ola::DmxBuffer buffer; // A DmxBuffer to hold the data.
#endif

#ifdef USE_LIBARTNET

     artnet_node node;


#endif

    moEventList *events;

protected:

	MOint	oladevices;
   	moOlaDevicePtrs		m_OlaDevices;
	moOlaDataCodes			m_Codes;

};


class moOlaFactory : public moIODeviceFactory {

    public:
        moOlaFactory() {}
        virtual ~moOlaFactory() {}
        moIODevice* Create();
        void Destroy(moIODevice* fx);

};

extern "C"
{
MO_PLG_API moIODeviceFactory* CreateIODeviceFactory();
MO_PLG_API void DestroyIODeviceFactory();
}


#endif
