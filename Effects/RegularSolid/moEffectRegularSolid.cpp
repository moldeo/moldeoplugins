/*******************************************************************************

                                moEffectRegularSolid.cpp

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
  Andr� Colubri

*******************************************************************************/

#include "moEffectRegularSolid.h"

//========================
//  Factory
//========================

moEffectRegularSolidFactory *m_EffectAssimpFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectAssimpFactory==NULL)
		m_EffectAssimpFactory = new moEffectRegularSolidFactory();
	return(moEffectFactory*) m_EffectAssimpFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectAssimpFactory;
	m_EffectAssimpFactory = NULL;
}

moEffect*  moEffectRegularSolidFactory::Create() {
	return new moEffectRegularSolid();
}

void moEffectRegularSolidFactory::Destroy(moEffect* fx) {
	delete fx;
}

/*
void Color4f(const aiColor4D *color)
{
	glColor4f(color->r, color->g, color->b, color->a);
}

void set_float4(float f[4], float a, float b, float c, float d)
{
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}

void color4_to_float4(const aiColor4D *c, float f[4])
{
	f[0] = c->r;
	f[1] = c->g;
	f[2] = c->b;
	f[3] = c->a;
}
// ----------------------------------------------------------------------------
void get_bounding_box_for_node (const aiNode* nd,
	aiVector3D* min,
	aiVector3D* max,
	aiMatrix4x4* trafo
){
	aiMatrix4x4 prev;
	unsigned int n = 0, t;

	prev = *trafo;
	aiMultiplyMatrix4(trafo,&nd->mTransformation);

	for (; n < nd->mNumMeshes; ++n) {
		const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {

			aiVector3D tmp = mesh->mVertices[t];
			aiTransformVecByMatrix4(&tmp,trafo);

			min->x = aisgl_min(min->x,tmp.x);
			min->y = aisgl_min(min->y,tmp.y);
			min->z = aisgl_min(min->z,tmp.z);

			max->x = aisgl_max(max->x,tmp.x);
			max->y = aisgl_max(max->y,tmp.y);
			max->z = aisgl_max(max->z,tmp.z);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
		get_bounding_box_for_node(nd->mChildren[n],min,max,trafo);
	}
	*trafo = prev;
}

// ----------------------------------------------------------------------------
void get_bounding_box (aiVector3D* min, aiVector3D* max)
{
	aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);

	min->x = min->y = min->z =  1e10f;
	max->x = max->y = max->z = -1e10f;
	get_bounding_box_for_node(scene->mRootNode,min,max,&trafo);
}


// ----------------------------------------------------------------------------


void moEffectRegularSolid::recursiveTextureLoad(const aiScene *sc, const aiNode* nd)
{

	unsigned int n = 0;

	aiMatrix4x4 m = nd->mTransformation;



	// update transform

	aiTransposeMatrix4(&m);

	glPushMatrix();

	glMultMatrixf((float*)&m);



	// draw all meshes assigned to this node

	for (; n < nd->mNumMeshes; ++n)

	{

		const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];

		unsigned int cont = aiGetMaterialTextureCount(sc->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE);





		if(cont > 0)

		{

			//aiGetMaterialString(sc->mMaterials[mesh->mMaterialIndex],AI_MATKEY_TEXTURE_DIFFUSE(0),str);

			aiGetMaterialTexture(sc->mMaterials[mesh->mMaterialIndex],aiTextureType_DIFFUSE,0,str,0,0,0,0,0,0);



			// See if another mesh is already using this texture, if so, just copy GLuint instead of remaking entire texture


      moText texture_name = moText( str->C_Str() );
      texture_name = AssetFile.GetPath() + texture_name;
      MODebug2->Message("moEffectRegularSolid::recursiveTextureLoad > texture_name "  + texture_name );

      int idx = m_pResourceManager->GetTextureMan()->GetTextureMOId( texture_name, true );

      if (idx>-1) {
        moTexture * pText = m_pResourceManager->GetTextureMan()->GetTexture(idx);
        if (pText) {
          MODebug2->Message( "moEffectRegularSolid::recursiveTextureLoad > Texture loaded : " + texture_name + " width:" + IntToStr( pText->GetWidth() ) );
        } else {
          MODebug2->Error( "moEffectRegularSolid::recursiveTextureLoad > Texture not loaded...." );
        }
      } else {
        MODebug2->Error( "moEffectRegularSolid::recursiveTextureLoad > Texture not loaded! " + texture_name );
      }

		}

	}



	// Get textures from all children

	for (n = 0; n < nd->mNumChildren; ++n)
		recursiveTextureLoad(sc, nd->mChildren[n]);

}

int moEffectRegularSolid::loadasset (const char* path)
{
	// we are taking one of the postprocessing presets to avoid
	// spelling out 20+ single postprocessing flags here.
	scene = aiImportFile(path,aiProcessPreset_TargetRealtime_MaxQuality);


	if (scene) {
    recursiveTextureLoad(scene, scene->mRootNode);
		get_bounding_box(&scene_min,&scene_max);
		scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
		scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
		scene_center.z = (scene_min.z + scene_max.z) / 2.0f;
		return AI_SUCCESS;
	}
	return AI_FAILURE;
}

void moEffectRegularSolid::apply_material(const aiMaterial *mtl)
{
	float c[4];

	GLenum fill_mode;
	int ret1, ret2;
	aiColor4D diffuse;
	aiColor4D specular;
	aiColor4D ambient;
	aiColor4D emission;
	float shininess, strength;
	int two_sided;
	int wireframe;
	unsigned int max;	// changed: to unsigned

	int texIndex = 0;
	aiString texPath;	//contains filename of texture

	if(AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath))
	{
		//bind texture
		//unsigned int texId = *textureIdMap[texPath.data];
		moText texture_name = moText( texPath.data );
    texture_name = AssetFile.GetPath() + texture_name;
    MODebug2->Message("moEffectRegularSolid::apply_material > loading texture: " + texture_name );
    int tex_moid = GetResourceManager()->GetTextureMan()->GetTextureMOId( texture_name, false );

    moTexture* Texture = NULL;

    unsigned int texId = 0;

    if (tex_moid>-1) {
      Texture = GetResourceManager()->GetTextureMan()->GetTexture(tex_moid);
      if (Texture) {
        texId = Texture->GetGLId();
      }
    }

		glBindTexture(GL_TEXTURE_2D, texId);
	}

	set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
		color4_to_float4(&diffuse, c);

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);

	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
		color4_to_float4(&specular, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);

	set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
		color4_to_float4(&ambient, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);

	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
		color4_to_float4(&emission, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);

	max = 1;
	ret1 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
	max = 1;
	ret2 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
	if((ret1 == AI_SUCCESS) && (ret2 == AI_SUCCESS))
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
	else {
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
		set_float4(c, 0.0f, 0.0f, 0.0f, 0.0f);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
	}

	max = 1;
	if(AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_ENABLE_WIREFRAME, &wireframe, &max))
		fill_mode = wireframe ? GL_LINE : GL_FILL;
	else
		fill_mode = GL_FILL;
	glPolygonMode(GL_FRONT_AND_BACK, fill_mode);

	max = 1;
	if((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, &max)) && two_sided)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}


void moEffectRegularSolid::recursive_render (const aiScene *sc, const aiNode* nd, float scale)
{
	unsigned int i;
	unsigned int n=0, t;
	aiMatrix4x4 m = nd->mTransformation;

	aiMatrix4x4 m2;
	aiMatrix4x4::Scaling(aiVector3D(scale, scale, scale), m2);
	m = m * m2;

	// update transform
	m.Transpose();
	glPushMatrix();
	glMultMatrixf((float*)&m);

	// draw all meshes assigned to this node
	for (; n < nd->mNumMeshes; ++n)
	{
		const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];

		apply_material(sc->mMaterials[mesh->mMaterialIndex]);


		if(mesh->mNormals == NULL)
		{
			glDisable(GL_LIGHTING);
		}
		else
		{
			glEnable(GL_LIGHTING);
		}

		if(mesh->mColors[0] != NULL)
		{
			glEnable(GL_COLOR_MATERIAL);
		}
		else
		{
			glDisable(GL_COLOR_MATERIAL);
		}



		for (t = 0; t < mesh->mNumFaces; ++t) {
			const struct aiFace* face = &mesh->mFaces[t];
			GLenum face_mode;

			switch(face->mNumIndices)
			{
				case 1: face_mode = GL_POINTS; break;
				case 2: face_mode = GL_LINES; break;
				case 3: face_mode = GL_TRIANGLES; break;
				default: face_mode = GL_POLYGON; break;
			}

			glBegin(face_mode);

			for(i = 0; i < face->mNumIndices; i++)		// go through all vertices in face
			{
				int vertexIndex = face->mIndices[i];	// get group index for current index
				if(mesh->mColors[0] != NULL)
					glColor4f( mesh->mColors[0][vertexIndex].r, mesh->mColors[0][vertexIndex].g, mesh->mColors[0][vertexIndex].b, mesh->mColors[0][vertexIndex].a);
				if(mesh->mNormals != NULL)

					if(mesh->HasTextureCoords(0))		//HasTextureCoords(texture_coordinates_set)
					{
						glTexCoord2f(mesh->mTextureCoords[0][vertexIndex].x, 1 - mesh->mTextureCoords[0][vertexIndex].y); //mTextureCoords[channel][vertex]
					}

					if (mesh->mNormals) glNormal3fv(&mesh->mNormals[vertexIndex].x);
					else glNormal3f( 1.0, 0.0, 0.0 );
					if (mesh->mVertices) glVertex3fv(&mesh->mVertices[vertexIndex].x);
			}

			glEnd();

		}

	}


	// draw all children
	for (n = 0; n < nd->mNumChildren; ++n)
	{
		recursive_render(sc, nd->mChildren[n], scale);
	}

	glPopMatrix();
}
*/


