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
	speed_ = PLAYER_SHOT_SPEED;
	colRadius_ = PLAYER_SHOT_COL_RADIUS;
	shotTimer_ = PLAYER_SHOT_TIME;
	damage_ = PLAYER_SHOT_DAMAGE;
}

void PlayerShot::Move()
{
	ShotBase::Move();
}

void PlayerShot::SetPosOnGround()
{
	//地形に沿う
	constexpr float LINE_LENGTH = GROUND_LINE_LENGTH;
	constexpr float POS_Y = GROUND_HEIGHT_OFFSET;
	constexpr float RANGE = GROUND_FOLLOW_RANGE;
	float newPosY = sceneGame_->GetOManager()->GetOnGroundY(GetPos(), LINE_LENGTH, LINE_LENGTH) + POS_Y;
	if (fabs(pos_.y - newPosY) > RANGE)
	{
		//範囲外なので地形に従わない
		return;
	}
	pos_.y = newPosY;
}
