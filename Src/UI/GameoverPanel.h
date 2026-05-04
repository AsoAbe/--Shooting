#pragma once
#include "PanelBase.h"

struct ScoreData;

class GameoverPanel :public PanelBase
{
public :
	GameoverPanel(SceneGame& sceneGame, const ScoreData& score);
	~GameoverPanel()override;

	void Update()override;
	void Draw()override;

protected:
	ScoreData score_;

	int counter_;

	void Release();
};