//========================
//  Efecto
//========================
moEffectRegularSolid::moEffectRegularSolid() {
	SetName("regularsolid");
}

moEffectRegularSolid::~moEffectRegularSolid() {
	Finish();
}

MOboolean moEffectRegularSolid::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( REGULARSOLID_ALPHA, moText("alpha") );
	moDefineParamIndex( REGULARSOLID_COLOR, moText("color") );
	moDefineParamIndex( REGULARSOLID_SYNC, moText("syncro") );
	moDefineParamIndex( REGULARSOLID_PHASE, moText("phase") );
	moDefineParamIndex( REGULARSOLID_OBJECT, moText("object") );
	moDefineParamIndex( REGULARSOLID_TEXTURE, moText("texture") );
	moDefineParamIndex( REGULARSOLID_MODEL_AMBIENT, moText("modelambient") );
	moDefineParamIndex( REGULARSOLID_AMBIENT, moText("ambient") );
	moDefineParamIndex( REGULARSOLID_SPECULAR, moText("specular") );
	moDefineParamIndex( REGULARSOLID_DIFFUSE, moText("diffuse") );
	moDefineParamIndex( REGULARSOLID_POLYGONMODE, moText("polygonmode") );
	moDefineParamIndex( REGULARSOLID_BLENDING, moText("blending") );
	moDefineParamIndex( REGULARSOLID_SEGMENTSA, moText("segmentsa") );
	moDefineParamIndex( REGULARSOLID_SEGMENTSB, moText("segmentsb") );
	moDefineParamIndex( REGULARSOLID_SEGMENTSC, moText("segmentsc") );
	moDefineParamIndex( REGULARSOLID_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( REGULARSOLID_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( REGULARSOLID_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( REGULARSOLID_ROTATEX, moText("rotatex") );
	moDefineParamIndex( REGULARSOLID_ROTATEY, moText("rotatey") );
	moDefineParamIndex( REGULARSOLID_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( REGULARSOLID_SCALEX, moText("scalex") );
	moDefineParamIndex( REGULARSOLID_SCALEY, moText("scaley") );
	moDefineParamIndex( REGULARSOLID_SCALEZ, moText("scalez") );
	moDefineParamIndex( REGULARSOLID_EYEX, moText("eyex") );
	moDefineParamIndex( REGULARSOLID_EYEY, moText("eyey") );
	moDefineParamIndex( REGULARSOLID_EYEZ, moText("eyez") );
	moDefineParamIndex( REGULARSOLID_VIEWX, moText("viewx") );
	moDefineParamIndex( REGULARSOLID_VIEWY, moText("viewy") );
	moDefineParamIndex( REGULARSOLID_VIEWZ, moText("viewz") );
	moDefineParamIndex( REGULARSOLID_LIGHTX, moText("lightx") );
	moDefineParamIndex( REGULARSOLID_LIGHTY, moText("lighty") );
	moDefineParamIndex( REGULARSOLID_LIGHTZ, moText("lightz") );
	moDefineParamIndex( REGULARSOLID_INLET, moText("inlet") );
	moDefineParamIndex( REGULARSOLID_OUTLET, moText("outlet") );



	Tx = 0; Ty = 0; Tz = 0;
	Sx = 1; Sy = 1; Sz = 1;
	Rx = 0; Ry = 0; Rz = 0;

//  str = (aiString*)malloc(sizeof(struct aiString));

	g_ViewMode = GL_TRIANGLES;
//  aiLogStream stream;
//  stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT,NULL);
//	aiAttachLogStream(&stream);

//  stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE,"assimp_log.txt");
//	aiAttachLogStream(&stream);

//  loadedAsset = AI_FAILURE;

  ArcBall.setBounds( 1.0, 1.0 );

isClicked  = false;
    isRPressed = false;
    isDragging = false;

    Transform.s.XX = 1.0f;
    Transform.s.XY = 0.0f;
    Transform.s.XZ = 0.0f;
    Transform.s.XW = 0.0f;
    Transform.s.YX = 0.0f;
    Transform.s.YY = 1.0f;
    Transform.s.YZ = 0.0f;
    Transform.s.YW = 0.0f;
    Transform.s.ZX = 0.0f;
    Transform.s.ZY = 0.0f;
    Transform.s.ZZ = 1.0f;
    Transform.s.ZW = 0.0f;
    Transform.s.TX = 0.0f;
    Transform.s.TY = 0.0f;
    Transform.s.TZ = 0.0f;
    Transform.s.TW = 1.0f;
    LastRot.s.XX = 1.0f;
    LastRot.s.XY = 0.0f;
    LastRot.s.XZ = 0.0f;
    LastRot.s.YX = 0.0f;
    LastRot.s.YY = 1.0f;
    LastRot.s.YZ = 0.0f;
    LastRot.s.ZX = 0.0f;
    LastRot.s.ZY = 0.0f;
    LastRot.s.ZZ = 1.0f;
    ThisRot.s.XX = 1.0f;
    ThisRot.s.XY = 0.0f;
    ThisRot.s.XZ = 0.0f;
    ThisRot.s.YX = 0.0f;
    ThisRot.s.YY = 1.0f;
    ThisRot.s.YZ = 0.0f;
    ThisRot.s.ZX = 0.0f;
    ThisRot.s.ZY = 0.0f;
    ThisRot.s.ZZ = 1.0f;

    distance_z = 0;
    show_cursor = 0;

	return true;
}

void moEffectRegularSolid::UpdateRotation()
{
    if (isRPressed)													// If Reset button pressed, Reset All Rotations
    {
		Matrix3fSetIdentity(&LastRot);								// Reset Rotation
		Matrix3fSetIdentity(&ThisRot);								// Reset Rotation
        Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);		// Reset Rotation
    }

    if (!isDragging)												// Not Dragging
    {
        if (isClicked)												// First Click
        {
			isDragging = true;										// Prepare For Dragging
			LastRot = ThisRot;										// Set Last Static Rotation To Last Dynamic One
			ArcBall.click(&MousePt);								// Update Start Vector And Prepare For Dragging
		}
    }
    else
    {
        if (isClicked)												// Still Clicked, So Still Dragging
        {
            Quat4fT     ThisQuat;

            ArcBall.drag(&MousePt, &ThisQuat);						// Update End Vector And Get Rotation As Quaternion
            Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);		// Convert Quaternion Into Matrix3fT
            Matrix3fMulMatrix3f(&ThisRot, &LastRot);				// Accumulate Last Rotation Into This One
            Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);	// Set Our Final Transform's Rotation From This One
        }
        else														// No Longer Dragging
			isDragging = false;
    }
}

