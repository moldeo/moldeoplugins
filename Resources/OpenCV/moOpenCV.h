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



#define HAVE_OPENGL 0

#include "moFileManager.h"

#include "cv.h"
//#include "highgui/highgui.hpp"

#define OPENCV2

#ifdef OPENCV2

    //#include "opencv2/legacy/legacy.hpp"

    #ifdef WIN32
        #include "opencv2/features2d/features2d.hpp"
        #include "opencv2/objdetect/objdetect.hpp"
        //#include "opencv2/highgui/highgui.hpp"
        #include "opencv2/imgproc/imgproc.hpp"
        #include <opencv2/opencv.hpp>
    #else


//#include "opencv2/core.hpp"
//#include "opencv2/face.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui/highgui.hpp"
        #include "opencv2/features2d/features2d.hpp"
        #include "opencv2/objdetect/objdetect.hpp"
        #include "opencv2/imgproc/imgproc.hpp"
        #include <opencv2/opencv.hpp>


    #endif


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
#ifdef MO_LINUX
    using namespace cv::face;
#endif

#ifndef __MO_OPENCV_H
#define __MO_OPENCV_H

enum moOpenCVParamIndex {
    OPENCV_INLET=0,
    OPENCV_OUTLET,
    OPENCV_SCRIPT,
    OPENCV_COLOR,
    OPENCV_TEXTURE,
    OPENCV_RECOGNITION_MODE,
    OPENCV_REDUCE_WIDTH,
    OPENCV_REDUCE_HEIGHT,
    OPENCV_THRESHOLD,
    OPENCV_THRESHOLD_MAX,
    OPENCV_THRESHOLD_TYPE,
    OPENCV_LINE_THICKNESS,
    OPENCV_LINE_COLOR,
    OPENCV_LINE_OFFSET_X,
    OPENCV_LINE_OFFSET_Y,
    OPENCV_LINE_STEPS,
    OPENCV_CROP_MIN_X,
    OPENCV_CROP_MAX_X,
    OPENCV_CROP_MIN_Y,
    OPENCV_CROP_MAX_Y,
    OPENCV_MOTION_PIXELS,
    OPENCV_MOTION_DEVIATION,
    OPENCV_BLOB_MIN_AREA,
    OPENCV_BLOB_MAX_AREA,
    OPENCV_BLOB_MIN_DISTANCE,
    OPENCV_TRAINING_IMAGES_FOLDER,

    /** SAVING IMAGES (FACES)*/
    OPENCV_SAVING_IMAGES_FOLDER,
    OPENCV_SAVING_IMAGES_MODE,
    OPENCV_SAVING_IMAGES_TIME,
    OPENCV_SAVING_IMAGES_SIZE_WIDTH,
    OPENCV_SAVING_IMAGES_SIZE_HEIGHT,
    OPENCV_ECHO_RESULT,
    OPENCV_DEBUG_ON,
};

enum moOpenCVSavingImagesMode {
  OPENCV_SAVING_IMAGES_MODE_TIME=0, /** Same face stand for certain time so it is remembered, see: OPENCV_SAVING_IMAGES_TIME for values */
  OPENCV_SAVING_IMAGES_MODE_STILL=1, /** */
  OPENCV_SAVING_IMAGES_MODE_SMILE=2, /** */

};

#define MO_OPENCV_SYTEM_LABELNAME	0
#define MO_OPENCV_LIVE_SYSTEM	1
#define MO_OPENCV_SYSTEM_ON 2

/** Recognition Mode
* 0: Face
* 1: Gpu Motion
* 2: Contour
* 3: Color
* 4: Motion
* 5: Blobs
* 6: Threshold
*/
enum moOpenCVRecognitionMode {
  OPENCV_RECOGNITION_MODE_UNDEFINED=-1,
  OPENCV_RECOGNITION_MODE_FACE=0,
  OPENCV_RECOGNITION_MODE_GPU_MOTION=1,
  OPENCV_RECOGNITION_MODE_CONTOUR=2,
  OPENCV_RECOGNITION_MODE_COLOR=3,
  OPENCV_RECOGNITION_MODE_MOTION=4,
  OPENCV_RECOGNITION_MODE_BLOBS=5,
  OPENCV_RECOGNITION_MODE_THRESHOLD=6,
  OPENCV_RECOGNITION_MODE_BODY=7,
  OPENCV_RECOGNITION_MODE_FACERECOGNITION_MEM=8,
  OPENCV_RECOGNITION_MODE_FACERECOGNITION_REM=9,
};

