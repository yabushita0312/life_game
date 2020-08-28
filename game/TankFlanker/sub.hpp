#pragma once

#define NOMINMAX
#include <D3D11.h>
#include <array>
#include <fstream>
#include <memory>
#include <optional>
#include <vector>
#include<fstream>
#include "DXLib_ref/DXLib_ref.h"
//
class Mainclass {
private:

public:
	//tgt
	class tgttmp {
	public:
		MV1 obj;
		GraphHandle tex;
		int col_tex = 0;
		int x_size = 0;
		int y_size = 0;
		frames frame;
		tgttmp() {
			MV1::Load("data/model/tgt/model.mv1", &this->obj, true);
			this->col_tex = LoadSoftImage("data/model/tgt/point.bmp");
			this->tex = GraphHandle::Load("data/model/tgt/Target-A2.png");
		}
		~tgttmp() {
			this->obj.Dispose();
			DeleteSoftImage(this->col_tex);
			this->tex.Dispose();
		}
		void Set_tgtdata() {
			this->tex.GetSize(&this->x_size, &this->y_size);
			this->frame = { 2,this->obj.frame(2) };
		}
	};
	class tgts {
	public:
		int id = 0;
		MV1 obj;
		GraphHandle pic;
		frames frame_x, frame_y;
		float rad = 0.f;
		float time = 0.f;
		float power = 0.f;
		bool LR = true, isMOVE = true;
		template<class Y, class D>
		void Set_tgt(std::unique_ptr<Y, D>& tgtparts, const VECTOR_ref& pos) {
			obj = tgtparts->obj.Duplicate();
			pic = GraphHandle::Make(tgtparts->x_size, tgtparts->y_size);
			pic.SetDraw_Screen(false);
			tgtparts->tex.DrawGraph(0, 0, true);

			obj.SetupCollInfo(8, 8, 8, 0, 1);
			obj.SetTextureGraphHandle(2, pic, false);
			frame_x.first = 4;
			frame_y.first = 3;
			LR = true;
			isMOVE = true;
			obj.SetPosition(pos);
		}
		void Delete_tgt() {
			this->obj.Dispose();
			this->pic.Dispose();
		}
	};
	//score
	class scores {
	public:
		std::vector<int> stack;

		int point = 0;
		int pointup = 0;
		int pointdown = 0;
		bool ready_f = false;
		bool start_f = false;
		bool end_f = false;
		float readytimer = 4.f;
		float timer = 30.f;

		scores() {
			SetOutApplicationLogValidFlag(FALSE);  /*log*/
			int mdata = FileRead_open("data/score.txt", FALSE);
			while (FileRead_eof(mdata) == 0) {
				this->stack.emplace_back(getparams::_int(mdata));
			}
			FileRead_close(mdata);
		}
		~scores() {
			std::ofstream outputfile("data/score.txt");
			for (auto& s : this->stack) {
				outputfile << s << "\n";
			}
			outputfile.close();
		}
		void insert() {
			int i = 0;
			for (auto& c : this->stack) {
				if (c < this->point) {
					break;
				}
				i++;
			}
			this->stack.insert(this->stack.begin() + i, this->point);
		}
		void reset() {
			this->point = 0;
			this->pointup = 0;
			this->pointdown = 0;
			this->ready_f = false;
			this->readytimer = 4.f;
			this->start_f = false;
			this->end_f = false;
			this->timer = 30.f;
		}
		void sub(const int& pt) {
			this->pointdown = pt;
			if (this->point + this->pointdown >= 0) {
				this->point += this->pointdown;
			}
		}
		void add(const int& pt) {
			this->pointup = pt;
			this->point += this->pointup;
		}
		void move_timer(void) {
			float fps = GetFPS();
			if (this->ready_f && !this->start_f) {
				this->readytimer -= 1.f / fps;
				if (this->readytimer <= 0.f) {
					this->start_f = true;
				}
			}
			if (this->start_f && !this->end_f) {
				this->timer -= 1.f / fps;
				if (this->timer <= 0.f) {
					this->timer = 0.f;
					this->end_f = true;
					this->insert();
				}
			}
		}
	};
	//�e�p�I�[�f�B�I
	class Audios {
	public:
		SoundHandle shot;
		SoundHandle slide;
		SoundHandle trigger;
		SoundHandle mag_down;
		SoundHandle mag_set;
		SoundHandle case_down;