void moEffectRegularSolid::UpdateParameters() {


  Tx = m_Config.Eval( moR(REGULARSOLID_TRANSLATEX));
  Ty = m_Config.Eval( moR(REGULARSOLID_TRANSLATEY));
  Tz = m_Config.Eval( moR(REGULARSOLID_TRANSLATEZ));
  Sx = m_Config.Eval( moR(REGULARSOLID_SCALEX) );
  Sy = m_Config.Eval( moR(REGULARSOLID_SCALEY) );
  Sz = m_Config.Eval( moR(REGULARSOLID_SCALEZ) );
  Rx = m_Config.Eval( moR(REGULARSOLID_ROTATEX) );
  Ry = m_Config.Eval( moR(REGULARSOLID_ROTATEY) );
  Rz = m_Config.Eval( moR(REGULARSOLID_ROTATEZ) );

  int solid_object = m_Config.Int( moR(REGULARSOLID_TEXTURE) );
  float sA = m_Config.Int( moR(REGULARSOLID_SEGMENTSA) );
  float sB = m_Config.Int( moR(REGULARSOLID_SEGMENTSB) );
  float sC = m_Config.Int( moR(REGULARSOLID_SEGMENTSC) );

  if (solid_object!=m_SolidObject || sA!=m_SegmentsA || sB!=m_SegmentsB || sC!=m_SegmentsC) {

        switch(solid_object) {
            case REGULARSOLID_OBJECT_TETRA:

                break;
            case REGULARSOLID_OBJECT_HEXA:
                break;
            case REGULARSOLID_OBJECT_OCTA:
                break;
            case REGULARSOLID_OBJECT_ICOSA:
                break;
            case REGULARSOLID_OBJECT_DODECA:
                break;
            case REGULARSOLID_OBJECT_SPHERE:
                ///float radius, int widthSegments, int heightSegments, float phiStart, float phiLength, float thetaStart, float thetaLength
                m_Sphere = moSphereGeometry( 1.0f, m_SegmentsA, m_SegmentsB  );
                break;
            case REGULARSOLID_OBJECT_PLANE:
                break;
        }
    m_SolidObject = solid_object;
    m_SegmentsA = sA;
    m_SegmentsB = sB;
    m_SegmentsC = sC;
  }

  /*if ( AssetFile.GetCompletePath() != testasset ) {

    AssetFile.SetCompletePath( testasset );

    MODebug2->Message( "moEffectRegularSolid::UpdateParameters > Loading Asset File, complete path is: "
                      + m_pResourceManager->GetDataMan()->GetDataPath() + AssetFile.GetCompletePath() );

    scene_list = 0;
    loadedAsset = loadasset( m_pResourceManager->GetDataMan()->GetDataPath() + AssetFile.GetCompletePath() );
  }
*/

  moData* TD = m_Config[moR(REGULARSOLID_TEXTURE)].GetData();
  if (TD && TD->Texture()==NULL) UpdateConnectors();
	///MATERIAL
	///pTMan->GetTextureMOId( "default", false )
  if (TD) m_Mat.m_Map = TD->Texture();
  //m_Mat.m_MapGLId = m_Config.GetGLId( moR(REGULARSOLID_TEXTURE), this );///Mat.m_Map->GetGLId();
  m_Mat.m_MapGLId = m_Config.GetGLId( moR(REGULARSOLID_TEXTURE), &this->m_EffectState.tempo );///Mat.m_Map->GetGLId();
  m_Mat.m_Color = m_Effect3D.m_Material.m_Color;
  m_Mat.m_fOpacity = m_Effect3D.m_Material.m_fOpacity;
  m_Mat.m_fTextWSegments = sA;
  m_Mat.m_fTextHSegments = sB;
  m_Mat.m_vLight = moVector3f( 0.0, 0.0, -1.0 );
  m_Mat.m_vLight.Normalize();
  //Mat.m_PolygonMode = MO_POLYGONMODE_LINE;
  m_Mat.m_PolygonMode = MO_POLYGONMODE_FILL;
  //m_Mat.m_fWireframeWidth = 0.0005f;
  m_Mat.m_fWireframeWidth = 0.0f;

  eyeX = m_Config.Eval( moR(REGULARSOLID_EYEX));
  eyeY = m_Config.Eval( moR(REGULARSOLID_EYEY));
  eyeZ = m_Config.Eval( moR(REGULARSOLID_EYEZ));

  viewX = m_Config.Eval( moR(REGULARSOLID_VIEWX));
  viewY = m_Config.Eval( moR(REGULARSOLID_VIEWY));
  viewZ = m_Config.Eval( moR(REGULARSOLID_VIEWZ));

  upX = 0;
  upY = 1;
  upZ = 0;


}

