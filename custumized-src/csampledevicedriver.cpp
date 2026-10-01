#include "csampledevicedriver.h"

#include "basics.h"

#include "textfile.h"
#include <stdlib.h>


#include <math.h>

using namespace vr;

//Head tracking vars
static double yaw = 0, pitch = 0, roll = 0;
static double pX = 0, pY = 0, pZ = 0;
static double t0, t1, t2, t3, t4, t5;

CSampleDeviceDriver::CSampleDeviceDriver()
{
    m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
    m_ulPropertyContainer = vr::k_ulInvalidPropertyContainer;

    //DriverLog( "Using settings values\n" );
    m_flIPD = vr::VRSettings()->GetFloat(k_pch_SteamVR_Section, k_pch_SteamVR_IPD_Float);

    char buf[1024];
    vr::VRSettings()->GetString(k_pch_Sample_Section, k_pch_Sample_SerialNumber_String, buf, sizeof(buf));
    m_sSerialNumber = buf;

    vr::VRSettings()->GetString(k_pch_Sample_Section, k_pch_Sample_ModelNumber_String, buf, sizeof(buf));
    m_sModelNumber = buf;

    m_nWindowX = vr::VRSettings()->GetInt32(k_pch_Sample_Section, k_pch_Sample_WindowX_Int32);
    m_nWindowY = vr::VRSettings()->GetInt32(k_pch_Sample_Section, k_pch_Sample_WindowY_Int32);
    m_nWindowWidth = vr::VRSettings()->GetInt32(k_pch_Sample_Section, k_pch_Sample_WindowWidth_Int32);
    m_nWindowHeight = vr::VRSettings()->GetInt32(k_pch_Sample_Section, k_pch_Sample_WindowHeight_Int32);
    m_nRenderWidth = vr::VRSettings()->GetInt32(k_pch_Sample_Section, k_pch_Sample_RenderWidth_Int32);
    m_nRenderHeight = vr::VRSettings()->GetInt32(k_pch_Sample_Section, k_pch_Sample_RenderHeight_Int32);
    m_flSecondsFromVsyncToPhotons = vr::VRSettings()->GetFloat(k_pch_Sample_Section, k_pch_Sample_SecondsFromVsyncToPhotons_Float);
    m_flDisplayFrequency = vr::VRSettings()->GetFloat(k_pch_Sample_Section, k_pch_Sample_DisplayFrequency_Float);

    /*DriverLog( "driver_null: Serial Number: %s\n", m_sSerialNumber.c_str() );
        DriverLog( "driver_null: Model Number: %s\n", m_sModelNumber.c_str() );
        DriverLog( "driver_null: Window: %d %d %d %d\n", m_nWindowX, m_nWindowY, m_nWindowWidth, m_nWindowHeight );
        DriverLog( "driver_null: Render Target: %d %d\n", m_nRenderWidth, m_nRenderHeight );
        DriverLog( "driver_null: Seconds from Vsync to Photons: %f\n", m_flSecondsFromVsyncToPhotons );
        DriverLog( "driver_null: Display Frequency: %f\n", m_flDisplayFrequency );
        DriverLog( "driver_null: IPD: %f\n", m_flIPD );*/
}

CSampleDeviceDriver::~CSampleDeviceDriver()
{
}



