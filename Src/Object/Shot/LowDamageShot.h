#pragma once
#include "ShotBase.h"

class LowDamageShot :
	public ShotBase
{
public:
	/// <summary>
	/// ’áˆÐ—Í’e‚ÌˆÚ“®‘¬“x
	/// </summary>
	static constexpr float LOW_DAMAGE_SHOT_SPEED = 16.0f;

	/// <summary>
	/// ’áˆÐ—Í’e‚Ì“–‚½‚è”»’è”¼Œa
	/// </summary>
	static constexpr float LOW_DAMAGE_SHOT_COL_RADIUS = 80.0f;
	LowDamageShot(SceneGame* parent, const ObjectModelData& model);
protected:
	void SetParam() override;
	void Move()override;
};