int moEffectRegularSolid::RenderModel( int w, int h) {

/*  if (loadedAsset!=AI_SUCCESS) {

    return;

  }
*/
	if(scene_list == 0) {

	    /*scene_list = glGenLists(1);
	    glNewList(scene_list, GL_COMPILE);

*/


    // now begin at the root node of the imported data and traverse
    // the scenegraph by multiplying subsequent local transforms
    // together on GL's matrix stack.
//	    recursive_render(scene, scene->mRootNode, 1.0 /*scale*/);
	    /*glEndList();*/
	}


///GEOMETRY
    ///check UpdateParameters

    ///MESH MODEL (aka SCENE NODE)
    m_Model.MakeIdentity()
         .Rotate( Rz*moMathf::DEG_TO_RAD, 0.0, 0.0, 1.0 )
         .Rotate( Ry*moMathf::DEG_TO_RAD, 0.0, 1.0, 0.0 )
         .Rotate( Rx*moMathf::DEG_TO_RAD, 1.0, 0.0, 0.0 )
         .Scale( Sx, Sy, Sz )
         .Translate( Tx, Ty, Tz );
    moMesh Mesh( m_Sphere, m_Mat );
    Mesh.SetModelMatrix(m_Model);

    ///CAMERA PERSPECTIVE
    moCamera3D Camera3D;
    m_pResourceManager->GetGLMan()->SetDefaultPerspectiveView( w, h );
    m_pResourceManager->GetGLMan()->LookAt( eyeX, eyeY, eyeZ, viewX, viewY, viewZ, upX, upY, upZ );
    //  Camera3D.MakePerspective(60.0f, p_display_info.Proportion(), 0.01f, 1000.0f );
    Camera3D = m_pResourceManager->GetGLMan()->GetProjectionMatrix();

    ///RENDERING
    m_pResourceManager->GetRenderMan()->Render( &Mesh, &Camera3D );

	//glCallList(scene_list);
}

