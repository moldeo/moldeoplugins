

#ifndef __KINECT_USER_GENERATOR
#define __KINECT_USER_GENERATOR

/*C wrapper*/
#include "XnOpenNI.h"
/*C** wrapper*/
#include <XnCodecIDs.h>
#include "XnCppWrapper.h"


    XnStatus m_nRetVal;
    xn::UserGenerator m_UserGenerator;
    XnCallbackHandle hUserCallbacks, hCalibrationStart, hCalibrationComplete, hPoseDetected, hCalibrationInProgress, hPoseInProgress;
    bool m_bNeedPose;
    XnChar m_strPose[20];

    void XN_CALLBACK_TYPE MyCalibrationInProgress(xn::SkeletonCapability& capability, XnUserID id, XnCalibrationStatus calibrationError, void* pCookie);
    void XN_CALLBACK_TYPE MyPoseInProgress(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID id, XnPoseDetectionStatus poseError, void* pCookie);
    // Callback: New user was detected
    void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie);
    // Callback: An existing user was lost
    void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie);
    // Callback: Detected a pose
    void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie);
    // Callback: Started calibration
    void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie);
    // Callback: Finished calibration
    void XN_CALLBACK_TYPE UserCalibration_CalibrationEnd(xn::SkeletonCapability& capability, XnUserID nId, XnBool bSuccess, void* pCookie);
    void XN_CALLBACK_TYPE UserCalibration_CalibrationComplete(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus, void* pCookie);
    #define XN_CALIBRATION_FILE_NAME "UserCalibration.bin"
    // Save calibration to file
    void SaveCalibration();
    // Load calibration from file
    void LoadCalibration();


#endif
