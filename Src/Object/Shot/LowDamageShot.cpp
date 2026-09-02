#include "../../Common/AsoUtility.h"
#include "LowDamageShot.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
LowDamageShot::LowDamageShot(SceneGame* parent, const ObjectModelData& model) : ShotBase(parent, model)
{
}

void LowDamageShot::SetParam()
{
	isPlayerTag_ = false;
	speed_ = LOW_DAMAGE_SHOT_SPEED;
	colRadius_ = LOW_DAMAGE_SHOT_COL_RADIUS;
	shotTimer_ = DEFAULT_SHOT_TIME;
}

void LowDamageShot::Move()
{
	ShotBase::Move();
}