void moEffectRegularSolid::Draw( moTempo* tempogral,moEffectState* parentstate)
{


    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    PreDraw( tempogral, parentstate);

/**
    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    glPushMatrix();
    m_pResourceManager->GetGLMan()->SetPerspectiveView( w, h );


    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    gluLookAt(		 m_Config.Eval( moR(REGULARSOLID_EYEX)),
                    m_Config.Eval( moR(REGULARSOLID_EYEY)),
                    m_Config.Eval( moR(REGULARSOLID_EYEZ)),
                    m_Config.Eval( moR(REGULARSOLID_VIEWX)),
                    m_Config.Eval( moR(REGULARSOLID_VIEWY)),
                    m_Config.Eval( moR(REGULARSOLID_VIEWZ)),
                    0, 1, 0);
*/


    // Guardar y resetar la matriz de vista del modelo //
    glMatrixMode(GL_MODELVIEW);                         // Select The Modelview Matrix

    UpdateParameters();

    glPushMatrix();                                     // Store The Modelview Matrix
    glLoadIdentity();									// Reset The View


    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);		 // Enables Smooth Shading
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClearDepth(1.0f);				// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);		// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);			// The Type Of Depth Test To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculation
    glDisable(GL_BLEND);

/**
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);    // Uses default lighting parameters
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_NORMALIZE);

    //moVector4d AmbientV4 = m_Config.Vector4d( moR( REGULARSOLID_MODEL_AMBIENT ) );
    LightModelAmbient[0] = m_Config.Eval(  moR( REGULARSOLID_MODEL_AMBIENT ) );
    LightModelAmbient[1] = LightModelAmbient[0];
    LightModelAmbient[2] =  LightModelAmbient[0];
    LightModelAmbient[3] =  1.0;
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, LightModelAmbient );

    glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);

    LightPosition[0] = m_Config.Eval( moR(REGULARSOLID_LIGHTX) );
    LightPosition[1] = m_Config.Eval( moR(REGULARSOLID_LIGHTY) );
    LightPosition[2] =  m_Config.Eval( moR(REGULARSOLID_LIGHTZ) );
    glLightfv(GL_LIGHT0,GL_POSITION,LightPosition);
*/
    // XXX docs say all polygons are emitted CCW, but tests show that some aren't.
    //if(getenv("MODEL_IS_BROKEN"))
    glFrontFace(GL_CW);

	//glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

