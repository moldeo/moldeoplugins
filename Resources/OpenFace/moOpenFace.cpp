/*******************************************************************************

                              moOpenFace.cpp

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

#include "moOpenFace.h"

#include "moArray.h"

moDefineDynamicArray( moOpenFaceSystems )


/*********************

  NOTAS:

  el tracker podr� funcionar como un thread que vaya calculando en funcion de q va llegando la info,
  o mejor aun, que trate de calcular, y cuando llega a un resultado el efecto en cuestion tome ese valor.

  //para que el tracker funcione sin shaders, debemos hacer el calculo antes de que se pase la informacion a la textura,
  para ello quizas no sea conveniente trabajar con threads, sino bloquear la ejecucion hasta terminar de tomar los valores q nos interesan.




***********************/


//========================
//  Factory
//========================

moOpenFaceFactory *m_OpenFaceFactory = NULL;

MO_PLG_API moResourceFactory* CreateResourceFactory(){
	if (m_OpenFaceFactory==NULL)
		m_OpenFaceFactory = new moOpenFaceFactory();
	return (moResourceFactory*) m_OpenFaceFactory;
}

MO_PLG_API void DestroyResourceFactory(){
	delete m_OpenFaceFactory;
	m_OpenFaceFactory = NULL;
}

moResource*  moOpenFaceFactory::Create() {
	return new moOpenFace();
}

void moOpenFaceFactory::Destroy(moResource* fx) {
	delete fx;
}



//===========================================
//
//     Class: moOpenFace
//
//===========================================


moOpenFace::moOpenFace() {
	SetName(moText("openface"));
	m_pCVSourceTexture = NULL;
	m_pCVResultTexture = NULL;
	m_pCVResult2Texture = NULL;
	m_pCVResult3Texture = NULL;
	m_pCVBlobs = NULL;
	m_pCVThresh = NULL;

  m_OutletDataMessage = NULL;
  m_OutletDataVectorMessage = NULL;
  m_pDataMessage = NULL;
  m_pDataVectorMessage = NULL;

  m_Blob1X = NULL;
  m_Blob1Y = NULL;
  m_Blob1Size = NULL;
  m_Blob1Vx = NULL;
  m_Blob1Vy = NULL;

  m_Blob2X = NULL;
  m_Blob2Y = NULL;
  m_Blob2Size = NULL;
  m_Blob2Vx = NULL;
  m_Blob2Vy = NULL;

  m_Blob3X = NULL;
  m_Blob3Y = NULL;
  m_Blob3Size = NULL;
  m_Blob3Vx = NULL;
  m_Blob3Vy = NULL;

  m_Blob4X = NULL;
  m_Blob4Y = NULL;
  m_Blob4Size = NULL;
  m_Blob4Vx = NULL;
  m_Blob4Vy = NULL;

  p_clnf_model = NULL;

  m_keepAspectRatio = true;

}

moOpenFace::~moOpenFace() {
    Finish();
}

