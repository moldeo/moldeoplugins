/*******************************************************************************

                              moEffectParticlesFractal.h

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


  TODO:
  1) ColorIn
  2) ColorOut
  3) BreakIn
  4) BreakOut



  5) Script general
  6) Script por particula ( se ejecuta por cada particula ) ( acceso a eventos [tecla presionada...] )

  *) movimiento y posicionamieno de particuclas....

  7) parametro desfazaje de movimiento??? que sea una opcion...

  8) Improve the orientation modes... they dont work correctly

*******************************************************************************/

#ifndef __MO_EFFECT_PARTICLESFRACTAL_H__
#define __MO_EFFECT_PARTICLESFRACTAL_H__

#include "moTypes.h"
#include "moTimer.h"
#include "moTempo.h"
#include "moPlugin.h"
#include "moTimeManager.h"
#include "moFilterManager.h"

#define MO_PARTICLES_TRANSLATE_X 0
#define MO_PARTICLES_TRANSLATE_Y 1
#define MO_PARTICLES_SCALE_X 2
#define MO_PARTICLES_SCALE_Y 3



///Emitter Type
/**
* \if spanish
* <b>"emittertype"</b>: <em>Tipo de emisor</em>
* Parámetro del efecto @ref moParticlesSimple
* El tipo de emisor es la configuración geométrica del espacio dónde nacen las partículas. Algo así como la incubadora de las partículas.
*
* \else
* <b>"emittertype"</b>: <em>Emitter Type is a parameter of the ParticlesSimple Effect Plugin @ref moParticlesSimple</em>
* Parameter of @ref moParticlesSimple
* The emitter type is the geometric base configuration where the particles can be born.
* \endif
*
* \if spanish <h4>Nombre del parámetro:</h4> \else <h4>Parameter name</h4> \endif @ref emittertype
* \if spanish <h4>Todos los parámetros:</h4> \else <h4>All parameters:</h4> \endif @ref ParticlesSimpleParameters
*
* @see moParticlesSimple
* @see moEffectParticlesSimple
*/
enum moParticlesSimpleEmitterType {
  /// 0: \if spanish Grilla (un rectángulo de "width" x "height") 2D \else  Grid shape emitter 2D \endif
  PARTICLES_EMITTERTYPE_GRID = 0,
  /// 1: \if spanish Esfera (una esfera de "width" longitudes y "height" latitudes) 3D \else Sphere shape emitter ( x=width, y=height, z=depth ) 3D \endif
  PARTICLES_EMITTERTYPE_SPHERE = 1,
  /// 2: \if spanish 2: Tubo ( un tubo de "width" facetas y "height" segmentos) 3D \else Tube shape emitter (3C) \endif
  PARTICLES_EMITTERTYPE_TUBE = 2,
  /// 3: \if Chorro ( colapsados en una línea ) \else Line shape emitter (3d) vector line (x,y,z) \endif
  PARTICLES_EMITTERTYPE_JET = 3,
  /// 4: \if spanish Punto ( colapsados en un punto ) \else point shape, position (x,y,z) \endif
  PARTICLES_EMITTERTYPE_POINT = 4,
  /// 5: \if spanish Trackeador ( reservado para información de sensado ) \else emitter is a connected tracker \endif
  PARTICLES_EMITTERTYPE_TRACKER = 5,
  /// 6: \if spanish Trackeador2 ( reservado para información de sensado ) \else emitter is a secondary connected tracker \endif
  PARTICLES_EMITTERTYPE_TRACKER2 = 6,
  /// 7: \if spanish Espiral ( forma de espiral, "width" ángulos que forman "height" ciclos ) \else Emitter is a spiral \endif
  PARTICLES_EMITTERTYPE_SPIRAL = 7,
  /// 8: \if spanish Círculo ( una ronda de "width"X"height" partículas  ) \else emitter is a circle \endif
  PARTICLES_EMITTERTYPE_CIRCLE = 8
};

///Folder Shot Type
/**
*   Folder
*/
enum moParticlesSimpleFolderShotType {
  /// 0: filename screenshot has a random id
  PARTICLES_SHOTTYPE_FOLDER_RANDOM = 0,
  /// 0: filename screenshot is date based
  PARTICLES_SHOTTYPE_FOLDER_SEQUENTIAL_BY_FILEDATE = 1,
  /// 0: filename screenshot is name based
  PARTICLES_SHOTTYPE_FOLDER_SEQUENTIAL_BY_FILENAME = 2
};

///Attractor Type
enum moParticlesSimpleAttractorType {
  /// 0: each particle attract diretly to the same point
  PARTICLES_ATTRACTORTYPE_POINT = 0,
  /// 1: each particle attract perp to a face of the grid
  PARTICLES_ATTRACTORTYPE_GRID = 1,
  /// 2: each particle attract perp to a face of the grid
  PARTICLES_ATTRACTORTYPE_SPHERE = 2,
  ///  3: each particle attract perp to a face of the grid
  PARTICLES_ATTRACTORTYPE_TUBE = 3,
  /// 4: each particle attract perpendicular to jet vector
  PARTICLES_ATTRACTORTYPE_JET = 4,
  /// 5: each particle attract each one to a dot of the tracker
  PARTICLES_ATTRACTORTYPE_TRACKER = 5,
  /// 6: each particle attract each one to a dot of the tracker
  PARTICLES_ATTRACTORTYPE_VERTEX = 6
};

