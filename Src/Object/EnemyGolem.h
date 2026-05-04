#pragma once
#include "StandardBoss.h"
#include "CharacterBase.h"
class EnemyGolem :
    public StandardBoss
{
public:
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
	//void Mv1Scale() override;

};

