#include "../Common/AsoUtility.h"
#include "../Common/Transform.h"
#include "../Common/Capsule.h"
#include "../Common/Collider.h"
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Scene/SceneGame.h"
#include "../Object/Map.h"
#include "../Common/CharacterModelData.h"
#include "../Common/AnimationController.h"
#include "CharacterBase.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
CharacterBase::CharacterBase(const CharacterModelData& modelData):modelData_(modelData)
{
	isActive_ = true;
	hp_ = 0;
	hpMax_ = 0;
	stamina_ = 0;
	staminamax = 0;
	animationController_ = nullptr;
	moveVec_ = AsoUtility::VECTOR_ZERO;
	jumpPow_ = AsoUtility::VECTOR_ZERO;
	notGroundedTimer_ = 0;

	colliderSize_ = 0;
	colliderRadiusShot_ = 0;

	activeGlider_ = false;
	preDamaged_ = false;

	capsule_ = std::make_unique<Capsule>(transform_);
}

CharacterBase::~CharacterBase()
{
}

void CharacterBase::Init(void)
{
	//パラメータ設定
	SetParam();

	

	//モデル
	transform_.SetModel(MV1DuplicateModel(modelData_.model_));
	transform_.scl = VScale(AsoUtility::VECTOR_ONE, modelData_.GetModelScale());

	animationController_ = new AnimationController(transform_.modelId, modelData_);

	transform_.Update();
	//アニメーションをアタッチ
#pragma region AddAnim
	for (int i = 0; i < static_cast<int>(ANIM::MAX); i++)
	{
		ANIM anim = static_cast<ANIM>(i);
		animationController_->Add(anim, modelData_.GetAnimMHandle(anim), modelData_.GetAnimSpeed(anim));
	}
#pragma endregion
	//IDLEで適用
	PlayAnim(ANIM::MAX,ANIM::IDLE);
	//シーンマネージャー
	sceneManager_ = &(SceneManager::GetInstance());

	SetMaterialSetting();
}

void CharacterBase::Update(void)
{
	if (!(IsActive()))
	{
		return;
	}
	Update_Move();
	Update_Shot();
	Update_Count();

	Animation();

	Collision();

	//3Dモデルの制御
	// -----
	transform_.Update();

}

void CharacterBase::Draw(void)
{
	if (IsActive())
	{

		if (transform_.modelId != -1)
		{
			MV1DrawModel(transform_.modelId);
		}
		else
		{
#ifdef _DEBUG
			DrawSphere3D(transform_.pos, 32, 8, 0xFF0000, 0xFFFFFF, true);
#endif
		}
	}
#ifdef _DEBUG
	capsule_->Draw();
	DrawLine3D(gravHitPosDown_, GetPos(), 0xffffff);
#endif
}

void CharacterBase::Release(void)
{
	delete animationController_;
	animationController_ = nullptr;
	MV1DeleteModel(transform_.modelId);
}

VECTOR CharacterBase::GetPos() const
{
	return transform_.pos;
}

VECTOR CharacterBase::GetRot() const
{
	return transform_.rot;
}

VECTOR CharacterBase::GetColPos() const
{
	VECTOR ret = transform_.pos;
	//Yに加算
	ret.y += DEFAULT_COLLIDER_POS_Y;
	return ret;
}


void CharacterBase::SetPos(VECTOR p)
{
	transform_.pos = p;
}

void CharacterBase::SetRot(VECTOR rot)
{
	transform_.rot = rot;
}

void CharacterBase::AddCollider(std::weak_ptr<Collider> collider)
{
	colliders_.emplace_back(collider);
}

void CharacterBase::ClearCollider(void)
{
	colliders_.clear();
}

const Capsule& CharacterBase::GetCapsule(void) const
{
	// TODO: return ステートメントをここに挿入します
	return *capsule_;
}

void CharacterBase::ResetStatus()
{
	hp_ = hpMax_;
}

bool CharacterBase::IsActiveCollision() const
{
	return true;
}

float CharacterBase::GetColRadiusShot()const
{
	return colliderRadiusShot_;
}

float CharacterBase::GetColSize() const
{
	return colliderSize_;
}

