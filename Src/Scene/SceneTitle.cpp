#include<DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/InputManager.h"
#include "../Manager/MessageManager.h"
#include "../Manager/SoundManager.h"
#include "../Manager/ScoreManager.h"
#include "../Common/ScoreData.h"
#include "../Common/DrawUtility.h"
#include "../Common/AsoUtility.h"
#include"../BGM.h"
#include "../Sub.h"
#include"SceneTitle.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

namespace {
	constexpr int appear_interval = 20;  // 出現までのフレーム
	constexpr int menu_line_height = 40; // メニューの1つあたりの高さ
	constexpr int margin_size = 20;      // ポーズメニュー枠の余白
	
	constexpr int CONTROL_TEXT_SIZE = 18;

	//SoundManager側をそのまま使うと計算が合わなくなるため、後で数値を書き換えてこの定数は不要にする
	constexpr int VOLUME_BGM_TITLE = static_cast<int>(SoundManager::VOLUME_BGM *1.17f);
	constexpr int VOLUME_BGM_MAIN = 255 * 75 / 100;

	constexpr int TITLE_MODEL_ROT_X = -90;
	constexpr int TITLE_MODEL_ROT_Z = 0;
}

//コンストラクタ
SceneTitle::SceneTitle(void):frame_(INITIAL_COUNT)
{
	Col.Read();

	// LoadSoundMem で読み込む音データの音程を１オクターブ低くする
	SetCreateSoundPitchRate(TITLE_BGM_PITCH_OFFSET);

	bgmplay_ = LoadSoundMem((Application::PATH_BGM + "GameBgm/nc378681_【東方アレンジ】砕月～萃めた夢の欠片～【東方動画BGM支援】.wav").c_str());
	SoundManager::GetInstance().ChangeVolumeSoundMem_bgm(VOLUME_BGM_MAIN, bgmplay_);
	PlaySoundMem(bgmplay_, DX_PLAYTYPE_LOOP);
	
	// LoadSoundMem で読み込む音データの音程を戻す
	SetCreateSoundPitchRate(DEFAULT_SOUND_PITCH_OFFSET);

	bgm_.SetBgmInputType(BgmType::none);
	f_col = Col.BRed;
	f_col2 = Col.Yellow;
	
	update_ = &SceneTitle::NormalUpdate;

	menuList_ = {
		"ゲームスタート",
		"ゲームチュートリアル:",
		"ゲームBGM",
		"ゲームスコア",
	};

	menuFuncTable_ = {
		{"ゲームスタート",[this](InputManager& messageM) {
			StartGameScene();
			SceneManager::GetInstance().BgmChang(bgm_.GetBgmInputType().c_str());
			}
		},
		
		{"ゲームチュートリアル:" ,[this](InputManager&) {

				UpdateKey();
			}
		},
		{"ゲームBGM",[this](InputManager&) {
				if (SoundManager::GetInstance().GetBgmVolume() == 0)
				{
					MessageManager::GetInstance().ShowNewMessage("BGMがoffなので使用不可", MessageManager::DEFAULT_MESSAGE_TIME/2);
					return;
				}
				if (bgm_switc == false) {
					bgm_switc = true;
					bgm_switc_n = false;
					update_ = &SceneTitle::AppearUpdate;
					}
			}},
			{"ゲームスコア",[this](InputManager&) {
					if (scor_switc == false) {
						scor_switc = true;
					
						update_ = &SceneTitle::AppearUpdate;
					}
				}
			},
	};

	menuList_b = {
		"ラクトガール ～ 少女密室",
		"月時計 ～ルナ・ダイアル",
		"亡き王女の為のセプテット",
		"U.N.オーエンは彼女なのか?",
		"不可能弾幕には反則を_アレンジ",
		"敗北の英雄～Somehow_survived",
		"砕月～萃めた夢の欠片～",
		"          閉じる",
	};

	menuFuncTable_b = {
		{"ラクトガール ～ 少女密室",[this](InputManager& messageM) {
			bgm_.SetBgmInputType(BgmType::eMenu_locked);
			f_col = Col.BRed;
			f_col2 = Col.Yellow;
			
			}
		},
		{"月時計 ～ルナ・ダイアル",[this](InputManager&) {
			bgm_.SetBgmInputType(BgmType::eMenu_Luna);
			f_col = Col.SkyBlue;
			f_col2 = Col.White;
			
			}
		},
		{"亡き王女の為のセプテット",[this](InputManager&) {
				bgm_.SetBgmInputType(BgmType::eMenu_septet);
				
				f_col = Col.Purple;
				f_col2 = Col.White;
			}
		},
		{"U.N.オーエンは彼女なのか?",[this](InputManager&) {

				bgm_.SetBgmInputType(BgmType::eMenu_UNowen);
				f_col = Col.Yellow;
				f_col2 = Col.BRed;
				
			}
		},
		{"不可能弾幕には反則を_アレンジ",[this](InputManager&) {

				bgm_.SetBgmInputType(BgmType::eMenu_Fukanou);
				f_col = Col.Yellow;
				f_col2 = Col.Black;
			}
		},
		{"敗北の英雄～Somehow_survived",[this](InputManager&) {

				bgm_.SetBgmInputType(BgmType::eMenu_Haiboku);
				f_col = Col.BRed;
				f_col2 = Col.LinePink;
			}
		},
		{"砕月～萃めた夢の欠片～",[this](InputManager&) {

				bgm_.SetBgmInputType(BgmType::eMenu_himetyumeno);
				f_col = Col.Green;
				f_col2 = Col.Blue;
			}
		},
		{"          閉じる",[this](InputManager&) {
				ExitBgmMenu();
			}},
	};

	menuList_s = {
		"          閉じる",
	};

	menuFuncTable_s = {
		
		{"          閉じる",[this](InputManager&) {
				ExitScorMenu();
			}},
	};

	tyrNow_ = false;
	ResetTable();
	keystr_ = {
		"on","off"
	};
	bgm_switc = false;
	bgm_switc_n = false;
	scor_switc = false;
	for (const auto keyvalue : inputTable_) {
		currentInput_[keyvalue.first] = false;
	}
	lastInput_ = currentInput_;
	for (int f = 0; f < FONT_MAX; ++f) {
		fontHndle_[f] = -1;
	}
	
	bgmcount_ = INITIAL_COUNT;
	currentIndex_ = INITIAL_COUNT;
	currentIndex_b = INITIAL_COUNT;
	currentIndex_s = INITIAL_COUNT;
	
	// 描画する文字列のサイズを設定
	f_size = INVALID_HANDLE;

	miniFont_ = INVALID_HANDLE;
	countimg_ = INITIAL_COUNT;
	countimg_end = YOZORA_DISPLAY_TIME;
	titleModel_ = INVALID_HANDLE;
	titleModelRotY_ = INITIAL_COUNT;
}
void SceneTitle::ExitBgmMenu()
{
	bgm_switc_n = true;
	bgm_switc = false;
	DisappearUpdate();
	update_ = &SceneTitle::NormalUpdate;
	bgmendcount_ = BGM_END_FRAME_COUNT;
}
void SceneTitle::ExitScorMenu()
{
	scor_switc = false;
	update_ = &SceneTitle::DisappearUpdate;
	update_ = &SceneTitle::NormalUpdate;
}
//デストラクタ
SceneTitle::~SceneTitle(void)
{
	for (int f = 0; f < FONT_MAX; ++f) {
		DeleteFontToHandle(fontHndle_[f]);
	}
	countimg_ = INITIAL_COUNT;
	countimg_end = YOZORA_DISPLAY_TIME;
	yc = INITIAL_COUNT;
	frame_ = INITIAL_COUNT;
	bgmcount_ = INITIAL_COUNT;
	bgmendcount_ = INVALID_HANDLE;
	DeleteSoundMem(bgmplay_);
	DeleteSoundMem(bgmtamesi_);
	DeleteGraph(f_size);
	bgm_.GetBgmName().clear();
}

