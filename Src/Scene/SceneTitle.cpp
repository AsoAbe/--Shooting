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
SceneTitle::SceneTitle(void):frame_(0)
{
	Col.Read();

	// LoadSoundMem で読み込む音データの音程を１オクターブ低くする
	SetCreateSoundPitchRate(-1200.0f);

	bgmplay_ = LoadSoundMem((Application::PATH_BGM + "GameBgm/nc378681_【東方アレンジ】砕月～萃めた夢の欠片～【東方動画BGM支援】.wav").c_str());
	//ChangeVolumeSoundMem(255 * 85 / 100, bgmplay_);
	SoundManager::GetInstance().ChangeVolumeSoundMem_bgm(VOLUME_BGM_MAIN, bgmplay_);
	PlaySoundMem(bgmplay_, DX_PLAYTYPE_LOOP);
	
	// LoadSoundMem で読み込む音データの音程を戻す
	SetCreateSoundPitchRate(0.0f);

	bgm_.SetBgmInputType(BgmType::none);
	f_col = Col.BRed;
	f_col2 = Col.Yellow;
	
	update_ = &SceneTitle::NormalUpdate;

	menuList_ = {
		"ゲームスタート",
		//"ゲーム",
		//"ゲーム説明",
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
		//{"ゲーム",[this](InputManager&) {
		//	//MessageManager::GetInstance().ShowNewMessage("Test : " + std::to_string(GetRand(9)), MessageManager::DEFAULT_MESSAGE_TIME);
		//	}
		//},
		//{"ゲーム説明",[this](InputManager&) {
		//		//MessageManager::GetInstance().ShowNewMessage("Test : " + std::to_string(GetRand(9)), MessageManager::DEFAULT_MESSAGE_TIME);
		//	}
		//},
		{"ゲームチュートリアル:" ,[this](InputManager&) {

				UpdateKey();
				//MessageManager::GetInstance().ShowNewMessage("Test : " + nomal_switc, MessageManager::DEFAULT_MESSAGE_TIME);

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
				//else
				//{
				//	bgm_switc = true;
				//	bgm_switc_n = false;
				//	AppearUpdate();
				//}
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
				//SceneManager::GetInstance().BgmChang(bgm_.GetBgmInputType().c_str());
			}
		},
		{"U.N.オーエンは彼女なのか?",[this](InputManager&) {

				bgm_.SetBgmInputType(BgmType::eMenu_UNowen);
				//SceneManager::GetInstance().BgmChang(bgm_.GetBgmInputType().c_str());
				f_col = Col.Yellow;
				f_col2 = Col.BRed;
				
			}
		},
		{"不可能弾幕には反則を_アレンジ",[this](InputManager&) {

				bgm_.SetBgmInputType(BgmType::eMenu_Fukanou);
				f_col = Col.Yellow;
				f_col2 = Col.Black;
				//SceneManager::GetInstance().BgmChang(bgm_.GetBgmInputType().c_str());
			}
		},
		{"敗北の英雄～Somehow_survived",[this](InputManager&) {

				bgm_.SetBgmInputType(BgmType::eMenu_Haiboku);
				f_col = Col.BRed;
				f_col2 = Col.LinePink;
				//SceneManager::GetInstance().BgmChang(bgm_.GetBgmInputType().c_str());
			}
		},
		{"砕月～萃めた夢の欠片～",[this](InputManager&) {

				bgm_.SetBgmInputType(BgmType::eMenu_himetyumeno);
				f_col = Col.Green;
				f_col2 = Col.Blue;
				//SceneManager::GetInstance().BgmChang(bgm_.GetBgmInputType().c_str());
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
	
	bgmcount_ = 0;
	currentIndex_ = 0;
	currentIndex_b = 0;
	currentIndex_s = 0;
	
	// 描画する文字列のサイズを設定
	f_size = -1;

	miniFont_ = -1;
	countimg_ = 0;
	countimg_end = 0;
	titleModel_ = -1;
	titleModelRotY_ = 0;
}
void SceneTitle::ExitBgmMenu()
{
	bgm_switc_n = true;
	bgm_switc = false;
	DisappearUpdate();
	update_ = &SceneTitle::NormalUpdate;
	bgmendcount_ = 20;
	//SceneManager::GetInstance().BgmChang(bgm_.GetBgmInputType().c_str());
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
	countimg_ = 0;
	countimg_end = 300;
	yc = 0;
	frame_ = 0;
	bgmcount_ = 0;
	bgmendcount_ = -1;
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

	titleImage = -1;
	
	yozoraImage_[0] = LoadGraph((Application::PATH_IMAGE + "yozora1.png").c_str());
	if (yozoraImage_[0] == -1)
	{
		//画像読み込み失敗
		OutputDebugString("タイトル画像読み込み失敗");
		return false;
	}

	yozoraImage_[1] = LoadGraph((Application::PATH_IMAGE + "yozora2.png").c_str());
	if (yozoraImage_[1] == -1)
	{
		//画像読み込み失敗
		OutputDebugString("タイトル画像読み込み失敗");
		return false;
	}
	yozoraImage_[2] = LoadGraph((Application::PATH_IMAGE + "yozora3.png").c_str());
	if (yozoraImage_[2] == -1)
	{
		//画像読み込み失敗
		OutputDebugString("タイトル画像読み込み失敗");
		return false;
	}

	//タイトル画像
	titleImage = LoadGraph((Application::PATH_IMAGE + "タイトル-東方シューティング3.png").c_str());
	if (titleImage == -1)
	{
		//画像読み込み失敗
		OutputDebugString("タイトル画像読み込み失敗");
		return false;
	}

	tyrNow_ = true;
	nomal_switc = { "on" };
	fontHndle_[0] = CreateFontToHandle("源暎ぽっぷる Black", 35, 5
		, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	fontHndle_[1] = CreateFontToHandle("源暎ぽっぷる Black", 25, 3
		, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);
	miniFont_ = CreateFontToHandle("源暎ぽっぷる Black", CONTROL_TEXT_SIZE, 5
		, DX_FONTTYPE_ANTIALIASING_EDGE_8X8);

	titleModel_ = MV1LoadModel((Application::PATH_MODEL + "御札/ofuda.mv1").c_str());
	MV1SetScale(titleModel_, VScale(AsoUtility::VECTOR_ONE, 0.3f));
	MV1SetRotationXYZ(titleModel_, { AsoUtility::Deg2RadF(TITLE_MODEL_ROT_X),0,AsoUtility::Deg2RadF(TITLE_MODEL_ROT_Z)});
	MV1SetPosition(titleModel_, { 0,12,0 });
	countimg_ = 0;
	countimg_end = 300;
	//タイトル画像
	return true;
}
//更新
void SceneTitle::Update(void)
{
	//countimg_++;
	
	auto& inputM = InputManager::GetInstance();
	auto& messageM = MessageManager::GetInstance();

	// 押したか押してないか記録する部分
	lastInput_ = currentInput_; // 前のプッシュ情報記録

	// キーボード情報
	char keystate[256] = {};
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
	//bgmcount_++;
	/*float settim = static_cast<int>(Application::FPS * 0.25f) * -1;
	float messageEndTimeF = 0.25f * Application::FPS;
	if (settim < 0 || 0.25f) {
		bgmtamesi_ = LoadSoundMem((SceneManager::GetInstance().GetBgmChangType()).c_str());
		PlaySoundMem(bgmtamesi_, DX_PLAYTYPE_LOOP);

	}
	else if (settim < messageEndTimeF)
	{
		StopSoundMem(bgmtamesi_);
	}
	else
	{
		DeleteSoundMem(bgmtamesi_);
		return;
	}*/

	UpdateYozora();
	(this->*update_)();
	//UpdateKey();
	/*if (bgm_switc) {
		return;
	}
	NormalUpdate();*/
	//if (inputM.IsTrgDown(KEY_INPUT_SPACE))
	//{
	//	StartGameScene();
	//}

	//アニメーション
	constexpr float ROTSPD = 90;
	titleModelRotY_ = fmodf(titleModelRotY_+(ROTSPD/Application::FPS), 360);
	MV1SetRotationXYZ(titleModel_, { AsoUtility::Deg2RadF(TITLE_MODEL_ROT_X), AsoUtility::Deg2RadF(titleModelRotY_) ,AsoUtility::Deg2RadF(TITLE_MODEL_ROT_Z)});
}
//描画
void SceneTitle::Draw(void)
{
	SetFontSize(f_size);
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x110033, true);
	
	YozoraDraw();
	if (titleModel_ != -1)
	{
		MV1DrawModel(titleModel_);
	}
	else
	{
		DrawSphere3D({ 0,0,0 }, 32, 8, 0xFF0000, 0xFFFFFF, true);
	}
	//フェードカウント用
	//DrawFormatStringToHandle(0, 32, Col.White, fontHndle_[0], "%d", countimg_);

	DrawGraph(230, 8, titleImage, TRUE);
	//DrawUtility::DrawStringCenter("数字キー1でメッセージのテスト"+ keystr_.size(), 0xFFFF00, 0);
	
	//AppearUpdate();

	if (bgm_switc){
		NormalBgmDraw();
		return;
	}

	if (scor_switc) {
		NormalDraw();
		return;
	}

	//NormalDraw();

	DrawMenuList();
	
	DrawControlKey();

	//スコア描画
	ScoreManager& scoreManager = ScoreManager::GetInstance();
	//DrawFormatString(Application::SCREEN_SIZE_X / 2 - (DrawUtility::DEFAULT_TEXT_SIZE * 3), 420,
	//	0xffffff, "ハイスコア : %d", scoreManager.GetHighScore());
	DrawFormatStringToHandle(8, 8,
	0xffffff,miniFont_, "ハイスコア : %d", scoreManager.GetHighScore());
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
			/*{"off",{
				{configuTpye::c_tyr,KEY_INPUT_1}
			}},*/
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
	/*if (inputM.GetInstance().IsTrgDown(KEY_INPUT_RETURN)) {
		messageM.ShowNewMessage("Test : " , MessageManager::DEFAULT_MESSAGE_TIME);
	}*/


	if (tyrNow_ == true) {
		//messageM.ShowNewMessage("Test : off", MessageManager::DEFAULT_MESSAGE_TIME);
		nomal_switc = { "off" };
		tyrNow_ = false;
		GetMod();
	}
	else
	{
		nomal_switc = { "on" };
		//messageM.ShowNewMessage("Test : on", MessageManager::DEFAULT_MESSAGE_TIME);
		tyrNow_ = true;
		GetMod();
	}
		
	//if (inputM.GetInstance().IsTrgDown(KEY_INPUT_R)) {
	//	if (tyrNow_ == false) {
	//		//messageM.ShowNewMessage("Test : off", MessageManager::DEFAULT_MESSAGE_TIME);
	//		nomal_switc = { "off" };
	//		tyrNow_ = true;
	//	}
	//	else  
	//	{
	//		nomal_switc = { "on" };
	//		//messageM.ShowNewMessage("Test : on", MessageManager::DEFAULT_MESSAGE_TIME);
	//		tyrNow_ = false;
	//	}
	//	messageM.ShowNewMessage("Test : " + nomal_switc, MessageManager::DEFAULT_MESSAGE_TIME);
	//}
			
		
	//SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME, true);
	
}

void SceneTitle::UpdateYozora()
{
	// カウンタ加算
	countimg_++;

	if (!isFading) {
		// 表示時間を超えたらフェード開始
		if (countimg_ >= displayTime) {
			isFading = true;
			fadeCounter = 0;
			countimg_ = 0;
			nextYc = (yc + 1) % YOZORA_MAX;
		}
	}
	else {
		// フェード中
		fadeCounter++;

		// 完了したら切り替え
		if (fadeCounter >= fadeTime) {
			yc = nextYc;
			isFading = false;
			fadeCounter = 0;
		}
	}
}

void SceneTitle::DrawMenuList()
{
	constexpr int line_start_y = margin_size + 450;
	constexpr int line_start_x = margin_size + 550;
	int lineY = line_start_y;
	
	int lineX2 = line_start_x + 388;
	
	unsigned int col2 = Col.SkyBlue;
	//unsigned int Col2 = Col.Black;
	f_size = 35;
	auto& currentStr = menuList_[currentIndex_];
	auto& currentStr2 = "ゲームチュートリアル:";
	for (auto& row : menuList_) {
		int lineX = line_start_x;
		int lineY2 = line_start_y + 40;
		
		unsigned int col = Col.SkyBlue;
		
		if (row == currentStr) {

			DrawString(lineX - 30, lineY, "⇒", 0xff0000);
			col = 0xff00ff;
			//col2 = 0xff00f;
			lineX += 10;
			//lineX2 += 10;
			
			//lineY2 = +120;
			SetFontSize(0);
		}
		else if (currentStr == currentStr2) {

			

			col2 = 0xff00ff;
			//Col2 = Col.LinePink;
			lineX2 = line_start_x+ 398;
			//lineX21 = 390;
			//lineY2 = +120;
			//lineY2 -= menu_line_height;
		}

		DrawFormatStringToHandle(lineX + 1, lineY + 1, 0x000000, fontHndle_[0], "%s", row.c_str());
		DrawFormatStringToHandle(lineX, lineY, col, fontHndle_[0], "%s", row.c_str());

		DrawStringToHandle(lineX2+ 1, lineY2 + 1, nomal_switc.c_str(), 0x000000, fontHndle_[0]);
		DrawStringToHandle(lineX2, lineY2, nomal_switc.c_str(), col2, fontHndle_[0]);

		lineY += menu_line_height;
		//lineY2 += menu_line_height;
	}
	
	//lineY2 -= menu_line_height;
}

void SceneTitle::DrawBgmList()
{
	constexpr int line_start_y = margin_size + 350;
	constexpr int line_start_x = margin_size + 460;
	int lineY = line_start_y;
	f_size = 35;
	auto currentStr = menuList_b[currentIndex_b];
	for (auto& row : menuList_b) {
		int lineX = line_start_x;
		unsigned int col = 0x4444ff;
		if (row == currentStr) {

			DrawString(lineX - 40, lineY, "⇒", 0xff0000);
			col = 0xff00ff;
			lineX += 20;
			SetFontSize(0);
		}
		DrawFormatStringToHandle(lineX + 1, lineY + 1, 0x000000, fontHndle_[0], "%s", row.c_str(), nomal_switc.c_str());
		DrawFormatStringToHandle(lineX, lineY, col, fontHndle_[0], "%s", row.c_str(), nomal_switc.c_str());
		lineY += menu_line_height;
	}
}

void SceneTitle::DrawScorList()
{
	//スコアロード
	ScoreData scoreData;
	const Size& wsize = Application::GetInstance().GetWindowSize();

	constexpr int Sline_start_y = margin_size + 250;
	constexpr int line_start_y = margin_size + 550;
	constexpr int line_start_x = margin_size + 460;
	int lineY = line_start_y;
	int SlineY = Sline_start_y;
	f_size = 35;
	auto currentStr = menuList_s[currentIndex_s];
	for (auto& row : menuList_s) {
		int lineX = line_start_x;
		unsigned int col = 0x4444ff;
		if (row == currentStr) {

			DrawString(lineX - 40, lineY, "⇒", 0xff0000);
			col = 0xff00ff;
			lineX += 20;
			SetFontSize(0);
		}
		scoreData.DrawScoreBoard(wsize.width, SlineY, fontHndle_[0], scoreData.scoreBoard_);
		DrawFormatStringToHandle(lineX + 1, lineY + 1, 0x000000, fontHndle_[0], "%s", row.c_str(), nomal_switc.c_str());
		DrawFormatStringToHandle(lineX, lineY, col, fontHndle_[0], "%s", row.c_str(), nomal_switc.c_str());
		lineY += menu_line_height;
		
	}
}

void SceneTitle::YozoraDraw()
{
	



	if (isFading) {
		float t = static_cast<float>(fadeCounter) / fadeTime;
		int alphaOut = static_cast<int>((1.0f - t) * 255);
		int alphaIn = static_cast<int>(t * 255);

		// 現在の画像をフェードアウト
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alphaOut);
		DrawGraph(0, 0, yozoraImage_[yc], true);

		// 次の画像をフェードイン
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alphaIn);
		DrawGraph(0, 0, yozoraImage_[nextYc], true);

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);  // 元に戻す
	}
	else {
		// 通常描画
		DrawGraph(0, 0, yozoraImage_[yc], true);
	}
	
	/*
	for (auto& img : yozoraImage_) {
			
		

			DrawGraph(0, 0, yozoraImage_[yc], true);



		
		if (1200 <= countimg_)
		{
			countimg_ = 0;
			yc = 0;
			countimg_end = 300;
		}
		else if (countimg_end <= countimg_)
		{

			
			yc += 1;
			countimg_end += countimg_end;

			
		}
		
		
	}*/
	
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
		currentIndex_ = (currentIndex_ + menuList_.size() - 1) % menuList_.size();
		sManager.PlaySE(SoundManager::SOUND_ID::CURSOR, true);
	}
	else if (inputM.IsTrgDown(KEY_INPUT_DOWN)) {
		currentIndex_ = (currentIndex_ + 1) % menuList_.size();
		sManager.PlaySE(SoundManager::SOUND_ID::CURSOR, true);
	}
	if (inputM.IsTrgDown(KEY_INPUT_X))
	{
		//最上段に戻す
		if (currentIndex_ != 0)
		{
			currentIndex_ = 0;
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
	//ChangeVolumeSoundMem(255 * 35 / 100, bgmplay_);
	sManager.ChangeVolumeSoundMem_bgm(static_cast<int>(VOLUME_BGM_MAIN * 0.35f), bgmplay_);
	if (bgmcount_ > 0)
	{
		bgmcount_ = 0;
		DeleteSoundMem(bgmtamesi_);
		bgmtamesi_ = LoadSoundMem((SceneManager::GetInstance().GetBgmChangType()).c_str());

		sManager.ChangeVolumeSoundMem_bgm(static_cast<int>(VOLUME_BGM_TITLE * 0.75f), bgmtamesi_);
		//ChangeVolumeSoundMem(255 * 75 / 100, bgmtamesi_);
		PlaySoundMem(bgmtamesi_, DX_PLAYTYPE_BACK);
	}

	if (bgmcount_ <= bgmendcount_)
	{
		sManager.ChangeVolumeSoundMem_bgm(VOLUME_BGM_MAIN, bgmplay_);
		//ChangeVolumeSoundMem(255 * 85 / 100, bgmplay_);
		bgmendcount_ = -1;
		bgmcount_ = 0;
		DeleteSoundMem(bgmtamesi_);
	}
	 

}

void SceneTitle::AppearUpdate()
{
	/*if (bgm_switc_n) {
		return;
	}*/
	
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
	
	if (--frame_ <= 0) {
		NormalUpdate();
		ProcessDraw();
		//currentIndex_b = 0;
		bgmcount_ =0;
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
	

	//if (inputM.IsTrgDown(KEY_INPUT_X))
	//{
	//	//ウィンドウを閉じて以降の処理をスキップ
	//	sManager.PlaySE(SoundManager::SOUND_ID::CURSOR, true);
	//	ExitBgmMenu();
	//	return;
	//}

	if (inputM.GetInstance().IsTrgDown(KEY_INPUT_UP)) {
		currentIndex_b = (currentIndex_b + menuList_b.size() - 1) % menuList_b.size();
		sManager.PlaySE(SoundManager::SOUND_ID::CURSOR, true);
	}
	else if (inputM.GetInstance().IsTrgDown(KEY_INPUT_DOWN)) {
		currentIndex_b = (currentIndex_b + 1) % menuList_b.size();
		sManager.PlaySE(SoundManager::SOUND_ID::CURSOR, true);
	}
	if (inputM.GetInstance().IsTrgDown(KEY_INPUT_Z)) {
		auto selectedName_b = menuList_b[currentIndex_b];
		menuFuncTable_b[selectedName_b](inputM);
		
		bgmCountUpdat();
		
		return;
	}	if (inputM.GetInstance().IsTrgDown(KEY_INPUT_SPACE)) {
		auto selectedName_b = menuList_b[currentIndex_b];
		menuFuncTable_b[selectedName_b](inputM);
		
		bgmCountUpdat();
		
		return;
	}

	if (1000 <= bgmcount_) {
		//ChangeVolumeSoundMem(255 * 85 / 100, bgmplay_);
		sManager.ChangeVolumeSoundMem_bgm(VOLUME_BGM_MAIN, bgmplay_);
		
		DeleteSoundMem(bgmtamesi_);
		bgmcount_ = 0;
	}
	else if (950 <= bgmcount_) {
		//ChangeVolumeSoundMem(255 * 20 / 100, bgmtamesi_);
		sManager.ChangeVolumeSoundMem_bgm(static_cast<int>(VOLUME_BGM_TITLE * 0.2f), bgmtamesi_);

	}
	else if (900 <= bgmcount_) {
		//ChangeVolumeSoundMem(255 * 35 / 100, bgmtamesi_);
		sManager.ChangeVolumeSoundMem_bgm(static_cast<int>(VOLUME_BGM_TITLE * 0.35f), bgmtamesi_);

	}
	else if (850 <= bgmcount_) {
		//ChangeVolumeSoundMem(255 * 45 / 100, bgmtamesi_);
		sManager.ChangeVolumeSoundMem_bgm(static_cast<int>(VOLUME_BGM_TITLE * 0.45f), bgmtamesi_);

	}

	

	if (bgm_switc==false) {

		//ProcessDraw();
		

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
		currentIndex_s = (currentIndex_s + menuList_s.size() - 1) % menuList_s.size();
		sManager.PlaySE(SoundManager::SOUND_ID::CURSOR, true);
	}
	else if (inputM.GetInstance().IsTrgDown(KEY_INPUT_DOWN)) {
		currentIndex_s = (currentIndex_s + 1) % menuList_s.size();
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
	int centerY = wsize.height / 2; // 画面中心Y
	int frameHalfHeight = (wsize.height - margin_size * 2) / 2; // 枠の高さの半分

	// 出現・消滅時の高さ変化率(0.0～1.0)
	float rate = static_cast<float>(frame_) /
		static_cast<float>(appear_interval);

	frameHalfHeight *= rate;

	// 白っぽいセロファン
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(margin_size, centerY - frameHalfHeight,
		wsize.width - margin_size, centerY + frameHalfHeight,
		0xffffff, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	// 白枠
	DrawBoxAA(margin_size, centerY - frameHalfHeight,
		wsize.width - margin_size, centerY + frameHalfHeight,
		0xffffff, false, 3.0f);
}

void SceneTitle::NormalDraw()
{
	const Size& wsize = Application::GetInstance().GetWindowSize();
	constexpr int line_start_y = margin_size + 80;
	constexpr int line_start_x = margin_size + 40;

	// 白っぽいセロファン
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 168);
	DrawBox(line_start_x, line_start_y,
		wsize.width - line_start_x, wsize.height - line_start_y,
		0xffffff, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	// 白枠
	DrawBoxAA(line_start_x, line_start_y,
		wsize.width - line_start_x, wsize.height - line_start_y,
		0xffffff, false, 3.0f);
	DrawStringToHandle(500, 150, "スコアランキング", 0x0000ff, fontHndle_[0], Col.LinePink);
	DrawScorList();
}

void SceneTitle::NormalBgmDraw()
{
	const Size& wsize = Application::GetInstance().GetWindowSize();
	// 白っぽいセロファン
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 168);
	DrawBox(margin_size, margin_size,
		wsize.width - margin_size, wsize.height - margin_size,
		0xffffff, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	// 白枠
	DrawBoxAA(margin_size, margin_size,
		wsize.width - margin_size, wsize.height - margin_size,
		0xffffff, false, 3.0f);
	DrawStringToHandle(margin_size + 10, margin_size + 10, "現在の曲", 0x0000ff, fontHndle_[0],Col.Pink);
	DrawStringToHandle(margin_size + 10, margin_size + 50, bgm_.GetBgmName().c_str(), f_col, fontHndle_[1], f_col2);
	/*DrawFormatStringToHandle(margin_size + 10, margin_size + 80
		, Col.White, fontHndle_[0], "現在のtime %d"
		, bgmcount_ );*/
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
	//constexpr int KEY_TEXT_ROW = 6;
	constexpr int KEY_TEXT_ROW = 3;

	const SoundManager& sManager = SoundManager::GetInstance();

	//上から順に描画
	int drawY = Application::SCREEN_SIZE_Y - SPACE - (CONTROL_TEXT_SIZE * KEY_TEXT_ROW);

	DrawStringToHandle(SPACE, drawY, "矢印キー     : 選択", 0xffffff, miniFont_);
	//行を一つ下に
	drawY += CONTROL_TEXT_SIZE;

	DrawStringToHandle(SPACE, drawY, "Zキー        : 決定", 0xffffff, miniFont_);
	drawY += CONTROL_TEXT_SIZE;

	//DrawStringToHandle(SPACE, drawY, "Xキー        : キャンセル", 0xffffff, miniFont_);
	//drawY += CONTROL_TEXT_SIZE;

	DrawFormatStringToHandle(SPACE, drawY, 0xffffff, miniFont_, "Cキー        : BGM切り替え %s",
		sManager.IsActiveBGM() ? "(現在の状態 : on)" : "(現在の状態 : off)");
	drawY += CONTROL_TEXT_SIZE;

	//DrawStringToHandle(SPACE, drawY, "Rキー        : チュートリアル切り替え", 0xffffff, miniFont_);
	//drawY += CONTROL_TEXT_SIZE;

	//DrawStringToHandle(SPACE, drawY, "スペースキー: ゲームスタート", 0xffffff, miniFont_);
	//drawY += CONTROL_TEXT_SIZE;
}