		void set(int mdata) {
			SetUseASyncLoadFlag(TRUE);
			SetCreate3DSoundFlag(TRUE);
			shot = SoundHandle::Load("data/audio/shot_" + getparams::_str(mdata) + ".wav");
			slide = SoundHandle::Load("data/audio/slide_" + getparams::_str(mdata) + ".wav");
			trigger = SoundHandle::Load("data/audio/trigger_" + getparams::_str(mdata) + ".wav");
			mag_down = SoundHandle::Load("data/audio/mag_down_" + getparams::_str(mdata) + ".wav");
			mag_set = SoundHandle::Load("data/audio/mag_set_" + getparams::_str(mdata) + ".wav");
			case_down = SoundHandle::Load("data/audio/case_" + getparams::_str(mdata) + ".wav");
			SetCreate3DSoundFlag(FALSE);
			SetUseASyncLoadFlag(FALSE);
		}
	};
	//�e�f�[�^
	class Ammos {
	public:
		std::string name;
		float caliber = 0.f;
		float speed = 100.f;//�e��
		float pene = 10.f;//�ђ�
		float damage = 10.f;//�_���[�W

		MV1 ammo;
		void set(void) {
			int mdata = FileRead_open(("data/ammo/" + this->name + "/data.txt").c_str(), FALSE);
			this->caliber = getparams::_float(mdata)*0.001f;//���a
			this->speed = getparams::_float(mdata);	//�e��
			this->pene = getparams::_float(mdata);	//�ђ�
			this->damage = getparams::_float(mdata);//�_���[�W
			FileRead_close(mdata);

			MV1::Load("data/ammo/" + this->name + "/ammo.mv1", &ammo, true);
		}
	};
	//�}�K�W���f�[�^
	class Mags {
	public:
		std::string name;
		int cap = 1;
		std::vector<Ammos> ammo;

