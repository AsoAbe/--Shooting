#pragma once
#include<string>
#include <DxLib.h>
#include"../Common/Quaternion.h"

class SceneManager;
class SceneGame;
class ObjectModelData;

class ObjectBase
{
public:
	ObjectBase(const ObjectModelData& modelData);
	virtual ~ObjectBase(void);
	virtual void Init(void);
	virtual void Update(void);
	virtual void Draw(void);
	virtual void Release(void);

	bool IsActive()const;
	void SetActive(bool b);

	VECTOR GetPos()const;
	void  SetPos(VECTOR pos);

protected:
	SceneGame* sceneGame_;

	//ベースモデル
	const ObjectModelData& baseModel_;
	//生成したモデルid
	int modelId_;

	SceneManager* sceneManager_;

	bool isActive_;

	//座標回転スケール
	VECTOR pos_;
	VECTOR rot_;
	VECTOR scale_;

	//ローカル座標
	VECTOR localPos_;

	//飛ぶ方向
	VECTOR vec_;

	//Quaternion回転制御
	Quaternion quaRot_;
	Quaternion quaLocalRot_;

	// パラメータ設定(純粋仮想関数。派生クラスで必ず初期設定を実装して貰うように)(複数回呼ばれる可能性あり)
	virtual void SetParam(void) = 0;
	//マトリクスでモデルを制御
	void SetMatrixModel(void);
	

};

