/*******************************************************************************

                                moEffectMovie.h

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

#ifndef __MO_EFFECT_MOVIE__
#define __MO_EFFECT_MOVIE__

#include "moPlugin.h"
#include "moScript.h"
#include "moFilterManager.h"

#define MO_MOVIE_STOP				0
#define MO_MOVIE_PLAY				1
#define MO_MOVIE_PAUSE				2
#define MO_MOVIE_REVERSE			3
#define MO_MOVIE_FW					4
#define MO_MOVIE_RW					5
#define MO_MOVIE_FF					6
#define MO_MOVIE_FR					7
#define MO_MOVIE_SEEK				8
#define MO_MOVIE_SPEED				9
#define MO_MOVIE_PREVFRAME			10
#define MO_MOVIE_NEXTFRAME			11
#define MO_MOVIE_LOOP				12
#define MO_MOVIE_GAIN_UP			13
#define MO_MOVIE_GAIN_DOWN			14
#define MO_MOVIE_EQ_LOW_GAIN_UP		15
#define MO_MOVIE_EQ_LOW_GAIN_DOWN	16
#define MO_MOVIE_EQ_LOW_CUTOFF_UP	20
#define MO_MOVIE_EQ_LOW_CUTOFF_DOWN 21
#define MO_MOVIE_EQ_MID1_CENTER_UP 22
#define MO_MOVIE_EQ_MID1_CENTER_DOWN 23
#define MO_MOVIE_EQ_MID1_WIDTH_UP 24
#define MO_MOVIE_EQ_MID1_WIDTH_DOWN 25
#define MO_MOVIE_EQ_MID1_GAIN_UP 26
#define MO_MOVIE_EQ_MID1_GAIN_DOWN 27
#define MO_MOVIE_EQ_MID2_CENTER_UP 28
#define MO_MOVIE_EQ_MID2_CENTER_DOWN 29
#define MO_MOVIE_EQ_MID2_WIDTH_UP 30
#define MO_MOVIE_EQ_MID2_WIDTH_DOWN 31
#define MO_MOVIE_EQ_MID2_GAIN_UP 32
#define MO_MOVIE_EQ_MID2_GAIN_DOWN 33
#define MO_MOVIE_EQ_HIGH_GAIN_UP 34
#define MO_MOVIE_EQ_HIGH_GAIN_DOWN 35
#define MO_MOVIE_EQ_HIGH_CUTOFF_UP 36
#define MO_MOVIE_EQ_HIGH_CUTOFF_DOWN 37

#ifndef MO_RED
#define MO_RED      0
#define MO_GREEN    1
#define MO_BLUE     2
#define MO_ALPHA    3
#endif

#define MO_MOVIE_EOF_TOLERANCE 7


enum moEffectMovieLaunch {
  MO_MOVIE_LAUNCH_OFF = 0, /** by default */
  MO_MOVIE_LAUNCH_ON = 1 /**  */
};

enum moEffectMovieMode {
	MO_MOVIE_MODE_VCR = 0,
	MO_MOVIE_MODE_SCRIPT = 1,
	MO_MOVIE_MODE_CYCLE = 2,
	MO_MOVIE_MODE_VCR_PLAYLIST = 3,
	MO_MOVIE_MODE_LAUNCH_AND_WAIT = 4,
	MO_MOVIE_MODE_LAUNCH_NEXT_AND_WAIT = 5
};

enum moEffectMoviePlayState {
	MO_MOVIE_PLAYSTATE_STOPPED = 0,
	MO_MOVIE_PLAYSTATE_PLAYING = 1,
	MO_MOVIE_PLAYSTATE_REVERSE = 2,
	MO_MOVIE_PLAYSTATE_SPEED = 3,
	MO_MOVIE_PLAYSTATE_PAUSED = 4,
	MO_MOVIE_PLAYSTATE_SEEKING = 5
};


enum moEffectMovieSeekState {
	MO_MOVIE_SEEKSTATE_SEEKING = 0,
	MO_MOVIE_SEEKSTATE_REACHED = 1,
	MO_MOVIE_SEEKSTATE_PLAYING = 2,
	MO_MOVIE_SEEKSTATE_PAUSED = 3
};

