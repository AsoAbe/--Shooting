#pragma once
#include "../ObjectBase.h"

class ObjectModelData;

class ShotBase :
	public ObjectBase
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="parent">SceneGame</param>
	/// <param name="model">MV1DuplicateModelの引数</param>
	ShotBase(SceneGame* parent, const ObjectModelData& modelData);
	~ShotBase(void) override;

	/// <summary>
	/// 標準的な弾の生存時間(秒)
	/// </summary>
	static constexpr float DEFAULT_SHOT_TIME = 8.0f;
	/// <summary>
	/// 標準的な弾の威力
	/// </summary>
	static constexpr int DEFAULT_DAMAGE = 8;


	/// <summary>
	/// Init後にActivationを呼ぶ
	/// </summary>
	void Init()override;

	void Update()override;

	/// <summary>
	/// アクティブ化
	/// </summary>
	/// <param name="startPos"></param>
	/// <param name="quaRot"></param>
	/// <param name="size">1で1倍</param>
	virtual void Activation(VECTOR startPos, Quaternion quaRot, float size);

	/// <summary>
	/// 当たり判定の球を描画
	/// </summary>
	virtual void DrawCol();

	/// <summary>
	/// 型を返す(同じタイプか確認するため)
	/// </summary>
	/// <returns></returns>
	const type_info& GetShotType(void) const;

	/// <summary>
	/// 炸裂エフェクトなどを再生して非アクティブ化する
	/// </summary>
	virtual void Hit();

	/// <summary>
	/// 実際の半径。colRadiusにscaleをかけた値
	/// </summary>
	/// <returns></returns>
	float GetColRadius()const;

	/// <summary>
	/// プレイヤーチーム扱いならtrueを返す
	/// </summary>
	bool IsPlayerTag()const;

	/// <summary>
	/// エフェクト付きで弾を非アクティブ化
	/// </summary>
	void DeactivateShot();

	/// <summary>
	/// 威力を返す
	/// </summary>
	/// <returns></returns>
	int GetDamage()const;
protected:

	//trueでプレイヤーチーム扱い
	bool isPlayerTag_;

	// 移動速度
	float speed_;

	//当たり判定半径
	float colRadius_;

	/// <summary>
	/// 初期値設定後にカウントダウンを始め、ゼロになったら消滅する(秒)
	/// </summary>
	float shotTimer_;

	/// <summary>
	/// 威力
	/// </summary>
	int damage_;

	/// <summary>
	/// ShotBaseの場合は直進
	/// </summary>
	virtual void Move();

	/// <summary>
	/// 常に接地させる。無効化する場合はこれをオーバーライドする
	/// </summary>
	virtual void SetPosOnGround();

};