int CharacterBase::GetHp() const
{
	return hp_;
}

int CharacterBase::GetHpMax() const
{
	return hpMax_;
}

int CharacterBase::GetStamina() const
{
	return stamina_;
}

int CharacterBase::GetStaminaMax() const
{
	return staminamax;
}

bool CharacterBase::Damage(int damage)
{
	if (preDamaged_ == true)
	{
		//同じ判定で連続ダメージ防止
		damaged_ = true;
		return false;
	}
	hp_ -= damage;
	//死亡判定
	if (hp_ <= 0)
	{
		Died();
	}
	damaged_ = true;
	return true;
}

void CharacterBase::Died()
{
	//SetActive(false);
}

bool CharacterBase::IsActive() const
{
	return isActive_;
}

void CharacterBase::SetActive(bool b)
{
	isActive_ = b;
}

void CharacterBase::Update_Move()
{
}

void CharacterBase::Update_Shot()
{
}

void CharacterBase::Update_Count()
{
	preDamaged_ = damaged_;
	damaged_ = false;
}

void CharacterBase::Move(VECTOR vec)
{
	movedPos_ = VAdd(transform_.pos, vec);

	CalcGravityPow();



	Collision();

	// 移動
	moveDiff_ = VSub(movedPos_, transform_.pos);
	transform_.pos = movedPos_;

	if (transform_.pos.y < -9999)
	{
		//落下でリスポーン
		Respawn();
	}
}

void CharacterBase::Respawn()
{
	moveVec_ = AsoUtility::VECTOR_ZERO;
	jumpPow_ = AsoUtility::VECTOR_ZERO;
	transform_.pos = { 0,999,0 };
}

void CharacterBase::Animation()
{
	animationController_->Update();
}

//void CharacterBase::SetRotation()
//{
//	// 回転を更新
//	MATRIX mat = MGetIdent();
//	mat = MMult(mat, MGetRotX(transform_.rot.x));
//	mat = MMult(mat, MGetRotY(transform_.rot.y));
//	mat = MMult(mat, MGetRotZ(transform_.rot.z));
//	// 調整用の回転行列
//	MATRIX localMat = MGetIdent();
//	localMat = MMult(localMat, MGetRotX(rotLocal_.x));
//	localMat = MMult(localMat, MGetRotY(rotLocal_.y));
//	localMat = MMult(localMat, MGetRotZ(rotLocal_.z));
//	// 行列の合成
//	mat = MMult(mat, localMat);
//
//	//マトリクスで回転を適用
//	MV1SetRotationMatrix(transform_.modelId, mat);
//}


ANIM CharacterBase::PlayAnim(ANIM curState,ANIM type, bool isLoop , float startStep, float endStep, bool isStop, bool isForce)
{
	if ((curState == ANIM::JUMP) && !(type == ANIM::FALLING || type == ANIM::ONGROUND)||
		(curState == ANIM::FALLING && !(isGrounded_)&& type != ANIM::ONGROUND) ||
		curState == ANIM::ONGROUND && !(animationController_->IsEnd()))
	{
		return curState;
	}
	switch (type)
	{
	case CharacterBase::ANIM::JUMP:
		isLoop = false;
		startStep = 0;
		endStep = modelData_.GetGliderS();
		isForce = true;
		break;
	case CharacterBase::ANIM::FALLING:
		isLoop = true;
		startStep = modelData_.GetGliderS();
		endStep = modelData_.GetGliderE();
		isForce = false;
		break;
	case CharacterBase::ANIM::ONGROUND:
		isLoop = false;
		startStep = modelData_.GetOnGroundS();
		endStep = -1;
		isForce = false;
		break;
	default:
		break;
	}
	animationController_->Play(type, isLoop, startStep, endStep, isStop, isForce);

	return type;
}

void CharacterBase::GliderEnd()
{
	animationController_->Play(ANIM::FALLING, false, modelData_.GetGliderE(), modelData_.GetOnGroundS(), false, false);
}

