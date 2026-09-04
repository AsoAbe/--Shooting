#pragma once
#include "PanelBase.h"

struct ScoreData;

class GameclearPanel : public PanelBase
{
public:
	
	// パネル表示時間
	static constexpr int GAMECLEAR_PANEL_MIN_TIME = PanelBase::PANEL_MIN_TIME * 2;

	// 描画位置
	static constexpr int DRAW_POS_START = 0;
	static constexpr int SCORE_TIME_ROW_OFFSET = 20;

	// 描画設定
	static constexpr int FULL_ALPHA = 255;
	static constexpr int HALF_DIVISOR = 2;

	// 描画色
	static constexpr int DRAW_COLOR_BLACK = 0x000000;
	static constexpr int DRAW_COLOR_WHITE = 0xFFFFFF;
	static constexpr int DRAW_COLOR_YELLOW = 0xFFFF00;

	GameclearPanel(SceneGame& sceneGame, const ScoreData& score);
	~GameclearPanel()override;

	void Update()override;
	void Draw()override;

	void SetActive(bool b)override;

protected:
	int counter_;
	//表示するスコア
	ScoreData score_;

	void Release();
};
