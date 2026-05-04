#pragma once
#include "CharacterBase.h"

class StandardBoss :
	public CharacterBase
{
public:
	enum class ATTACK_STATE
	{
		NONE,
		POWER_CHARGING,		//突進予備動作
		DASH,			//突進
		RETURN,			//初期位置に戻る
		RANDOM,			//周囲ランダム
		RANDOM_MOVING,	//動きながら全方向
		AROUND,			//時計回りにずらしながら全方向
		AROUND_MOVING	//動きながら全方向
	};
	enum class MOVE_STATE
	{
		NONE,
		L,	//左方向へ移動
		R	//右方向へ移動
	};


	StandardBoss(SceneGame* parent, const CharacterModelData& modelData);
	virtual ~StandardBoss(void) override;

	/// <summary>
	/// ステートを切り替えて関係する変数をリセットする
	/// </summary>
	/// <param name="state"></param>
	void ChangeState(ATTACK_STATE state);

	virtual void Update()override;

	virtual void Died() override;

	VECTOR GetLocalDiff(void);

	void virtual ResetStatus() override;

	bool virtual Damage(int damage)override;
protected:
	using FuncAttackUpdate_t = void(StandardBoss::*)(void);

	ATTACK_STATE state_;
	//次の攻撃
	ATTACK_STATE nextState_;

	MOVE_STATE moveState_;

	/// <summary>
	/// 攻撃処理の関数ポインタ
	/// </summary>
	FuncAttackUpdate_t attackUpdate_;

	/// <summary>
	/// 突進ベクトル
	/// </summary>
	VECTOR dashVec_;

	/// <summary>
	/// 行動カウント
	/// </summary>
	int stateChangeCount_;
	/// <summary>
	/// 突進カウントが達したら次の行動へ
	/// </summary>
	int dashCountEnd_;
	/// <summary>
	/// 突進カウント
	/// </summary>
	int dashCount_;
	/// <summary>
	/// 突進時の方向転換のフラグ
	/// </summary>
	bool dashChangeVecFlag_;
	/// <summary>
	/// 直前の行動を終えた座標を記録
	/// </summary>
	VECTOR lastPos_;
	/// <summary>
	/// 突進後に向かう座標
	/// </summary>
	VECTOR returnPos_;

	/// <summary>
	/// 最後に切り替わった時のカウント数
	/// </summary>
	int lastCount_;
	/// <summary>
	/// 累計フレーム数
	/// </summary>
	int counter_;

	/// <summary>
	/// 自動切り替えまでの時間(秒)
	/// </summary>
	float changeTime_;

	/// <summary>
	/// チャージエフェクトのハンドル。エフェクト終了後は-1に
	/// </summary>
	int chargeEffect_;

	//死亡判定
	bool died_;

	/// <summary>
	/// HP変化で使う技を必ず最初に使うためのフラグ。使用後にfalse
	/// </summary>
	bool lowHpAttackFlag_;

	virtual void SetParam() override;

	virtual void Update_Move() override;
	virtual void Update_Shot() override;

	void AttackUpdate_Shot();
	virtual void AttackUpdate_PowerCharging();
	virtual void AttackUpdate_Dash();
	virtual void AttackUpdate_Return();
	
	/// <summary>
	/// チャージエフェクトと効果音再生
	/// </summary>
	/// <returns>エフェクトハンドル</returns>
	int CreateChargeEffect();

	/// <summary>
	/// ダッシュ中の振り向き処理
	/// </summary>
	/// <param name="diffXZ">プレイヤーとの座標の差からYを除いたもの</param>
	virtual void DashTurn(const VECTOR& diffXZ);

	virtual void ChangeState_PowerCharging(const VECTOR& diff);
	virtual void ChangeState_Return();

	//基準fpsを基準としたカウントを返す
	int GetShotCount(int count);

	//ステート切り替え
	void NextState();

	bool IsLowHp()const;

	/// <summary>
	/// 現在が移動撃ち中ならtrueを返す
	/// </summary>
	bool IsMovingShot();

	/// <summary>
	/// 周囲に弾をばら撒く
	/// </summary>
	/// <param name="type">弾の種類</param>
	/// <param name="pos">中心座標</param>
	/// <param name="loop">ループ数</param>
	/// <param name="addRot">追加する回転</param>
	void ShotAround(ObjectManager::ShotType type, VECTOR pos, int loop,float addRot = 0);

	virtual void Mv1Scale() override;
	virtual bool IsActiveCollision() const override;
};

