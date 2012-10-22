
//#include "moKinectUserGenerator.h"
//#include "moDebugManager.h"

#include <map>
std::map<XnUInt32, std::pair<XnCalibrationStatus, XnPoseDetectionStatus> > m_Errors;



void MyCalibrationInProgress(xn::SkeletonCapability& capability, XnUserID id, XnCalibrationStatus calibrationError, void* pCookie)
{
	m_Errors[id].first = calibrationError;
}
void MyPoseInProgress(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID id, XnPoseDetectionStatus poseError, void* pCookie)
{
	m_Errors[id].second = poseError;
}



// Callback: New user was detected
void User_NewUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	//printf("New User %d\n", nId);

	moAbstract::MODebug2->Push( moText("New user:") + IntToStr(nId));

	// New user found
	if (m_bNeedPose)
	{
		m_UserGenerator.GetPoseDetectionCap().StartPoseDetection(m_strPose, nId);
	}
	else
	{
		m_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
	}
}


// Callback: An existing user was lost
void User_LostUser(xn::UserGenerator& generator, XnUserID nId, void* pCookie)
{
	moAbstract::MODebug2->Push( moText("Lost user:") + IntToStr(nId));
	//printf("Lost user %d\n", nId);
}


// Callback: Detected a pose
void UserPose_PoseDetected(xn::PoseDetectionCapability& capability, const XnChar* strPose, XnUserID nId, void* pCookie)
{
	//printf("Pose %s detected for user %d\n", strPose, nId);
	moAbstract::MODebug2->Push( moText("Pose ") + moText(strPose) + moText(" detected for user: ") + IntToStr(nId));

	m_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
	m_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}
// Callback: Started calibration
void UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie)
{
    moAbstract::MODebug2->Push( moText("Calibration started for user:") + IntToStr(nId));
	//printf("Calibration started for user %d\n", nId);
}
// Callback: Finished calibration
void UserCalibration_CalibrationEnd(xn::SkeletonCapability& capability, XnUserID nId, XnBool bSuccess, void* pCookie)
{
	if (bSuccess)
	{
		// Calibration succeeded
		moAbstract::MODebug2->Push( moText("Calibration complete, start tracking user:") + IntToStr(nId));
		//printf("Calibration complete, start tracking user %d\n", nId);
		m_UserGenerator.GetSkeletonCap().StartTracking(nId);
	}
	else
	{
		// Calibration failed
		//printf("Calibration failed for user %d\n", nId);
		moAbstract::MODebug2->Push( moText("Calibration failed for user:") + IntToStr(nId));
		if (m_bNeedPose)
		{
			m_UserGenerator.GetPoseDetectionCap().StartPoseDetection(m_strPose, nId);
		}
		else
		{
			m_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
		}
	}
}

void UserCalibration_CalibrationComplete(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus, void* pCookie)
{
	if (eStatus == XN_CALIBRATION_STATUS_OK)
	{
		// Calibration succeeded
		//printf("Calibration complete, start tracking user %d\n", nId);
		moAbstract::MODebug2->Push( moText("Calibration complete, start tracking user:") + IntToStr(nId));
		m_UserGenerator.GetSkeletonCap().StartTracking(nId);
	}
	else
	{
		// Calibration failed
		//printf("Calibration failed for user %d\n", nId);
		moAbstract::MODebug2->Push( moText("Calibration failed for user:") + IntToStr(nId));
		if (m_bNeedPose)
		{
			m_UserGenerator.GetPoseDetectionCap().StartPoseDetection(m_strPose, nId);
		}
		else
		{
			m_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
		}
	}
}

// Save calibration to file
void SaveCalibration()
{
	XnUserID aUserIDs[20] = {0};
	XnUInt16 nUsers = 20;
	m_UserGenerator.GetUsers(aUserIDs, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
		// Find a user who is already calibrated
		if (m_UserGenerator.GetSkeletonCap().IsCalibrated(aUserIDs[i]))
		{
			// Save user's calibration to file
			m_UserGenerator.GetSkeletonCap().SaveCalibrationDataToFile(aUserIDs[i], XN_CALIBRATION_FILE_NAME);
			break;
		}
	}
}
// Load calibration from file
void LoadCalibration()
{
	XnUserID aUserIDs[20] = {0};
	XnUInt16 nUsers = 20;
	m_UserGenerator.GetUsers(aUserIDs, nUsers);
	for (int i = 0; i < nUsers; ++i)
	{
		// Find a user who isn't calibrated or currently in pose
		if (m_UserGenerator.GetSkeletonCap().IsCalibrated(aUserIDs[i])) continue;
		if (m_UserGenerator.GetSkeletonCap().IsCalibrating(aUserIDs[i])) continue;

		// Load user's calibration from file
		XnStatus rc = m_UserGenerator.GetSkeletonCap().LoadCalibrationDataFromFile(aUserIDs[i], XN_CALIBRATION_FILE_NAME);
		if (rc == XN_STATUS_OK)
		{
			// Make sure state is coherent
			m_UserGenerator.GetPoseDetectionCap().StopPoseDetection(aUserIDs[i]);
			m_UserGenerator.GetSkeletonCap().StartTracking(aUserIDs[i]);
		}
		break;
	}
}