//初期化
bool SceneTitle::Init(void)
{
	SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::FIXED_POINT);
	SoundManager::GetInstance().SetActiveBGM(true);

	titleImage = INVALID_HANDLE;
	
	yozoraImage_[0] = LoadGraph((Application::PATH_IMAGE + "yozora1.png").c_str());
	if (yozoraImage_[0] == INVALID_HANDLE)
	{
		//画像読み込み失敗
		OutputDebugString("タイトル画像読み込み失敗");
		return false;
	}

	yozoraImage_[1] = LoadGraph((Application::PATH_IMAGE + "yozora2.png").c_str());
	if (yozoraImage_[1] == INVALID_HANDLE)
	{
		//画像読み込み失敗
		OutputDebugString("タイトル画像読み込み失敗");
		return false;
	}
	yozoraImage_[2] = LoadGraph((Application::PATH_IMAGE + "yozora3.png").c_str());
	if (yozoraImage_[2] == INVALID_HANDLE)
	{
		//画像読み込み失敗
		OutputDebugString("タイトル画像読み込み失敗");
		return false;
	}

	//タイトル画像
	titleImage = LoadGraph((Application::PATH_IMAGE + "タイトル-東方シューティング3.png").c_str());
	if (titleImage == INVALID_HANDLE)
	{
		//画像読み込み失敗
		OutputDebugString("タイトル画像読み込み失敗");
		return false;
	}

	tyrNow_ = true;
	nomal_switc = { "on" };
	fontHndle_[0] = CreateFontToHandle("源暎ぽっぷる Black", TITLE_FONT_SIZE,
		TITLE_FONT_EDGE_SIZE, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	fontHndle_[1] = CreateFontToHandle("源暎ぽっぷる Black", SUBTITLE_FONT_SIZE,
		SUBTITLE_FONT_EDGE_SIZE, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	miniFont_ = CreateFontToHandle("源暎ぽっぷる Black", CONTROL_TEXT_SIZE, MINI_FONT_EDGE_SIZE
		, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);

	titleModel_ = MV1LoadModel((Application::PATH_MODEL + "御札/ofuda.mv1").c_str());
	MV1SetScale(titleModel_, VScale(AsoUtility::VECTOR_ONE, TITLE_MODEL_SCALE));
	MV1SetRotationXYZ(titleModel_, { AsoUtility::Deg2RadF(TITLE_MODEL_ROT_X),TITLE_MODEL_ROT_Y,AsoUtility::Deg2RadF(TITLE_MODEL_ROT_Z)});
	MV1SetPosition(titleModel_, {
			TITLE_MODEL_POS_X,
			TITLE_MODEL_POS_Y,
			TITLE_MODEL_POS_Z
		});
	countimg_ = INITIAL_COUNT;
	countimg_end = YOZORA_DISPLAY_TIME;
	//タイトル画像
	return true;
}
//更新
void SceneTitle::Update(void)
{
	
	auto& inputM = InputManager::GetInstance();
	auto& messageM = MessageManager::GetInstance();

	// 押したか押してないか記録する部分
	lastInput_ = currentInput_; // 前のプッシュ情報記録

	// キーボード情報
	char keystate[KEY_STATE_SIZE] = {};
	lastInput_ = currentInput_;
	for (const auto keyvalue : inputTable_) {     // テーブルの行を回す
		for (auto input : keyvalue.second) {      // 特定のキー入力情報
			bool pressed = false;                 // 押してない状態で初期化
			if (input.type == configuTpye::c_tyr) {
				pressed = keystate[input.code];
			}
			currentInput_[keyvalue.first] = pressed;
			if (pressed) {
				break;
			}
		}
	}
	
	UpdateYozora();
	(this->*update_)();

	//アニメーション
	constexpr float ROTSPD = TITLE_MODEL_ROT_SPEED;;
	titleModelRotY_ = fmodf(titleModelRotY_+(ROTSPD/Application::FPS), FULL_ROTATION_DEG);
	MV1SetRotationXYZ(titleModel_, { AsoUtility::Deg2RadF(TITLE_MODEL_ROT_X), AsoUtility::Deg2RadF(titleModelRotY_) ,AsoUtility::Deg2RadF(TITLE_MODEL_ROT_Z)});
}
//描画
void SceneTitle::Draw(void)
{
	SetFontSize(f_size);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, TITLE_DRAW_COLOR, true);
	
	YozoraDraw();
	if (titleModel_ != INVALID_HANDLE)
	{
		MV1DrawModel(titleModel_);
	}
	else
	{
		DrawSphere3D(AsoUtility::VECTOR_ZERO,
			MODEL_SPHERE_RADIUS,
			MODEL_SPHERE_SEGMENTS,
			MODEL_SPHERE_COLOR,
			MODEL_SPHERE_OUTLINE_COLOR,
			true);
	}
	//フェードカウント用
	DrawGraph(TITLE_IMAGE_POS_X, TITLE_IMAGE_POS_Y, titleImage, TRUE);

	if (bgm_switc){
		NormalBgmDraw();
		return;
	}

	if (scor_switc) {
		NormalDraw();
		return;
	}

	DrawMenuList();
	
	DrawControlKey();

	//スコア描画
	ScoreManager& scoreManager = ScoreManager::GetInstance();
	DrawFormatStringToHandle(
		HIGH_SCORE_POS_X,
		HIGH_SCORE_POS_Y,
		DRAW_COLOR_WHITE, 
		miniFont_, "ハイスコア : %d", scoreManager.GetHighScore());
}
//解放
bool SceneTitle::Release(void)
{
	DeleteFontToHandle(miniFont_);

	for (int i = 0; i < YOZORA_MAX; i++) {
		DeleteGraph(yozoraImage_[i]);
	}
	
	DeleteGraph(titleImage);

	MV1DeleteModel(titleModel_);

	return true;
}