/*
    // Cambiar la proyeccion para una vista ortogonal //
	//glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	//glEnable(GL_ALPHA_TEST);
	//glDisable(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_CULL_FACE);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	glEnable(GL_SMOOTH);
	glEnable(GL_LIGHT0);								// Turn on a light with defaults set
	glEnable(GL_LIGHTING);								// Turn on lighting
	glEnable(GL_COLOR_MATERIAL);						// Allow color
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	float specularColor[4] = {1.0,1.0,1.0,1.0};
	float LightPos[3] = {0,30,0};
	float LightDif[4] = {1.0f,1.0f,1.0f,1.0f};
	float LightAmb[4] = {1.0f,1.0f,1.0f,1.0f};
	float g_Shine = 50.0;

	// This is where we actually set the specular color and intensity.

	// To change the material properties of our objects, we use glMaterialfv().
	// GL_FRONT_AND_BACK says that we want this applied to both sides of the polygons.
	// GL_SPECULAR say that we want to set the specular color.
	// specularColor is our color array, so we pass that in lastly.
	specularColor[0] = m_Config[moR(REGULARSOLID_SPECULAR)][MO_SELECTED][MO_RED].Fun()->Eval(m_EffectState.tempo.ang);
	specularColor[1] = m_Config[moR(REGULARSOLID_SPECULAR)][MO_SELECTED][MO_GREEN].Fun()->Eval(m_EffectState.tempo.ang);
	specularColor[2] = m_Config[moR(REGULARSOLID_SPECULAR)][MO_SELECTED][MO_BLUE].Fun()->Eval(m_EffectState.tempo.ang);
	specularColor[3] = m_Config[moR(REGULARSOLID_SPECULAR)][MO_SELECTED][MO_ALPHA].Fun()->Eval(m_EffectState.tempo.ang);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);


	// The same thing applies as above, but this time we pass in
	// GL_SHININESS to say we want to change the intensity of the specular.
	// We then pass in an address to our shine intensity, g_Shine.
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 80.0);

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	//glLightiv(GL_LIGHT0,GL_POSITION,LightPos);

// Give OpenGL our position,	then view,		then up vector
	gluPerspective(45.0f,(float)m_pResourceManager->GetRenderMan()->ScreenWidth() / (float)m_pResourceManager->GetRenderMan()->ScreenHeight(), 4.0f ,4000.0f);
	//m_pResourceManager->GetGLMan()->SetPerspectiveView(m_pResourceManager->GetRenderMan()->ScreenWidth(),m_pResourceManager->GetRenderMan()->ScreenHeight());

	gluLookAt(		m_Config[moR(REGULARSOLID_EYEX)].GetData()->Fun()->Eval(m_EffectState.tempo.ang),
					m_Config[moR(REGULARSOLID_EYEY)].GetData()->Fun()->Eval(m_EffectState.tempo.ang),
					m_Config[moR(REGULARSOLID_EYEZ)].GetData()->Fun()->Eval(m_EffectState.tempo.ang),
					m_Config[moR(REGULARSOLID_VIEWX)].GetData()->Fun()->Eval(m_EffectState.tempo.ang),
					m_Config[moR(REGULARSOLID_VIEWY)].GetData()->Fun()->Eval(m_EffectState.tempo.ang),
					m_Config[moR(REGULARSOLID_VIEWZ)].GetData()->Fun()->Eval(m_EffectState.tempo.ang),
					0, 1, 0);

	LightPos[0] = m_Config[moR(REGULARSOLID_LIGHTX)].GetData()->Fun()->Eval(m_EffectState.tempo.ang);
	LightPos[1] = m_Config[moR(REGULARSOLID_LIGHTY)].GetData()->Fun()->Eval(m_EffectState.tempo.ang);
	LightPos[2] = m_Config[moR(REGULARSOLID_LIGHTZ)].GetData()->Fun()->Eval(m_EffectState.tempo.ang);
	glLightfv(GL_LIGHT0,GL_POSITION,LightPos);


	LightDif[0] = m_Config[moR(REGULARSOLID_DIFFUSE)][MO_SELECTED][MO_RED].Fun()->Eval(m_EffectState.tempo.ang);
	LightDif[1] = m_Config[moR(REGULARSOLID_DIFFUSE)][MO_SELECTED][MO_GREEN].Fun()->Eval(m_EffectState.tempo.ang);
	LightDif[2] = m_Config[moR(REGULARSOLID_DIFFUSE)][MO_SELECTED][MO_BLUE].Fun()->Eval(m_EffectState.tempo.ang);
	LightDif[3] = m_Config[moR(REGULARSOLID_DIFFUSE)][MO_SELECTED][MO_ALPHA].Fun()->Eval(m_EffectState.tempo.ang);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,LightDif);

	LightAmb[0] = m_Config[moR(REGULARSOLID_AMBIENT)][MO_SELECTED][MO_RED].Fun()->Eval(m_EffectState.tempo.ang);
	LightAmb[1] = m_Config[moR(REGULARSOLID_AMBIENT)][MO_SELECTED][MO_GREEN].Fun()->Eval(m_EffectState.tempo.ang);
	LightAmb[2] = m_Config[moR(REGULARSOLID_AMBIENT)][MO_SELECTED][MO_BLUE].Fun()->Eval(m_EffectState.tempo.ang);
	LightAmb[3] = m_Config[moR(REGULARSOLID_AMBIENT)][MO_SELECTED][MO_ALPHA].Fun()->Eval(m_EffectState.tempo.ang);
	glLightfv(GL_LIGHT0,GL_AMBIENT,LightAmb);


*/
    // Draw //
	//BEGIN
	//glBindTexture(GL_TEXTURE_2D, Images.Get(Image,&m_EffectState.tempo));
                  // Select The Modelview Matrix

	SetPolygonMode( (moPolygonModes)m_Config[moR(REGULARSOLID_POLYGONMODE)].GetValue().GetSubValue(0).Int());
	//SetBlending( );

  SetColor( m_Config[moR(REGULARSOLID_COLOR)], m_Config[moR(REGULARSOLID_ALPHA)], m_EffectState );

	SetBlending( (moBlendingModes) m_Config.Int( moR(REGULARSOLID_BLENDING) ) );