///Attractor Mode
enum moParticlesSimpleAttractorMode {
  /// 0: accelerate with no stop
  PARTICLES_ATTRACTORMODE_ACCELERATION = 0,
  /// 1: accelerate, reach and stop instantly
  PARTICLES_ATTRACTORMODE_STICK = 1,
  /// 2: accelerate and bounce....(inverse direction)
  PARTICLES_ATTRACTORMODE_BOUNCE = 2,
  /// 3: accelerate and breaks (generate debris on place)
  PARTICLES_ATTRACTORMODE_BREAKS = 3,
  /// 4: accelerate then brake and slowdown slowly
  PARTICLES_ATTRACTORMODE_BRAKE = 4,
  /// 5: constant speed to attractortype
  PARTICLES_ATTRACTORMODE_LINEAR = 5
};

///Behaviour Mode
enum moParticlesSimpleBehaviourMode {
  /// 0: las partículas son atraídas entre ellas
  PARTICLES_BEHAVIOUR_COHESION = 0,
  /// 1: las partículas son libres y mantienen una distancia mínima de separación
  PARTICLES_BEHAVIOUR_SEPARATION = 1,
  /// 2: las partículas son repelidas entre ellas
  PARTICLES_BEHAVIOUR_AVOIDANCE = 2,
  /// 3: las partículas se alinean y acomodan cristalmente
  PARTICLES_BEHAVIOUR_ALIGNMENT = 3
};


///Texture Mode
enum moParticlesSimpleTextureMode {
    /// 0: One Same Texture Image for each Particle (taken from texture)
    PARTICLES_TEXTUREMODE_UNIT = 0,
    /// 1: Same Texture Image Divided In Different Fragments for each Particle (taken from texture, divided in width*height)
    PARTICLES_TEXTUREMODE_PATCH = 1,
    /// 2: Many Different Textures Image for each Particle ( taken from texturefolder )
    PARTICLES_TEXTUREMODE_MANY = 2,
    /// 3: Many textures/particle to construct a patched texture one ( taken from texturefolder, build the one defined on texture parameter, or from a folder, call to Shot(source) then ReInit to build... )
    PARTICLES_TEXTUREMODE_MANY2PATCH = 3,
    /// 4: Many Different Textures Image for each Particle ( taken from texturefolder in loading order! )
    PARTICLES_TEXTUREMODE_MANYBYORDER = 4

};

static moTextArray TextureModeOptions;

///Creation Method
enum moParticlesCreationMethod {
    /// 0: \if spanish Alineado. Los lugares de nacimientos están alineados con el orden de los vertices del emisor.  \else Aligned. Particles birth position follow the emitter vertices order. \endif
    PARTICLES_CREATIONMETHOD_LINEAR=0,
    /// 1: \if spanish Superficial. Los lugares de nacimientos están diseminados sobre la superficie del emisor aleatoriamente.  \else Surface. Particles birth position follow randomly the surface of the emitter. \endif
    PARTICLES_CREATIONMETHOD_PLANAR=1,
    ///  2: \if spanish Volumétrico. Los lugares de nacimientos están diseminados dentro del volumen del emisor aleatoriamente.  \else Volumetric. Particles birth position are into the volume of the emitter. \endif
    PARTICLES_CREATIONMETHOD_VOLUMETRIC=2,
    /// 3: \if spanish Central. Los lugares de nacimiento están colapsados en el centro del emisor. \else Central. Particles birth position are collapsed into the emitter center position. \endif
    PARTICLES_CREATIONMETHOD_CENTER=3
};

///Random Method
enum moParticlesRandomMethod {
    ///  0: \if spanish Ruidoso. \else Noisy \endif
    PARTICLES_RANDOMMETHOD_NOISY=0,
    ///  1: \if spanish Co-Lineal. \else Co-Linear \endif
    PARTICLES_RANDOMMETHOD_COLINEAR=1,
    ///  2: \if spanish Perpendicular. \else Perpendicular. \endif
    PARTICLES_RANDOMMETHOD_PERPENDICULAR=2
};

///Orientation Method
enum moParticlesOrientationMode {
    /// 0: \if spanish Fijo. \else Default position. \endif
    PARTICLES_ORIENTATIONMODE_FIXED=0,
    /// 1: \if spanish De frente al observador. \else Facing camera. \endif
    PARTICLES_ORIENTATIONMODE_CAMERA=1,
    /// 2: \if spanish Según el vector velocidad. \else Following motion direction. \endif
    PARTICLES_ORIENTATIONMODE_MOTION=2,
    /// 3: \if spanish Según el vector aceleración. \else Following acceleration direction vector. \endif
    PARTICLES_ORIENTATIONMODE_ACCELERATION=3,
    /// 4: \if spanish Según la normal de la generatriz \else Following generator normal vector \endif
    PARTICLES_ORIENTATIONMODE_NORMAL=4,
    /// 5: \if spanish Según la normal personalizada en los parametros NORMAL_PARTICLEX|Y|Z \else Following generator normal vector defined in parameters NORMAL_PARTICLEX|Y|Z \endif
    PARTICLES_ORIENTATIONMODE_CUSTOMNORMAL=5
};

