/*******************************************************************************

                                moKinect.h

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

  Copyright(C) 2011 Fabricio Costa

  Authors:
  Fabricio Costa


*******************************************************************************/

#ifndef __MO_KINECT_H
#define __MO_KINECT_H


//#define KINECT_PCL
#define KINECT_OPENNI


#include <iostream>


/**
*  KINECT OPENNI
*/
#ifdef KINECT_OPENNI
    /*C wrapper*/
    #include "XnOpenNI.h"
    /*C** wrapper*/
    #include <XnCodecIDs.h>
    #include "XnCppWrapper.h"

#endif


#include "moTypes.h"
/**

MOLDEO

*/

#include "moConfig.h"
#include "moDeviceCode.h"
#include "moEventList.h"
#include "moIODeviceManager.h"


enum moKinectParamIndex {

	KINECT_OFFSET_MIN=0,
	KINECT_OFFSET_MAX,

	KINECT_OFFSET_LEFT,
	KINECT_OFFSET_RIGHT,
	KINECT_OFFSET_TOP,
	KINECT_OFFSET_BOTTOM,

	KINECT_OFFSET_RADIUS,


	KINECT_REF_POINT_DEEP,
	KINECT_REF_POINT_WIDTH,
	KINECT_REF_POINT_HEIGHT,

	KINECT_REF_POINT_X,
	KINECT_REF_POINT_Y,
	KINECT_REF_POINT_Z,
	KINECT_REF_POINT_TILT,

	KINECT_OBJECT_RAD_MIN,
	KINECT_OBJECT_RAD_MAX,
	KINECT_CALIBRATE_BASE,
	KINECT_BASE_CAMERA_I,
	KINECT_BASE_CAMERA_J,

	KINECT_UMBRAL_CAMERA_PRESENCIA_Z_NEAR,
	KINECT_UMBRAL_CAMERA_PRESENCIA_Z_FAR,
	KINECT_UMBRAL_CAMERA_PRESENCIA_Y_OFFSET,

    KINECT_UMBRAL_TRACKEO_Y_OFFSET,
    KINECT_UMBRAL_TRACKEO_Z_NEAR,
    KINECT_UMBRAL_TRACKEO_Z_FAR,
    KINECT_TRACKEO_FACTOR_ANGULO,

    KINECT_UMBRAL_OBJETO_Z_NEAR,

    KINECT_VIEW_SCALEFACTOR,
	KINECT_VIEW_FOVY,

	KINECT_OBJECT_COLOR1_TOP,
	KINECT_OBJECT_COLOR1_BOTTOM,

	KINECT_OBJECT_COLOR2_TOP,
	KINECT_OBJECT_COLOR2_BOTTOM,

	KINECT_OBJECT_COLOR3_TOP,
	KINECT_OBJECT_COLOR3_BOTTOM,

	KINECT_OBJECT_MIN_SURFACE,
	KINECT_OBJECT_MAX_SURFACE,

	KINECT_OBJECT_MIN_VOLUME,
	KINECT_OBJECT_MAX_VOLUME,

	KINECT_OBJECT_TEXTURE1,
	KINECT_OBJECT_TEXTURE2,
	KINECT_OBJECT_TEXTURE3,

	KINECT_UPDATE_ON
};

//Buttons
#define KINECT_BUTTON_LEFT 0
#define KINECT_BUTTON_MIDDLE 1
#define KINECT_BUTTON_RIGHT 2
//Wheels
#define MOTION_REL_X 0
#define MOTION_REL_Y 1
//Position
#define POSITION_X 0
#define POSITION_Y 1

