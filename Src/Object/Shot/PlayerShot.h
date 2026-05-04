#pragma once
#include "ShotBase.h"

class PlayerShot :
	public ShotBase
{
public:
	PlayerShot(SceneGame* parent, const ObjectModelData& model);
protected:
	void SetParam() override;
	void Move()override;

	void SetPosOnGround()override;
};