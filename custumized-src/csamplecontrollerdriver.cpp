#include "csamplecontrollerdriver.h"
#include "basics.h"

#include <math.h>

using namespace vr;

static double cyaw = 0, cpitch = 0, croll = 0;
static double cpX = 0, cpY = 0, cpZ = 0;
static double ct0, ct1, ct2, ct3, ct4, ct5;

static double c2pX = 0, c2pY = 0, c2pZ = 0;

CSampleControllerDriver::CSampleControllerDriver()
{
    m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
    m_ulPropertyContainer = vr::k_ulInvalidPropertyContainer;
}

void CSampleControllerDriver::SetControllerIndex(int32_t CtrlIndex)
{
    ControllerIndex = CtrlIndex;
}

CSampleControllerDriver::~CSampleControllerDriver()
{
}

vr::EVRInitError CSampleControllerDriver::Activate(vr::TrackedDeviceIndex_t unObjectId)
{
    m_unObjectId = unObjectId;
    m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);

    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_ControllerType_String, "oculus_touch");
    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_LegacyInputProfile_String, "oculus_touch");

    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_ModelNumber_String, "Oculus Touch Controller");
    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_ManufacturerName_String, "Oculus");
	switch (ControllerIndex){
	case 1:
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_RenderModelName_String, "oculus_cv1_controller_left");
		break;
	case 2:
		vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_RenderModelName_String, "oculus_cv1_controller_right");
		break;
	};

    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_TrackingSystemName_String, "oculus");
    vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_DeviceClass_Int32, TrackedDeviceClass_Controller);

    switch (ControllerIndex) {
    case 1:
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_SerialNumber_String, "CTRL1Serial");
        break;
    case 2:
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_SerialNumber_String, "CTRL2Serial");
        break;
    }

    uint64_t supportedButtons = 0xFFFFFFFFFFFFFFFFULL;
    vr::VRProperties()->SetUint64Property(m_ulPropertyContainer, vr::Prop_SupportedButtons_Uint64, supportedButtons);

    // return a constant that's not 0 (invalid) or 1 (reserved for Oculus)
    //vr::VRProperties()->SetUint64Property( m_ulPropertyContainer, Prop_CurrentUniverseId_Uint64, 2 );

    // avoid "not fullscreen" warnings from vrmonitor
    //vr::VRProperties()->SetBoolProperty( m_ulPropertyContainer, Prop_IsOnDesktop_Bool, false );

    // our sample device isn't actually tracked, so set this property to avoid having the icon blink in the status window
    //vr::VRProperties()->SetBoolProperty( m_ulPropertyContainer, Prop_NeverTracked_Bool, false );

    // even though we won't ever track we want to pretend to be the right hand so binding will work as expected

    switch (ControllerIndex) {
    case 1:
        vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_ControllerRoleHint_Int32, TrackedControllerRole_LeftHand);
        break;
    case 2:
        vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_ControllerRoleHint_Int32, TrackedControllerRole_RightHand);
        break;
    }

    // this file tells the UI what to show the user for binding this controller as well as what default bindings should
    // be for legacy or other apps
    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_InputProfilePath_String, "{null}/input/mycontroller_profile.json");

    //  Buttons handles
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/application_menu/click", &HButtons[0]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/grip/click", &HButtons[1]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/system/click", &HButtons[2]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/trackpad/click", &HButtons[3]);

    // Analog handles
    vr::VRDriverInput()->CreateScalarComponent(
                m_ulPropertyContainer, "/input/trackpad/x", &HAnalog[0],
            vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided
            );
    vr::VRDriverInput()->CreateScalarComponent(
                m_ulPropertyContainer, "/input/trackpad/y", &HAnalog[1],
            vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided
            );
    vr::VRDriverInput()->CreateScalarComponent(
                m_ulPropertyContainer, "/input/trigger/value", &HAnalog[2],
            vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedOneSided
            );

    vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, vr::Prop_Axis0Type_Int32, vr::k_eControllerAxis_TrackPad);

    // create our haptic component
    vr::VRDriverInput()->CreateHapticComponent(m_ulPropertyContainer, "/output/haptic", &m_compHaptic);

	//キーコードをtxtから読みこみ
	keyCode.LoadByFile(CONTROLLER_KEYCODEFILE, 6, 10);

    return VRInitError_None;
}

void CSampleControllerDriver::Deactivate()
{
    m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
}

void CSampleControllerDriver::EnterStandby()
{
}

void *CSampleControllerDriver::GetComponent(const char *pchComponentNameAndVersion)
{
    // override this to add a component to a driver
    return NULL;
}