/**
	glTranslatef(   m_Config.Eval( moR(REGULARSOLID_TRANSLATEX) ),
                  m_Config.Eval( moR(REGULARSOLID_TRANSLATEY) ),
                  m_Config.Eval( moR(REGULARSOLID_TRANSLATEZ) ));

	glRotatef(  m_Config.Eval( moR(REGULARSOLID_ROTATEX) ), 1.0, 0.0, 0.0 );
  glRotatef(  m_Config.Eval( moR(REGULARSOLID_ROTATEY) ), 0.0, 1.0, 0.0 );
  glRotatef(  m_Config.Eval( moR(REGULARSOLID_ROTATEZ) ), 0.0, 0.0, 1.0 );

	glScalef(   m_Config.Eval( moR(REGULARSOLID_SCALEX) ),
                m_Config.Eval( moR(REGULARSOLID_SCALEY) ),
                m_Config.Eval( moR(REGULARSOLID_SCALEZ) ));
*/

  /*float tmp = scene_max.x-scene_min.x;
	tmp = aisgl_max(scene_max.y - scene_min.y,tmp);
	tmp = aisgl_max(scene_max.z - scene_min.z,tmp);
	tmp = 1.f / tmp;
	glScalef(tmp, tmp, tmp);
*/
        // center the model
//	glTranslatef( -scene_center.x, -scene_center.y, -scene_center.z );

  // if the display list has not been made yet, create a new one and
  // fill it with scene contents

  RenderModel( w, h );

	glDisable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

MOboolean moEffectRegularSolid::Finish()
{

    return PreFinish();
}



void moEffectRegularSolid::Update( moEventList* p_EventList ) {



  moEffect::Update(p_EventList);
}