//channel0virtual
#define SDL_MOTION_REL_X_LEFT	0
#define SDL_MOTION_REL_Y_LEFT	1
#define SDL_MOTION_REL_X_MIDDLE	2
#define SDL_MOTION_REL_Y_MIDDLE	3
#define SDL_MOTION_REL_X_RIGHT	4
#define SDL_MOTION_REL_Y_RIGHT	5
#define SDL_MOTION_POS_X		6
#define SDL_MOTION_POS_Y		7
/*
class  moKinectButton {
public:
	MOboolean pushed;
	SDLMod pushedmod;
	MOboolean released;
	SDLMod releasedmod;
	MOint pressed;
	SDLMod pressedmod;

	moKinectButton() {
		pushed = MO_FALSE;
		pushedmod = KMOD_NONE;
		released = MO_FALSE;
		releasedmod = KMOD_NONE;
		pressed = 0;
		pressedmod = KMOD_NONE;
	}

	MOboolean Init() {
		pushed = MO_FALSE;
		pushedmod = KMOD_NONE;
		released = MO_FALSE;
		releasedmod = KMOD_NONE;
		pressed = 0;
		pressedmod = KMOD_NONE;
		return true;
	}

	void Push(SDLMod mod) {
		pushed = MO_TRUE;
		pushedmod =  mod;
		released = MO_FALSE;
		releasedmod = KMOD_NONE;
		pressedmod = KMOD_NONE;
	}

	MOint Release(SDLMod mod) {
		released = MO_TRUE;
		releasedmod = mod;
		//se soltó pero nunca se pulsó(es un error) o no coincide el mapa de keys especiales
		if((pushed == MO_FALSE) ||(releasedmod!=pushedmod)) {
			Init();
			return MO_FALSE;
		} else {
			pressed = pressed + 1;
			pressedmod = mod;
			pushed = MO_FALSE;
			pushedmod = KMOD_NONE;
			released = MO_FALSE;
			releasedmod = KMOD_NONE;
			return MO_TRUE;
		}
	}
};

class  moKinectWheel {
public:
	MOboolean motion;//verdadero si se movio
	MOint rel;
	MOint kinectmod;

	moKinectWheel() {
		motion = MO_FALSE;
		rel = 0;
		kinectmod = 0;
	}

	MOboolean Init() {
		motion = MO_FALSE;
		rel = 0;
		kinectmod = 0;
		return true;
	}

	void Motion(MOint r) {
		if(r!=0) {
			rel = r;
			motion = MO_TRUE;
		}
	}
};
*/
class  moKinectPosition {
public:
	MOboolean motion;//verdadero si se movio
	MOint pos;
	MOint kinectmod;

	moKinectPosition() {
		motion = MO_FALSE;
		pos = 0;
	}

	MOboolean Init() {
		motion = MO_FALSE;
		pos = 0;
		return true;
	}

	void Motion(MOint r) {
		if(r!=pos) {
			pos = r;
			motion = MO_TRUE;
		}
	}
};


class  moKinectCode {
public:
	moText strcod;
	MOint code;
	MOint kinectcod;//mapeo type SDL
	MOint type;//pushedr:0 ,prendedor:1, selector:2, seteador:3,...otros
	MOint especiales;//keys especiales, como alt, caps lock, etc...
	MOint state;//0 MO_FALSE(no esta activo el device) MO_ON(está on) MO_OFF(está off)
	MOint value;//depende
};



/**
* the KinectPosture is a vector representation of a kinect posture
* moKinectPosture constructs itself with the real skeleton position set
*
*
*

Indicadores para el sensado de señas.... (KINECT)

    1) posicion 2D de las 2 manos relativa al cuello (x abs, y abs)
    2) distancia entre las 2 manos
    3) movimiento bidimensional de cada mano (x,y) (posicion relativa a la posicion anterior (o dos anteriores?) )
    4) posicion 2d de ambos codos relativos al cuello
    5) velocidad absoluta de cada mano
    6) la distancia absoluta de cada mano al cuello
    7) velocidad normalizada absoluta (2d) de cada mano...

Movimientos:

Sobre una sola mano que queda inmóvil (“Alemania (Int.)”, “Boca”)
• Sobre una sola mano que se mueve (“Ahora”, “Blanco”, “Cereza”)
• Sobre las dos manos simétricas que se mueven (“Contacto”, “Reino”)
• Sobre las dos manos asimétricas. La mano "dominante" actúa sobre la otra
(mano base o "pasiva"( que no se mueve) (“Norma”, “Rana”).



        XN_SKEL_HEAD                        = 1,
        XN_SKEL_NECK                        = 2,
        XN_SKEL_TORSO                        = 3,
        XN_SKEL_WAIST                        = 4,

        XN_SKEL_LEFT_COLLAR                = 5,
        XN_SKEL_LEFT_SHOULDER        = 6,
        XN_SKEL_LEFT_ELBOW                = 7,
        XN_SKEL_LEFT_WRIST                = 8,
        XN_SKEL_LEFT_HAND                = 9,
        XN_SKEL_LEFT_FINGERTIP        =10,

        XN_SKEL_RIGHT_COLLAR        =11,
        XN_SKEL_RIGHT_SHOULDER        =12,
        XN_SKEL_RIGHT_ELBOW                =13,
        XN_SKEL_RIGHT_WRIST                =14,
        XN_SKEL_RIGHT_HAND                =15,
        XN_SKEL_RIGHT_FINGERTIP        =16,

        XN_SKEL_LEFT_HIP                =17,
        XN_SKEL_LEFT_KNEE                =18,
        XN_SKEL_LEFT_ANKLE                =19,
        XN_SKEL_LEFT_FOOT                =20,

        XN_SKEL_RIGHT_HIP                =21,
        XN_SKEL_RIGHT_KNEE                =22,
        XN_SKEL_RIGHT_ANKLE                =23,
        XN_SKEL_RIGHT_FOOT                =24

*/

