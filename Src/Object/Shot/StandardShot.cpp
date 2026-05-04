#include "../../Common/AsoUtility.h"
#include "StandardShot.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
StandardShot::StandardShot(SceneGame* parent, const ObjectModelData& model) : ShotBase(parent, model)
{
}

void StandardShot::SetParam()
{
	isPlayerTag_ = false;
	speed_ = 22;
	colRadius_ = 80;
	shotTimer_ = DEFAULT_SHOT_TIME;
}

void StandardShot::Move()
{
	ShotBase::Move();
}
