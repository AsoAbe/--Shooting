#pragma once
#include <vector>
#include<string>
#include<memory>
#include <chrono>
#include <DxLib.h>
#include"../Common/Transform.h"

class SceneManager;
class SceneGame;
class CharacterModelData;
class AnimationController;
class Collider;
class Capsule;
class TimeOut;

class CharacterBase
{
public:
	enum class ANIM
	{
		IDLE,
		WALK,
		RUN,
		JUMP,
		FALLING,
		ONGROUND,
		ATTACK1,
		ATTACK2,
		ATTACK3,
		DOWN,
		MAX			//要素の数
	};

	static constexpr float GRAVITY = -9.8f / 60.0f*2.5f;
	//滑空時の落下加速度。数値分重力に掛け算する
	static constexpr float GLIDER_FALLSPD = 0.025f;
	//滑空時の最大落下速度。数値分重力に掛け算する
	static constexpr float GLIDER_MAX_FALLSPD = 0.05f;
	//滑空を維持できる最低高度
	static constexpr float GLIDER_MIN_Y = 200.0f;

	//落下モーションに移るまでの時間(フレーム数)
	static constexpr int NOT_GROUNDED_TIME = 45;

	// 移動速度
	static constexpr float SPEED_WALK = 8.0f;
	//回転にかかる時間
	static constexpr float TIME_ROT = 1.0f;
	//回転量(deg)
	static constexpr float SPEED_ROT = 8.0f;
	//回転量(rad)
	static constexpr float SPEED_ROT_RAD = SPEED_ROT * (DX_PI_F / 180.0f);

	static constexpr float DEFAULT_COLLIDERSIZE=32;

	static constexpr float DEFAULT_COLLIDER_POS_Y = 100;

	CharacterBase(const CharacterModelData& modelData);
	virtual ~CharacterBase(void);
	virtual void Init(void);
	virtual void Update(void);
	virtual void Draw(void);
	virtual void Release(void);

	//座標を返す
	VECTOR GetPos()const;
	//回転を返す
	VECTOR GetRot()const;

	//弾に対する当たり判定の中心
	VECTOR GetColPos()const;

	//座標を設定
	void SetPos(VECTOR pos);
	//回転を設定
	void SetRot(VECTOR rot);

	//弾に対する当たり判定の半径
	float GetColRadiusShot()const;

	//当たり判定の半径
	float GetColSize()const;
	/// <summary>
	/// ANIMをAnimationControllerで使う整数値にする
	/// </summary>
	static int GetAnimType(ANIM anim)
	{
		return static_cast<int>(anim);
	}

	/// <summary>
	/// 現在値
	/// </summary>
	int GetHp()const;
	/// <summary>
	/// 最大値
	/// </summary>
	int GetHpMax()const;
	//スタミナ
	int GetStamina()const;
	int GetStaminaMax()const;

	/// <summary>
	/// ダメージを受けて死亡判定
	/// </summary>
	/// <param name="damage"></param>
	/// <returns></returns>
	virtual bool Damage(int damage);

	/// <summary>
	/// 死亡時の処理
	/// </summary>
	virtual void Died();

	/// <summary>
	/// アクティブ状態
	/// </summary>
	/// <returns></returns>
	bool IsActive()const;
	/// <summary>
	/// アクティブ状態をセットする
	/// </summary>
	virtual void SetActive(bool b);
	// 衝突判定に用いられるコライダ制御
	void AddCollider(std::weak_ptr<Collider> collider);
	void ClearCollider(void);

	// 衝突用カプセルの取得
	const Capsule& GetCapsule(void) const;

	/// <summary>
	/// HPやその他のカウントなどをリセットする
	/// </summary>
	virtual void ResetStatus();

	virtual bool IsActiveCollision() const;
protected:

	//ゲームシーン
	SceneGame* sceneGame_;

	// モデルの参照
	const CharacterModelData& modelData_;

	// アニメーション
	AnimationController* animationController_;

	/// <summary>
	/// アクティブ状態
	/// </summary>
	bool isActive_;

