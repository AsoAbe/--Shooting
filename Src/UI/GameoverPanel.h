#pragma once
#include "PanelBase.h"

struct ScoreData;

class GameoverPanel :public PanelBase
{
public :
	// 初期値
	static constexpr int SCORE_DISPLAY_INVALID = 0;

	// パネル表示時間
	static constexpr int GAMEOVER_PANEL_MIN_TIME = PanelBase::PANEL_MIN_TIME * 2;

	GameoverPanel(SceneGame& sceneGame, const ScoreData& score);
	~GameoverPanel()override;

	void Update()override;
	void Draw()override;

protected:
	ScoreData score_;

	int counter_;

	void Release();
};

