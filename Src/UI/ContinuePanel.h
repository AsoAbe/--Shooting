#pragma once
#include "PanelBase.h"
class ContinuePanel:public PanelBase
{
public :
	ContinuePanel(SceneGame& sceneGame);
	~ContinuePanel()override;

	void Update()override;
	void Draw()override;

	void SetActive(bool b)override;

protected:
	int counter_;

	void Release();
};

