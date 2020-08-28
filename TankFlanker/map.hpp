#pragma once

// プレイヤー関係の定義
#define PLAYER_ENUM_DEFAULT_SIZE	1.8f	// 周囲のポリゴン検出に使用する球の初期サイズ
#define PLAYER_HIT_WIDTH			0.5f	// 当たり判定カプセルの半径
#define PLAYER_HIT_HEIGHT			1.8f	// 当たり判定カプセルの高さ
#define PLAYER_HIT_TRYNUM			16		// 壁押し出し処理の最大試行回数
#define PLAYER_HIT_SLIDE_LENGTH		0.05f	// 一度の壁押し出し処理でスライドさせる距離

class Mapclass:Mainclass {
private:
	MV1 map, map_col;					    //地面
	//MV1 tree_model, tree_far;				    //木
	MV1 sky;	  //空
	//MV1 sea;	  //海
	GraphHandle SkyScreen;
	int disp_x = 1920;
	int disp_y = 1080;

	SoundHandle envi;
public:
	Mapclass(const int& xd, const int& yd) {
		disp_x = xd;
		disp_y = yd;

		SkyScreen = GraphHandle::Make(disp_x, disp_y);    //空描画
	}
	~Mapclass() {

	}
	void Ready_map(std::string dir) {
		MV1::Load(dir + "/model.mv1", &map, true);		   //map
		MV1::Load(dir + "/col.mv1", &map_col, true);		   //mapコリジョン
		MV1::Load(dir + "/sky/model.mv1", &sky, true);	 //空
		SetUseASyncLoadFlag(TRUE);
		envi = SoundHandle::Load(dir + "/envi.wav");
		SetUseASyncLoadFlag(FALSE);
	}
	void Set_map(const char* item_txt, std::vector<Items>& item_data, std::vector<Gun>& gun_data) {
		map.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
		VECTOR_ref size;
		for (int i = 0; i < map_col.mesh_num(); i++) {
			VECTOR_ref sizetmp = map_col.mesh_maxpos(i) - map_col.mesh_minpos(i);
			if (size.x() < sizetmp.x()) {
				size.x(sizetmp.x());
			}
			if (size.y() < sizetmp.y()) {
				size.y(sizetmp.y());
			}
			if (size.z() < sizetmp.z()) {
				size.z(sizetmp.z());
			}
		}
		for (int i = 0; i < map_col.mesh_num(); i++) {
			map_col.SetupCollInfo(int(size.x() / 5.f), int(size.y() / 5.f), int(size.z() / 5.f), 0, i);
		}
		SetFogStartEnd(0.0f, 300.f);
		SetFogColor(128, 128, 128);

		item_data.clear();
		{
			int mdata = FileRead_open(item_txt, FALSE);
			//item_data
			while (true) {
				auto p = getparams::_str(mdata);
				if (getright(p.c_str()).find("end") == std::string::npos) {
					int p1 = 0;
					float p2 = 0.f, p3 = 0.f, p4 = 0.f;
					for (auto& g : gun_data) {
						if (p.find(g.name) != std::string::npos) {
							p1 = g.id;
							break;
						}
					}
					p2 = getparams::_float(mdata);
					p3 = getparams::_float(mdata);
					p4 = getparams::_float(mdata);

					item_data.resize(item_data.size() + 1);
					item_data.back().Set_item(&gun_data[p1], VGet(p2, p3, p4), MGetIdent(),0);
				}
				else {
					break;
				}
			}
			//magitem
			while (true) {
				auto p = getparams::_str(mdata);
				if (getright(p.c_str()).find("end") == std::string::npos) {
					int p1 = 0;
					float p2 = 0.f, p3 = 0.f, p4 = 0.f;
					for (auto& g : gun_data) {
						if (p.find(g.mag.name) != std::string::npos) {
							p1 = g.id;
							break;
						}
					}
					p2 = getparams::_float(mdata);
					p3 = getparams::_float(mdata);
					p4 = getparams::_float(mdata);

					item_data.resize(item_data.size() + 1);
					item_data.back().Set_item(&gun_data[p1], VGet(p2, p3, p4), MGetIdent(),1);
					if (item_data.back().ptr != nullptr) {
						item_data.back().cap = item_data.back().ptr->ammo_max;
					}
				}
				else {
					break;
				}
			}
			FileRead_close(mdata);
		}
	}
	void Start_map() {
		envi.play(DX_PLAYTYPE_LOOP, TRUE);
	}
	void Delete_map() {
		map.Dispose();		   //map
		map_col.Dispose();		   //mapコリジョン
		sky.Dispose();	 //空
		envi.Dispose();
	}
	auto& map_get() { return map; }
	auto& map_col_get() { return map_col; }
	auto map_col_line(const VECTOR_ref& startpos, const VECTOR_ref& endpos, const int&  i) {
		return map_col.CollCheck_Line(startpos, endpos, 0, i);
	}
	void map_col_wall(const VECTOR_ref& OldPos, VECTOR_ref* NowPos) {
		auto MoveVector = *NowPos - OldPos;
		// プレイヤーの周囲にあるステージポリゴンを取得する( 検出する範囲は移動距離も考慮する )
		auto HitDim = map_col.CollCheck_Sphere(OldPos, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.size(), 0, 0);
		std::vector<MV1_COLL_RESULT_POLY*> kabe_;// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておく
		// 検出されたポリゴンが壁ポリゴン( ＸＺ平面に垂直なポリゴン )か床ポリゴン( ＸＺ平面に垂直ではないポリゴン )かを判断する
		for (int i = 0; i < HitDim.HitNum; i++) {
			auto& h_d = HitDim.Dim[i];
			//壁ポリゴンと判断された場合でも、プレイヤーのＹ座標＋0.1fより高いポリゴンのみ当たり判定を行う
			if (
				(abs(atan2f(h_d.Normal.y, std::hypotf(h_d.Normal.x, h_d.Normal.z)))<=deg2rad(30))
				&& (h_d.Position[0].y > OldPos.y() + 0.1f || h_d.Position[1].y > OldPos.y() + 0.1f || h_d.Position[2].y > OldPos.y() + 0.1f)
				&& (h_d.Position[0].y < OldPos.y() + 1.6f || h_d.Position[1].y < OldPos.y() + 1.6f || h_d.Position[2].y< OldPos.y() + 1.6f)
				) {
				kabe_.emplace_back(&h_d);// ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
			}
		}
		// 壁ポリゴンとの当たり判定処理
		if (kabe_.size() > 0) {
			bool HitFlag = false;
			for (auto& k_ : kabe_) {
				if (Hit_Capsule_Tri(*NowPos, *NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, k_->Position[0], k_->Position[1], k_->Position[2])) {				// ポリゴンとプレイヤーが当たっていなかったら次のカウントへ
					HitFlag = true;// ここにきたらポリゴンとプレイヤーが当たっているということなので、ポリゴンに当たったフラグを立てる
					if (MoveVector.size() >= 0.0001f) {	// x軸かy軸方向に 0.0001f 以上移動した場合は移動したと判定
						// 壁に当たったら壁に遮られない移動成分分だけ移動する
						VECTOR_ref SlideVec = k_->Normal;
						*NowPos = SlideVec.cross(MoveVector.cross(SlideVec))+ OldPos;
						bool j = false;
						for (auto& b_ : kabe_) {
							if (Hit_Capsule_Tri(*NowPos, *NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2])) {
								j = true;
								break;// 当たっていたらループから抜ける
							}
						}
						if (!j) {
							HitFlag = false;
							break;//どのポリゴンとも当たらなかったということなので壁に当たったフラグを倒した上でループから抜ける
						}
					}
					else {
						break;
					}
				}
			}
			if (
				false//HitFlag
				) {		// 壁に当たっていたら壁から押し出す処理を行う
				for (int k = 0; k < PLAYER_HIT_TRYNUM; k++) {			// 壁からの押し出し処理を試みる最大数だけ繰り返し
					bool i = false;
					for (auto& k_ : kabe_) {
						if (Hit_Capsule_Tri(*NowPos, *NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, k_->Position[0], k_->Position[1], k_->Position[2])) {// プレイヤーと当たっているかを判定
							VECTOR_ref SlideVec = k_->Normal;
							*NowPos += SlideVec * PLAYER_HIT_SLIDE_LENGTH;					// 当たっていたら規定距離分プレイヤーを壁の法線方向に移動させる
							bool j = false;
							for (auto& b_ : kabe_) {
								if (Hit_Capsule_Tri(*NowPos, *NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2])) {// 当たっていたらループを抜ける
									j = true;
									break;
								}
							}
							if (!j) {// 全てのポリゴンと当たっていなかったらここでループ終了
								break;
							}
							i = true;
						}
					}
					if (!i) {//全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと接触しなくなったということなのでループから抜ける
						break;
					}
				}
			}
			kabe_.clear();
		}
		MV1CollResultPolyDimTerminate(HitDim);	// 検出したプレイヤーの周囲のポリゴン情報を開放する
	}

	//空描画
	GraphHandle& sky_draw(const VECTOR_ref& campos, const VECTOR_ref&camvec, const VECTOR_ref& camup, float fov) {
		SkyScreen.SetDraw_Screen(1000.0f, 5000.0f, fov, VECTOR_ref(campos) - camvec, VGet(0, 0, 0), camup);
		{
			SetFogEnable(FALSE);
			SetUseLighting(FALSE);
			sky.DrawModel();
			SetUseLighting(TRUE);
			SetFogEnable(TRUE);
		}
		return SkyScreen;
	}

};