enum moParticlesOrderingMode {
    /// 0: \if spanish Sin reordenamiento espacial, sigue el orden de la matriz WxH. \else no reordering for drawing \endif
    PARTICLES_ORDERING_MODE_NONE=0,
    /// 1: \if spanish Ordenamiento por profundidad de pixel (ZBuffer) \else Ordering using depth field (ZBuffer) \endif
    PARTICLES_ORDERING_MODE_ZDEPTHTEST=1,
    /// 2: \if spanish Ordenamiento por coordenada Z \else Ordering using z coordinate \endif
    PARTICLES_ORDERING_MODE_ZPOSITION=2,
    /// 3: \if spanish Ordenamiento por distancia a la cámara \else Ordering using camera distance to particle \endif
    PARTICLES_ORDERING_MODE_COMPLETE=3
};


enum moParticlesSimpleLightMode {
  /// 0: \if spanish Sin luces. \else No lights \endif
  PARTICLES_LIGHTMODE_NONE=0,
  /// 1: point \if spanish Luz omnidireccional \else Omnidirectional light \endif
  PARTICLES_LIGHTMODE_OMNI=1,
  /// 1: spot \if spanish Luz direccional \else Directional light \endif
  PARTICLES_LIGHTMODE_SPOT=2
};


enum moParticlesFractalParamIndex {
  PARTICLES_INLET,
  PARTICLES_OUTLET,
  PARTICLES_SCRIPT,
	PARTICLES_ALPHA,
	PARTICLES_COLOR,
	PARTICLES_SYNC,
	PARTICLES_PHASE,

	PARTICLES_MATURITY,
	PARTICLES_LEFT_REP_ANGLE_MIN,
	PARTICLES_LEFT_REP_ANGLE_MAX,
	PARTICLES_RIGHT_REP_ANGLE_MIN,
	PARTICLES_RIGHT_REP_ANGLE_MAX,

	PARTICLES_LEFT_MOV_ANGLE_MIN,
	PARTICLES_LEFT_MOV_ANGLE_MAX,
	PARTICLES_RIGHT_MOV_ANGLE_MIN,
	PARTICLES_RIGHT_MOV_ANGLE_MAX,


	PARTICLES_PARTICLECOLOR,
	PARTICLES_FONT,
	PARTICLES_TEXT,
	PARTICLES_ORTHO,

	PARTICLES_TEXTURE,
	PARTICLES_FOLDERS,
	PARTICLES_TEXTURE_CODE,
	PARTICLES_TEXTURE_MEMORY,
	PARTICLES_TEXTURE_MEDIUM,
	PARTICLES_TEXTURE_ALTITUDE,
	PARTICLES_TEXTURE_VARIABILITY,
	PARTICLES_TEXTURE_CONFIDENCE,
	PARTICLES_TEXTUREMODE,
	PARTICLES_BLENDING,

	PARTICLES_WIDTH,
	PARTICLES_HEIGHT,
	PARTICLES_MEMORY,
	PARTICLES_CODE,
	PARTICLES_SIZEX,
	PARTICLES_SIZEY,
	PARTICLES_SIZEZ,

	PARTICLES_GRAVITY,
	PARTICLES_VISCOSITY,

	PARTICLES_MAXAGE,
	PARTICLES_EMITIONPERIOD,
	PARTICLES_EMITIONRATE,
	PARTICLES_DEATHPERIOD,
  PARTICLES_SCRIPT2,

	PARTICLES_FADEIN,
	PARTICLES_FADEOUT,
	PARTICLES_SIZEIN,
	PARTICLES_SIZEOUT,

	PARTICLES_RANDOMMETHOD,
	PARTICLES_CREATIONMETHOD,
	PARTICLES_ORIENTATIONMODE,

	PARTICLES_RANDOMPOSITION,
	PARTICLES_RANDOMPOSITION_X,
	PARTICLES_RANDOMPOSITION_Y,
	PARTICLES_RANDOMPOSITION_Z,

	PARTICLES_RANDOMVELOCITY,
	PARTICLES_RANDOMVELOCITY_X,
	PARTICLES_RANDOMVELOCITY_Y,
	PARTICLES_RANDOMVELOCITY_Z,

	PARTICLES_RANDOMMOTION,
	PARTICLES_RANDOMMOTION_X,
	PARTICLES_RANDOMMOTION_Y,
	PARTICLES_RANDOMMOTION_Z,


	PARTICLES_EMITTERTYPE,
	PARTICLES_EMITTERVECTOR_X,
	PARTICLES_EMITTERVECTOR_Y,
	PARTICLES_EMITTERVECTOR_Z,

	PARTICLES_ATTRACTORTYPE,
	PARTICLES_ATTRACTORMODE,
	PARTICLES_ATTRACTORVECTOR_X,
	PARTICLES_ATTRACTORVECTOR_Y,
	PARTICLES_ATTRACTORVECTOR_Z,

	PARTICLES_ROTATEX_PARTICLE,
	PARTICLES_ROTATEY_PARTICLE,
	PARTICLES_ROTATEZ_PARTICLE,
  PARTICLES_SCALEX_PARTICLE,
	PARTICLES_SCALEY_PARTICLE,
	PARTICLES_SCALEZ_PARTICLE,

	PARTICLES_TIMETOREVELATION,
	PARTICLES_TIMEOFREVELATION,
	PARTICLES_TIMETORESTORATION,
	PARTICLES_TIMEOFRESTORATION,
	PARTICLES_DRAWINGFEATURES,