enum moEffectMovieVCRCommand {
	MO_MOVIE_VCR_STOP = 0,//Stop
	MO_MOVIE_VCR_PLAY = 1,//Play
	MO_MOVIE_VCR_PAUSE = 2,//Stop
	MO_MOVIE_VCR_REVERSE = 3,//Reverse
	MO_MOVIE_VCR_FW = 4,//Forward
	MO_MOVIE_VCR_RW = 5,//Rewind
	MO_MOVIE_VCR_FF = 6,//Fast Forward
	MO_MOVIE_VCR_FR = 7,//Fast Rewind
	MO_MOVIE_VCR_SEEK = 8,//Seek for a frame
	MO_MOVIE_VCR_SPEED = 9,//Play at speed
	MO_MOVIE_VCR_PREVFRAME = 10,//move to prev frame
	MO_MOVIE_VCR_NEXTFRAME = 11,//move to next frame
	MO_MOVIE_VCR_LOOP = 12//loop
};


enum moMovieParamIndex {
	MOVIE_ALPHA,
	MOVIE_COLOR,
	MOVIE_SYNC,
	MOVIE_MOVIES,
	MOVIE_SOUNDS,
	MOVIE_POSITION,
	MOVIE_SPEED,
	MOVIE_VOLUME,
	MOVIE_BALANCE,
	MOVIE_BRIGHTNESS,
	MOVIE_CONTRAST,
	MOVIE_SATURATION,
	MOVIE_HUE,
	MOVIE_MODE,
	MOVIE_LAUNCH,
	MOVIE_BLENDING,
	MOVIE_STARTPLAYING,
	MOVIE_LOOP,
	MOVIE_SHOWTRACKDATA,
	MOVIE_INTERPOLATION,
	MOVIE_POSTEXX,
	MOVIE_POSTEXY,
	MOVIE_ANCTEXX,
	MOVIE_ALTTEXY,
	MOVIE_POSCUADX,
	MOVIE_POSCUADY,
	MOVIE_ANCCUADX,
	MOVIE_ALTCUADY,
	MOVIE_SHOWMOVIEDATA,
	MOVIE_DISPLAY_X,
	MOVIE_DISPLAY_Y,
	MOVIE_DISPLAY_WIDTH,
	MOVIE_DISPLAY_HEIGHT,
	MOVIE_NO_AUDIO_OUTPUT,
	MOVIE_NO_VIDEO_OUTPUT,
	MOVIE_NO_TEXT_OUTPUT,
	MOVIE_INLET,
	MOVIE_OUTLET
};

/*
#define MO_TRACKER_PROCESSSIGNAL			94
#define MO_TRACKER_SMOOTHDIFF   			95
#define MO_TRACKER_REMOVENOISE   			96
#define MO_TRACKER_AREAFILTER   			97
#define MO_TRACKER_HEURISTICFILTER 			98
#define MO_TRACKER_DISCRETIZE				99
#define MO_TRACKER_CALIBRATION				100
#define MO_TRACKER_NOISECOEFF_UP			101
#define MO_TRACKER_NOISECOEFF_DOWN			102
#define MO_TRACKER_AREACOEFF_UP				103
#define MO_TRACKER_AREACOEFF_DOWN			104
#define MO_TRACKER_NOISECOEFF    			105
#define MO_TRACKER_AREACOEFF    			106
*/
class moEffectMovie : public moEffect
{

  public:

    moEffectMovie();
    virtual ~moEffectMovie();

    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
    MOboolean Finish();
    virtual void Activate();
    virtual void Deactivate();
    void Interaction( moIODeviceManager * );
    virtual void Update( moEventList *Events );
    virtual moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

    // Public script functions.
    virtual int ScriptCalling(moLuaVirtualMachine& vm, int iFunctionNumber);
    void HandleReturns(moLuaVirtualMachine& vm, const char *strFunc);
    virtual void ScriptExeRun();
    virtual void ScriptExeInit();
    virtual void ScriptExeDraw();
    virtual void ScriptExeUpdate();

    void DrawDisplayInfo( MOfloat x, MOfloat y);

  protected:

    moTextHeap	m_tScriptDebug;

    //movies
    moTexture* m_pTexture;
    moMovie*   m_pMovie;
    moTextureAnimated* m_pAnim;
    moSound*	m_pSound;

    moTexture* m_pNextTexture;
    moMovie*   m_pNextMovie;


    MOint startplaying;
    MOint loop;
    moEffectMovieMode moviemode;
    moEffectMovieLaunch movielaunchon;
    bool movielaunched;

    MOboolean m_bPlayingSound;

    moGsGraph m_SoundSystem;

    //movie
    long            m_UserPosition;

    MOboolean				m_bDisplayOn;
    MOboolean				m_bStartPlayingOn;
    MOboolean				m_bLoop;//true: loop false: no loop
    MOfloat					m_PlaySpeed;//1X = 1.0   2X = 2.0

