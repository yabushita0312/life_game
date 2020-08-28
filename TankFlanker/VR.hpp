#pragma once
#include"DXLib_ref/DXLib_ref.h"
#include <fstream>
#include <array>
#include <vector>
#include <D3D11.h>
#include <openvr.h>
#include <memory>

#define BUTTON_TRIGGER vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Trigger)
#define BUTTON_SIDE vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_Grip)
#define BUTTON_TOUCHPAD vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Touchpad)
#define BUTTON_TOPBUTTON vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_ApplicationMenu)

#define DEVICE_HMD vr::TrackedDeviceClass_HMD
#define DEVICE_CONTROLLER vr::TrackedDeviceClass_Controller
#define DEVICE_BASESTATION vr::TrackedDeviceClass_TrackingReference
class VRDraw {
public:
	struct systems {
		int id = 0;
		VECTOR_ref pos = VGet(0, 0, 0);
		VECTOR_ref xvec = VGet(1, 0, 0);
		VECTOR_ref yvec = VGet(0, 1, 0);
		VECTOR_ref zvec = VGet(0, 0, 1);
		std::array<uint64_t, 2> on{ 0 };
		VECTOR_ref touch;
		char num = 0;
		vr::ETrackedDeviceClass type = vr::TrackedDeviceClass_Invalid;
		bool turn = false, now = false;
	};
private:
	bool use_vr = true;