	PARTICLES_TRANSLATEX,
	PARTICLES_TRANSLATEY,
	PARTICLES_TRANSLATEZ,
  PARTICLES_SCALEX,
	PARTICLES_SCALEY,
	PARTICLES_SCALEZ,
	PARTICLES_ROTATEX,
	PARTICLES_ROTATEY,
	PARTICLES_ROTATEZ,
	PARTICLES_EYEX,
	PARTICLES_EYEY,
	PARTICLES_EYEZ,
	PARTICLES_VIEWX,
	PARTICLES_VIEWY,
	PARTICLES_VIEWZ,
  PARTICLES_UPVIEWX,
	PARTICLES_UPVIEWY,
	PARTICLES_UPVIEWZ,
	PARTICLES_RANDOMCOLORFUNCTION,
	PARTICLES_SEPARATION
	/*
  PARTICLES_ORDERING_MODE,
  PARTICLES_LIGHTMODE,
  PARTICLES_LIGHTX,
  PARTICLES_LIGHTY,
  PARTICLES_LIGHTZ,
  PARTICLES_ORDERING_MODE*/
};


class moStateFilterParams : public moTextFilterParam {
  public:
  moStateFilterParams() {

  }
  virtual ~moStateFilterParams() {}


};

class moParticlesFractal : public moAbstract {

  public:

    moParticlesFractal() {

      Pos3d = moVector3f(0.0,0.0,0.0);
      Destination = moVector3f(0.0,0.0,0.0);
      Velocity = moVector3f(0.0,0.0,0.0);
      Force = moVector3f(0.0,0.0,0.0);
      U = moVector3f(0.0,0.0,0.0);
      V = moVector3f(0.0,0.0,0.0);
      W = moVector3f(0.0,0.0,0.0);
      dpdt = moVector3f(0.0,0.0,0.0);
      dvdt = moVector3f(0.0,0.0,0.0);
      Rotation = moVector3f(0.0,0.0,0.0);


      Pos = moVector2f(0.0,0.0);
      TCoord = moVector2f(0.0,0.0);
      TCoord2 = moVector2f(0.0,0.0);
      Size = moVector2f(0.0,0.0);
      TSize = moVector2f(0.0,0.0);
      TSize2 = moVector2f(0.0,0.0);
      Color = moVector3f(1.0,1.0,1.0);

      Mass = 1.0;
      Alpha = 1.0;
      Scale = 1.0;
      ImageProportion = 1.0;

      Fixed = false;
      Visible = false;
      Captured = false;

      /* Graphics  */
      GLId = -1;
      GLId2 = -1;
      pTextureMemory = NULL;

      MOId = -1;

      ActualFrame = 0;
      FrameCount = 0;
      FramePS = 0;

      FrameForced = false;

      MaxAge = 0; //depends on Physics.MaxAge first....
      Age.Stop();

    }
    virtual ~moParticlesFractal() {}

    ///Position absolute
    moVector3f  Pos3d;

    ///Destination
    moVector3f  Destination;

    ///Speed or Velocity vector
    moVector3f  Velocity;

    ///Self force (propulsion) of the particle
    moVector3f  Force;

    ///Particles Vectors

    ///position relative to grid...
    /**
        Works for indexation on a grid relative geometry ( bidimentional array )
        X > i [0..width]
        Y > j [0..height]
    */
    moVector2f  Pos;//relativo a una grilla

    ///texture coordinate
    moVector2f  TCoord;
    moVector2f  TCoord2;

    ///particle size
    /**
        This size is dependent on particles number and Emitter size
        Size.X > EmitterSize.X() / m_cols
        Size.Y > EmitterSize.Y() / m_rows
    */
    moVector2f  Size;

    ///particle texture size
    moVector2f  TSize;
    moVector2f  TSize2;

    ///particle orientation
    /**
        Dependant on Orientation Mode

    */
    moVector3f  U,V,W;

    ///Differentials of position (speed) and velocity (acceleration)
    moVector3f  dpdt;
    moVector3f  dvdt;

    ///Mass in g of the particle
    double      Mass;

    ///No motion
    bool        Fixed;

    ///Exists but is invisible
    bool        Visible;

    ///Captured
    /**
        Captured by the general script, out of normal flow
    */
    bool        Captured;


    ///Transparency of the particle
    double      Alpha;

    ///Scale of the particle
    double      Scale;

    ///Angle of the particle
    moVector3f      Rotation;

    ///Unique ID of OpenGL Texture
    MOint       GLId;///para asociar la textura al momento de la creación
    MOint       GLId2;
    moTextureMemory *pTextureMemory;

    bool        FrameForced;
    int         MOId;
    MOuint      ActualFrame;
    MOuint      FrameCount;
    MOuint      FramePS;

    ///Texture image proportion Width / Height ....
    float       ImageProportion;

    moVector3f  Color;

    ///Age of the particle
    moTimer     Age;
    long        MaxAge;
};

class moParticlesFractalPhysics : public moAbstract {

  public:

