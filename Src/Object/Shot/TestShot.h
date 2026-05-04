#pragma once
#include "ShotBase.h"

class TestShot :
	public ShotBase
{
public:
	TestShot(SceneGame* parent, const ObjectModelData& model);
protected:
	void SetParam() override;
	void Move()override;
};