void SceneTitle::ResetTable()
{
	inputTable_ = {

			{"on",{
			{configuTpye::c_tyr,KEY_INPUT_1}}
			}
	};
}

bool SceneTitle::IsNow(const std::string& eventstr) const
{
	return currentInput_.at(eventstr) && !lastInput_.at(eventstr);;
}

void SceneTitle::UpdateKey()
{
	SoundManager& sManager = SoundManager::GetInstance();
	
	auto& inputM = InputManager::GetInstance();
	auto& messageM = MessageManager::GetInstance();

	if (tyrNow_ == true) {
		nomal_switc = { "off" };
		tyrNow_ = false;
		GetMod();
	}
	else
	{
		nomal_switc = { "on" };
		tyrNow_ = true;
		GetMod();
	}

}

void SceneTitle::UpdateYozora()
{
	// カウンタ加算
	countimg_++;

	if (!isFading) {
		// 表示時間を超えたらフェード開始
		if (countimg_ >= displayTime) {
			isFading = true;
			fadeCounter = INITIAL_COUNT;
			countimg_ = INITIAL_COUNT;
			nextYc = (yc + NEXT_IMAGE_INDEX_OFFSET) % YOZORA_MAX;
		}
	}
	else {
		// フェード中
		fadeCounter++;

		// 完了したら切り替え
		if (fadeCounter >= fadeTime) {
			yc = nextYc;
			isFading = false;
			fadeCounter = INITIAL_COUNT;
		}
	}
}

