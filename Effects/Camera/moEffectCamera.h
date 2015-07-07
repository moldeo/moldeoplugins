/*******************************************************************************

                                moEffectCamera.h

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

#ifndef __MO_EFFECT_CAMERA__
#define __MO_EFFECT_CAMERA__

#include "moPlugin.h"
#include "moScript.h"
#include "moFilterManager.h"

#define MO_CAMERA_STOP				0
#define MO_CAMERA_PLAY				1
#define MO_CAMERA_PAUSE				2
#define MO_CAMERA_REVERSE			3
#define MO_CAMERA_FW					4
#define MO_CAMERA_RW					5
#define MO_CAMERA_FF					6
#define MO_CAMERA_FR					7
#define MO_CAMERA_SEEK				8
#define MO_CAMERA_SPEED				9
#define MO_CAMERA_PREVFRAME			10
#define MO_CAMERA_NEXTFRAME			11
#define MO_CAMERA_LOOP				12
#define MO_CAMERA_GAIN_UP			13
#define MO_CAMERA_GAIN_DOWN			14
#define MO_CAMERA_EQ_LOW_GAIN_UP		15
#define MO_CAMERA_EQ_LOW_GAIN_DOWN	16
#define MO_CAMERA_EQ_LOW_CUTOFF_UP	20
#define MO_CAMERA_EQ_LOW_CUTOFF_DOWN 21
#define MO_CAMERA_EQ_MID1_CENTER_UP 22
#define MO_CAMERA_EQ_MID1_CENTER_DOWN 23
#define MO_CAMERA_EQ_MID1_WIDTH_UP 24
#define MO_CAMERA_EQ_MID1_WIDTH_DOWN 25
#define MO_CAMERA_EQ_MID1_GAIN_UP 26
#define MO_CAMERA_EQ_MID1_GAIN_DOWN 27
#define MO_CAMERA_EQ_MID2_CENTER_UP 28
#define MO_CAMERA_EQ_MID2_CENTER_DOWN 29
#define MO_CAMERA_EQ_MID2_WIDTH_UP 30
#define MO_CAMERA_EQ_MID2_WIDTH_DOWN 31
#define MO_CAMERA_EQ_MID2_GAIN_UP 32
#define MO_CAMERA_EQ_MID2_GAIN_DOWN 33
#define MO_CAMERA_EQ_HIGH_GAIN_UP 34
#define MO_CAMERA_EQ_HIGH_GAIN_DOWN 35
#define MO_CAMERA_EQ_HIGH_CUTOFF_UP 36
#define MO_CAMERA_EQ_HIGH_CUTOFF_DOWN 37

#ifndef MO_RED
#define MO_RED      0
#define MO_GREEN    1
#define MO_BLUE     2
#define MO_ALPHA    3
#endif

enum moEffectCameraMode {
	MO_CAMERA_MODE_VCR = 0,
	MO_CAMERA_MODE_SCRIPT = 1,
	MO_CAMERA_MODE_CYCLE = 2,
	MO_CAMERA_MODE_VCR_PLAYLIST = 3
};

enum moEffectCameraPlayState {
	MO_CAMERA_PLAYSTATE_STOPPED = 0,
	MO_CAMERA_PLAYSTATE_PLAYING = 1,
	MO_CAMERA_PLAYSTATE_REVERSE = 2,
	MO_CAMERA_PLAYSTATE_SPEED = 3,
	MO_CAMERA_PLAYSTATE_PAUSED = 4,
	MO_CAMERA_PLAYSTATE_SEEKING = 5
};


enum moEffectCameraSeekState {
	MO_CAMERA_SEEKSTATE_SEEKING = 0,
	MO_CAMERA_SEEKSTATE_REACHED = 1,
	MO_CAMERA_SEEKSTATE_PLAYING = 2,
	MO_CAMERA_SEEKSTATE_PAUSED = 3
};

enum moEffectCameraVCRCommand {
	MO_CAMERA_VCR_STOP = 0,//Stop
	MO_CAMERA_VCR_PLAY = 1,//Play
	MO_CAMERA_VCR_PAUSE = 2,//Stop
	MO_CAMERA_VCR_REVERSE = 3,//Reverse
	MO_CAMERA_VCR_FW = 4,//Forward
	MO_CAMERA_VCR_RW = 5,//Rewind
	MO_CAMERA_VCR_FF = 6,//Fast Forward
	MO_CAMERA_VCR_FR = 7,//Fast Rewind
	MO_CAMERA_VCR_SEEK = 8,//Seek for a frame
	MO_CAMERA_VCR_SPEED = 9,//Play at speed
	MO_CAMERA_VCR_PREVFRAME = 10,//move to prev frame
	MO_CAMERA_VCR_NEXTFRAME = 11,//move to next frame
	MO_CAMERA_VCR_LOOP = 12//loop
};


enum moCameraParamIndex {
	CAMERA_INLET,
	CAMERA_OUTLET,
	CAMERA_ALPHA,
	CAMERA_COLOR,
	CAMERA_SYNC,

	CAMERA_CAMERA,
	CAMERA_COLOR_FORMAT,
	CAMERA_WIDTH,
	CAMERA_HEIGHT,

	CAMERA_COLOR_BITS,

	CAMERA_SCALE_WIDTH,
	CAMERA_SCALE_HEIGHT,

	CAMERA_FLIP_HORIZONTAL,
	CAMERA_FLIP_VERTICAL,

	CAMERA_TEXTURE,

	CAMERA_POSITION,
	CAMERA_SPEED,
	CAMERA_VOLUME,
	CAMERA_BALANCE,
	CAMERA_BRIGHTNESS,
	CAMERA_CONTRAST,
	CAMERA_SATURATION,
	CAMERA_HUE,

	CAMERA_MODE,

	CAMERA_BLENDING,

	CAMERA_STARTPLAYING,

	CAMERA_LOOP,

	CAMERA_INTERPOLATION,

	CAMERA_POSTEXX,
	CAMERA_POSTEXY,
	CAMERA_ANCTEXX,
	CAMERA_ALTTEXY,
	CAMERA_POSCUADX,
	CAMERA_POSCUADY,
	CAMERA_ANCCUADX,
	CAMERA_ALTCUADY,

	CAMERA_SHOWCAMERADATA,

	CAMERA_DISPLAY_X,
	CAMERA_DISPLAY_Y,
	CAMERA_DISPLAY_WIDTH,
	CAMERA_DISPLAY_HEIGHT
};

class moEffectCamera : public moEffect
{
public:
    moEffectCamera();
    virtual ~moEffectCamera();

    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
	void Interaction( moIODeviceManager * );
	virtual void Update( moEventList *Events );
	virtual moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

	void DrawDisplayInfo( MOfloat x, MOfloat y);

protected:

	moTextHeap	m_tScriptDebug;

  //cameras
  moCamera*   m_pCamera;
  moVideoGraph* m_pCameraGraph;
  moCaptureDevice m_CaptureDevice;
  moCaptureDevices CapDevs;

  moText          m_DeviceName;

  moTexture* m_pCameraTexture;
	moTextureAnimated* m_pAnim;
	moSound*	Sound;

	MOint startplaying;
	MOint loop;
	MOint cameramode;

	MOboolean m_bPlayingSound;

	//camera
	long            m_UserPosition;

	MOboolean				m_bDisplayOn;

	MOfloat         m_Volume;
	MOfloat         m_Balance;
	MOfloat         m_Brightness;
	MOfloat         m_Contrast;
	MOfloat         m_Hue;
	MOfloat         m_Saturation;

	MOfloat					m_FramesPerSecond;//NTSC 29.97 fps PAL 25 fps
	moEffectCameraPlayState	m_PlayState;
	moEffectCameraSeekState	m_SeekState;

	float         m_DisplayX;
	float         m_DisplayY;
	float         m_DisplayW;
	float         m_DisplayH;

	long					m_FramesLength;
	long					m_FramePosition;
	long					m_FramePositionAux;
	double					m_FramePositionF;
	double					m_FramePositionFAux;

	double                  m_FramePositionInc;

	long                    m_Ticks;
	long					m_TicksAux;

	double					m_DiffFrame;
	bool					m_bInterpolation;

    int                     m_CursorX,m_CursorY;
    bool                    showcameradata;
	//additional fields


  bool CheckIfDeviceNameExists( const moText& camera );
  bool InitDevice( moText camera );

  void UpdateParameters();
  void UpdateCamera();
};

class moEffectCameraFactory : public moEffectFactory
{
public:
    moEffectCameraFactory() {}
    virtual ~moEffectCameraFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);


};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