void moEffectRegularSolid::Interaction( moIODeviceManager *IODeviceManager ) {

	moDeviceCode *temp;
	MOint did,cid,state,valor;
	moText txt;

	if (devicecode!=NULL)
	for(int i=0; i<ncodes;i++) {
		temp = devicecode[i].First;
		while(temp!=NULL) {
			did = temp->device;
			cid = temp->devicecode;
			state = IODeviceManager->IODevices().Get(did)->GetStatus(cid);
			valor = IODeviceManager->IODevices().Get(did)->GetValue(cid);
			if (state)
			switch(i) {

        case MO_MOUSE_MOTION_X:
					MousePt.s.X = valor;
					break;
				case MO_MOUSE_MOTION_Y:
					MousePt.s.Y = valor;
					break;
				case MO_MOUSE_DOWN_LEFT:
					isClicked = true;
					break;
				case MO_MOUSE_MOTION_Y_RIGHT:
					distance_z -= 5.0 * valor;
					break;
				case MO_KEY_PRESS_C:
					show_cursor = !show_cursor;
					//SDL_ShowCursor(show_cursor);
					break;

				case MO_REGULARSOLID_TRANSLATE_X:
					Tx = valor;
					txt = FloatToStr(Tx);
					MODebug->Push(moText("Tx: ") + (moText)txt);
					break;
				case MO_REGULARSOLID_TRANSLATE_Y:
					Ty = valor;
					txt = FloatToStr(Ty);
					MODebug->Push(moText("Ty: ") + (moText)txt);
					break;
				case MO_REGULARSOLID_TRANSLATE_Z:
					Tz = valor;
					txt = FloatToStr(Tz);
					MODebug->Push(moText("Tz: ") + (moText)txt);
					break;
				case MO_REGULARSOLID_SCALE_X:
					Sx+=10*((float) valor / (float) 256.0);
					txt = FloatToStr(Sx);
					MODebug->Push(moText("Sx: ") + (moText)txt);
					break;
				case MO_REGULARSOLID_SCALE_Y:
					Sy+=10*((float) valor / (float) 256.0);
					txt = FloatToStr(Sy);
					MODebug->Push(moText("Sy: ") + (moText)txt);
				case MO_REGULARSOLID_SCALE_Z:
					Sz+=10*((float) valor / (float) 256.0);
					txt = FloatToStr(Sz);
					MODebug->Push(moText("Sz: ") + (moText)txt);
					break;
			}
		temp = temp->next;
		}
	}

}


moConfigDefinition *
moEffectRegularSolid::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
  p_configdefinition->Add( moText("object"), MO_PARAM_NUMERIC, REGULARSOLID_OBJECT, moValue( "0", "NUM").Ref(), moText("Tetra,Hexa,Octa,Icosa,Dodeca,Sphere")  );
  p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, REGULARSOLID_TEXTURE, moValue( "default", "TXT") );
  p_configdefinition->Add( moText("modelambient"), MO_PARAM_FUNCTION, REGULARSOLID_MODEL_AMBIENT );
	p_configdefinition->Add( moText("ambient"), MO_PARAM_COLOR, REGULARSOLID_AMBIENT );
	p_configdefinition->Add( moText("specular"), MO_PARAM_COLOR, REGULARSOLID_SPECULAR );
	p_configdefinition->Add( moText("diffuse"), MO_PARAM_COLOR, REGULARSOLID_DIFFUSE );
	p_configdefinition->Add( moText("polygonmode"), MO_PARAM_POLYGONMODE, REGULARSOLID_POLYGONMODE, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, REGULARSOLID_BLENDING, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("segmentsa"), MO_PARAM_NUMERIC, REGULARSOLID_SEGMENTSA, moValue( "8", "NUM").Ref() );
	p_configdefinition->Add( moText("segmentsb"), MO_PARAM_NUMERIC, REGULARSOLID_SEGMENTSB,  moValue( "8", "NUM").Ref() );
	p_configdefinition->Add( moText("segmentsc"), MO_PARAM_NUMERIC, REGULARSOLID_SEGMENTSC,  moValue( "8", "NUM").Ref() );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, REGULARSOLID_TRANSLATEX );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, REGULARSOLID_TRANSLATEY );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, REGULARSOLID_TRANSLATEZ );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, REGULARSOLID_ROTATEX );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, REGULARSOLID_ROTATEY );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, REGULARSOLID_ROTATEZ );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, REGULARSOLID_SCALEX );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, REGULARSOLID_SCALEY );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, REGULARSOLID_SCALEZ );
	p_configdefinition->Add( moText("eyex"), MO_PARAM_FUNCTION, REGULARSOLID_EYEX );
	p_configdefinition->Add( moText("eyey"), MO_PARAM_FUNCTION, REGULARSOLID_EYEY );
	p_configdefinition->Add( moText("eyez"), MO_PARAM_FUNCTION, REGULARSOLID_EYEZ );
	p_configdefinition->Add( moText("viewx"), MO_PARAM_FUNCTION, REGULARSOLID_VIEWX );
	p_configdefinition->Add( moText("viewy"), MO_PARAM_FUNCTION, REGULARSOLID_VIEWY );
	p_configdefinition->Add( moText("viewz"), MO_PARAM_FUNCTION, REGULARSOLID_VIEWZ );
	p_configdefinition->Add( moText("lightx"), MO_PARAM_FUNCTION, REGULARSOLID_LIGHTX );
	p_configdefinition->Add( moText("lighty"), MO_PARAM_FUNCTION, REGULARSOLID_LIGHTY );
	p_configdefinition->Add( moText("lightz"), MO_PARAM_FUNCTION, REGULARSOLID_LIGHTZ );
	return p_configdefinition;
}
