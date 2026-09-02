#pragma once
#include "ShotBase.h"

class TestShot :
	public ShotBase
{
public:
	/// <summary>
	/// テスト弾の移動速度
	/// </summary>
	static constexpr float TEST_SHOT_SPEED = 12.0f;
	TestShot(SceneGame* parent, const ObjectModelData& model);
protected:
	void SetParam() override;
	void Move()override;
};