enum moKinectGestureZone {

  /**body*/
  MO_GEST_ZONE_HAND_LEFT=0,
  MO_GEST_ZONE_HAND_RIGHT,

  MO_GEST_ZONE_ELBOW_LEFT,
  MO_GEST_ZONE_ELBOW_RIGHT,

  MO_GEST_ZONE_SHOULDER_LEFT,
  MO_GEST_ZONE_SHOULDER_RIGHT,

  MO_GEST_ZONE_FOREARM_RIGHT,/** calculated*/
  MO_GEST_ZONE_FOREARM_LEFT,/**calculated*/

  MO_GEST_ZONE_ARM_RIGHT,/** calculated*/
  MO_GEST_ZONE_ARM_LEFT,/**calculated*/

  MO_GEST_ZONE_CHEST_LEFT,
  MO_GEST_ZONE_CHEST_CENTER,
  MO_GEST_ZONE_CHEST_RIGHT,
  MO_GEST_ZONE_CHEST_ABDOMINAL,

  /**face*/
  MO_GEST_ZONE_HEAD,/*calculated*/
  MO_GEST_ZONE_FACE,
  MO_GEST_ZONE_NECK,
  MO_GEST_ZONE_COLLAR,

  /**space*/
  MO_GEST_ZONE_FACE_FRONT,
  MO_GEST_ZONE_HEAD_LEFT,
  MO_GEST_ZONE_HEAD_RIGHT,

  MO_GEST_ZONE_BODY_FRONT,
  MO_GEST_ZONE_BODY_FRONT_LEFT,
  MO_GEST_ZONE_BODY_FRONT_CENTER,
  MO_GEST_ZONE_BODY_FRONT_RIGHT,

  /**performatic*/
  MO_GEST_ZONE_WRIST_LEFT,
  MO_GEST_ZONE_WRIST_RIGHT,
  MO_GEST_ZONE_HIP_RIGHT,
  MO_GEST_ZONE_HIP_LEFT,
  MO_GEST_ZONE_ANKLE_LEFT,
  MO_GEST_ZONE_ANKLE_RIGHT,
  MO_GEST_ZONE_FOOT_RIGHT,
  MO_GEST_ZONE_FOOT_LEFT,

  MO_GEST_ZONE_MAX

};

enum moKinectElement {


  /**basic*/
  MO_KIN_CONFIDENCE=0,
  MO_KIN_REL_HAND_LEFT,
  MO_KIN_REL_HAND_RIGHT,
  MO_KIN_REL_ELBOW_LEFT,
  MO_KIN_REL_ELBOW_RIGHT,
  MO_KIN_REL_SHOULDER_LEFT,
  MO_KIN_REL_SHOULDER_RIGHT,


  /**distances*/
  MO_KIN_DIST_ABS_INTER_HANDS,
  MO_KIN_DIST_ABS_HAND_LEFT_REL,
  MO_KIN_DIST_ABS_HAND_RIGHT_REL,