EVRInitError CSampleDeviceDriver::Activate(TrackedDeviceIndex_t unObjectId)
{
    m_unObjectId = unObjectId;
    m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);

    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_ModelNumber_String, m_sModelNumber.c_str());
    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_RenderModelName_String, m_sModelNumber.c_str());
    vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_UserIpdMeters_Float, m_flIPD);
    vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_UserHeadToEyeDepthMeters_Float, 0.f);
    vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_DisplayFrequency_Float, m_flDisplayFrequency);
    vr::VRProperties()->SetFloatProperty(m_ulPropertyContainer, Prop_SecondsFromVsyncToPhotons_Float, m_flSecondsFromVsyncToPhotons);

    // return a constant that's not 0 (invalid) or 1 (reserved for Oculus)
    vr::VRProperties()->SetUint64Property(m_ulPropertyContainer, Prop_CurrentUniverseId_Uint64, 2);

    // avoid "not fullscreen" warnings from vrmonitor
    vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_IsOnDesktop_Bool, false);

    //Debug mode activate Windowed Mode (borderless fullscreen), locked to 30 FPS, for testing
    vr::VRProperties()->SetBoolProperty(m_ulPropertyContainer, Prop_DisplayDebugMode_Bool, true);

    // Icons can be configured in code or automatically configured by an external file "drivername\resources\driver.vrresources".
    // Icon properties NOT configured in code (post Activate) are then auto-configured by the optional presence of a driver's "drivername\resources\driver.vrresources".
    // In this manner a driver can configure their icons in a flexible data driven fashion by using an external file.
    //
    // The structure of the driver.vrresources file allows a driver to specialize their icons based on their HW.
    // Keys matching the value in "Prop_ModelNumber_String" are considered first, since the driver may have model specific icons.
    // An absence of a matching "Prop_ModelNumber_String" then considers the ETrackedDeviceClass ("HMD", "Controller", "GenericTracker", "TrackingReference")
    // since the driver may have specialized icons based on those device class names.
    //
    // An absence of either then falls back to the "system.vrresources" where generic device class icons are then supplied.
    //
    // Please refer to "bin\drivers\sample\resources\driver.vrresources" which contains this sample configuration.
    //
    // "Alias" is a reserved key and specifies chaining to another json block.
    //
    // In this sample configuration file (overly complex FOR EXAMPLE PURPOSES ONLY)....
    //
    // "Model-v2.0" chains through the alias to "Model-v1.0" which chains through the alias to "Model-v Defaults".
    //
    // Keys NOT found in "Model-v2.0" would then chase through the "Alias" to be resolved in "Model-v1.0" and either resolve their or continue through the alias.
    // Thus "Prop_NamedIconPathDeviceAlertLow_String" in each model's block represent a specialization specific for that "model".
    // Keys in "Model-v Defaults" are an example of mapping to the same states, and here all map to "Prop_NamedIconPathDeviceOff_String".
    //
    bool bSetupIconUsingExternalResourceFile = true;
    if (!bSetupIconUsingExternalResourceFile) {
        // Setup properties directly in code.
        // Path values are of the form {drivername}\icons\some_icon_filename.png
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceOff_String, "{null}/icons/headset_sample_status_off.png");
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceSearching_String, "{null}/icons/headset_sample_status_searching.gif");
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{null}/icons/headset_sample_status_searching_alert.gif");
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceReady_String, "{null}/icons/headset_sample_status_ready.png");
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{null}/icons/headset_sample_status_ready_alert.png");
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceNotReady_String, "{null}/icons/headset_sample_status_error.png");
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceStandby_String, "{null}/icons/headset_sample_status_standby.png");
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_NamedIconPathDeviceAlertLow_String, "{null}/icons/headset_sample_status_ready_low.png");
    }

	//キーコードをtxtから読みこみ
	FilePath fp;
	char str[TSTR_MAX+2], strSub[TSTR_MAX+2] = {};
	fp.InitializeA(NULL);
	strcpy(str, fp.RelativePathA("damy"));
	while(strcmp(strSub, "SteamVR")){
		fp.CutUniqueNameA(str);/*SteanVRディレクトリに上がるまで下のフォルダ名をカット*/
		fp.GetUniqueNameA(str, strSub, TSTR_MAX);
	}
	strcpy(&str[strlen(str)], "\\drivers\\OpenVR-customizable-driver\\resources\\settings\\keyCode(device).txt");
	//MessageBoxA(NULL, str, NULL, MB_OK);
	keyCode.LoadByFile(str, 5, 10);

    return VRInitError_None;
}

void CSampleDeviceDriver::Deactivate()
{
    m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
}

void CSampleDeviceDriver::EnterStandby()
{
}

void *CSampleDeviceDriver::GetComponent(const char *pchComponentNameAndVersion)
{
    if (!_stricmp(pchComponentNameAndVersion, vr::IVRDisplayComponent_Version)) {
        return (vr::IVRDisplayComponent *)this;
    }

    // override this to add a component to a driver
    return NULL;
}

void CSampleDeviceDriver::PowerOff()
{
}

void CSampleDeviceDriver::DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize)
{
    if (unResponseBufferSize >= 1) {
        pchResponseBuffer[0] = 0;
    }
}

void CSampleDeviceDriver::GetWindowBounds(int32_t *pnX, int32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight)
{
    *pnX = m_nWindowX;
    *pnY = m_nWindowY;
    *pnWidth = m_nWindowWidth;
    *pnHeight = m_nWindowHeight;
}

bool CSampleDeviceDriver::IsDisplayOnDesktop()
{
    return true;
}

bool CSampleDeviceDriver::IsDisplayRealDisplay()
{
    return false;
}

void CSampleDeviceDriver::GetRecommendedRenderTargetSize(uint32_t *pnWidth, uint32_t *pnHeight)
{
    *pnWidth = m_nRenderWidth;
    *pnHeight = m_nRenderHeight;
}

