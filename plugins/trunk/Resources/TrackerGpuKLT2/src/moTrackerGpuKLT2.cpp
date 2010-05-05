/*******************************************************************************

                             moTrackerGpuKLT2.cpp

  ****************************************************************************
  *                                                                          *
  *   This source is free software; you can redistribute it and/or modify    *
  *   it under the terms of the GNU General Public License as published by   *
  *   the Free Software Foundation; either version 2 of the License, or      *
  *  (at your option) any later version.                                     *
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

  Copyright(C) 2008 Fabricio Costa

  Authors:
    Copyright (c) 2008 University of North Carolina at Chapel Hill


*******************************************************************************/

#include "moTrackerGpuKLT2.h"

using namespace V3D_GPU;
//========================
//  Factory
//========================

moTrackerGpuKLT2Factory *m_TrackerGpuKLT2Factory = NULL;

MO_PLG_API moResourceFactory* CreateResourceFactory(){
	if (m_TrackerGpuKLT2Factory==NULL)
		m_TrackerGpuKLT2Factory = new moTrackerGpuKLT2Factory();
	return (moResourceFactory*) m_TrackerGpuKLT2Factory;
}

MO_PLG_API void DestroyResourceFactory(){
	delete m_TrackerGpuKLT2Factory;
	m_TrackerGpuKLT2Factory = NULL;
}

moResource*  moTrackerGpuKLT2Factory::Create() {
	return new moTrackerGpuKLT2();
}

void moTrackerGpuKLT2Factory::Destroy(moResource* fx) {
	delete fx;
}

//===========================================
//
//     Class: moTrackerGpuKLT2System
//
//===========================================

moTrackerGpuKLT2System::moTrackerGpuKLT2System()
{
//	gpuComputor = NULL;
	m_init = false;
	m_TrackerSystemData.features = NULL;
	m_TrackerSystemData.tracks = NULL;
	m_TrackerSystemData.nDetectedFeatures = 0;
	m_TrackerSystemData.nNewFeatures = 0;
	m_FrameCount = 0;
	m_TrackCount = 0;
    tracker = NULL;

//S	list = NULL;
}

moTrackerGpuKLT2System::~moTrackerGpuKLT2System()
{
	Finish();
}

MOboolean moTrackerGpuKLT2System::Init(MOint p_nFeatures, MOint p_width, MOint p_height,
					moText p_shaders_dir, MOint p_arch,
					MOint p_kltmindist, MOfloat p_klteigenthreshold)
{
	if (p_width <= 0 || p_height == 0) return false;

    //OLD
	//strcpy(gopt._CGdirectory, p_shaders_dir);

    //OLD
	//gopt._enableTiming = false;
	//gopt._verbose = false;

	m_width = p_width;
	m_height = p_height;

	m_ReinjectRate = 1;  /// 1  esto es trackedframes....!!!!

	// Store info for so many video frames in GPU memory. num levels in pyramid.
    //OLD
	/*
	gopt._nFrames = 2; // 2
	gopt._nLevels = 4; // 4

	// Pixel range in which to search for features while tracking.
	gopt._klt_search_range = 100;// 25 RUEDA: 11

	gopt._kltwin            = 64;  // KLT Window Size 7 RUEDA:10
	gopt._kltborder         = 8; // Window Border Size to be avoided. 32 RUEDA:8
	gopt._kltnumfeats       = p_nFeatures; // Number of features.
	gopt._klteigenthreshold = p_klteigenthreshold; // Eigen Value threshold for Cornerness (pixel brightness is between 0 to 1).
	gopt._kltmindist        = p_kltmindist; // Minimum Distance between KLT features (in pixels).
	gopt._kltiterations     = 5;    // Number of track iterations within the same klt_pyramid level. 5
	gopt._kltSSDthresh      = 0.05; // KLT SSD Threshold (pixel brightness varies between 0 and 1). 0.05
	gopt._kltConvthresh     = 0.5;  // KLT Min distance convergence threshold (in pixels). 0.5

	if (p_arch == MO_GPU_NV) gopt._arch = GPU_NVIDIA;
	else if (p_arch == MO_GPU_NV) gopt._arch = GPU_ATI;

	gopt._enableDebug = false;

    gpuComputor = new GpuVis(gopt); // Create GpuVis computor object
*/
    if (m_TrackerSystemData.features) {
        delete m_TrackerSystemData.features;
        m_TrackerSystemData.features = NULL;
    }
    if (m_TrackerSystemData.tracks) {
        delete m_TrackerSystemData.tracks;
        m_TrackerSystemData.tracks = NULL;
    }

    ///with and height must be integers, approximate by square root
    featuresWidth = sqrt( p_nFeatures );
    featuresHeight = featuresWidth;
    ///recalculate nfeatures
    p_nFeatures = featuresWidth*featuresHeight;
    m_TrackerSystemData.nFeatures = p_nFeatures;

    m_TrackerSystemData.features = new KLT_TrackedFeature[m_TrackerSystemData.nFeatures];
    m_TrackerSystemData.tracks = new PointTrack[m_TrackerSystemData.nFeatures];

    cfg.nIterations = 10;//10
    cfg.nLevels     = 4;
    cfg.levelSkip   = 1;
    cfg.trackBorderMargin    = 10.0f;
    cfg.convergenceThreshold = 0.1f;//0.1
    cfg.SSD_Threshold        = 1000.0f;//1000
    cfg.trackWithGain        = true;//true

    cfg.minDistance   = 10;//10
    cfg.minCornerness = 5.0f;//50
    cfg.detectBorderMargin = cfg.trackBorderMargin;



    tracker = new KLT_SequenceTracker(cfg);
    tracker->allocate( m_TrackerSystemData.GetVideoFormat().m_Width, m_TrackerSystemData.GetVideoFormat().m_Height, cfg.nLevels, featuresWidth, featuresHeight);

	m_TrackCount = 0;
	m_FrameCount = 0;
	m_init = true;

	return true;
}