void SceneTitle::DrawMenuList()
{
	constexpr int line_start_y = margin_size + MENU_LINE_START_Y_OFFSET;
	constexpr int line_start_x = margin_size + MENU_LINE_START_X_OFFSET;
	int lineY = line_start_y;
	
	int lineX2 = line_start_x + MENU_LINE_WIDTH;
	
	unsigned int col2 = Col.SkyBlue;
	f_size = 35;
	auto& currentStr = menuList_[currentIndex_];
	auto& currentStr2 = "ゲームチュートリアル:";
	for (auto& row : menuList_) {
		int lineX = line_start_x;
		int lineY2 = line_start_y + MENU_LINE_HEIGHT;
		
		unsigned int col = Col.SkyBlue;
		
		if (row == currentStr) {

			DrawString(lineX - MENU_ARROW_X_OFFSET, lineY, "⇒", MENU_ARROW_COLOR);
			col = MENU_SELECTED_COLOR;
			lineX += MENU_SELECTED_X_OFFSET;
			
			SetFontSize(FONT_SIZE_RESET);
		}
		else if (currentStr == currentStr2) {
			col2 = MENU_SELECTED_COLOR;
			lineX2 = line_start_x + MENU_TUTORIAL_X_OFFSET;
		}

		DrawFormatStringToHandle(lineX + 1, lineY + 1, DRAW_COLOR_BLACK, fontHndle_[0], "%s", row.c_str());
		DrawFormatStringToHandle(lineX, lineY, col, fontHndle_[0], "%s", row.c_str());

		DrawStringToHandle(lineX2+ 1, lineY2 + 1, nomal_switc.c_str(), DRAW_COLOR_BLACK, fontHndle_[0]);
		DrawStringToHandle(lineX2, lineY2, nomal_switc.c_str(), col2, fontHndle_[0]);

		lineY += menu_line_height;
	}
}

