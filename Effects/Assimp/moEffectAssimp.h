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
#include "moMathMatrix.h"
#include "moMathVector3.h"
#include <string>
#include <iostream>
#include <map>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
	ASSIMP_OFFSETX,
	ASSIMP_OFFSETY,
	ASSIMP_OFFSETZ,
	ASSIMP_CULLFACE,
	ASSIMP_VERTICES_OPENFACE,
	ASSIMP_NO_PERSPECTIVE,
	ASSIMP_INLET,
	ASSIMP_OUTLET
};

enum moAssimpCullface {
    ASSIMP_CULLFACE_AUTOMATIC = 0,
    ASSIMP_CULLFACE_ENABLED = 1,
    ASSIMP_CULLFACE_DISABLED = 2
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
    moMatrix4d& Euler2RotationMatrix( double ex, double ey, double ez );

    moMatrix4d rotation_matrix;
    moMatrix4d offset_matrix;

    // the global Assimp scene object
    const aiScene* scene;
    GLuint scene_list;
    aiVector3D scene_min, scene_max, scene_center;

    int m_iCullFace;

    void get_bounding_box (aiVector3D* min, aiVector3D* max);

    void get_bounding_box_for_node (const aiNode* nd,
                                    aiVector3D* min,
                                    aiVector3D* max,
                                    aiMatrix4x4* trafo);


private:
    aiString* str;
    void UpdateRotation();

    MOint g_ViewMode;

    MOfloat Tx,Ty,Tz,Sx,Sy,Sz;
    moInlet* m_Euler_Angle_X;
    moInlet* m_Euler_Angle_Y;
    moInlet* m_Euler_Angle_Z;

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


    std::map<std::string, int> mapOfVertexStrings;
    std::map<int, int> mapOfVertexAssociation;
    moVector3dArray  m_Vertices_OpenFace;
    bool m_print_mesh_vertices;
    bool m_b_no_perspective;

    float m_Alpha;
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
