#pragma once
#include "ShotBase.h"

class LowDamageShot :
	public ShotBase
{
public:
	LowDamageShot(SceneGame* parent, const ObjectModelData& model);
protected:
	void SetParam() override;
	void Move()override;
};