#pragma once

class HostPassEffect {
private:
	GraphHandle FarScreen;
	GraphHandle MainScreen;
	GraphHandle NearScreen;
	GraphHandle GaussScreen;
	const int EXTEND = 4;
	bool dof_flag = true;
	bool bloom_flag = true;
	int disp_x = 1920;
	int disp_y = 1080;
public:
	template<class Y, class D>
	HostPassEffect(std::unique_ptr<Y, D>& settings) {
		disp_x = settings->dispx;
		disp_y = settings->dispy;
		dof_flag = settings->dof_e;
		bloom_flag = settings->bloom_e;
		FarScreen = GraphHandle::Make(disp_x, disp_y, true);
		MainScreen = GraphHandle::Make(disp_x, disp_y, true);
		NearScreen = GraphHandle::Make(disp_x, disp_y, true);
		GaussScreen = GraphHandle::Make(disp_x / EXTEND, disp_y / EXTEND); /*エフェクト*/
	}
	~HostPassEffect() {
	}
	//何もせず描画
	template <typename T2>
	void draw(
		GraphHandle* buf, GraphHandle& skyhandle, T2 doing,
		const VECTOR_ref& campos, const VECTOR_ref& camvec, const VECTOR_ref& camup, const float& fov,
		const float& far_distance = 2000.f, const float& near_distance = 100.f) {
		//
		NearScreen.SetDraw_Screen(std::clamp(near_distance, 0.1f, far_distance), far_distance, fov, campos, camvec, camup);
		{
			skyhandle.DrawGraph(0, 0, FALSE);
			doing();
			Effekseer_Sync3DSetting();
			DrawEffekseer3D();
		}
		buf->SetDraw_Screen();
		{
			int x = 0, y = 0;
			GetGraphSize(buf->get(), &x, &y);
			DrawRotaGraph(x / 2, y / 2, 1.0, 0.0, NearScreen.get(), false);
		}
	}
	//被写体深度描画
	template <typename T2>
	void dof(
		GraphHandle* buf, GraphHandle& skyhandle, T2 doing,
		const VECTOR_ref& campos, const VECTOR_ref& camvec, const VECTOR_ref& camup, const float& fov,
		const float& far_distance = 1000.f, const float& near_distance = 100.f) {
		if (dof_flag) {
			//
			FarScreen.SetDraw_Screen(far_distance, (far_distance < 5000.f) ? 6000.0f : (far_distance + 1000.f), fov, campos, camvec, camup);
			skyhandle.DrawGraph(0, 0, FALSE);
			doing();
			Effekseer_Sync3DSetting();
			DrawEffekseer3D();
			//
			MainScreen.SetDraw_Screen(near_distance, far_distance + 50.f, fov, campos, camvec, camup);
			GraphFilter(FarScreen.get(), DX_GRAPH_FILTER_GAUSS, 16, 200);
			FarScreen.DrawGraph(0, 0, false);
			doing();
			Effekseer_Sync3DSetting();
			DrawEffekseer3D();
			//
			NearScreen.SetDraw_Screen(0.01f, near_distance + 1.f, fov, campos, camvec, camup);
			MainScreen.DrawGraph(0, 0, false);
			doing();
		}
		else {
			//
			NearScreen.SetDraw_Screen(std::clamp(near_distance, 0.1f, 2000.f), 2000.f, fov, campos, camvec, camup);
			skyhandle.DrawGraph(0, 0, FALSE);
			doing();
			Effekseer_Sync3DSetting();
			DrawEffekseer3D();
		}
		buf->SetDraw_Screen();
		NearScreen.DrawGraph(0, 0, false);
	}
	//ブルームエフェクト
	void bloom(GraphHandle& BufScreen, const int& level = 255) {
		BufScreen.DrawGraph(0, 0, false);
		if (bloom_flag) {
			GraphFilter(BufScreen.get(), DX_GRAPH_FILTER_TWO_COLOR, 245, GetColor(0, 0, 0), 255, GetColor(128, 128, 128), 255);
			GraphFilterBlt(BufScreen.get(), GaussScreen.get(), DX_GRAPH_FILTER_DOWN_SCALE, EXTEND);
			GraphFilter(GaussScreen.get(), DX_GRAPH_FILTER_GAUSS, 16, 1000);
			SetDrawMode(DX_DRAWMODE_BILINEAR);
			SetDrawBlendMode(DX_BLENDMODE_ADD, level);
			GaussScreen.DrawExtendGraph(0, 0, disp_x, disp_y, false);
			GaussScreen.DrawExtendGraph(0, 0, disp_x, disp_y, false);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			SetDrawMode(DX_DRAWMODE_NEAREST);
		}
	}

public:
};
