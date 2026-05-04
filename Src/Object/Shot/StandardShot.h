#pragma once
#include "ShotBase.h"

class StandardShot :
	public ShotBase
{
public:
	StandardShot(SceneGame* parent, const ObjectModelData& model);
protected:
	void SetParam() override;
	void Move()override;
};