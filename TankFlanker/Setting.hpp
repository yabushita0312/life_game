#pragma once

#include "DXLib_ref/DXLib_ref.h"

class Setting{
private:
	GraphHandle bufScreen;
	FontHandle font18;
	FontHandle font12;
	uint8_t up_c = 0;
	uint8_t down_c = 0;

	uint8_t selup_c = 0;
	uint8_t seldown_c = 0;
	uint8_t sel_p = 0;
	float sel_cnt = 0.f;

	bool dof_o = false;
	bool bloom_o = false;
	bool shadow_o = false;
	int shadow_level_o = 13;
	bool useVR_o = true;
	bool getlog_o = true;

	const size_t siz = 6;
	std::array<std::string, 6> words[3];
public:
	bool dof_e = false;
	bool bloom_e = false;
	bool shadow_e = false;
	int shadow_level_e = 13;
	bool useVR_e = true;
	bool getlog_e = true;

	int dispx = 0, dispy = 0;						//描画サイズ
	int out_dispx = 0, out_dispy = 0;				//ウィンドウサイズ

	Setting() {
		SetOutApplicationLogValidFlag(FALSE);  /*log*/
		int mdata = FileRead_open("data/setting.txt", FALSE);
		dof_e = getparams::_bool(mdata);
		bloom_e = getparams::_bool(mdata);
		shadow_e = getparams::_bool(mdata);
		shadow_level_e = getparams::_int(mdata);
		useVR_e = getparams::_bool(mdata);
		getlog_e = getparams::_bool(mdata);
		FileRead_close(mdata);

		dof_o = dof_e;
		words[0][0] = "Depth of Field";
		words[2][0] = "ピンぼけの有無";
		bloom_o = bloom_e;
		words[0][1] = "Bloom";
		words[2][1] = "光沢の有無";
		shadow_o = shadow_e;
		words[0][2] = "Shadow";
		words[2][2] = "影の有無";
		shadow_level_o = shadow_level_e;
		words[0][3] = "Shadow Level";
		words[2][3] = "影のクオリティ";
		useVR_o = useVR_e;
		words[0][4] = "Use VR";
		words[2][4] = "VRモードの使用";
		getlog_o = getlog_e;
		words[0][5] = "Get Log";
		words[2][5] = "DXLibのログファイルを出力するか否か";
	}

	void set_dispsize() {
		if (this->useVR_e) {
			this->dispx = 1080 * 2;
			this->dispy = 1200 * 2;
			this->out_dispx = this->dispx * (desky * 8 / 9) / this->dispy;
			this->out_dispy = this->dispy * (desky * 8 / 9) / this->dispy;
		}
		else {
			this->dispx = deskx;
			this->dispy = desky;
			this->out_dispx = this->dispx;
			this->out_dispy = this->dispy;
		}
	}

	~Setting() {
	}
	void reset(void) {
		dof_e = dof_o;
		bloom_e = bloom_o;
		shadow_e = shadow_o;
		shadow_level_e = shadow_level_o;
		useVR_e = useVR_o;
		getlog_e = getlog_o;
	}
	void save(void) {
		std::ofstream outputfile("data/setting.txt");
		outputfile << "hostpass=" << (dof_e ? "true" : "false") << "\n";
		outputfile << "bloom=" << (bloom_e ? "true" : "false") << "\n";
		outputfile << "shadow=" << (shadow_e ? "true" : "false") << "\n";
		outputfile << "shadow_level=" << shadow_level_e << "\n";
		outputfile << "usevr=" << (useVR_e ? "true" : "false") << "\n";
		outputfile << "getlog=" << (getlog_e ? "true" : "false") << "\n";
		outputfile.close();
	}

