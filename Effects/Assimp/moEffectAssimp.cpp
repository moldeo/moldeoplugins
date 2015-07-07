/*******************************************************************************

                                moEffectAssimp.cpp

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

#include "moEffectAssimp.h"

//========================
//  Factory
//========================

moEffectAssimpFactory *m_EffectAssimpFactory = NULL;

MO_PLG_API moEffectFactory* CreateEffectFactory(){
	if(m_EffectAssimpFactory==NULL)
		m_EffectAssimpFactory = new moEffectAssimpFactory();
	return(moEffectFactory*) m_EffectAssimpFactory;
}

MO_PLG_API void DestroyEffectFactory(){
	delete m_EffectAssimpFactory;
	m_EffectAssimpFactory = NULL;
}

moEffect*  moEffectAssimpFactory::Create() {
	return new moEffectAssimp();
}

void moEffectAssimpFactory::Destroy(moEffect* fx) {
	delete fx;
}


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


void moEffectAssimp::recursiveTextureLoad(const aiScene *sc, const aiNode* nd)
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
      MODebug2->Message("moEffectAssimp::recursiveTextureLoad > texture_name "  + texture_name );

      int idx = m_pResourceManager->GetTextureMan()->GetTextureMOId( texture_name, true /*create if not exists*/);
      /*if (idx==-1) {
        texture_name = texture_name + moText(".png");
        idx = m_pResourceManager->GetTextureMan()->GetTextureMOId( texture_name, true );
      }*/

      if (idx>-1) {
        moTexture * pText = m_pResourceManager->GetTextureMan()->GetTexture(idx);
        if (pText) {
          MODebug2->Message( "moEffectAssimp::recursiveTextureLoad > Texture loaded : " + texture_name + " width:" + IntToStr( pText->GetWidth() ) );
        } else {
          MODebug2->Error( "moEffectAssimp::recursiveTextureLoad > Texture not loaded...." );
        }
      } else {
        MODebug2->Error( "moEffectAssimp::recursiveTextureLoad > Texture not loaded! " + texture_name );
      }
/*
			for(int x = 0; x < texturesAndPaths.size(); x++)

			{

				if(texturesAndPaths[x].pathName == *str)

				{

					TextureAndPath reusedTexture;

					reusedTexture.hTexture = texturesAndPaths[x].hTexture;

					reusedTexture.pathName = *str;

					texturesAndPaths.push_back(reusedTexture);

					newTextureToBeLoaded = false;



					std::cout << "Texture reused." << std::endl;



					break;

				}

			}



			if(newTextureToBeLoaded)

			{

				FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(str->data,0);

				//Automatocally detects the format(from over 20 formats!)

				FIBITMAP* imagen = FreeImage_Load(formato, str->data);

				FIBITMAP* temp = imagen;

				imagen = FreeImage_ConvertTo32Bits(imagen);

				FreeImage_Unload(temp);

				int w = FreeImage_GetWidth(imagen);

				int h = FreeImage_GetHeight(imagen);



				//Some debugging code

				char* pixeles = (char*)FreeImage_GetBits(imagen);

				//FreeImage loads in BGR format, so you need to swap some bytes(Or use GL_BGR).

				//Now generate the OpenGL texture object

				TextureAndPath newTexture;

				newTexture.pathName = *str;

				glGenTextures(1, &newTexture.hTexture);



				glBindTexture(GL_TEXTURE_2D, newTexture.hTexture);

				glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, w, h, 0, GL_BGRA_EXT,GL_UNSIGNED_BYTE,(GLvoid*)pixeles );

				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);



				glBindTexture(GL_TEXTURE_2D, newTexture.hTexture);



				GLenum huboError = glGetError();



				if(huboError)

				{

					std::cout<<"There was an error loading the texture"<<std::endl;

				}



				std::cout << "texture loaded." << std::endl;



				texturesAndPaths.push_back(newTexture);

			}
*/
		}

	}



	// Get textures from all children

	for (n = 0; n < nd->mNumChildren; ++n)
		recursiveTextureLoad(sc, nd->mChildren[n]);

}

int moEffectAssimp::loadasset (const char* path)
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

void moEffectAssimp::apply_material(const aiMaterial *mtl)
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
    MODebug2->Message("moEffectAssimp::apply_material > loading texture: " + texture_name );
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


void moEffectAssimp::recursive_render (const aiScene *sc, const aiNode* nd, float scale)
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