		MV1 mag;
		void set(void) {
			if (this->name.find("none") == std::string::npos) {
				int mdata = FileRead_open(("data/mag/" + this->name + "/data.txt").c_str(), FALSE);
				this->cap = getparams::_int(mdata);//���a
				//�e�f�[�^
				while (true) {
					auto p = getparams::_str(mdata);
					if (getright(p.c_str()).find("end") != std::string::npos) {
						break;
					}
					else {
						this->ammo.resize(this->ammo.size() + 1);
						this->ammo.back().name = p;
					}
				}
				FileRead_close(mdata);
				MV1::Load("data/mag/" + this->name + "/mag.mv1", &mag, true);
			}
		}
	};
	//�e�f�[�^
	class Gun {
		class Models {
		public:
			GraphHandle UIScreen;
			GraphHandle lenzScreen;
			MV1 obj;
			std::string name;
			void set(std::string named) {
				name = named;
				MV1::Load("data/gun/" + named + "/model.mv1", &obj, true);
				lenzScreen = GraphHandle::Load("data/gun/" + named + "/lenz.png");
				UIScreen = GraphHandle::Load("data/gun/" + named + "/pic.bmp");
			}
		};
	public:
		int id = 0;
		uint8_t cate = 0;
		std::string name;
		std::vector<frames> frame;
		std::vector <uint8_t> select;//�Z���N�^�[
		Models mod;
		Audios audio;
		size_t ammo_max = 0;//�}�K�W���̑��e��
		float recoil_xup = 0.f;
		float recoil_xdn = 0.f;
		float recoil_yup = 0.f;
		float recoil_ydn = 0.f;
		float reload_time = 1.f;//�đ��U����
		float ammo_speed = 100.f;//�e��
		float ammo_pene = 10.f;//�ђ�
		float ammo_damege = 10.f;//�_���[�W
		std::vector<Ammos> ammo;
		Mags mag;
		void set_data() {
			this->name = this->mod.name;
			this->mod.obj.SetMatrix(MGetIdent());
			{
				//�t���[��
				this->frame.resize(9);
				this->frame[0].first = INT_MAX;
				this->frame[1].first = INT_MAX;
				this->frame[2].first = INT_MAX;
				this->frame[3].first = INT_MAX;
				this->frame[4].first = INT_MAX;
				this->frame[5].first = INT_MAX;
				this->frame[6].first = INT_MAX;
				this->frame[7].first = INT_MAX;
				this->frame[8].first = INT_MAX;
				for (int i = 0; i < this->mod.obj.frame_num(); i++) {
					std::string s = this->mod.obj.frame_name(i);
					if (s.find("mag_fall") != std::string::npos) {
						this->frame[0].first = i;//�}�K�W��
						this->frame[0].second = this->mod.obj.frame(i);
						this->frame[1].first = i + 1;
						this->frame[1].second = this->mod.obj.frame(i + 1);
					}
					else if (s.find("case") != std::string::npos) {
						this->frame[2].first = i;//�r�
					}
					else if (s.find("mazzule") != std::string::npos) {
						this->frame[3].first = i;//�}�Y��
					}
					else if (s.find("scope") != std::string::npos) {
						this->frame[4].first = i;//�X�R�[�v
						this->frame[4].second = this->mod.obj.frame(i);
					}
					else if (s.find("trigger") != std::string::npos) {
						this->frame[5].first = i + 1;//�g���K�[
					}
					else if (s.find("LEFT") != std::string::npos) {
						this->frame[6].first = i;//����
					}
					else if (s.find("site") != std::string::npos) {
						this->frame[7].first = i;//�A�C�A���T�C�g
						this->frame[7].second = this->mod.obj.frame(i);
					}
					else if (s.find("RIGHT") != std::string::npos) {
						this->frame[8].first = i;//����
					}
				}
				//�e�L�X�g
				{
					int mdata = FileRead_open(("data/gun/" + this->name + "/data.txt").c_str(), FALSE);
					//�J�e�S��
					{
						auto p = getparams::_str(mdata);
						if (getright(p.c_str()).find("knife") != std::string::npos) {
							this->cate = 0;
						}
						else if (getright(p.c_str()).find("gun") != std::string::npos) {
							this->cate = 1;
						}
					}
					//�Z���N�^�[�ݒ�
					while (true) {
						auto p = getparams::_str(mdata);
						if (getright(p.c_str()).find("end") != std::string::npos) {
							break;
						}
						else if (getright(p.c_str()).find("semi") != std::string::npos) {
							this->select.emplace_back(uint8_t(1));					//�Z�~�I�[�g=1
						}
						else if (getright(p.c_str()).find("full") != std::string::npos) {
							this->select.emplace_back(uint8_t(2));					//�t���I�[�g=2
						}
						else if (getright(p.c_str()).find("3b") != std::string::npos) {
							this->select.emplace_back(uint8_t(3));					//3�A�o�[�X�g=3
						}
						else if (getright(p.c_str()).find("2b") != std::string::npos) {
							this->select.emplace_back(uint8_t(4));					//2�A�o�[�X�g=4
						}
						else {
							this->select.emplace_back(uint8_t(1));
						}
					}
					//�T�E���h
					this->audio.set(mdata);
					//���e��
					this->ammo_max = getparams::_long(mdata);
					//�e�f�[�^
					while (true) {
						auto p = getparams::_str(mdata);
						if (getright(p.c_str()).find("end") != std::string::npos) {
							break;
						}
						else {
							this->ammo.resize(this->ammo.size() + 1);
							this->ammo.back().name = p;
						}
					}
					//�}�K�W��
					this->mag.name = getparams::_str(mdata);
					//����
					this->recoil_xup = getparams::_float(mdata);
					this->recoil_xdn = getparams::_float(mdata);
					this->recoil_yup = getparams::_float(mdata);
					this->recoil_ydn = getparams::_float(mdata);
					//�����[�h�^�C��
					this->reload_time = getparams::_float(mdata);
					FileRead_close(mdata);
					//
					for (auto& a : this->ammo) {
						a.set();
					}
					this->mag.set();
				}
			}
		}
	};
	//���ۂɔ��˂����e
	class ammos {
	public:
		bool hit{ false };
		bool flug{ false };
		float cnt = 0.f;
		unsigned int color = 0;
		Ammos* spec = nullptr;
		float yadd = 0.f;
		VECTOR_ref pos, repos, vec;

