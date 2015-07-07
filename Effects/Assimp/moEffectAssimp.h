/*******************************************************************************

                                moEffectAssimp.h

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

#ifndef __MO_EFFECT_ASSIMP_H__
#define __MO_EFFECT_ASSIMP_H__

#include "moPlugin.h"
#include "mo3dModelManager.h"
#include "moArcBall.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// the global Assimp scene object
const struct aiScene* scene = NULL;
GLuint scene_list = 0;
aiVector3D scene_min, scene_max, scene_center;

// current rotation angle
#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)


#define MO_MOUSE_MOTION_X 0
#define MO_MOUSE_MOTION_Y 1
#define MO_MOUSE_DOWN_LEFT 2
#define MO_MOUSE_MOTION_Y_RIGHT 3
#define MO_KEY_PRESS_C 4

#define MO_ASSIMP_TRANSLATE_X		5
#define MO_ASSIMP_TRANSLATE_Y		6
#define MO_ASSIMP_TRANSLATE_Z		7
#define MO_ASSIMP_SCALE_X			8
#define MO_ASSIMP_SCALE_Y			9
#define MO_ASSIMP_SCALE_Z			10


GLfloat LightAmbient[]= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightSpecular[]= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[]= { 0.0f, 0.0f, 1.0f, 1.0f };

GLfloat LightModelAmbient[]= { 4.0f, 4.0f, 4.0f, 1.0f };

enum moIcon3dParamIndex {
	ASSIMP_ALPHA,
	ASSIMP_COLOR,
	ASSIMP_SYNC,
	ASSIMP_PHASE,
	ASSIMP_OBJECT,
	ASSIMP_TEXTURE,
	ASSIMP_MODEL_AMBIENT,
	ASSIMP_AMBIENT,
	ASSIMP_SPECULAR,
	ASSIMP_DIFFUSE,
	ASSIMP_POLYGONMODE,
	ASSIMP_BLENDING,
	ASSIMP_TRANSLATEX,
	ASSIMP_TRANSLATEY,
	ASSIMP_TRANSLATEZ,
	ASSIMP_ROTATEX,
	ASSIMP_ROTATEY,
	ASSIMP_ROTATEZ,
	ASSIMP_SCALEX,
	ASSIMP_SCALEY,
	ASSIMP_SCALEZ,
	ASSIMP_EYEX,
	ASSIMP_EYEY,
	ASSIMP_EYEZ,
	ASSIMP_VIEWX,
	ASSIMP_VIEWY,
	ASSIMP_VIEWZ,
	ASSIMP_LIGHTX,
	ASSIMP_LIGHTY,
	ASSIMP_LIGHTZ,
	ASSIMP_INLET,
	ASSIMP_OUTLET
};

class moEffectAssimp: public moEffect
{
public:
    moEffectAssimp();
    virtual ~moEffectAssimp();

    MOboolean Init();
    void Draw(moTempo*, moEffectState* parentstate = NULL);
    void Interaction( moIODeviceManager *IODeviceManager );
    moConfigDefinition *GetDefinition( moConfigDefinition *p_configdefinition );
    virtual void Update( moEventList* p_EventList );
    void UpdateParameters();
    void RenderModel();
    MOboolean Finish();

    void recursiveTextureLoad(const aiScene *sc, const aiNode* nd);
    int loadasset (const char* path);
    void recursive_render (const aiScene *sc, const aiNode* nd, float scale);
    void apply_material(const aiMaterial *mtl);


private:
    aiString* str;
    void UpdateRotation();

    MOint g_ViewMode;

    MOfloat	Tx,Ty,Tz,Sx,Sy,Sz;

    bool loadedAsset;
    moFile AssetFile;

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

    mo3dModelManagerRef Models3d;
};

class moEffectAssimpFactory : public moEffectFactory
{
public:
    moEffectAssimpFactory() {}
    virtual ~moEffectAssimpFactory() {}
    moEffect* Create();
    void Destroy(moEffect* fx);
};

extern "C"
{
MO_PLG_API moEffectFactory* CreateEffectFactory();
MO_PLG_API void DestroyEffectFactory();
}

#endif
