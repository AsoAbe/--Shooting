#pragma once
#include "ShotBase.h"

class StandardShot :
	public ShotBase
{
public:
	/// <summary>
	/// ’Êí’e‚ÌˆÚ“®‘¬“x
	/// </summary>
	static constexpr float STANDARD_SHOT_SPEED = 22.0f;

	
	StandardShot(SceneGame* parent, const ObjectModelData& model);
protected:
	void SetParam() override;
	void Move()override;
};