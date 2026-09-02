#pragma once
#include <memory>
#include <DxLib.h>
#include "Quaternion.h"
#include "Collider.h"

/// <summary>
/// モデル制御の基本情報
/// 大きさ：VECTOR基準
/// 回転　：Quaternion基準
/// 位置　：VECTOR基準
/// </summary>
class Transform
{

public:
	// 無効なモデルハンドル
	static constexpr int INVALID_MODEL_ID = -1;

	// コライダー設定時の対象モデル全体
	static constexpr int COLLISION_TARGET_ALL = -1;

	// コライダー設定時の判定数
	static constexpr int COLLISION_DIVISION_COUNT = 1;

	// コライダー設定時の判定フラグ
	static constexpr int COLLISION_ENABLED = 1;
	// コンストラクタ
	Transform(void);
	Transform(int model);

	// デストラクタ
	~Transform(void);

	// モデルのハンドルID
	int modelId;

	// 大きさ
	VECTOR scl;
	// 回転
	VECTOR rot;
	// 位置
	VECTOR pos;
	VECTOR localPos;

	MATRIX matScl;
	MATRIX matRot;
	MATRIX matPos;

	// 回転
	Quaternion quaRot;

	// ローカル回転
	Quaternion quaRotLocal;

	// コライダ
	std::shared_ptr<Collider> collider;

	/// <summary>
	/// モデル制御の基本情報更新
	/// </summary>
	/// <param name=""></param>
	void Update(void);

	void SetModel(int modelHId);
	void MakeCollider(Collider::TYPE type);

	// 前方方向を取得
	VECTOR GetForward(void) const;

	// 後方方向を取得
	VECTOR GetBack(void) const;

	// 右方向を取得
	VECTOR GetRight(void) const;

	// 左方向を取得
	VECTOR GetLeft(void) const;

	// 上方向を取得
	VECTOR GetUp(void) const;

	// 下方向を取得
	VECTOR GetDown(void) const;

	// 対象方向を取得
	VECTOR GetDir(const VECTOR& vec) const;

};

