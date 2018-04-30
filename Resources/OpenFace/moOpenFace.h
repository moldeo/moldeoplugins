/*******************************************************************************

                                moOpenFace.h

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
  moOpenFace

  Description:
  Motion Analyzer


*******************************************************************************/



#define HAVE_OPENGL 0

#include "moFileManager.h"

#include "cv.h"
//#include "highgui/highgui.hpp"

#define OPENCV2

#ifdef OPENCV2
/*
    #include "opencv2/legacy/legacy.hpp"

    #ifdef WIN32
        #include "opencv2/features2d/features2d.hpp"
        #include "opencv2/objdetect/objdetect.hpp"
        //#include "opencv2/highgui/highgui.hpp"
        #include "opencv2/imgproc/imgproc.hpp"
        #include <opencv2/opencv.hpp>
    #else
        #include "opencv2/features2d/features2d.hpp"
        #include "opencv2/objdetect/objdetect.hpp"
        #include "opencv2/imgproc/imgproc.hpp"
        #include <opencv2/opencv.hpp>


    #endif
*/

#endif // OPENCV2


// Libraries for landmark detection (includes CLNF and CLM modules)
#include "LandmarkCoreIncludes.h"
#include "GazeEstimation.h"

#include <fstream>
#include <sstream>

// OpenCV includes
//#include <opencv2/videoio/videoio.hpp>  // Video write
//#include <opencv2/videoio/videoio_c.h>  // Video write
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

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

#ifndef __MO_OPENFACE_H
#define __MO_OPENFACE_H

enum moOpenFaceParamIndex {
    OPENFACE_INLET=0,
    OPENFACE_OUTLET,
    OPENFACE_SCRIPT,
    OPENFACE_COLOR,
    OPENFACE_TEXTURE,
    OPENFACE_RECOGNITION_MODE,
    OPENFACE_REDUCE_WIDTH,
    OPENFACE_REDUCE_HEIGHT,
    OPENFACE_THRESHOLD,
    OPENFACE_THRESHOLD_MAX,
    OPENFACE_THRESHOLD_TYPE,
    OPENFACE_LINE_THICKNESS,
    OPENFACE_LINE_COLOR,
    OPENFACE_LINE_OFFSET_X,
    OPENFACE_LINE_OFFSET_Y,
    OPENFACE_LINE_STEPS,
    OPENFACE_CROP_MIN_X,
    OPENFACE_CROP_MAX_X,
    OPENFACE_CROP_MIN_Y,
    OPENFACE_CROP_MAX_Y,
    OPENFACE_MOTION_PIXELS,
    OPENFACE_MOTION_DEVIATION,
    OPENFACE_BLOB_MIN_AREA,
    OPENFACE_BLOB_MAX_AREA,
    OPENFACE_BLOB_MIN_DISTANCE,
    OPENFACE_TRAINING_IMAGES_FOLDER,

    /** SAVING IMAGES (FACES)*/
    OPENFACE_SAVING_IMAGES_FOLDER,
    OPENFACE_SAVING_IMAGES_MODE,
    OPENFACE_SAVING_IMAGES_TIME,
    OPENFACE_SAVING_IMAGES_SIZE_WIDTH,
    OPENFACE_SAVING_IMAGES_SIZE_HEIGHT,
    OPENFACE_DEBUG_ON,
};

enum moOpenFaceSavingImagesMode {
  OPENFACE_SAVING_IMAGES_MODE_TIME=0, /** Same face stand for certain time so it is remembered, see: OPENFACE_SAVING_IMAGES_TIME for values */
  OPENFACE_SAVING_IMAGES_MODE_STILL=1, /** */
  OPENFACE_SAVING_IMAGES_MODE_SMILE=2, /** */

};

#define MO_OPENFACE_SYTEM_LABELNAME	0
#define MO_OPENFACE_LIVE_SYSTEM	1
#define MO_OPENFACE_SYSTEM_ON 2