//========================
//  Efecto
//========================
moEffectAssimp::moEffectAssimp() {
	SetName("assimp");
}

moEffectAssimp::~moEffectAssimp() {
	Finish();
}

MOboolean moEffectAssimp::Init() {

    if (!PreInit()) return false;

	moDefineParamIndex( ASSIMP_ALPHA, moText("alpha") );
	moDefineParamIndex( ASSIMP_COLOR, moText("color") );
	moDefineParamIndex( ASSIMP_SYNC, moText("syncro") );
	moDefineParamIndex( ASSIMP_PHASE, moText("phase") );
	moDefineParamIndex( ASSIMP_OBJECT, moText("object") );
	moDefineParamIndex( ASSIMP_TEXTURE, moText("texture") );
	moDefineParamIndex( ASSIMP_MODEL_AMBIENT, moText("modelambient") );
	moDefineParamIndex( ASSIMP_AMBIENT, moText("ambient") );
	moDefineParamIndex( ASSIMP_SPECULAR, moText("specular") );
	moDefineParamIndex( ASSIMP_DIFFUSE, moText("diffuse") );
	moDefineParamIndex( ASSIMP_POLYGONMODE, moText("polygonmode") );
	moDefineParamIndex( ASSIMP_BLENDING, moText("blending") );
	moDefineParamIndex( ASSIMP_TRANSLATEX, moText("translatex") );
	moDefineParamIndex( ASSIMP_TRANSLATEY, moText("translatey") );
	moDefineParamIndex( ASSIMP_TRANSLATEZ, moText("translatez") );
	moDefineParamIndex( ASSIMP_ROTATEX, moText("rotatex") );
	moDefineParamIndex( ASSIMP_ROTATEY, moText("rotatey") );
	moDefineParamIndex( ASSIMP_ROTATEZ, moText("rotatez") );
	moDefineParamIndex( ASSIMP_SCALEX, moText("scalex") );
	moDefineParamIndex( ASSIMP_SCALEY, moText("scaley") );
	moDefineParamIndex( ASSIMP_SCALEZ, moText("scalez") );
	moDefineParamIndex( ASSIMP_EYEX, moText("eyex") );
	moDefineParamIndex( ASSIMP_EYEY, moText("eyey") );
	moDefineParamIndex( ASSIMP_EYEZ, moText("eyez") );
	moDefineParamIndex( ASSIMP_VIEWX, moText("viewx") );
	moDefineParamIndex( ASSIMP_VIEWY, moText("viewy") );
	moDefineParamIndex( ASSIMP_VIEWZ, moText("viewz") );
	moDefineParamIndex( ASSIMP_LIGHTX, moText("lightx") );
	moDefineParamIndex( ASSIMP_LIGHTY, moText("lighty") );
	moDefineParamIndex( ASSIMP_LIGHTZ, moText("lightz") );
	moDefineParamIndex( ASSIMP_INLET, moText("inlet") );
	moDefineParamIndex( ASSIMP_OUTLET, moText("outlet") );



	Tx = 1; Ty = 1; Tz = 1;
	Sx = 1; Sy = 1; Sz = 1;

  str = (aiString*)malloc(sizeof(struct aiString));

	g_ViewMode = GL_TRIANGLES;
  aiLogStream stream;
  stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT,NULL);
	aiAttachLogStream(&stream);

  stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE,"assimp_log.txt");
	aiAttachLogStream(&stream);

  loadedAsset = AI_FAILURE;

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

void moEffectAssimp::UpdateRotation()
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

void moEffectAssimp::UpdateParameters() {

  moText testasset = m_Config.Text( moR(ASSIMP_OBJECT) );

  if ( AssetFile.GetCompletePath() != testasset ) {

    AssetFile.SetCompletePath( testasset );

    MODebug2->Message( "moEffectAssimp::UpdateParameters > Loading Asset File, complete path is: "
                      + m_pResourceManager->GetDataMan()->GetDataPath() + AssetFile.GetCompletePath() );

    scene_list = 0;
    loadedAsset = loadasset( m_pResourceManager->GetDataMan()->GetDataPath() + AssetFile.GetCompletePath() );

    if ( loadedAsset == AI_SUCCESS ) {
      MODebug2->Message("moEffectAssimp::UpdateParameters > loaded asset: "+AssetFile.GetCompletePath());
    } else {
      MODebug2->Error("ERROR: moEffectAssimp::UpdateParameters > loading asset: "+AssetFile.GetCompletePath());
    }
  }



}

