/*******************************************************************************

                              moGPIO.h

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

#include "moTypes.h"
#include "moActions.h"

#ifndef __MO_GPIO_H__
#define __MO_GPIO_H__

#include "moConfig.h"
#include "moDeviceCode.h"
#include "moEventList.h"
#include "moIODeviceManager.h"
#include "moFilterManager.h"

#ifndef MO_WIN32
  #include <wiringPi.h>
  #include <wpiExtensions.h>
#endif // MO_WIN32

enum moGPIOParamIndex {
    GPIO_INLET,
    GPIO_OUTLET,
    GPIO_SCRIPT,
    GPIO_HOSTS,
    GPIO_PORT,
    GPIO_DEVICES,
    GPIO_LATENCY,
    GPIO_SENDMOLDEOAPI,
    GPIO_DEBUG
};


// moNetOSCOut class **************************************************
bool debug_is_on = false;

class moGPIO : public moIODevice
{
public:
    moGPIO();
    virtual ~moGPIO();

    void Update(moEventList*);
    MOboolean Init();
    MOswitch GetStatus(MOdevcode);
    MOswitch SetStatus( MOdevcode,MOswitch);
    MOint GetValue(MOdevcode);
    MOdevcode GetCode( moText);
    MOboolean Finish();

    virtual moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

    void UpdateParameters();

private:

    int m_Port;

    // Parameters.
    moTextArray host_name;
    moIntArray host_port;

    void SendEvent(int i);
    void SendDataMessage( int i, moDataMessage &datamessage );

};

class moGPIOFactory : public moIODeviceFactory {

public:
    moGPIOFactory() {}
    virtual ~moGPIOFactory() {}
    moIODevice* Create();
    void Destroy(moIODevice* fx);
};

extern "C"
{
MO_PLG_API moIODeviceFactory* CreateIODeviceFactory();
MO_PLG_API void DestroyIODeviceFactory();
}


#endif
