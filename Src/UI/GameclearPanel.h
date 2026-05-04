#pragma once
#include "PanelBase.h"

struct ScoreData;

class GameclearPanel : public PanelBase
{
public:
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


