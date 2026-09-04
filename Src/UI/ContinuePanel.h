#pragma once
#include "PanelBase.h"
class ContinuePanel:public PanelBase
{
public :
	
	// •`‰æ
	static constexpr int BLEND_ALPHA_RESET = 0;
	static constexpr int LIFE_TEXT_POS_Y = 232;
	static constexpr int CONTINUE_TEXT_POS_Y = 400;

	// •`‰æˆÊ’u
	static constexpr int DEATH_TEXT_POS_Y = 200;
	static constexpr int HALF_DIVISOR = 2;

	ContinuePanel(SceneGame& sceneGame);
	~ContinuePanel()override;

	void Update()override;
	void Draw()override;

	void SetActive(bool b)override;

protected:
	int counter_;

	void Release();
};