void CSampleDeviceDriver::GetEyeOutputViewport(EVREye eEye, uint32_t *pnX, uint32_t *pnY, uint32_t *pnWidth, uint32_t *pnHeight)
{
    *pnY = 0;
    *pnWidth = m_nWindowWidth / 2;
    *pnHeight = m_nWindowHeight;

    if (eEye == Eye_Left) {
        *pnX = 0;
    } else {
        *pnX = m_nWindowWidth / 2;
    }
}

void CSampleDeviceDriver::GetProjectionRaw(EVREye eEye, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom)
{
    *pfLeft = -1.0;
    *pfRight = 1.0;
    *pfTop = -1.0;
    *pfBottom = 1.0;
}

DistortionCoordinates_t CSampleDeviceDriver::ComputeDistortion(EVREye eEye, float fU, float fV)
{
    DistortionCoordinates_t coordinates;
    coordinates.rfBlue[0] = fU;
    coordinates.rfBlue[1] = fV;
    coordinates.rfGreen[0] = fU;
    coordinates.rfGreen[1] = fV;
    coordinates.rfRed[0] = fU;
    coordinates.rfRed[1] = fV;
    return coordinates;
}

// クォータニオンでベクトルを回転する
static void RotateByQuat(const vr::HmdQuaternion_t& q, double vx, double vy, double vz,
                          double& outX, double& outY, double& outZ)
{
    // t = 2 * cross(q.xyz, v)
    double tx = 2 * (q.y * vz - q.z * vy);
    double ty = 2 * (q.z * vx - q.x * vz);
    double tz = 2 * (q.x * vy - q.y * vx);
    // v' = v + q.w * t + cross(q.xyz, t)
    outX = vx + q.w * tx + (q.y * tz - q.z * ty);
    outY = vy + q.w * ty + (q.z * tx - q.x * tz);
    outZ = vz + q.w * tz + (q.x * ty - q.y * tx);
}

// クォータニオンの掛け算(w,x,y,z順)
static void QMul(double w1,double x1,double y1,double z1,
                  double w2,double x2,double y2,double z2,
                  double& w,double& x,double& y,double& z)
{
    w = w1*w2 - x1*x2 - y1*y2 - z1*z2;
    x = w1*x2 + x1*w2 + y1*z2 - z1*y2;
    y = w1*y2 - x1*z2 + y1*w2 + z1*x2;
    z = w1*z2 + x1*y2 - y1*x2 + z1*w2;
}