    MOfloat         m_Volume;
    MOfloat         m_Balance;
    MOfloat         m_Brightness;
    MOfloat         m_Contrast;
    MOfloat         m_Hue;
    MOfloat         m_Saturation;

    MOfloat					m_FramesPerSecond;//NTSC 29.97 fps PAL 25 fps
    moEffectMoviePlayState	m_PlayState;
    moEffectMovieSeekState	m_SeekState;

    float         m_DisplayX;
    float         m_DisplayY;
    float         m_DisplayW;
    float         m_DisplayH;

    long					m_FramesLength;
    long					m_FramePosition;
    long					m_FramePositionAux;
    double					m_FramePositionF;
    double					m_FramePositionFAux;

    moOutlet*     m_pOutletFramePosition;
    moInlet*     m_pInletFramePosition;

    double                  m_FramePositionInc;

    long                    m_Ticks;
    long					m_TicksAux;

    double					m_DiffFrame;
    bool					m_bInterpolation;

    int                     m_CursorX,m_CursorY;
    bool                    showmoviedata;
    //additional fields

    // CUSTOM 1
    moText m_MovieScriptFN;
    //TRACKER
    MOboolean				m_bTrackerInit;
    int tracker_width,tw;
    int tracker_height,th;

    MOdouble PosTextX,  AncTextX,  PosTextY,  AltTextY;
    MOdouble PosTextX0, PosTextX1, PosTextY0, PosTextY1;
    MOdouble PosCuadX,  AncCuadX,  PosCuadY,  AltCuadY;
    MOdouble PosCuadX0, PosCuadX1, PosCuadY0, PosCuadY1;
    /*
    moTrackerKLTSystemData*	m_pTrackerData;
    moTrackerGpuKLTSystemData*	m_pTrackerGpuData;
    */
    moTrackerSystemData*	m_pTrackerData;

    // For indexing scripted methods.
    int m_iMethodBase;

    void DrawTrackerFeatures();

    //CUSTOM FUNCTIONS
    MOuint MovieGLId();

    /** Assign Movie or Animation texture object
    * Updating Movie/Video/Circular/Texture Buffers
    * @param p_Texture base texture from config
    */
    MOboolean UpdateMovieTexture(moTexture* p_Texture);
    void UpdateParameters();
    void UpdateMoviePosition();
    void UpdateFilterParameters(MOboolean p_ReBalance);
    void UpdateInlets();
    void UpdateOutlets();

    void SpeedRegulation( MOfloat p_fMinSpeed, MOfloat p_fMaxSpeed );
    void VCRCommand( moEffectMovieVCRCommand p_Command, MOint p_iValue=0, MOfloat p_fValue = 0.0 );
    void VCRPlaySpeed();

    moEffectMoviePlayState VCRState();

  private:
    // Script functions.
    void RegisterFunctions();
    int GetFeature(moLuaVirtualMachine& vm);
    int SpeedRegulation(moLuaVirtualMachine& vm);
    int VCRStop(moLuaVirtualMachine& vm);
    int VCRPlay(moLuaVirtualMachine& vm);
    int VCRPause(moLuaVirtualMachine& vm);
    int VCRReverse(moLuaVirtualMachine& vm);
    int VCRForward(moLuaVirtualMachine& vm);
    int VCRRewind(moLuaVirtualMachine& vm);
    int VCRFForward(moLuaVirtualMachine& vm);
    int VCRFRewind(moLuaVirtualMachine& vm);
    int VCRSeek(moLuaVirtualMachine& vm);
    int VCRSpeed(moLuaVirtualMachine& vm);
    int VCRPrevFrame(moLuaVirtualMachine& vm);
    int VCRNextFrame(moLuaVirtualMachine& vm);
    int VCRLoop(moLuaVirtualMachine& vm);
    int VCRPlaySpeed(moLuaVirtualMachine& vm);
    int GetFramePosition(moLuaVirtualMachine& vm);
    int GetFramePositionF(moLuaVirtualMachine& vm);
    int GetPlaySpeed(moLuaVirtualMachine& vm);
    int SetFramePosition(moLuaVirtualMachine& vm);
    int SetFramePositionF(moLuaVirtualMachine& vm);
    int SetPlaySpeed(moLuaVirtualMachine& vm);
    int GetTicks(moLuaVirtualMachine& vm);
    int DrawLine(moLuaVirtualMachine& vm);
    int PushDebugString(moLuaVirtualMachine& vm);
};

class moEffectMovieFactory : public moEffectFactory
{
  public:
    moEffectMovieFactory() {}
    virtual ~moEffectMovieFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);

};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
