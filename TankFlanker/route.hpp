#pragma once
#define FRAME_RATE 90.f
class main_c : Mainclass {
	std::array<GraphHandle, 3> outScreen;	//�X�N���[���o�b�t�@
	GraphHandle BufScreen;					//�X�N���[���o�b�t�@
	GraphHandle ScopeScreen;				//�X�R�[�v�p�X�N���[��
	std::vector<Gun> gun_data;				//GUN�f�[�^
	std::vector<Chara> chara;				//�L����
	std::vector<tgts> tgt_pic;				//�^�[�Q�b�g
	std::vector<Items> item_data;			//�E����A�C�e��
	bool ending = true;						//
	int sel_gun = 0;						//
	//�v���C���[����ϐ��Q
	switchs TPS, ads, chgun, delgun, usegun, change_gun;		//����X�C�b�`
	VECTOR_ref gunpos_TPS;										//�}�E�X�G�C���p�e���W
	float xrad_p = 0.f;											//�}�E�X�G�C���p�ϐ��m��
	bool running = false;										//���邩�ۂ�
	VECTOR_ref add_pos, add_pos_buf;							//�ړ�
	VECTOR_ref campos, campos_buf, camvec, camup, campos_TPS;	//�J����
	float fov = 0.f, fov_fps = 0.f;								//�J����
	size_t id_mine = 0;											//���@ID
	MV1 body_obj;												//�g�̃��f��
public:
	main_c() {
		//
		auto settings = std::make_unique<Setting>();			//�ݒ�ǂݍ���
		auto vrparts = std::make_unique<VRDraw>(settings);		//DXLib�`��
		//��ʎw��
		settings->set_dispsize();
		auto Drawparts = std::make_unique<DXDraw>("FPS_0",settings, FRAME_RATE);		/*�ėp�N���X*/
		auto UIparts = std::make_unique<UI>(settings);									/*UI*/
		auto Debugparts = std::make_unique<DeBuG>(FRAME_RATE);							/*�f�o�b�O*/
		auto Hostpassparts = std::make_unique<HostPassEffect>(settings);				/*�z�X�g�p�X�G�t�F�N�g*/
		this->outScreen[0] = GraphHandle::Make(settings->dispx, settings->dispy);		//����
		this->outScreen[1] = GraphHandle::Make(settings->dispx, settings->dispy);		/*�E��*/
		this->outScreen[2] = GraphHandle::Make(settings->dispx, settings->dispy);		//TPS�p
		this->BufScreen = GraphHandle::Make(settings->dispx, settings->dispy);			//
		this->ScopeScreen = GraphHandle::Make(1080, 1080);								//
		settings->ready_draw_setting();													//�Z�b�e�B���O
		//MV1::Load("data/model/hand/model_b.mv1", &this->body_obj, true);				//�g��
		MV1::Load("data/model/body/model.mv1", &this->body_obj, true);					//�g��
		auto mapparts = std::make_unique<Mapclass>(settings->dispx, settings->dispy);	//map
		auto tgtparts = std::make_unique<tgttmp>();										//�^�[�Q�b�g
		//GUN�f�[�^
		this->gun_data.resize(5);
		this->gun_data[0].mod.set("Knife");
		this->gun_data[1].mod.set("1911");
		this->gun_data[2].mod.set("M82A2");
		this->gun_data[3].mod.set("CAR15_M4");
		this->gun_data[4].mod.set("AK74");
		UIparts->load_window("�e���f��");					//���[�h���1
		fill_id(this->gun_data);							//GUN�f�[�^�擾1
		for (auto& g : this->gun_data) { g.set_data(); }	//GUN�f�[�^�擾2
		UIparts->load_window("�e���f��");					//���[�h���2
		auto scoreparts = std::make_unique<scores>();		//�X�R�A
		tgtparts->Set_tgtdata();							//�^�[�Q�b�g
		vrparts->Set_Device();								//VR�Z�b�g
		do {
			//�L�����ݒ�
			auto sel_g = UIparts->select_window(this->gun_data, vrparts, settings);
			if (sel_g < 0) { break; }
			chara.resize(1);
			chara[id_mine].Ready_chara(&this->gun_data[sel_g], &this->gun_data[0], this->gun_data.size(), this->body_obj, &this->ScopeScreen);
			chara[id_mine].Set_chara_Position(VGet(0.0f, 9.0f, 0.f), MGetIdent(), MATRIX_ref::RotY(DX_PI_F));			//chara[id_mine].Set_chara_Position(VGet(58.73f, 8.0f, 60.59f), MGetIdent(), MATRIX_ref::RotY(DX_PI_F));
			chara[id_mine].Set_chara(0);
			this->sel_gun = 0;
			this->usegun.ready(true);
			//�}�b�v�ǂݍ���
			mapparts->Ready_map("data/map");			//mapparts->Ready_map("data/new");
			UIparts->load_window("�}�b�v");
			mapparts->Set_map("data/maps/set.txt", this->item_data, this->gun_data);
			//�^�[�Q�b�g
			tgt_pic.clear();
			/*
			tgt_pic.resize(5);
			tgt_pic[0].Set_tgt(tgtparts, VGet(4, 0, 12.f));
			tgt_pic[1].Set_tgt(tgtparts, VGet(-4, 0, 18.f));
			tgt_pic[2].Set_tgt(tgtparts, VGet(2, 0, 27.f));
			tgt_pic[3].Set_tgt(tgtparts, VGet(-2, 0, 36.f));
			tgt_pic[4].Set_tgt(tgtparts, VGet(0, 0, 45.f));
			fill_id(tgt_pic);
			for (auto& p : tgt_pic) { p.obj.SetPosition(mapparts->map_col_line(p.obj.GetPosition() - VGet(0, -10.f, 0), p.obj.GetPosition() - VGet(0, 10.f, 0), 0).HitPosition); }
			*/
			//���C�e�B���O
			Drawparts->Set_Light_Shadow(
				mapparts->map_col_get().mesh_maxpos(0),
				mapparts->map_col_get().mesh_minpos(0),
				VGet(0.5f,-0.5f,0.5f),
				[&] {mapparts->map_get().DrawModel(); });
			//�e�ɕ`�悷����̂��w�肷��(��)
			auto draw_in_shadow = [&] {
				for (auto& p : this->tgt_pic) { p.obj.DrawModel(); }
				for (auto& c : this->chara) { c.Draw_chara(this->usegun.first, this->sel_gun); }
				for (auto& g : this->item_data) { g.Draw_item(); }
			};
			auto draw_by_shadow = [&] {
				Drawparts->Draw_by_Shadow([&] {
					mapparts->map_get().DrawModel();
					for (auto& p : this->tgt_pic) { p.obj.DrawModel(); }
					for (auto& c : this->chara) { c.Draw_chara(this->usegun.first, this->sel_gun); }
					for (auto& g : this->item_data) { g.Draw_item(this->chara[id_mine]); }
					//�e�e
					SetFogEnable(FALSE);
					SetUseLighting(FALSE);
					for (auto& c : this->chara) { c.Draw_ammo(); }
					SetUseLighting(TRUE);
					SetFogEnable(TRUE);
				});
			};
			//�J�n
			{
				scoreparts->reset();
				//�v���C���[����ϐ��Q
				this->xrad_p = 0.f;									//�}�E�X�G�C��
				this->fov = deg2rad(settings->useVR_e ? 90 : 45);	//
				this->fov_fps = this->fov;							//
				this->TPS.ready(true);
				this->ads.ready(false);
				SetMousePoint(deskx / 2, desky / 2);
				//��
				mapparts->Start_map();
				//
				for (auto& tp : tgt_pic) {
					tp.isMOVE = false;
				}
				bool cansh_gun = true;
				bool start_c = true;
				while (ProcessMessage() == 0) {
					const auto fps = GetFPS();
					const auto waits = GetNowHiPerformanceCount();
					Debugparts->put_way();
					{
						//�e�ύX
						{
							auto& c = chara[id_mine];
							if (this->usegun.first) {
								if (this->change_gun.first == true || cansh_gun == false) {
									++this->sel_gun %= c.gun_slot.size();
									if (c.gun_slot[this->sel_gun].ptr == nullptr) {
										this->sel_gun = 0;
									}
									c.Delete_chara();
									c.Set_chara(this->sel_gun);
									this->gunpos_TPS = VGet(0, 0, 0);
									this->change_gun.first = false;
								}
								cansh_gun = true;
							}
							else if (cansh_gun) {
								c.Delete_chara();
								c.Set_chara(-1);
								this->gunpos_TPS = VGet(0, 0, 0);

								if (this->sel_gun > 0) {
									--this->sel_gun;
								}
								else {
									this->sel_gun = int(c.gun_slot.size() - 1);
									while (true) {
										if (this->sel_gun == -1) {
											break;
										}
										if (c.gun_slot[this->sel_gun].ptr == nullptr) {
											this->sel_gun--;
										}
										else {
											break;
										}
									}
								}
								cansh_gun = false;
							}
						}
						//�v���C���[����
						if (settings->useVR_e) {
							auto& c = chara[id_mine];
							//HMD
							vrparts->GetDevicePositionVR(vrparts->get_hmd_num(), &c.pos_HMD, &c.mat_HMD);
							//�ړ�
							if (vrparts->get_hand2_num() != -1) {
								auto& ptr_ = (*vrparts->get_device())[vrparts->get_hand2_num()];
								if (ptr_.turn && ptr_.now) {
									if ((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) {
										//running
										running = (ptr_.on[1] & BUTTON_TOUCHPAD) != 0;
										if (this->ads.first) {
											running = false;
										}
										auto speed = (running ? 8.f : (this->ads.first ? 2.f : 4.f));

										easing_set(&this->add_pos_buf, (c.mat_HMD.zvec()*ptr_.touch.y() + c.mat_HMD.xvec()*ptr_.touch.x())*-speed / fps, 0.95f, fps);
									}
									else {
										easing_set(&this->add_pos_buf, VGet(0, 0, 0), 0.95f, fps);
									}
									if (c.add_ypos == 0.f) {
										if ((ptr_.on[0] & BUTTON_SIDE) != 0) {
											c.add_ypos = 0.05f;
										}
										this->add_pos = this->add_pos_buf;
									}
									else {
										easing_set(&this->add_pos, VGet(0, 0, 0), 0.995f, fps);
									}
								}
								c.pos += this->add_pos;
								//��������
								auto pp = mapparts->map_col_line(c.pos + VGet(0, 1.8f, 0), c.pos + VGet(0, -0.1f, 0), 0);
								if (c.add_ypos <= 0.f && pp.HitFlag == 1) {
									c.pos = pp.HitPosition;
									c.add_ypos = 0.f;
								}
								else {
									c.pos.yadd(c.add_ypos);
									c.add_ypos -= 9.8f / std::powf(fps, 2.f);
									//���A
									if (c.pos.y() <= -5.f) {
										c.pos = VGet(0.f, 5.f, 0.f);
										c.add_ypos = 0.f;
									}
								}
							}
							//RIGHTHAND
							{
								vrparts->GetDevicePositionVR(vrparts->get_hand1_num(), &c.pos_RIGHTHAND, &c.mat_RIGHTHAND);
								c.mat_RIGHTHAND = c.mat_RIGHTHAND*MATRIX_ref::RotAxis(c.mat_RIGHTHAND.xvec(), deg2rad(-60));
								easing_set(&this->campos_TPS, VGet(-0.35f, 0.15f, 1.f), 0.95f, fps);
								c.mat_RIGHTHAND = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), c.vecadd_RIGHTHAND)*c.mat_RIGHTHAND;//���R�C��
							}
							//LEFTHAND
							{
								vrparts->GetDevicePositionVR(vrparts->get_hand2_num(), &c.pos_LEFTHAND, &c.mat_LEFTHAND);
								c.mat_LEFTHAND = c.mat_LEFTHAND*MATRIX_ref::RotAxis(c.mat_LEFTHAND.xvec(), deg2rad(-60));
							}
							//pos
							{
								//�g��
								{
									VECTOR_ref v_ = c.mat_HMD.zvec();
									float x_1 = -sinf(c.body_yrad);
									float y_1 = cosf(c.body_yrad);
									float x_2 = v_.x();
									float y_2 = -v_.z();
									c.body_yrad += std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2) * FRAME_RATE / fps / 2.f;
								}
								{
									VECTOR_ref v_ = c.mat_HMD.zvec();
									float x_1 = sinf(c.body_xrad);
									float y_1 = -cosf(c.body_xrad);
									float x_2 = -v_.y();
									float y_2 = -std::hypotf(v_.x(), v_.z());
									c.body_xrad += std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2) * FRAME_RATE / fps / 2.f;
								}
								MATRIX_ref m_inv = MATRIX_ref::RotX(c.body_xrad)*MATRIX_ref::RotY(DX_PI_F + c.body_yrad);
								{
									c.body.SetFrameLocalMatrix(c.body_f.first, m_inv*MATRIX_ref::Mtrans(c.body_f.second));
									//
									for (size_t i = 0; i < c.gun_slot.size(); i++) {
										if (c.gun_slot[i].ptr != nullptr) {
											c.gun_slot[i].obj.SetMatrix(MATRIX_ref::RotY(DX_PI_F)*MATRIX_ref::RotX(DX_PI_F / 2) *m_inv * MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(VGet((float(i) - float(c.gun_slot.size()) *0.5f)*0.1f, -0.15f, 0.3f), m_inv) + (c.pos_HMD + c.pos)));
										}
									}
								}
								//����
								c.body.SetFrameLocalMatrix(c.head_f.first, c.mat_HMD*m_inv.Inverse()*MATRIX_ref::Mtrans(c.head_f.second));
								//���_�擾
								c.body.SetMatrix(c.mat*MATRIX_ref::Mtrans(c.pos));
								//�E��
								{
									VECTOR_ref vec_a1 = MATRIX_ref::Vtrans(((c.pos + c.pos_RIGHTHAND) - c.body.frame(c.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());//�
									VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0�Ƃ���
									VECTOR_ref vec_a1L2 = VECTOR_ref(VGet(-1.f, 0.f, vec_a1.x() / vec_a1.z())).Norm();//y=0�Ƃ���
									float rad_t = getcos_tri((c.body.frame(c.RIGHThand_f.first) - c.body.frame(c.RIGHTarm2_f.first)).size(), (c.body.frame(c.RIGHTarm2_f.first) - c.body.frame(c.RIGHTarm1_f.first)).size(), (c.body.frame(c.RIGHTarm1_f.first) - (c.pos + c.pos_RIGHTHAND)).size());
									VECTOR_ref vec_t = vec_a1 * rad_t + vec_a1L1 * std::sqrtf(1.f - rad_t * rad_t);
									//
									c.body.SetFrameLocalMatrix(c.RIGHTarm1_f.first, m_inv.Inverse()*MATRIX_ref::Mtrans(c.RIGHTarm1_f.second));
									MATRIX_ref a1_inv = MATRIX_ref::RotVec2(c.body.frame(c.RIGHTarm2_f.first) - c.body.frame(c.RIGHTarm1_f.first), vec_t);
									c.body.SetFrameLocalMatrix(c.RIGHTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(c.RIGHTarm1_f.second));
									//
									c.body.SetFrameLocalMatrix(c.RIGHTarm2_f.first, m_inv.Inverse()*a1_inv.Inverse()*MATRIX_ref::Mtrans(c.RIGHTarm2_f.second));
									MATRIX_ref a2_inv = MATRIX_ref::RotVec2(c.body.frame(c.RIGHThand_f.first) - c.body.frame(c.RIGHTarm2_f.first), MATRIX_ref::Vtrans(((c.pos + c.pos_RIGHTHAND) - c.body.frame(c.RIGHTarm2_f.first)).Norm(), m_inv.Inverse()));
									c.body.SetFrameLocalMatrix(c.RIGHTarm2_f.first, a2_inv*a1_inv.Inverse()*MATRIX_ref::Mtrans(c.RIGHTarm2_f.second));
									//
									c.body.SetFrameLocalMatrix(c.RIGHThand_f.first, m_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.RIGHThand_f.second));
									MATRIX_ref hand_inv = MATRIX_ref::RotVec2(c.body.frame(c.RIGHThand2_f.first) - c.body.frame(c.RIGHThand_f.first), MATRIX_ref::Vtrans(c.mat_RIGHTHAND.zvec()*-1.f, m_inv.Inverse()));
									c.body.SetFrameLocalMatrix(c.RIGHThand_f.first, hand_inv*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.RIGHThand_f.second));
								}
								//�e��
								c.obj.SetMatrix(c.mat_RIGHTHAND*MATRIX_ref::Mtrans(c.pos_RIGHTHAND + c.pos));
								//����
								{
									float dist_ = ((c.pos_LEFTHAND + c.pos) - c.obj.frame(c.ptr_now->frame[6].first)).size();
									if (dist_ <= 0.2f && (!c.reloadf || !c.down_mag)) {
										c.LEFT_hand = true;
										//MATRIX_ref m4 = MATRIX_ref::RotZ(deg2rad(-90));
										//	c.body.SetFrameLocalMatrix(17, m4*MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(c.obj.frame(c.ptr_now->frame[6].first) - (c.pos_HMD + c.pos), c.mat_HMD.Inverse())));
										c.pos_LEFTHAND = c.obj.frame(c.ptr_now->frame[6].first) - c.pos;
									}
									else {
										c.LEFT_hand = false;
										//MATRIX_ref m4 = MATRIX_ref::RotZ(deg2rad(-90))*c.mat_LEFTHAND*c.mat_HMD.Inverse();
										//	c.body.SetFrameLocalMatrix(17, m4*MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(c.pos_LEFTHAND - c.pos_HMD, c.mat_HMD.Inverse())));
									}
									{
										VECTOR_ref vec_a1 = MATRIX_ref::Vtrans(((c.pos + c.pos_LEFTHAND) - c.body.frame(c.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//�
										VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0�Ƃ���
										VECTOR_ref vec_a1L2 = VECTOR_ref(VGet(-1.f, 0.f, vec_a1.x() / vec_a1.z())).Norm();//y=0�Ƃ���
										float rad_t = getcos_tri((c.body.frame(c.LEFThand_f.first) - c.body.frame(c.LEFTarm2_f.first)).size(), (c.body.frame(c.LEFTarm2_f.first) - c.body.frame(c.LEFTarm1_f.first)).size(), (c.body.frame(c.LEFTarm1_f.first) - (c.pos + c.pos_LEFTHAND)).size());
										VECTOR_ref vec_t = vec_a1 * rad_t + vec_a1L1 * std::sqrtf(1.f - rad_t * rad_t);
										//
										c.body.SetFrameLocalMatrix(c.LEFTarm1_f.first, m_inv.Inverse()*MATRIX_ref::Mtrans(c.LEFTarm1_f.second));
										MATRIX_ref a1_inv = MATRIX_ref::RotVec2(c.body.frame(c.LEFTarm2_f.first) - c.body.frame(c.LEFTarm1_f.first), vec_t);
										c.body.SetFrameLocalMatrix(c.LEFTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(c.LEFTarm1_f.second));
										//
										c.body.SetFrameLocalMatrix(c.LEFTarm2_f.first, m_inv.Inverse()*a1_inv.Inverse()*MATRIX_ref::Mtrans(c.LEFTarm2_f.second));
										MATRIX_ref a2_inv = MATRIX_ref::RotVec2(c.body.frame(c.LEFThand_f.first) - c.body.frame(c.LEFTarm2_f.first), MATRIX_ref::Vtrans(((c.pos + c.pos_LEFTHAND) - c.body.frame(c.LEFTarm2_f.first)).Norm(), m_inv.Inverse()));
										c.body.SetFrameLocalMatrix(c.LEFTarm2_f.first, a2_inv*a1_inv.Inverse()*MATRIX_ref::Mtrans(c.LEFTarm2_f.second));
										//
										c.body.SetFrameLocalMatrix(c.LEFThand_f.first, m_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.LEFThand_f.second));
										MATRIX_ref hand_inv = MATRIX_ref::RotVec2(c.body.frame(c.LEFThand2_f.first) - c.body.frame(c.LEFThand_f.first), MATRIX_ref::Vtrans((c.obj.frame(c.ptr_now->frame[6].first) - c.obj.frame(c.ptr_now->frame[6].first + 1)).Norm(), m_inv.Inverse()));
										c.body.SetFrameLocalMatrix(c.LEFThand_f.first, hand_inv*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.LEFThand_f.second));
									}
								}
							}
							//�e����
							if (c.obj.get_anime(3).per == 1.f) {
								c.audio.slide.play_3D(c.pos + c.pos_RIGHTHAND, 1.f);
							}
							c.obj.get_anime(3).per = std::max(c.obj.get_anime(3).per - 12.f / fps, 0.f);
							//����
							{
								if (vrparts->get_hand1_num() != -1) {
									auto& ptr_ = (*vrparts->get_device())[vrparts->get_hand1_num()];
									if (ptr_.turn && ptr_.now) {
										//������
										easing_set(&c.obj.get_anime(2).per, float((ptr_.on[0] & BUTTON_TRIGGER) != 0), 0.5f, fps);
										//�}�O�L���b�`
										easing_set(&c.obj.get_anime(5).per, float((ptr_.on[0] & BUTTON_SIDE) != 0), 0.5f, fps);
										//�Z���N�^�[
										c.selkey.get_in(((ptr_.on[0] & BUTTON_TOUCHPAD) != 0) && (ptr_.touch.x() > 0.5f && ptr_.touch.y() < 0.5f&&ptr_.touch.y() > -0.5f));
										//�e�̎g�p
										this->usegun.get_in((CheckHitKey(KEY_INPUT_P) != 0) && (this->sel_gun != -1));//<---
										if (this->sel_gun == -1) {
											this->usegun.first = false;
										}
										//�����ύX
										this->change_gun.get_in(((ptr_.on[0] & BUTTON_TOPBUTTON) != 0) && this->usegun.first);
									}
								}
								if (vrparts->get_hand2_num() != -1) {
									auto& ptr_ = (*vrparts->get_device())[vrparts->get_hand2_num()];
									if (ptr_.turn && ptr_.now) {
										//�}�K�W���擾
										c.down_mag |= (((ptr_.on[0] & BUTTON_TRIGGER) != 0) && (c.gun_stat[c.ptr_now->id].mag_in.size() >= 1));
										//�e�ύX
										this->chgun.get_in((ptr_.on[0] & BUTTON_TOPBUTTON) != 0);
										//�e�j��
										this->delgun.get_in((CheckHitKey(KEY_INPUT_G) != 0) && this->usegun.first);
										//�^�C�}�[�I��
										scoreparts->ready_f |= ((ptr_.on[0] & BUTTON_SIDE) != 0);
										//�v�����Z�b�g
										if ((ptr_.on[0] & BUTTON_TOUCHPAD) != 0 && scoreparts->end_f) { scoreparts->reset(); }
									}
								}
							}
							//�ˌ��֘A
							{
								{
									easing_set(&c.vecadd_RIGHTHAND, c.vecadd_RIGHTHAND_p, 0.9f, fps);
									easing_set(&c.vecadd_RIGHTHAND_p, VGet(0, 0, 1.f), 0.975f, fps);
									//���R�C��
									if (c.gunf) {
										if (c.ammo_cnt >= 1) {
											c.obj.get_anime(0).per = 1.f;
											c.obj.get_anime(1).per = 0.f;
											c.obj.get_anime(0).time += 60.f / fps;
											if (c.obj.get_anime(0).time >= c.obj.get_anime(0).alltime) {
												c.obj.get_anime(0).time = 0.f;
												c.gunf = false;
											}
										}
										else {
											c.obj.get_anime(1).per = 1.f;
											c.obj.get_anime(0).per = 0.f;
											c.obj.get_anime(1).time += 60.f / fps;
											if (c.obj.get_anime(1).time >= c.obj.get_anime(1).alltime) {
												c.obj.get_anime(1).time = c.obj.get_anime(1).alltime;
												c.gunf = false;
											}
										}

									}
									if (c.ptr_now->cate == 1) {
										//�}�K�W���r�o
										if (c.obj.get_anime(5).per >= 0.5f && !c.reloadf && c.gun_stat[c.ptr_now->id].mag_in.size() >= 1) {
											c.audio.mag_down.play_3D(c.pos + c.pos_RIGHTHAND, 1.f);
											int dnm = int(c.ammo_cnt) - 1;
											//�e��
											if (c.ammo_cnt >= 1) {
												c.ammo_cnt = 1;
											}
											else {
												dnm = 0;
											}
											c.gun_stat[c.gun_slot[this->sel_gun].ptr->id].in -= dnm;
											//�o�C�u���[�V�����@�o�b�e���[������������߃R�����g�A�E�g
											/*
												vrparts->Haptic(vrparts->get_hand1_num(), unsigned short(60000));
											*/
											//�}�K�W���r�o
											c.reload_cnt = 0.f;
											c.gun_stat[c.ptr_now->id].mag_in.erase(c.gun_stat[c.ptr_now->id].mag_in.begin());
											//�}�K�W���r�o
											bool tt = false;
											for (auto& g : this->item_data) {
												if (g.ptr == nullptr && g.cate == 1) {
													tt = true;
													g.Set_item(c.ptr_now, c.pos_mag, c.mat_mag, 1);
													g.add = (c.obj.frame(c.ptr_now->frame[1].first) - c.obj.frame(c.ptr_now->frame[0].first)).Norm()*-1.f / fps;//�r䰃x�N�g��
													g.cap = dnm;
													break;
												}
											}
											if (!tt) {
												this->item_data.resize(this->item_data.size() + 1);
												auto& g = this->item_data.back();
												g.Set_item(c.ptr_now, c.pos_mag, c.mat_mag, 1);
												g.add = (c.obj.frame(c.ptr_now->frame[1].first) - c.obj.frame(c.ptr_now->frame[0].first)).Norm()*-1.f / fps;//�r䰃x�N�g��
												g.cap = dnm;
											}
											//
											c.reloadf = true;
										}
										//�Z���N�^�[
										if (c.selkey.second == 1) {
											++c.gun_stat[c.gun_slot[this->sel_gun].ptr->id].select %= c.ptr_now->select.size();
										}
									}
									//
									if (c.reloadf && c.gun_stat[c.ptr_now->id].mag_in.size() >= 1) {
										c.reload_cnt += 1.f / fps;
										if (c.reload_cnt < c.ptr_now->reload_time) {
											c.down_mag = false;
										}
									}
									//�Z�t�e�B
									easing_set(&c.obj.get_anime(4).per, float(0.f), 0.5f, fps);
									//�ˌ�
									if (!c.gunf && c.ammo_cnt >= 1) {
										if (c.ptr_now->select[c.gun_stat[c.gun_slot[this->sel_gun].ptr->id].select] == 2) {//�t���I�[�g�p
											c.trigger.second = 0;
										}
									}
									c.trigger.get_in(c.obj.get_anime(2).per >= 0.5f);
									if (c.trigger.second == 1) {
										c.audio.trigger.play_3D(c.pos + c.pos_RIGHTHAND, 1.f);
									}
									switch (c.ptr_now->cate) {
									case 0:
									{
										//�ߐ�
										if (c.trigger.second == 1 && !c.gunf) {
											c.gunf = true;
											//�o�C�u���[�V�����@�o�b�e���[������������߃R�����g�A�E�g
											/*
												vrparts->Haptic(vrparts->get_hand1_num(), unsigned short(60000));
											*/
											//�T�E���h
											c.audio.shot.play_3D(c.pos + c.pos_RIGHTHAND, 1.f);
											c.audio.slide.play_3D(c.pos + c.pos_RIGHTHAND, 1.f);
											//�X�R�A
											if (scoreparts->start_f && !scoreparts->end_f) {
												scoreparts->sub(-4);
											}
										}
										break;
									}
									case 1:
									{
										if (c.trigger.second == 1 && !c.gunf && c.ammo_cnt >= 1) {
											c.gunf = true;
											//�o�C�u���[�V�����@�o�b�e���[������������߃R�����g�A�E�g
											/*
												vrparts->Haptic(vrparts->get_hand1_num(), unsigned short(60000));
											*/
											if (!settings->useVR_e && this->ads.first) {
												this->fov_fps *= 0.95f;
											}
											//�e���Ǘ�
											c.ammo_cnt--;
											c.gun_stat[c.gun_slot[this->sel_gun].ptr->id].in--;
											if (!c.reloadf && c.gun_stat[c.gun_slot[this->sel_gun].ptr->id].mag_in.size() >= 1 && c.gun_stat[c.gun_slot[this->sel_gun].ptr->id].mag_in.front() > 0) {
												c.gun_stat[c.gun_slot[this->sel_gun].ptr->id].mag_in.front()--;
											}
											//����������ƃR�R�����E�����
											c.vecadd_RIGHTHAND_p = MATRIX_ref::Vtrans(c.vecadd_RIGHTHAND_p,
												MATRIX_ref::RotY(deg2rad(float(int(c.ptr_now->recoil_xdn*100.f) + GetRand(int((c.ptr_now->recoil_xup - c.ptr_now->recoil_xdn)*100.f))) / (100.f*(c.LEFT_hand ? 3.f : 1.f))))*
												MATRIX_ref::RotX(deg2rad(float(int(c.ptr_now->recoil_ydn*100.f) + GetRand(int((c.ptr_now->recoil_yup - c.ptr_now->recoil_ydn)*100.f))) / (100.f*(c.LEFT_hand ? 3.f : 1.f)))));
											//�e
											c.bullet[c.use_bullet].set(&c.ptr_now->ammo[0], c.obj.frame(c.ptr_now->frame[3].first), c.mat_RIGHTHAND.zvec()*-1.f);
											//���
											c.cart[c.use_bullet].set(&c.ptr_now->ammo[0], c.obj.frame(c.ptr_now->frame[2].first), (c.obj.frame(c.ptr_now->frame[2].first + 1) - c.obj.frame(c.ptr_now->frame[2].first)).Norm()*2.5f / fps, c.mat_RIGHTHAND);
											//�G�t�F�N�g
											set_effect(&c.effcs[ef_fire], c.obj.frame(c.ptr_now->frame[3].first), c.mat_RIGHTHAND.zvec()*-1.f, 0.0025f / 0.1f);

											set_effect(&c.effcs_gun[c.use_effcsgun].effect, c.obj.frame(c.ptr_now->frame[3].first), c.mat_RIGHTHAND.zvec()*-1.f, 0.11f / 0.1f);
											set_pos_effect(&c.effcs_gun[c.use_effcsgun].effect, Drawparts->get_effHandle(ef_smoke));
											c.effcs_gun[c.use_effcsgun].ptr = &c.bullet[c.use_bullet];
											c.effcs_gun[c.use_effcsgun].cnt = 0.f;
											++c.use_effcsgun %= c.effcs_gun.size();
											//�T�E���h
											c.audio.shot.play_3D(c.pos + c.pos_RIGHTHAND, 1.f);
											c.audio.slide.play_3D(c.pos + c.pos_RIGHTHAND, 1.f);
											//����ID��
											++c.use_bullet %= c.bullet.size();
											//�X�R�A
											if (scoreparts->start_f && !scoreparts->end_f) {
												scoreparts->sub(-4);
											}
										}
										//�}�K�W���擾
										if (c.reloadf && c.gun_stat[c.ptr_now->id].mag_in.size() >= 1) {
											if (c.down_mag) {
												auto p = MATRIX_ref::RotVec2(c.mat_LEFTHAND.yvec(), (c.obj.frame(c.ptr_now->frame[0].first) - (c.pos_LEFTHAND + c.pos)));
												c.mat_mag = c.mag.GetFrameLocalMatrix(3)* (c.mat_LEFTHAND*p);
												c.pos_mag = c.pos_LEFTHAND + c.pos;
												if ((c.mag.frame(3) - c.obj.frame(c.ptr_now->frame[0].first)).size() <= 0.1f) {
													c.obj.get_anime(1).time = 0.f;
													c.obj.get_anime(0).per = 1.f;
													c.obj.get_anime(1).per = 0.f;
													if (c.ammo_cnt == 0) {
														c.obj.get_anime(3).per = 1.f;
													}
													c.audio.mag_set.play_3D(c.pos + c.pos_RIGHTHAND, 1.f);
													c.ammo_cnt += c.gun_stat[c.gun_slot[this->sel_gun].ptr->id].mag_in.front();
													c.reloadf = false;
												}
											}
										}
										else {
											c.down_mag = false;
											c.mat_mag = c.mat_RIGHTHAND;
											c.pos_mag = c.obj.frame(c.ptr_now->frame[1].first);
										}
										break;
									}
									}
								}
								c.mag.SetMatrix(c.mat_mag* MATRIX_ref::Mtrans(c.pos_mag));
								c.obj.work_anime();
								for (auto& a : c.bullet) {
									if (a.flug) {
										a.repos = a.pos;
										a.pos += a.vec * (a.spec->speed / fps);
										//����
										{
											auto p = mapparts->map_col_line(a.repos, a.pos, 0);
											if (p.HitFlag == TRUE) {
												a.pos = p.HitPosition;
											}
											for (auto& tp : tgt_pic) {
												auto q = tp.obj.CollCheck_Line(a.repos, a.pos, 0, 1);
												if (q.HitFlag == TRUE) {
													a.pos = q.HitPosition;
													//
													tp.power = (tp.obj.frame(tgtparts->frame.first) - a.pos).y();
													tp.time = 0.f;
													int UI_xpos = 0;
													int UI_ypos = 0;
													//�e������
													{
														tp.pic.SetDraw_Screen(false);
														VECTOR_ref pvecp = (a.pos - tp.obj.frame(tgtparts->frame.first));
														VECTOR_ref xvecp = MATRIX_ref::Vtrans(VGet(0, 0, 0), tp.obj.GetFrameLocalMatrix(tp.frame_x.first));
														VECTOR_ref yvecp = MATRIX_ref::Vtrans(VGet(0, 0, 0), tp.obj.GetFrameLocalMatrix(tp.frame_y.first));
														UI_xpos = int(float(tgtparts->x_size)*(xvecp.Norm().dot(pvecp)) / xvecp.size());//X����
														UI_ypos = int(float(tgtparts->y_size)*(yvecp.Norm().dot(pvecp)) / yvecp.size());//Y����
														DrawCircle(UI_xpos, UI_ypos, 10, GetColor(255, 0, 0));//�e��
													}
													//�|�C���g����
													if (scoreparts->start_f && !scoreparts->end_f) {
														int r_, g_, b_, a_;
														GetPixelSoftImage(tgtparts->col_tex, UI_xpos, UI_ypos, &r_, &g_, &b_, &a_);
														int pt = 0;
														switch (r_) {
														case 0:
															pt = 15;
															break;
														case 44:
															pt = 10;
															break;
														case 86:
															pt = 9;
															break;
														case 128:
															pt = 8;
															break;
														case 170:
															pt = 7;
															break;
														case 212:
															pt = 6;
															break;
														default:
															pt = 5;
															break;
														}
														scoreparts->add(int(float(pt) * ((a.pos - (c.pos + c.pos_RIGHTHAND)).size() / 9.144f)*(a.spec->damage / 10.f)));
													}
													//
													set_effect(&c.effcs[ef_reco], a.pos, q.Normal, 0.011f / 0.1f);
													//
													a.hit = true;
													a.flug = false;
													break;
												}
											}
											if (p.HitFlag == TRUE && a.flug) {
												a.flug = false;
												set_effect(&c.effcs_gndhit[c.use_effcsgndhit], a.pos, p.Normal, 0.025f / 0.1f);
												++c.use_effcsgndhit %= c.effcs_gndhit.size();
											}
										}
										//����(3�b�������A�X�s�[�h��0�ȉ��A�ђʂ�0�ȉ�)
										if (a.cnt >= 3.f || a.spec->speed < 0.f || a.spec->pene <= 0.f) {
											a.flug = false;
										}
										//�I��
										if (!a.flug) {
											for (auto& b : c.effcs_gun) {
												if (b.ptr == &a) {
													b.cnt = 2.5f;
													b.effect.handle.SetPos(b.ptr->pos);
													break;
												}
											}
										}
										//
									}
								}
								//��䰂̏���
								for (auto& a : c.cart) {
									a.get(mapparts, c);
								}
								for (auto& t : c.effcs) {
									if (t.id != ef_smoke) {
										set_pos_effect(&t, Drawparts->get_effHandle(int(t.id)));
									}
								}
								for (auto& t : c.effcs_gndhit) {
									set_pos_effect(&t, Drawparts->get_gndhitHandle());
								}
								for (auto& a : c.effcs_gun) {
									if (a.ptr != nullptr) {
										if (a.ptr->flug) {
											a.effect.handle.SetPos(a.ptr->pos);
										}
										if (a.cnt >= 0.f) {
											a.cnt += 1.f / fps;
											if (a.cnt >= 3.f) {
												a.effect.handle.Stop();
												a.cnt = -1.f;
											}
										}
									}
								}
							}
							//item
							{
								c.canget_gunitem = false;
								c.canget_magitem = false;
								for (auto& g : this->item_data) {
									g.Get_item(c, item_data, mapparts, chgun, sel_gun, usegun, change_gun, gunpos_TPS);
								}
								//�e�𗎂Ƃ�
								if (this->delgun.second == 1) {
									bool tt = false;
									for (auto& g : this->item_data) {
										if (g.ptr == nullptr && g.cate == 0) {
											tt = true;
											g.Set_item(c.ptr_now, c.pos + c.pos_RIGHTHAND, c.mat_RIGHTHAND, 0);
											break;
										}
									}
									if (!tt) {
										this->item_data.resize(this->item_data.size() + 1);
										auto& g = this->item_data.back();
										g.Set_item(c.ptr_now, c.pos + c.pos_RIGHTHAND, c.mat_RIGHTHAND, 0);
									}
									//
									for (size_t i = 0; i < c.gun_slot.size(); i++) {
										if (this->sel_gun == i) {
											if ((this->sel_gun == c.gun_slot.size() - 1) || c.gun_slot[std::clamp(this->sel_gun + 1, 0, int(c.gun_slot.size()) - 1)].ptr == nullptr) {
												c.gun_slot[this->sel_gun].ptr = nullptr;
												c.gun_slot[this->sel_gun].obj.Dispose();
												this->sel_gun--;
												break;
											}
											else {
												for (size_t j = i; j < c.gun_slot.size() - 1; j++) {
													c.gun_slot[j].delete_gun();
													c.gun_slot[j].set(c.gun_slot[j + 1].ptr);
												}
												c.gun_slot.back().delete_gun();
												break;
											}
										}
									}
									//
									c.Delete_chara();
									c.Set_chara(this->sel_gun);
									this->gunpos_TPS = VGet(0, 0, 0);
								}
							}
						}
						else {
							auto& c = chara[id_mine];
							//HMD_mat
							{
								auto qkey = (CheckHitKey(KEY_INPUT_Q) != 0);
								auto ekey = (CheckHitKey(KEY_INPUT_E) != 0);
								{
									c.mat_HMD *= MATRIX_ref::RotAxis(c.mat_HMD.zvec(), c.body_zrad).Inverse();
									if (qkey) {
										easing_set(&c.body_zrad, deg2rad(-30), 0.9f, fps);
									}
									else if (ekey) {
										easing_set(&c.body_zrad, deg2rad(30), 0.9f, fps);
									}
									else {
										easing_set(&c.body_zrad, 0.f, 0.9f, fps);
									}
									c.mat_HMD *= MATRIX_ref::RotAxis(c.mat_HMD.zvec(), c.body_zrad);
								}

								int x_m, y_m;
								GetMousePoint(&x_m, &y_m);
								c.mat_HMD = MATRIX_ref::RotX(-this->xrad_p)*c.mat_HMD;
								this->xrad_p = std::clamp(this->xrad_p - deg2rad(std::clamp(y_m - desky / 2, -120, 120))*0.1f*this->fov_fps / this->fov, deg2rad(-45), deg2rad(45));
								c.mat_HMD *= MATRIX_ref::RotY(deg2rad(std::clamp(x_m - deskx / 2, -120, 120))*0.1f*this->fov_fps / this->fov);
								c.mat_HMD = MATRIX_ref::RotX(this->xrad_p)*c.mat_HMD;
								SetMousePoint(deskx / 2, desky / 2);
								SetMouseDispFlag(FALSE);
							}
							//�ړ�
							{
								auto wkey = (CheckHitKey(KEY_INPUT_W) != 0);
								auto skey = (CheckHitKey(KEY_INPUT_S) != 0);
								auto akey = (CheckHitKey(KEY_INPUT_A) != 0);
								auto dkey = (CheckHitKey(KEY_INPUT_D) != 0);
								auto jampkey = (CheckHitKey(KEY_INPUT_SPACE) != 0);
								running = (CheckHitKey(KEY_INPUT_LSHIFT) != 0);
								if (this->ads.first) {
									running = false;
								}
								auto speed = (running ? 8.f : (this->ads.first ? 2.f : 4.f));
								VECTOR_ref zv_t = c.mat_HMD.zvec();
								zv_t.y(0.f);
								zv_t = zv_t.Norm();

								VECTOR_ref xv_t = c.mat_HMD.xvec();
								xv_t.y(0.f);
								xv_t = xv_t.Norm();

								if (wkey) {
									easing_set(&this->add_pos_buf, zv_t*-speed / fps, 0.95f, fps);
								}
								if (skey) {
									easing_set(&this->add_pos_buf, zv_t*speed / fps, 0.95f, fps);
								}
								if (akey) {
									easing_set(&this->add_pos_buf, xv_t*speed / fps, 0.95f, fps);
								}
								if (dkey) {
									easing_set(&this->add_pos_buf, xv_t*-speed / fps, 0.95f, fps);
								}
								if (!wkey && !skey && !akey && !dkey) {
									easing_set(&this->add_pos_buf, VGet(0, 0, 0), 0.95f, fps);
								}
								if (c.add_ypos == 0.f) {
									if (jampkey) {
										c.add_ypos = 0.05f;
									}
									this->add_pos = this->add_pos_buf;
								}
								else {
									easing_set(&this->add_pos, VGet(0, 0, 0), 0.995f, fps);
								}
								{
									VECTOR_ref pos_t = c.pos;
									pos_t += this->add_pos;
									//�ǔ���
									{
										mapparts->map_col_wall(c.pos, &pos_t);
										if ((this->add_pos - (pos_t - c.pos)).size() != 0.f) {
											this->add_pos = pos_t - c.pos;
											if (c.add_ypos == 0.f) {
												this->add_pos_buf = this->add_pos;
											}
										}
									}
									//��������
									{
										auto pp = mapparts->map_col_line(pos_t + VGet(0, 1.6f, 0), pos_t, 0);
										if (c.add_ypos <= 0.f && pp.HitFlag == 1) {
											if (VECTOR_ref(VGet(0, 1.f, 0.f)).dot(pp.Normal) >= cos(deg2rad(30))) {
												pos_t = pp.HitPosition;
											}
											else {
												//�u���b�N����x�N�g��
												auto v_t = VECTOR_ref(pp.Normal);
												v_t.y(0);
												v_t = v_t.Norm();
												//
												pos_t -= this->add_pos;
												this->add_pos += v_t * this->add_pos.cross(v_t.cross(this->add_pos).Norm()).size();
												if (c.add_ypos == 0.f) {
													this->add_pos_buf = this->add_pos;
												}
												pos_t += this->add_pos;
											}
											c.add_ypos = 0.f;
										}
										else {
											pos_t.yadd(c.add_ypos);
											c.add_ypos -= 9.8f / std::powf(fps, 2.f);
											//���A
											if (pos_t.y() <= -5.f) {
												pos_t = VGet(0.f, 9.f, 0.f);
												c.add_ypos = 0.f;
												c.body.SetMatrix(c.mat*MATRIX_ref::Mtrans(pos_t));
												c.body.PhysicsResetState();
											}
										}
									}
									//��������
									c.pos = pos_t;
								}
							}
							//pos
							{
								//�g��
								{
									VECTOR_ref v_ = c.mat_HMD.zvec();
									float x_1 = -sinf(c.body_yrad);
									float y_1 = cosf(c.body_yrad);
									float x_2 = v_.x();
									float y_2 = -v_.z();
									float r_ = std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
									c.body_yrad += r_ * FRAME_RATE / fps / 2.f;
								}
								{
									VECTOR_ref v_ = c.mat_HMD.zvec();
									float x_1 = sinf(c.body_xrad);
									float y_1 = -cosf(c.body_xrad);
									float x_2 = -v_.y();
									float y_2 = -std::hypotf(v_.x(), v_.z());
									c.body_xrad += std::atan2f(x_1*y_2 - x_2 * y_1, x_1*x_2 + y_1 * y_2);
								}
								MATRIX_ref m_inv = MATRIX_ref::RotY(deg2rad(30))*MATRIX_ref::RotZ(c.body_zrad)*MATRIX_ref::RotX(c.body_xrad)*MATRIX_ref::RotY(DX_PI_F + c.body_yrad);
								MATRIX_ref mb_inv = MATRIX_ref::RotY(deg2rad(15))*MATRIX_ref::RotY(DX_PI_F + c.body_yrad);
								MATRIX_ref mg_inv = MATRIX_ref::RotY(DX_PI_F + c.body_yrad);
								if (c.reloadf) {
									m_inv = MATRIX_ref::RotZ(c.body_zrad)*MATRIX_ref::RotX(c.body_xrad)*MATRIX_ref::RotY(DX_PI_F + c.body_yrad);
									mb_inv = MATRIX_ref::RotY(DX_PI_F + c.body_yrad);
									mg_inv = MATRIX_ref::RotY(DX_PI_F + c.body_yrad);
								}

								{
									//
									c.body.SetFrameLocalMatrix(c.bodyg_f.first, mg_inv*MATRIX_ref::Mtrans(c.bodyg_f.second));
									c.body.SetFrameLocalMatrix(c.bodyb_f.first, mb_inv*mg_inv.Inverse()*MATRIX_ref::Mtrans(c.bodyb_f.second));
									c.body.SetFrameLocalMatrix(c.body_f.first, m_inv*mb_inv.Inverse()*MATRIX_ref::Mtrans(c.body_f.second));
									//
									for (size_t i = 0; i < c.gun_slot.size(); i++) {
										if (c.gun_slot[i].ptr != nullptr) {
											c.gun_slot[i].obj.SetMatrix(MATRIX_ref::RotY(DX_PI_F)*MATRIX_ref::RotX(DX_PI_F / 2) *m_inv * MATRIX_ref::Mtrans(MATRIX_ref::Vtrans(VGet((float(i) - float(c.gun_slot.size()) *0.5f)*0.1f, -0.15f, 0.3f), m_inv) + (c.pos_HMD + c.pos)));
										}
									}
								}
								//����
								c.body.SetFrameLocalMatrix(c.head_f.first, c.mat_HMD*m_inv.Inverse()*MATRIX_ref::Mtrans(c.head_f.second));
								if (c.reloadf) {
									c.body.frame_reset(c.head_f.first);
								}
								//���_�擾
								{
									c.body.SetMatrix(c.mat*MATRIX_ref::Mtrans(c.pos));
									c.pos_HMD = (c.body.frame(c.RIGHTeye_f.first) + (c.body.frame(c.LEFTeye_f.first) - c.body.frame(c.RIGHTeye_f.first))*0.5f) - c.pos;
								}
								auto ratio_t = this->add_pos.size() / ((running ? 8.f : (this->ads.first ? 2.f : 4.f)) / fps);
								//�E�l�����w
								{
									c.body.get_anime(0).per = 1.f;
									c.body.get_anime(5).per = c.obj.get_anime(2).per;
								}
								//��
								if (running) {
									easing_set(&c.body.get_anime(2).per, 1.f*ratio_t, 0.95f, fps);
									easing_set(&c.body.get_anime(1).per, 0.f, 0.95f, fps);
								}
								else if (this->ads.first) {
									easing_set(&c.body.get_anime(2).per, 0.f, 0.95f, fps);
									easing_set(&c.body.get_anime(1).per, 0.5f*ratio_t, 0.95f, fps);
								}
								else {
									easing_set(&c.body.get_anime(2).per, 0.f, 0.95f, fps);
									easing_set(&c.body.get_anime(1).per, 1.f*ratio_t, 0.95f, fps);
									easing_set(&c.body.get_anime(0).per, 0.f, 0.95f, fps);
								}
								c.body.get_anime(1).time += 30.f / fps;
								if (c.body.get_anime(1).time >= c.body.get_anime(1).alltime) {
									c.body.get_anime(1).time = 0.f;
								}
								c.body.get_anime(2).time += 30.f / fps;
								if (c.body.get_anime(2).time >= c.body.get_anime(2).alltime) {
									c.body.get_anime(2).time = 0.f;
								}

								if (running) {
									c.body.get_anime(6).per = 1.f;
									c.body.get_anime(6).time += 30.f / fps;
									if (c.body.get_anime(6).time >= c.body.get_anime(6).alltime) {
										c.body.get_anime(6).time = 0.f;
									}
									c.body.frame_reset(c.RIGHTarm1_f.first);
									c.body.frame_reset(c.RIGHTarm2_f.first);
									c.body.frame_reset(c.RIGHThand_f.first);
									c.body.frame_reset(c.LEFTarm1_f.first);
									c.body.frame_reset(c.LEFTarm2_f.first);
									c.body.frame_reset(c.LEFThand_f.first);

									easing_set(&this->fov_fps, this->fov, 0.9f, fps);
								}
								else {
									c.body.get_anime(6).per = 0.f;
									c.body.get_anime(6).time = 0.f;
									if (c.reloadf && c.gun_stat[c.ptr_now->id].mag_in.size() >= 1) {
										easing_set(&this->fov_fps, this->fov, 0.9f, fps);

										c.body.get_anime(3).per = 1.f;
										c.body.get_anime(3).time += 30.f / fps * ((c.body.get_anime(3).alltime / 30.f) / c.ptr_now->reload_time);
										if (c.body.get_anime(3).time >= c.body.get_anime(3).alltime) {
											c.body.get_anime(3).time = 0.f;
										}

										c.body.frame_reset(c.RIGHTarm1_f.first);
										c.body.frame_reset(c.RIGHTarm2_f.first);
										c.body.frame_reset(c.RIGHThand_f.first);
										c.body.frame_reset(c.LEFTarm1_f.first);
										c.body.frame_reset(c.LEFTarm2_f.first);
										c.body.frame_reset(c.LEFThand_f.first);
									}
									else {
										c.body.get_anime(3).per = 0.f;
										c.body.get_anime(3).time = 0.f;
										//�E��
										{
											VECTOR_ref pv = VGet(0, 0, 0);
											if (c.ptr_now->frame[4].first != INT_MAX) {
												pv = c.ptr_now->frame[4].second;
											}
											else if (c.ptr_now->frame[7].first != INT_MAX) {
												pv = c.ptr_now->frame[7].second;
											}
											if (this->ads.first) {
												easing_set(&this->gunpos_TPS, VGet(-0.035f, 0.f - pv.y(), -0.225f), 0.75f, fps);
												easing_set(&this->fov_fps, (this->fov*0.6f) / ((c.ptr_now->frame[4].first != INT_MAX) ? 4.f : 1.f), 0.9f, fps);
												easing_set(&this->campos_TPS, VGet(-0.35f, 0.15f, 3.f), 0.9f, fps);
											}
											else {
												if (running) {
													easing_set(&this->gunpos_TPS, VGet(-0.1f, -0.1f - pv.y(), -0.25f), 0.9f, fps);
													easing_set(&this->fov_fps, (this->fov*1.2f), 0.9f, fps);
													easing_set(&this->campos_TPS, VGet(-0.35f, 0.15f, 3.f), 0.95f, fps);
												}
												else {
													easing_set(&this->gunpos_TPS, VGet(-0.1f, -0.05f - pv.y(), -0.3f), 0.75f, fps);
													easing_set(&this->fov_fps, this->fov, 0.9f, fps);
													easing_set(&this->campos_TPS, VGet(-0.35f, 0.15f, 3.f), 0.95f, fps);
												}
											}
											c.mat_RIGHTHAND = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), c.vecadd_RIGHTHAND)*c.mat_HMD;//���R�C��
											c.pos_RIGHTHAND = c.pos_HMD + MATRIX_ref::Vtrans(this->gunpos_TPS, c.mat_RIGHTHAND);
											//�e��
											c.obj.SetMatrix(c.mat_RIGHTHAND*
												//MATRIX_ref::RotAxis(c.mat_HMD.zvec(),c.body_yrad)*
												MATRIX_ref::Mtrans(c.pos_RIGHTHAND + c.pos));
											VECTOR_ref tgt_pt = c.obj.frame(c.ptr_now->frame[8].first);
											//
											VECTOR_ref vec_a1 = MATRIX_ref::Vtrans((tgt_pt - c.body.frame(c.RIGHTarm1_f.first)).Norm(), m_inv.Inverse());//�
											VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0�Ƃ���
											float cos_t = getcos_tri((c.body.frame(c.RIGHThand_f.first) - c.body.frame(c.RIGHTarm2_f.first)).size(), (c.body.frame(c.RIGHTarm2_f.first) - c.body.frame(c.RIGHTarm1_f.first)).size(), (c.body.frame(c.RIGHTarm1_f.first) - tgt_pt).size());
											VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
											//��r
											c.body.SetFrameLocalMatrix(c.RIGHTarm1_f.first, MATRIX_ref::Mtrans(c.RIGHTarm1_f.second));
											MATRIX_ref a1_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.RIGHTarm2_f.first) - c.body.frame(c.RIGHTarm1_f.first), m_inv.Inverse()), vec_t);
											c.body.SetFrameLocalMatrix(c.RIGHTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(c.RIGHTarm1_f.second));
											//���r
											c.body.SetFrameLocalMatrix(c.RIGHTarm2_f.first, MATRIX_ref::Mtrans(c.RIGHTarm2_f.second));
											MATRIX_ref a2_inv = MATRIX_ref::RotVec2(MATRIX_ref::Vtrans(c.body.frame(c.RIGHThand_f.first) - c.body.frame(c.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()), MATRIX_ref::Vtrans(tgt_pt - c.body.frame(c.RIGHTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()));
											c.body.SetFrameLocalMatrix(c.RIGHTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(c.RIGHTarm2_f.second));
											//��
											c.body.SetFrameLocalMatrix(c.RIGHThand_f.first,
												MATRIX_ref::RotY(deg2rad(-10))*
												MATRIX_ref::RotZ(deg2rad(50))*
												MATRIX_ref::RotX(deg2rad(90))*
												c.mat_RIGHTHAND*
												m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.RIGHThand_f.second));

										}
										//����
										{
											c.pos_LEFTHAND = c.obj.frame(c.ptr_now->frame[6].first) - c.pos;
											if (c.down_mag) {
												c.pos_LEFTHAND = c.obj.frame(c.ptr_now->frame[0].first) + c.mat_RIGHTHAND.yvec()*-0.05f - c.pos;
											}
											c.mat_LEFTHAND = c.mat_HMD;

											float dist_ = ((c.pos_LEFTHAND + c.pos) - c.obj.frame(c.ptr_now->frame[6].first)).size();
											if (dist_ <= 0.2f && (!c.reloadf || !c.down_mag)) {
												c.LEFT_hand = true;
												c.pos_LEFTHAND = c.obj.frame(c.ptr_now->frame[6].first) - c.pos;
											}
											else {
												c.LEFT_hand = false;
											}
											{
												VECTOR_ref vec_a1 = MATRIX_ref::Vtrans(((c.pos + c.pos_LEFTHAND) - c.body.frame(c.LEFTarm1_f.first)).Norm(), m_inv.Inverse());//�
												VECTOR_ref vec_a1L1 = VECTOR_ref(VGet(0.f, -1.f, vec_a1.y() / vec_a1.z())).Norm();//x=0�Ƃ���
												float cos_t = getcos_tri((c.body.frame(c.LEFThand_f.first) - c.body.frame(c.LEFTarm2_f.first)).size(), (c.body.frame(c.LEFTarm2_f.first) - c.body.frame(c.LEFTarm1_f.first)).size(), (c.body.frame(c.LEFTarm1_f.first) - (c.pos + c.pos_LEFTHAND)).size());
												VECTOR_ref vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
												//��r
												c.body.SetFrameLocalMatrix(c.LEFTarm1_f.first, MATRIX_ref::Mtrans(c.LEFTarm1_f.second));
												MATRIX_ref a1_inv = MATRIX_ref::RotVec2(
													MATRIX_ref::Vtrans(c.body.frame(c.LEFTarm2_f.first) - c.body.frame(c.LEFTarm1_f.first), m_inv.Inverse()),
													vec_t
												);
												c.body.SetFrameLocalMatrix(c.LEFTarm1_f.first, a1_inv*MATRIX_ref::Mtrans(c.LEFTarm1_f.second));
												//���r
												c.body.SetFrameLocalMatrix(c.LEFTarm2_f.first, MATRIX_ref::Mtrans(c.LEFTarm2_f.second));
												MATRIX_ref a2_inv = MATRIX_ref::RotVec2(
													MATRIX_ref::Vtrans(c.body.frame(c.LEFThand_f.first) - c.body.frame(c.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse()),
													MATRIX_ref::Vtrans((c.pos + c.pos_LEFTHAND) - c.body.frame(c.LEFTarm2_f.first), m_inv.Inverse()*a1_inv.Inverse())
												);
												c.body.SetFrameLocalMatrix(c.LEFTarm2_f.first, a2_inv*MATRIX_ref::Mtrans(c.LEFTarm2_f.second));
												//��
												c.body.SetFrameLocalMatrix(c.LEFThand_f.first,
													MATRIX_ref::RotZ(deg2rad(-60))*
													MATRIX_ref::RotX(deg2rad(80))*
													c.mat_LEFTHAND*
													m_inv.Inverse()*a1_inv.Inverse()*a2_inv.Inverse()*MATRIX_ref::Mtrans(c.LEFThand_f.second));
											}
										}
									}
								}
								//
								c.body.work_anime();
								c.pos_HMD = (c.body.frame(c.RIGHTeye_f.first) + (c.body.frame(c.LEFTeye_f.first) - c.body.frame(c.RIGHTeye_f.first))*0.5f) - c.pos;
								if (running) {
									//�e��
									c.mat_RIGHTHAND = MATRIX_ref::RotY(deg2rad(45))* MATRIX_ref::RotX(deg2rad(-90))* c.body.GetFrameLocalWorldMatrix(c.RIGHThand2_f.first);
									c.pos_RIGHTHAND = c.body.frame(c.RIGHThand_f.first) - c.pos;
									c.obj.SetMatrix(c.mat_RIGHTHAND*MATRIX_ref::Mtrans(c.pos_RIGHTHAND + c.pos));
									c.pos_RIGHTHAND -= c.obj.frame(c.ptr_now->frame[8].first) - (c.pos_RIGHTHAND + c.pos);
									c.obj.SetMatrix(c.mat_RIGHTHAND*MATRIX_ref::Mtrans(c.pos_RIGHTHAND + c.pos));
									//
									c.mat_LEFTHAND = MATRIX_ref::RotY(deg2rad(-90 + 45))* MATRIX_ref::RotX(deg2rad(-90))*  (c.body.GetFrameLocalWorldMatrix(c.LEFThand2_f.first)*MATRIX_ref::Mtrans(c.body.frame(c.LEFThand2_f.first)).Inverse());
									c.pos_LEFTHAND = c.body.frame(c.LEFThand_f.first) - c.pos + c.mat_LEFTHAND.yvec()*0.1f;
								}
								else {
									if (c.reloadf && c.gun_stat[c.ptr_now->id].mag_in.size() >= 1) {
										//�e��
										c.mat_RIGHTHAND = MATRIX_ref::RotY(deg2rad(45))* MATRIX_ref::RotX(deg2rad(-90))* c.body.GetFrameLocalWorldMatrix(c.RIGHThand2_f.first);
										c.pos_RIGHTHAND = c.body.frame(c.RIGHThand_f.first) - c.pos;
										c.obj.SetMatrix(c.mat_RIGHTHAND*MATRIX_ref::Mtrans(c.pos_RIGHTHAND + c.pos));
										c.pos_RIGHTHAND -= c.obj.frame(c.ptr_now->frame[8].first) - (c.pos_RIGHTHAND + c.pos);
										c.obj.SetMatrix(c.mat_RIGHTHAND*MATRIX_ref::Mtrans(c.pos_RIGHTHAND + c.pos));
										//
										c.mat_LEFTHAND = MATRIX_ref::RotY(deg2rad(-90 + 45))* MATRIX_ref::RotX(deg2rad(-90))*  (c.body.GetFrameLocalWorldMatrix(c.LEFThand2_f.first)*MATRIX_ref::Mtrans(c.body.frame(c.LEFThand2_f.first)).Inverse());
										c.pos_LEFTHAND = c.body.frame(c.LEFThand_f.first) - c.pos + c.mat_LEFTHAND.yvec()*0.1f;
									}
									else {
										c.mat_RIGHTHAND = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), c.vecadd_RIGHTHAND)*c.mat_HMD;//���R�C��
										c.pos_RIGHTHAND = c.pos_HMD + MATRIX_ref::Vtrans(this->gunpos_TPS, c.mat_RIGHTHAND);
										c.obj.SetMatrix(c.mat_RIGHTHAND*MATRIX_ref::Mtrans(c.pos_RIGHTHAND + c.pos));
									}
								}
							}
							//�e����
							if (c.obj.get_anime(3).per == 1.f) {
								c.audio.slide.play_3D(c.pos + c.pos_RIGHTHAND, 1.f);
							}
							c.obj.get_anime(3).per = std::max(c.obj.get_anime(3).per - 12.f / fps, 0.f);
							if (start_c) {
								c.body.PhysicsResetState();
								start_c = false;
							}
							else {
								c.body.PhysicsCalculation(1000.f / fps);
							}
							//����
							{
								//�^�C�}�[�I��(B�L�[)
								scoreparts->ready_f |= (CheckHitKey(KEY_INPUT_B) != 0);
								//�v�����Z�b�g(V�L�[)
								if (CheckHitKey(KEY_INPUT_V) != 0 && scoreparts->end_f) {
									scoreparts->reset();
								}
								//�}�K�W���擾
								c.down_mag = true;
								//������(���N���b�N)
								easing_set(&c.obj.get_anime(2).per, float((GetMouseInput() & MOUSE_INPUT_LEFT) != 0), 0.5f, fps);
								if (!this->ads.first) {
									//�e�擾
									this->chgun.get_in(CheckHitKey(KEY_INPUT_F) != 0);
									//�e�̎g�p
									this->usegun.get_in((CheckHitKey(KEY_INPUT_P) != 0) && (this->sel_gun != -1));//<---
									if (this->sel_gun == -1) {
										this->usegun.first = false;
									}
								}
								//ADS
								this->ads.first = ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) && (this->usegun.first && c.ptr_now->cate == 1) && (!c.reloadf);
								//�Z���N�^�[(���{�^��)
								c.selkey.get_in(((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0) && (this->usegun.first));
								//�e�j��
								this->delgun.get_in((CheckHitKey(KEY_INPUT_G) != 0) && this->usegun.first);
								//�}�O�L���b�`(R�L�[)
								easing_set(&c.obj.get_anime(5).per, float((CheckHitKey(KEY_INPUT_R) != 0) && this->usegun.first), 0.5f, fps);
								if (!this->ads.first) {
									//�����ύX
									this->change_gun.get_in((GetMouseWheelRotVol() != 0) && this->usegun.first);
								}
							}
							//�ˌ��֘A
							{
								{
									easing_set(&c.vecadd_RIGHTHAND, c.vecadd_RIGHTHAND_p, 0.9f, fps);
									easing_set(&c.vecadd_RIGHTHAND_p, VGet(0, 0, 1.f), 0.975f, fps);
									//���R�C��
									if (c.gunf) {
										if (c.ammo_cnt >= 1) {
											c.obj.get_anime(0).per = 1.f;
											c.obj.get_anime(1).per = 0.f;
											c.obj.get_anime(0).time += 60.f / fps;
											if (c.obj.get_anime(0).time >= c.obj.get_anime(0).alltime) {
												c.obj.get_anime(0).time = 0.f;
												c.gunf = false;
											}
										}
										else {
											c.obj.get_anime(1).per = 1.f;
											c.obj.get_anime(0).per = 0.f;
											c.obj.get_anime(1).time += 60.f / fps;
											if (c.obj.get_anime(1).time >= c.obj.get_anime(1).alltime) {
												c.obj.get_anime(1).time = c.obj.get_anime(1).alltime;
												c.gunf = false;
											}
										}

									}
									if (c.ptr_now->cate == 1) {
										//�}�K�W���r�o
										if (c.obj.get_anime(5).per >= 0.5f && !c.reloadf && c.gun_stat[c.ptr_now->id].mag_in.size() >= 1) {
											c.audio.mag_down.play_3D(c.pos + c.pos_RIGHTHAND, 1.f);
											int dnm = int(c.ammo_cnt) - 1;
											//�e��
											if (c.ammo_cnt >= 1) {
												c.ammo_cnt = 1;
											}
											else {
												dnm = 0;
											}
											c.gun_stat[c.gun_slot[this->sel_gun].ptr->id].in -= dnm;
											//�o�C�u���[�V�����@�o�b�e���[������������߃R�����g�A�E�g
											/*
											vrparts->Haptic(vrparts->get_hand1_num(), unsigned short(60000));
											*/
											//�}�K�W���r�o
											c.reload_cnt = 0.f;
											c.gun_stat[c.ptr_now->id].mag_in.erase(c.gun_stat[c.ptr_now->id].mag_in.begin());
											//�}�K�W���r�o
											bool tt = false;
											for (auto& g : this->item_data) {
												if (g.ptr == nullptr && g.cate == 1) {
													tt = true;
													g.Set_item(c.ptr_now, c.pos_mag, c.mat_mag, 1);
													g.add = (c.obj.frame(c.ptr_now->frame[1].first) - c.obj.frame(c.ptr_now->frame[0].first)).Norm()*-1.f / fps;//�r䰃x�N�g��
													g.cap = dnm;
													break;
												}
											}
											if (!tt) {
												this->item_data.resize(this->item_data.size() + 1);
												auto& g = this->item_data.back();
												g.Set_item(c.ptr_now, c.pos_mag, c.mat_mag, 1);
												g.add = (c.obj.frame(c.ptr_now->frame[1].first) - c.obj.frame(c.ptr_now->frame[0].first)).Norm()*-1.f / fps;//�r䰃x�N�g��
												g.cap = dnm;
											}
											//
											c.reloadf = true;
										}
										//�Z���N�^�[
										if (c.selkey.second == 1) {
											++c.gun_stat[c.gun_slot[this->sel_gun].ptr->id].select %= c.ptr_now->select.size();
										}
									}
									//
									if (c.reloadf && c.gun_stat[c.ptr_now->id].mag_in.size() >= 1) {
										c.reload_cnt += 1.f / fps;
										if (c.reload_cnt < c.ptr_now->reload_time/3) {
											//c.down_mag = false;
										}
									}
									//�Z�t�e�B
									easing_set(&c.obj.get_anime(4).per, float(0.f), 0.5f, fps);
									//�ˌ�
									if (!c.gunf && c.ammo_cnt >= 1) {
										if (c.ptr_now->select[c.gun_stat[c.gun_slot[this->sel_gun].ptr->id].select] == 2) {//�t���I�[�g�p
											c.trigger.second = 0;
										}
									}
									c.trigger.get_in(c.obj.get_anime(2).per >= 0.5f);
									if (c.trigger.second == 1) {
										c.audio.trigger.play_3D(c.pos + c.pos_RIGHTHAND, 1.f);
									}
									switch (c.ptr_now->cate) {
									case 0:
									{
										//�ߐ�
										if (c.trigger.second == 1 && !c.gunf) {
											c.gunf = true;
											/*
												vrparts->Haptic(vrparts->get_hand1_num(), unsigned short(60000));
											*/
											//�T�E���h
											c.audio.shot.play_3D(c.pos + c.pos_RIGHTHAND, 1.f);
											c.audio.slide.play_3D(c.pos + c.pos_RIGHTHAND, 1.f);
											//�X�R�A
											if (scoreparts->start_f && !scoreparts->end_f) {
												scoreparts->sub(-4);
											}
										}
										break;
									}
									case 1:
									{
										if (c.trigger.second == 1 && !c.gunf && c.ammo_cnt >= 1) {
											c.gunf = true;
											/*
												vrparts->Haptic(vrparts->get_hand1_num(), unsigned short(60000));
											*/
											if (!settings->useVR_e && this->ads.first) {
												this->fov_fps *= 0.95f;
											}
											//�e���Ǘ�
											c.ammo_cnt--;
											c.gun_stat[c.gun_slot[this->sel_gun].ptr->id].in--;
											if (!c.reloadf && c.gun_stat[c.gun_slot[this->sel_gun].ptr->id].mag_in.size() >= 1 && c.gun_stat[c.gun_slot[this->sel_gun].ptr->id].mag_in.front() > 0) {
												c.gun_stat[c.gun_slot[this->sel_gun].ptr->id].mag_in.front()--;
											}
											//����������ƃR�R�����E�����
											c.vecadd_RIGHTHAND_p = MATRIX_ref::Vtrans(c.vecadd_RIGHTHAND_p,
												MATRIX_ref::RotY(deg2rad(float(int(c.ptr_now->recoil_xdn*100.f) + GetRand(int((c.ptr_now->recoil_xup - c.ptr_now->recoil_xdn)*100.f))) / (100.f*(c.LEFT_hand ? 3.f : 1.f))))*
												MATRIX_ref::RotX(deg2rad(float(int(c.ptr_now->recoil_ydn*100.f) + GetRand(int((c.ptr_now->recoil_yup - c.ptr_now->recoil_ydn)*100.f))) / (100.f*(c.LEFT_hand ? 3.f : 1.f)))));
											//�e
											c.bullet[c.use_bullet].set(&c.ptr_now->ammo[0], c.obj.frame(c.ptr_now->frame[3].first), c.mat_RIGHTHAND.zvec()*-1.f);
											//���
											c.cart[c.use_bullet].set(&c.ptr_now->ammo[0], c.obj.frame(c.ptr_now->frame[2].first), (c.obj.frame(c.ptr_now->frame[2].first + 1) - c.obj.frame(c.ptr_now->frame[2].first)).Norm()*2.5f / fps, c.mat_RIGHTHAND);
											//�G�t�F�N�g
											set_effect(&c.effcs[ef_fire], c.obj.frame(c.ptr_now->frame[3].first), c.mat_RIGHTHAND.zvec()*-1.f, 0.0025f / 0.1f);

											set_effect(&c.effcs_gun[c.use_effcsgun].effect, c.obj.frame(c.ptr_now->frame[3].first), c.mat_RIGHTHAND.zvec()*-1.f, 0.11f / 0.1f);
											set_pos_effect(&c.effcs_gun[c.use_effcsgun].effect, Drawparts->get_effHandle(ef_smoke));
											c.effcs_gun[c.use_effcsgun].ptr = &c.bullet[c.use_bullet];
											c.effcs_gun[c.use_effcsgun].cnt = 0.f;
											++c.use_effcsgun %= c.effcs_gun.size();
											//�T�E���h
											c.audio.shot.play_3D(c.pos + c.pos_RIGHTHAND, 1.f);
											c.audio.slide.play_3D(c.pos + c.pos_RIGHTHAND, 1.f);
											//����ID��
											++c.use_bullet %= c.bullet.size();
											//�X�R�A
											if (scoreparts->start_f && !scoreparts->end_f) {
												scoreparts->sub(-4);
											}
										}
										//�}�K�W���擾
										if (c.reloadf && c.gun_stat[c.ptr_now->id].mag_in.size() >= 1) {
											if (c.down_mag) {
												c.mat_mag = c.mat_LEFTHAND;
												c.pos_mag = c.pos_LEFTHAND + c.pos;
												if (c.reload_cnt > c.ptr_now->reload_time) {
													c.obj.get_anime(1).time = 0.f;
													c.obj.get_anime(0).per = 1.f;
													c.obj.get_anime(1).per = 0.f;
													if (c.ammo_cnt == 0) {
														c.obj.get_anime(3).per = 1.f;
													}
													c.audio.mag_set.play_3D(c.pos + c.pos_RIGHTHAND, 1.f);
													c.ammo_cnt += c.gun_stat[c.gun_slot[this->sel_gun].ptr->id].mag_in.front();
													c.reloadf = false;
												}
											}
										}
										else {
											c.down_mag = false;
											c.mat_mag = c.mat_RIGHTHAND;
											c.pos_mag = c.obj.frame(c.ptr_now->frame[1].first);
										}
										break;
									}
									}
								}
								c.mag.SetMatrix(c.mat_mag* MATRIX_ref::Mtrans(c.pos_mag));
								c.obj.work_anime();
								for (auto& a : c.bullet) {
									if (a.flug) {
										a.repos = a.pos;
										a.pos += a.vec * (a.spec->speed / fps);
										//����
										{
											auto p = mapparts->map_col_line(a.repos, a.pos, 0);
											if (p.HitFlag == TRUE) {
												a.pos = p.HitPosition;
											}
											for (auto& tp : tgt_pic) {
												auto q = tp.obj.CollCheck_Line(a.repos, a.pos, 0, 1);
												if (q.HitFlag == TRUE) {
													a.pos = q.HitPosition;
													//
													tp.power = (tp.obj.frame(tgtparts->frame.first) - a.pos).y();
													tp.time = 0.f;
													int UI_xpos = 0;
													int UI_ypos = 0;
													//�e������
													{
														tp.pic.SetDraw_Screen(false);
														VECTOR_ref pvecp = (a.pos - tp.obj.frame(tgtparts->frame.first));
														VECTOR_ref xvecp = MATRIX_ref::Vtrans(VGet(0, 0, 0), tp.obj.GetFrameLocalMatrix(tp.frame_x.first));
														VECTOR_ref yvecp = MATRIX_ref::Vtrans(VGet(0, 0, 0), tp.obj.GetFrameLocalMatrix(tp.frame_y.first));
														UI_xpos = int(float(tgtparts->x_size)*(xvecp.Norm().dot(pvecp)) / xvecp.size());//X����
														UI_ypos = int(float(tgtparts->y_size)*(yvecp.Norm().dot(pvecp)) / yvecp.size());//Y����
														DrawCircle(UI_xpos, UI_ypos, 10, GetColor(255, 0, 0));//�e��
													}
													//�|�C���g����
													if (scoreparts->start_f && !scoreparts->end_f) {
														int r_, g_, b_, a_;
														GetPixelSoftImage(tgtparts->col_tex, UI_xpos, UI_ypos, &r_, &g_, &b_, &a_);
														int pt = 0;
														switch (r_) {
														case 0:
															pt = 15;
															break;
														case 44:
															pt = 10;
															break;
														case 86:
															pt = 9;
															break;
														case 128:
															pt = 8;
															break;
														case 170:
															pt = 7;
															break;
														case 212:
															pt = 6;
															break;
														default:
															pt = 5;
															break;
														}
														scoreparts->add(int(float(pt) * ((a.pos - (c.pos + c.pos_RIGHTHAND)).size() / 9.144f)*(a.spec->damage / 10.f)));
													}
													//
													set_effect(&c.effcs[ef_reco], a.pos, q.Normal, 0.011f / 0.1f);
													//
													a.hit = true;
													a.flug = false;
													break;
												}
											}
											if (p.HitFlag == TRUE && a.flug) {
												a.flug = false;
												set_effect(&c.effcs_gndhit[c.use_effcsgndhit], a.pos, p.Normal, 0.025f / 0.1f);
												++c.use_effcsgndhit %= c.effcs_gndhit.size();
											}
										}
										//����(3�b�������A�X�s�[�h��0�ȉ��A�ђʂ�0�ȉ�)
										if (a.cnt >= 3.f || a.spec->speed < 0.f || a.spec->pene <= 0.f) {
											a.flug = false;
										}
										//�I��
										if (!a.flug) {
											for (auto& b : c.effcs_gun) {
												if (b.ptr == &a) {
													b.cnt = 2.5f;
													b.effect.handle.SetPos(b.ptr->pos);
													break;
												}
											}
										}
										//
									}
								}
								//��䰂̏���
								for (auto& a : c.cart) {
									a.get(mapparts, c);
								}
								for (auto& t : c.effcs) {
									if (t.id != ef_smoke) {
										set_pos_effect(&t, Drawparts->get_effHandle(int(t.id)));
									}
								}
								for (auto& t : c.effcs_gndhit) {
									set_pos_effect(&t, Drawparts->get_gndhitHandle());
								}
								for (auto& a : c.effcs_gun) {
									if (a.ptr != nullptr) {
										if (a.ptr->flug) {
											a.effect.handle.SetPos(a.ptr->pos);
										}
										if (a.cnt >= 0.f) {
											a.cnt += 1.f / fps;
											if (a.cnt >= 3.f) {
												a.effect.handle.Stop();
												a.cnt = -1.f;
											}
										}
									}
								}
							}
							//�A�C�e���֘A
							{
								c.canget_gunitem = false;
								c.canget_magitem = false;
								for (auto& g : this->item_data) {
									g.Get_item(c, item_data, mapparts, chgun, sel_gun, usegun, change_gun, gunpos_TPS);
								}
								//�e�𗎂Ƃ�
								if (this->delgun.second == 1) {
									//�A�C�e����
									bool tt = true;
									for (auto& g : this->item_data) {
										if (g.ptr == nullptr && g.cate == 0) {
											tt = false;
											g.Set_item(c.ptr_now, c.pos + c.pos_RIGHTHAND, c.mat_RIGHTHAND, 0);
											break;
										}
									}
									if (tt) {
										this->item_data.resize(this->item_data.size() + 1);
										this->item_data.back().Set_item(c.ptr_now, c.pos + c.pos_RIGHTHAND, c.mat_RIGHTHAND, 0);
									}
									//
									for (size_t i = 0; i < c.gun_slot.size(); i++) {
										if (this->sel_gun == i) {
											if ((this->sel_gun == c.gun_slot.size() - 1) || c.gun_slot[this->sel_gun + 1].ptr == nullptr) {
												c.gun_slot[this->sel_gun].delete_gun();
												this->sel_gun--;
											}
											else {
												for (size_t j = i; j < c.gun_slot.size() - 1; j++) {
													c.gun_slot[j].delete_gun();
													c.gun_slot[j].set(c.gun_slot[j + 1].ptr);
												}
												c.gun_slot.back().delete_gun();
											}
											break;
										}
									}
									//
									c.Delete_chara();
									c.Set_chara(this->sel_gun);
									this->gunpos_TPS = VGet(0, 0, 0);
								}
							}
						}
						//�^�C�}�[����
						scoreparts->move_timer();
						//�^�[�Q�b�g�̉��Z
						for (auto& tp : tgt_pic) {
							if (tp.isMOVE) {
								if (std::abs(tp.obj.GetPosition().x()) > 7.5f) {
									tp.LR ^= 1;
								}
								VECTOR_ref pos = VGet(tp.obj.GetPosition().x() + float(tp.LR ? 1 : -1) / fps, 0, tp.obj.GetPosition().z());
								auto pp = mapparts->map_col_line(pos - VGet(0, -10.f, 0), pos - VGet(0, 10.f, 0), 0);
								if (pp.HitFlag == 1) {
									tp.obj.SetPosition(pp.HitPosition);
								}
							}
							tp.rad = std::clamp(tp.rad + (-cos(tp.time)*deg2rad(tp.power)), deg2rad(-90), deg2rad(90));
							tp.obj.SetFrameLocalMatrix(tgtparts->frame.first, MATRIX_ref::RotX(tp.rad)* MATRIX_ref::Mtrans(tgtparts->frame.second));
							tp.obj.RefreshCollInfo(0, 1);

							easing_set(&tp.rad, 0.f, 0.995f, fps);
							easing_set(&tp.power, 0.f, 0.995f, fps);
							tp.time += deg2rad(180.f / fps);
						}
						//campos,camvec,camup�̎w��
						{
							auto& c = chara[id_mine];
							this->campos_buf = c.pos + c.pos_HMD;
							if (settings->useVR_e) {
								this->camvec = c.mat_HMD.zvec()*-1.f;
								this->camup = c.mat_HMD.yvec();
							}
							else {
								if (this->ads.first) {
									MATRIX_ref m_t = MATRIX_ref::RotVec2(VGet(0, 0, 1.f), c.vecadd_RIGHTHAND)*c.mat_RIGHTHAND;//���R�C��
									this->camvec = m_t.zvec()*-1.f;
									this->camup = m_t.yvec();
								}
								else {
									if (running) {
										this->camvec = c.mat_HMD.zvec()*-1.f;
										this->camup = c.mat_HMD.yvec();
									}
									else {
										if (c.reloadf) {
											this->camvec = c.mat_HMD.zvec()*-1.f;
											this->camup = c.mat_HMD.yvec();
										}
										else {
											this->camvec = c.mat_RIGHTHAND.zvec()*-1.f;
											this->camup = c.mat_RIGHTHAND.yvec();
										}
									}
								}
							}
						}
						Set3DSoundListenerPosAndFrontPosAndUpVec(this->campos_buf.get(), (this->campos_buf + this->camvec).get(), this->camup.get());
						UpdateEffekseer3D();
						//�e�p��
						Drawparts->Ready_Shadow(this->campos_buf, draw_in_shadow, VGet(5.f, 2.5f, 5.f));
						//VR��ԂɓK�p
						vrparts->Move_Player();
						//�X�R�[�v
						{
							auto& c = chara[id_mine];
							if (c.ptr_now->frame[4].first != INT_MAX) {
								VECTOR_ref cam = c.obj.frame(c.ptr_now->frame[4].first);
								VECTOR_ref vec = cam - c.mat_RIGHTHAND.zvec();
								VECTOR_ref yvec = c.mat_RIGHTHAND.yvec();
								Hostpassparts->draw(&this->ScopeScreen, mapparts->sky_draw(cam, vec, yvec, (this->fov / 7.5f) / 4.f), draw_by_shadow, cam, vec, yvec, (this->fov / 7.5f) / 4.f, 100.f, 0.1f);
								c.ptr_now->mod.lenzScreen.DrawExtendGraph(0, 0, 1080, 1080, true);
							}
						}
						//�`��
						UIparts->set_draw(chara[id_mine], scoreparts, settings->useVR_e, this->usegun.first, this->sel_gun);
						if (settings->useVR_e) {
							for (char i = 0; i < 2; i++) {
								this->campos = this->campos_buf + vrparts->GetEyePosition_minVR(i);
								//��ʑ̐[�x�`��
								Hostpassparts->dof(&this->BufScreen, mapparts->sky_draw(this->campos, this->campos + this->camvec, this->camup, this->fov), draw_by_shadow, this->campos, this->campos + this->camvec, this->camup, this->fov, 100.f, 0.1f);
								//�`��
								this->outScreen[i].SetDraw_Screen(0.1f, 100.f, this->fov_fps, this->campos, this->campos + this->camvec, this->camup);
								{
									Hostpassparts->bloom(this->BufScreen, 64);//�u���[���t���`��
									UIparts->draw();//UI
									UIparts->item_draw(this->item_data, this->campos_buf);
								}
								//VR�Ɉڂ�
								GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
								{
									this->outScreen[i].DrawGraph(0, 0, false);
									ID3D11Texture2D* ptr_DX11 = (ID3D11Texture2D*)GetUseDirect3D11BackBufferTexture2D();
									vrparts->PutEye(ptr_DX11, i);
								}
							}
						}
						else {
							this->campos = this->campos_buf + MATRIX_ref::Vtrans(VGet(-0.035f, 0.f, 0.f), chara[id_mine].mat_HMD);
							//��ʑ̐[�x�`��
							Hostpassparts->dof(&this->BufScreen, mapparts->sky_draw(this->campos, this->campos + this->camvec, this->camup, this->fov_fps), draw_by_shadow, this->campos, this->campos + this->camvec, this->camup, this->fov_fps, 100.f, 0.1f);
							//�`��
							this->outScreen[1].SetDraw_Screen(0.1f, 100.f, this->fov_fps, this->campos, this->campos + this->camvec, this->camup);
							{
								Hostpassparts->bloom(this->BufScreen, 64);//�u���[���t���`��
								UIparts->draw();//UI
								UIparts->item_draw(this->item_data, this->campos_buf);
							}
						}
						//�f�B�X�v���C�`��
						{
							auto& c = chara[id_mine];

							this->TPS.get_in(CheckHitKey(KEY_INPUT_LCONTROL) != 0);

							VECTOR_ref cam = c.pos + c.pos_HMD +
								MATRIX_ref::Vtrans(
									this->campos_TPS
									//, c.mat_HMD)
									, MATRIX_ref::RotY(deg2rad(-89)))
								;
							VECTOR_ref vec = c.pos + c.pos_HMD +
								MATRIX_ref::Vtrans(
									VGet(-0.35f, 0.15f, 0.f)
									//, c.mat_HMD)
									, MATRIX_ref::RotY(deg2rad(-89)))
								;
							if (this->TPS.first) {//TPS���_
								Hostpassparts->draw(&this->outScreen[2], mapparts->sky_draw(cam, vec, VGet(0, 1.f, 0), this->fov), draw_by_shadow, cam, vec, VGet(0, 1.f, 0), this->fov, 100.f, 0.1f);
								GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK, 0.1f, 100.f, this->fov, cam, vec, VGet(0, 1.f, 0));
								{
									this->outScreen[2].DrawExtendGraph(0, 0, settings->out_dispx, settings->out_dispy, false);
									//�^�[�Q�b�g���f��
									UIparts->TGT_drw(tgt_pic, c.obj.frame(c.ptr_now->frame[3].first), MATRIX_ref::Vtrans(VGet(0, 0, -1.f), c.mat_RIGHTHAND), tgtparts->x_size, tgtparts->y_size);
									//�X�R�[�v�̃G�C��
									if (c.ptr_now->frame[4].first != INT_MAX) {
										this->ScopeScreen.DrawExtendGraph(settings->out_dispx - 200, 0, settings->out_dispx, 200, true);
									}
								}
							}
							else {//FPS���_
								GraphHandle::SetDraw_Screen((int)DX_SCREEN_BACK);
								{
									this->outScreen[1].DrawExtendGraph(0, 0, settings->out_dispx, settings->out_dispy, false);
								}
							}
							//�f�o�b�O
							Debugparts->end_way();
							Debugparts->debug(10, 10, float(GetNowHiPerformanceCount() - waits) / 1000.f);
						}
					}
					DXDraw::Screen_Flip();
					vrparts->Eye_Flip(waits,FRAME_RATE);//�t���[���J�n�̐��~���b�O��start����܂Ńu���b�N���A�����_�����O���J�n���钼�O�ɌĂяo���K�v������܂��B
					if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
						this->ending = false;
						break;
					}
					if (CheckHitKey(KEY_INPUT_O) != 0) {
						break;
					}
				}
			}
			//���
			{
				for (auto& c : chara) {
					c.Delete_chara();
				}
				chara.clear();
				for (auto& p : tgt_pic) {
					p.Delete_tgt();
				}
				tgt_pic.clear();
				mapparts->Delete_map();
				Drawparts->Delete_Shadow();
			}
		} while (ProcessMessage() == 0 && this->ending);
	}
};