void SceneTitle::DrawBgmList()
{
	constexpr int line_start_y = margin_size + BGM_LINE_START_Y_OFFSET;
	constexpr int line_start_x = margin_size + BGM_LINE_START_X_OFFSET;
	int lineY = line_start_y;
	f_size = 35;
	auto currentStr = menuList_b[currentIndex_b];
	for (auto& row : menuList_b) {
		int lineX = line_start_x;
		unsigned int col = MENU_DEFAULT_COLOR;
		if (row == currentStr) {

			DrawString(lineX - BGM_SELECTED_X_OFFSET, lineY, "⇒", MENU_ARROW_COLOR);
			col = MENU_SELECTED_COLOR;
			lineX += BGM_SELECTED_MOVE_X;

			SetFontSize(FONT_SIZE_RESET);
		}
		DrawFormatStringToHandle(lineX + MENU_TEXT_SHADOW_OFFSET, lineY + MENU_TEXT_SHADOW_OFFSET, DRAW_COLOR_BLACK, fontHndle_[0], "%s", row.c_str(), nomal_switc.c_str());
		DrawFormatStringToHandle(lineX, lineY, col, fontHndle_[0], "%s", row.c_str(), nomal_switc.c_str());
		lineY += menu_line_height;
	}
}

void SceneTitle::DrawScorList()
{
	//スコアロード
	ScoreData scoreData;
	const Size& wsize = Application::GetInstance().GetWindowSize();

	constexpr int Sline_start_y = margin_size + SCORE_LINE_START_Y_OFFSET;
	constexpr int line_start_y = margin_size + SCORE_LIST_START_Y_OFFSET;
	constexpr int line_start_x = margin_size + SCORE_LINE_START_X_OFFSET;

	int lineY = line_start_y;
	int SlineY = Sline_start_y;
	f_size = TITLE_FONT_SIZE;
	auto currentStr = menuList_s[currentIndex_s];
	for (auto& row : menuList_s) {
		int lineX = line_start_x;
		unsigned int col = MENU_DEFAULT_COLOR;
		if (row == currentStr) {

			DrawString(lineX - BGM_SELECTED_X_OFFSET, lineY, "⇒", MENU_ARROW_COLOR);
			col = MENU_SELECTED_COLOR;
			lineX += BGM_SELECTED_MOVE_X;

			SetFontSize(FONT_SIZE_RESET);
		}
		scoreData.DrawScoreBoard(wsize.width, SlineY, fontHndle_[0], scoreData.scoreBoard_);
		DrawFormatStringToHandle(lineX + MENU_TEXT_SHADOW_OFFSET, lineY + MENU_TEXT_SHADOW_OFFSET, DRAW_COLOR_BLACK, fontHndle_[0], "%s", row.c_str(), nomal_switc.c_str());
		DrawFormatStringToHandle(lineX, lineY, col, fontHndle_[0], "%s", row.c_str(), nomal_switc.c_str());
		lineY += menu_line_height;
	}
}

void SceneTitle::YozoraDraw()
{
	if (isFading) {
		float t = static_cast<float>(fadeCounter) / fadeTime;
		int alphaOut = static_cast<int>((FADE_RATE_MAX - t) * FULL_ALPHA);
		int alphaIn = static_cast<int>(t * FULL_ALPHA);

		// 現在の画像をフェードアウト
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alphaOut);
		DrawGraph(0, 0, yozoraImage_[yc], true);

		// 次の画像をフェードイン
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alphaIn);
		DrawGraph(0, 0, yozoraImage_[nextYc], true);

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, INITIAL_COUNT);  // 元に戻す
	}
	else {
		// 通常描画
		DrawGraph(0, 0, yozoraImage_[yc], true);
	}
}

std::string SceneTitle::GetMod()
{
	return nomal_switc;
}