  /**speeds*/
  MO_KIN_SPEED_ABS_HAND_LEFT,
  MO_KIN_SPEED_ABS_HAND_RIGHT,
  MO_KIN_SPEED_ABS_HAND_LEFT_N,
  MO_KIN_SPEED_ABS_HAND_RIGHT_N,
  MO_KIN_SPEED_PROP_HAND_LEFT,
  MO_KIN_SPEED_PROP_HAND_RIGHT,

  /**zones*/
  MO_KIN_ZONE_HAND_LEFT, /** body space relative */
  MO_KIN_ZONE_HAND_RIGHT, /** body space relative */
  MO_KIN_ZONE_HAND_LEFT_REL, /** body parts relative */
  MO_KIN_ZONE_HAND_RIGHT_REL, /** body parts relative */

  MO_KIN_MAX

};

#define MO_KIN_RING_MAX 30

moDeclareDynamicArray( moVector3f, moVector3fs );


class moKinectPosture : public moAbstract {

  public:

    moKinectPosture() {
    }
    moKinectPosture( const moKinectPosture& src ) {
      (*this) = src;
    }
    virtual ~moKinectPosture() {
    }
    moKinectPosture& operator=( const moKinectPosture& src ) {
      m_PostureVector = src.m_PostureVector;
      m_PostureVectorOld = src.m_PostureVectorOld;
      return (*this);
    }

    virtual bool Init( int p_PostureMax=0 ) {
      if (p_PostureMax==0) p_PostureMax = MO_KIN_MAX+1;
      m_PostureVector.Init( p_PostureMax, moVector3f( 0.0f, 0.0f, 0.0f) );
      m_PostureVectorOld = m_PostureVector;
      return (m_bInitialized=true);
    }

    void Calculate( XnUserID player );

    moVector3fs  m_PostureVector;
    moVector3fs  m_PostureVectorOld;

};

moDeclareDynamicArray( moKinectPosture, moKinectPostures);

class moKinectGestureRule : public moAbstract {

  public:
    moKinectGestureRule() {
      //m_KinectElement=0;
      m_NestedRule = NULL;
    }
    virtual ~moKinectGestureRule() {}
    moKinectGestureRule( const moKinectGestureRule& src) {
      (*this) = src;
    }

    moKinectGestureRule(  moKinectElement p_Element, const moVector3f& p_VectorElement, const moVector3f& p_Min, const moVector3f& p_Max ) {
      m_KinectElement = p_Element;
      m_VectorElement = p_VectorElement;
      m_MinElement = p_Min;
      m_MaxElement = p_Max;
    }


    moKinectGestureRule& operator=( const moKinectGestureRule& src ) {
      m_KinectElement = src.m_KinectElement;
      m_VectorElement = src.m_VectorElement;
      m_MinElement = src.m_MinElement;
      m_MaxElement = src.m_MaxElement;
      return (*this);
    }

    bool Evaluate( const moKinectPosture& p_Posture ) {

      bool valid = true;

      moVector3f V,VMIN, VMAX;
      V = p_Posture.m_PostureVector.Get((int)(m_KinectElement));
      V =  moVector3f( V.X()*m_VectorElement.X(), V.Y()*m_VectorElement.Y(), V.Z()*m_VectorElement.Z() );
      VMIN = moVector3f( m_MinElement.X()*m_VectorElement.X(), m_MinElement.Y()*m_VectorElement.Y(), m_MinElement.Z()*m_VectorElement.Z() );
      VMAX = moVector3f( m_MaxElement.X()*m_VectorElement.X(), m_MaxElement.Y()*m_VectorElement.Y(), m_MaxElement.Z()*m_VectorElement.Z() );

      //MODebug2->Message( "Evaluate: VMIN.X: " + FloatToStr(VMIN.X()) + " V.X:"+FloatToStr(V.X()) + " VMAX.X:"+FloatToStr(VMAX.X()) );
      valid = valid && ( VMIN.X() <= V.X() );
      valid = valid && (VMAX.X() >= V.X() );

      //MODebug2->Message( "Evaluate: VMIN.Y: " + FloatToStr(VMIN.Y()) + " V.Y:"+FloatToStr(V.Y()) + " VMAX.Y:"+FloatToStr(VMAX.Y()) );
      valid = valid && (VMIN.Y() <= V.Y() );
      valid = valid && (VMAX.Y() >= V.Y() );

      //MODebug2->Message( "Evaluate: VMIN.Z: " + FloatToStr(VMIN.Z()) + " V.Z:"+FloatToStr(V.Z()) + " VMAX.Z:"+FloatToStr(VMAX.Z()) );
      valid = valid && (VMIN.Z() <= V.Z() );
      valid = valid && (VMAX.Z() >= V.Z() );

      return valid;

    }