    moParticlesFractalPhysics() {
      m_GravityCenter = moVector3f(0.0,0.0,0.0);
      m_GravityVector = moVector3f(0.0,0.0,0.0);
      m_GravityDisc = moVector3f(0.0,0.0,0.0);

      m_Plane = moVector3f(0.0,0.0,0.0);

      m_MaxAge = 0;
      m_EmitionPeriod = 0;
      m_EmitionRate = 0;
      m_DeathPeriod = 0;

      m_CreationMethod = PARTICLES_CREATIONMETHOD_LINEAR;
      m_RandomMethod = PARTICLES_RANDOMMETHOD_NOISY;
      m_OrientationMode = PARTICLES_ORIENTATIONMODE_FIXED;
      m_OrderingMode = PARTICLES_ORDERING_MODE_NONE;

      m_FadeIn = 0.0;
      m_FadeOut = 0.0;
      m_SizeIn = 0.0;
      m_SizeOut = 0.0;

      m_EmitterType = PARTICLES_EMITTERTYPE_GRID;
      m_EmitterSize = moVector3f( 0.0, 0.0, 0.0 );
      m_EmitterVector = moVector3f( 0.0, 0.0, 0.0 );

      m_AttractorType = PARTICLES_ATTRACTORTYPE_POINT;
      m_AttractorMode = PARTICLES_ATTRACTORMODE_ACCELERATION;
      m_AttractorVector = moVector3f( 0.0, 0.0, 0.0 );

      m_EyeVector = moVector3f( 0.0, 0.0, 0.0 );

      m_RandomVelocity = 0.0;
      m_VelocityVector = moVector3f( 0.0, 0.0, 0.0 );

      m_RandomPosition = 0.0;
      m_PositionVector = moVector3f( 0.0, 0.0, 0.0 );

      m_RandomMotion = 0.0;
      m_MotionVector = moVector3f( 0.0, 0.0, 0.0 );

      gravitational = 0.0;
      viscousdrag = 0.0;

      EmitionTimer.Stop();

      m_ParticleScript = moText("");

      m_pLastBordParticle = NULL;


    }
    virtual ~moParticlesFractalPhysics() {}


    moVector3f      m_GravityCenter;//x,y,z,intensity
    moVector3f      m_GravityVector;//x,y,z,intensity
    moVector3f      m_GravityDisc;//normal x, normal y, normal z,intensity

    moVector3f      m_Plane;

    long            m_MaxAge;
    long            m_EmitionPeriod;
    long            m_EmitionRate;
    long            m_DeathPeriod;

    moParticlesCreationMethod   m_CreationMethod;
    moParticlesRandomMethod     m_RandomMethod;
    moParticlesOrientationMode  m_OrientationMode;
    moParticlesOrderingMode     m_OrderingMode;

    double          m_FadeIn;
    double            m_FadeOut;
    double            m_SizeIn;
    double            m_SizeOut;

    moParticlesSimpleEmitterType    m_EmitterType;
    moVector3f      m_EmitterVector;
    moVector3f      m_EmitterSize;

    moParticlesSimpleAttractorType    m_AttractorType;
    moParticlesSimpleAttractorMode    m_AttractorMode;
    moVector3f      m_AttractorVector;

    moVector3f      m_EyeVector;
    moVector3f      m_TargetViewVector;
    moVector3f      m_UpViewVector;

  /*
    moVector3f      m_SourceLightVector;
    moParticlesSimpleLightMode m_SourceLighMode;*/

    double          m_RandomVelocity;
    moVector3f      m_VelocityVector;

    double           m_RandomPosition;
    moVector3f      m_PositionVector;

    double          m_RandomMotion;
    moVector3f      m_MotionVector;

    double gravitational;
    double viscousdrag;


    moTimer EmitionTimer;

    moText  m_ParticleScript;

    moParticlesFractal*  m_pLastBordParticle;




};


enum enumRevelationStatus {

    PARTICLES_FULLRESTORED = 0,
    PARTICLES_REVEALING = 1,
    PARTICLES_REVEALINGALL = 2,
    PARTICLES_FULLREVEALED = 3,
    PARTICLES_RESTORING = 4,
    PARTICLES_RESTORINGALL = 5
};



moDeclareDynamicArray( moParticlesFractal*, moParticlesFractalArray );

typedef std::map< double, moParticlesFractal* > TMapDepthToParticleSimple;


class moTextureFilterVelocity : public moTextureFilter {

  public:
    moTextureFilterVelocity();
    virtual ~moTextureFilterVelocity();

    virtual MOboolean Init(moGLManager* p_glman, moRenderManager* p_renderman, moTextureArray &p_src_tex, moTextureArray &p_dest_tex, moShader *p_shader, moTextFilterParam *p_params = NULL);

    virtual void Apply( moMoldeoObject *p_src_mob = NULL, moTempo* p_tempo = NULL, MOfloat p_fade = 1.0, moTextFilterParam *p_params = NULL );
    virtual void SetupShader(MOint w, MOint h, moTempo *p_tempo, MOfloat p_fade, moTextFilterParam *p_params, moMoldeoObject* p_src_object = NULL);

    GLint m_gravity;
};


/// Clase Particle Simple
/**
 *
 *
 * @see moEffect
 */
class moEffectParticlesFractal : public moEffect
{
    public:

        ///Constructor
        moEffectParticlesFractal();

        ///Destructor
        virtual ~moEffectParticlesFractal();

        ///Inicialización
        MOboolean Init();

        ///Dibujado
        void Draw(moTempo*, moEffectState* parentstate = NULL);

        ///Finalizacion
        MOboolean Finish();

        ///Reiniciar el analisis de la imagen
        void ReInit();

        ///Foto de la cámara
        void Shot( moText source = moText(""), int shot_type = 0, int shot_file=0  );

        ///Interacción
        void Interaction( moIODeviceManager * );

        ///Actualización del ciclo de eventos
        void Update( moEventList * p_eventlist);

        ///Definicion de campos
        moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

        ///Llamada de scripts
        int ScriptCalling(moLuaVirtualMachine& vm, int iFunctionNumber);