MOboolean moTrackerGpuKLT2System::Init(MOint p_nFeatures, moVideoSample* p_pVideoSample,
					moText p_shaders_dir, MOint p_arch,
					MOint p_kltmindist, MOfloat p_klteigenthreshold)
{
	if ( p_pVideoSample != NULL )
		m_TrackerSystemData.GetVideoFormat() = p_pVideoSample->m_VideoFormat;
	else return false;

	if ( m_TrackerSystemData.GetVideoFormat().m_Width<=0 || m_TrackerSystemData.GetVideoFormat().m_Height==0)
		return false;

	MOboolean res = Init(p_nFeatures, m_TrackerSystemData.GetVideoFormat().m_Width, m_TrackerSystemData.GetVideoFormat().m_Height,
						p_shaders_dir, p_arch,
						p_kltmindist, p_klteigenthreshold);

	if (res)
	{
//OLD
//		m_TrackerSystemData.m_NFeatures = p_nFeatures;
//		m_TrackerSystemData.m_FeatureList = list;
		return true;
	}
	else return false;
}

MOboolean moTrackerGpuKLT2System::Finish()
{
    //OLD
/*
	if (gpuComputor != NULL)
	{
		delete gpuComputor;
		gpuComputor = NULL;
	}
	*/
	return true;
}

void moTrackerGpuKLT2System::GetFeature(MOint p_feature, MOfloat &x, MOfloat &y, MOboolean &v)
{
//OLD
/*
	if (p_feature < list->_nFeats)
	{
		if (list->_list[p_feature]->valid)
		{
			x = list->_list[p_feature]->x;
			y = list->_list[p_feature]->y;
			v = list->_list[p_feature]->valid;
		}
		else
		{
			x = y = 0;
			v = false;
		}
	}
	*/
}

void moTrackerGpuKLT2System::Track(GLubyte *p_pBuffer, MOuint p_RGB_mode)
{

	if (m_FrameCount == 0) {
        FirstTracking(p_pBuffer, p_RGB_mode);
    } else if ( (m_TrackCount % m_ReinjectRate) == 0) {
		StartTracking(p_pBuffer, p_RGB_mode);
	} else {
	    ContinueTracking(p_pBuffer, p_RGB_mode);
    }

    m_FrameCount++;
}

void moTrackerGpuKLT2System::FirstTracking(GLubyte *p_pBuffer, MOuint p_RGB_mode)
{
//OLD
//	image.setPointer(p_pBuffer, m_width, m_height);

//	glDisable(GL_DEPTH_TEST);
//	glDisable(GL_TEXTURE_2D);
//	glDisable(GL_BLEND);

//OLD
/*	gpuComputor->Init(image);
	gpuComputor->computePyramid(image);
	list = gpuComputor->selectGoodFeatures();
	m_TrackerSystemData.m_FeatureList = list;
*/
    ///if (frameNo == 0)
    tracker->detect( (V3D_GPU::uchar *)p_pBuffer, m_TrackerSystemData.nDetectedFeatures, m_TrackerSystemData.features);

	m_TrackCount = 1;
}

