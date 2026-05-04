#pragma once
#include "ObjectBase.h"

class ObjectModelData;
class TestObject :
	public ObjectBase
{
public:
	TestObject(SceneGame* parent, const ObjectModelData& model);
	~TestObject(void) override;

private:

	void SetParam() override;
};

