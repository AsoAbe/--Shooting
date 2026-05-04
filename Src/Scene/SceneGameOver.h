//#pragma once

#include"SceneBase.h"

class SceneGameOver : public SceneBase
{
public:
	//メンバー関数
	//-----

	//コンストラクタ
	//SceneGameOver(void);
	//デストラクタ
	//~SceneGameOver(void);

	//初期化
	bool Init(void);
	//更新
	void Update(void);
	//描画
	void Draw(void);
	//解放
	bool Release(void);
};