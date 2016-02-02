/*******************************************************************************

                              moGPIO.cpp

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

#include "moGPIO.h"

#include "moArray.h"

//========================
//  Factory
//========================

moGPIOFactory *m_GPIOFactory = NULL;

MO_PLG_API moIODeviceFactory* CreateIODeviceFactory(){
	if (m_GPIOFactory==NULL)
		m_GPIOFactory = new moGPIOFactory();
	return (moIODeviceFactory*) m_GPIOFactory;
}

MO_PLG_API void DestroyIODeviceFactory(){
	delete m_GPIOFactory;
	m_GPIOFactory = NULL;
}

moIODevice*  moGPIOFactory::Create() {
	return new moGPIO();
}

void moGPIOFactory::Destroy(moIODevice* fx) {
	delete fx;
}



// moGPIO class **************************************************

moGPIO::moGPIO()
{
    SetName("gpio");
    m_Port = 7400;
}

moGPIO::~moGPIO()
{
    Finish();
}

MOboolean moGPIO::Init()
{
    int dev;
    MOuint i, n, n_dev, n_hosts;
    moText conf, dev_name;

    // Loading config file.
    if (moMoldeoObject::Init()) {
      moMoldeoObject::CreateConnectors();
    } else return false;


    moDefineParamIndex( GPIO_HOSTS, moText("hosts") );
    moDefineParamIndex( GPIO_PORT, moText("port") );
    moDefineParamIndex( GPIO_DEBUG, moText("debug") );


    // Reading hosts names and ports.
    n = m_Config.GetParamIndex("hosts");
	n_hosts = m_Config.GetValuesCount(n);


    UpdateParameters();

	return true;
}

void
moGPIO::UpdateParameters() {

    m_Port = m_Config.Int( moR( GPIO_PORT ) );
    debug_is_on = ( 0 != m_Config.Int( moR(GPIO_DEBUG) ) );
}


MOswitch moGPIO::SetStatus(MOdevcode codisp, MOswitch state)
{
    return true;
}

MOswitch moGPIO::GetStatus(MOdevcode codisp)
{
    return(-1);
}

MOint moGPIO::GetValue(MOdevcode codisp)
{
    return(-1);
}

MOdevcode moGPIO::GetCode(moText strcod)
{
    return(-1);
}

moConfigDefinition *
moGPIO::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moMoldeoObject::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("hosts"), MO_PARAM_TEXT, GPIO_HOSTS, moValue( "127.0.0.1", "TXT", "3335", "INT") );
	p_configdefinition->Add( moText("port"), MO_PARAM_NUMERIC, GPIO_PORT, moValue( "3335", "INT") );

	p_configdefinition->Add( moText("debug"), MO_PARAM_NUMERIC, GPIO_DEBUG, moValue("0","INT") );

	return p_configdefinition;
}

void moGPIO::Update(moEventList *Eventos)
{
    MOuint i;

    bool res;
    moEvent *tmp;
    moEvent *actual;

    UpdateParameters();
    // Sending over the network the events that correspond to recognized devices.
    //Eventos->Add( MO_IODEVICE_TRACKER, moGetTicks(), 112, 113, 114, 115 );

    //inlets outlets
    moMoldeoObject::Update(Eventos);

}

MOboolean moGPIO::Finish()
{
	return true;
}

void moGPIO::SendEvent(int i)
{
}

void moGPIO::SendDataMessage( int i, moDataMessage &datamessage ) {

}

