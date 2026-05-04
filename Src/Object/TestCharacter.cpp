#include "TestCharacter.h"
#include "../Common/AsoUtility.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
TestCharacter::TestCharacter(SceneGame* parent, const CharacterModelData& modelData):CharacterBase(modelData)
{
	sceneGame_ = parent;
}

TestCharacter::~TestCharacter(void)
{
}


void TestCharacter::Update_Move()
{

}

void TestCharacter::SetParam()
{
	colliderSize_ = DEFAULT_COLLIDERSIZE;
	transform_.quaRotLocal = Quaternion::AngleAxis(AsoUtility::Deg2RadF(180), AsoUtility::AXIS_Y);
}