		void ready() {
			this->flug = false;
			this->color = GetColor(255, 255, 172);
		}
		void set(Ammos* spec_t, const VECTOR_ref& pos_t, const VECTOR_ref& vec_t) {
			this->spec = spec_t;
			this->pos = pos_t;
			this->vec = vec_t;
			this->hit = false;
			this->flug = true;
			this->cnt = 0.f;
			this->yadd = 0.f;
			this->repos = this->pos;
		}
		void draw() {
			if (this->flug) {
				DXDraw::Capsule3D(this->pos, this->repos, ((this->spec->caliber - 0.00762f) * 0.1f + 0.00762f), this->color, GetColor(255, 255, 255));
			}
		}
	};
	//player
	class Chara {
		struct ef_guns {
			EffectS effect;
			ammos* ptr = nullptr;
			float cnt = -1.f;
		};
		class ammo_obj {
		public:
			MV1 second;
			float cnt = -1.f;
			bool down = false;
			VECTOR_ref pos;
			VECTOR_ref add;
			MATRIX_ref mat;
			void ready() {
				this->cnt = -1.f;
				this->pos = VGet(0, 0, 0);
				this->mat = MGetIdent();
			}
			void set(Ammos* spec_t, const VECTOR_ref& pos_t, const VECTOR_ref& vec_t, const MATRIX_ref& mat_t) {
				this->second.Dispose();
				this->second = spec_t->ammo.Duplicate();
				this->cnt = 0.f;
				this->pos = pos_t;//�r�
				this->add = vec_t;//�r䰃x�N�g��
				this->mat = mat_t;
			}
			template<class Y, class D>
			void get(std::unique_ptr<Y, D>& mapparts,Chara&c) {
				if (this->cnt >= 0.f) {
					float fps = GetFPS();

					this->cnt += 1.f / fps;
					this->pos += this->add*(float(200 + GetRand(1000)) / 1000.f);
					this->add.yadd(-9.8f / powf(fps, 2.f));

					auto pp = mapparts->map_col_line(this->pos + VGet(0, 1.f, 0), this->pos - VGet(0, 0.008f, 0), 0);
					if (pp.HitFlag == 1) {
						this->pos = VECTOR_ref(pp.HitPosition) + VGet(0, 0.008f, 0);
						this->add += VECTOR_ref(pp.Normal)*(VECTOR_ref(pp.Normal).dot(this->add*-1.f)*1.25f);
						easing_set(&this->add, VGet(0, 0, 0), 0.95f, fps);
						if (!this->down) {
							c.audio.case_down.play_3D(c.pos + c.pos_RIGHTHAND, 1.f);
						}

						this->mat *= MATRIX_ref::RotVec2(this->mat.yvec(), VECTOR_ref(pp.Normal));

						this->down = true;
					}
					else {
						this->mat *= MATRIX_ref::RotY(deg2rad(360 * 10 + GetRand(360 * 20)) / fps);
					}
					if (this->cnt >= 3.f) {
						this->cnt = -1.f;
					}

					this->second.SetMatrix(this->mat*MATRIX_ref::Mtrans(this->pos));
				}
				else {
					this->down = false;
				}
			}
			void draw() {
				if (this->cnt >= 0.f) {
					this->second.DrawModel();
				}
			}
			void delete_cart() {
				this->second.Dispose();
			}
		};
		struct gun_state {
			size_t in = 0;				//�����e��
			std::vector<size_t> mag_in;	//�}�K�W����
			uint8_t select = 0;			//�Z���N�^�[
		};
		class gun_have {
		public:
			int id = 0;
			Gun* ptr = nullptr;
			MV1 obj;
			void set(Gun* ptr_) {
				this->ptr = ptr_;
				if (this->ptr != nullptr) {
					this->obj = this->ptr->mod.obj.Duplicate();
				}
			}
			void delete_gun() {
				this->ptr = nullptr;
				this->obj.Dispose();
			}
		};
		GraphHandle* ScopeScreen=nullptr;
	public:
		/*�G�t�F�N�g*/
		std::array<EffectS, effects> effcs;
		std::array<ef_guns, 60> effcs_gun;
		size_t use_effcsgun = 0;
		std::array<EffectS, 12> effcs_gndhit;
		size_t use_effcsgndhit = 0;
		/*�m�ۂ���e*/
		std::array<ammos, 64> bullet;
		size_t use_bullet = 0;
		std::array<ammo_obj, 64> cart;
		//�����e���Ȃǂ̃f�[�^
		std::vector<gun_state> gun_stat;
		/*����|�C���^*/
		Gun* ptr_now = nullptr;				//���ݎg�p���̕���
		size_t ammo_cnt = 0;				//���e���J�E���g
		std::array<gun_have, 3> gun_slot;	//�e�X���b�g
		Gun* gun_slot_backup = nullptr;		//�o�b�N�A�b�v����X���b�g
		/*���f���A����*/
		Audios audio;
		MV1 obj, mag, body;
		/**/
		float reload_cnt = 0.f;//�����[�h�J�n�܂ł̃J�E���g
		switchs trigger;//�g���K�[
		bool gunf = false;//�ˌ��t���O
		switchs selkey;//�g���K�[
		bool LEFT_hand = false;//�����Y���Ă��邩
		bool reloadf = false;
		bool down_mag = false;
		//�v���C���[���W�n
		VECTOR_ref pos;
		MATRIX_ref mat;
		//�}�K�W�����W�n
		VECTOR_ref pos_mag;
		MATRIX_ref mat_mag;
		//�������W�n
		VECTOR_ref pos_HMD;
		MATRIX_ref mat_HMD;
		float add_ypos = 0.f;//���������x
		float body_xrad = 0.f;//���̊p�x
		float body_yrad = 0.f;//���̊p�x
		float body_zrad = 0.f;//���̊p�x
		frames head_f;
		frames LEFTeye_f;
		frames RIGHTeye_f;
		//
		frames bodyg_f;
		frames bodyc_f;
		frames bodyb_f;
		frames body_f;
		//�E����W�n
		VECTOR_ref pos_RIGHTHAND;
		MATRIX_ref mat_RIGHTHAND;
		VECTOR_ref vecadd_RIGHTHAND, vecadd_RIGHTHAND_p;//
		frames RIGHThand2_f;
		frames RIGHThand_f;
		frames RIGHTarm2_f;
		frames RIGHTarm1_f;
		//������W�n
		VECTOR_ref pos_LEFTHAND;
		MATRIX_ref mat_LEFTHAND;
		frames LEFThand2_f;
		frames LEFThand_f;
		frames LEFTarm2_f;
		frames LEFTarm1_f;
		//
		bool canget_gunitem = false;
		std::string canget_gun;
		//
		bool canget_magitem = false;
		std::string canget_mag;
		//
		void Ready_chara(Gun*gundata, Gun*gundata_backup, size_t state_s, MV1& hand_, GraphHandle* scope) {
			this->gun_slot_backup = gundata_backup;
			this->gun_slot[0].set(gundata);
			this->gun_slot[1].delete_gun();
			this->gun_slot[2].delete_gun();
			fill_id(this->gun_slot);
			this->gun_stat.resize(state_s);
			for (auto& s : this->gun_stat) {
				s.in = 0;
				s.select = 0;
			}
			//��
			hand_.DuplicateonAnime(&this->body);
			for (int i = 0; i < int(this->body.frame_num());i++) {
				std::string p = this->body.frame_name(i);
				if (p == std::string("�O���[�u")) {
					bodyg_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
				}
				else if (p == std::string("�����g")) {
					bodyc_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
				}
				else if (p.find("����") != std::string::npos) {

				}
				else if (p.find("���Ђ�") != std::string::npos) {

				}
				else if (p.find("������") != std::string::npos) {

				}
				else if (p.find("�������") != std::string::npos) {

				}
				else if (p.find("�E��") != std::string::npos) {

				}
				else if (p.find("�E�Ђ�") != std::string::npos) {

				}
				else if (p.find("�E����") != std::string::npos) {

				}
				else if (p.find("�E�����") != std::string::npos) {

				}
				else if (p.find("�����g��") != std::string::npos) {

				}
				else if (p.find("�㔼�g") != std::string::npos && p.find("�㔼�g2") == std::string::npos) {
					bodyb_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
				}
				else if (p.find("�㔼�g2") != std::string::npos) {
					body_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
				}
				else if (p.find("��") != std::string::npos && p.find("��") == std::string::npos) {

				}
				else if (p.find("��") != std::string::npos && p.find("��") == std::string::npos) {
					head_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
				}
				else if (p.find("�E�ڐ�") != std::string::npos) {
					RIGHTeye_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
				}
				else if (p.find("���ڐ�") != std::string::npos) {
					LEFTeye_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
				}
				else if (p.find("����") != std::string::npos) {

				}
				else if (p.find("�E��") != std::string::npos) {

				}
				else if (p.find("�E�r") != std::string::npos && p.find("��") == std::string::npos) {
					RIGHTarm1_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
				}
				else if (p.find("�E�r��") != std::string::npos) {
				}
				else if (p.find("�E�Ђ�") != std::string::npos) {
					RIGHTarm2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
				}
				else if (p.find("�E�蝀") != std::string::npos) {
				}
				else if (p == std::string("�E���")) {
					RIGHThand_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
				}
				else if (p.find("�E����") != std::string::npos) {
					RIGHThand2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
				}
				else if (p.find("����") != std::string::npos) {

				}
				else if (p.find("���r") != std::string::npos && p.find("��") == std::string::npos) {
					LEFTarm1_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
				}
				else if (p.find("���r��") != std::string::npos) {

				}
				else if (p.find("���Ђ�") != std::string::npos) {
					LEFTarm2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
				}
				else if (p.find("���蝀") != std::string::npos) {

				}
				else if (p == std::string("�����")) {
					LEFThand_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
				}
				else if (p.find("������") != std::string::npos) {
					LEFThand2_f = { int(i),MATRIX_ref::Vtrans(VGet(0,0,0),this->body.GetFrameLocalMatrix(i)) };
				}
			}

			ScopeScreen = scope;
		}
		void Set_chara_Position(const VECTOR_ref& pos_, const MATRIX_ref& mat_, const MATRIX_ref& mat_H) {
			this->pos = pos_;
			this->mat = mat_;
			this->mat_HMD = mat_H;
		}
		void Set_chara(int gunid) {
			if (gunid >= 0) {
				this->ptr_now = this->gun_slot[gunid].ptr;
			}
			else {
				this->ptr_now = this->gun_slot_backup;
			}
			this->ptr_now->mod.obj.DuplicateonAnime(&this->obj);
			if (this->ptr_now->frame[4].first != INT_MAX) {
				this->obj.SetTextureGraphHandle(1, *ScopeScreen, false);	//�X�R�[�v
			}
			this->mag = this->ptr_now->mag.mag.Duplicate();
			for (auto& a : this->cart) {
				a.ready();
			}
			this->LEFT_hand = false;
			if (this->gun_stat[this->ptr_now->id].mag_in.size() >= 1) {
				this->ammo_cnt = this->gun_stat[this->ptr_now->id].mag_in.front();//����
			}
			else {
				this->ammo_cnt = std::clamp<size_t>(this->gun_stat[this->ptr_now->id].in, 0, this->ptr_now->ammo_max);//����
			}


			this->gunf = false;
			this->vecadd_RIGHTHAND = VGet(0, 0, 1.f);
			this->vecadd_RIGHTHAND_p = this->vecadd_RIGHTHAND;
			this->reloadf = false;
			this->down_mag = true;
			this->selkey.second = 0;
			for (auto& a : this->obj.get_anime()) {
				a.per = 0.f;
			}
			fill_id(this->effcs);			      //�G�t�F�N�g
			for (auto& a : this->bullet) {
				a.ready();
			}
			SetCreate3DSoundFlag(TRUE);
			this->audio.shot = this->ptr_now->audio.shot.Duplicate();
			this->audio.slide = this->ptr_now->audio.slide.Duplicate();
			this->audio.trigger = this->ptr_now->audio.trigger.Duplicate();
			this->audio.mag_down = this->ptr_now->audio.mag_down.Duplicate();
			this->audio.mag_set = this->ptr_now->audio.mag_set.Duplicate();
			this->audio.case_down = this->ptr_now->audio.case_down.Duplicate();
			SetCreate3DSoundFlag(FALSE);
		}
		void Draw_chara(const bool& usegun, const int& sel_gun) {
			this->body.DrawModel();
			this->obj.DrawModel();
			if (this->ptr_now->cate == 1) {
				if ((!this->reloadf || this->down_mag) && this->gun_stat[this->ptr_now->id].mag_in.size() >= 1) {
					this->mag.DrawModel();
				}
				for (auto& a : this->cart) {
					a.draw();
				}
			}
			for (auto& s : this->gun_slot) {
				if ((!usegun || s.id != sel_gun) && s.ptr != nullptr) {
					s.obj.DrawModel();
				}
			}
		}
		void Draw_ammo() {
			if (this->ptr_now->cate == 1) {
				for (auto& a : this->bullet) {
					a.draw();
				}
			}
		}
		void Delete_chara() {
			this->ptr_now = nullptr;
			this->obj.Dispose();
			this->mag.Dispose();
			for (auto& a : this->cart) {
				a.delete_cart();
			}
			this->audio.shot.Dispose();
			this->audio.slide.Dispose();
			this->audio.trigger.Dispose();
			this->audio.mag_down.Dispose();
			this->audio.mag_set.Dispose();
			this->audio.case_down.Dispose();
			for (auto& t : this->effcs_gun) {
				t.effect.handle.Dispose();
			}
			for (auto& t : this->effcs) {
				t.handle.Dispose();
			}
		}
		void Init_chara() {
			this->body.Dispose();
			Delete_chara();
			for (auto& h : gun_slot) {
				h.delete_gun();
			}
		}
	};