        void TrackParticle( int partid );

        void OrderParticles();

        int         m_nVertexCount;                     // Vertex Count
        moVector3f*      m_pVertices;
        moVector2f*      m_pTexCoords;
        moVector4d        m_Color;
        double             m_Alpha;

        void BuildVBOs();
        // Vertex Buffer Object Names
        unsigned int    m_nVBOVertices;                     // Vertex VBO Name
        unsigned int    m_nVBOTexCoords;                    // Texture Coordinate VBO Name

        bool g_fVBOSupported;
    private:

        ///Actualizar el dt para iteracion de particulas
        void UpdateDt();

        ///Actualizar los parametros de configuración
        void UpdateParameters();

        ///Resetear el temporizador
        void ResetTimers();
        ///Revelar
        void RevealingAll();
        ///
        void RestoringAll();

        ///Inicio del sistema de partículas
        void InitParticlesFractal( int p_cols, int p_rows , bool p_forced = true  );

        ///Calcular las fuerzas sobre las partículas
        void CalculateForces( bool tmparray = false );

        ///Calcular los incrementos
        void CalculateDerivatives( bool tmparray = false, double dt = 0.0 );

        ///Actualizar todas las particulas
        /**
        *   Calcula la nueva posición de las partículas utilizando el diferencial de tiempo "dt"
        *   Llama a Regenerate() para la regeneración de partículas muertas.
        *
        *   @see Regenerate
        *   @see moParticlesFractalPhysics
        *
        *   @param dt diferencial de tiempo en milisegundos
        *   @param method método de cálculo de la posición, Euler o Midpoint (Punto medio)
        */
        void UpdateParticles( double dt, int method );

        ///Regeneración de partículas
        /**
        *   Mata aquellas que superaron su tiempo de vida
        *   Regenera las muertas en función del intervalo de emisión (en milisegundos)
        *   y la tasa de generación (cantidad de particulas por intervalo)
        *
        *
        *
        *   @see Regenerate
        *   @see moParticlesFractalPhysics (m_EmitionPeriod, m_EmitionRate, m_CreationMethod )
        *   @see moEffectParticlesFractal (texture_mode)
        *
        */
        void Regenerate();

        ///Resolvedor de ecuación
        double Solver1D( double h, double y0, int method, double (*fcn)(double) );

        ///Fija la posicion de la particula que se genera
        void SetParticlePosition( moParticlesFractal* pParticle );


        void ParticlesFractalInfluence( float posx, float posy, float velx, float vely, float veln );
        void ParticlesFractalAnimation( moTempo* tempogral, moEffectState* parentstate );

        ///Dibuja la particula simple
        void DrawParticlesFractal( moTempo* tempogral, moEffectState* parentstate );
        void DrawTracker();

        /// Funciones para la escritura de scripts específicos de particulas
        void RegisterFunctions();

        int luaGetDelta(moLuaVirtualMachine& vm);
        int luaGetParticleCount(moLuaVirtualMachine& vm);
        int luaGetParticle(moLuaVirtualMachine& vm);
        int luaGetParticlePosition(moLuaVirtualMachine& vm);
        int luaGetParticleSize(moLuaVirtualMachine& vm);
        int luaGetParticleScale(moLuaVirtualMachine& vm);
        int luaGetParticleVelocity(moLuaVirtualMachine& vm);
        int luaGetParticleRotation(moLuaVirtualMachine& vm);
        int luaGetParticleGraphics(moLuaVirtualMachine& vm);
        int luaGetParticleOpacity(moLuaVirtualMachine& vm);
        int luaGetParticleColor(moLuaVirtualMachine& vm);

        int luaUpdateParticle( moLuaVirtualMachine& vm );
        int luaUpdateParticlePosition( moLuaVirtualMachine& vm );
        int luaUpdateParticleSize( moLuaVirtualMachine& vm );
        int luaUpdateParticleScale( moLuaVirtualMachine& vm );
        int luaUpdateParticleVelocity( moLuaVirtualMachine& vm );
        int luaUpdateParticleRotation( moLuaVirtualMachine& vm );
        int luaUpdateParticleGraphics( moLuaVirtualMachine& vm );
        int luaUpdateParticleOpacity( moLuaVirtualMachine& vm );
        int luaUpdateParticleColor( moLuaVirtualMachine& vm );

        int luaUpdateForce( moLuaVirtualMachine& vm );

        int luaShot(moLuaVirtualMachine& vm);
        int luaReInit(moLuaVirtualMachine& vm);

        int luaDrawPoint(moLuaVirtualMachine& vm);
        int luaGetParticleIntersection(moLuaVirtualMachine& vm);

        int lua_id_cell;
        int luaCellBeginProgram(moLuaVirtualMachine& vm);

        int luaCellAge(moLuaVirtualMachine& vm);
        int luaCellDuplicate(moLuaVirtualMachine& vm);
        int luaCellMutate(moLuaVirtualMachine& vm);
        int luaCellCrossover(moLuaVirtualMachine& vm);
        int luaCellDie(moLuaVirtualMachine& vm);

        int luaCmpMemory(moLuaVirtualMachine& vm);
        int luaReadMemory(moLuaVirtualMachine& vm);
        int luaWriteMemory(moLuaVirtualMachine& vm);
        int luaDumpMemory(moLuaVirtualMachine& vm);
        //int luaCellGrow(moLuaVirtualMachine& vm);
        //int luaCell(moLuaVirtualMachine& vm);