moConfigDefinition * moOpenFace::GetDefinition( moConfigDefinition *p_configdefinition ) {

	//default: alpha, color, syncro
	p_configdefinition = moMoldeoObject::GetDefinition( p_configdefinition );
	p_configdefinition->Add( moText("texture"), MO_PARAM_TEXTURE, OPENFACE_TEXTURE, moValue( "default", "TXT") );
	p_configdefinition->Add( moText("color"), MO_PARAM_COLOR, OPENFACE_COLOR );
/*
enum moOpenFaceRecognitionMode {
  OPENFACE_RECOGNITION_MODE_UNDEFINED=-1,
  OPENFACE_RECOGNITION_MODE_FACE=0,
  OPENFACE_RECOGNITION_MODE_GPU_MOTION=1,
  OPENFACE_RECOGNITION_MODE_CONTOUR=2,
  OPENFACE_RECOGNITION_MODE_COLOR=3,
  OPENFACE_RECOGNITION_MODE_MOTION=4,
  OPENFACE_RECOGNITION_MODE_BLOBS=5,
  OPENFACE_RECOGNITION_MODE_THRESHOLD=6
};*/
	p_configdefinition->Add( moText("recognition_mode"), MO_PARAM_NUMERIC, OPENFACE_RECOGNITION_MODE, moValue( "0", "NUM"),
                         moText("Face,Gpu Motion,Contour,Color,Motion Detection,Blobs,Threshold,Body,Face Recognition Memorize,Face Recognition Remember") );
	p_configdefinition->Add( moText("reduce_width"), MO_PARAM_NUMERIC, OPENFACE_REDUCE_WIDTH, moValue( "64","INT").Ref() );
	p_configdefinition->Add( moText("reduce_height"), MO_PARAM_NUMERIC, OPENFACE_REDUCE_HEIGHT, moValue( "64","INT").Ref() );
	p_configdefinition->Add( moText("threshold"), MO_PARAM_FUNCTION, OPENFACE_THRESHOLD, moValue( "50", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("threshold_max"), MO_PARAM_FUNCTION, OPENFACE_THRESHOLD_MAX, moValue( "255", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("threshold_type"), MO_PARAM_NUMERIC, OPENFACE_THRESHOLD_TYPE, moValue( "0", "NUM"), moText("Binary,Binary Inverted,Threshold Truncated,To Zero,To Zero Inverted") );

	p_configdefinition->Add( moText("line_thickness"), MO_PARAM_FUNCTION, OPENFACE_LINE_THICKNESS, moValue( "2", "FUNCTION").Ref() );
  p_configdefinition->Add( moText("line_color"), MO_PARAM_COLOR, OPENFACE_LINE_COLOR );
	p_configdefinition->Add( moText("line_offset_x"), MO_PARAM_FUNCTION, OPENFACE_LINE_OFFSET_X, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("line_offset_y"), MO_PARAM_FUNCTION, OPENFACE_LINE_OFFSET_Y, moValue( "0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("line_steps"), MO_PARAM_FUNCTION, OPENFACE_LINE_STEPS, moValue( "0", "FUNCTION").Ref() );


	p_configdefinition->Add( moText("crop_min_x"), MO_PARAM_FUNCTION, OPENFACE_CROP_MIN_X, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("crop_max_x"), MO_PARAM_FUNCTION, OPENFACE_CROP_MAX_X, moValue( "1.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("crop_min_y"), MO_PARAM_FUNCTION, OPENFACE_CROP_MIN_Y, moValue( "0.0", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("crop_max_y"), MO_PARAM_FUNCTION, OPENFACE_CROP_MAX_Y, moValue( "1.0", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("motion_pixels"), MO_PARAM_NUMERIC, OPENFACE_MOTION_PIXELS, moValue( "5", "INT").Ref() );
	p_configdefinition->Add( moText("motion_deviation"), MO_PARAM_NUMERIC, OPENFACE_MOTION_DEVIATION, moValue( "20", "INT").Ref() );

	p_configdefinition->Add( moText("blob_min_area"), MO_PARAM_FUNCTION, OPENFACE_BLOB_MIN_AREA, moValue( "1000", "FUNCTION").Ref() );
	p_configdefinition->Add( moText("blob_max_area"), MO_PARAM_FUNCTION, OPENFACE_BLOB_MAX_AREA, moValue( "50000", "FUNCTION").Ref() );

	p_configdefinition->Add( moText("blob_min_distance"), MO_PARAM_FUNCTION, OPENFACE_BLOB_MIN_DISTANCE, moValue( "10", "FUNCTION").Ref() );

  p_configdefinition->Add( moText("training_images_folder"), MO_PARAM_TEXT, OPENFACE_TRAINING_IMAGES_FOLDER );
  p_configdefinition->Add( moText("saving_images_folder"), MO_PARAM_TEXT, OPENFACE_SAVING_IMAGES_FOLDER );

  p_configdefinition->Add( moText("saving_images_mode"), MO_PARAM_NUMERIC, OPENFACE_SAVING_IMAGES_MODE, moValue( "0", "NUM"),
                         moText("Time,Still,Smile") );
  p_configdefinition->Add( moText("saving_images_time"), MO_PARAM_FUNCTION, OPENFACE_SAVING_IMAGES_TIME, moValue( "5000", "FUNCTION").Ref());
  p_configdefinition->Add( moText("saving_images_size_width"), MO_PARAM_FUNCTION, OPENFACE_SAVING_IMAGES_SIZE_WIDTH, moValue( "30", "FUNCTION").Ref());
  p_configdefinition->Add( moText("saving_images_size_height"), MO_PARAM_FUNCTION, OPENFACE_SAVING_IMAGES_SIZE_HEIGHT, moValue( "30", "FUNCTION").Ref());

	p_configdefinition->Add( moText("debug_on"), MO_PARAM_NUMERIC, OPENFACE_DEBUG_ON, moValue( "0", "NUM").Ref(), moText("Off,On,Full") );


	return p_configdefinition;

}

MOboolean moOpenFace::Init() {

  moText configname;
  MOint nvalues;
  MOint trackersystems;

  if ( GetConfigName().Length()==0 ) return false;

  if (!moResource::Init()) return false;

  moDefineParamIndex( OPENFACE_TEXTURE, "texture" );
  moDefineParamIndex( OPENFACE_COLOR, "color" );
  moDefineParamIndex( OPENFACE_RECOGNITION_MODE, "recognition_mode" );
  moDefineParamIndex( OPENFACE_REDUCE_WIDTH, moText("reduce_width") );
  moDefineParamIndex( OPENFACE_REDUCE_HEIGHT, moText("reduce_height") );
  moDefineParamIndex( OPENFACE_THRESHOLD, moText("threshold") );
  moDefineParamIndex( OPENFACE_THRESHOLD_MAX, moText("threshold_max") );
  moDefineParamIndex( OPENFACE_THRESHOLD_TYPE, moText("threshold_type") );

  moDefineParamIndex( OPENFACE_LINE_THICKNESS, moText("line_thickness") );
  moDefineParamIndex( OPENFACE_LINE_COLOR, moText("line_color") );
  moDefineParamIndex( OPENFACE_LINE_OFFSET_X, moText("line_offset_x") );
  moDefineParamIndex( OPENFACE_LINE_OFFSET_Y, moText("line_offset_y") );
  moDefineParamIndex( OPENFACE_LINE_STEPS, moText("line_steps") );


  moDefineParamIndex( OPENFACE_CROP_MIN_X, "crop_min_x" );
  moDefineParamIndex( OPENFACE_CROP_MAX_X, "crop_max_x" );
  moDefineParamIndex( OPENFACE_CROP_MIN_Y, "crop_min_y" );
  moDefineParamIndex( OPENFACE_CROP_MAX_Y, "crop_max_y" );

  moDefineParamIndex( OPENFACE_MOTION_PIXELS, "motion_pixels" );
  moDefineParamIndex( OPENFACE_MOTION_DEVIATION, "motion_deviation" );

  moDefineParamIndex( OPENFACE_BLOB_MIN_AREA, "blob_min_area" );
  moDefineParamIndex( OPENFACE_BLOB_MAX_AREA, "blob_max_area" );

  moDefineParamIndex( OPENFACE_BLOB_MIN_DISTANCE, "blob_min_distance" );
  moDefineParamIndex( OPENFACE_TRAINING_IMAGES_FOLDER, "training_images_folder" );
  moDefineParamIndex( OPENFACE_SAVING_IMAGES_FOLDER, "saving_images_folder" );
  moDefineParamIndex( OPENFACE_SAVING_IMAGES_MODE, "saving_images_mode" );
  moDefineParamIndex( OPENFACE_SAVING_IMAGES_TIME, "saving_images_time" );
  moDefineParamIndex( OPENFACE_SAVING_IMAGES_SIZE_WIDTH, "saving_images_size_width" );
  moDefineParamIndex( OPENFACE_SAVING_IMAGES_SIZE_HEIGHT, "saving_images_size_height" );
  moDefineParamIndex( OPENFACE_DEBUG_ON, "debug_on" );

/*
	img = cvCreateImage( cvSize(w,w), 8, 1 );
	cvZero( img );

    levels = 0;
    threshold = 150;
    threshold_max = 3;
    contours = NULL;

    storage = cvCreateMemStorage(0);


    idopencvout = -1;


    //contours = cvApproxPoly( contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 3, 1 );
*/

    m_pBuffer = NULL;
    m_BufferSize = 0;
    m_pIplImage = NULL;

    m_RecognitionMode = OPENFACE_RECOGNITION_MODE_UNDEFINED;
    m_bReInit = true;

    m_pSrcTexture = NULL;
    m_pDest0Texture = NULL;
    m_pDest1Texture = NULL;
    m_pDest2Texture = NULL;
    m_pDestDiff1Texture = NULL;
    m_pDestDiff2Texture = NULL;

    m_pTFDest0Texture = NULL;
    m_pTFDest1Texture = NULL;
    m_pTFDest2Texture = NULL;
    m_pTFDestDiff1Texture = NULL;
    m_pTFDestDiff2Texture = NULL;

    m_pTrackerSystemData = NULL;
    m_pBucketDiff1 = NULL;
    m_pBucketDiff2 = NULL;

    switch_texture = -1;

    m_FacePositionX = NULL;
    m_FacePositionY = NULL;
    m_FaceSizeWidth = NULL;
    m_FaceSizeHeight = NULL;

    m_FaceDetection = NULL;
    m_FaceDetectionCertainty = NULL;

    m_GazeDirectionL_X = NULL;
    m_GazeDirectionL_Y = NULL;
    m_GazeDirectionL_Z = NULL;

    m_GazeDirectionR_X = NULL;
    m_GazeDirectionR_Y = NULL;
    m_GazeDirectionR_Z = NULL;

    m_FacePositionZ = NULL;
    m_FaceSizeDepth = NULL;

    m_FaceAngleX = NULL;
    m_FaceAngleY = NULL;
    m_FaceAngleZ = NULL;

    m_MotionDetection = NULL;
    m_MotionDetectionX = NULL;
    m_MotionDetectionY = NULL;

    m_EyeLeftX = NULL;
    m_EyeLeftY = NULL;
    m_EyeRightX = NULL;
    m_EyeRightY = NULL;

    m_OutTracker = NULL;
    m_OutletDataMessage = NULL;
    m_OutletDataVectorMessage = NULL;
    m_pDataMessage = NULL;
    m_pDataMessage = new moDataMessage();
    m_pDataVectorMessage = NULL;
    m_pDataVectorMessage = new moDataMessage();

    moTexParam tparam = MODefTex2DParams;
    //tparam.internal_format = GL_RGBA32F_ARB;
    tparam.internal_format = GL_RGB;
    int Mid = GetResourceManager()->GetTextureMan()->AddTexture( "CVSOURCE", 128, 128, tparam );
    if (Mid>0) {
        m_pCVSourceTexture = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
        m_pCVSourceTexture->BuildEmpty(128, 128);

        if (m_debug_on) MODebug2->Message("CVSOURCE texture created!!");
    } else {
        MODebug2->Error("Couldn't create texture: CVSOURCE");
    }

    Mid = GetResourceManager()->GetTextureMan()->AddTexture( "CVRESULT", 128, 128, tparam );
    if (Mid>0) {
        m_pCVResultTexture = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
        m_pCVResultTexture->BuildEmpty(128, 128);

        if (m_debug_on) MODebug2->Message("CVRESULT texture created!!");
    } else {
        MODebug2->Error("Couldn't create texture: CVRESULT");
    }

    Mid = GetResourceManager()->GetTextureMan()->AddTexture( "CVRESULT2", 128, 128, tparam );
    if (Mid>0) {
        m_pCVResult2Texture = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
        m_pCVResult2Texture->BuildEmpty(128, 128);

        if (m_debug_on) MODebug2->Message("CVRESULT2 texture created!!");
    } else {
        MODebug2->Error("Couldn't create texture: CVRESULT2");
    }

    Mid = GetResourceManager()->GetTextureMan()->AddTexture( "CVRESULT3", 128, 128, tparam );
    if (Mid>0) {
        m_pCVResult3Texture = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
        m_pCVResult3Texture->BuildEmpty(128, 128);

        if (m_debug_on) MODebug2->Message("CVRESULT3 texture created!!");
    } else {
        MODebug2->Error("Couldn't create texture: CVRESULT3");
    }

    Mid = GetResourceManager()->GetTextureMan()->AddTexture( "CVBLOBS", 512, 512, tparam );
    if (Mid>0) {
        m_pCVBlobs = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
        m_pCVBlobs->BuildEmpty(512, 512);

        if (m_debug_on) MODebug2->Message("CVBLOBS texture created!!");
    } else {
        MODebug2->Error("Couldn't create texture: CVBLOBS");
    }

    Mid = GetResourceManager()->GetTextureMan()->AddTexture( "CVTHRESH", 512, 512, tparam );
    if (Mid>0) {
        m_pCVThresh = GetResourceManager()->GetTextureMan()->GetTexture(Mid);
        m_pCVThresh->BuildEmpty(512, 512);

        if (m_debug_on) MODebug2->Message("CVTHRESH texture created!!");
    } else {
        MODebug2->Error("Couldn't create texture: CVTHRESH");
    }

  m_pContourIndex = new moInlet();

  if (m_pContourIndex) {
    ((moConnector*)m_pContourIndex)->Init( moText("contourindex"), m_Inlets.Count(), MO_DATA_NUMBER_LONG );
    m_Inlets.Add(m_pContourIndex);
  }

  m_pLineIndex = new moInlet();

  if (m_pLineIndex) {
    ((moConnector*)m_pLineIndex)->Init( moText("lineindex"), m_Inlets.Count(), MO_DATA_NUMBER_LONG );
    m_Inlets.Add(m_pLineIndex);
  }


    UpdateParameters();

	return true;

}

void moOpenFace::UpdateParameters() {

  moOpenFaceRecognitionMode mode = (moOpenFaceRecognitionMode) m_Config.Int( moR(OPENFACE_RECOGNITION_MODE) );

  if (m_RecognitionMode != mode ) {
    m_bReInit = true;
    m_RecognitionMode = mode;
  }

  m_debug_on = m_Config.Int(moR(OPENFACE_DEBUG_ON ));

  int reduce_width = m_Config.Int(moR(OPENFACE_REDUCE_WIDTH ));
  int reduce_height = m_Config.Int(moR(OPENFACE_REDUCE_HEIGHT ));
  if ( m_reduce_width!=reduce_width || m_reduce_height!=reduce_height ) m_bReInit = true;

  m_reduce_width = reduce_width;
  m_reduce_height = reduce_height;
  m_threshold = m_Config.Eval( moR(OPENFACE_THRESHOLD));
  m_threshold_max = m_Config.Eval( moR(OPENFACE_THRESHOLD_MAX));
  m_threshold_type = (moOpenFaceThresholdType) min( (int)OPENFACE_THRESHOLD_TYPE_MAX, m_Config.Int( moR(OPENFACE_THRESHOLD_TYPE)));
  m_crop_min_x =  m_Config.Eval( moR(OPENFACE_CROP_MIN_X));
  m_crop_max_x =  m_Config.Eval( moR(OPENFACE_CROP_MAX_X));
  m_crop_min_y =  m_Config.Eval( moR(OPENFACE_CROP_MIN_Y));
  m_crop_max_y =  m_Config.Eval( moR(OPENFACE_CROP_MAX_Y));

  m_line_thickness = fabs( m_Config.Eval( moR(OPENFACE_LINE_THICKNESS)) );
  m_line_offset_x = m_Config.Eval( moR(OPENFACE_LINE_OFFSET_X));
  m_line_offset_y = m_Config.Eval( moR(OPENFACE_LINE_OFFSET_Y));
  m_line_steps = m_Config.Eval( moR(OPENFACE_LINE_STEPS));
  m_line_color = m_Config.EvalColor(moR( OPENFACE_LINE_COLOR ));

  m_motion_pixels = m_Config.Eval( moR(OPENFACE_MOTION_PIXELS));
  m_motion_deviation = m_Config.Eval( moR(OPENFACE_MOTION_DEVIATION));

  m_blob_min_area = m_Config.Eval( moR(OPENFACE_BLOB_MIN_AREA));
  m_blob_max_area = m_Config.Eval( moR(OPENFACE_BLOB_MAX_AREA));
  m_blob_min_distance = m_Config.Eval( moR(OPENFACE_BLOB_MIN_DISTANCE));

  m_trainingImagesPath = m_pResourceManager->GetDataMan()->GetDataPath() + moSlash+m_Config.Text( moR(OPENFACE_TRAINING_IMAGES_FOLDER) );
  m_savingImagesPath = m_Config.Text( moR(OPENFACE_SAVING_IMAGES_FOLDER) );
  m_savingImagesMode = m_Config.Int( moR(OPENFACE_SAVING_IMAGES_MODE) );
  m_savingImagesTime = m_Config.Eval( moR(OPENFACE_SAVING_IMAGES_TIME) );
  m_savingImagesSizeWidth = m_Config.Eval( moR(OPENFACE_SAVING_IMAGES_SIZE_WIDTH) );
  m_savingImagesSizeHeight = m_Config.Eval( moR(OPENFACE_SAVING_IMAGES_SIZE_HEIGHT) );
  m_keepAspectRatio = true;
  m_goalSize = cvSize(m_savingImagesSizeWidth, m_savingImagesSizeHeight);

  moData* pTexData = m_Config[moR( OPENFACE_TEXTURE )].GetData();
  //moTexture rTexture = m_Config.Texture( moR( OPENFACE_TEXTURE );
  if (pTexData
      && pTexData->Texture()) {
        //MODebug2->Message( "moOpenFace::UpdateParameters() > " + pTexData->Texture()->GetName() );
  } else {
    MODebug2->Error("moOpenFace::UpdateParameters() > no  TexData !! Trying updating the connectors..." );
    moMoldeoObject::UpdateConnectors();
    pTexData = m_Config[moR( OPENFACE_TEXTURE )].GetData();
    if ( pTexData)
      if (pTexData->Texture())
        if (m_debug_on) MODebug2->Message( "moOpenFace::UpdateParameters() > " + pTexData->Texture()->GetName() );
  }

  moTexture* m_pRecTexture = NULL;

  if (m_pSrcTexture==NULL) {
      m_bReInit = true;
  }

  if (m_pDataMessage)
    m_pDataMessage->Empty();

  if (m_pDataVectorMessage)
    m_pDataVectorMessage->Empty();

	if (pTexData) {
    ///segun el modelo aplicamos...
    pTexData->GetGLId();

    switch(pTexData->Type()) {
        case MO_DATA_IMAGESAMPLE:
            m_pRecTexture = pTexData->Texture();
            break;
        case MO_DATA_IMAGESAMPLE_FILTERED:
            m_pRecTexture = pTexData->TextureDestination();
            break;
        default:
            m_pRecTexture = NULL;
            break;
    }
	} else {

	}

	if (m_pRecTexture) {
    if (m_pSrcTexture) {
      if (m_pRecTexture->GetName()!=m_pSrcTexture->GetName()) {
        if (m_debug_on) MODebug2->Message("moOpenFace::UpdateParameters() > m_bReInit TRUE! for " + m_pRecTexture->GetName());
        m_bReInit = true;
      }
    } else {
      if (m_debug_on) MODebug2->Message("moOpenFace::UpdateParameters() > set texture to " + m_pRecTexture->GetName());
    }

    m_pSrcTexture = m_pRecTexture;
	}

	if (m_pSrcTexture==NULL) {
      MODebug2->Error("moOpenFace::UpdateParameters() > texture is null ");
      return;
	}


  switch( m_RecognitionMode ) {
    case OPENFACE_RECOGNITION_MODE_FACE:
      {
      //int stepN = 5;
      //( m_steps<0 || m_steps>stepN )?  m_steps = 0 : m_steps++;
      //if (m_steps==stepN) { FaceDetection(); }
        FaceDetection();
      }
      break;
    case OPENFACE_RECOGNITION_MODE_FACERECOGNITION_REM:
    case OPENFACE_RECOGNITION_MODE_FACERECOGNITION_MEM:
      {
        int stepN = 5;
        ( m_steps<0 || m_steps>stepN )?  m_steps = 0 : m_steps++;
        if (m_steps==stepN) { FaceRecognition(); }
      }
      break;
    case OPENFACE_RECOGNITION_MODE_BODY:
      {
      int stepN = 5;
      ( m_steps<0 || m_steps>stepN )?  m_steps = 0 : m_steps++;
      if (m_steps==stepN) { BodyDetection(); }
      }
      break;
    case OPENFACE_RECOGNITION_MODE_GPU_MOTION:
      GpuMotionRecognition();
      break;
    case OPENFACE_RECOGNITION_MODE_CONTOUR:
      ContourRecognition();
      break;
    case OPENFACE_RECOGNITION_MODE_BLOBS:
      BlobRecognition();
      break;
    case OPENFACE_RECOGNITION_MODE_COLOR:
      ColorRecognition();
      break;
    case OPENFACE_RECOGNITION_MODE_MOTION:
      MotionRecognition();
      break;
    case OPENFACE_RECOGNITION_MODE_THRESHOLD:
      ThresholdRecognition();
      break;
    default:
      m_bReInit = false;
      break;

  }

  if (!m_OutletDataMessage) {
      m_OutletDataMessage = m_Outlets.GetRef( GetOutletIndex( moText("DATAMESSAGE") ) );
      //MODebug2->Message("moOpenFace::FaceDetection > outlet FACE_POSITION_X: "+IntToStr((int)m_FacePositionX));
  } else {
    //m_OutletDataMessage->GetData()->SetInt(  (int)(number_of_sequence>0) );
    if (m_pDataMessage) {
      m_OutletDataMessage->GetData()->SetMessage(m_pDataMessage);
      m_OutletDataMessage->Update(true);
    }

  }

  if (!m_OutletDataVectorMessage) {
      m_OutletDataVectorMessage = m_Outlets.GetRef( GetOutletIndex( moText("DATAVECTORMESSAGE") ) );
      //MODebug2->Message("moOpenFace::FaceDetection > outlet DATAVECTORMESSAGE: "+IntToStr((int)m_OutletDataVectorMessage));
  } else {
    //m_OutletDataMessage->GetData()->SetInt(  (int)(number_of_sequence>0) );
    if (m_pDataVectorMessage) {
      m_OutletDataVectorMessage->GetData()->SetMessage(m_pDataVectorMessage);
      m_OutletDataVectorMessage->Update(true);
    }

  }



}

#include "moDebugManager.h"
// Check if there is motion in the result matrix
// count the number of changes and return.
int moOpenFace::detectMotion(const Mat & motion, Mat & result, Mat & result_cropped,
                 int x_start, int x_stop, int y_start, int y_stop,
                 int max_deviation,
                 Scalar & color)
{
    Scalar mean, stddev;
    meanStdDev(motion, mean, stddev);
    // if not to much changes then the motion is real (neglect agressive snow, temporary sunlight)
    if(stddev[0] < max_deviation)
    {
        //MODebug2->Message("max_deviation ok");
        int number_of_changes = 0;
        int min_x = motion.cols, max_x = 0;
        int min_y = motion.rows, max_y = 0;
        // loop over image and detect changes
        for(int j = y_start; j < y_stop; j+=2){ // height
            for(int i = x_start; i < x_stop; i+=2){ // width
                // check if at pixel (j,i) intensity is equal to 255
                // this means that the pixel is different in the sequence
                // of images (prev_frame, current_frame, next_frame)
                int pi = static_cast<int>(motion.at<uchar>(j,i));
                if( pi == 255)
                {
                    number_of_changes++;
                    if(min_x>i) min_x = i;
                    if(max_x<i) max_x = i;
                    if(min_y>j) min_y = j;
                    if(max_y<j) max_y = j;
                }
                /*
                if (pi>100)
                MODebug2->Message( "i,j: (" + IntToStr(i)
                                  + "," + IntToStr(j)
                                  + "): " + IntToStr(pi)  );
                                  */
            }
        }
        if(number_of_changes){
            //check if not out of bounds
            if(min_x-10 > 0) min_x -= 10;
            if(min_y-10 > 0) min_y -= 10;
            if(max_x+10 < result.cols-1) max_x += 10;
            if(max_y+10 < result.rows-1) max_y += 10;
            // draw rectangle round the changed pixel
            Point x(min_x,min_y);
            Point y(max_x,max_y);
            Rect rect(x,y);
            Mat cropped = result(rect);
            cropped.copyTo(result_cropped);
            rectangle(result,rect,color,1);
        }
        return number_of_changes;
    }
    return 0;
}

void
moOpenFace::MotionRecognition() {

  Mat d1, d2, motion;
  Mat motioncolor;
  Scalar mean_, color(0,255,255); // yellow
  Mat kernel_ero = getStructuringElement(MORPH_RECT, Size(2,2));

  //MODebug2->Message("MotionRecognition");

  // Detect motion in window
  int x_start = (int)((float)m_reduce_width*m_crop_min_x), x_stop = (int)((float)m_reduce_width*m_crop_max_x);
  int y_start = (int)((float)m_reduce_height*m_crop_min_y), y_stop = (int)((float)m_reduce_height*m_crop_max_y);

  // If more than 'there_is_motion' pixels are changed, we say there is motion
  // and store an image on disk
  int there_is_motion = m_motion_pixels;

  // Maximum deviation of the image, the higher the value, the more motion is allowed
  int max_deviation = m_motion_deviation;

  if (m_pSrcTexture==NULL) {

      if (m_debug_on) MODebug2->Message("moOpenFace::MotionRecognition >> no src texture");

      return;
  }

  moVector2i resizer( m_reduce_width, m_reduce_height );
  IplImage* srcframe = TextureToCvImage( m_pSrcTexture,  resizer  );

  if (srcframe==NULL) {
    MODebug2->Error("Error TextureToCvImage() : " + m_pSrcTexture->GetName() );
    return;
  }

  //Mat frame( srcframe );
  Mat frame = cv::cvarrToMat(srcframe);
  Mat framecol;

  CvMatToTexture( frame, 0 , 0, 0, m_pCVSourceTexture );

  if (m_bReInit) {
      if (m_debug_on) MODebug2->Message("MotionRecognition INIT");
      number_of_sequence = 0;
      cvtColor(frame, next_frame, COLOR_BGR2GRAY);

      next_frame.copyTo(prev_frame);
      next_frame.copyTo(current_frame);
  }

  /**CONVERT AND ENHANCE TO GRAYSCALE*/
  current_frame.copyTo(prev_frame);
  next_frame.copyTo(current_frame);
  cvtColor(frame, next_frame, COLOR_BGR2GRAY);

  Mat result, result_cropped;
  result = next_frame;

  absdiff(prev_frame, next_frame, d1);
  absdiff(next_frame, current_frame, d2);

/*
  Mat motioncolor1;
  cvtColor( next_frame, motioncolor1, COLOR_GRAY2RGB);
  CvMatToTexture( motioncolor1, 0 , 0, 0, m_pCVResultTexture );

  Mat motioncolor2;
  cvtColor( current_frame, motioncolor2, COLOR_GRAY2RGB);
  CvMatToTexture( motioncolor2, 0 , 0, 0, m_pCVResult2Texture );

  Mat motioncolor3;
  cvtColor( prev_frame, motioncolor3, COLOR_GRAY2RGB);
  CvMatToTexture( motioncolor3, 0 , 0, 0, m_pCVResult3Texture );
*/
  bitwise_and(d1, d2, motion);
  threshold(motion, motion, 35, 255, CV_THRESH_BINARY);
  erode(motion, motion, kernel_ero);

  Mat motioncolor1;
  cvtColor( motion, motioncolor1, COLOR_GRAY2RGB);
  CvMatToTexture( motioncolor1, 0 , 0, 0, m_pCVResultTexture );

  number_of_changes = detectMotion(motion, result, result_cropped,  x_start, x_stop, y_start, y_stop, max_deviation, color);
  //MODebug2->Push( "number_of_changes: " + IntToStr(number_of_changes)
  //               + "there_is_motion: " +  IntToStr(there_is_motion) );

  // If a lot of changes happened, we assume something changed.
  if(number_of_changes>=there_is_motion)
  {
      if (m_debug_on) MODebug2->Message( "number_of_changes: " + IntToStr(number_of_changes)
                 + "there_is_motion: " +  IntToStr(there_is_motion) );
      number_of_sequence++;
      if(number_of_sequence>0){
          if (m_debug_on) MODebug2->Message("Motion Detected!");

          //saveImg(result,DIR,EXT,DIR_FORMAT.c_str(),FILE_FORMAT.c_str());
          //saveImg(result_cropped,DIR,EXT,DIR_FORMAT.c_str(),CROPPED_FILE_FORMAT.c_str());
      }


  }
  else
  {
      number_of_sequence = 0;
  }


  if (!m_MotionDetection) {
      m_MotionDetection = m_Outlets.GetRef( GetOutletIndex( moText("MOTION_DETECTION") ) );
      //MODebug2->Message("moOpenFace::FaceDetection > outlet FACE_POSITION_X: "+IntToStr((int)m_FacePositionX));
  } else {
    m_MotionDetection->GetData()->SetInt(  (int)(number_of_sequence>0) );
    m_MotionDetection->Update(true);
  }

  if (!m_MotionDetectionX) {
      m_MotionDetectionX = m_Outlets.GetRef( GetOutletIndex( moText("MOTION_DETECTION_X") ) );
      //MODebug2->Message("moOpenFace::FaceDetection > outlet FACE_POSITION_X: "+IntToStr((int)m_FacePositionX));
  } else {
    m_MotionDetectionX->GetData()->SetInt(  (int)(number_of_sequence>0) );
    m_MotionDetectionX->Update(true);
  }

  if (!m_MotionDetectionY) {
      m_MotionDetectionY = m_Outlets.GetRef( GetOutletIndex( moText("MOTION_DETECTION_Y") ) );
      //MODebug2->Message("moOpenFace::FaceDetection > outlet FACE_POSITION_X: "+IntToStr((int)m_FacePositionX));
  } else {
    m_MotionDetectionY->GetData()->SetInt(  (int)(number_of_sequence>0) );
    m_MotionDetectionY->Update(true);
  }


  //SENDING DATAMESSAGE
  if (m_pDataMessage) {
    m_pDataMessage->Empty();//EMPTY IN UpdateParameters()
    moData pData;

    pData.SetText( moText("opencv") );
    m_pDataMessage->Add(pData);

    pData.SetText( moText("MOTION_DETECTION") );
    m_pDataMessage->Add(pData);

    pData.SetInt( (int)(number_of_sequence>0) );
    m_pDataMessage->Add(pData);

    pData.SetText( moText("MOTION_DETECTION_X") );
    m_pDataMessage->Add(pData);

    pData.SetInt( (int)(number_of_sequence>0) );
    m_pDataMessage->Add(pData);

    pData.SetText( moText("MOTION_DETECTION_Y") );
    m_pDataMessage->Add(pData);

    pData.SetInt( (int)(number_of_sequence>0) );
    m_pDataMessage->Add(pData);
/*
    moText ccc = "";
    for( int c=0; c<m_pDataMessage->Count(); c++) {
      ccc = ccc + m_pDataMessage->Get(c).ToText();
    }
    //MODebug2->Push(ccc);
*/
  }

  m_bReInit = false;
}

void
moOpenFace::GpuMotionRecognition() {



  if (m_pSrcTexture==NULL) {

      if (m_debug_on) MODebug2->Error("moOpenFace::GpuMotionRecognition >> no src texture");

      return;
  }

  moText reduce_resolution = IntToStr(m_reduce_width)+moText("x")+IntToStr(m_reduce_height);

  if (m_bReInit) {
    if (m_debug_on) MODebug2->Message("moOpenFace::GpuMotionRecognition >> REINIT");

    m_pTFDest0Texture = NULL;
    m_pTFDest1Texture = NULL;
    m_pTFDest2Texture = NULL;

    m_pTFDestDiff1Texture = NULL;
    m_pTFDestDiff2Texture = NULL;

  }

  //crea textura que recibe frame0 desde el SrcTexture

  //crea textura que recibe frame1

  //crea textura que recibe frame_diff
  int idt = -1;

  ///creamos la textura de destino de la copia:
  /// y su shader asociado
  if (!m_pTFDest2Texture && m_pSrcTexture->GetWidth()>0 ) {
    moTextArray copy_filter_0;
    copy_filter_0.Add( m_pSrcTexture->GetName()+moText(" shaders/Copy.cfg res:"+reduce_resolution+" "+m_pSrcTexture->GetName()+"dest2") );
    int idx = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->LoadFilters( &copy_filter_0 );
    if (idx>-1) {
        m_pTFDest2Texture = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->Get(idx-1);
        if (m_debug_on) MODebug2->Message( moText("filter loaded m_pTFDest2Texture") );
    }
  }


  ///creamos la textura de destino de la copia del cuadro anterior
  /// y su shader asociado
  if (!m_pTFDest1Texture && m_pSrcTexture->GetWidth()>0 ) {
      moTextArray copy_filter_0;
      copy_filter_0.Add( moText("livein0dest2 shaders/Copy.cfg res:"+reduce_resolution+" "+m_pSrcTexture->GetName()+"dest1") );
      int idx = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->LoadFilters( &copy_filter_0 );
      if (idx>-1) {
          m_pTFDest1Texture = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->Get(idx-1);
          if (m_debug_on) MODebug2->Message( moText("filter loaded m_pTFDest1Texture") );
      }
  }


  ///creamos la textura de destino de la copia del cuadro anterior
  /// y su shader asociado
  if (!m_pTFDest0Texture && m_pSrcTexture->GetWidth()>0 ) {
      moTextArray copy_filter_0;
      copy_filter_0.Add( moText("livein0dest1 shaders/Copy.cfg res:"+reduce_resolution+" "+m_pSrcTexture->GetName()+"dest0") );
      int idx = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->LoadFilters( &copy_filter_0 );
      if (idx>-1) {
          m_pTFDest0Texture = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->Get(idx-1);
          if (m_debug_on) MODebug2->Message( moText("filter loaded m_pTFDest0Texture") );
      }
  }


  ///creamos la textura final, que contendra la diferencia entre el cuadro actual y el anterior..
  /// y su shader asociado
  if (!m_pTFDestDiff2Texture && m_pSrcTexture->GetWidth()>0 ) {
      moTextArray copy_filter_0;
      copy_filter_0.Add( moText("livein0dest1 livein0dest2 shaders/Diff.cfg res:"+reduce_resolution+" "+m_pSrcTexture->GetName()+"diff2") );
      int idx = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->LoadFilters( &copy_filter_0 );
      if (idx>-1) {
          m_pTFDestDiff2Texture = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->Get(idx-1);
          if (m_pTFDestDiff2Texture) {
              m_pDestDiff2Texture = m_pTFDestDiff2Texture->GetDestTex()->GetTexture(0);
              if (m_debug_on) MODebug2->Message( moText("filter loaded m_pTFDestDiff2Texture") );
          }
      }
  }

  ///creamos la textura final, que contendra la diferencia entre el cuadro anterior y el ante-ultimo..
  /// y su shader asociado
  if (!m_pTFDestDiff1Texture && m_pSrcTexture->GetWidth()>0 ) {
      moTextArray copy_filter_0;
      copy_filter_0.Add( moText("livein0dest0 livein0dest1 shaders/Diff.cfg res:"+reduce_resolution+" "+m_pSrcTexture->GetName()+"diff1") );
      int idx = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->LoadFilters( &copy_filter_0 );
      if (idx>-1) {
          m_pTFDestDiff1Texture = m_pResourceManager->GetShaderMan()->GetTextureFilterIndex()->Get(idx-1);
          if (m_pTFDestDiff1Texture) {
              m_pDestDiff1Texture = m_pTFDestDiff1Texture->GetDestTex()->GetTexture(0);
              if (m_debug_on) MODebug2->Message( moText("filter loaded m_pTFDestDiff1Texture") );
          }
      }
  }


/*

  pFBO->AddTexture(   160,
                      120,
                      m_pSrcTexture->GetTexParam(),
                      m_pSrcTexture->GetGLId(),
                      attach_point );

  m_pSrcTexture->SetFBO( pFBO );
  m_pSrcTexture->SetFBOAttachPoint( attach_point );
*/
  ///Guarda el frame ante-anterior....
  if (m_pTFDest0Texture) {
      m_pTFDest0Texture->Apply( (GLuint)0 );
  }


  ///Guarda el frame anterior....
  if (m_pTFDest1Texture) {
      m_pTFDest1Texture->Apply( (GLuint)0 );
  }


  ///Guarda el frame actual.... (esto lo hacemos ultimo para no perder los anteriores.
  if (m_pTFDest2Texture) {
      m_pTFDest2Texture->Apply( (GLuint)0 );
  }

  ///Calcular la diferencia del frame anterior con el anterior a este
  if (m_pTFDestDiff1Texture) {
      m_pTFDestDiff1Texture->Apply( (GLuint)0 );
  }

  ///Calcular la diferencia con el frame anterior
  if (m_pTFDestDiff2Texture) {
      m_pTFDestDiff2Texture->Apply( (GLuint)0 );
  }


  if (m_pDestDiff2Texture) {

      /// BAJAR A BUFFER Y PASAR A ARRAY DE TRACKING FEATURES.... (BLOBS CON OPENCV ?)

          if (!m_pBucketDiff2) {
              if (m_debug_on) MODebug2->Message("moOpenFace::UpdateParameters > moBucket Object created to receive TextureFilters Data.");
              m_pBucketDiff2 = new moBucket();
          }

          if (m_pBucketDiff2) {
              ///ATENCION!!! al hacer un BuildBucket, hay que hacer un EmptyBucket!!!
              //pBucket->BuildBucket( pTexSample->m_VideoFormat.m_BufferSize, 0);
              m_pBucketDiff2->BuildBucket( m_pDestDiff2Texture->GetWidth()*m_pDestDiff2Texture->GetHeight()*4, 0);
              m_pDestDiff2Texture->GetBuffer( (void*) m_pBucketDiff2->GetBuffer(), GL_RGBA, GL_UNSIGNED_BYTE );

              ///asignamos el bucket al videosample
              //pTexSample->m_pSampleBuffer = pBucket;
          }

          if (!m_pTrackerSystemData) {
              if (m_debug_on) MODebug2->Message("moOpenFace::UpdateParameters > creating moTrackerSystemData() Object.");
              m_pTrackerSystemData = new moTrackerSystemData();
              if (m_pTrackerSystemData) {
                  if (m_debug_on) MODebug2->Message( "moOpenFace::UpdateParameters > moTrackerSystemData() Object OK.");
                  if (m_debug_on) MODebug2->Message( moText("moOpenFace::UpdateParameters > m_pDestDiff2Texture->GetWidth().")
                                 + IntToStr( m_pDestDiff2Texture->GetWidth() )
                                 + moText(" m_pDestDiff2Texture->GetHeight().")
                                 + IntToStr( m_pDestDiff2Texture->GetHeight() )
                                  );

              }
          }

          if (m_pTrackerSystemData) {

              m_pTrackerSystemData->GetVideoFormat().m_Width = m_pDestDiff2Texture->GetWidth();
              m_pTrackerSystemData->GetVideoFormat().m_Height = m_pDestDiff2Texture->GetHeight();
              m_pTrackerSystemData->GetVideoFormat().m_BufferSize = m_pDestDiff2Texture->GetWidth()*m_pDestDiff2Texture->GetHeight();

              for(int i=0; i<m_pTrackerSystemData->GetFeatures().Count(); i++ ) {
                  if (m_pTrackerSystemData->GetFeatures().GetRef(i)!=NULL)
                  delete m_pTrackerSystemData->GetFeatures().GetRef(i);
              }

              ///RESET DATA !!!!
              m_pTrackerSystemData->GetFeatures().Empty();
              m_pTrackerSystemData->ResetMatrix();

              ///GET NEW DATA!!!!
              moTrackerFeature* TF = NULL;
              MOubyte* pBuf = NULL;

              if (m_pBucketDiff2)
                  pBuf = (MOubyte*) m_pBucketDiff2->GetBuffer();

              int validfeatures = 0;

              float sumX = 0.0f,sumY = 0.0f;
              float sumN = 0.0f;
              float varX = 0.0f, varY = 0.0f;
              float minX = 1.0f, minY = 1.0;
              float maxX = 0.0f, maxY = 0.0;

              float vel=0.0,acc=0.0,tor=0.0;
              float velAverage = 0.0, accAverage =0.0, torAverage=0.0;
              moVector2f velAverage_v(0,0);

              for(int j=0; j < m_pTrackerSystemData->GetVideoFormat().m_Height && pBuf; j++ ) {
                  for(int i=0; i < m_pTrackerSystemData->GetVideoFormat().m_Width && pBuf; i++ ) {
                      if (pBuf) {

                          int idx = i+j*m_pTrackerSystemData->GetVideoFormat().m_Width;

                          if ( 0<=idx && idx < m_pBucketDiff2->GetSize()  ) {
                              int r = pBuf[idx*4];
                              //int g = pBuf[idx*4+1];
                              //int b = pBuf[idx*4+2];
                              //int a = pBuf[idx*4+3];
                              if ( r > 0) {
                                  //crear el feature aqu�....
                                  TF = new moTrackerFeature();
                                  if (TF) {
                                      TF->x = (float) i / (float)m_pTrackerSystemData->GetVideoFormat().m_Width;
                                      TF->y =  (float) j / (float)m_pTrackerSystemData->GetVideoFormat().m_Height;
                                      TF->val = 0;
                                      TF->valid = 1;
                                      TF->tr_x = TF->x;
                                      TF->tr_y = TF->y;

                                      ///CALCULATE AVERAGE FOR BARYCENTER AND VARIANCE
                                      sumX+= TF->x;
                                      sumY+= TF->y;

                                      sumN+= 1.0f;

                                      if (sumN==1.0f) {
                                          /*
                                          MODebug2->Push( moText("moOpenFace::UpdateParameters > TF > TF->x:")
                                                      + FloatToStr(TF->x)
                                                      + moText(" TF->y:")
                                                      + FloatToStr(TF->y)
                                                     );
                                          */
                                      }

                                      ///maximos
                                      if (TF->x>maxX) maxX = TF->x;
                                      if (TF->y>maxY) maxY = TF->y;

                                      ///minimos
                                      if (TF->x<minX) minX = TF->x;
                                      if (TF->y<minY) minY = TF->y;

                                      ///esta es simplemente una matriz que cuenta la cantidad de....
                                      m_pTrackerSystemData->SetPositionMatrix( TF->x, TF->y, 1 );
                                      ///genera la matrix de referencia rapida por zonas
                                      ///m_pTrackerSystemData->SetPositionMatrix( TF );

                                      ///CALCULATE VELOCITY AND ACCELERATION
                                      TF->ap_x = TF->a_x;
                                      TF->ap_y = TF->a_x;
                                      TF->vp_x = TF->v_x;
                                      TF->vp_y = TF->v_x;
                                      TF->v_x = TF->x - TF->tr_x;
                                      TF->v_y = TF->y - TF->tr_y;
                                      TF->a_x = TF->v_x - TF->vp_x;
                                      TF->a_y = TF->v_y - TF->vp_y;
                                      TF->t_x = TF->a_x - TF->ap_x;
                                      TF->t_y = TF->a_y - TF->ap_y;

                                      vel = moVector2f( TF->v_x, TF->v_y ).Length();
                                      acc = moVector2f( TF->a_x, TF->a_y ).Length();
                                      tor = moVector2f( TF->t_x, TF->t_y ).Length();
                                      velAverage+= vel;
                                      accAverage+= acc;
                                      torAverage+= tor;
                                      velAverage_v+= moVector2f( fabs(TF->v_x), fabs(TF->v_y) );


                                      if ( vel >= 0.001 && vel <=0.05 ) m_pTrackerSystemData->SetMotionMatrix( TF->x, TF->y, 1 );
                                      if ( acc >= 0.001 ) m_pTrackerSystemData->SetAccelerationMatrix( TF->x, TF->y, 1 );

                                  }
                                  m_pTrackerSystemData->GetFeatures().Add(TF);
                              }
                          }
                      }
                  }
              }


              moVector2f previous_B = m_pTrackerSystemData->GetBarycenter();
              moVector2f previous_BM = m_pTrackerSystemData->GetBarycenterMotion();

              moVector2f BarPos;
              moVector2f BarMot;
              moVector2f BarAcc;

              m_pTrackerSystemData->SetBarycenter( 0, 0 );
              m_pTrackerSystemData->SetBarycenterMotion( 0, 0);
              m_pTrackerSystemData->SetBarycenterAcceleration( 0, 0 );

              m_pTrackerSystemData->SetMax( 0, 0 );
              m_pTrackerSystemData->SetMin( 0, 0 );

              m_pTrackerSystemData->SetDeltaValidFeatures( m_pTrackerSystemData->GetValidFeatures() - (int)sumN );
              m_pTrackerSystemData->SetValidFeatures( (int)sumN );

              if (sumN>=1.0f) {

                  BarPos = moVector2f( sumX/sumN, sumY/sumN);

                  BarMot = BarPos - previous_B;
                  BarAcc = BarMot - previous_BM;

                  m_pTrackerSystemData->SetBarycenter( BarPos.X(), BarPos.Y() );
                  m_pTrackerSystemData->SetBarycenterMotion( BarMot.X(), BarMot.Y() );
                  m_pTrackerSystemData->SetBarycenterAcceleration( BarAcc.X(), BarAcc.Y() );

                  velAverage = velAverage / (float)sumN;
                  accAverage = accAverage / (float)sumN;
                  torAverage = torAverage / (float)sumN;
                  velAverage_v = velAverage_v * 1.0f / (float)sumN;

                  m_pTrackerSystemData->SetAbsoluteSpeedAverage( velAverage );
                  m_pTrackerSystemData->SetAbsoluteAccelerationAverage( accAverage );
                  m_pTrackerSystemData->SetAbsoluteTorqueAverage( torAverage );

                  m_pTrackerSystemData->SetMax( maxX, maxY );
                  m_pTrackerSystemData->SetMin( minX, minY );

                  ///CALCULATE VARIANCE FOR EACH COMPONENT

                  moVector2f Bar = m_pTrackerSystemData->GetBarycenter();
                  for(int i=0; i<m_pTrackerSystemData->GetFeatures().Count(); i++ ) {
                      TF = m_pTrackerSystemData->GetFeatures().GetRef(i);
                      if (TF) {
                          if (TF->val>=0) {
                              varX+= moMathf::Sqr( TF->x - Bar.X() );
                              varY+= moMathf::Sqr( TF->y - Bar.Y() );
                          }
                      }
                  }
                  m_pTrackerSystemData->SetVariance( varX/sumN, varY/sumN );
                  //m_pTrackerSystemData->SetVariance( velAverage_v.X(), velAverage_v.Y() );

                  /*
                  MODebug2->Push( moText("TrackerKLT: varX: ") + FloatToStr( m_pTrackerSystemData->GetVariance().X())
                                 + moText(" varY: ") + FloatToStr(m_pTrackerSystemData->GetVariance().Y()) );
                                 */

                  ///CALCULATE CIRCULAR MATRIX
                  for(int i=0; i<m_pTrackerSystemData->GetFeatures().Count(); i++ ) {
                      TF = m_pTrackerSystemData->GetFeatures().GetRef(i);
                      if (TF) {
                          if (TF->val>=0) {
                              m_pTrackerSystemData->SetPositionMatrixC( TF->x, TF->y, 1 );
                              vel = moVector2f( TF->v_x, TF->v_y ).Length();
                              //acc = moVector2f( TF->a_x, TF->a_y ).Length();
                              if (vel>=0.01) m_pTrackerSystemData->SetMotionMatrixC( TF->x, TF->y, 1 );
                          }
                      }
                  }
                  /*

                  MODebug2->Push( moText("moOpenFace::UpdateParameters > ValidFeatures: ")
                                                  + FloatToStr(sumN)
                                                  + moText(" GetValidFeatures:")
                                                  + IntToStr( m_pTrackerSystemData->GetValidFeatures() )
                                                 );*/



              }


          }

          if (m_OutTracker==NULL) {
              int oidx = GetOutletIndex("TRACKER");
              if ( oidx>=0 ) {
                m_OutTracker = m_Outlets.Get( oidx );
              }
          }

          if (m_OutTracker && m_pTrackerSystemData) {
              m_OutTracker->GetData()->SetPointer( (MOpointer) m_pTrackerSystemData, sizeof(moTrackerSystemData) );
              m_OutTracker->Update(true); ///to notify Inlets!!
          }

          if (m_pBucketDiff2) {
              m_pBucketDiff2->EmptyBucket();
          }


  }

  m_bReInit = false;
}

void
moOpenFace::BodyDetection() {

  std::vector<Rect> bodies;


  if (m_pSrcTexture==NULL) {
    MODebug2->Error("moOpenFace::BodyDetection() > Source Texture not selected " );
    return;
  }

  /**INIT CASCADE CLASIFFIER with hog or haarcascades standard training XML files*/
  if (m_bReInit) {
    moText cascade_file;
    string cfile;
/*
    cascade_file = m_pResourceManager->GetDataMan()->GetDataPath() + "/haarcascades/" + "haarcascade_body.xml";
    cfile = (char*)cascade_file;
    if (!body_cascade.load(cfile))
    {
        MODebug2->Error("Error loading : " + cascade_file );
        return;
    }
    MODebug2->Message("Loaded! : " + cascade_file );
*/
/*
    cascade_file = m_pResourceManager->GetDataMan()->GetDataPath() + "/hogcascades/" + "hogcascade_pedestrians.xml";
    cfile = (char*)cascade_file ;
    if (!hog_cascade.load(cfile))
    {
        MODebug2->Error("Error loading : " + cascade_file );
        return;
    }
    MODebug2->Message("Loaded! : " + cascade_file );
*/
    hog_cascade.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

    cascade_file = m_pResourceManager->GetDataMan()->GetDataPath() + "/haarcascades/" + "haarcascade_frontalface_alt.xml";
    cfile = (char*)cascade_file;
    if (!face_cascade.load(cfile))
    {
        MODebug2->Error("Error loading : " + cascade_file );
        return;
    }
    if (m_debug_on) MODebug2->Message("Loaded! : " + cascade_file );

  }


  /**GET THE IMAGE: resized*/
  //moVector2i resizer( 200, 150 );
  moVector2i resizer( m_reduce_width, m_reduce_height );
  IplImage* srcframe = TextureToCvImage( m_pSrcTexture,  resizer  );
  if (srcframe==NULL) {
    MODebug2->Error("Error TextureToCvImage() : " + m_pSrcTexture->GetName() );
    return;
  }

 /**CONVERT AND ENHANCE TO GRAYSCALE*/
  //Mat frame( srcframe );
  IplImage* dstframe;
  //cvCvtColor ( srcframe, dstframe, CV_BGR2RGB );
  Mat framebgr = cv::cvarrToMat(srcframe);
  Mat frame = framebgr;
  cvtColor ( framebgr, frame, COLOR_BGR2RGB );

  //Mat frame = cv::cvarrToMat(srcframe);

  Mat frame_gray;
  cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
  //equalizeHist(frame_gray, frame_gray);

// Detect faces
  //body_cascade.detectMultiScale( frame_gray, bodies, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
  hog_cascade.detectMultiScale( frame_gray, bodies, 0, Size(8,8), Size(32,32), 1.05, 2 );

  if (resizer.X()==0 ) resizer = moVector2i( m_pSrcTexture->GetWidth(),m_pSrcTexture->GetHeight() );

  moData pData;
  /**
    JSBodies contains all bodies:
    [
//body 1
      { x: 0.0, y: 0.0, w: 1.0, h: 1.0, fx: 0.0, fy: 0.0 , fw: 1.0, fh: 1.0 },
//body 2
      { x: 0.0, y: 0.0, w: 1.0, h: 1.0, fx: 0.0, fy: 0.0 , fw: 1.0, fh: 1.0 },
//body 3
      { x: 0.0, y: 0.0, w: 1.0, h: 1.0, fx: 0.0, fy: 0.0 , fw: 1.0, fh: 1.0 },
    ]
  */
  moText JSBodies;
  if (m_pDataMessage) {
    m_pDataMessage->Empty();//EMPTY IN UpdateParameters()
  }

  if (m_pDataMessage && bodies.size()) {
      JSBodies = "[";
      pData.SetText( moText("opencv") );
      m_pDataMessage->Add(pData);

      pData.SetText( moText("BODY_DETECTIONS") );
      m_pDataMessage->Add(pData);
  }

  for (int ic = 0; ic < bodies.size(); ic++) // Iterate through all current elements (detected faces)
  {
    if (m_debug_on) MODebug2->Message("Body Detected: x:"+ IntToStr(bodies[ic].x) + " y:"+ IntToStr(bodies[ic].y) + " w:"+ IntToStr(bodies[ic].width)+ " h:"+ IntToStr(bodies[ic].height) );

    float BodyWidth = ((float)bodies[ic].width)/(float)resizer.X();
    float BodyHeight = ((float)bodies[ic].height)/(float)resizer.Y();

    float BodyLeft = (float)bodies[ic].x/(float)resizer.X() - 0.5f  + BodyWidth*0.5f;
    float BodyTop = -(float)bodies[ic].y/(float)resizer.Y() + 0.5f - BodyHeight*0.5f;

    if (m_pDataMessage) {
        JSBodies+= "{";
        JSBodies+= "\"x\": \""+ FloatToStr(BodyLeft,4) + "\",";
        JSBodies+= "\"y\": \"" + FloatToStr(BodyTop,4) + "\",";
        JSBodies+= "\"w\": \"" + FloatToStr(BodyWidth,4) + "\",";
        JSBodies+= "\"h\": \"" + FloatToStr(BodyHeight,4) + "\",";
    }

    rectangle( frame,
           Point( bodies[ic].x, bodies[ic].y ),
           Point( bodies[ic].x+bodies[ic].width, bodies[ic].y+bodies[ic].height),
           Scalar( 128, 255, 128 ), 2);

    std::vector<Rect> faces;

    int left = max(0,bodies[ic].x);
    int top = max( 0, bodies[ic].y);
    int right = bodies[ic].x+bodies[ic].width;
    int bottom = bodies[ic].y+bodies[ic].height;

    Mat roi_gray( frame_gray, cvRect( left, top, min( frame_gray.cols-left, right-left ), min( frame_gray.rows-top, bottom - top ) ) );
    face_cascade.detectMultiScale( roi_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(8, 8) );
      for (int ice = 0; ice < faces.size(); ice++) // Iterate through all current elements (detected faces)
      {
        rectangle( frame,
           Point( bodies[ic].x+faces[ice].x, bodies[ic].y+faces[ice].y ),
           Point( bodies[ic].x+faces[ice].x+faces[ice].width, bodies[ic].y+faces[ice].y+faces[ice].height),
           Scalar( 0, 255, 255 ), 2);
      }


    if (m_pDataMessage) {
      JSBodies+= "},";
    }

  }


  if (m_pDataMessage && bodies.size()) {
      JSBodies+= "]";
      pData.SetText( JSBodies );
      m_pDataMessage->Add(pData);
  }

  #ifndef WIN32
  imwrite( "/tmp/dstblobs/dstblobs.jpg", frame );
  #endif
  CvMatToTexture( frame, 0 , 0, 0, m_pCVBlobs );

  m_bReInit = false;

}

// Visualising the results
void
moOpenFace::visualise_tracking(cv::Mat& captured_image,
    cv::Mat_<float>& depth_image,
    const LandmarkDetector::CLNF& face_model,
    const LandmarkDetector::FaceModelParameters& det_parameters,
    cv::Point3f gazeDirection0,
    cv::Point3f gazeDirection1,
    int frame_count,
    double fx,
    double fy,
    double cx,
    double cy)
{

	// Drawing the facial landmarks on the face and the bounding box around it if tracking is successful and initialised
	double detection_certainty = face_model.detection_certainty;
	bool detection_success = face_model.detection_success;

	double visualisation_boundary = 0.2;
	// Only draw if the reliability is reasonable, the value is slightly ad-hoc
	if (detection_certainty < visualisation_boundary)
	{
		LandmarkDetector::Draw(captured_image, face_model);

		int n = face_model.detected_landmarks.rows/2;
		int eyebrow_left = 20;
		int eyebrow_right = 23;
		int eye_left = 38;
		int eye_right = 43;
		int jaw_nose = 33;
		int jaw_chin = 8;
		cv::Point eyebrowLeft(
            cvRound(face_model.detected_landmarks.at<double>(eyebrow_left) * (double)draw_multiplier),
            cvRound(face_model.detected_landmarks.at<double>(eyebrow_left + n)* (double)draw_multiplier));
		cv::Point eyeLeft(
            cvRound(face_model.detected_landmarks.at<double>(eye_left) * (double)draw_multiplier),
            cvRound(face_model.detected_landmarks.at<double>(eye_left + n)* (double)draw_multiplier));

        cv::Point jawNose(
            cvRound(face_model.detected_landmarks.at<double>(jaw_nose) * (double)draw_multiplier),
            cvRound(face_model.detected_landmarks.at<double>(jaw_nose + n)* (double)draw_multiplier));
        cv::Point jawChin(
            cvRound(face_model.detected_landmarks.at<double>(jaw_chin) * (double)draw_multiplier),
            cvRound(face_model.detected_landmarks.at<double>(jaw_chin + n)* (double)draw_multiplier));

        cv::Point openEyeBrowLeft(eyebrowLeft-eyeLeft);
        cv::Point openJaw(jawChin-jawNose);

        vecEyeBrowLeft = moVector2d( openEyeBrowLeft.x, openEyeBrowLeft.y );
        vecOpenJaw = moVector2d( openJaw.x, openJaw.y );

        //cout << "eye left:" << eyeLeft << "eyebrow left:" << eyebrowLeft << "L:" << vecEyeBrowLeft.Length() << endl;

		double vis_certainty = detection_certainty;
		if (vis_certainty > 1)
			vis_certainty = 1;
		if (vis_certainty < -1)
			vis_certainty = -1;


		vis_certainty = (vis_certainty + 1) / (visualisation_boundary + 1);

		// A rough heuristic for box around the face width
		int thickness = (int)std::ceil(2.0* ((double)captured_image.cols) / 640.0);

		///cv::Vec6d pose_estimate_to_draw = LandmarkDetector::GetCorrectedPoseWorld(face_model, fx, fy, cx, cy);
		cv::Vec6d pose_estimate_to_draw = LandmarkDetector::GetPose(face_model, fx, fy, cx, cy);

		// Draw it in reddish if uncertain, blueish if certain
		LandmarkDetector::DrawBox(captured_image, pose_estimate_to_draw, cv::Scalar((1 - vis_certainty)*255.0, 0, vis_certainty * 255), thickness, fx, fy, cx, cy);

		if (det_parameters.track_gaze && detection_success && face_model.eye_model)
		{
			GazeAnalysis::DrawGaze(captured_image, face_model, gazeDirection0, gazeDirection1, fx, fy, cx, cy);
		}
	}

	// Work out the framerate
	if (frame_count % 10 == 0)
	{
		double t1 = cv::getTickCount();
		fps_tracker = 10.0 / (double(t1 - t0) / cv::getTickFrequency());
		t0 = t1;
	}

	// Write out the framerate on the image before displaying it
	char fpsC[255];
	std::sprintf(fpsC, "%d", (int)fps_tracker);
	string fpsSt("FPS:");
	fpsSt += fpsC;
	cv::putText(captured_image, fpsSt, cv::Point(10, 20), CV_FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 0, 0));

}

void
moOpenFace::FaceDetection() {


  if (m_pSrcTexture==NULL) {
    MODebug2->Error("moOpenFace::FaceDetection() > Source Texture not selected " );
    return;
  }

    //ThresholdFilter();
  /**INIT CASCADE CLASIFFIER with haarcascades standard training XML files*/



  if (m_bReInit) {

    MODebug2->Message("Loaded OpenFace FaceModelParameters." );
    MODebug2->Message("face_detector_location: " + moText(det_parameters.face_detector_location.c_str()) );
    MODebug2->Message("model_location: " + moText(det_parameters.model_location.c_str()) );

    MODebug2->Message("Loading CLNF." );
    p_clnf_model = new LandmarkDetector::CLNF(det_parameters.model_location);
    det_parameters.track_gaze = false;

  }

  /**GET THE IMAGE: resized*/
  //moVector2i resizer( 200, 150 );
  moVector2i resizer( m_reduce_width, m_reduce_height );
  IplImage* srcframe = TextureToCvImage( m_pSrcTexture,  resizer  );
  if (srcframe==NULL) {
    MODebug2->Error("Error TextureToCvImage() : " + m_pSrcTexture->GetName() );
    return;
  }


  //return;
  //Mat dstblobs = Mat::zeros(m_reduce_width, m_reduce_height, CV_8UC3);
  //Scalar color( 255, 255, 255 );



  /**CONVERT AND ENHANCE TO GRAYSCALE*/
  //Mat frame( srcframe );
  Mat framebgr = cv::cvarrToMat(srcframe);
  Mat frame = framebgr;
  cvtColor ( framebgr, frame, COLOR_BGR2RGB );
  Mat frame_gray;
  cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
  //equalizeHist(frame_gray, frame_gray);
  bool rotatem;
  //M = cv2.getRotationMatrix2D((cols/2,rows/2),90,1)
  //dst = cv2.warpAffine(img,M,(cols,rows))
  cv::Mat M = cv::getRotationMatrix2D( cv::Point2f( m_reduce_width/2, m_reduce_height/2) , 90, 1 );
  cv::Mat dst;
  cv::warpAffine( frame_gray ,dst, M,cv::Size(m_reduce_width,m_reduce_height));
  //cv::warpAffine(
// The actual facial landmark detection / tracking

    cv::Mat_<float> depth_image;
    cv::Vec6d pose_estimate_to_draw;

	//cv::Mat_<uchar> grayscale_image;
        cv::Point3f gazeDirection0(0, 0, -1);
        cv::Point3f gazeDirection1(0, 0, -1);

    //bool detection_success = LandmarkDetector::DetectLandmarksInVideo(frame_gray, depth_image, *p_clnf_model, det_parameters);
	bool detection_success = LandmarkDetector::DetectLandmarksInVideo(frame_gray, *p_clnf_model, det_parameters);
    if (detection_success && p_clnf_model) {
        //MODebug2->Message("Detection Success!");
        // Visualising the results
        // Drawing the facial landmarks on the face and the bounding box around it if tracking is successful and initialised
        double detection_certainty = p_clnf_model->detection_certainty;
        float fx = 0, fy = 0, cx = 0, cy = 0;
        // If cx (optical axis centre) is undefined will use the image size/2 as an estimate
        bool cx_undefined = false;
        bool fx_undefined = false;
        if (cx == 0 || cy == 0)
        {
            cx_undefined = true;
        }
        if (fx == 0 || fy == 0)
        {
            fx_undefined = true;
        }
        // If optical centers are not defined just use center of image
		if (cx_undefined)
		{
			cx = m_reduce_width / 2.0f;
			cy = m_reduce_height / 2.0f;
		}
		// Use a rough guess-timate of focal length
		if (fx_undefined)
		{
			fx = 500 * (m_reduce_width / 640.0);
			fy = 500 * (m_reduce_height / 480.0);

			fx = (fx + fy) / 2.0;
			fy = fx;
		}

        // Gaze tracking, absolute gaze direction


        if (det_parameters.track_gaze && detection_success && p_clnf_model->eye_model)
        {
            GazeAnalysis::EstimateGaze(*p_clnf_model, gazeDirection0, fx, fy, cx, cy, true);
            GazeAnalysis::EstimateGaze(*p_clnf_model, gazeDirection1, fx, fy, cx, cy, false);
        }

        cv::Mat_<float> depth_image;
        frame_count++;
        visualise_tracking(frame, depth_image, *p_clnf_model, det_parameters, gazeDirection0, gazeDirection1, frame_count, fx, fy, cx, cy);

        //pose_estimate_to_draw = LandmarkDetector::GetCorrectedPoseWorld(*p_clnf_model, fx, fy, cx, cy);
		pose_estimate_to_draw = LandmarkDetector::GetPose(*p_clnf_model, fx, fy, cx, cy);
    }


  float FaceX = 0,FaceY = 0, FaceZ = 0;
  float FaceWidth  = 0,FaceHeight = 0, FaceDepth = 0;
  float FaceAngleX = 0,FaceAngleY = 0, FaceAngleZ = 0;

  float EyeLeftX = 0,EyeLeftY = 0;
  float EyeLeftWidth = 0,EyeLeftHeight = 0;
  float EyeRightX = 0,EyeRightY = 0;
  float EyeRightWidth = 0,EyeRightHeight = 0;

  if (resizer.X()==0 ) resizer = moVector2i( m_pSrcTexture->GetWidth(),m_pSrcTexture->GetHeight() );

    if (p_clnf_model) {
        FaceX = pose_estimate_to_draw[0];
        FaceY = pose_estimate_to_draw[1];
        FaceZ = pose_estimate_to_draw[2];
        FaceAngleX = pose_estimate_to_draw[3];
        FaceAngleY = pose_estimate_to_draw[4];
        FaceAngleZ = pose_estimate_to_draw[5];
    }

/*
  for (int ic = 0; ic < faces.size(); ic++) // Iterate through all current elements (detected faces)
  {


      FaceWidth = ((float)faces[ic].width)/(float)resizer.X();
      FaceHeight = ((float)faces[ic].height)/(float)resizer.Y();

      FaceLeft = (float)faces[ic].x/(float)resizer.X() - 0.5f  + FaceWidth*0.5f;
      FaceTop = -(float)faces[ic].y/(float)resizer.Y() + 0.5f - FaceHeight*0.5f;

      FaceWidth*=1.3;
      FaceHeight*=1.3;*/
/*
      std::vector<Rect> eyes;

      //eyes = eye_cascade.detectMultiScale(roi_gray)
      Mat roi_gray( frame_gray, cvRect( faces[ic].x,faces[ic].y, faces[ic].width, faces[ic].height ) );
      eye_cascade.detectMultiScale( roi_gray, eyes, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30) );
      for (int ice = 0; ice < eyes.size(); ice++) // Iterate through all current elements (detected faces)
      {
        EyeLeftWidth = ((float)eyes[ice].width)/(float)resizer.X();
        EyeLeftHeight = ((float)eyes[ice].height)/(float)resizer.Y();

        EyeLeftX = (float)eyes[ic].x/(float)resizer.X() - 0.5f  + EyeLeftWidth*0.5f;
        EyeLeftY = -(float)eyes[ic].y/(float)resizer.Y() + 0.5f - EyeLeftHeight*0.5f;

        EyeLeftWidth*=1.3;
        EyeLeftHeight*=1.3;
        rectangle( frame,
           Point( faces[ic].x+eyes[ice].x, faces[ic].y+eyes[ice].y ),
           Point( faces[ic].x+eyes[ice].x+eyes[ice].width, faces[ic].y+eyes[ice].y+eyes[ice].height),
           Scalar( 0, 255, 255 ), 2);
      }
*/
/*
      rectangle( frame,
           Point( faces[ic].x, faces[ic].y ),
           Point( faces[ic].x+faces[ic].width, faces[ic].y+faces[ic].height),
           Scalar( 128, 255, 128 ), 2);
  }

  if (faces.size()) {
    if (m_debug_on) MODebug2->Message("moOpenFace::FaceDetection on '"+m_pSrcTexture->GetName()+"' > "
    + moText(" FaceLeft:") + FloatToStr( FaceLeft )
    + moText(" FaceTop:")+ FloatToStr( FaceTop )
    + moText(" FaceWidth:") + FloatToStr( FaceWidth )
    + moText(" FaceHeight:")+ FloatToStr( FaceHeight )
    );


  }*/


  if (!m_FaceDetection) {
      m_FaceDetection = m_Outlets.GetRef( GetOutletIndex( moText("FACE_DETECTION") ) );
      //MODebug2->Message("moOpenFace::FaceDetection > outlet FACE_POSITION_X: "+IntToStr((int)m_FacePositionX));
  } else {
    if (p_clnf_model) {
      m_FaceDetection->GetData()->SetFloat(  p_clnf_model->detection_success );
      m_FaceDetection->Update(true);
    }
  }

    if (!m_FaceDetectionCertainty) {
      m_FaceDetectionCertainty = m_Outlets.GetRef( GetOutletIndex( moText("FACE_DETECTION_CERTAINTY") ) );
      //MODebug2->Message("moOpenFace::FaceDetection > outlet FACE_POSITION_X: "+IntToStr((int)m_FacePositionX));
  } else {
    if (p_clnf_model) {
      m_FaceDetectionCertainty->GetData()->SetFloat(  p_clnf_model->detection_certainty );
      m_FaceDetectionCertainty->Update(true);
    }
  }


  if (!m_GazeDirectionL_X) {
      m_GazeDirectionL_X = m_Outlets.GetRef( GetOutletIndex( moText("GAZE_DIRECTIONL_X") ) );
      //MODebug2->Message("moOpenFace::FaceDetection > outlet FACE_POSITION_X: "+IntToStr((int)m_FacePositionX));
  } else {
    if (p_clnf_model) {
      m_GazeDirectionL_X->GetData()->SetFloat( gazeDirection0.x );
      m_GazeDirectionL_X->Update(true);
    }
  }
  if (!m_GazeDirectionL_Y) {
      m_GazeDirectionL_Y = m_Outlets.GetRef( GetOutletIndex( moText("GAZE_DIRECTIONL_Y") ) );
      //MODebug2->Message("moOpenFace::FaceDetection > outlet FACE_POSITION_X: "+IntToStr((int)m_FacePositionX));
  } else {
    if (p_clnf_model) {
      m_GazeDirectionL_Y->GetData()->SetFloat( gazeDirection0.y );
      m_GazeDirectionL_Y->Update(true);
    }
  }
  if (!m_GazeDirectionL_Z) {
      m_GazeDirectionL_Z = m_Outlets.GetRef( GetOutletIndex( moText("GAZE_DIRECTIONL_Z") ) );
      //MODebug2->Message("moOpenFace::FaceDetection > outlet FACE_POSITION_X: "+IntToStr((int)m_FacePositionX));
  } else {
    if (p_clnf_model) {
      m_GazeDirectionL_Z->GetData()->SetFloat( gazeDirection0.z );
      m_GazeDirectionL_Z->Update(true);
    }
  }



/**
  rectangle( frame,
           Point( 1,1 ),
           Point( 127,127),
           Scalar( 255, 255, 255 ));*/

  if (!m_FacePositionX) {
      m_FacePositionX = m_Outlets.GetRef( GetOutletIndex( moText("FACE_POSITION_X") ) );
      //MODebug2->Message("moOpenFace::FaceDetection > outlet FACE_POSITION_X: "+IntToStr((int)m_FacePositionX));
  } else {
    m_FacePositionX->GetData()->SetFloat(  FaceX );
    m_FacePositionX->Update(true);
  }
  if (!m_FacePositionY) {
      m_FacePositionY = m_Outlets.GetRef( this->GetOutletIndex( moText("FACE_POSITION_Y") ) );
      //MODebug2->Message("moOpenFace::FaceDetection > outlet FACE_POSITION_Y: "+IntToStr((int)m_FacePositionY));
  } else {
    m_FacePositionY->GetData()->SetFloat(  FaceY );
    m_FacePositionY->Update(true);
  }
  if (!m_FacePositionZ) {
      m_FacePositionZ = m_Outlets.GetRef( GetOutletIndex( moText("FACE_POSITION_Z") ) );
      //MODebug2->Message("moOpenFace::FaceDetection > outlet FACE_POSITION_X: "+IntToStr((int)m_FacePositionX));
  } else {
    m_FacePositionZ->GetData()->SetFloat(  FaceZ );
    m_FacePositionZ->Update(true);
    //MODebug2->Message("Z:"+FloatToStr(FaceZ));
  }

  if (!m_FaceSizeWidth) {
      m_FaceSizeWidth = m_Outlets.GetRef( this->GetOutletIndex( moText("FACE_SIZE_WIDTH") ) );
      //MODebug2->Message("moOpenFace::FaceDetection > outlet FACE_SIZE_WIDTH: "+IntToStr((int)m_FaceSizeWidth));
  } else {
    m_FaceSizeWidth->GetData()->SetFloat(  FaceWidth );
    m_FaceSizeWidth->Update(true);
  }
  if (!m_FaceSizeHeight) {
      m_FaceSizeHeight = m_Outlets.GetRef( this->GetOutletIndex( moText("FACE_SIZE_HEIGHT") ) );
      //MODebug2->Message("moOpenFace::FaceDetection > outlet FACE_SIZE_HEIGHT: "+IntToStr((int)m_FaceSizeHeight));
  } else {
    m_FaceSizeHeight->GetData()->SetFloat(  FaceHeight );
    m_FaceSizeHeight->Update(true);
  }


    if (!m_FaceAngleX) {
      m_FaceAngleX = m_Outlets.GetRef( GetOutletIndex( moText("FACE_ANGLE_X") ) );
      //MODebug2->Message("moOpenFace::FaceDetection > outlet FACE_POSITION_X: "+IntToStr((int)m_FacePositionX));
  } else {
    m_FaceAngleX->GetData()->SetFloat(  FaceAngleX );
    m_FaceAngleX->Update(true);
  }

    if (!m_FaceAngleY) {
      m_FaceAngleY = m_Outlets.GetRef( GetOutletIndex( moText("FACE_ANGLE_Y") ) );
      //MODebug2->Message("moOpenFace::FaceDetection > outlet FACE_POSITION_X: "+IntToStr((int)m_FacePositionX));
  } else {
    m_FaceAngleY->GetData()->SetFloat(  FaceAngleY );
    m_FaceAngleY->Update(true);
  }

  if (!m_FaceAngleZ) {
      m_FaceAngleZ = m_Outlets.GetRef( GetOutletIndex( moText("FACE_ANGLE_Z") ) );
      //MODebug2->Message("moOpenFace::FaceDetection > outlet FACE_POSITION_X: "+IntToStr((int)m_FacePositionX));
  } else {
    m_FaceAngleZ->GetData()->SetFloat(  FaceAngleZ );
    m_FaceAngleZ->Update(true);
  }



  CvMatToTexture( frame, 0 , 0, 0, m_pCVBlobs );
  #ifndef WIN32
  //imwrite( "/tmp/dstblobs/dstblobs.jpg", frame );
  //m_pCVBlobs->CreateThumbnail( "JPG", m_pCVBlobs->GetWidth(), m_pCVBlobs->GetHeight(), "/tmp/dstblobs/dstblobs"  );
  #endif // WIN32

    if (m_pDataVectorMessage && p_clnf_model) {

        m_pDataVectorMessage->Empty();
        moData pData;
        int n = p_clnf_model->detected_landmarks.rows/2;
        //MODebug2->Message("moOpenFace::FaceDetection > Sending n marks:"+IntToStr(n));
        for( int i=0; i<n; i++) {
            cv::Point featurePoint(
                cvRound(p_clnf_model->detected_landmarks.at<double>(i) * (double)draw_multiplier),
                cvRound(p_clnf_model->detected_landmarks.at<double>(i + n) * (double)draw_multiplier));
            pData.SetFloat( featurePoint.x );
            m_pDataVectorMessage->Add(pData);
            pData.SetFloat( featurePoint.y );
            m_pDataVectorMessage->Add(pData);
        }


    }

    if (m_pDataMessage) {
        m_pDataMessage->Empty();//EMPTY IN UpdateParameters()
        moData pData;

        pData.SetText( moText("opencv") );
        m_pDataMessage->Add(pData);

        pData.SetText( moText("FACE_DETECTION") );
        m_pDataMessage->Add(pData);

        pData.SetInt( (int)(faces.size()) );
        m_pDataMessage->Add(pData);

        pData.SetText( moText("FACE_POSITION_X") );
        m_pDataMessage->Add(pData);

        pData.SetFloat( FaceX );
        m_pDataMessage->Add(pData);

        pData.SetText( moText("FACE_POSITION_Y") );
        m_pDataMessage->Add(pData);

        pData.SetFloat( FaceY );
        m_pDataMessage->Add(pData);

        pData.SetText( moText("FACE_POSITION_Z") );
        m_pDataMessage->Add(pData);

        pData.SetFloat( FaceZ );
        m_pDataMessage->Add(pData);

        pData.SetText( moText("FACE_SIZE_WIDTH") );
        m_pDataMessage->Add(pData);

        pData.SetFloat( FaceWidth );
        m_pDataMessage->Add(pData);

        pData.SetText( moText("FACE_SIZE_HEIGHT") );
        m_pDataMessage->Add(pData);

        pData.SetFloat( FaceHeight );
        m_pDataMessage->Add(pData);
        /*
        moText ccc = "";
        for( int c=0; c<m_pDataMessage->Count(); c++) {
          ccc = ccc + m_pDataMessage->Get(c).ToText();
        }
        //MODebug2->Push(ccc);
        */
    }

  framebgr.release();
  frame.release();
  frame_gray.release();
  cvReleaseImage(&srcframe);
  faces.clear();
  m_bReInit = false;
}



void
moOpenFace::FaceRecognition() {


}

void
moOpenFace::ThresholdFilter() {
 /**GET THE IMAGE: resized*/
  //moVector2i resizer( m_reduce_width, m_reduce_height );
  moVector2i resizer( 128, 128 );
  IplImage* srcframe = TextureToCvImage( m_pSrcTexture,  resizer  );
  if (srcframe==NULL) {
    MODebug2->Error("Error TextureToCvImage() : " + m_pSrcTexture->GetName() );
    return;
  }
  //cvFree(srcframe);

  /**CONVERT AND ENHANCE TO GRAYSCALE*/
  //Mat frame( srcframe );
  Mat frame = cv::cvarrToMat(srcframe);
  Mat frame_gray;
  cvtColor( frame, frame_gray, COLOR_BGR2GRAY);
  frame.release();

  threshold( frame_gray, dstthresh, m_threshold, m_threshold_max, (int)m_threshold_type );
  frame_gray.release();

  Mat frame_color;
  cvtColor( dstthresh, frame_color, COLOR_GRAY2BGR);
  CvMatToTexture( frame_color, 0 , 0, 0, m_pCVThresh );
  frame_color.release();
  cvReleaseImage(&srcframe);
}

void
moOpenFace::ThresholdRecognition() {

  ThresholdFilter();
  dstthresh.release();
}

void
moOpenFace::ContourRecognition(){

}

void
moOpenFace::BlobRecognition() {

}

void
moOpenFace::ColorRecognition() {

}



MOswitch moOpenFace::SetStatus(MOdevcode devcode, MOswitch state) {
    return true;
}

void moOpenFace::SetValue( MOdevcode cd, MOfloat vl ) {

}

void moOpenFace::SetValue( MOdevcode cd, MOint vl ) {

}


//0: false    1  -1 :true
MOswitch moOpenFace::GetStatus(MOdevcode devcode) {

	return 0;
}

MOint moOpenFace::GetValue(MOdevcode devcode) {
    return(-1);
}

MOpointer moOpenFace::GetPointer(MOdevcode devcode ) {
	return (NULL);
}

//return devicecode index of corresponding code string
MOdevcode moOpenFace::GetCode(moText strcod) {
    return(-1);
}

GLuint
moOpenFace::CvMatToTexture( Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter, moTexture* p_destTexture )
{
	// Generate a number for our textureID's unique handle
	/*
	GLuint textureID;
	glGenTextures(1, &textureID);

  //p_destTexture = NULL;

	// Bind to our texture handle

	glBindTexture(GL_TEXTURE_2D, textureID);

	// Catch silly-mistake texture interpolation method for magnification
	if (magFilter == GL_LINEAR_MIPMAP_LINEAR  ||
	    magFilter == GL_LINEAR_MIPMAP_NEAREST ||
	    magFilter == GL_NEAREST_MIPMAP_LINEAR ||
	    magFilter == GL_NEAREST_MIPMAP_NEAREST)
	{
		cout << "You can't use MIPMAPs for magnification - setting filter to GL_LINEAR" << endl;
		magFilter = GL_LINEAR;
	}

	// Set texture interpolation methods for minification and magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

	// Set texture clamping method
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapFilter);

	// Set incoming texture format to:
	// GL_BGR       for CV_CAP_OPENNI_BGR_IMAGE,
	// GL_LUMINANCE for CV_CAP_OPENNI_DISPARITY_MAP,
	// Work out other mappings as required ( there's a list in comments in main() )
	GLenum inputColourFormat = GL_BGR;
	if (mat.channels() == 1)
	{
		inputColourFormat = GL_LUMINANCE;
	}

	// Create the texture
	glTexImage2D(GL_TEXTURE_2D,     // Type of texture
	             0,                 // Pyramid level (for mip-mapping) - 0 is the top level
	             GL_RGB,            // Internal colour format to convert to
	             mat.cols,          // Image width  i.e. 640 for Kinect in standard mode
	             mat.rows,          // Image height i.e. 480 for Kinect in standard mode
	             0,                 // Border width in pixels (can either be 1 or 0)
	             inputColourFormat, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
	             GL_UNSIGNED_BYTE,  // Image data type
	             mat.ptr());        // The actual image data itself

	// If we're using mipmaps then generate them. Note: This requires OpenGL 3.0 or higher
	if (minFilter == GL_LINEAR_MIPMAP_LINEAR  ||
	    minFilter == GL_LINEAR_MIPMAP_NEAREST ||
	    minFilter == GL_NEAREST_MIPMAP_LINEAR ||
	    minFilter == GL_NEAREST_MIPMAP_NEAREST)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}
*/
  int textureID = 0;

  if (p_destTexture==NULL) {
    p_destTexture = m_pCVSourceTexture;
  }

  if (p_destTexture) {
      if (p_destTexture->GetWidth()!=mat.cols || p_destTexture->GetHeight()!=mat.rows)
        p_destTexture->BuildEmpty( mat.cols, mat.rows );

      //p_destTexture->SetBuffer( mat.ptr(), GL_BGR );
      p_destTexture->SetBuffer( mat.ptr(), GL_RGB );
      textureID = p_destTexture->GetGLId();
  }

	return textureID;
}

IplImage*
moOpenFace::TextureToCvImage( moTexture* p_pTexture, moVector2i p_Resize ) {

  if (p_pTexture==NULL) {
    MODebug2->Error("moOpenFace::TextureToCvImage > error NULL texture");
    return NULL;
  }

  if (p_pTexture->GetWidth()==0 || p_pTexture->GetHeight()==0 ) {
    MODebug2->Error("moOpenFace::TextureToCvImage > source Texture Width or Height is NULL");
    return NULL;
  }

  int _bufsize = p_pTexture->GetWidth() * p_pTexture->GetHeight() * 3;
  if (m_BufferSize != _bufsize) {
    if (m_pBuffer!=NULL) {
      delete [] m_pBuffer;
      m_pBuffer = NULL;
    }
  }
  if (m_pBuffer==NULL) {
    m_BufferSize = _bufsize;
    m_pBuffer = new MOubyte [ m_BufferSize ];
  }

  if (m_pBuffer==NULL) {
    MODebug2->Error("moOpenFace::TextureToCvImage > Buffer can't be assigned for Texture:"
                    + p_pTexture->GetName()
                    + IntToStr(p_pTexture->GetWidth())
                    +moText("x")
                    + IntToStr(p_pTexture->GetHeight())
                    );
    return NULL;
  }
  //p_pTexture->GetBuffer( m_pBuffer, GL_BGR, GL_UNSIGNED_BYTE );
  p_pTexture->GetBuffer( m_pBuffer, GL_RGB, GL_UNSIGNED_BYTE );

  //(MOpointer)pTS->GetData();

  if ( m_bReInit) {
      //if (m_pIplImage) cvReleaseImage( &m_pIplImage );
      if (m_pIplImage==NULL) m_pIplImage = cvCreateImage( cvSize( p_pTexture->GetWidth(), p_pTexture->GetHeight()),
                      IPL_DEPTH_8U,
                      3 );
      if (m_debug_on) MODebug2->Message("moOpenFace::TextureToCvImage > created IplImage for: " + p_pTexture->GetName());

  }

  if (m_pIplImage==NULL) {
    MODebug2->Error("moOpenFace::TextureToCvImage > IplImage is NULL !!");
    return NULL;
  }

  //img = cvCreateImageHeader( cvSize( pSample->m_VideoFormat.m_Width, pSample->m_VideoFormat.m_Height), IPL_DEPTH_8U, 3 );
  //cvInitImageHeader( img, cvSize( pSample->m_VideoFormat.m_Width, pSample->m_VideoFormat.m_Height), IPL_DEPTH_8U, 3, IPL_ORIGIN_TL);

  //cvZero( m_pIplImage );
  try {
    cvSetData( m_pIplImage, (void*)m_pBuffer, p_pTexture->GetWidth()*3);
    if (m_debug_on) MODebug2->Message("moOpenFace::TextureToCvImage > cvSetData called.");
  } catch(...) {
    MODebug2->Error("moOpenFace::TextureToCvImage > ");
  }

  if (m_pIplImage && p_Resize.X()>0 && p_Resize.Y()>0) {

    //MODebug2->Message("moOpenFace::TextureToCvImage > Resizing ");
    IplImage* new_img = NULL;
    new_img = cvCreateImage(cvSize(p_Resize.X(), p_Resize.Y()), m_pIplImage->depth, m_pIplImage->nChannels);
    cvResize( m_pIplImage,  new_img );
    return new_img;

  }


  return m_pIplImage;

}

/*
moTexture*
moOpenFace::CvImageToTexture(  Mat p_srcCvImage ) {

  return false;
}
*/

//esto se ejecuta por cada CICLO PRINCIPAL DE moConsole(frame)
//por ahora no nos sirve de nada, porque no mandamos events....ni procesamos events...
//al menos que Ligia...o algun device especial(nada mas el hecho de que se haya
//enchufado la camara por ejemplo
//podriamos poner una funcion aqui de reconocimiento de DV....
void moOpenFace::Update(moEventList *Events) {
	//get the pointer from the Moldeo Object sending it...

	moBucket* pBucket = NULL;
	moVideoSample* pSample = NULL;

    ///Always delete
	moEvent *actual,*tmp;


	actual = Events->First;
	while(actual!=NULL) {
		tmp = actual->next;
		if (actual->deviceid == GetId() && actual->reservedvalue3!=MO_MESSAGE) {
			Events->Delete(actual);
		}
		actual = tmp;
	}


    UpdateParameters();

    //MODebug2->Push( moText("threshold") + IntToStr(threshold)  );
    //MODebug2->Push( moText("threshold_max") + IntToStr(threshold_max)  );

    int test = m_threshold + 10;

    //MODebug2->Push( moText("test") + IntToStr(test)  );

	actual = Events->First;
	//Buscamos los eventos del VideoManager...samples... obsoleto, seran datos de Outlets e Inlets...
	//Outlet de videomanager LIVEOUT0 a inlet de tracker
	///Updating OpenCV Outlets
	while(actual!=NULL) {
		//solo nos interesan los del VideoManager por ahora
		if(actual->deviceid == m_pResourceManager->GetVideoMan()->GetId() && 1==2 ) {

			pSample = (moVideoSample*)actual->pointer;
			pBucket = (moBucket*)pSample->m_pSampleBuffer;

			//atencion! el devicecode corresponde al  iesimo dispositivo de captura...
			if (  pSample && pBucket ) {

                int m_sizebuffer = pBucket->GetSize();
                MOubyte * m_buffer = pBucket->GetBuffer();

			    //(MOpointer)pTS->GetData();

			    if (m_pIplImage) {
              cvReleaseImage( &m_pIplImage );
          }

			    m_pIplImage = cvCreateImage( cvSize( pSample->m_VideoFormat.m_Width, pSample->m_VideoFormat.m_Height), IPL_DEPTH_8U, 3 );

			    //img = cvCreateImageHeader( cvSize( pSample->m_VideoFormat.m_Width, pSample->m_VideoFormat.m_Height), IPL_DEPTH_8U, 3 );
			    //cvInitImageHeader( img, cvSize( pSample->m_VideoFormat.m_Width, pSample->m_VideoFormat.m_Height), IPL_DEPTH_8U, 3, IPL_ORIGIN_TL);

                cvZero( m_pIplImage );
                ///cvSet( img, cvScalar( 255, 0, 0 ),0);
                //img->imageData = (char*)pSample->m_pSampleBuffer;
/*
                MODebug2->Push(moText("moOpenFace::Update:: videosample buffersize") + IntToStr(pSample->m_VideoFormat.m_BufferSize));
                int cc = 0;
                uchar * ddata = (uchar *)img->imageData;
                uchar * mdata = (uchar *) m_buffer;
                for( int jj=0; jj<pSample->m_VideoFormat.m_Height; jj++) {
                    for( int ii=0; ii<pSample->m_VideoFormat.m_Width; ii++) {
                        //cvSet2D( img, ii, jj, cvScalar(mdata[cc],mdata[cc+1],mdata[cc+2]) );
                        //cvSet2D( img, ii, jj, cvScalar(0,255,255) );
                        ddata[cc] = mdata[cc];
                        ddata[cc+1] = mdata[cc+1];
                        mdata[cc+2] = mdata[cc+2];
                        cc+=3;
                    }
                }
*/
                cvSetData( m_pIplImage, (void*)m_buffer, pSample->m_VideoFormat.m_Width*3);
//                (bh.biWdith*3 + 3) & -4

                if (idopencvout==-1) {
                    ///create texture in moldeo to see it
                    moTexParam tparam = MODefTex2DParams;
                    tparam.internal_format = GL_RGB;
                    CvSize size = cvGetSize(m_pIplImage);
                    idopencvout = m_pResourceManager->GetTextureMan()->AddTexture( moText("opencvout"), size.width, size.height, tparam );
                } else {

                    IplImage* img_gray = cvCreateImage( cvGetSize(m_pIplImage), 8, 1 );
                    cvCvtColor( m_pIplImage, img_gray, CV_BGR2GRAY );
                    //cvSobel( img_gray, img_gray, 1, 1, 3);
                    double highthresh;
                    highthresh = m_threshold;
                    int aperturesize;
                    aperturesize = (int)m_threshold_max;

                    //aperturesize = threshold * threshold_max;
                    double t = 150;

                    t = (double) m_threshold;

                    cvCanny( img_gray, img_gray, 0, t, 3 );
                    //cvDilate( img_gray, img_gray, 0, 1 );
                    //cvFindContours( img_gray, storage, &contours, sizeof(CvContour),CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
                    //cvDrawContours( img_gray, contours, CV_RGB(255,0,0), CV_RGB(0,255,0), 0, 3, CV_AA, cvPoint(0,0) );

                    cvCvtColor( img_gray, m_pIplImage, CV_GRAY2BGR );



                    ///FINDING SQUARES!!!
                    //CvSeq* polys = findSquares4( img, storage );
                    //drawSquares( img, polys );
                    ///END

                    /*
                    IplImage* img_gray = cvCreateImage( cvGetSize(img), 8, 1 );
                    cvCvtColor( img, img_gray, CV_BGR2GRAY );
                    //cvThreshold( img_gray, img_gray, 100, 255, CV_THRESH_BINARY );
                    cvCvtColor( img_gray, img, CV_BGR2GRAY );
*/
                    // object that will contain blobs of inputImage
                    /* // BlobsLib 0.8
                        CBlobResult blobs;
                        IplImage* img_gray = cvCreateImage( cvGetSize(img), 8, 1 );
                        cvCvtColor( img, img_gray, CV_BGR2GRAY );

                        //IplImage *imgThr;
                        //cvThreshold( img_gray, imgThr, 100, 255, CV_THRESH_BINARY );

                        // Extract the blobs using a threshold of 100 in the image
                        blobs = CBlobResult( img_gray, NULL, 100 );

                        // create a file with some of the extracted features
                        //blobs.PrintBlobs( "c:\\tmp\\blobs.txt" );

                        // discard the blobs with less area than 5000 pixels
                        // ( the criteria to filter can be any class derived from COperadorBlob )
                        blobs.Filter( blobs, B_INCLUDE, CBlobGetArea(), B_GREATER, 100 );
                        // create a file with filtered results
                        //blobs.PrintBlobs( "c:\\tmp\\filteredBlobs.txt" );

                        // object with the blob with most perimeter in the image
                        // ( the criteria to select can be any class derived from COperadorBlob )
                        CBlob blobWithBiggestPerimeter;
                        CBlob blobWithLessArea;

                        // from the filtered blobs, get the blob with biggest perimeter
                        blobs.GetNthBlob( CBlobGetPerimeter(), 0, blobWithBiggestPerimeter );
                        // get the blob with less area
                        blobs.GetNthBlob( CBlobGetArea(), blobs.GetNumBlobs() - 1, blobWithLessArea );

                        CBlob* currentBlob;

                        for (int i = 0; i < blobs.GetNumBlobs(); i++ )
                        {
                                currentBlob = blobs.GetBlob(i);
                                currentBlob->FillBlob( img, cvScalar(255,0,0));
                        }
                        */




                        /// cvblob
                        /*

                        CvSize size = cvGetSize(img);

                        cvThreshold(img, img, 100, 200, CV_THRESH_BINARY);

                        cvSetImageROI(img, cvRect(0, 0, size.width, size.height));

                        IplImage *chB=cvCreateImage(cvGetSize(img),8,1);

                        cvSplit(img,chB,NULL,NULL,NULL);

                        IplImage *labelImg = cvCreateImage(cvGetSize(chB),IPL_DEPTH_LABEL,1);

                        CvBlobs blobs;
                        unsigned int result = cvLabel(chB, labelImg, blobs);

                        cvRenderBlobs( labelImg, blobs, img, img);
                        */

                        /*

                        for (CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
                          {
                            CvContourChainCode *contour = cvGetContour(it->second, labelImg);
                            cvRenderContourChainCode(contour, img);

                            CvContourPolygon *polygon = cvConvertChainCodesToPolygon(contour);

                            CvContourPolygon *sPolygon = cvSimplifyPolygon(polygon, 10.);
                            CvContourPolygon *cPolygon = cvPolygonContourConvexHull(sPolygon);

                            cvRenderContourPolygon(sPolygon, img, CV_RGB(0, 0, 255));
                            cvRenderContourPolygon(cPolygon, img, CV_RGB(0, 255, 0));

                            delete cPolygon;
                            delete sPolygon;
                            delete polygon;
                            delete contour;
                          }
*/





/*
                    IplImage *imgThr;
                    CBlob* currentBlob;
                    int param2 = 20;
                    CBlobResult blobs;
                    cvThreshold( img, imgThr, 100, 255, CV_THRESH_BINARY );
                    // find non-white blobs in thresholded image
                    blobs = CBlobResult( imgThr, NULL, 255 );
                    // exclude the ones smaller than param2 value
                    blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, param2 );
                    // get mean gray color of biggest blob
                    CBlob biggestBlob;
                    CBlobGetMean getMeanColor( img );
                    double meanGray;

                    blobs.GetNthBlob( CBlobGetArea(), 0, biggestBlob );
                    meanGray = getMeanColor( biggestBlob );

                    // display filtered blobs
                    cvMerge( imgThr, imgThr, imgThr, NULL, img );

                    for (int i = 0; i < blobs.GetNumBlobs(); i++ )
                    {
                            currentBlob = blobs.GetBlob(i);
                            currentBlob->FillBlob( img, CV_RGB(255,0,0));
                    }
                    //FIN BLOB
                    */

                    /*

                    ///Convert to gray
                    IplImage* img_gray = cvCreateImage( cvGetSize(img), 8, 1 );
                    cvCvtColor( img, img_gray, CV_BGR2GRAY );

                    //IplImage* img_gray_dst = cvCreateImage( cvGetSize(img_gray), 8, 1 );
                    //IplImage* img_gray_dst = cvCreateImage( cvGetSize(img_gray), 8, 1 );
                    IplImage* img_gray_dst = cvCloneImage( img_gray );
                    //cvEqualizeHist( img_gray, img_gray_dst );
                    //cvThreshold( img_gray, img_gray_dst, 128, 255, CV_THRESH_BINARY );

                    //cvFindContours( img_gray_dst, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );




                    ///convert the gray to rgb gray

                    IplImage* img_grayrgb = cvCreateImage( cvGetSize(img_gray_dst), 8, 3 );
                    cvCvtColor( img_gray_dst, img_grayrgb, CV_GRAY2BGR );

                    //cvDrawContours( img_grayrgb, contours, CV_RGB(255,0,0), CV_RGB(0,255,0), 0, 3, CV_AA, cvPoint(0,0) );
                    */

                    ///now set the moldeo texture buffer with the data
                    void* data;
                    int step;
                    CvSize size2;


                    //cvGetRawData( img_grayrgb, (uchar**)&data, &step, &size );
                    cvGetRawData( m_pIplImage, (uchar**)&data, &step, &size2 );
                    //cvGetRawData( img, (uchar**)&data, &step, &size );

                    //MODebug2->Push(moText("moOpenFace::Update:: step from img:") + IntToStr(step));


                    ///get the moldeo texture and set it
                    moTexture* pTex;
                    pTex = m_pResourceManager->GetTextureMan()->GetTexture(idopencvout);

                    //pTex->SetBuffer( data, GL_BGR, GL_UNSIGNED_BYTE );
                    pTex->SetBuffer( data, GL_RGB, GL_UNSIGNED_BYTE );
                }

			    //

                /*
			    MODebug2->Push(moText("moOpenFace::Update "));
			    MODebug2->Push(
                                moText(" width: ")
                                + IntToStr(pSample->m_VideoFormat.m_Width)
                                + moText(" height: ")
                                + IntToStr(pSample->m_VideoFormat.m_Height)
                            );
                */
			    //cvFindContours( img, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
			    //if (contours->




                //m_Outlets[actual->devicecode]->GetData()->SetPointer( NULL, sizeof(moTrackerSystemData) );
                //m_Outlets[actual->devicecode]->Update();

			}
			tmp = actual->next;
			actual = tmp;
		} else actual = actual->next;//no es el que necesitamos...
	}


	moMoldeoObject::Update(Events);
}


MOboolean moOpenFace::Finish() {
	return true;
}


cv::Mat moOpenFace::processImage(cv::Mat& image)
{
  cv::Mat resized;
  cv::Size originalSize = image.size();

  if(m_keepAspectRatio)
  {
    float ratio = static_cast<float>(m_goalSize.height) / originalSize.height;
    cv::Size newSize((int) (originalSize.width * ratio), (int) (originalSize.height * ratio));

    //fix possible rounding error by float
    if(newSize.height != m_goalSize.height) newSize.height = m_goalSize.height;

    cv::resize(image, resized, newSize);

    if(resized.size().width != m_goalSize.width)
    {
      if(m_keepAspectRatio)
      {
        int delta = m_goalSize.width - resized.size().width;

        if(delta < 0)
        {
          cv::Rect clipRect(std::abs(delta) / 2, 0, m_goalSize.width, resized.size().height);
          resized = resized(clipRect);
        }
        else if (delta > 0)
        {
          //width needs to be widened, create bigger mat, get region of
          //interest at the center that matches the size of the resized
          //image, and copy the resized image into that ROI

          cv::Mat widened( m_goalSize, resized.type());
          cv::Rect widenRect(delta / 2, 0, m_goalSize.width, m_goalSize.height);
          cv::Mat widenedCenter = widened(widenRect);
          resized.copyTo(widenedCenter);
          resized = widened; //we return resized, so set widened to resized
        }
      }
    }
  }
  else
    cv::resize(image, resized, m_goalSize);

  //cv::Mat grayFrame;
  //cv::cvtColor(resized, grayFrame, CV_BGR2GRAY);
  //return grayFrame;
  return resized;
}

bool moOpenFace::processAndSaveImage(cv::Mat& image, const std::string& name)
{
  cv::Mat processed = processImage(image);
  moText filepath = m_pResourceManager->GetDataMan()->GetDataPath() + moSlash + m_savingImagesPath;
  if (!moFileManager::DirectoryExists(filepath)) moFileManager::CreateDirectory(filepath);
  filepath+= moSlash + moText( name.c_str() );
  MODebug2->Message("moOpenFace::processAndSaveImage > to:" + filepath );

  return cv::imwrite( (char*)filepath, processed);

}

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double angle( CvPoint* pt1, CvPoint* pt2, CvPoint* pt0 )
{
    double dx1 = pt1->x - pt0->x;
    double dy1 = pt1->y - pt0->y;
    double dx2 = pt2->x - pt0->x;
    double dy2 = pt2->y - pt0->y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

CvSeq* moOpenFace::findSquares4( IplImage* img, CvMemStorage* storage ) {
    CvSeq* contours;
    int thresh = 50;
    int i, c, l, N = 1;
    CvSize sz = cvSize( img->width & -2, img->height & -2 );
    //IplImage* timg = cvCloneImage( img ); // make a copy of input image
    IplImage* timg = img;
    IplImage* gray = cvCreateImage( sz, 8, 1 );
    IplImage* pyr = cvCreateImage( cvSize(sz.width/2, sz.height/2), 8, 1 );
    IplImage* tgray;
    CvSeq* result;
    double s, t;
    // create empty sequence that will contain points -
    // 4 points per square (the square's vertices)
    CvSeq* squares = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint), storage );

    // select the maximum ROI in the image
    // with the width and height divisible by 2
    cvSetImageROI( timg, cvRect( 0, 0, sz.width, sz.height ));

    // down-scale and upscale the image to filter out the noise

    tgray = cvCreateImage( sz, 8, 1 );
    cvCvtColor( timg, tgray, CV_BGR2GRAY );

    cvPyrDown( tgray, pyr, CV_GAUSSIAN_5x5 );
    cvPyrUp( pyr, tgray, CV_GAUSSIAN_5x5 );

    // find squares in every color plane of the image
    //for( c = 0; c < 3; c++ )
    //{
        // extract the c-th color plane
        //cvSetImageCOI( timg, c+1 );
        //cvCopy( timg, tgray, 0 );

        // try several threshold levels
        for( l = 0; l < N; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                cvCanny( tgray, gray, 0, thresh, 5 );
                // dilate canny output to remove potential
                // holes between edge segments
                cvDilate( gray, gray, 0, 1 );
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                cvThreshold( tgray, gray, (l+1)*255/N, 255, CV_THRESH_BINARY );
            }

            // find contours and store them all as a list
            cvFindContours( gray, storage, &contours, sizeof(CvContour),
                CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );

            // test each contour
            while( contours )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                result = cvApproxPoly( contours, sizeof(CvContour), storage,
                    CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0 );
                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if( result->total == 4 &&
                    fabs(cvContourArea(result,CV_WHOLE_SEQ)) > 100 &&
                    cvCheckContourConvexity(result) )
                {
                    s = 0;

                    for( i = 0; i < 5; i++ )
                    {
                        // find minimum angle between joint
                        // edges (maximum of cosine)
                        if( i >= 2 )
                        {
                            t = fabs(angle(
                            (CvPoint*)cvGetSeqElem( result, i ),
                            (CvPoint*)cvGetSeqElem( result, i-2 ),
                            (CvPoint*)cvGetSeqElem( result, i-1 )));
                            s = s > t ? s : t;
                        }
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if( s < 0.3 )
                        for( i = 0; i < 4; i++ )
                            cvSeqPush( squares,
                                (CvPoint*)cvGetSeqElem( result, i ));
                }

                // take the next contour
                contours = contours->h_next;
            }
        }
    //}

    // release all the temporary images
    cvReleaseImage( &gray );
    //cvReleaseImage( &pyr );
    //cvReleaseImage( &tgray );
    //cvReleaseImage( &timg );

    return squares;
}


void moOpenFace::drawSquares( IplImage* cpy, CvSeq* squares ) {
 CvSeqReader reader;

    int i;

    // initialize reader of the sequence
    cvStartReadSeq( squares, &reader, 0 );

    // read 4 sequence elements at a time (all vertices of a square)
    for( i = 0; i < squares->total; i += 4 )
    {
        CvPoint pt[4], *rect = pt;
        int count = 4;

        // read 4 vertices
        CV_READ_SEQ_ELEM( pt[0], reader );
        CV_READ_SEQ_ELEM( pt[1], reader );
        CV_READ_SEQ_ELEM( pt[2], reader );
        CV_READ_SEQ_ELEM( pt[3], reader );

        // draw the square as a closed polyline
        cvPolyLine( cpy, &rect, &count, 1, 1, CV_RGB(0,255,0), 3, CV_AA, 0 );
    }

}



//===========================================
//
//     Class: moOpenFaceSystem
//
//===========================================



moOpenFaceSystem::moOpenFaceSystem() {

}

moOpenFaceSystem::~moOpenFaceSystem() {

}
