#pragma once
#include"SceneBase.h"
#include<string>
#include<vector>
#include<unordered_map>
#include <functional>
#include<memory>
#include<list>
#include"../BGM.h"

enum class configuTpye
{
	c_tyr,
	c_strt,
	c_stumei,

};


class InputManager;
class BGM;

struct ScoreData;

class SceneTitle : public SceneBase
{
public:
	// 初期値
	static constexpr int INITIAL_COUNT = 0;
	static constexpr int INVALID_HANDLE = -1;
	static constexpr int INVALID_COUNT = -1;

	// タイトルモデル
	static constexpr float TITLE_MODEL_SCALE = 0.3f;
	static constexpr int TITLE_MODEL_ROT_X = -90;
	static constexpr int TITLE_MODEL_ROT_Y = 0;
	static constexpr int TITLE_MODEL_ROT_Z = 0;
	static constexpr int TITLE_MODEL_POS_X = 0;
	static constexpr int TITLE_MODEL_POS_Y = 12;
	static constexpr int TITLE_MODEL_POS_Z = 0;


	// 夜空画像
	static constexpr int YOZORA_DISPLAY_TIME = 300;
	static constexpr int YOZORA_FADE_TIME = 60;
	static constexpr int NEXT_IMAGE_INDEX_OFFSET = 1;

	// タイトルモデル回転
	static constexpr float TITLE_MODEL_ROT_SPEED = 90.0f;
	static constexpr float FULL_ROTATION_DEG = 360.0f;

	// 描画
	static constexpr int TITLE_IMAGE_POS_X = 230;
	static constexpr int TITLE_IMAGE_POS_Y = 8;
	static constexpr int TITLE_DRAW_COLOR = 0x110033;
	static constexpr int MODEL_SPHERE_RADIUS = 32;
	static constexpr int MODEL_SPHERE_SEGMENTS = 8;
	static constexpr int MODEL_SPHERE_COLOR = 0xFF0000;
	static constexpr int MODEL_SPHERE_OUTLINE_COLOR = 0xFFFFFF;

	// フォント
	static constexpr int TITLE_FONT_SIZE = 35;
	static constexpr int TITLE_FONT_EDGE_SIZE = 5;
	static constexpr int SUBTITLE_FONT_SIZE = 25;
	static constexpr int SUBTITLE_FONT_EDGE_SIZE = 3;
	static constexpr int MINI_FONT_EDGE_SIZE = 5;
	static constexpr int FONT_SIZE_RESET = 0;

	// メニュー選択インデックス
	static constexpr int MENU_INDEX_FIRST = 0;
	static constexpr int MENU_INDEX_PREVIOUS_OFFSET = 1;
	static constexpr int MENU_INDEX_NEXT_OFFSET = 1;

	// メニュー
	static constexpr int MENU_LINE_START_Y_OFFSET = 450;
	static constexpr int MENU_LINE_START_X_OFFSET = 550;
	static constexpr int MENU_LINE_WIDTH = 388;
	static constexpr int MENU_TUTORIAL_X_OFFSET = 398;
	static constexpr int MENU_LINE_HEIGHT = 40;
	static constexpr int MENU_ARROW_X_OFFSET = 30;
	static constexpr int MENU_SELECTED_X_OFFSET = 10;

	// BGMメニュー
	static constexpr int BGM_LINE_START_Y_OFFSET = 350;
	static constexpr int BGM_LINE_START_X_OFFSET = 460;
	static constexpr int BGM_SELECTED_X_OFFSET = 40;
	static constexpr int BGM_SELECTED_MOVE_X = 20;

	// スコアメニュー
	static constexpr int SCORE_LINE_START_Y_OFFSET = 250;
	static constexpr int SCORE_LIST_START_Y_OFFSET = 550;
	static constexpr int SCORE_LINE_START_X_OFFSET = 460;

