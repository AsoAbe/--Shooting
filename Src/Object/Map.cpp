#include <DxLib.h>
#include "../Application.h"
#include "../Manager/ObjectManager.h"
#include "../Common/Transform.h"
#include "Player.h"
#include "Map.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
Map::Map(ObjectManager* objectManager)
{
	objectManager_ = objectManager;
}

Map::~Map()
{
}

bool Map::Init()
{
	//ここまでステージの描画、当たり判定
	//mapBaseModel[1] = MV1LoadModel((Application::PATH_MODEL + "cube.mv1").c_str());
	transform_.SetModel(
		MV1LoadModel((Application::PATH_STAGE + "Stage.mv1").c_str()));

	transform_.scl = AsoUtility::VECTOR_ONE;
	transform_.quaRot = Quaternion();
	transform_.pos = { 0.0f, -100.0f, 0.0f };

	// 当たり判定(コライダ)作成
	transform_.MakeCollider(Collider::TYPE::STAGE);

	transform_.Update();

	auto* player = objectManager_->GetPlayer();
	player->AddCollider(transform_.collider);

	StoneWall();
	WhiteWall();

	return true;
}

void Map::DrawMap(void)
{

	MV1DrawModel(transform_.modelId);
	MV1DrawModel(walltransform_.modelId);
	//MV1DrawModel(whitewall_.modelId);
			
}

bool Map::Release(void)
{
	MV1DeleteModel(transform_.modelId);
	MV1DeleteModel(walltransform_.modelId);
	//MV1DeleteModel(whitewall_.modelId);
		
	return true;
}

int Map::GetModelId() const
{
	return transform_.modelId;
}

//石壁
void Map::StoneWall(void)
{
	walltransform_.SetModel(
		MV1LoadModel((Application::PATH_STAGE + "Wall2.mv1").c_str()));

	walltransform_.scl = AsoUtility::VECTOR_ONE;
	walltransform_.quaRot = Quaternion();
	walltransform_.pos = { 0.0f, -100.0f, 0.0f };

	// 当たり判定(コライダ)作成
	//walltransform_.MakeCollider(Collider::TYPE::STAGE);

	walltransform_.Update();

	auto* player = objectManager_->GetPlayer();
	//player->AddCollider(walltransform_.collider);

}

//透明な壁
void Map::WhiteWall(void)
{

	whitewall_.SetModel(
		MV1LoadModel((Application::PATH_STAGE + "Wall4.mv1").c_str()));

	whitewall_.scl = AsoUtility::VECTOR_ONE;
	whitewall_.quaRot = Quaternion();
	whitewall_.pos = { 0.0f, -100.0f, 0.0f };

	// 当たり判定(コライダ)作成
	whitewall_.MakeCollider(Collider::TYPE::WALL);

	whitewall_.Update();

	auto* player = objectManager_->GetPlayer();
	player->AddCollider(whitewall_.collider);

}
