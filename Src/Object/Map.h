#pragma once
#include<string>
#include"../Common/Vector2.h"
#include"../Common/AsoUtility.h"
#include"../Common/Transform.h"

class ObjectManager;

class Map
{
public:

	// 無効なモデルハンドル
	static constexpr int INVALID_MODEL_ID = -1;

	// ステージのY座標
	static constexpr float STAGE_POS_Y = -100.0f;
	
	//基本的な地面の高さ
	static constexpr float BASE_GROUND_POS_Y = 0.0f;

	//コンストラクタ
	Map(ObjectManager* parent);
	~Map();

	bool Init();
	//描画
	void DrawMap(void);
	//解放
	bool Release(void);

	/// <summary>
	/// 地形のモデルを取得する。
	/// </summary>
	/// <returns></returns>
	int GetModelId()const;
private:

	ObjectManager* objectManager_;

	Transform transform_;
	Transform walltransform_;
	Transform whitewall_;
	void StoneWall(void);
	void WhiteWall(void);
};