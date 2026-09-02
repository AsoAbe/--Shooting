#pragma once
#include "StandardBoss.h"
#include "CharacterBase.h"
class EnemyGolem :
    public StandardBoss
{
public:
	// 無効なモデルハンドル
	static constexpr int INVALID_MODEL_ID = -1;

	// デバッグ表示する球体の半径
	static constexpr float DEBUG_SPHERE_RADIUS = 32.0f;

	// デバッグ表示する球体の分割数
	static constexpr int DEBUG_SPHERE_SEGMENTS = 8;

	// デバッグ表示する球体の内側の色
	static constexpr unsigned int DEBUG_SPHERE_INNER_COLOR = 0xFF0000;

	// デバッグ表示する球体の外側の色
	static constexpr unsigned int DEBUG_SPHERE_OUTER_COLOR = 0xFFFFFF;

	EnemyGolem(SceneGame* parent, const CharacterModelData& modelData);
	virtual ~EnemyGolem(void) override;

	/// <summary>
	/// ステートを切り替えて関係する変数をリセットする
	/// </summary>
	/// <param name="state"></param>
	void Draw() override;
	void Update_Move() override;
	void AttackUpdate_Dash() override;
	void AttackUpdate_Return() override;
	void AttackUpdate_PowerCharging() override;

	void DashTurn(const VECTOR& diffXZ) override;

	void ChangeState_PowerCharging(const VECTOR& diff) override;

	void Died() override;

};