bool CharacterBase::LazyRotation(float goalRot, float spd)
{
	bool ret = false;

	float rotSpd = SPEED_ROT * spd;
	float rotSpdRad = SPEED_ROT_RAD * spd;

	//quaRotをrotに変換
	VECTOR rot = Quaternion::ToEuler(transform_.quaRot);

	float degNowAngleY = AsoUtility::DegIn360(AsoUtility::RadToDegF(rot.y));
	float degGoalAngleY = AsoUtility::DegIn360(AsoUtility::RadToDegF(goalRot));
	//0～360度以内に収める
	degGoalAngleY = static_cast<float>(AsoUtility::DegIn360(degGoalAngleY));
	//回転方向が少ない方の回転を取得(1:時計回り,-1:反時計回り)
	int aroundDir = AsoUtility::DirNearAroundDeg(degNowAngleY, degGoalAngleY);

	float diff = AsoUtility::DegIn360(degGoalAngleY - degNowAngleY);
	if (diff > 180)
	{
		//-180から180の範囲に入れる
		diff -= 360;
	}

	if (fabsf(diff) > rotSpd)
	{
		//到達したい角度に回転を加える
		rot.y += rotSpdRad * static_cast<float>(aroundDir);
		rot.y = static_cast<float>(AsoUtility::RadIn2PI(rot.y));
		ret = true;
	}
	else
	{
		//差が小さい場合はそのまま適用する
		rot.y = goalRot;
	}
	//quaRotに反映
	transform_.quaRot = Quaternion::Euler(rot);
	return ret;
}

void CharacterBase::Collision(void)
{
	// 衝突(カプセル)
	CollisionCapsule();

	// 衝突(重力)
	CollisionGravity();

}

void CharacterBase::CollisionGravity(void)
{
	// ジャンプ量を加算
	movedPos_ = VAdd(movedPos_, jumpPow_);
	// 重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;
	// 重力方向の反対
	VECTOR dirUpGravity = AsoUtility::DIR_U;
	// 重力の強さ
	float gravityPow = 25.0f;
	float checkPow = 10.0f;
	gravHitPosUp_ = VAdd(movedPos_, VScale(dirUpGravity, gravityPow));
	gravHitPosUp_ = VAdd(gravHitPosUp_, VScale(dirUpGravity, checkPow * 2.0f));
	gravHitPosDown_ = VAdd(movedPos_, VScale(dirGravity, checkPow));


	bool isGroundedFlag = false;
	for (const auto c : colliders_)
	{
		// 地面との衝突
		auto hit = MV1CollCheck_Line(
			c.lock()->modelId_, -1, gravHitPosUp_, gravHitPosDown_);
		// 最初は上の行のように実装して、木の上に登ってしまうことを確認する
		//if (hit.HitFlag > 0)
		//if (hit.HitFlag > 0 && VDot(dirGravity, jumpPow_) > 0.9f)
		if (hit.HitFlag > 0)
		{
			// 衝突地点から、少し上に移動
			movedPos_ = VAdd(hit.HitPosition, VScale(dirUpGravity, 2.0f));
			// ジャンプリセット
			jumpPow_ = AsoUtility::VECTOR_ZERO;
			stepJump_ = 0.0f;
			if (isJump_)
			{
				// 着地モーション
				//PlayAnim(ANIM::MAX, ANIM::ONGROUND);
			}
			isJump_ = false;
			//着地した場合isGrounded_をtrueに
			isGroundedFlag = true;
		}
	}
	//判定
	if (isGrounded_ && !(isGroundedFlag))
	{
		notGroundedTimer_++;
		if(notGroundedTimer_ > NOT_GROUNDED_TIME)
		{
			//落下モーション
			PlayAnim(ANIM::MAX, ANIM::FALLING);
			isGrounded_ = false;
		}
	} 
	else
	{
		notGroundedTimer_ = 0;
	}
	if (isGroundedFlag)
	{
		isGrounded_ = true;
	}
}