void moEffectAssimp::RenderModel() {

  if (loadedAsset!=AI_SUCCESS) {

    return;

  }

	if(scene_list == 0) {

	    scene_list = glGenLists(1);
	    glNewList(scene_list, GL_COMPILE);

    // now begin at the root node of the imported data and traverse
    // the scenegraph by multiplying subsequent local transforms
    // together on GL's matrix stack.
	    recursive_render(scene, scene->mRootNode, 1.0 /*scale*/);
	    glEndList();
	}

	glCallList(scene_list);
}

void moEffectAssimp::Draw( moTempo* tempogral,moEffectState* parentstate)
{


    int w = m_pResourceManager->GetRenderMan()->ScreenWidth();
    int h = m_pResourceManager->GetRenderMan()->ScreenHeight();

    PreDraw( tempogral, parentstate);


    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    glPushMatrix();
    m_pResourceManager->GetGLMan()->SetPerspectiveView( w, h );


    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    gluLookAt(		 m_Config.Eval( moR(ASSIMP_EYEX)),
                    m_Config.Eval( moR(ASSIMP_EYEY)),
                    m_Config.Eval( moR(ASSIMP_EYEZ)),
                    m_Config.Eval( moR(ASSIMP_VIEWX)),
                    m_Config.Eval( moR(ASSIMP_VIEWY)),
                    m_Config.Eval( moR(ASSIMP_VIEWZ)),
                    0, 1, 0);



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


    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);    // Uses default lighting parameters
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_NORMALIZE);

    //moVector4d AmbientV4 = m_Config.Vector4d( moR( ASSIMP_MODEL_AMBIENT ) );
    LightModelAmbient[0] = m_Config.Eval(  moR( ASSIMP_MODEL_AMBIENT ) );
    LightModelAmbient[1] = LightModelAmbient[0];
    LightModelAmbient[2] =  LightModelAmbient[0];
    LightModelAmbient[3] =  1.0;
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, LightModelAmbient );

    glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);

    LightPosition[0] = m_Config.Eval( moR(ASSIMP_LIGHTX) );
    LightPosition[1] = m_Config.Eval( moR(ASSIMP_LIGHTY) );
    LightPosition[2] =  m_Config.Eval( moR(ASSIMP_LIGHTZ) );
    glLightfv(GL_LIGHT0,GL_POSITION,LightPosition);

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
	specularColor[0] = m_Config[moR(ASSIMP_SPECULAR)][MO_SELECTED][MO_RED].Fun()->Eval(m_EffectState.tempo.ang);
	specularColor[1] = m_Config[moR(ASSIMP_SPECULAR)][MO_SELECTED][MO_GREEN].Fun()->Eval(m_EffectState.tempo.ang);
	specularColor[2] = m_Config[moR(ASSIMP_SPECULAR)][MO_SELECTED][MO_BLUE].Fun()->Eval(m_EffectState.tempo.ang);
	specularColor[3] = m_Config[moR(ASSIMP_SPECULAR)][MO_SELECTED][MO_ALPHA].Fun()->Eval(m_EffectState.tempo.ang);
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

	gluLookAt(		m_Config[moR(ASSIMP_EYEX)].GetData()->Fun()->Eval(m_EffectState.tempo.ang),
					m_Config[moR(ASSIMP_EYEY)].GetData()->Fun()->Eval(m_EffectState.tempo.ang),
					m_Config[moR(ASSIMP_EYEZ)].GetData()->Fun()->Eval(m_EffectState.tempo.ang),
					m_Config[moR(ASSIMP_VIEWX)].GetData()->Fun()->Eval(m_EffectState.tempo.ang),
					m_Config[moR(ASSIMP_VIEWY)].GetData()->Fun()->Eval(m_EffectState.tempo.ang),
					m_Config[moR(ASSIMP_VIEWZ)].GetData()->Fun()->Eval(m_EffectState.tempo.ang),
					0, 1, 0);

	LightPos[0] = m_Config[moR(ASSIMP_LIGHTX)].GetData()->Fun()->Eval(m_EffectState.tempo.ang);
	LightPos[1] = m_Config[moR(ASSIMP_LIGHTY)].GetData()->Fun()->Eval(m_EffectState.tempo.ang);
	LightPos[2] = m_Config[moR(ASSIMP_LIGHTZ)].GetData()->Fun()->Eval(m_EffectState.tempo.ang);
	glLightfv(GL_LIGHT0,GL_POSITION,LightPos);


	LightDif[0] = m_Config[moR(ASSIMP_DIFFUSE)][MO_SELECTED][MO_RED].Fun()->Eval(m_EffectState.tempo.ang);
	LightDif[1] = m_Config[moR(ASSIMP_DIFFUSE)][MO_SELECTED][MO_GREEN].Fun()->Eval(m_EffectState.tempo.ang);
	LightDif[2] = m_Config[moR(ASSIMP_DIFFUSE)][MO_SELECTED][MO_BLUE].Fun()->Eval(m_EffectState.tempo.ang);
	LightDif[3] = m_Config[moR(ASSIMP_DIFFUSE)][MO_SELECTED][MO_ALPHA].Fun()->Eval(m_EffectState.tempo.ang);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,LightDif);

	LightAmb[0] = m_Config[moR(ASSIMP_AMBIENT)][MO_SELECTED][MO_RED].Fun()->Eval(m_EffectState.tempo.ang);
	LightAmb[1] = m_Config[moR(ASSIMP_AMBIENT)][MO_SELECTED][MO_GREEN].Fun()->Eval(m_EffectState.tempo.ang);
	LightAmb[2] = m_Config[moR(ASSIMP_AMBIENT)][MO_SELECTED][MO_BLUE].Fun()->Eval(m_EffectState.tempo.ang);
	LightAmb[3] = m_Config[moR(ASSIMP_AMBIENT)][MO_SELECTED][MO_ALPHA].Fun()->Eval(m_EffectState.tempo.ang);
	glLightfv(GL_LIGHT0,GL_AMBIENT,LightAmb);


