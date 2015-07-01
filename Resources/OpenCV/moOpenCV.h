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



#define OPENCV2 1
#define HAVE_OPENGL 0

#include "cv.h"

#ifdef OPENCV2
#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#endif // OPENCV2

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
using namespace std;
using namespace cv;

#ifndef __MO_OPENCV_H
#define __MO_OPENCV_H

enum moOpenCVParamIndex {
    OPENCV_INLET=0,
    OPENCV_OUTLET,
    OPENCV_SCRIPT,
    OPENCV_TEXTURE,
    OPENCV_RECOGNITION_MODE,
    OPENCV_REDUCE_WIDTH,
    OPENCV_REDUCE_HEIGHT,
    OPENCV_THRESHOLD,
    OPENCV_THRESHOLD_MAX,
    OPENCV_CROP_MIN_X,
    OPENCV_CROP_MAX_X,
    OPENCV_CROP_MIN_Y,
    OPENCV_CROP_MAX_Y,
    OPENCV_MOTION_PIXELS,
    OPENCV_MOTION_DEVIATION
};

#define MO_OPENCV_SYTEM_LABELNAME	0
#define MO_OPENCV_LIVE_SYSTEM	1
#define MO_OPENCV_SYSTEM_ON 2

/**
"Face,Gpu Motion,Blobs,Color"
*/
enum moOpenCVRecognitionMode {
  OPENCV_RECOGNITION_MODE_UNDEFINED=-1,
  OPENCV_RECOGNITION_MODE_FACE=0,
  OPENCV_RECOGNITION_MODE_GPU_MOTION=1,
  OPENCV_RECOGNITION_MODE_BLOBS=2,
  OPENCV_RECOGNITION_MODE_COLOR=3,
  OPENCV_RECOGNITION_MODE_MOTION=4
};

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

    void MotionRecognition();
    void GpuMotionRecognition();
    void FaceDetection();
    void BlobRecognition();
    void ColorRecognition();

    int detectMotion(const Mat & motion, Mat & result, Mat & result_cropped,
                 int x_start, int x_stop, int y_start, int y_stop,
                 int max_deviation,
                 Scalar & color);


    moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

    moTrackerSystemData*    m_pTrackerSystemData;
    moDataMessage*          m_pDataMessage;

private:

    moTexture* m_pSrcTexture;

    bool      m_bReInit;

    moTexture* m_pCVSourceTexture;
    moTexture* m_pCVResultTexture;
    moTexture* m_pCVResult2Texture;
    moTexture* m_pCVResult3Texture;

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

    moOpenCVRecognitionMode m_RecognitionMode;

    moConfig config;

    moEventList *events;

    IplImage* TextureToCvImage( moTexture* p_pTexture, moVector2i p_Resize = moVector2i( 0, 0  ) );
    GLuint CvMatToTexture( Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter, moTexture* p_destTexture=NULL );

    CascadeClassifier face_cascade;
    CascadeClassifier eye_cascade;

    moOutlet* m_MotionDetection; //0 o 1
    moOutlet* m_MotionDetectionX; //0 o 1
    moOutlet* m_MotionDetectionY;

    moOutlet* m_FacePositionX;
    moOutlet* m_FacePositionY;
    moOutlet* m_FaceSizeWidth;
    moOutlet* m_FaceSizeHeight;

    moOutlet* m_EyeLeftX;
    moOutlet* m_EyeLeftY;
    moOutlet* m_EyeLeftWidth;
    moOutlet* m_EyeLeftHeight;

    moOutlet* m_EyeRightX;
    moOutlet* m_EyeRightY;
    moOutlet* m_EyeRightWidth;
    moOutlet* m_EyeRightHeight;

    moOutlet* m_OutTracker;
    moOutlet* m_OutletDataMessage; //MESSAGE THAT CAN BE CONNECTED TO NetOscOut "DATAMESSAGE"

protected:
    int m_steps;
    int idopencvout;
    int m_reduce_width;
    int m_reduce_height;
    int m_threshold;
    int m_threshold_max;

    float m_crop_min_x;
    float m_crop_max_x;
    float m_crop_min_y;
    float m_crop_max_y;
    int m_motion_pixels;
    int m_motion_deviation;

    IplImage* m_pIplImage;
    MOubyte * m_pBuffer;

    /** MOTION DETECTION */
    Mat current_frame;
    Mat prev_frame;
    Mat next_frame;
    int number_of_changes, number_of_sequence;
    /** END MOTION DETECTION */

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