	/// <summary>
	/// ダメージを受けて0以下になると死亡
	/// </summary>
	int hp_;
	/// <summary>
	/// hpの最大値
	/// </summary>
	int hpMax_;
	//スタミナ
	float stamina_;
	float staminamax;

	//それぞれのアニメーションアタッチ番号
	int idleAnim_;
	int walkAnim_;
	int runAnim_;

	// アニメーションの総再生時間
	float animTotalTime_;
	// 再生中のアニメーション時間
	float stepAnim_;

	//座標と回転
	Transform transform_;

	//当たり判定の半径(地形)
	float colliderSize_;

	//当たり判定の半径(弾に対して)
	float colliderRadiusShot_;

	SceneManager* sceneManager_;

	//移動中のベクトル
	VECTOR moveVec_;

	//trueなら滑空を試みる
	bool activeGlider_;

	//接地中
	bool isGrounded_;

	int notGroundedTimer_;

	//そのフレームでダメージを受けた
	bool damaged_;
	//直前のダメージ判定
	bool preDamaged_;
	///----------------------------------
	// 移動スピード
	float speed_;
	// 移動方向
	VECTOR moveDir_;
	// 移動量
	VECTOR movePow_;
	// 移動後の座標
	VECTOR movedPos_; //→ ※ transform_.pos に移動後座標を
	//傾斜の方向
	VECTOR slopeDir_;

	//傾斜角
	float slopeAngleDeg_;

	//傾斜の力
	VECTOR slopePow_;

	//衝突している地面ポリゴンの法線
	VECTOR hitNormal_;

	//衝突している地面との座標
	VECTOR hitPos_;
	// フレームごとの移動値
	VECTOR moveDiff_;

	// ジャンプ量
	VECTOR jumpPow_;
	// 衝突判定に用いられるコライダ
	std::vector<std::weak_ptr<Collider>> colliders_;
	// 衝突チェック
	VECTOR gravHitPosDown_;
	VECTOR gravHitPosUp_;

	// ジャンプ判定
	bool isJump_;
	// ジャンプの入力受付時間
	float stepJump_;

 ///-----------------------------


	// パラメータ設定(純粋仮想関数。派生クラスで必ず初期設定を実装して貰うように)
	virtual void SetParam(void) = 0;

	//移動処理
	virtual void Update_Move();

	//攻撃処理
	virtual void Update_Shot();

	/// <summary>
	/// カウントを進める
	/// </summary>
	virtual void Update_Count();
	/// <summary>
	/// 物理を考慮して移動
	/// </summary>
	void Move(VECTOR vec);

	//落下時などの座標リセット
	virtual void Respawn();

	//アニメーション
	void Animation();

	//回転を反映
	//void SetRotation();

	/// <summary>
	/// アニメーション再生
	/// </summary>
	/// <param name="curState">変更前。指定しない場合はANIM::MAX</param>
	/// <param name="type">変更先</param>
	/// <returns>成功した場合は変更先と同じ値をそのまま返す</returns>
	virtual ANIM PlayAnim(ANIM curState, ANIM type, bool isLoop = true,
		float startStep = 0.0f, float endStep = -1.0f, bool isStop = false, bool isForce = false);

	/// <summary>
	/// 落下アニメーションループ解除
	/// </summary>
	void GliderEnd();

	/// <summary>
	/// カメラの角度を基準に方向分の角度へゆっくり近づく
	/// </summary>
	/// <param name="goalRot">目標</param>
	/// <param name="spd">速度の倍率</param>
	/// <returns>回転が実行されるとtrue</returns>
	bool LazyRotation(float goalRot,float spd = 1);

	// 衝突判定
	void Collision(void);
	void CollisionGravity(void);
	// 移動量の計算
	void CalcGravityPow(void);

	// 傾斜の計算
	void CalcSlope(void);
	void CollisionCapsule(void);

	std::unique_ptr<Capsule> capsule_;
	//void CollisionCapsule(void);

	void virtual SetMaterialSetting();

	//大きさ
	virtual void Mv1Scale();

};