void SceneTitle::NormalUpdate()
{
	auto& inputM = InputManager::GetInstance();
	auto& messageM = MessageManager::GetInstance();
	SoundManager& sManager = SoundManager::GetInstance();
	if (inputM.IsTrgDown(KEY_INPUT_UP)) {
		currentIndex_ = static_cast<int>((currentIndex_ + menuList_.size() - MENU_INDEX_PREVIOUS_OFFSET) % menuList_.size());
		sManager.PlaySE(SoundManager::SOUND_ID::CURSOR, true);
	}
	else if (inputM.IsTrgDown(KEY_INPUT_DOWN)) {
		currentIndex_ = (currentIndex_ + MENU_INDEX_NEXT_OFFSET) % menuList_.size();
		sManager.PlaySE(SoundManager::SOUND_ID::CURSOR, true);
	}
	if (inputM.IsTrgDown(KEY_INPUT_X))
	{
		//最上段に戻す
		if (currentIndex_ != MENU_INDEX_FIRST)
		{
			currentIndex_ = MENU_INDEX_FIRST;
			sManager.PlaySE(SoundManager::SOUND_ID::CURSOR, true);
		}
	}

	if (inputM.IsTrgDown(KEY_INPUT_C))
	{
		//トグル
		sManager.PlaySE(SoundManager::SOUND_ID::CURSOR, true);
		sManager.SetActiveBGM(!(sManager.IsActiveBGM()));
		sManager.ChangeVolumeSoundMem_bgm(VOLUME_BGM_MAIN, bgmplay_);

	}
	if (inputM.IsTrgDown(KEY_INPUT_R))
	{
		//ショートカット
		UpdateKey();
		sManager.PlaySE(SoundManager::SOUND_ID::CURSOR, true);
	}
	if (inputM.GetInstance().IsTrgDown(KEY_INPUT_Z)) {
		auto selectedName = menuList_[currentIndex_];
		menuFuncTable_[selectedName](inputM);
		sManager.PlaySE(SoundManager::SOUND_ID::CURSOR, true);
		
		return;
	}
	if (inputM.IsTrgDown(KEY_INPUT_SPACE))
	{
		auto selectedName = menuList_[currentIndex_];
		menuFuncTable_[selectedName](inputM);
		sManager.PlaySE(SoundManager::SOUND_ID::CURSOR, true);

		return;
	}
}

void SceneTitle::bgmCountUpdat()
{
	SceneManager::GetInstance().BgmChang(bgm_.GetBgmInputType().c_str());
	SoundManager& sManager = SoundManager::GetInstance();
	sManager.ChangeVolumeSoundMem_bgm(static_cast<int>(VOLUME_BGM_MAIN * BGM_PREVIEW_VOLUME_RATE), bgmplay_);
	if (bgmcount_ > INITIAL_COUNT)
	{
		bgmcount_ = INITIAL_COUNT;
		DeleteSoundMem(bgmtamesi_);
		bgmtamesi_ = LoadSoundMem((SceneManager::GetInstance().GetBgmChangType()).c_str());

		sManager.ChangeVolumeSoundMem_bgm(static_cast<int>(VOLUME_BGM_TITLE * BGM_TITLE_VOLUME_RATE), bgmtamesi_);
		PlaySoundMem(bgmtamesi_, DX_PLAYTYPE_BACK);
	}

	if (bgmcount_ <= bgmendcount_)
	{
		sManager.ChangeVolumeSoundMem_bgm(VOLUME_BGM_MAIN, bgmplay_);
		bgmendcount_ = INVALID_COUNT;
		bgmcount_ = INITIAL_COUNT;
		DeleteSoundMem(bgmtamesi_);
	}
}

void SceneTitle::AppearUpdate()
{
	if (++frame_ >= appear_interval) {
		if (bgm_switc) {
			bgmcount_++;
			NormalBgmUpdate();
			
		}
		if (scor_switc) {
			
			NormalScorUpdate();
		}
	}
}

void SceneTitle::DisappearUpdate()
{
	
	if (--frame_ <= INITIAL_COUNT) {
		NormalUpdate();
		ProcessDraw();
		bgmcount_ = INITIAL_COUNT;
	}
}

int SceneTitle::GetbgmCount()
{
	return bgmcount_;
}

