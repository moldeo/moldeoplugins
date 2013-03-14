/*******************************************************************************

                                moOpenCV.h

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

  Class:
  moOpenCV

  Description:
  Motion Analyzer


*******************************************************************************/

#include "cv.h"

#include "moConfig.h"
#include "moDeviceCode.h"
#include "moEventList.h"
#include "moIODeviceManager.h"
#include "moTypes.h"
#include "moVideoGraph.h"
#include "moArray.h"
#include "moShaderGLSL.h"
#include "moRenderManager.h"

#include "moFilterManager.h"
#include "moVideoGraph.h"

#include "moTextureFilter.h"
#include "moTextureFilterIndex.h"


/*
#include "cvblob.h"
*/

/* //BlobsLib
#include "blob.h"
#include "BlobResult.h"
*/

#ifndef __MO_OPENCV_H
#define __MO_OPENCV_H

enum moOpenCVParamIndex {
    OPENCV_INLET=0,
    OPENCV_OUTLET,
    OPENCV_SCRIPT,
    OPENCV_TEXTURE,
	OPENCV_THRESHOLD,
	OPENCV_THRESHOLD_MAX
};

#define MO_OPENCV_SYTEM_LABELNAME	0
#define MO_OPENCV_LIVE_SYSTEM	1
#define MO_OPENCV_SYSTEM_ON 2

#define w 500

class moOpenCVSystem : public moAbstract {

	public:

		moOpenCVSystem();
		virtual ~moOpenCVSystem();

};


typedef moOpenCVSystem* moOpenCVSystemPtr;
/*
template class LIBMOLDEO_API moDynamicArray<moOpenCVSystemPtr>;
typedef  moDynamicArray<moOpenCVSystemPtr> moOpenCVSystems;
*/
moDeclareDynamicArray( moOpenCVSystemPtr, moOpenCVSystems );


class moOpenCV : public moResource
{
public:
    moOpenCV();
    virtual ~moOpenCV();

    virtual MOboolean Init();
    virtual MOboolean Finish();

    MOswitch GetStatus(MOdevcode);
    MOswitch SetStatus( MOdevcode,MOswitch);
	void SetValue( MOdevcode cd, MOint vl );
	void SetValue( MOdevcode cd, MOfloat vl );
    MOint GetValue(MOdevcode);
	MOpointer GetPointer(MOdevcode devcode );
    MOdevcode GetCode( moText);

    CvSeq* findSquares4( IplImage* img, CvMemStorage* storage );
    void drawSquares( IplImage* img, CvSeq* squares );

    void UpdateParameters();
    void Update(moEventList*);

    moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

    moTrackerSystemData*    m_pTrackerSystemData;

private:

    moTexture* m_pSrcTexture;

    moTexture* m_pDest0Texture; //very old texture
    moTexture* m_pDest1Texture; //old texture
    moTexture* m_pDest2Texture; //new texture

    moTexture* m_pDestDiff1Texture;//old difference
    moTexture* m_pDestDiff2Texture;//new difference

    moTextureFilter*    m_pTFDest0Texture;//very old texture filter (with shader)
    moTextureFilter*    m_pTFDest1Texture;//old texture filter (with shader)
    moTextureFilter*    m_pTFDest2Texture;//new texture filter (with shader)

    moTextureFilter*    m_pTFDestDiff1Texture;//old texture filter shader
    moTextureFilter*    m_pTFDestDiff2Texture;//new texture filter shader

    moBucket* m_pBucketDiff1;
    moBucket* m_pBucketDiff2;

    float sumN;
    float sumX, sumY;

    int switch_texture;

	int threshold;
	int threshold_max;
    moConfig config;

    moEventList *events;

protected:

    int idopencvout;

    IplImage* img;

	int levels;
    CvSeq* contours;
    CvMemStorage* storage;

	moOpenCVSystems		m_OpenCVSystems;



};


class moOpenCVFactory : public moResourceFactory {

public:
    moOpenCVFactory() {}
    virtual ~moOpenCVFactory() {}
    moResource* Create();
    void Destroy(moResource* fx);
};


extern "C"
{
MO_PLG_API moResourceFactory* CreateResourceFactory();
MO_PLG_API void DestroyResourceFactory();
}


#endif