    moKinectElement   m_KinectElement;  /// element to be mesured...
    moVector3f        m_VectorElement;  /// (1,0,0) for only X, (0,1,0) for only Y,
    moVector3f        m_MinElement; /// 0.25, 0.0, 0.0
    moVector3f        m_MaxElement; /// 0.75, 0.0, 0.0

    moKinectGestureRule*  m_NestedRule;
};

moDeclareDynamicArray( moKinectGestureRule, moKinectGestureRules);


class moKinectGestureInterval : public moAbstract {

  public:

    moKinectGestureInterval() {
      frames = 0;
      in = 0;
      out = 0;
      count = 0;
    }

    moKinectGestureInterval( int p_frames, int p_in, int p_out ) {
      frames = p_frames;
      in = p_in;
      out = p_out;
      count = 0;

    }

    virtual ~moKinectGestureInterval() {

    }

    moKinectGestureInterval( const moKinectGestureInterval& src) {
      (*this) = src;
    }

    moKinectGestureInterval& operator=( const moKinectGestureInterval& src) {
      frames = src.frames;
      in = src.in;
      out = src.out;
      count = src.count;
      return (*this);
    }

    /**
    * Return true (trigger an event) only if parameter is true and count==frames
    */
    bool IsValid( bool p_validrules ) {
      if (!p_validrules) {
        //not valid... reset
        count = 0;
        return false;
      } else {

        count++;

        if (count==frames) {
          //OK! trigger!
          count = 0;
          return true;
        }
        /* else {
          //reset
          count = 0;
          return false;
        }
        */

      }
    }

    int frames;
    int in;
    int out;

    int count;

};

/**
*
* This class store a gesture: an array of postures in an ideal 25 fps rate.
*
*/

class moKinectGesture : public moAbstract {

  public:

    moKinectGesture() {
      m_ValidRules = false;
      m_Active = true;
    }
    moKinectGesture( const moKinectGesture& src ) {
      (*this) = src;
    }
    virtual ~moKinectGesture() {
    }

    virtual bool Init( int p_MaxPostures ) {
      m_Postures.Init( p_MaxPostures, moKinectPosture()  );
      return (m_bInitialized = true);
    }

    moKinectGesture& operator=( const moKinectGesture& src ) {
      m_Postures = src.m_Postures;
      m_Rules = src.m_Rules;
      m_Interval = src.m_Interval;
      m_GestureEvent = src.m_GestureEvent;
      m_ValidRules = src.m_ValidRules;
      m_Active = src.m_Active;
      return (*this);
    }

    void Set( int p_position, moKinectPosture& p_Posture ) {
      m_Postures.Set( p_position, p_Posture );
    }

    void AddGestureRule( moKinectElement p_Element, const moVector3f& p_VectorElement, const moVector3f& p_Min, const moVector3f& p_Max ) {
      AddGestureRule(  moKinectGestureRule( p_Element, p_VectorElement, p_Min, p_Max ) );
    }

    void AddGestureRule( const moKinectGestureRule& p_Rule ) {
      m_Rules.Add( p_Rule );
    }

    bool EvaluateRules( moKinectPosture& p_Posture ) {
        bool valid = true;
        bool validall = true;
        bool sendmessage = false;
        moKinectGestureRule GRule;

        for( int rule = 0; rule < m_Rules.Count(); rule++) {

          GRule = m_Rules[ rule ];
          valid = GRule.Evaluate( p_Posture );

          //MODebug2->Message("rule: " + IntToStr(rule) + " valid? " + IntToStr(valid) );

          validall = validall && valid;
        }

        m_ValidRules = validall;
        sendmessage = m_Interval.IsValid(m_ValidRules);

        //MODebug2->Message("valid: " + IntToStr(validall) + " count: " + IntToStr(m_Interval.count) + " / " + IntToStr(m_Interval.frames) );

        return sendmessage;
    }
/*
    Start() //interaval
    Validate()
    End()
*/