/**
* 0: \if spanish Binario \else Binary \endif
* 1: \if spanish Binario invertido \else Binary Inverted \endif
* 2: \if spanish Umbral truncado \else Threshold Truncated \endif
* 3: \if spanish Truncado a cero \else TThreshold to Zero \endif
* 4: \if spanish Truncado a cero invertido \else TThreshold to Zero Inverted \endif
*
* \link http://docs.opencv.org/2.4/doc/tutorials/imgproc/threshold/threshold.html
*/
enum moOpenCVThresholdType {
  OPENCV_THRESHOLD_TYPE_UNDEFINED=-1,
  OPENCV_THRESHOLD_TYPE_BINARY=0,
  OPENCV_THRESHOLD_TYPE_BINARY_INVERTED=1,
  OPENCV_THRESHOLD_TYPE_TRUNCATED=2,
  OPENCV_THRESHOLD_TYPE_TO_ZERO=3,
  OPENCV_THRESHOLD_TYPE_TO_ZERO_INVERTED=4,
  OPENCV_THRESHOLD_TYPE_MAX=4
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

moDeclareDynamicArray( IplImage*, moOpenCVImages );


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

    void BodyDetection();
    void FaceDetection();
    void FaceRecognition();

    void BlobRecognition();
    void ColorRecognition();
    void ContourRecognition();
    void ThresholdFilter();
    void ThresholdRecognition();

    int detectMotion(const Mat & motion, Mat & result, Mat & result_cropped,
                 int x_start, int x_stop, int y_start, int y_stop,
                 int max_deviation,
                 Scalar & color);


    moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

    moTrackerSystemData*    m_pTrackerSystemData;
    moDataMessage*          m_pDataMessage;
    moDataMessages          m_DataMessages;

private:

    moTexture* m_pSrcTexture;

    bool      m_bReInit;

    moTexture* m_pCVSourceTexture;
    moTexture* m_pCVResultTexture;
    moTexture* m_pCVResult2Texture;
    moTexture* m_pCVResult3Texture;
    moTexture* m_pCVBlobs;
    moTexture* m_pCVThresh;

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

    CascadeClassifier body_cascade;
    CascadeClassifier face_cascade;
    CascadeClassifier eye_cascade;
    HOGDescriptor hog_cascade;

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

    moOutlet* m_Blob1X;
    moOutlet* m_Blob1Y;
    moOutlet* m_Blob1Size;
    moOutlet* m_Blob1Vx;
    moOutlet* m_Blob1Vy;

    moOutlet* m_Blob2X;
    moOutlet* m_Blob2Y;
    moOutlet* m_Blob2Size;
    moOutlet* m_Blob2Vx;
    moOutlet* m_Blob2Vy;

    moOutlet* m_Blob3X;
    moOutlet* m_Blob3Y;
    moOutlet* m_Blob3Size;
    moOutlet* m_Blob3Vx;
    moOutlet* m_Blob3Vy;

    moOutlet* m_Blob4X;
    moOutlet* m_Blob4Y;
    moOutlet* m_Blob4Size;
    moOutlet* m_Blob4Vx;
    moOutlet* m_Blob4Vy;


    moInlet*  m_pContourIndex;
    moInlet*  m_pLineIndex;
    moInlet*  m_pBlobIndex;
    moInlet*  m_pObjectIndex;
    moInlet*  m_pFaceIndex;

protected:
    int m_steps;
    int idopencvout;
    int m_reduce_width;
    int m_reduce_height;
    int m_threshold;
    int m_threshold_max;
    double m_echo_result;
    moOpenCVThresholdType m_threshold_type;

    int m_debug_on;
    std::vector<Rect> faces;


    float m_line_thickness;
    float m_line_offset_x;
    float m_line_offset_y;
    moVector4d m_line_color;
    float m_line_steps;

    float m_crop_min_x;
    float m_crop_max_x;
    float m_crop_min_y;
    float m_crop_max_y;
    int m_motion_pixels;
    int m_motion_deviation;

    float m_blob_min_area;
    float m_blob_max_area;
    float m_blob_min_distance;

    IplImage* m_pIplImage;
    MOubyte * m_pBuffer;
    int       m_BufferSize;

    /** MOTION DETECTION */
    Mat current_frame;
    Mat prev_frame;
    Mat next_frame;
    int number_of_changes, number_of_sequence;
    /** END MOTION DETECTION */

    /** THRESHOLD */
    Mat dstthresh;
    /** BLENDING */
    Mat dstblending;

    int levels;
    //CvSeq* contours;
    CvMemStorage* storage;

    moOpenCVSystems		m_OpenCVSystems;

    vector<Vec4i> hierarchy;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<KeyPoint> keypoints;


    Ptr<FaceRecognizer> m_pFaceRecognizer;
    vector<Mat> images;
    vector<int> labels;
    vector<string> names;
    bool isTrained;

    cv::Mat processImage(cv::Mat& image);
    bool processAndSaveImage(cv::Mat& image, const std::string& name);
    typedef std::map<std::string, std::string> StringMap;


    moTimerAbsolute m_FaceTimer;

    moText m_trainingImagesPath;
    moText m_savingImagesPath;
    int  m_savingImagesMode;
    int  m_savingImagesSizeWidth;
    int  m_savingImagesSizeHeight;
    float m_savingImagesTime;

		cv::Size m_goalSize;
		bool m_keepAspectRatio;

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