vr::DriverPose_t CSampleDeviceDriver::GetPose()
{
    vr::DriverPose_t pose = { 0 };
    pose.poseIsValid = true;
    pose.result = vr::TrackingResult_Running_OK;
    pose.deviceIsConnected = true;

    pose.qWorldFromDriverRotation = HmdQuaternion_Init(1, 0, 0, 0);
    pose.qDriverFromHeadRotation = HmdQuaternion_Init(1, 0, 0, 0);

	double moveForward = 0, moveRight = 0, moveUp = 0;

	if(keyCode.GetState()){
		bool f08 = (GetAsyncKeyState(keyCode[0][8]) & 0x8000) != 0;
		bool f18 = (GetAsyncKeyState(keyCode[1][8]) & 0x8000) != 0;
		bool f28 = (GetAsyncKeyState(keyCode[2][8]) & 0x8000) != 0;
		bool f38 = (GetAsyncKeyState(keyCode[3][8]) & 0x8000) != 0;

		if ((GetAsyncKeyState(keyCode[1][0]) & 0x8000) != 0 && !f18 || f38 && (GetAsyncKeyState(keyCode[3][0])) & 0x8000) {
			yaw += 0.01;
		}
		if ((GetAsyncKeyState(keyCode[1][1]) & 0x8000) != 0 && !f18 || f38 && (GetAsyncKeyState(keyCode[3][1])) & 0x8000) {
			yaw += -0.01;
		}

		if ((GetAsyncKeyState(keyCode[1][2]) & 0x8000) != 0 && !f18 || f38 && (GetAsyncKeyState(keyCode[3][2])) & 0x8000) {
			pitch += 0.01;
		}
		if ((GetAsyncKeyState(keyCode[1][3]) & 0x8000) != 0 && !f18 || f38 && (GetAsyncKeyState(keyCode[3][3])) & 0x8000) {
			pitch += -0.01;
		}

		if ((GetAsyncKeyState(keyCode[1][4]) & 0x8000) != 0 && !f18 || f38 && (GetAsyncKeyState(keyCode[3][4])) & 0x8000) {
			roll += 0.01;
		}
		if ((GetAsyncKeyState(keyCode[1][5]) & 0x8000) != 0 && !f18 || f38 && (GetAsyncKeyState(keyCode[3][5])) & 0x8000) {
			roll += -0.01;
		}

		if ((GetAsyncKeyState(keyCode[1][9]) & 0x8000) != 0 && !f18 || f38 && (GetAsyncKeyState(keyCode[3][9])) & 0x8000) {
			yaw = 0;
			pitch = 0;
			roll = 0;
		}

		if ((GetAsyncKeyState(keyCode[0][0]) & 0x8000) != 0 && !f08 || f28 && (GetAsyncKeyState(keyCode[2][0])) & 0x8000) {
			moveForward += -1;
		}
		if ((GetAsyncKeyState(keyCode[0][1]) & 0x8000) != 0 && !f08 || f28 && (GetAsyncKeyState(keyCode[2][1])) & 0x8000) {
			moveForward += 1;
		}

		if ((GetAsyncKeyState(keyCode[0][2]) & 0x8000) != 0 && !f08 || f28 && (GetAsyncKeyState(keyCode[2][2])) & 0x8000) {
			moveRight += -1;
		}
		if ((GetAsyncKeyState(keyCode[0][3]) & 0x8000) != 0 && !f08 || f28 && (GetAsyncKeyState(keyCode[2][3])) & 0x8000) {
			moveRight += 1;
		}

		if ((GetAsyncKeyState(keyCode[0][4]) & 0x8000) != 0 && !f08 || f28 && (GetAsyncKeyState(keyCode[2][4])) & 0x8000) {
			moveUp += 1;
		}
		if ((GetAsyncKeyState(keyCode[0][5]) & 0x8000) != 0 && !f08 || f28 && (GetAsyncKeyState(keyCode[2][5])) & 0x8000) {
			moveUp += -1;
		}

		if ((GetAsyncKeyState(keyCode[0][9]) & 0x8000) != 0 && !f08 || f28 && (GetAsyncKeyState(keyCode[2][9])) & 0x8000) {
			pX = 0;
			pY = 0;
			pZ = 0;
			moveForward = 0;
			moveRight = 0;
			moveUp = 0;
		}
	}

	static bool fKeyDownContinuation = 0;
	if(((GetAsyncKeyState(keyCode[4][0]) & 0x8000) != 0)){
		if(!fKeyDownContinuation){
			keyCode.SetState(!keyCode.GetState());
			fKeyDownContinuation = true;
		}
	}else{
		fKeyDownContinuation = false;
	}

	
	// yaw: 世界のY軸(左右を向く)を一番外側に
	double qyw = cos(yaw * 0.5), qyx = 0, qyy = sin(yaw * 0.5), qyz = 0;
	// pitch: ローカルX軸(見上げ下げ)を2番目に
	double qpw = cos(pitch * 0.5), qpx = sin(pitch * 0.5), qpy = 0, qpz = 0;
	// roll: ローカルZ軸(首を傾げる)を一番内側に
	double qrw = cos(roll * 0.5), qrx = 0, qry = 0, qrz = sin(roll * 0.5);

	double w1,x1,y1,z1;
	QMul(qyw,qyx,qyy,qyz, qpw,qpx,qpy,qpz, w1,x1,y1,z1);   // yaw * pitch
	QMul(w1,x1,y1,z1, qrw,qrx,qry,qrz,
		 pose.qRotation.w, pose.qRotation.x, pose.qRotation.y, pose.qRotation.z);  // (yaw*pitch) * roll

// 3. 「前・右・上」を、実際に使ったqRotationから直接求める(水平化はしない)
double fx, fy, fz;   // 前(ローカルの -Z を回転)
double rx, ry, rz;   // 右(ローカルの +X を回転)
double ux, uy, uz;   // 上(ローカルの +Y を回転)
RotateByQuat(pose.qRotation, 0, 0, -1, fx, fy, fz);
RotateByQuat(pose.qRotation, 1, 0, 0,  rx, ry, rz);
RotateByQuat(pose.qRotation, 0, 1, 0,  ux, uy, uz);

// 4. 移動(forward/right/upは回転で長さが変わらないので、正規化は不要)
const double step = 0.01;
pX += (fx * moveForward + rx * moveRight + ux * moveUp) * step;
pY += (fy * moveForward + ry * moveRight + uy * moveUp) * step;
pZ += (fz * moveForward + rz * moveRight + uz * moveUp) * step;

pose.vecPosition[0] = pX;
pose.vecPosition[1] = pY;
pose.vecPosition[2] = pZ;

    return pose;
}

void CSampleDeviceDriver::RunFrame()
{
    // In a real driver, this should happen from some pose tracking thread.
    // The RunFrame interval is unspecified and can be very irregular if some other
    // driver blocks it for some periodic task.
    if (m_unObjectId != vr::k_unTrackedDeviceIndexInvalid) {
        vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, GetPose(), sizeof(DriverPose_t));
    }
}
