#include <DxLib.h>
#include "AsoUtility.h"
#include "Transform.h"

Transform::Transform(void)
{
	modelId = INVALID_MODEL_ID;


	scl = AsoUtility::VECTOR_ONE;
	rot = AsoUtility::VECTOR_ZERO;
	pos = AsoUtility::VECTOR_ZERO;
	localPos = AsoUtility::VECTOR_ZERO;

	matScl = MGetIdent();
	matRot = MGetIdent();
	matPos = MGetIdent();
	quaRot = Quaternion();
	quaRotLocal = Quaternion();

	collider = nullptr;
}

Transform::Transform(int model)
{
	modelId = model;

	scl = AsoUtility::VECTOR_ONE;
	rot = AsoUtility::VECTOR_ZERO;
	pos = AsoUtility::VECTOR_ZERO;
	localPos = AsoUtility::VECTOR_ZERO;

	matScl = MGetIdent();
	matRot = MGetIdent();
	matPos = MGetIdent();
	quaRot = Quaternion();
	quaRotLocal = Quaternion();

	collider = nullptr;
}

Transform::~Transform(void)
{
}

void Transform::Update(void)
{

	// 大きさ
	matScl = MGetScale(scl);

	// 回転
	rot = quaRot.ToEuler();
	matRot = quaRot.ToMatrix();

	// 位置
	matPos = MGetTranslate(pos);

	// 行列の合成
	MATRIX mat = MGetIdent();
	mat = MMult(mat, matScl);
	Quaternion q = quaRot.Mult(quaRotLocal);
	mat = MMult(mat, q.ToMatrix());
	mat = MMult(mat, matPos);

	// 行列をモデルに判定
	if (modelId != INVALID_MODEL_ID)
	{
		MV1SetMatrix(modelId, mat);
	}

	// 衝突判定の更新
	if (collider != nullptr)
	{
		MV1RefreshCollInfo(modelId);
	}

}

void Transform::SetModel(int model)
{
	modelId = model;
}

void Transform::MakeCollider(Collider::TYPE type)
{

	if (modelId == INVALID_MODEL_ID)
	{
		return;
	}

	collider = std::make_shared<Collider>(type, modelId);
	int ret = MV1SetupCollInfo(modelId,
		COLLISION_TARGET_ALL,
		COLLISION_DIVISION_COUNT,
		COLLISION_ENABLED,
		COLLISION_ENABLED);

}

VECTOR Transform::GetForward(void) const
{
	return GetDir(AsoUtility::DIR_F);
}

VECTOR Transform::GetBack(void) const
{
	return GetDir(AsoUtility::DIR_B);
}

VECTOR Transform::GetRight(void) const
{
	return GetDir(AsoUtility::DIR_R);
}

VECTOR Transform::GetLeft(void) const
{
	return GetDir(AsoUtility::DIR_L);
}

VECTOR Transform::GetUp(void) const
{
	return GetDir(AsoUtility::DIR_U);
}

VECTOR Transform::GetDown(void) const
{
	return GetDir(AsoUtility::DIR_D);
}

VECTOR Transform::GetDir(const VECTOR& vec) const
{
	return quaRot.PosAxis(vec);
}