void CharacterBase::CalcGravityPow(void)
{
	// 重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;
	// 重力の強さ
	float gravityPow = GRAVITY * -1;
	if (activeGlider_)
	{
		//滑空
		jumpPow_.y += GRAVITY * GLIDER_FALLSPD;
		float maxFallSpd = GRAVITY * Application::DEFAULT_FPS * GLIDER_MAX_FALLSPD;
		if (jumpPow_.y < maxFallSpd)
		{
			jumpPow_.y = maxFallSpd;
		}
	}
	else
	{
		//重力
		// メンバ変数 jumpPow_ に重力計算を行う(加速度)
		// 内積
		VECTOR gravity = VScale(dirGravity, gravityPow);
		jumpPow_ = VAdd(jumpPow_, gravity);
	}
	
	float dot = VDot(dirGravity, jumpPow_);
	//if (dot >= 0.0f)
	//{
	//	// 重力方向と反対方向(マイナス)でなければ、ジャンプ力を無くす
	//	jumpPow_ = gravity;
	//}

}

void CharacterBase::CalcSlope(void)
{
	VECTOR gravityUp = AsoUtility::DIR_U;

	// 重力の反対方向から地面の法線方向に向けた回転量を取得
	Quaternion up2GNorQua = Quaternion::FromToRotation(gravityUp, hitNormal_);

	// 取得した回転の軸と角度を取得する
	float angle = 0.0f;
	float* anglePtr = &angle;
	VECTOR axis;
	up2GNorQua.ToAngleAxis(anglePtr, &axis);

	// 90度足して、傾斜ベクトルへの回転を取得する
	Quaternion slopeQ = Quaternion::AngleAxis(
		angle + AsoUtility::Deg2RadD(90.0), axis);

	// 地面の傾斜線(黄色)
	slopeDir_ = slopeQ.PosAxis(gravityUp);
	// 傾斜の角度
	slopeAngleDeg_ = static_cast<float>(
		AsoUtility::AngleDeg(gravityUp, slopeDir_));

	// 傾斜による移動
	if (AsoUtility::SqrMagnitude(jumpPow_) == 0.0f)
	{
		float CHECK_ANGLE = 120.0f;
		if (slopeAngleDeg_ >= CHECK_ANGLE)
		{
			float diff = abs(slopeAngleDeg_ - CHECK_ANGLE);
			slopePow_ = VScale(slopeDir_, diff / 3.0f);
			movePow_ = VAdd(movePow_, slopePow_);
		}
	}
}

void CharacterBase::CollisionCapsule(void)
{

	// カプセルを移動させる
	Transform trans = Transform(transform_);
	trans.pos = movedPos_;
	trans.Update();

	Capsule cap = Capsule(*capsule_, trans);
	// カプセルとの衝突判定
	for (const auto c : colliders_)
	{
		auto hits = MV1CollCheck_Capsule(
			c.lock()->modelId_, -1,
			cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius());
		// 衝突した複数のポリゴンと衝突回避するまで、
		// プレイヤーの位置を移動させる
		for (int i = 0; i < hits.HitNum; i++)
		{

			int maxTry = 10;
			switch (c.lock()->type_)
			{
			case Collider::TYPE::WALL:
				maxTry = 30;
				break;
			}

			auto hit = hits.Dim[i];
			// 地面と異なり、衝突回避位置が不明なため、何度か移動させる
			// この時、移動させる方向は、移動前座標に向いた方向であったり、
			// 衝突したポリゴンの法線方向だったりする
			for (int tryCnt = 0; tryCnt < maxTry; tryCnt++)
			{
				// 再度、モデル全体と衝突検出するには、効率が悪過ぎるので、
				// 最初の衝突判定で検出した衝突ポリゴン1枚と衝突判定を取る
				int pHit = HitCheck_Capsule_Triangle(
					cap.GetPosTop(), cap.GetPosDown(), cap.GetRadius(),
					hit.Position[0], hit.Position[1], hit.Position[2]);
				if (pHit)
				{
					// 法線の方向にちょっとだけ移動させる
					movedPos_ = VAdd(movedPos_, VScale(hit.Normal, 1.0f));
					// カプセルも一緒に移動させる
					trans.pos = movedPos_;
					trans.Update();
					continue;
				}
				break;
			}
		}
		// 検出した地面ポリゴン情報の後始末
		MV1CollResultPolyDimTerminate(hits);
	}
}

void CharacterBase::SetMaterialSetting()
{
}

void CharacterBase::Mv1Scale()
{
}