void moTrackerGpuKLT2System::StartTracking(GLubyte *p_pBuffer, MOuint p_RGB_mode)
{
//OLD
//	image.setPointer(p_pBuffer, m_width, m_height);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

//OLD
/*	gpuComputor->Init(image);
	gpuComputor->computePyramid(image);
	list = gpuComputor->selectGoodFeatures();
	m_TrackerSystemData.m_FeatureList = list;
*/
    ///if (relFrameNo == 0)
    tracker->redetect((V3D_GPU::uchar *)p_pBuffer, m_TrackerSystemData.nNewFeatures, m_TrackerSystemData.features);

	m_TrackCount = 1;
}

void moTrackerGpuKLT2System::ContinueTracking(GLubyte *p_pBuffer, MOuint p_RGB_mode)
{
	int addedCount;

//OLD
/*	image.setPointer(p_pBuffer, m_width, m_height);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	for(int i=0; i<list->_nFeats; i++) {
		list->_list[i]->tr_x = list->_list[i]->x;
		list->_list[i]->tr_y = list->_list[i]->y;
	}

	gpuComputor->computePyramid(image);
	gpuComputor->trackFeatures(&list);

	if (!(m_TrackCount % m_ReinjectRate))
		addedCount = gpuComputor->reselectGoodFeatures(&list, 1, 0.9); // Re-select in full image.

	gpuComputor->uploadFeaturesToGPU(&list);
	m_TrackerSystemData.m_FeatureList = list;
*/
    ///if (relFrameNo > 0)
    tracker->track((V3D_GPU::uchar *)p_pBuffer, m_TrackerSystemData.nPresentFeatures, m_TrackerSystemData.features);
	m_TrackCount++;
}

void moTrackerGpuKLT2System::NewData( moVideoSample* p_pVideoSample )
{
	MOint m_sizebuffer;
	MOpointer m_buffer;

	if (p_pVideoSample)
	{
		moBucket* pBucket = (moBucket*)p_pVideoSample->m_pSampleBuffer;
		if (pBucket)
		{
			m_sizebuffer = pBucket->GetSize();
			m_buffer = pBucket->GetBuffer();
		}
		else return;
	}
	else return;

	if (m_buffer == NULL) return;

	Track(m_buffer, GL_RGB);


	for(int i=0; i<m_TrackerSystemData.GetFeatures().Count(); i++ ) {
	    delete m_TrackerSystemData.GetFeatures().Get(i);
	}

	m_TrackerSystemData.GetFeatures().Empty();
	moTrackerFeature* TF = NULL;

	float sumX = 0.0f,sumY = 0.0f;
	float sumN = 0.0f;

    int nTracks = 0;
    for (int i = 0; i < m_TrackerSystemData.nFeatures; ++i)
    {
     if (m_TrackerSystemData.features[i].status == 0)
     {
        m_TrackerSystemData.tracks[i].add(m_TrackerSystemData.features[i].pos[0], m_TrackerSystemData.features[i].pos[1]);
        ++nTracks;
     }
     else if (m_TrackerSystemData.features[i].status > 0)
     {
        m_TrackerSystemData.tracks[i].len = 1;
        m_TrackerSystemData.tracks[i].pos[0][0] = m_TrackerSystemData.features[i].pos[0];
        m_TrackerSystemData.tracks[i].pos[0][1] = m_TrackerSystemData.features[i].pos[1];
     }
     else
        m_TrackerSystemData.tracks[i].clear();

        TF = new moTrackerFeature();
        if (TF) {
            TF->x = m_TrackerSystemData.features[i].pos[0]*(float)m_width;
            TF->y = m_TrackerSystemData.features[i].pos[1]*(float)m_height;
            TF->tr_x = TF->x;
            TF->tr_y = TF->y;
            TF->val = m_TrackerSystemData.features[i].status;
            TF->valid = (m_TrackerSystemData.features[i].status >= 0);
            if (m_TrackerSystemData.features[i].status>0) {
                TF->x = m_TrackerSystemData.features[i].pos[0]*(float)m_width;
                TF->y = m_TrackerSystemData.features[i].pos[1]*(float)m_height;
                TF->tr_x = m_TrackerSystemData.features[i].pos[0]*(float)m_width;
                TF->tr_y = m_TrackerSystemData.features[i].pos[1]*(float)m_height;
            }
            //TF->tr_x = list->_list[i]->tr_x;
            //TF->tr_y = list->_list[i]->tr_y;
            TF->normx = m_TrackerSystemData.features[i].pos[0];
            TF->normy = m_TrackerSystemData.features[i].pos[1];
            m_TrackerSystemData.GetFeatures().Add(TF);

           sumX+= TF->x;
           sumY+= TF->y;
           sumN+= 1.0f;
        }

    } // end for (i)

    if (sumN>0.0f) m_TrackerSystemData.SetBarycenter( sumX/sumN, sumY/sumN );
    else  m_TrackerSystemData.SetBarycenter( 0, 0 );
    m_TrackerSystemData.SetValidFeatures( (int)sumN );
}