        int luaCellEndProgram(moLuaVirtualMachine& vm);
        int luaCellDumpProgram(moLuaVirtualMachine& vm);

        ///end script functions

        moInlet*                  m_pParticleTime;
        moParticlesFractalArray    m_ParticlesFractalArray;
        moParticlesFractalArray    m_ParticlesFractalArrayTmp;
        moParticlesFractalPhysics    m_Physics;

        bool                    m_bTrackerInit;

        moTrackerSystemData*    m_pTrackerData;
        MOint                   m_InletTrackerSystemIndex;

        #ifdef USE_TUIO
        moTUIOSystemData*       m_pTUIOData;
        MOint                   m_InletTuioSystemIndex;
        #endif



        moVector2f              m_TrackerBarycenter;


        void setUpLighting();

        int m_rows,m_cols;
        int m_cellmem,m_cellcode;
        float normalf; ///width of full floor usually 100.0f

        long time_tofull_revelation;
        long time_tofull_restoration;
        long time_of_revelation;
        long time_of_restoration;
        long drawing_features; /// 0: nothing 1: motion  2: all
        long texture_mode;
        float particles_separation;
        int random_color_function;


        bool ortho;

    ///internal
        moTimer MotionTimer;

        moTimer TimerFullRevelation; ///begins on first motion activity!!!!
        moTimer TimerFullRestoration;///begins on full revelation finished....
        moTimer TimerOfRevelation; ///begins on revealing all
        moTimer TimerOfRestoration;///begins on restoring all

        moTimer FeatureActivity;///start on first feature activity, ends on
        moTimer MotionActivity;///start on first motion activity, ends on no motion
        moTimer NoMotionActivity;///start on no motion, ends on first motion activity

        enumRevelationStatus revelation_status; /// 5: full revealed 0: full hidden

        long    m_Rate;

        long    last_tick;

        //special for script
        moTexture* pTextureDest;
        moTexture* pSubSample;
        MObyte* samplebuffer;

        int glidori;
        int glid;
        int frame;

        int original_width;
        int original_height;
        float original_proportion;

        float emiper;
        long emiperi;

        ///Medium Texture defined with gl id
        int glid_medium;

/*
        float midi_red, midi_green, midi_blue;
        float midi_maxage; //in millis
        float midi_emitionperiod;//in millisec
        float midi_emitionrate; // n per emitionperiod
        float midi_randomvelocity; //inicial vel
        float midi_randommotion; //motion dynamic
*/
        float tx,ty,tz;
        float sx,sy,sz;
        float rx,ry,rz;

        TMapDepthToParticleSimple m_OrderedParticles;

        double dtrel;
        double dt;
        long gral_ticks;

        bool  m_bCellCodeTextureSwapOn;
        moTexture*  m_pCellCodeTextureSwap;
        moTextureFilter*  m_pTFilter_CellCodeTextureSwap;
        moStateFilterParams* m_pCellCodeFilterParams;
        moTexture*  m_pCellCodeTexture;
        moTextureFilter*  m_pTFilter_CellCodeTexture;
        moTexture* m_pCellCodeTextureFinal;

        bool  m_bCellMemoryTextureSwapOn;
        moTexture*  m_pCellMemoryTextureSwap;
        moTextureFilter*  m_pTFilter_CellMemoryTextureSwap;
        moStateFilterParams* m_pCellMemoryFilterParams;
        moTexture*  m_pCellMemoryTexture;
        moTextureFilter*  m_pTFilter_CellMemoryTexture;
        moTexture*  m_pCellMemoryTextureFinal;

        //CONVOLUTION MATRIX
        bool  m_bCellMemoryTextureSwapOnDX[4];
        moTexture*  m_pCellMemoryTextureSwapDX[4]; /// CONVOLUTION AVERAGES >> 64 >> 32 >> 16 >> 8 >> 4 >> 2 >> 1
        moTextureFilter*  m_pTFilter_CellMemoryTextureSwapDX[4];
        moStateFilterParams* m_pCellMemoryFilterParamsDX[4];
        moTexture*  m_pCellMemoryTextureDX[4];
        moTextureFilter*  m_pTFilter_CellMemoryTextureDX[4];
        moTexture*  m_pCellMemoryTextureFinalDX[4];


        bool  m_bMediumTextureSwapOn;
        moTexture*  m_pMediumTextureSwap;
        moTextureFilter*  m_pTFilter_MediumTextureSwap;
        moStateFilterParams* m_pMediumFilterParams;
        moTexture*  m_pMediumTexture;
        moTextureFilter*  m_pTFilter_MediumTexture;

        moText  m_MediumTextureLoadedName;//Composition Medium [COLORS are OPTIONS]


        bool  m_bAltitudeTextureSwapOn;
        moTexture*  m_pAltitudeTextureSwap;
        moTextureFilter*  m_pTFilter_AltitudeTextureSwap;
        moStateFilterParams* m_pAltitudeFilterParams;
        moTexture*  m_pAltitudeTexture;
        moTextureFilter*  m_pTFilter_AltitudeTexture;

        moText  m_AltitudeTextureLoadedName;//Altitude values [LIGHTER is more IMPORTANT]

        bool  m_bVariabilityTextureSwapOn;
        moTexture*  m_pVariabilityTextureSwap;
        moTextureFilter*  m_pTFilter_VariabilityTextureSwap;
        moStateFilterParams* m_pVariabilityFilterParams;
        moTexture*  m_pVariabilityTexture;
        moTextureFilter*  m_pTFilter_VariabilityTexture;