    bool                      m_Active;
    bool                      m_ValidRules;
    moText                    m_GestureEvent; ///HAND_SLIDE_RIGHT, HAND_SLIDE_LEFT
    moKinectPostures          m_Postures;
    moKinectGestureRules      m_Rules;
    moKinectGestureInterval   m_Interval;


};

moDeclareDynamicArray( moKinectGesture, moKinectGestures);

/**
*
*
*
*/
class moKinectGestureRecognition : public moAbstract {

  public:

    moKinectGestureRecognition() {}
    virtual ~moKinectGestureRecognition() {}

    virtual bool Init( int p_RingMax = 0 ) {

      m_ActualPosture.Init();

      m_RingPosition = 0;
      if (p_RingMax==0) p_RingMax = MO_KIN_RING_MAX + 1;
      m_RingMaxPosition = p_RingMax;

      m_RingGesture.Init( m_RingMaxPosition );

      return (m_bInitialized = true);
    }

    /**
    * Add a gesture to be recognized in the Gesture buffer.
    */
    void AddGesture( moKinectGesture& gesture ) {
      m_Gestures.Add( gesture );
    }

    /**
    * Record actual gesture in the ring buffer
    */
    void RecordPosture( moKinectPosture& posture ) {
      m_RingGesture.Set( m_RingPosition, posture );
    }

    /**
    * Update actual posture for user i
    */
    bool UpdateUser( XnUserID p_user ) {

      m_ActualPosture.Calculate( p_user );
      m_RingGesture.Set( m_RingPosition, m_ActualPosture );
      m_RingPosition++;
      if (m_RingPosition>=m_RingMaxPosition) {
        m_RingPosition = 0;
      }

      Recognize();
      return true;
    }


    bool Recognize() {

      m_DataMessage.Empty();

      for(int g=0; g<m_Gestures.Count(); g++) {

        //MODebug2->Message("ANALYSING GESTURE: " + IntToStr(g) + "  " + m_Gestures[g].m_GestureEvent );

        if ( m_Gestures[g].EvaluateRules( m_ActualPosture ) ) {

          ///Send Event Message!!!
          MODebug2->Message("SENDING MESSAGE FROM GESTURES!!!!**************************************************");
          MODebug2->Message("GESTURE: " + m_Gestures[g].m_GestureEvent);

          moData actual_data;
          actual_data.SetText( m_Gestures[g].m_GestureEvent );
          m_DataMessage.Add( actual_data );

        }

      }
      // algun callback???

      return true;
    }


    moDataMessage   m_DataMessage;

    int m_RingPosition;
    int m_RingMaxPosition;

    ///Store the actual gesture in motion
    moKinectGesture m_RingGesture;

    ///Store the recorded gestures for recognition comparison
    moKinectGestures m_Gestures;

    ///The structure storing actual posture...
    moKinectPosture m_ActualPosture;

};


class  moKinect : public moIODevice {

public:

    moKinect();
    virtual ~moKinect();

	MOboolean		Init();
	MOboolean		Finish();

    void			Update(moEventList*);
    MOswitch		GetStatus( MOdevcode);
    MOswitch		SetStatus( MOdevcode,MOswitch);
    MOint			GetValue( MOdevcode);
    MOdevcode		GetCode( moText);

    virtual moConfigDefinition *GetDefinition( moConfigDefinition *p_configdefinition );

    void UpdateParameters();

    moVector3f  m_ReferencePoint;
    moVector3f  m_ReferencePointDimension;

    moVector2f  m_Offset;
    moVector4f  m_OffsetBox;
    float  m_OffsetRadius;

    float       m_RadMin,m_RadMax;

    moVector3f  m_CenterPoint;

    moVector3f*  pCloud;
    moVector3f*  pCloudRGB;
    moVector3f*  pCloudDif;

    int calibrate_base;
    int base_camera_i;
    int base_camera_j;

    int umbral_camera_presencia_z_near;
    int umbral_camera_presencia_z_far;
    int umbral_camera_presencia_y_offset;

    int umbral_trackeo_y_offset;
    int umbral_trackeo_z_near;
    int umbral_trackeo_z_far;
    float trackeo_factor_angulo;

    int umbral_objeto_z_near;