//===========================================
//
//     Class: moTrackerGpuKLT2
//
//===========================================

moTrackerGpuKLT2::moTrackerGpuKLT2() : moResource()
{
	SetResourceType(MO_RESOURCETYPE_FILTER);
	SetName("trackergpuklt2");
}

moTrackerGpuKLT2::~moTrackerGpuKLT2()
{
    Finish();
}

#include <stdlib.h>

MOboolean moTrackerGpuKLT2::Init()
{
    //MOuint port;
	//MOuint mode;
	//MOuint nvaluees;
    moText conf;
	MOint nvalues;
	MOint trackersystems;

	if (m_pResourceManager==NULL)
		return false;

    char vendor[80];
	strcpy(vendor, (const char*)glGetString(GL_VENDOR));

    if (strstr(vendor, "NVIDIA") != NULL) gpu_arch = MO_GPU_NV;
    else if (strstr(vendor, "ATI") != NULL) gpu_arch = MO_GPU_ATI;
    else if (strstr(vendor, "INTEL") != NULL) gpu_arch = MO_GPU_INTEL;
    else gpu_arch = MO_GPU_OTHER;

	//int i;

    // Loading config file.
	conf = m_pResourceManager->GetDataMan()->GetDataPath()+moSlash;
	conf += GetConfigName();
    conf += moText(".cfg");
	if (m_Config.LoadConfig(conf) != MO_CONFIG_OK ) {
		moText text = "Couldn't load trackergpuklt2 config:" + conf;
		MODebug2->Error(text);
		return false;
	}

	moMoldeoObject::Init();

	trackersystems = m_Config.GetParamIndex("trackersystems");

	num_feat = m_Config.GetParam(m_Config.GetParamIndex("num_feat")).GetValue().GetSubValue().Int();
	min_dist = m_Config.GetParam(m_Config.GetParamIndex("min_dist")).GetValue().GetSubValue().Float();
	threshold_eigen = m_Config.GetParam(m_Config.GetParamIndex("threshold_eigen")).GetValue().GetSubValue().Float();;
	klt_shaders_dir = m_Config.GetParam(m_Config.GetParamIndex("klt_shaders_dir")).GetValue().GetSubValue().Text();;
	klt_shaders_dir = (moText)m_pResourceManager->GetDataMan()->GetDataPath()+moText("/") + (moText)klt_shaders_dir;
	m_SampleRate = m_Config.GetParam(m_Config.GetParamIndex("sample_rate")).GetValue().GetSubValue().Int();
	num_frames = m_Config.GetParam(m_Config.GetParamIndex("num_frames")).GetValue().GetSubValue(0).Int();

    MODebug2->Message( moText("In moTrackerGpuKLT2::Init ***********************************************\n") );

    MODebug2->Message( moText("Initializing Tracker's System...\n"));

	//por cada camara inicializa un system:
	nvalues = m_Config.GetValuesCount( trackersystems );
	m_Config.SetCurrentParamIndex(trackersystems);
	/*
MO_TRACKER1D_SYTEM_LABELNAME	0
MO_TRACKER1D_LIVE_SYSTEM	1
MO_TRACKER1D_SYSTEM_ON 2
	*/


    moSetEnv( "V3D_SHADER_DIR", "./plugins/resources/trackergpuklt2/" );

    Cg_ProgramBase::initializeCg();

	for( int i = 0; i < nvalues; i++) {

		m_Config.SetCurrentValueIndex( trackersystems, i );

		moTrackerGpuKLT2SystemPtr pTSystem = NULL;

		pTSystem = new moTrackerGpuKLT2System();

		if (pTSystem!=NULL) {
			pTSystem->MODebug = MODebug;
			pTSystem->MODebug2 = MODebug2;
			pTSystem->SetName( m_Config.GetParam().GetValue().GetSubValue(MO_TRACKERGPUKLT_SYTEM_LABELNAME).Text() );
			pTSystem->SetLive( m_Config.GetParam().GetValue().GetSubValue(MO_TRACKERGPUKLT_LIVE_SYSTEM).Text() );
			pTSystem->SetActive( m_Config.GetParam().GetValue().GetSubValue(MO_TRACKERGPUKLT_SYSTEM_ON).Int() );
			MODebug2->Message( moText(" Tracker system: Name:") + pTSystem->GetName() + moText(" mLive:") + pTSystem->GetLive() );
		}

		m_TrackerSystems.Add( pTSystem );

	}
	return true;
}

