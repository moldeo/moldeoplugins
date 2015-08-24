/*******************************************************************************

                              moEffectParticlesSimple.h

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

  \brief Particles Effect Plugin, Simple Version
  \todo 1) ColorIn 2) ColorOut 3) BreakIn 4) BreakOut
  \todo 5) Script general
  \todo 6) Script por particula ( se ejecuta por cada particula ) ( acceso a eventos [tecla presionada...] )
  \todo *) movimiento y posicionamieno de particuclas....
  \todo 7) parametro desfazaje de movimiento??? que sea una opcion...
  \todo 8) Improve the orientation modes... they dont work correctly


*******************************************************************************/

/**
* \mainpage \if spanish ParticlesSimple de Moldeo. \else ParticlesSimple for Moldeo platform. \endif
*
* \if Description: A particle system programmed in C++ as a Moldeo Plugin Effect Object. \else OpenGL Particle system written in C++ for Moldeo 1.0 Platform. \endif
*
* \if spanish Ver lista de parámetros \else List all parameters: \endif @ref ParticlesSimpleParameters
*
* @see moEffect
* @see moMoldeoObject
*/

#ifndef __MO_EFFECT_PARTICLES_H__
#define __MO_EFFECT_PARTICLES_H__

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
    PARTICLES_TEXTUREMODE_MANY2PATCH = 3
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
    /// 0: \if spanish \else \endif
    PARTICLES_ORIENTATIONMODE_FIXED=0,
    /// 1: \if spanish \else \endif
    PARTICLES_ORIENTATIONMODE_CAMERA=1,
    /// 2: \if spanish \else \endif
    PARTICLES_ORIENTATIONMODE_MOTION=2,
    /// 3: \if spanish \else \endif
    PARTICLES_ORIENTATIONMODE_ACCELERATION=3
};

enum moParticlesOrderingMode {
    /// 0: \if spanish \else \endif 0: no reordering for drawing
    PARTICLES_ORDERING_MODE_NONE=0,
    /// 1: \if spanish \else \endif 1: reordered
    PARTICLES_ORDERING_MODE_ZDEPTHTEST=1,
    /// 2: \if spanish \else \endif zposition -> simple order relative to particle Z calculated position
    PARTICLES_ORDERING_MODE_ZPOSITION=2,
    /// 3: \if spanish \else \endif complete view dependent ordering >
    PARTICLES_ORDERING_MODE_COMPLETE=3
};


enum moParticlesSimpleParamIndex {
  PARTICLES_INLET,
  PARTICLES_OUTLET,
  PARTICLES_SCRIPT,
	PARTICLES_ALPHA,
	PARTICLES_COLOR,
	PARTICLES_SYNC,
	PARTICLES_PHASE,
	PARTICLES_PARTICLECOLOR,
	PARTICLES_FONT,
	PARTICLES_TEXT,
	PARTICLES_ORTHO,

	PARTICLES_TEXTURE,
	PARTICLES_FOLDERS,
	PARTICLES_TEXTUREMODE,
	PARTICLES_BLENDING,

	PARTICLES_WIDTH,
	PARTICLES_HEIGHT,
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
  PARTICLES_ORDERING_MODE
};


class moParticlesSimple : public moAbstract {

  public:

