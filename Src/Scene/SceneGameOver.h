//#pragma once

#include"SceneBase.h"

class SceneGameOver : public SceneBase
{
public:
	// キー入力の押下状態
	static constexpr int KEY_PRESSED = 1;

	// ゲームオーバー画面の描画余白
	static constexpr int GAME_OVER_DRAW_MARGIN = 100;

	// ゲームオーバー画面の背景色
	static constexpr unsigned int GAME_OVER_COLOR = 0xFFFF00;

	//メンバー関数
	//-----

	//初期化
	bool Init(void);
	//更新
	void Update(void);
	//描画
	void Draw(void);
	//解放
	bool Release(void);
};