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

	static constexpr int FONT_MAX = 3;//フォント最大数
	static constexpr int BGM_NAME_MAX = 5;//BGM最大数
	static constexpr int YOZORA_MAX = 3;//夜空最大数

	

	//メンバー関数
	//-----

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
	
	//std::string bgmName_ = {};

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