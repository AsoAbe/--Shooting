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
	speed_ = 16;
	colRadius_ = 80;
	shotTimer_ = DEFAULT_SHOT_TIME;
	//damage_ = 6;
}

void LowDamageShot::Move()
{
	ShotBase::Move();
}