*/
    // Draw //
	//BEGIN
	//glBindTexture(GL_TEXTURE_2D, Images.Get(Image,&m_EffectState.tempo));
                  // Select The Modelview Matrix

	SetPolygonMode( (moPolygonModes)m_Config[moR(ASSIMP_POLYGONMODE)].GetValue().GetSubValue(0).Int());
	//SetBlending( );

  SetColor( m_Config[moR(ASSIMP_COLOR)], m_Config[moR(ASSIMP_ALPHA)], m_EffectState );

	SetBlending( (moBlendingModes) m_Config.Int( moR(ASSIMP_BLENDING) ) );

	glTranslatef(   m_Config.Eval( moR(ASSIMP_TRANSLATEX) ),
                  m_Config.Eval( moR(ASSIMP_TRANSLATEY) ),
                  m_Config.Eval( moR(ASSIMP_TRANSLATEZ) ));

	glRotatef(  m_Config.Eval( moR(ASSIMP_ROTATEX) ), 1.0, 0.0, 0.0 );
  glRotatef(  m_Config.Eval( moR(ASSIMP_ROTATEY) ), 0.0, 1.0, 0.0 );
  glRotatef(  m_Config.Eval( moR(ASSIMP_ROTATEZ) ), 0.0, 0.0, 1.0 );

	glScalef(   m_Config.Eval( moR(ASSIMP_SCALEX) ),
                m_Config.Eval( moR(ASSIMP_SCALEY) ),
                m_Config.Eval( moR(ASSIMP_SCALEZ) ));


  float tmp = scene_max.x-scene_min.x;
	tmp = aisgl_max(scene_max.y - scene_min.y,tmp);
	tmp = aisgl_max(scene_max.z - scene_min.z,tmp);
	tmp = 1.f / tmp;
	glScalef(tmp, tmp, tmp);

        // center the model
	glTranslatef( -scene_center.x, -scene_center.y, -scene_center.z );

  // if the display list has not been made yet, create a new one and
  // fill it with scene contents
  RenderModel();
	glDisable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

MOboolean moEffectAssimp::Finish()
{
    //Models3d.Finish();

    return PreFinish();
}



void moEffectAssimp::Update( moEventList* p_EventList ) {



  moEffect::Update(p_EventList);
}