    moParticlesSimple() {

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
    virtual ~moParticlesSimple() {}

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

class moParticlesSimplePhysics : public moAbstract {

  public:

    moParticlesSimplePhysics() {
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
    virtual ~moParticlesSimplePhysics() {}


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

    moParticlesSimple*  m_pLastBordParticle;




};


enum enumRevelationStatus {

    PARTICLES_FULLRESTORED = 0,
    PARTICLES_REVEALING = 1,
    PARTICLES_REVEALINGALL = 2,
    PARTICLES_FULLREVEALED = 3,
    PARTICLES_RESTORING = 4,
    PARTICLES_RESTORINGALL = 5
};



moDeclareDynamicArray( moParticlesSimple*, moParticlesSimpleArray );

typedef std::map< double, moParticlesSimple* > TMapDepthToParticleSimple;


/// moEffectParticlesSimple
/**
 * \ingroup Effects
 * \ingroup Objects
 * \if spanish
 *
 * Plugin de efecto de partículas clásicas (método aproximación usado: Euler )
 *
 * Este efecto de partículas tiene un motor de física básico que aplica fuerzas y velocidad según la aproximación de Euler.
 * Se conforma en un arreglo de "width" x "height" = N particulas con parámetros globales de emisión, comportamiento y atracción.
 *
 * Los parámetros de emisión definen la configuración geométrica del espacio emisor dónde nacerán las partículas
 * y el método, condición y velocidad de creación de esas partículas, y son:
 *
 * \else
 *
 * Classic particles system.
 *
 * This is a simple particles system based on Euler formula to apply basic physic rules.
 * The particlessimple system build a "width" x "height" = N particles system with emission, behaviour and attraction global parameters.
 *
 * The emition parameters define the geometric configuration of the space where the particles will be born
 * and the creation, condition and speed of creation of these particles, are:
 *
 * \endif
 *
 * \anchor ParticlesSimpleParameters
 * \if spanish <h2>Emisión / Nacimiento</h2> \else <h2>Emition / Particle birth</h2> \endif
 * \if spanish <h3>Son los parámetros que afectan las condiciones iniciales de nacimiento de las partículas</h3> \else <h3>Parameters affecting the particles birth conditions</h3> \endif
 *
 * @anchor emittertype
 * @param emittertype \if spanish Tipo de de emisor ( opciones:  \else Emitter Type ( options:  \endif \ref moParticlesSimpleEmitterType)
 * @anchor emittervectorx
 * @param emittervectorx \if spanish Ancho del emisor \else Emitter size width \endif
 * @anchor emittervectory
 * @param emittervectory \if spanish Alto del emisor \else Emitter size height \endif
 * @anchor emittervectorz
 * @param emittervectorz \if spanish Profundidad del emisor \else Emitter size depth \endif
 * @anchor emitterperiod
 * @param emitterperiod \if spanish Intervalo en milisegundos entre conjunto de nacimientos \else Period between each birth groups. \endif
 * @anchor emitterrate
 * @param emitterrate \if spanish Cantidad de nacimientos en cada intervalo de \ref emitterperiod \else Maximum quantity of particles born after each \ref emitterperiod \endif
 * @anchor maxage
 * @param maxage \if spanish Edad máxima de la partícula. \else Maximum particle age in milliseconds. \endif
 * @anchor randommethod
 * @param randommethod \if spanish Modo de aleatoriedad, noisy, colinear o  Opciones: \else Emitter random method. Options: \endif \ref moParticlesRandomMethod
 * @anchor creationmethod
 * @param creationmethod \if spanish Modo de creación, linear , coplanar o volumétrico. Opciones: \else Emitter creation method. Options: \endif \ref moParticlesCreationMethod
 *
 * \if spanish <h2>Fuerzas / Gravedad</h2>  \else <h2>Forces / Gravity</h2> \endif
 * \if spanish <h3>Los paramétros aquí son fuerzas aplicadas a partir del nacimiento de la partícula.</h3>  \else These parameters are related to forces applied to all system. \endif
 * @param gravity \if spanish Gravedad aplicada a cada partícula, el centro de la fuerza gravitatoria se define en attractorvector(x|y|z)  \else Gravity, can be negative too! \endif
 * @param viscosity \if spanish Viscosidad, es la fuerza que resiste al movimiento, sigue la regla de la fricción.  \else Viscosity, friction. \endif
 * @param attractormode \if spanish Modo de funcionamiento del atractor. Opciones: \else Attractor mode. Options: \endif \ref moParticlesSimpleAttractorMode
 *
 * \if spanish <h2>Comportamiento</h2> \else Behaviour \endif
 * \if spanish <h3>El comportamiento se define por parámetros de movimiento aleatorio (browniano), cohesión o separación entre partículas </h3>  \else These paramteres affects random movement and other behaviour characteristics. \endif
 * @anchor randommotion
 * @param randommotion \if spanish Amplitud del movimiento aleatorio  \else Random motion amplitude. \endif
 * @anchor randommotionx
 * @param randommotionx \if spanish Ancho del espacio relativo que abarca el movimiento aleatorio  \else Relative motion width. \endif
 * @anchor randommotiony
 * @param randommotiony \if spanish Alto del espacio relativo que abarca el movimiento aleatorio  \else Relative motion height. \endif
 * @anchor randommotionz
 * @param randommotionz \if spanish Profundidad del espacio relativo que abarca el movimiento aleatorio  \else Relative motion depth. \endif
 *
 *
 * \endif
 *
 * @see moEffect
 * @see moParticlesSimple
 *
 * \author Fabricio Costa
 *
 * \image html screen_00.png
 * \image html screen_01.png
 * \image html screen_02.png
 * \image html screen_03.png
 *
 *  \todo \if spanish Habilitar parámetro de ordenamiento \else Activate order algorithm parameter \endif
 *  \todo \if spanish Activar dinámicamente el "texture_folder" \else Activate and load texture_folder on the fly \endif
 *  \todo
 *
 */
class moEffectParticlesSimple : public moEffect
{
    public:

        ///Constructor
        moEffectParticlesSimple();

        ///Destructor
        virtual ~moEffectParticlesSimple();

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
        void InitParticlesSimple( int p_cols, int p_rows , bool p_forced = true  );

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
        *   @see moParticlesSimplePhysics
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
        *   @see moParticlesSimplePhysics (m_EmitionPeriod, m_EmitionRate, m_CreationMethod )
        *   @see moEffectParticlesSimple (texture_mode)
        *
        */
        void Regenerate();

        ///Resolvedor de ecuación
        double Solver1D( double h, double y0, int method, double (*fcn)(double) );

        ///Fija la posicion de la particula que se genera
        void SetParticlePosition( moParticlesSimple* pParticle );


        void ParticlesSimpleInfluence( float posx, float posy, float velx, float vely, float veln );
        void ParticlesSimpleAnimation( moTempo* tempogral, moEffectState* parentstate );

        ///Dibuja la particula simple
        void DrawParticlesSimple( moTempo* tempogral, moEffectState* parentstate );
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

        int luaUpdateParticle( moLuaVirtualMachine& vm );
        int luaUpdateParticlePosition( moLuaVirtualMachine& vm );
        int luaUpdateParticleSize( moLuaVirtualMachine& vm );
        int luaUpdateParticleScale( moLuaVirtualMachine& vm );
        int luaUpdateParticleVelocity( moLuaVirtualMachine& vm );
        int luaUpdateParticleRotation( moLuaVirtualMachine& vm );
        int luaUpdateParticleGraphics( moLuaVirtualMachine& vm );

        int luaUpdateForce( moLuaVirtualMachine& vm );

        int luaShot(moLuaVirtualMachine& vm);
        int luaReInit(moLuaVirtualMachine& vm);

        int luaDrawPoint(moLuaVirtualMachine& vm);
        int luaGetParticleIntersection(moLuaVirtualMachine& vm);

        ///end script functions

        moInlet*                  m_pParticleTime;
        moInlet*                  m_pParticleIndex;

        moParticlesSimpleArray    m_ParticlesSimpleArray;
        moParticlesSimpleArray    m_ParticlesSimpleArrayTmp;
        moParticlesSimplePhysics    m_Physics;

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
        float normalf; ///width of full floor usually 100.0f

        long time_tofull_revelation;
        long time_tofull_restoration;
        long time_of_revelation;
        long time_of_restoration;
        long drawing_features; /// 0: nothing 1: motion  2: all
        long texture_mode;



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

        float midi_red, midi_green, midi_blue;
        float midi_maxage; //in millis
        float midi_emitionperiod;//in millisec
        float midi_emitionrate; // n per emitionperiod
        float midi_randomvelocity; //inicial vel
        float midi_randommotion; //motion dynamic

        float tx,ty,tz;
        float sx,sy,sz;
        float rx,ry,rz;

        TMapDepthToParticleSimple m_OrderedParticles;

        double dtrel;
        double dt;
        long gral_ticks;

};

class moEffectParticlesSimpleFactory : public moEffectFactory
{
public:
    moEffectParticlesSimpleFactory() {}
    virtual ~moEffectParticlesSimpleFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