    float scaleFactor;
    float fovy;

    int m_haypresencia;

    moVector3f BaseNormal;

    moVector3f m_BasePosition;
    moVector3f m_BaseRGB;
    moVector3f m_BaseHSV;

    moVector3f m_TargetPosition;
    moVector3f m_TargetRGB;
    moVector3f m_TargetHSV;

    moVector4d m_Color1Bottom;
    moVector4d m_Color1Top;

    moVector4d m_Color2Bottom;
    moVector4d m_Color2Top;

    moVector4d m_Color3Bottom;
    moVector4d m_Color3Top;

    float m_MinSurface;
    float m_MaxSurface;

    float m_MinVolume;
    float m_MaxVolume;


    int m_fbo_idx;
    MOuint attach_point;
    moFBO* pFBO;

    bool show_callback;

    moLock  m_DataLock;

    int update_on;

    int NDIF1;
    int NDIF2;
    int NDIF3;
    int SURFACE1;
    int SURFACE2;
    int SURFACE3;

    int SURFACE;
    int VOLUME;

private:

    moConfig config;


	/** OPENNI KINECT*/
	//moKinectButton Buttons[3];
	//moKinectWheel Wheels[2];
	moKinectPosition Position[2];

	moKinectCode *Codes;
	MOuint ncodes;

	//BUFFER DE POSICIONES ANTERIORES
	//MOint nchars;
	//MOint Text[256];
	//MOswitch TextActive;

	//CONSOLA VIRTUAL
	moConfig configcv;
	MOuint channel0virtual[6];//simulamos rueditas para el channel virtual
	MOuint masterchannelvirtual[6];//simulamos rueditas para el channel virtual

#ifdef KINECT_OPENNI
    xn::Context m_Context;
    xn::DepthGenerator m_Depth;
    xn::ImageGenerator m_RGBImage;
    xn::IRGenerator m_IRImage;

    XnDepthPixel m_DepthPixel;
    XnStatus m_nRetVal;

    bool DrawLimb( XnUserID player, XnSkeletonJoint eJoint1, XnSkeletonJoint eJoint2);
    void drawCircle(float x, float y, float radius);
    void DrawJoint(XnUserID player, XnSkeletonJoint eJoint);

    void DrawGestureRecognition();

    moOutlet* m_RightHand;
    moOutlet* m_RightHandX;
    moOutlet* m_RightHandY;
    moOutlet* m_RightHandZ;
    moOutlet* m_RightHandC;

    moOutlet* m_LeftHand;
    moOutlet* m_LeftHandX;
    moOutlet* m_LeftHandY;
    moOutlet* m_LeftHandZ;
    moOutlet* m_LeftHandC;

    moOutlet* m_Head;
    moOutlet* m_HeadX;
    moOutlet* m_HeadY;
    moOutlet* m_HeadZ;
    moOutlet* m_HeadC;

    moOutlet* m_Gesture;

    moVector4d m_VRightHand;
    moVector4d m_VLeftHand;
    moVector4d m_VHead;

    moKinectGestureRecognition m_GestureRecognition;

#endif
    XnMapOutputMode m_OutputMode;

    moTexture* m_pDepthTexture;
    moTexture* m_pRGBTexture;
    moTexture* m_pIRTexture;
    moTexture* m_pUserTexture;

    moTexture* m_pDepthTexture2;

    unsigned char* pData;
    unsigned char* pData2;
    unsigned char* pImage;
    unsigned char* pIR;

    unsigned char* pDataObj1;
    unsigned char* pDataObj2;
    unsigned char* pDataObj3;

    int id_tex1;
    int id_tex2;
    int id_tex3;

    float dif_tex1;
    float dif_tex2;
    float dif_tex3;

    moTexture* m_pTex1;
    moTexture* m_pTex2;
    moTexture* m_pTex3;

    bool CheckError();

};


class moKinectFactory : public moIODeviceFactory {

public:
    moKinectFactory() {}
    virtual ~moKinectFactory() {}
    moIODevice* Create();
    void Destroy(moIODevice* fx);
};

extern "C"
{
MO_PLG_API moIODeviceFactory* CreateIODeviceFactory();
MO_PLG_API void DestroyIODeviceFactory();
}

#endif
