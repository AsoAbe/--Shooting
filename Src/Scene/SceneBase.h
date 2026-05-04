#pragma once

#include <DxLib.h>

class SceneBase
{
public:
	//メンバー関数
	//-----

	//コンストラクタ
	SceneBase(void);
	//デストラクタ
	virtual ~SceneBase(void);

	//初期化
	virtual bool Init(void);
	//更新
	virtual void Update(void);
	//描画
	virtual void Draw(void);
	//解放
	virtual bool Release(void);

protected:
	//メンバー変数
	//-----


};