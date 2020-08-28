#pragma once
#include "DxLib.h"
#include"EffekseerForDXLib.h"

#include"DXLib_mat.hpp"

#include "SoundHandle.hpp"
#include "GraphHandle.hpp"
#include "FontHandle.hpp"
#include "MV1ModelHandle.hpp"
#include "EffekseerEffectHandle.hpp"

#include "useful.hpp"

#include <array>
#include <list>
#include <vector>
enum Effect {
	ef_fire, //発砲炎
	ef_reco, //小口径跳弾
	ef_smoke, //銃の軌跡
	effects, //読み込む
};
struct EffectS {
	bool flug{ false };		 /**/
	size_t id = 0;
	Effekseer3DPlayingHandle handle; /**/
	VECTOR_ref pos;			 /**/
	VECTOR_ref nor;			 /**/
	float scale = 1.f;		 /**/
};

class DXDraw {
private:
	bool use_shadow = true;			/*影描画*/
	int shadow_near = 0;			/*近影*/
	int shadow_far = 0;				/*遠影*/
	size_t shadow_size = 10;		/*影サイズ*/
	bool use_pixellighting = true;			     /**/
	bool use_vsync = false;				     /*垂直同期*/
	float frate = 60.f;				     /*フレームレート*/
	std::array<EffekseerEffectHandle, effects> effHndle; /*エフェクトリソース*/
	EffekseerEffectHandle gndsmkHndle;		     /*エフェクトリソース*/
	int disp_x = 1920;
	int disp_y = 1080;
public:
	EffekseerEffectHandle& get_effHandle(int p1) noexcept { return effHndle[p1]; }
	const EffekseerEffectHandle& get_effHandle(int p1) const noexcept { return effHndle[p1]; }
	EffekseerEffectHandle& get_gndhitHandle() noexcept { return gndsmkHndle; }
	const EffekseerEffectHandle& get_gndhitHandle() const noexcept { return gndsmkHndle; }
	template<class Y, class D>
	DXDraw(const char* title, std::unique_ptr<Y, D>& settings, const float& fps = 60.f) {
		this->use_shadow = settings->shadow_e;
		this->shadow_size = settings->shadow_level_e;
		this->disp_x = settings->dispx;
		this->disp_y = settings->dispy;

		frate = fps;
		SetOutApplicationLogValidFlag(settings->getlog_e ? TRUE : FALSE);	/*log*/
		SetMainWindowText(title);											/*タイトル*/
		ChangeWindowMode(TRUE);												/*窓表示*/
		SetUseDirect3DVersion(DX_DIRECT3D_11);								/*directX ver*/
		SetGraphMode(this->disp_x, this->disp_y, 32);						/*解像度*/
		SetUseDirectInputFlag(TRUE);										/**/
		SetDirectInputMouseMode(TRUE);										/**/
		SetWindowSizeChangeEnableFlag(FALSE, FALSE);						/*ウインドウサイズを手動不可、ウインドウサイズに合わせて拡大もしないようにする*/
		SetUsePixelLighting(use_pixellighting ? TRUE : FALSE);				/*ピクセルシェーダの使用*/
		SetFullSceneAntiAliasingMode(4, 2);									/*アンチエイリアス*/
		SetEnableXAudioFlag(TRUE);
		Set3DSoundOneMetre(1.0f);
		SetWaitVSyncFlag(use_vsync ? TRUE : FALSE);							/*垂直同期*/
		DxLib_Init();														/**/
		Effekseer_Init(8000);												/*Effekseer*/
		SetChangeScreenModeGraphicsSystemResetFlag(FALSE);					/*Effekseer*/
		Effekseer_SetGraphicsDeviceLostCallbackFunctions();					/*Effekseer*/
		SetAlwaysRunFlag(TRUE);												/*background*/
		SetUseZBuffer3D(TRUE);												/*zbufuse*/
		SetWriteZBuffer3D(TRUE);											/*zbufwrite*/
		MV1SetLoadModelPhysicsWorldGravity(-9.8f);							/*重力*/
		//エフェクト
		{
			size_t j = 0;
			for (auto& e : effHndle) {
				e = EffekseerEffectHandle::load("data/effect/" + std::to_string(j++) + ".efk");
			}
			gndsmkHndle = EffekseerEffectHandle::load("data/effect/gndsmk.efk");
		}
		SetWindowSize(settings->out_dispx, settings->out_dispy);
		SetWindowPosition(
			//*
			deskx+
			//*/
			(deskx - settings->out_dispx) / 2 - 8, (desky - settings->out_dispy) / 2 - 32);
	}
	~DXDraw(void) {
		Effkseer_End();
		DxLib_End();
	}
	template <typename T>
	bool Set_Light_Shadow(const VECTOR_ref& farsize, const VECTOR_ref& nearsize, const VECTOR_ref& Light_dir, T doing) {
		SetGlobalAmbientLight(GetColorF(0.12f, 0.11f, 0.10f, 0.0f));
		SetLightDirection(Light_dir.get());
		if (this->use_shadow) {
			shadow_near = MakeShadowMap(int(pow(2, this->shadow_size)), int(pow(2, this->shadow_size)));
			shadow_far = MakeShadowMap(int(pow(2, this->shadow_size)), int(pow(2, this->shadow_size)));
			SetShadowMapAdjustDepth(shadow_near, 0.0005f);
			SetShadowMapLightDirection(shadow_near, Light_dir.get());
			SetShadowMapLightDirection(shadow_far, Light_dir.get());
			SetShadowMapDrawArea(shadow_far, nearsize.get(), farsize.get());
			ShadowMap_DrawSetup(shadow_far);
			doing();
			ShadowMap_DrawEnd();
		}
		return true;
	}
	bool Delete_Shadow() {
		if (this->use_shadow) {
			DeleteShadowMap(shadow_near);
			DeleteShadowMap(shadow_far);
		}
		return true;
	}