        moText  m_VariabilityTextureLoadedName;//Variability values [LIGHTER is ENTROPIC]

        bool  m_bConfidenceTextureSwapOn;
        moTexture*  m_pConfidenceTextureSwap;
        moTextureFilter*  m_pTFilter_ConfidenceTextureSwap;
        moStateFilterParams* m_pConfidenceFilterParams;
        moTexture*  m_pConfidenceTexture;
        moTextureFilter*  m_pTFilter_ConfidenceTexture;

        moText  m_ConfidenceTextureLoadedName;//Confidence values [LIGHTER is more CONFIDENT]


        bool  m_bStateTextureSwapOn;
        moTexture*  m_pStateTextureSwap;
        moTextureFilter*  m_pTFilter_StateTextureSwap;
        moStateFilterParams* m_pStateFilterParams;
        moTexture*  m_pStateTexture;
        moTextureFilter*  m_pTFilter_StateTexture;

        bool  m_bColorTextureSwapOn;
        moTexture*  m_pColorTextureSwap;
        moTextureFilter*  m_pTFilter_ColorTextureSwap;
        moTexture*  m_pColorTexture;
        moTextureFilter*  m_pTFilter_ColorTexture;
        moTexture*  m_pColorTextureFinal;

        bool  m_bPositionTextureSwapOn;
        moTexture*  m_pPositionTextureSwap;
        moTextureFilter*  m_pTFilter_PositionTextureSwap;
        moTexture*  m_pPositionTexture;
        moTextureFilter*  m_pTFilter_PositionTexture;

        GLfloat* m_PositionArray;

        bool  m_bVelocityTextureSwapOn;
        moTexture*  m_pVelocityTextureSwap;
        //moTextureFilterVelocity*  m_pTFilter_VelocityTextureSwap;
        moTextureFilter*  m_pTFilter_VelocityTextureSwap;
        moTexture*  m_pVelocityTexture;
        //moTextureFilterVelocity*  m_pTFilter_VelocityTexture;
        moTextureFilter*  m_pTFilter_VelocityTexture;

        bool  m_bScaleTextureSwapOn;
        moTexture*  m_pScaleTextureSwap;
        moTextureFilter*  m_pTFilter_ScaleTextureSwap;
        moTexture*  m_pScaleTexture;
        moTextureFilter*  m_pTFilter_ScaleTexture;

        bool  m_bOrientationTextureSwapOn;
        moTexture*  m_pOrientationTextureSwap;
        moTextureFilter*  m_pTFilter_OrientationTextureSwap;
        moTexture*  m_pOrientationTexture;
        moTextureFilter*  m_pTFilter_OrientationTexture;

        moTexture*  m_pStateTextureFinal;

        /**
        * Material Dim
        * Material dim is the set of information layers
        * that defines:
        *   Composition  (TT) m, g, a
        *   Scale (TA@) sx, sy, sz
        *   Position  (TA) x, y, z
        *   Orientation (TF) ox, oy, oz
        */
        moTexture*  m_pCompositionTextureFinal;/// TT
        moTexture*  m_pScaleTextureFinal;/// TA@
        moTexture*  m_pPositionTextureFinal;/// TA
        moTexture*  m_pOrientationTextureFinal; /// TF


        /**
        * Emotional Dim
        * Emotional dim is the set of information layers
        * that defines:
        *   Differentiation (A@T) dm, dg, da
        *   Grow (A@A@) gsx, gsy, gsz
        *   Velocity  (A@A) vx, vy, vz
        *   Polarization (A@F) pox, poy, poz
        */
        moTexture*  m_pDiferentiationTextureFinal;/// TT
        moTexture*  m_pGrowTextureFinal;/// TA@
        moTexture*  m_pVelocityTextureFinal;/// TA
        moTexture*  m_pPolarizationTextureFinal;/// TF

        /**
        * Mental Dim
        * Mental dim is the set of information layers
        * that defines:
        *   Evolution (A@T) evom, evog, evoa
        *   Vibration (A@A@) vibx, viby, vibz
        *   Acceleration  (A@A) ax, ay, az
        *   Frequency (A@F) frex, frey, frez
        */
        moTexture*  m_pEvolutionTextureFinal;
        moTexture*  m_pVibrationTextureFinal;
        moTexture*  m_pAccelerationTextureFinal;
        moTexture*  m_pFrequencyTextureFinal;

        /**
        * Creative Dim
        * Creative dim is the set of information layers
        * that defines:
        *   Gender (FT)
        *   Emotion (FA@)
        *   Force (FA)
        *   Creation (FF)
        */
        moTexture*  m_pGenderTextureFinal;
        moTexture*  m_pEmotionTextureFinal;
        moTexture*  m_pForceTextureFinal;
        moTexture*  m_pCreationTextureFinal;

        int numParticles;
        GLfloat *posArray;
        GLfloat *scaleArray;
        GLfloat *orientationArray;
        GLfloat *stateArray;
        GLfloat *velocityArray;
        GLfloat *colorArray;

        long    cell_position;
        long    cell_position_i;
        long    cell_position_j;
        GLfloat *cellcodeArray;
        GLfloat *cellmemoryArray;

        ///calligram
        moTextureBuffer*    m_pTexBuf;
        int     m_nImages;
};


#endif