	// メニュー演出
	static constexpr int APPEAR_FRAME_COUNT = 20;
	static constexpr int BGM_END_FRAME_COUNT = 20;
	static constexpr int BGM_FADE_START_FRAME = 850;
	static constexpr int BGM_FADE_MIDDLE_FRAME = 900;
	static constexpr int BGM_FADE_NEAR_END_FRAME = 950;
	static constexpr int BGM_FADE_END_FRAME = 1000;

	// メニュー描画
	static constexpr int MENU_TEXT_SHADOW_OFFSET = 1;

	// フェード
	static constexpr int BLEND_ALPHA_PROCESS = 128;
	static constexpr int BLEND_ALPHA_MENU = 168;
	static constexpr int FULL_ALPHA = 255;
	static constexpr float FADE_RATE_MAX = 1.0f;

	// キーボード
	static constexpr int KEY_STATE_SIZE = 256;

	// 描画色
	static constexpr unsigned int MENU_DEFAULT_COLOR = 0x4444FF;
	static constexpr unsigned int MENU_SELECTED_COLOR = 0xFF00FF;
	static constexpr unsigned int MENU_ARROW_COLOR = 0xFF0000;
	static constexpr unsigned int DRAW_COLOR_WHITE = 0xFFFFFF;
	static constexpr unsigned int DRAW_COLOR_BLACK = 0x000000;
	static constexpr unsigned int DRAW_COLOR_BLUE = 0x0000FF;

	// 枠
	static constexpr float FRAME_LINE_WIDTH = 3.0f;
	// 枠の上下余白数
	static constexpr int FRAME_MARGIN_COUNT = 2;

	// 操作説明
	static constexpr int CONTROL_TEXT_POS_X = 8;
	static constexpr int CONTROL_TEXT_START_OFFSET = 8;
	static constexpr int CONTROL_TEXT_ROW_COUNT = 3;
	static constexpr int CONTROL_TEXT_SIZE = 18;
	
	// 操作説明位置
	static constexpr int CONTROL_TEXT_Z_OFFSET = 10;
	static constexpr int CONTROL_TEXT_CURRENT_BGM_OFFSET = 50;

	// ハイスコア表示
	static constexpr int HIGH_SCORE_POS_X = 8;
	static constexpr int HIGH_SCORE_POS_Y = 8;

	// スコアランキングタイトル
	static constexpr int SCORE_TITLE_POS_X = 500;
	static constexpr int SCORE_TITLE_POS_Y = 150;

	// スコア画面
	static constexpr int SCORE_FRAME_POS_Y_OFFSET = 80;
	static constexpr int SCORE_FRAME_POS_X_OFFSET = 40;

	// BGM音量
	static constexpr float BGM_PREVIEW_VOLUME_RATE = 0.35f;
	static constexpr float BGM_TITLE_VOLUME_RATE = 0.75f;
	static constexpr float BGM_FADE_VOLUME_RATE = 0.2f;
	static constexpr float BGM_FADE_MIDDLE_VOLUME_RATE = 0.35f;
	static constexpr float BGM_FADE_NEAR_END_VOLUME_RATE = 0.45f;

	// 音程
	static constexpr float TITLE_BGM_PITCH_OFFSET = -1200.0f; // タイトルBGM読み込み時の音程補正
	static constexpr float DEFAULT_SOUND_PITCH_OFFSET = 0.0f; // 音程補正を元に戻す値

	// BGM表示
	static constexpr int BGM_NAME_POS_X_OFFSET = 10;
	static constexpr int BGM_NAME_POS_Y_OFFSET = 10;
	static constexpr int BGM_NAME_POS_Y_OFFSET_2 = 50;

	static constexpr int FONT_MAX = 3;//フォント最大数
	static constexpr int BGM_NAME_MAX = 5;//BGM最大数
	static constexpr int YOZORA_MAX = 3;//夜空最大数

	
	//コンストラクタ
	SceneTitle(void);
	//デストラクタ
	~SceneTitle(void);

	//初期化
	bool Init(void) override;
	//更新
	void Update(void) override;
	//描画
	void Draw(void) override;
	//解放
	bool Release(void) override;