void SceneTitle::NormalBgmUpdate()
{
	
	auto& inputM = InputManager::GetInstance();
	auto& messageM = MessageManager::GetInstance();
	SoundManager& sManager = SoundManager::GetInstance();
	if (bgm_switc) {
		
		ProcessDraw();
		DisappearUpdate();
	}
	
	if (inputM.GetInstance().IsTrgDown(KEY_INPUT_UP)) {
		currentIndex_b = static_cast<int>((currentIndex_b + menuList_b.size() - MENU_INDEX_PREVIOUS_OFFSET) % menuList_b.size());
		sManager.PlaySE(SoundManager::SOUND_ID::CURSOR, true);
	}
	else if (inputM.GetInstance().IsTrgDown(KEY_INPUT_DOWN)) {
		currentIndex_b = (currentIndex_b + MENU_INDEX_NEXT_OFFSET) % menuList_b.size();
		sManager.PlaySE(SoundManager::SOUND_ID::CURSOR, true);
	}
	if (inputM.GetInstance().IsTrgDown(KEY_INPUT_Z)) {
		auto selectedName_b = menuList_b[currentIndex_b];
		menuFuncTable_b[selectedName_b](inputM);
		
		bgmCountUpdat();
		
		return;
	}	
	
	if (inputM.GetInstance().IsTrgDown(KEY_INPUT_SPACE)) {
		auto selectedName_b = menuList_b[currentIndex_b];
		menuFuncTable_b[selectedName_b](inputM);
		
		bgmCountUpdat();
		
		return;
	}

	if (BGM_FADE_END_FRAME <= bgmcount_) {
		sManager.ChangeVolumeSoundMem_bgm(VOLUME_BGM_MAIN, bgmplay_);
		
		DeleteSoundMem(bgmtamesi_);
		bgmcount_ = INITIAL_COUNT;
	}
	else if (BGM_FADE_NEAR_END_FRAME <= bgmcount_) {
		sManager.ChangeVolumeSoundMem_bgm(static_cast<int>(VOLUME_BGM_TITLE * BGM_FADE_VOLUME_RATE), bgmtamesi_);

	}
	else if (BGM_FADE_MIDDLE_FRAME <= bgmcount_) {
		sManager.ChangeVolumeSoundMem_bgm(static_cast<int>(VOLUME_BGM_TITLE * BGM_FADE_MIDDLE_VOLUME_RATE), bgmtamesi_);

	}
	else if (BGM_FADE_START_FRAME <= bgmcount_) {
		sManager.ChangeVolumeSoundMem_bgm(static_cast<int>(VOLUME_BGM_TITLE * BGM_FADE_NEAR_END_VOLUME_RATE), bgmtamesi_);

	}

	if (bgm_switc==false) {
	}
}

void SceneTitle::NormalScorUpdate()
{
	auto& inputM = InputManager::GetInstance();
	auto& messageM = MessageManager::GetInstance();
	SoundManager& sManager = SoundManager::GetInstance();
	if (scor_switc) {
	}
	ProcessDraw();
	DisappearUpdate();

	if (inputM.GetInstance().IsTrgDown(KEY_INPUT_UP)) {
		currentIndex_s = static_cast<int>((currentIndex_s + menuList_s.size() - MENU_INDEX_PREVIOUS_OFFSET) % menuList_s.size());
		sManager.PlaySE(SoundManager::SOUND_ID::CURSOR, true);
	}
	else if (inputM.GetInstance().IsTrgDown(KEY_INPUT_DOWN)) {
		currentIndex_s = (currentIndex_s + MENU_INDEX_NEXT_OFFSET) % menuList_s.size();
		sManager.PlaySE(SoundManager::SOUND_ID::CURSOR, true);
	}
	if (inputM.GetInstance().IsTrgDown(KEY_INPUT_Z)) {
		auto selectedName_b = menuList_s[currentIndex_s];
		menuFuncTable_s[selectedName_b](inputM);

		return;
	}	if (inputM.GetInstance().IsTrgDown(KEY_INPUT_SPACE)) {
		auto selectedName_b = menuList_s[currentIndex_s];
		menuFuncTable_s[selectedName_b](inputM);

		return;
	}
}

