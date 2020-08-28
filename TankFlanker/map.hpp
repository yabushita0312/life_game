#pragma once

// �v���C���[�֌W�̒�`
#define PLAYER_ENUM_DEFAULT_SIZE	1.8f	// ���͂̃|���S�����o�Ɏg�p���鋅�̏����T�C�Y
#define PLAYER_HIT_WIDTH			0.5f	// �����蔻��J�v�Z���̔��a
#define PLAYER_HIT_HEIGHT			1.8f	// �����蔻��J�v�Z���̍���
#define PLAYER_HIT_TRYNUM			16		// �ǉ����o�������̍ő厎�s��
#define PLAYER_HIT_SLIDE_LENGTH		0.05f	// ��x�̕ǉ����o�������ŃX���C�h�����鋗��

class Mapclass:Mainclass {
private:
	MV1 map, map_col;					    //�n��
	//MV1 tree_model, tree_far;				    //��
	MV1 sky;	  //��
	//MV1 sea;	  //�C
	GraphHandle SkyScreen;
	int disp_x = 1920;
	int disp_y = 1080;

	SoundHandle envi;
public:
	Mapclass(const int& xd, const int& yd) {
		disp_x = xd;
		disp_y = yd;

		SkyScreen = GraphHandle::Make(disp_x, disp_y);    //��`��
	}
	~Mapclass() {

	}
	void Ready_map(std::string dir) {
		MV1::Load(dir + "/model.mv1", &map, true);		   //map
		MV1::Load(dir + "/col.mv1", &map_col, true);		   //map�R���W����
		MV1::Load(dir + "/sky/model.mv1", &sky, true);	 //��
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
		map_col.Dispose();		   //map�R���W����
		sky.Dispose();	 //��
		envi.Dispose();
	}
	auto& map_get() { return map; }
	auto& map_col_get() { return map_col; }
	auto map_col_line(const VECTOR_ref& startpos, const VECTOR_ref& endpos, const int&  i) {
		return map_col.CollCheck_Line(startpos, endpos, 0, i);
	}
	void map_col_wall(const VECTOR_ref& OldPos, VECTOR_ref* NowPos) {
		auto MoveVector = *NowPos - OldPos;
		// �v���C���[�̎��͂ɂ���X�e�[�W�|���S�����擾����( ���o����͈͈͂ړ��������l������ )
		auto HitDim = map_col.CollCheck_Sphere(OldPos, PLAYER_ENUM_DEFAULT_SIZE + MoveVector.size(), 0, 0);
		std::vector<MV1_COLL_RESULT_POLY*> kabe_;// �ǃ|���S���Ɣ��f���ꂽ�|���S���̍\���̂̃A�h���X��ۑ����Ă���
		// ���o���ꂽ�|���S�����ǃ|���S��( �w�y���ʂɐ����ȃ|���S�� )�����|���S��( �w�y���ʂɐ����ł͂Ȃ��|���S�� )���𔻒f����
		for (int i = 0; i < HitDim.HitNum; i++) {
			auto& h_d = HitDim.Dim[i];
			//�ǃ|���S���Ɣ��f���ꂽ�ꍇ�ł��A�v���C���[�̂x���W�{0.1f��荂���|���S���̂ݓ����蔻����s��
			if (
				(abs(atan2f(h_d.Normal.y, std::hypotf(h_d.Normal.x, h_d.Normal.z)))<=deg2rad(30))
				&& (h_d.Position[0].y > OldPos.y() + 0.1f || h_d.Position[1].y > OldPos.y() + 0.1f || h_d.Position[2].y > OldPos.y() + 0.1f)
				&& (h_d.Position[0].y < OldPos.y() + 1.6f || h_d.Position[1].y < OldPos.y() + 1.6f || h_d.Position[2].y< OldPos.y() + 1.6f)
				) {
				kabe_.emplace_back(&h_d);// �|���S���̍\���̂̃A�h���X��ǃ|���S���|�C���^�z��ɕۑ�����
			}
		}
		// �ǃ|���S���Ƃ̓����蔻�菈��
		if (kabe_.size() > 0) {
			bool HitFlag = false;
			for (auto& k_ : kabe_) {
				if (Hit_Capsule_Tri(*NowPos, *NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, k_->Position[0], k_->Position[1], k_->Position[2])) {				// �|���S���ƃv���C���[���������Ă��Ȃ������玟�̃J�E���g��
					HitFlag = true;// �����ɂ�����|���S���ƃv���C���[���������Ă���Ƃ������ƂȂ̂ŁA�|���S���ɓ��������t���O�𗧂Ă�
					if (MoveVector.size() >= 0.0001f) {	// x����y�������� 0.0001f �ȏ�ړ������ꍇ�͈ړ������Ɣ���
						// �ǂɓ���������ǂɎՂ��Ȃ��ړ������������ړ�����
						VECTOR_ref SlideVec = k_->Normal;
						*NowPos = SlideVec.cross(MoveVector.cross(SlideVec))+ OldPos;
						bool j = false;
						for (auto& b_ : kabe_) {
							if (Hit_Capsule_Tri(*NowPos, *NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2])) {
								j = true;
								break;// �������Ă����烋�[�v���甲����
							}
						}
						if (!j) {
							HitFlag = false;
							break;//�ǂ̃|���S���Ƃ�������Ȃ������Ƃ������ƂȂ̂ŕǂɓ��������t���O��|������Ń��[�v���甲����
						}
					}
					else {
						break;
					}
				}
			}
			if (
				false//HitFlag
				) {		// �ǂɓ������Ă�����ǂ��牟���o���������s��
				for (int k = 0; k < PLAYER_HIT_TRYNUM; k++) {			// �ǂ���̉����o�����������݂�ő吔�����J��Ԃ�
					bool i = false;
					for (auto& k_ : kabe_) {
						if (Hit_Capsule_Tri(*NowPos, *NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, k_->Position[0], k_->Position[1], k_->Position[2])) {// �v���C���[�Ɠ������Ă��邩�𔻒�
							VECTOR_ref SlideVec = k_->Normal;
							*NowPos += SlideVec * PLAYER_HIT_SLIDE_LENGTH;					// �������Ă�����K�苗�����v���C���[��ǂ̖@�������Ɉړ�������
							bool j = false;
							for (auto& b_ : kabe_) {
								if (Hit_Capsule_Tri(*NowPos, *NowPos + VGet(0.0f, PLAYER_HIT_HEIGHT, 0.0f), PLAYER_HIT_WIDTH, b_->Position[0], b_->Position[1], b_->Position[2])) {// �������Ă����烋�[�v�𔲂���
									j = true;
									break;
								}
							}
							if (!j) {// �S�Ẵ|���S���Ɠ������Ă��Ȃ������炱���Ń��[�v�I��
								break;
							}
							i = true;
						}
					}
					if (!i) {//�S���̃|���S���ŉ����o�������݂�O�ɑS�Ă̕ǃ|���S���ƐڐG���Ȃ��Ȃ����Ƃ������ƂȂ̂Ń��[�v���甲����
						break;
					}
				}
			}
			kabe_.clear();
		}
		MV1CollResultPolyDimTerminate(HitDim);	// ���o�����v���C���[�̎��͂̃|���S�������J������
	}

	//��`��
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