	template <typename T>
	bool Ready_Shadow(const VECTOR_ref& pos, T doing, const VECTOR_ref& nearsize) {
		if (this->use_shadow) {
			SetShadowMapDrawArea(shadow_near, (nearsize*(-1.f) + pos).get(), (VECTOR_ref(nearsize) + pos).get());
			ShadowMap_DrawSetup(shadow_near);
			doing();
			ShadowMap_DrawEnd();
			return true;
		}
		return false;
	}
	template <typename T>
	bool Draw_by_Shadow(T doing) {
		if (this->use_shadow) {
			SetUseShadowMap(0, shadow_near);
			SetUseShadowMap(1, shadow_far);
		}
		doing();
		if (this->use_shadow) {
			SetUseShadowMap(0, -1);
			SetUseShadowMap(1, -1);
		}
		return true;
	}
	bool Screen_Flip(const LONGLONG& waits) {
		ScreenFlip();
		if (!use_vsync) {
			while (GetNowHiPerformanceCount() - waits < 1000000.0f / frate) {}
		}
		return true;
	}

	static bool Screen_Flip(const LONGLONG& waits,const float& f_rate) {
		ScreenFlip();
		if (GetWaitVSyncFlag()==FALSE) {
			while (GetNowHiPerformanceCount() - waits < 1000000.0f / f_rate) {}
		}
		return true;
	}
	static bool Screen_Flip(void) {
		ScreenFlip();
		return true;
	}


	static bool Capsule3D(const VECTOR_ref& p1, const VECTOR_ref& p2, const float& range, const unsigned int& color, const unsigned int& speccolor) {
		return DxLib::DrawCapsule3D(p1.get(), p2.get(), range, 8, color, speccolor, TRUE) == TRUE;
	}

	static bool Line2D(const int& p1x, const int& p1y, const int& p2x, const int& p2y, const unsigned int& color, const int& thickness=1) {
		return DxLib::DrawLine(p1x,p1y,p2x,p2y,color,thickness) == TRUE;
	}

};

void set_effect(EffectS* efh, VECTOR_ref pos, VECTOR_ref nor, float scale = 1.f) {
	efh->flug = true;
	efh->pos = pos;
	efh->nor = nor;
	efh->scale = scale;
}
void set_pos_effect(EffectS* efh, const EffekseerEffectHandle& handle) {
	if (efh->flug) {
		if (efh->handle.IsPlaying()) {
			efh->handle.Stop();
		}
		efh->handle = handle.Play3D();
		efh->handle.SetPos(efh->pos);
		efh->handle.SetRotation(atan2(efh->nor.y(), std::hypot(efh->nor.x(), efh->nor.z())), atan2(-efh->nor.x(), -efh->nor.z()), 0);
		efh->handle.SetScale(efh->scale);
		efh->flug = false;
	}
}
typedef std::pair<int, VECTOR_ref> frames;
class switchs {
public:
	bool first;
	uint8_t second;

	switchs() {
		first = false;
		second = 0;
	};

	void ready(bool on) {
		first = on;
		second = 0;
	}

	void get_in(bool key) {
		second = std::clamp<uint8_t>(second + 1, 0, (key ? 2 : 0));
		if (second == 1) {
			first ^= 1;
		}
	}
};
