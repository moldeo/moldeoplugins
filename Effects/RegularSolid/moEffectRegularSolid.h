/*******************************************************************************

                                moEffectRegularSolid.h

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

#ifndef __MO_EFFECT_REGULARSOLID_H__
#define __MO_EFFECT_REGULARSOLID_H__

#include "moPlugin.h"
#include "mo3dModelManager.h"
#include "moArcBall.h"

/**
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
*/

// the global Assimp scene object
const struct aiScene* scene = NULL;
GLuint scene_list = 0;
/*aiVector3D scene_min, scene_max, scene_center;*/

// current rotation angle
#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)


#define MO_MOUSE_MOTION_X 0
#define MO_MOUSE_MOTION_Y 1
#define MO_MOUSE_DOWN_LEFT 2
#define MO_MOUSE_MOTION_Y_RIGHT 3
#define MO_KEY_PRESS_C 4

#define MO_REGULARSOLID_TRANSLATE_X		5
#define MO_REGULARSOLID_TRANSLATE_Y		6
#define MO_REGULARSOLID_TRANSLATE_Z		7
#define MO_REGULARSOLID_SCALE_X			8
#define MO_REGULARSOLID_SCALE_Y			9
#define MO_REGULARSOLID_SCALE_Z			10


GLfloat LightAmbient[]= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightSpecular[]= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[]= { 0.0f, 0.0f, 1.0f, 1.0f };

GLfloat LightModelAmbient[]= { 4.0f, 4.0f, 4.0f, 1.0f };

enum moRegularSolidParamIndex {
	REGULARSOLID_ALPHA,
	REGULARSOLID_COLOR,
	REGULARSOLID_SYNC,
	REGULARSOLID_PHASE,
	REGULARSOLID_OBJECT,
	REGULARSOLID_TEXTURE,
	REGULARSOLID_MODEL_AMBIENT,
	REGULARSOLID_AMBIENT,
	REGULARSOLID_SPECULAR,
	REGULARSOLID_DIFFUSE,
	REGULARSOLID_POLYGONMODE,
	REGULARSOLID_BLENDING,
	REGULARSOLID_TRANSLATEX,
	REGULARSOLID_TRANSLATEY,
	REGULARSOLID_TRANSLATEZ,
	REGULARSOLID_ROTATEX,
	REGULARSOLID_ROTATEY,
	REGULARSOLID_ROTATEZ,
	REGULARSOLID_SCALEX,
	REGULARSOLID_SCALEY,
	REGULARSOLID_SCALEZ,
	REGULARSOLID_EYEX,
	REGULARSOLID_EYEY,
	REGULARSOLID_EYEZ,
	REGULARSOLID_VIEWX,
	REGULARSOLID_VIEWY,
	REGULARSOLID_VIEWZ,
	REGULARSOLID_LIGHTX,
	REGULARSOLID_LIGHTY,
	REGULARSOLID_LIGHTZ,
	REGULARSOLID_INLET,
	REGULARSOLID_OUTLET
};

enum moRegularSolidObject {
    REGULARSOLID_OBJECT_TETRA = 0,
    REGULARSOLID_OBJECT_HEXA = 1,
    REGULARSOLID_OBJECT_OCTA = 2,
    REGULARSOLID_OBJECT_ICOSA = 3,
    REGULARSOLID_OBJECT_DODECA = 4,
    REGULARSOLID_OBJECT_SPHERE = 5,
    REGULARSOLID_OBJECT_PLANE = 6
};

class moEffectRegularSolid: public moEffect
{
public:
    moEffectRegularSolid();
    virtual ~moEffectRegularSolid();

    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
    void Interaction( moIODeviceManager *IODeviceManager );
    moConfigDefinition *GetDefinition( moConfigDefinition *p_configdefinition );
    virtual void Update( moEventList* p_EventList );
    void UpdateParameters();
    void RenderModel();
    MOboolean Finish();

    /*void recursiveTextureLoad(const aiScene *sc, const aiNode* nd);*/
    int loadasset (const char* path);
    /*void recursive_render (const aiScene *sc, const aiNode* nd, float scale);*/
    /*void apply_material(const aiMaterial *mtl);*/


private:
    /*aiString* str;*/
    void UpdateRotation();

    MOint g_ViewMode;

    MOfloat	Tx,Ty,Tz,Sx,Sy,Sz;

    bool loadedAsset;
    moFile AssetFile;
    int m_SolidObject;

    moGeometry* m_pGeometry;

    moTetrahedronGeometry m_Tetra;
    moBoxGeometry m_Hexa;
    moOctahedronGeometry m_Octa;
    moIcosahedronGeometry m_Icosa;
    moDodecahedronGeometry m_Dodeca;
    moSphereGeometry m_Sphere;

    // Mouse input.
    bool show_cursor;
    MOdouble distance_z;

    ArcBallT    ArcBall;
    Point2fT    MousePt;                 // Current Mouse Point
    bool        isClicked;               // Clicking The Mouse?
    bool        isRPressed;              // Pressing The Reset Button?
    bool        isDragging;              // Dragging The Mouse?
    Matrix4fT   Transform;               // Final Transform
    Matrix3fT   LastRot;                 // Last Rotation
    Matrix3fT   ThisRot;                 // This Rotation

};

class moEffectRegularSolidFactory : public moEffectFactory
{
public:
    moEffectRegularSolidFactory() {}
    virtual ~moEffectRegularSolidFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