/** Recognition Mode
* 0: Face
* 1: Gpu Motion
* 2: Contour
* 3: Color
* 4: Motion
* 5: Blobs
* 6: Threshold
*/
enum moOpenFaceRecognitionMode {
  OPENFACE_RECOGNITION_MODE_UNDEFINED=-1,
  OPENFACE_RECOGNITION_MODE_FACE=0,
  OPENFACE_RECOGNITION_MODE_GPU_MOTION=1,
  OPENFACE_RECOGNITION_MODE_CONTOUR=2,
  OPENFACE_RECOGNITION_MODE_COLOR=3,
  OPENFACE_RECOGNITION_MODE_MOTION=4,
  OPENFACE_RECOGNITION_MODE_BLOBS=5,
  OPENFACE_RECOGNITION_MODE_THRESHOLD=6,
  OPENFACE_RECOGNITION_MODE_BODY=7,
  OPENFACE_RECOGNITION_MODE_FACERECOGNITION_MEM=8,
  OPENFACE_RECOGNITION_MODE_FACERECOGNITION_REM=9,
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
enum moOpenFaceThresholdType {
  OPENFACE_THRESHOLD_TYPE_UNDEFINED=-1,
  OPENFACE_THRESHOLD_TYPE_BINARY=0,
  OPENFACE_THRESHOLD_TYPE_BINARY_INVERTED=1,
  OPENFACE_THRESHOLD_TYPE_TRUNCATED=2,
  OPENFACE_THRESHOLD_TYPE_TO_ZERO=3,
  OPENFACE_THRESHOLD_TYPE_TO_ZERO_INVERTED=4,
  OPENFACE_THRESHOLD_TYPE_MAX=4
};

#define w 500

class moOpenFaceSystem : public moAbstract {

	public:

		moOpenFaceSystem();
		virtual ~moOpenFaceSystem();

};


typedef moOpenFaceSystem* moOpenFaceSystemPtr;
/*
template class LIBMOLDEO_API moDynamicArray<moOpenFaceSystemPtr>;
typedef  moDynamicArray<moOpenFaceSystemPtr> moOpenFaceSystems;
*/
moDeclareDynamicArray( moOpenFaceSystemPtr, moOpenFaceSystems );

moDeclareDynamicArray( IplImage*, moOpenFaceImages );


class moOpenFace : public moResource
{
public:
    moOpenFace();
    virtual ~moOpenFace();

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

    double fps_tracker = -1.0;
    int64 t0 = 0;
    int64 frame_count = 0;
    // For subpixel accuracy drawing
    const int draw_shiftbits = 4;
    const int draw_multiplier = 1 << 4;

    void visualise_tracking(cv::Mat& captured_image,
        cv::Mat_<float>& depth_image,
        const LandmarkDetector::CLNF& face_model,
        const LandmarkDetector::FaceModelParameters& det_parameters,
        cv::Point3f gazeDirection0,
        cv::Point3f gazeDirection1,
        int frame_count,
        double fx,
        double fy,
        double cx,
        double cy);

    moConfigDefinition * GetDefinition( moConfigDefinition *p_configdefinition );

    moTrackerSystemData*    m_pTrackerSystemData;
    moDataMessage*          m_pDataMessage;
    moDataMessage*          m_pDataVectorMessage;
    moDataMessages          m_DataMessages;

private:

    moTexture* m_pSrcTexture;

    bool      m_bReInit;

    moVector2d vecEyeBrowLeft;
    moVector2d vecOpenJaw;

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

    moOpenFaceRecognitionMode m_RecognitionMode;

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

    moOutlet* m_FaceDetection;
    moOutlet* m_FaceDetectionCertainty;

    moOutlet* m_GazeDirectionL_X;
    moOutlet* m_GazeDirectionL_Y;
    moOutlet* m_GazeDirectionL_Z;

    moOutlet* m_GazeDirectionR_X;
    moOutlet* m_GazeDirectionR_Y;
    moOutlet* m_GazeDirectionR_Z;

    moOutlet* m_FacePositionX;
    moOutlet* m_FacePositionY;
    moOutlet* m_FaceSizeWidth;
    moOutlet* m_FaceSizeHeight;

    moOutlet* m_FacePositionZ;
    moOutlet* m_FaceSizeDepth;

    moOutlet* m_FaceAngleX;
    moOutlet* m_FaceAngleY;
    moOutlet* m_FaceAngleZ;


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
    moOutlet* m_OutletDataVectorMessage;

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
    moOpenFaceThresholdType m_threshold_type;

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
    /** */

    int levels;
    //CvSeq* contours;
    CvMemStorage* storage;

    moOpenFaceSystems		m_OpenFaceSystems;

    vector<Vec4i> hierarchy;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<KeyPoint> keypoints;


    //Ptr<FaceRecognizer> m_pFaceRecognizer;
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


    /** OpenFace
    */

    LandmarkDetector::FaceModelParameters det_parameters;
    LandmarkDetector::CLNF*  p_clnf_model;

};


class moOpenFaceFactory : public moResourceFactory {

public:
    moOpenFaceFactory() {}
    virtual ~moOpenFaceFactory() {}
    moResource* Create();
    void Destroy(moResource* fx);
};


extern "C"
{
MO_PLG_API moResourceFactory* CreateResourceFactory();
MO_PLG_API void DestroyResourceFactory();
}


#endif
