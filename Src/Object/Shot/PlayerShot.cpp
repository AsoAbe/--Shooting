#include "../../Common/AsoUtility.h"
#include "../../Scene/SceneGame.h"
#include "../../Manager/ObjectManager.h"
#include "PlayerShot.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
PlayerShot::PlayerShot(SceneGame* parent, const ObjectModelData& model) : ShotBase(parent, model)
{
}

void PlayerShot::SetParam()
{
	isPlayerTag_ = true;
	speed_ = 40;
	colRadius_ = 80;
	shotTimer_ = 1.6f;
	damage_ = 1;
}

void PlayerShot::Move()
{
	ShotBase::Move();
}

void PlayerShot::SetPosOnGround()
{
	//地形に沿う
	constexpr float LINE_LENGTH = 500;
	constexpr float POS_Y = 100;
	constexpr float RANGE = 100;
	float newPosY = sceneGame_->GetOManager()->GetOnGroundY(GetPos(), LINE_LENGTH, LINE_LENGTH) + POS_Y;
	if (fabs(pos_.y - newPosY) > RANGE)
	{
		//範囲外なので地形に従わない
		return;
	}
	pos_.y = newPosY;
}
