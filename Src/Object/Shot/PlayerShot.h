#pragma once
#include "ShotBase.h"

class PlayerShot :
	public ShotBase
{
public:
	/// <summary>
	/// プレイヤー弾の移動速度
	/// </summary>
	static constexpr float PLAYER_SHOT_SPEED = 40.0f;

	/// <summary>
	/// プレイヤー弾の当たり判定半径
	/// </summary>
	static constexpr float PLAYER_SHOT_COL_RADIUS = 80.0f;

	/// <summary>
	/// プレイヤー弾の生存時間
	/// </summary>
	static constexpr float PLAYER_SHOT_TIME = 1.6f;

	/// <summary>
	/// プレイヤー弾の威力
	/// </summary>
	static constexpr int PLAYER_SHOT_DAMAGE = 1;

	/// <summary>
	/// 地形に沿わせるか判定する範囲
	/// </summary>
	static constexpr float GROUND_FOLLOW_RANGE = 100.0f;
	PlayerShot(SceneGame* parent, const ObjectModelData& model);
protected:
	void SetParam() override;
	void Move()override;

	void SetPosOnGround()override;
};