	void ResetTable();
private:
	struct ConfiguState {
		configuTpye type;
		uint32_t code; // 入力コード(凡用)
	};
	bool IsNow(const std::string& eventstr)const;

	using InputData_t = std::unordered_map < std::string, bool>;
	InputData_t currentInput_; //そのイベントに対応するボタンが押されてる状態か
	InputData_t lastInput_;    //そのイベントに対応するボタンが押されてる状態か(直前)

	BGM bgm_;

	//メンバー変数
	int titleImage;//タイトル画像格納
	int yozoraImage_[YOZORA_MAX];//夜空画像格納
	int currentIndex_ = 0;

	int frame_ = 0;
	int currentIndex_b = 0;
	int currentIndex_s = 0;

	using InputTable_t = std::unordered_map<std::string, std::vector<ConfiguState>>;
	InputTable_t inputTable_; //イベントと実際の入力の対応表

	std::vector<std::string> keystr_;
	bool tyrNow_;
	void UpdateKey();

	using UpdateFunc_t = void (SceneTitle::*)();
	UpdateFunc_t update_;

	using DrawFunc_t = void (SceneTitle::*)();
	DrawFunc_t draw_;

	// メニューのリストを表示する
	void DrawMenuList();
	std::list<std::shared_ptr<SceneTitle>> scenes_;

	// メニューの文字列と関数のテーブル
	using MenuFunc_t = std::function<void(InputManager&)>;
	std::vector<std::string>menuList_;
	std::map<std::string, MenuFunc_t> menuFuncTable_;
	std::string nomal_switc = {};
	std::string GetMod();

	void NormalUpdate();   //枠およびメニューを表示する関数

	// BGMメニューの文字列と関数のテーブル
	using MenuFunc_t = std::function<void(InputManager&)>;
	std::vector<std::string>menuList_b ={};
	std::map<std::string, MenuFunc_t> menuFuncTable_b;
	int bgmplay_;
	int bgmcount_;
	int bgmendcount_ = -1;
	int bgmtamesi_;
	void bgmCountUpdat();

	// スコアメニューの文字列と関数のテーブル
	using MenuFunc_t = std::function<void(InputManager&)>;
	std::vector<std::string>menuList_s = {};
	std::map<std::string, MenuFunc_t> menuFuncTable_s;

	void ExitBgmMenu();	//bgmメニューを閉じる
	void ExitScorMenu();	//scorメニューを閉じる

	//更新関数
	void AppearUpdate();   //枠を広げて表示する関数
	void NormalBgmUpdate();   //枠およびメニューを表示する関数
	void NormalScorUpdate();   //枠およびメニューを表示する関数
	void DisappearUpdate();//枠を縮小して消す関数
	void UpdateYozora();
	int GetbgmCount();


	//描画
	void ProcessDraw();//枠の出現・消滅の演出描画
	void NormalDraw(); //通常のポーズメニュー描画
	void NormalBgmDraw(); //通常のポーズメニュー描画
	void DrawBgmList();
	void DrawScorList();
	void YozoraDraw();//夜空

	//描画変数(夜空)
	int countimg_;
	int countimg_end;
	int yc = 0;
	int nextYc = 1;                      // 次の画像インデックス

	//描画変数フェード(夜空)
	int fadeTime = 60;                   // フェードにかけるフレーム数（1秒なら60）
	int displayTime = 240;               // 各画像の表示時間（フレーム数）
	bool isFading = false;
	int fadeCounter = 0;

	bool bgm_switc;
	bool bgm_switc_n;

	//スコア用スイッチ
	bool scor_switc;

	//フォント
	int fontHndle_[FONT_MAX];
	int f_size;
	int f_col = 0;
	int f_col2 = 0;
	
	int miniFont_;

	//モデルハンドル
	int titleModel_;
	float titleModelRotY_;

	/// <summary>
	/// 選択したゲームシーンにシーン遷移する
	/// </summary>
	void StartGameScene();


	// 操作説明を表示する
	void DrawControlKey();
};