	//�A�C�e��
	class Items {
	public:
		//���
		int cate = 0;
		//����
		VECTOR_ref pos, add;
		MATRIX_ref mat;
		MV1 obj;
		Gun* ptr = nullptr;
		//�}�K�W����p�p�����[�^�[
		size_t cap = 0;
		//
		void Set_item(Gun*gundata, const VECTOR_ref& pos_, const MATRIX_ref& mat_, int cat) {
			bool choose = true;
			this->cate = cat;
			switch (this->cate) {
			case 0:
				choose = true;
				break;
			case 1:
				choose = gundata->mag.name.find("none") == std::string::npos;
				break;
			default:
				choose = true;
				break;
			}

			if (choose) {
				this->pos = pos_;
				this->add = VGet(0, 0, 0);
				this->mat = mat_;
				//��
				this->ptr = gundata;
				switch (this->cate) {
				case 0:
					this->obj = this->ptr->mod.obj.Duplicate();
					break;
				case 1:
					this->obj = this->ptr->mag.mag.Duplicate();
					break;
				default:
					this->obj = this->ptr->mod.obj.Duplicate();
					break;
				}
			}
			else {
				this->Delete_item();
			}
		}
		template<class Y, class D>
		void Get_item( Chara& chara, std::vector<Items>& item, std::unique_ptr<Y, D>& mapparts, switchs& chgun, int& sel_gun, switchs& usegun, switchs& change_gun, VECTOR_ref& gunpos_TPS) {
			if (this->ptr != nullptr) {
				float fps = GetFPS();
				this->obj.SetMatrix(this->mat*MATRIX_ref::Mtrans(this->pos));
				this->pos += this->add;
				this->add.yadd(-9.8f / powf(fps, 2.f));
				for (auto& p : item) {
					if (p.ptr != nullptr && &p != &*this) {
						if ((p.pos - this->pos).size() <= 0.1f) {
							p.add.xadd((p.pos - this->pos).x()*10.f / fps);
							p.add.zadd((p.pos - this->pos).z()*10.f / fps);
							this->add.xadd((this->pos - p.pos).x()*10.f / fps);
							this->add.zadd((this->pos - p.pos).z()*10.f / fps);
						}
					}
				}
				auto pp = mapparts->map_col_line(this->pos + VGet(0, 1.f, 0), this->pos - VGet(0, 0.05f, 0), 0);
				if (pp.HitFlag == 1) {
					this->pos = VECTOR_ref(pp.HitPosition) + VGet(0, 0.05f, 0);
					this->mat *= MATRIX_ref::RotVec2(this->mat.xvec(), VECTOR_ref(pp.Normal));
					easing_set(&this->add, VGet(0, 0, 0), 0.8f, fps);
				}
				//
				VECTOR_ref startpos = chara.obj.frame(chara.ptr_now->frame[3].first);
				VECTOR_ref endpos = chara.obj.frame(chara.ptr_now->frame[3].first) + chara.mat_LEFTHAND.zvec()*-3.f;
				auto p = mapparts->map_col_line(startpos, endpos, 0);
				if (p.HitFlag == 1) {
					endpos = p.HitPosition;
				}
				switch (this->cate){
				case 0:
					chara.canget_gunitem = (Segment_Point_MinLength(startpos.get(), endpos.get(), this->pos.get()) <= 0.3f);
					if (chara.canget_gunitem) {
						chara.canget_gun = this->ptr->name;
						if (chgun.second == 1) {
							int sel_t = sel_gun;
							Gun* gun_t = this->ptr;
							this->Delete_item();
							for (size_t i = 0; i < chara.gun_slot.size(); i++) {
								if (chara.gun_slot[i].ptr == nullptr) {
									sel_t = sel_gun = int(i);
									break;
								}
								if (i == chara.gun_slot.size() - 1) {
									sel_t = sel_gun;
									if (!usegun.first) {
										++sel_t %= chara.gun_slot.size();
										if (chara.gun_slot[sel_t].ptr == nullptr) {
											sel_t = 0;
										}
									}
									this->Set_item(chara.gun_slot[sel_t].ptr, chara.pos + chara.pos_RIGHTHAND, chara.mat_RIGHTHAND, 0);
								}

							}
							if (!usegun.first) {
								usegun.first = true;
								change_gun.first = 1;
							}
							//
							chara.gun_slot[sel_t].delete_gun();
							chara.gun_slot[sel_t].set(gun_t);
							chara.Delete_chara();
							chara.Set_chara(sel_t);
							gunpos_TPS = VGet(0, 0, 0);
						}
					}
					break;
				case 1:
					chara.canget_magitem = (Segment_Point_MinLength(startpos.get(), endpos.get(), this->pos.get()) <= 0.3f);
					if (chara.canget_magitem) {
						chara.canget_mag = this->ptr->mag.name;
						if (chgun.second == 1) {
							chara.gun_stat[this->ptr->id].in += this->cap;
							chara.gun_stat[this->ptr->id].mag_in.insert(chara.gun_stat[this->ptr->id].mag_in.end(), this->cap);
							if (chara.gun_stat[this->ptr->id].mag_in.size() == 1) {
								chara.reloadf = true;
							}
							this->Delete_item();
						}
					}
					break;
				default:
					break;
				}
			}
			//��
		}
		void Draw_item() {
			if (this->ptr != nullptr) {
				this->obj.DrawModel();
			}
		}
		void Draw_item(Chara& chara) {
			if (this->ptr != nullptr) {
				if (this->cate == 0) {
					if (chara.canget_gunitem && chara.canget_gun == this->ptr->name) {
						DrawLine3D(this->pos.get(), (this->pos + VGet(0, 0.1f, 0)).get(), GetColor(255, 0, 0));
						auto c = MV1GetDifColorScale(this->obj.get());
						MV1SetDifColorScale(this->obj.get(), GetColorF(0.f, 1.f, 0.f, 1.f));
						this->obj.DrawModel();
						MV1SetDifColorScale(this->obj.get(), c);
					}
					else {
						this->obj.DrawModel();
					}
				}
				else if (this->cate == 1) {
					if (chara.canget_magitem && chara.canget_mag == this->ptr->name) {
						DrawLine3D(this->pos.get(), (this->pos + VGet(0, 0.1f, 0)).get(), GetColor(255, 0, 0));
						auto c = MV1GetDifColorScale(this->obj.get());
						MV1SetDifColorScale(this->obj.get(), GetColorF(0.f, 1.f, 0.f, 1.f));
						this->obj.DrawModel();
						MV1SetDifColorScale(this->obj.get(), c);
					}
					else {
						this->obj.DrawModel();
					}
				}
			}
		}
		void Delete_item() {
			this->ptr = nullptr;
			this->obj.Dispose();
		}
	};
};
//