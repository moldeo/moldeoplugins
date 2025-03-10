/*******************************************************************************

                                moTablet.h

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                     *
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

  Copyright(C) 2007 Andr�s Colubri

  Authors:
  Fabricio Costa
  Andr�s Colubri

*******************************************************************************/

//////////////////////////////////////////////////////////////////////
//
// ** Example using WINTAB for graphics tablets **
//
// Example of how to implement a graphics tablet in an MFC application
// It assumes the tablet controls the mouse pointer so that location
// and button information is obtained from the normal mouse packets.
// To reduce the overhead, only extra information provided by the tablet, is
// captured and processed.
//
// Main Purpose:	Capture the pressure level from the tablet pen
// Additions:		Other potentially useful tablet information, see 'Information Processed:'
// Excluded:		Pointer location and button information provided from mouse driver
//
// Information Processed:	(worked out using experimentation)
//			Pressure:	Just a number representing how hard you are pressing on the tip.
//						0 = Not touching the surface.
//						255 = Maximum pressure (only tested on the Wacom tablet.)
//			Altitude:	Angle between the pen and the tablet surface. Wacom tablets use
//				or		this to indicate attitude as well.  Positive is when the pen
//			Tilt		is the normal way up and negative when the pen is upside down with
//						the eraser tip towards the surface.  See also; the cursor number.
//						0 Tilt would be with the pen laying on the surface, but in practice my
//						tablet stopped detecting at an angle of less than 260 degrees.
//						900 Tilt is straight upright, perpendicular to the tablet.
//			Twist:		Untested, but I think this is a number representing rotating the pen
//						between your fingers, or a rotation of a puck about it's point.
//						As far as I can tell it is not provided on the Wacom tablet I was using.
//			Azimuth:	This is the direction the pen is leaning in relation to the pen tip.
//				or		I refer to this as 'Compass', which I find a more memorable term.
//			Compass:	0 = Leaning away from you which I call north, tip nearest to you.
//						900 = East, tip left top end right.
//						1800 = South, tip away from you, top towards you.
//						2700 = West, tip right, top end left.
//						3599 = Nearly back to the north position again.
//			Cursor:		If the tablet or pen has more than one setting a cursor number
//						will be returned.  For example; on the Wacom tablet, cursor number
//						one (1) is the normal tip and cursor number two (2) is the erase
//						tip when the pen is upside down.
//
// Author:			John C. Brown, john@clue.demon.co.uk	http://www.clue.demon.co.uk
// Date:			August 1998
// Copyright:		None, use it as you see fit.
// Warranty:		None, it's free so it's your problem.
// Test Tablet:		Wacom UltraPad A4 (European), ArtZ II 12x12 (US)
//
// Based on:		Information found in the Wintab documentation and developer
//					resources on the Wacom and the LCS/Telegraphics web sites.
//					The latest documentation I have is version 1.1 dated: 9 May 1996.
//					ftp://ftp.pointing.com/pointing/Wintab/wt1pt1.zip
//					I am only using functions from version 1.0 dated: 13 December 1994.
//					ftp://ftp.wacom.com/pub/developer/ibmpc/wintab32.zip
// Required:		Wintab library and header files from the developer kit (wtkit124.zip):
// Sources:			http://www.pointing.com
//					ftp://ftp.pointing.com/pointing/Wintab/wtkit124.zip
//					http://www.wacom.com
//					ftp://ftp.wacom.com/pub/developer/ibmpc/wtkit124.zip
//					ftp://ftp.wacom.de/pub/Others/DEVDISK.ZIP
//
// Documentation:	Although WinTab is fully documented, I found it was not clearly
//					explained.  To work out what most of the terms mean, I had to use
//					EXPERIMENTATION using my Wacom tablet, other tablets may differ
//					slightly in their function.
//
// Rotation:		NOT included in this work.  Rotation is new in the WinTab 1.1 specification
//					dated 9 May 1996.  It is NOT included in the wintab.h header file
//					provided in the wintab developer kit version 1.24.
//
// Improvements:	If you make any improvements or have tested it on other makes of tablet
//					let me know your findings, so I can pass the information on.
//
//////////////////////////////////////////////////////////////////////

#include "moTypes.h"
#include "moConfig.h"
#include "moDeviceCode.h"
#include "moEventList.h"
#include "moIODeviceManager.h"

#ifndef __MO_TABLET_H__
#define __MO_TABLET_H__

//TABLET: setup required here
//
#ifdef MO_WIN32
#include "SDL.h"

#include "wintab.h"
//#define EASYTAB_IMPLEMENTATION
//#include "EasyTab/easytab.h"
//
//    define what packet data is required here.  A PACKET structure
//    will be created by pktdef.h  Only the required information
//    will be returned in each packet, so some of the members will
//    not be available, depending on your choices here.  Change your code as appropriate.
//    For multiple packet types refer to the WinTab API reference.
//
//#define PACKETDATA	(PK_Z | PK_NORMAL_PRESSURE | PK_ORIENTATION | PK_TANGENT_PRESSURE)
//#define PACKETDATA	(PK_Z | PK_NORMAL_PRESSURE)
//#define PACKETDATA	(PK_X | PK_Y | PK_BUTTONS | PK_NORMAL_PRESSURE | PK_ORIENTATION | PK_CURSOR)
//#define PACKETDATA	(PK_Z | PK_NORMAL_PRESSURE | PK_ORIENTATION | PK_CURSOR)
#define PACKETDATA	(PK_X | PK_Y | PK_Z |PK_BUTTONS | PK_NORMAL_PRESSURE | PK_ORIENTATION | PK_CURSOR)
#define PACKETMODE      0
#include "pktdef.h"