void SceneTitle::ProcessDraw()
{
	const Size& wsize = Application::GetInstance().GetWindowSize();
	int centerY = wsize.height / FRAME_MARGIN_COUNT; // 画面中心Y
	int frameHalfHeight = (wsize.height - margin_size * FRAME_MARGIN_COUNT) / FRAME_MARGIN_COUNT; // 枠の高さの半分

	// 出現・消滅時の高さ変化率(0.0～1.0)
	float rate = static_cast<float>(frame_) /
		static_cast<float>(appear_interval);

	frameHalfHeight *= static_cast<int>(rate);

	// 白っぽいセロファン
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, BLEND_ALPHA_PROCESS);
	DrawBox(margin_size,
		centerY - frameHalfHeight,
		wsize.width - margin_size,
		centerY + frameHalfHeight,
		DRAW_COLOR_WHITE,
		true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, INITIAL_COUNT);
	// 白枠
	DrawBoxAA(static_cast<float>((margin_size)),
		static_cast<float>(centerY - frameHalfHeight),
		static_cast<float>(wsize.width - margin_size),
		static_cast<float>(centerY + frameHalfHeight),
		DRAW_COLOR_WHITE,
		false, 
		FRAME_LINE_WIDTH);
}

void SceneTitle::NormalDraw()
{
	const Size& wsize = Application::GetInstance().GetWindowSize();
	constexpr int line_start_y = margin_size + SCORE_FRAME_POS_Y_OFFSET;
	constexpr int line_start_x = SCORE_FRAME_POS_X_OFFSET;

	// 白っぽいセロファン
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, BLEND_ALPHA_MENU);
	DrawBox(line_start_x,
		line_start_y,
		wsize.width - line_start_x,
		wsize.height - line_start_y,
		DRAW_COLOR_WHITE,
		true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, INITIAL_COUNT);
	// 白枠
	DrawBoxAA(static_cast<float>(line_start_x),
		static_cast<float>(line_start_y),
		static_cast<float>(wsize.width - line_start_x),
		static_cast<float>(wsize.height - line_start_y),
		DRAW_COLOR_WHITE,
		false,
		FRAME_LINE_WIDTH);
	DrawStringToHandle(SCORE_TITLE_POS_X,
		SCORE_TITLE_POS_Y, "スコアランキング", DRAW_COLOR_BLUE, fontHndle_[0], Col.LinePink);
	DrawScorList();
}

void SceneTitle::NormalBgmDraw()
{
	const Size& wsize = Application::GetInstance().GetWindowSize();
	// 白っぽいセロファン
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, BLEND_ALPHA_MENU);
	DrawBox(margin_size,
		margin_size,
		wsize.width - margin_size,
		wsize.height - margin_size,
		DRAW_COLOR_WHITE,
		true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, INITIAL_COUNT);
	// 白枠
	DrawBoxAA(static_cast<float>(margin_size),
		static_cast<float>(margin_size),
		static_cast<float>(wsize.width - margin_size),
		static_cast<float>(wsize.height - margin_size),
		DRAW_COLOR_WHITE,
		false,
		FRAME_LINE_WIDTH);
	DrawStringToHandle(margin_size + BGM_NAME_POS_X_OFFSET, margin_size + BGM_NAME_POS_Y_OFFSET, "現在の曲", 0x0000ff, fontHndle_[0],Col.Pink);
	DrawStringToHandle(margin_size + BGM_NAME_POS_X_OFFSET, margin_size + BGM_NAME_POS_Y_OFFSET_2, bgm_.GetBgmName().c_str(), f_col, fontHndle_[1], f_col2);
	DrawBgmList();
}

void SceneTitle::StartGameScene()
{
	if (tyrNow_)
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TUTORIAL, true);
	}
	else
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME, true);
	}
}

void SceneTitle::DrawControlKey()
{
	constexpr int SPACE = 8;
	constexpr int KEY_TEXT_ROW = 3;

	const SoundManager& sManager = SoundManager::GetInstance();

	//上から順に描画
	int drawY = Application::SCREEN_SIZE_Y - SPACE - (CONTROL_TEXT_SIZE * KEY_TEXT_ROW);

	DrawStringToHandle(SPACE, drawY, "矢印キー     : 選択", DRAW_COLOR_WHITE, miniFont_);
	//行を一つ下に
	drawY += CONTROL_TEXT_SIZE;

	DrawStringToHandle(SPACE, drawY, "Zキー        : 決定", DRAW_COLOR_WHITE, miniFont_);
	drawY += CONTROL_TEXT_SIZE;

	DrawFormatStringToHandle(SPACE, drawY, DRAW_COLOR_WHITE, miniFont_, "Cキー        : BGM切り替え %s",
		sManager.IsActiveBGM() ? "(現在の状態 : on)" : "(現在の状態 : off)");
	drawY += CONTROL_TEXT_SIZE;

}