void CSampleControllerDriver::PowerOff()
{
}

void CSampleControllerDriver::DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize)
{
    if (unResponseBufferSize >= 1) {
        pchResponseBuffer[0] = 0;
    }
}

DriverPose_t CSampleControllerDriver::GetPose()
{
    DriverPose_t pose = { 0 };
    //pose.poseIsValid = false;
    pose.poseIsValid = true;
    //pose.result = TrackingResult_Calibrating_OutOfRange;
    pose.result = TrackingResult_Running_OK;
    pose.deviceIsConnected = true;

    pose.qWorldFromDriverRotation = HmdQuaternion_Init(1, 0, 0, 0);
    pose.qDriverFromHeadRotation = HmdQuaternion_Init(1, 0, 0, 0);

    if (ControllerIndex == 1) {

        if ((GetAsyncKeyState(keyCode[0][0]) & 0x8000) != 0) {
            cyaw += 0.1;                                       //F
        }
        if ((GetAsyncKeyState(keyCode[0][1]) & 0x8000) != 0) {
            cyaw += -0.1;                                       //H
        }
        if ((GetAsyncKeyState(keyCode[0][2]) & 0x8000) != 0) {
            cpitch += 0.1;                                       //R
        }
        if ((GetAsyncKeyState(keyCode[0][3]) & 0x8000) != 0) {
            cpitch += -0.1;                                       //Y
        }
		if ((GetAsyncKeyState(keyCode[0][4]) & 0x8000) != 0) {
            croll += 0.1;                                       //T
        }
        if ((GetAsyncKeyState(keyCode[0][5]) & 0x8000) != 0) {
            croll += -0.1;                                       //G
        }
        if ((GetAsyncKeyState(keyCode[0][9]) & 0x8000) != 0) { //V
            cyaw = 0;
            cpitch = 0;
            croll = 0;
        }

		bool f28 = (GetAsyncKeyState(keyCode[2][8]) & 0x8000) != 0;

        //Change position controller1
        if ((GetAsyncKeyState(keyCode[2][0]) & 0x8000) != 0 && !f28) {
            cpZ += -0.01;                                       //W
        }
        if ((GetAsyncKeyState(keyCode[2][1]) & 0x8000) != 0 && !f28) {
            cpZ += 0.01;                                       //S
        }
        if ((GetAsyncKeyState(keyCode[2][2]) & 0x8000) != 0 && !f28) {
            cpX += -0.01;                                       //A
        }
        if ((GetAsyncKeyState(keyCode[2][3]) & 0x8000) != 0 && !f28) {
            cpX += 0.01;                                       //D
        }
        if ((GetAsyncKeyState(keyCode[2][4]) & 0x8000) != 0 && !f28) {
            cpY += 0.01;                                       //Q
        }
        if ((GetAsyncKeyState(keyCode[2][5]) & 0x8000) != 0 && !f28) {
            cpY += -0.01;                                       //E
        }
        if ((GetAsyncKeyState(keyCode[2][9]) & 0x8000) != 0 && !f28) {
            cpX = 0;
            cpY = 0;
            cpZ = 0;
        }                                                                        //1

        pose.vecPosition[0] = cpX;
        pose.vecPosition[1] = cpY;
        pose.vecPosition[2] = cpZ;
    } else {
		bool f48 = (GetAsyncKeyState(keyCode[4][8]) & 0x8000) != 0;

        //Controller2
        if ((GetAsyncKeyState(keyCode[4][0]) & 0x8000) != 0 && !f48) {
            c2pZ += -0.01;                                       //I
        }
        if ((GetAsyncKeyState(keyCode[4][1]) & 0x8000) != 0 && !f48) {
            c2pZ += 0.01;                                       //K
        }
        if ((GetAsyncKeyState(keyCode[4][2]) & 0x8000) != 0 && !f48) {
            c2pX += -0.01;                                       //J
        }
        if ((GetAsyncKeyState(keyCode[4][3]) & 0x8000) != 0 && !f48) {
            c2pX += 0.01;                                       //L
        }
        if ((GetAsyncKeyState(keyCode[4][4]) & 0x8000) != 0 && !f48) {
            c2pY += 0.01;                                       //U
        }
        if ((GetAsyncKeyState(keyCode[4][5]) & 0x8000) != 0 && !f48) {
            c2pY += -0.01;                                       //O
        }
        if ((GetAsyncKeyState(keyCode[4][9]) & 0x8000) != 0 && !f48) {
            c2pX = 0;
            c2pY = 0;
            c2pZ = 0;
        }                                                                           //0

        pose.vecPosition[0] = c2pX;
        pose.vecPosition[1] = c2pY;
        pose.vecPosition[2] = c2pZ;
    }

    //Convert yaw, pitch, roll to quaternion
    ct0 = cos(cyaw * 0.5);
    ct1 = sin(cyaw * 0.5);
    ct2 = cos(croll * 0.5);
    ct3 = sin(croll * 0.5);
    ct4 = cos(cpitch * 0.5);
    ct5 = sin(cpitch * 0.5);

    //Set controller rotation
    pose.qRotation.w = ct0 * ct2 * ct4 + ct1 * ct3 * ct5;
    pose.qRotation.x = ct0 * ct3 * ct4 - ct1 * ct2 * ct5;
    pose.qRotation.y = ct0 * ct2 * ct5 + ct1 * ct3 * ct4;
    pose.qRotation.z = ct1 * ct2 * ct4 - ct0 * ct3 * ct5;

    return pose;
}

