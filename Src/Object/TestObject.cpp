#include "TestObject.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
TestObject::TestObject(SceneGame* parent, const ObjectModelData& model):ObjectBase(model)
{
	sceneGame_ = parent;
}

TestObject::~TestObject(void)
{
}

void TestObject::SetParam()
{
}
