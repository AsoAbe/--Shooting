#pragma once

#include<string>
#include"../Common/Vector2.h"
#include"../Common/AsoUtility.h"
#include"../Common/Transform.h"

class ObjectManager;

class Map
{
public:
	static constexpr float BASE_GROUND_POS_Y = 0;//基本的な地面の高さ


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