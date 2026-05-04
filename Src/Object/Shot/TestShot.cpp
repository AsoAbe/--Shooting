#include "../../Common/AsoUtility.h"
#include "TestShot.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
TestShot::TestShot(SceneGame* parent, const ObjectModelData& model) : ShotBase(parent, model)
{
}

void TestShot::SetParam()
{
	isPlayerTag_ = false;
	speed_ = 12;
	colRadius_ = 80;
	shotTimer_ = DEFAULT_SHOT_TIME;
}

void TestShot::Move()
{
	//íºêi
	pos_ = VAdd(pos_, VScale(quaRot_.GetForward(), speed_));
}