void CSampleControllerDriver::RunFrame()
{
#if defined(_WINDOWS)
    // Your driver would read whatever hardware state is associated with its input components and pass that
    // in to UpdateBooleanComponent. This could happen in RunFrame or on a thread of your own that's reading USB
    // state. There's no need to update input state unless it changes, but it doesn't do any harm to do so.

    if (ControllerIndex == 1) {
		/*left controller*/
        //vr::VRDriverInput()->UpdateBooleanComponent(HButtons[0], (0x8000 & GetAsyncKeyState('Z')) != 0, 0); //Application Menu
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[1], (0x8000 & GetAsyncKeyState(keyCode[3][4])) != 0, 0); //Grip
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[2], (0x8000 & GetAsyncKeyState(keyCode[3][2])) != 0, 0); //System
        //vr::VRDriverInput()->UpdateBooleanComponent(HButtons[3], (0x8000 & GetAsyncKeyState(keyCode[3][2])) != 0, 0); //trackpad

        vr::VRDriverInput()->UpdateScalarComponent(HAnalog[0], (0x8000 & GetAsyncKeyState(keyCode[3][0])) ? 1.0 : 0.0, 0); //Trackpad x
        vr::VRDriverInput()->UpdateScalarComponent(HAnalog[1], (0x8000 & GetAsyncKeyState(keyCode[3][1])) ? 1.0 : 0.0, 0); //Trackpad y
        vr::VRDriverInput()->UpdateScalarComponent(HAnalog[2], (0x8000 & GetAsyncKeyState(keyCode[3][3])) ? 1.0 : 0.0, 0); //Triger
        
    } else {
		/*right controller*/
        //vr::VRDriverInput()->UpdateBooleanComponent(HButtons[0], (0x8000 & GetAsyncKeyState(190)) != 0, 0); //Application Menu
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[1], (0x8000 & GetAsyncKeyState(keyCode[5][4])) != 0, 0); //Grip
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[2], (0x8000 & GetAsyncKeyState(keyCode[5][2])) != 0, 0); //System
        //vr::VRDriverInput()->UpdateBooleanComponent(HButtons[3], (0x8000 & GetAsyncKeyState('2')) != 0, 0); //Trackpad

        vr::VRDriverInput()->UpdateScalarComponent(HAnalog[0], (0x8000 & GetAsyncKeyState(keyCode[5][0])) ? 1.0 : 0.0, 0); //Trackpad a
        vr::VRDriverInput()->UpdateScalarComponent(HAnalog[1], (0x8000 & GetAsyncKeyState(keyCode[5][1])) ? 1.0 : 0.0, 0); //Trackpad b
        vr::VRDriverInput()->UpdateScalarComponent(HAnalog[2], (0x8000 & GetAsyncKeyState(keyCode[5][3])) ? 1.0 : 0.0, 0); //Triger
    }

    if (m_unObjectId != vr::k_unTrackedDeviceIndexInvalid) {
        vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, GetPose(), sizeof(DriverPose_t));
    }

#endif
}

void CSampleControllerDriver::ProcessEvent(const vr::VREvent_t &vrEvent)
{
    switch (vrEvent.eventType) {
    case vr::VREvent_Input_HapticVibration:
        if (vrEvent.data.hapticVibration.componentHandle == m_compHaptic) {
            // This is where you would send a signal to your hardware to trigger actual haptic feedback
            //DriverLog( "BUZZ!\n" );
        }
        break;
    }
}

std::string CSampleControllerDriver::GetSerialNumber() const
{
    switch (ControllerIndex) {
    case 1:
        return "CTRL1Serial";
        break;
    case 2:
        return "CTRL2Serial";
        break;
    }
}