void moEffectAssimp::Interaction( moIODeviceManager *IODeviceManager ) {

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

				case MO_ASSIMP_TRANSLATE_X:
					Tx = valor;
					txt = FloatToStr(Tx);
					MODebug->Push(moText("Tx: ") + (moText)txt);
					break;
				case MO_ASSIMP_TRANSLATE_Y:
					Ty = valor;
					txt = FloatToStr(Ty);
					MODebug->Push(moText("Ty: ") + (moText)txt);
					break;
				case MO_ASSIMP_TRANSLATE_Z:
					Tz = valor;
					txt = FloatToStr(Tz);
					MODebug->Push(moText("Tz: ") + (moText)txt);
					break;
				case MO_ASSIMP_SCALE_X:
					Sx+=10*((float) valor / (float) 256.0);
					txt = FloatToStr(Sx);
					MODebug->Push(moText("Sx: ") + (moText)txt);
					break;
				case MO_ASSIMP_SCALE_Y:
					Sy+=10*((float) valor / (float) 256.0);
					txt = FloatToStr(Sy);
					MODebug->Push(moText("Sy: ") + (moText)txt);
				case MO_ASSIMP_SCALE_Z:
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
moEffectAssimp::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moEffect::GetDefinition( p_configdefinition );
  p_configdefinition->Add( moText("object"), MO_PARAM_TEXT, ASSIMP_OBJECT, moValue( "default", "TXT") );
  p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, ASSIMP_TEXTURE, moValue( "default", "TXT") );
  p_configdefinition->Add( moText("modelambient"), MO_PARAM_FUNCTION, ASSIMP_MODEL_AMBIENT );
	p_configdefinition->Add( moText("ambient"), MO_PARAM_COLOR, ASSIMP_AMBIENT );
	p_configdefinition->Add( moText("specular"), MO_PARAM_COLOR, ASSIMP_SPECULAR );
	p_configdefinition->Add( moText("diffuse"), MO_PARAM_COLOR, ASSIMP_DIFFUSE );
	p_configdefinition->Add( moText("polygonmode"), MO_PARAM_POLYGONMODE, ASSIMP_POLYGONMODE, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("blending"), MO_PARAM_BLENDING, ASSIMP_BLENDING, moValue( "0", "NUM").Ref() );
	p_configdefinition->Add( moText("translatex"), MO_PARAM_TRANSLATEX, ASSIMP_TRANSLATEX );
	p_configdefinition->Add( moText("translatey"), MO_PARAM_TRANSLATEY, ASSIMP_TRANSLATEY );
	p_configdefinition->Add( moText("translatez"), MO_PARAM_TRANSLATEZ, ASSIMP_TRANSLATEZ );
	p_configdefinition->Add( moText("rotatex"), MO_PARAM_ROTATEX, ASSIMP_ROTATEX );
	p_configdefinition->Add( moText("rotatey"), MO_PARAM_ROTATEY, ASSIMP_ROTATEY );
	p_configdefinition->Add( moText("rotatez"), MO_PARAM_ROTATEZ, ASSIMP_ROTATEZ );
	p_configdefinition->Add( moText("scalex"), MO_PARAM_SCALEX, ASSIMP_SCALEX );
	p_configdefinition->Add( moText("scaley"), MO_PARAM_SCALEY, ASSIMP_SCALEY );
	p_configdefinition->Add( moText("scalez"), MO_PARAM_SCALEZ, ASSIMP_SCALEZ );
	p_configdefinition->Add( moText("eyex"), MO_PARAM_FUNCTION, ASSIMP_EYEX );
	p_configdefinition->Add( moText("eyey"), MO_PARAM_FUNCTION, ASSIMP_EYEY );
	p_configdefinition->Add( moText("eyez"), MO_PARAM_FUNCTION, ASSIMP_EYEZ );
	p_configdefinition->Add( moText("viewx"), MO_PARAM_FUNCTION, ASSIMP_VIEWX );
	p_configdefinition->Add( moText("viewy"), MO_PARAM_FUNCTION, ASSIMP_VIEWY );
	p_configdefinition->Add( moText("viewz"), MO_PARAM_FUNCTION, ASSIMP_VIEWZ );
	p_configdefinition->Add( moText("lightx"), MO_PARAM_FUNCTION, ASSIMP_LIGHTX );
	p_configdefinition->Add( moText("lighty"), MO_PARAM_FUNCTION, ASSIMP_LIGHTY );
	p_configdefinition->Add( moText("lightz"), MO_PARAM_FUNCTION, ASSIMP_LIGHTZ );
	return p_configdefinition;
}