	vr::IVRSystem* m_pHMD=nullptr;
	vr::EVRInitError eError = vr::VRInitError_None;
	std::vector<systems> ctrl;							/*HMD,controller*/
	char deviceall = 0;
	VECTOR_ref pos;
	char hmd_num = -1;
	char hand1_num = -1;
	char hand2_num = -1;
public:
	const auto& get_hmd_num(void) { return hmd_num; }
	const auto& get_hand1_num(void) { return hand1_num; }
	const auto& get_hand2_num(void) { return hand2_num; }
	auto* get_device(void) { return &ctrl; }
	template<class Y, class D>
	VRDraw(std::unique_ptr<Y, D>& settings) {
		use_vr = settings->useVR_e;
		if (use_vr) {
			eError = vr::VRInitError_None;
			m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);
			if (eError != vr::VRInitError_None) {
				m_pHMD = nullptr;
				use_vr = false;
				settings->useVR_e = false;
			}
		}
	}
	~VRDraw(void) {
		if (use_vr&&m_pHMD) {
			//vr::VR_Shutdown();
			m_pHMD = NULL;
		}
	}
	void Set_Device(void) {
		if (use_vr && m_pHMD) {
			deviceall = 0;
			int i = 0;
			for (char k = 0; k < 5; k++) {
				auto old = deviceall;
				auto dev = m_pHMD->GetTrackedDeviceClass(k);
				if (dev == DEVICE_HMD) {
					hmd_num = deviceall;
					deviceall++;
				}
				else if (dev == DEVICE_CONTROLLER) {
					switch (i) {
					case 0:
						hand1_num = deviceall;
						i++;
						break;
					case 1:
						hand2_num = deviceall;
						i++;
						break;
					default:
						break;
					}
					deviceall++;
				}
				else if (dev == DEVICE_BASESTATION) {
					deviceall++;
				}
				if (deviceall != old) {
					ctrl.resize(deviceall);
					ctrl.back().now = false;
					ctrl.back().id = old;
					ctrl.back().num = k;
					ctrl.back().type = dev;
					ctrl.back().turn = true;
				}
			}
		}
	}
	void Move_Player(void) {
		if (use_vr&&m_pHMD) {
			vr::TrackedDevicePose_t tmp;
			vr::VRControllerState_t night;
			for (auto& c : ctrl) {
				if (c.type == DEVICE_HMD) {
					m_pHMD->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0f, &tmp, 1);
					c.on[0] = 0;
					c.on[1] = 0;
					c.touch = VGet(0, 0, 0);
					c.now = tmp.bPoseIsValid;
					c.pos = VGet(tmp.mDeviceToAbsoluteTracking.m[0][3], tmp.mDeviceToAbsoluteTracking.m[1][3], -tmp.mDeviceToAbsoluteTracking.m[2][3]);
					c.xvec = VGet(tmp.mDeviceToAbsoluteTracking.m[0][0], tmp.mDeviceToAbsoluteTracking.m[1][0], -tmp.mDeviceToAbsoluteTracking.m[2][0]);
					c.yvec = VGet(tmp.mDeviceToAbsoluteTracking.m[0][1], tmp.mDeviceToAbsoluteTracking.m[1][1], -tmp.mDeviceToAbsoluteTracking.m[2][1]);
					c.zvec = VGet(-tmp.mDeviceToAbsoluteTracking.m[0][2], -tmp.mDeviceToAbsoluteTracking.m[1][2], tmp.mDeviceToAbsoluteTracking.m[2][2]);
				}
				else if (c.type == DEVICE_CONTROLLER || c.type == DEVICE_BASESTATION) {
					m_pHMD->GetControllerStateWithPose(vr::TrackingUniverseStanding, c.num, &night, sizeof(night), &tmp);
					c.on[0] = night.ulButtonPressed;
					c.on[1] = night.ulButtonTouched;
					c.touch = VGet(night.rAxis[0].x, night.rAxis[0].y, 0);
					c.now = tmp.bPoseIsValid;
					c.pos = VGet(tmp.mDeviceToAbsoluteTracking.m[0][3], tmp.mDeviceToAbsoluteTracking.m[1][3], -tmp.mDeviceToAbsoluteTracking.m[2][3]);
					c.xvec = VGet(tmp.mDeviceToAbsoluteTracking.m[0][0], tmp.mDeviceToAbsoluteTracking.m[1][0], -tmp.mDeviceToAbsoluteTracking.m[2][0]);
					c.yvec = VGet(tmp.mDeviceToAbsoluteTracking.m[0][1], tmp.mDeviceToAbsoluteTracking.m[1][1], -tmp.mDeviceToAbsoluteTracking.m[2][1]);
					c.zvec = VGet(-tmp.mDeviceToAbsoluteTracking.m[0][2], -tmp.mDeviceToAbsoluteTracking.m[1][2], tmp.mDeviceToAbsoluteTracking.m[2][2]);
				}
			}
		}
		else {
			for (auto& c : ctrl) {
				c.on[0] = 0;
				c.on[1] = 0;
				c.touch = VGet(0, 0, 0);
				c.pos = VGet(0, 0, 0);
				c.xvec = VGet(1, 0, 0);
				c.yvec = VGet(0, 1, 0);
				c.zvec = VGet(0, 0, 1);
			}
		}
	}
	inline VECTOR_ref SetEyePositionVR(const char& eye_type) {
		if (use_vr&&m_pHMD) {
			const vr::HmdMatrix34_t tmpmat = vr::VRSystem()->GetEyeToHeadTransform((vr::EVREye)eye_type);
			return ctrl[hmd_num].pos + ctrl[hmd_num].xvec*(tmpmat.m[0][3]) + ctrl[hmd_num].yvec*(tmpmat.m[1][3]) + ctrl[hmd_num].zvec*(-tmpmat.m[2][3]);
		}
		else {
			return VGet(0, 0, 0);
		}
	}

	inline void GetDevicePositionVR(const char& handle_,VECTOR_ref* pos_,MATRIX_ref*mat) {
		if (use_vr) {
			if (handle_ != -1) {
				auto& ptr_ = ctrl[handle_];
				*pos_ = ptr_.pos;
				*mat = MATRIX_ref::Axis1(ptr_.xvec*-1.f, ptr_.yvec, ptr_.zvec*-1.f);
				//ptr_HMD.now;
			}
			else {
				*pos_ = VGet(0, 1.f, 0);
				*mat = MATRIX_ref::Axis1(VGet(-1, 0, 0), VGet(0, 1, 0), VGet(0, 0, -1));
			}
		}
		else {
			*pos_ = VGet(0, 1.f, 0);
			*mat = MATRIX_ref::Axis1(VGet(-1, 0, 0), VGet(0, 1, 0), VGet(0, 0, -1));
		}
	}

	inline VECTOR_ref GetEyePosition_minVR(const char& eye_type) {
		if (use_vr&&m_pHMD) {
			const vr::HmdMatrix34_t tmpmat = vr::VRSystem()->GetEyeToHeadTransform((vr::EVREye)eye_type);
			return ctrl[hmd_num].xvec*(tmpmat.m[0][3]) + ctrl[hmd_num].yvec*(tmpmat.m[1][3]) + ctrl[hmd_num].zvec*(-tmpmat.m[2][3]);
		}
		else {
			return VGet(0, 0, 0);
		}
	}

	inline void PutEye(ID3D11Texture2D* texte, const char& i) {
		if (use_vr) {
			vr::Texture_t tex = { (void*)texte, vr::ETextureType::TextureType_DirectX,vr::EColorSpace::ColorSpace_Auto };
			vr::VRCompositor()->Submit((vr::EVREye)i, &tex, NULL, vr::Submit_Default);
		}
	}
	inline void Eye_Flip(const LONGLONG& waits, const float& f_rate) {
		if (use_vr&&m_pHMD) {
			vr::TrackedDevicePose_t tmp;
			vr::VRCompositor()->WaitGetPoses(&tmp, 1, NULL, 1);
		}
		else {
			while (GetNowHiPerformanceCount() - waits < 1000000.0f / f_rate) {}
		}
	}

	inline void Haptic(const char&id_, unsigned short times) {
		if (id_ != -1) {
			if (use_vr&&m_pHMD) {
				m_pHMD->TriggerHapticPulse(ctrl[id_].id, 2, times);
			}
		}
	}
};