MOboolean moTrackerGpuKLT2::Finish()
{
	moTrackerGpuKLT2SystemPtr pTSystem = NULL;
	for( int i = 0; i < m_TrackerSystems.Count(); i++) {
		pTSystem = m_TrackerSystems.Get(i);
		if (pTSystem!=NULL) {
			pTSystem->Finish();
			delete pTSystem;
		}
	}
	m_TrackerSystems.Empty();
	return true;
}

MOswitch moTrackerGpuKLT2::SetStatus(MOdevcode devcode, MOswitch state)
{
    return true;
}

void moTrackerGpuKLT2::SetValue( MOdevcode cd, MOfloat vl ) {}

void moTrackerGpuKLT2::SetValue( MOdevcode cd, MOint vl ) {}

MOswitch moTrackerGpuKLT2::GetStatus(MOdevcode devcode)
{
	if ( 0 <= devcode && devcode < m_TrackerSystems.Count() ) {
		return m_TrackerSystems.Get(devcode)->IsInit();
	} else if ( m_TrackerSystems.Count()<=devcode && devcode<(m_TrackerSystems.Count()*2) ) {
		return m_TrackerSystems.Get(devcode - m_TrackerSystems.Count())->IsInit();
	}
	return 0;
}

MOint moTrackerGpuKLT2::GetValue(MOdevcode devcode)
{
	moTrackerGpuKLT2SystemPtr pTS = NULL;

	if ( 0 <= devcode && devcode < m_TrackerSystems.Count() ) {
		return m_TrackerSystems.Get(devcode)->GetData()->GetFeaturesCount();
	} else if ( m_TrackerSystems.Count()<=devcode && devcode<(m_TrackerSystems.Count()*2) ) {
		return m_TrackerSystems.Get( devcode - m_TrackerSystems.Count() )->GetData()->GetVideoFormat().m_Width;
	}

    return(-1);
}

MOpointer moTrackerGpuKLT2::GetPointer(MOdevcode devcode ) {

	if ( m_TrackerSystems.Count()<=devcode && devcode<(m_TrackerSystems.Count()*2) ) {
		return (MOpointer)( m_TrackerSystems.Get(devcode-m_TrackerSystems.Count())->GetData() );
	}
	return (NULL);
}

MOdevcode moTrackerGpuKLT2::GetCode(moText strcod)
{
	moText TrackerSystemName;
	moText TrackerX;
	moText TrackerData;

	//example:
	//TRACKERKLT0_N		-> returns 0
	//TRACKERKLT1_N		-> returns 1
	//TRACKERKLT0_DATA		-> returns 2
	//TRACKERKLT1_DATA		-> returns 3

	for( int i = 0; i< m_TrackerSystems.Count(); i++) {

		TrackerSystemName = m_TrackerSystems.Get(i)->GetName();
		TrackerX = TrackerSystemName+"_N";
		TrackerData = TrackerSystemName+"_DATA";

		if ( !stricmp( strcod, TrackerX ) ) {

			return i;

		} else if (!stricmp( strcod, TrackerData )) {

			return ( i + m_TrackerSystems.Count() );

		}

	}

    return(-1);
}