	void ready_draw_setting(void) {
		bufScreen = GraphHandle::Make(640, 480);
		font18 = FontHandle::Create(18, DX_FONTTYPE_EDGE);
		font12 = FontHandle::Create(12, DX_FONTTYPE_EDGE);
	}
	unsigned char set_draw_setting(void) {
		unsigned char restart = 0;
		{
			selup_c = std::clamp<uint8_t>(selup_c + 1, 0, (CheckHitKey(KEY_INPUT_W) != 0) ? 2 : 0);
			if (selup_c == 1) {
				if (sel_p == 0) {
					sel_p = uint8_t(siz) + 2;
				}
				--sel_p;
			}
			seldown_c = std::clamp<uint8_t>(seldown_c + 1, 0, (CheckHitKey(KEY_INPUT_S) != 0) ? 2 : 0);
			if (seldown_c == 1) {
				++sel_p;
				if (sel_p == siz + 2) {
					sel_p = 0;
				}
			}

			up_c = std::clamp<uint8_t>(up_c + 1, 0, (CheckHitKey(KEY_INPUT_D) != 0) ? 2 : 0);
			if (up_c == 1) {
				switch (sel_p) {
				case 0:
					dof_e ^= 1;
					break;
				case 1:
					bloom_e ^= 1;
					break;
				case 2:
					shadow_e ^= 1;
					break;
				case 3:
					++shadow_level_e;
					break;
				case 4:
					useVR_e ^= 1;
					break;
				case 5:
					getlog_e ^= 1;
					break;
				}
			}
			down_c = std::clamp<uint8_t>(down_c + 1, 0, (CheckHitKey(KEY_INPUT_A) != 0) ? 2 : 0);
			if (down_c == 1) {
				switch (sel_p) {
				case 0:
					dof_e ^= 1;
					break;
				case 1:
					bloom_e ^= 1;
					break;
				case 2:
					shadow_e ^= 1;
					break;
				case 3:
					--shadow_level_e;
					break;
				case 4:
					useVR_e ^= 1;
					break;
				case 5:
					getlog_e ^= 1;
					break;
				}
			}
			shadow_level_e = std::clamp(shadow_level_e, 10, 14);
			//再起動フラグ
			{
				restart = 0;
				if (dof_o != dof_e) { restart = 1; }
				if (bloom_o != bloom_e) { restart = 1; }
				if (shadow_o != shadow_e) { restart = 1; }
				if (shadow_level_o != shadow_level_e) { restart = 1; }
				if (useVR_o != useVR_e) { restart = 1; }
				if (getlog_o != getlog_e) { restart = 1; }
			}
			//
			{
				words[1][0] = (dof_e ? "true" : "false");
				words[1][1] = (bloom_e ? "true" : "false");
				words[1][2] = (shadow_e ? "true" : "false");
				words[1][3] = std::to_string(shadow_level_e - 10);
				words[1][4] = (useVR_e ? "true" : "false");
				words[1][5] = (getlog_e ? "true" : "false");
			}
		}
		bufScreen.SetDraw_Screen();
		{
			DrawBox(2, 2, 640, 480, GetColor(255, 0, 0), FALSE);
			int xp = 10 + 2;
			int yp = 24 + 2;

			int xs1 = 200;
			int xs2 = 220;
			//
			//
			for (size_t i = 0; i < siz; i++) {
				if (sel_p == i) {
					font18.DrawString(xp, yp, words[0][i], GetColor(255, 255, 0));
					font18.DrawString(xp + xs1, yp, ((sel_cnt >= 0.5f) ? "< " : "  ") + words[1][i] + ((sel_cnt >= 0.5f) ? " >" : "  "), GetColor(255, 255, 0));
					font18.DrawString(xp + xs2, yp + 18, words[2][i], GetColor(255, 255, 0));
				}
				else {
					font12.DrawString(xp, yp, words[0][i], GetColor(255, 0, 0));
					font12.DrawString(xp + xs1, yp, words[1][i], GetColor(255, 0, 0));
					font12.DrawString(xp + xs2, yp + 18, words[2][i], GetColor(255, 0, 0));
				}
				yp += 40;
			}
			//
			if (sel_p == siz) {
				font18.DrawString(640 - font18.GetDrawWidth("再起動して設定を適応") - 6, 480 - 18 - 18 - 6, "再起動して設定を適応", (restart == 1) ? GetColor(255, 255, 0) : GetColor(128, 128, 128));
			}
			else {
				font12.DrawString(640 - font12.GetDrawWidth("再起動して設定を適応") - 6, 480 - 18 - 12 - 6, "再起動して設定を適応", (restart == 1) ? GetColor(255, 0, 0) : GetColor(128, 128, 128));
			}
			if (sel_p == siz+1) {
				font18.DrawString(640 - font18.GetDrawWidth("キャンセル") - 6, 480 - 18 - 6, "キャンセル", GetColor(255, 255, 0));
			}
			else {
				font12.DrawString(640 - font12.GetDrawWidth("キャンセ  ル") - 6, 480 - 18 - 6, "キャンセル", GetColor(255, 0, 0));
			}

			font12.DrawString(6, 480 - 18 - 6, "選択:WS 変更:AD 決定:RShift", GetColor(255, 0, 0));

			if (CheckHitKey(KEY_INPUT_RSHIFT) != 0) {
				if (sel_p == siz && restart == 1) {
					restart = 3;
				}
				if (sel_p == siz+1) {
					reset();

					restart = 2;
				}
			}
		}
		sel_cnt += 1.f / GetFPS();
		if (sel_cnt >= 1.f) {
			sel_cnt = 0.f;
		}
		return restart;
	}
	auto& settinggraphs(void) { return bufScreen; }
};