#define FIX_DOUBLE(x)   ((double)(INT(x))+((double)FRAC(x)/65536))	// converts FIX32 to double
//#include "SDL2/SDL.h"
#endif // MO_WIN32



#ifdef MO_LINUX
#include "SDL2/SDL.h"
#define EASYTAB_IMPLEMENTATION
#include "EasyTab/easytab.h"
#define UINT unsigned int
#define ORIENTATION unsigned int
#define LONG long
#endif

#ifdef MO_MACOSX

#endif

// Pen
#define TABLET_PEN_MOTION_X 0
#define TABLET_PEN_MOTION_Y 1
#define TABLET_PEN_MOTION_Z 2
#define TABLET_PEN_PRESSURE 3
#define TABLET_PEN_AZIMUTH  4
#define TABLET_PEN_TILT     5
#define TABLET_PEN_TWIST    6
#define TABLET_PEN_CURSOR   7
#define TABLET_PEN_BUTTON	8

#define WT_MOTION_X_CHANGE 0
#define WT_MOTION_Y_CHANGE 1
#define WT_MOTION_Z_CHANGE 2
#define WT_PRESSURE_CHANGE 3
#define WT_AZIMUTH_CHANGE  4
#define WT_TILT_CHANGE     5
#define WT_TWIST_CHANGE    6
#define WT_CURSOR_CHANGE   7
#define WT_BUTTON_PRESSED  8

#define MO_TABLET_CURSOR_UNDEFINED	0
#define MO_TABLET_CURSOR_PEN		1
#define MO_TABLET_CURSOR_ERASER		2


class moTabletPen
{
public:
	MOboolean change;
	MOint value;

	moTabletPen() {
		change = MO_FALSE;
		value = 0;
	}

	void Init() {
		change = MO_FALSE;
		value = 0;
	}

	void Change(MOint r) {
		if(r!=value) {
			value = r;
			change = MO_TRUE;
		}
	}
};

class moTabletCode
{
public:
	moText strcod;
	MOint code;
	MOint tabletcod;    //mapeo type SDL
	MOint type;         //pushedr:0 ,prendedor:1, selector:2, seteador:3,...otros
	MOint especiales;   //keys especiales, como alt, caps lock, etc...
	MOint state;        //0 MO_FALSE(no esta activo el device) MO_ON(est� on) MO_OFF(est� off)
	MOint value;

	MOint* values;
	MOint nMaxValues;
	MOint nValues;

	void Cleanup() { state = MO_FALSE; value = 0; nValues = 0; }

	void CreateValuesArray(int n) { values = new int[n]; nMaxValues = n; }
	void DeleteValuesArray() { delete[] values; }
	void AddValueToArray(int v) { if (nValues < nMaxValues) { values[nValues] = v; nValues++; }   }
};

class moTablet : public moIODevice {
public:
    moTablet();
	~moTablet();

	MOboolean		Init();
	MOboolean		Finish();

    MOswitch		GetStatus( MOdevcode);
    MOswitch		SetStatus( MOdevcode,MOswitch);
	MOint			GetValue( MOdevcode);
    MOint			GetNValues( MOdevcode);
	MOint			GetValue( MOdevcode devcode, MOint);
    MOdevcode		GetCode( moText);
	MOint			getTabletCod(moText);
    void			Update(moEventList*);
private:
	moConfig config;
	moTabletCode *Codes;
	MOuint ncodes;

	moTabletPen Pen[8];

	//CONSOLA VIRTUAL
	//moConfig configcv;
	//MOuint channel0virtual[5];
	//MOuint masterchannelvirtual[5];

	//TABLET: members available throughout class

	#ifdef MO_WIN32
	LOGCONTEXT      lcMine;           // The context of the tablet

	HCTX t_hTablet;			// Tablet context

	BOOL t_bTiltSupport;	// TRUE if tablet provides tilt and direction information
	//	current pen statistics
	UINT            t_prsNew;		// Pressure value
	UINT            t_curNew;		// Cursor number
	ORIENTATION     t_ortNew;		// Varios movement values, direction and tilt

	LONG            t_pkXNew;
	LONG            t_pkYNew;
	LONG            t_pkZNew;

	//	tablet specific functions
	HCTX InitTablet(HWND hWnd);
	BOOL IsTabletInstalled();
	UINT            m_cMaxPkts; // maximum number of packets in the queue.
	PACKET* m_lpPkts;           // packet buffer
	//Display* m_Disp;
    //Window   m_Win;
	#endif // MO_WIN32

#ifdef MO_LINUX
  Display* m_Disp;
  Window   m_Win;

#endif

	moText GetTabletName();


#ifdef MO_LINUX

#endif
	//	adjustments used in example
	double t_dblAltAdjust;		// Tablet Altitude zero adjust
	double t_dblAltFactor;	// Tablet Altitude factor
	double t_dblAziFactor;	// Table Azimuth factor
	//End Tablet specific

	int MOCursorCode(int wt_code);
};

class moTabletFactory : public moIODeviceFactory {

public:
    moTabletFactory() {}
    virtual ~moTabletFactory() {}
    moIODevice* Create();
    void Destroy(moIODevice* fx);
};

extern "C"
{
MO_PLG_API moIODeviceFactory* CreateIODeviceFactory();
MO_PLG_API void DestroyIODeviceFactory();
}

#endif