void moTrackerGpuKLT2::Update(moEventList *Events)
{
	//get the pointer from moLive...

	moBucket* pBucket = NULL;
	moVideoSample* pSample = NULL;

//	MOuint i;
	moEvent *actual,*tmp;

	actual = Events->First;

	if (m_SampleCounter>=200000) m_SampleCounter = 0;
	m_SampleCounter++;

	while(actual!=NULL) {
		tmp = actual->next;
		if (actual->deviceid == MO_IODEVICE_TRACKER && actual->reservedvalue3!=MO_MESSAGE) {
			Events->Delete(actual);
		}
		actual = tmp;
	}

	actual = Events->First;
	//recorremos todos los events y parseamos el resultado
	//borrando aquellos que ya usamos
	while(actual!=NULL) {
		//solo nos interesan los del LIVE IODevice
		if(actual->deviceid == m_pResourceManager->GetVideoMan()->GetId() ) {

            //MODebug2->Push( moText("new sample received") );

			pSample = (moVideoSample*)actual->pointer;

			pBucket = (moBucket*)pSample->m_pSampleBuffer;

			//atencion! el devicecode corresponde al  iesimo dispositivo de captura...
			if ( actual->devicecode < m_TrackerSystems.Count() && pSample && pBucket && ( (m_SampleCounter % m_SampleRate)==0)) {
				moTrackerGpuKLT2SystemPtr pTS=NULL;
				pTS = m_TrackerSystems.Get( actual->devicecode );
				if ( pTS )
					if (pTS->IsActive() ) {
						if (!pTS->IsInit()) {
						    MOboolean res = false;
							res = pTS->Init(num_feat, pSample,
									  klt_shaders_dir, gpu_arch,
									  min_dist, threshold_eigen);
                            if (res) {
                                MODebug2->Message( "moTrackerGpuKLT2::Update TrackerGPUKltSystem initialized..." );
                            } else {
                                MODebug2->Error( "moTrackerGpuKLT2::Update Error Couldn't initialized effect" );
                            }
						}

						pTS->NewData( pSample );
/*
						//calcular pesos y otras yerbas
						moTrackerGpuKLT2SystemData* pTData;
						pTData = pTS->GetData();

						MOint i;

						MOint NF, TrackersX, TrackersY, Variance;

						MOfloat sumX,sumY;
						MOfloat meanX,meanY,disV;

						NF = 0;
						sumX = 0; meanX = 0;
						sumY = 0; meanY = 0;
						TrackersX = 0;
						TrackersY = 0;
						disV = 0;
						Variance = 0;
*/
//OLD
/*
						for( i=0; i<pTData->m_FeatureList->_nFeats; i++) {
							if (pTData->m_FeatureList->_list[i]->valid) {
								sumX+= pTData->m_FeatureList->_list[i]->x;
								sumY+= pTData->m_FeatureList->_list[i]->y;
								NF++;
							}
						}

						if (NF>0) {
							meanX = sumX / (float)NF;
							meanY = sumY / (float)NF;
							TrackersX = (int) meanX;
							TrackersY = (int) meanY;

							for( i=0; i<pTData->m_FeatureList->_nFeats; i++) {
								if (pTData->m_FeatureList->_list[i]->valid) {
									disV+= (meanX - pTData->m_FeatureList->_list[i]->x)*(meanX - pTData->m_FeatureList->_list[i]->x)
										+ (meanY - pTData->m_FeatureList->_list[i]->y)*(meanY - pTData->m_FeatureList->_list[i]->y);
								}
							}
							Variance =(int) ( (float)disV / (float)NF);
						}
*/
						//TrackersX = (TrackersX*320)/pSample->m_VideoFormat.m_Width;
						//TrackersY = (TrackersY*240)/pSample->m_VideoFormat.m_Height;


						//Events->Add( MO_IODEVICE_TRACKER, NF, TrackersX , TrackersY, Variance, 0 );

						m_Outlets[actual->devicecode]->GetData()->SetPointer( (MOpointer)pTS->GetData(), sizeof(moTrackerGpuKLT2SystemData) );
						m_Outlets[actual->devicecode]->Update();
					}
			}

			tmp = actual->next;
			actual = tmp;
		} else actual = actual->next;//no es el que necesitamos...
	}

	moMoldeoObject::Update(Events);

}
