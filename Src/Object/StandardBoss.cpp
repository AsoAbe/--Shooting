#include <EffekseerForDXLib.h>
#include "../Common/AsoUtility.h"
#include "../Scene/SceneGame.h"
#include "../Manager/ObjectManager.h"
#include "../Manager/SoundManager.h"
#include "./Player.h"
#include "../Application.h"

#include "StandardBoss.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

namespace
{
	//少なくともこのZを越えてから突進を繰り返す
	constexpr float DASH_END_MIN_Z = -1000;

	constexpr int ATTACK_STATE_NUM = 3;

	/// <summary>
	/// 予備動作完了までのフレーム
	/// </summary>
	constexpr int DASH_START_END_F = 90;
	/// <summary>
	/// 予備動作アニメション完了までのフレーム
	/// </summary>
	constexpr int DASH_START_ANIM_F = 30;
	/// <summary>
	/// 予備動作前の行動を止める時間
	/// </summary>
	constexpr int DASH_START_WAIT_F = 45;
	/// <summary>
	/// 予備動作の移動距離
	/// </summary>
	constexpr int DASH_START_MOVE = 400;
	/// <summary>
	/// 移動速度
	/// </summary>
	constexpr int DASH_SPEED = 40;
	/// <summary>
	/// 繰り返し時の移動時間(フレーム)
	/// </summary>
	constexpr int DASH_TIME_F = 30;
	/// <summary>
	/// 繰り返し時の移動時間(フレーム)
	/// </summary>
	constexpr int DASH_WAIT_F = 15;
	/// <summary>
	/// プレイヤーを越えた後の移動時間(フレーム)
	/// </summary>
	constexpr int DASH_SHORT_F = 12;

	/// <summary>
	/// ダッシュ時に元の座標に戻る際のランダム幅
	/// </summary>
	constexpr int DASH_RETURN_RANDOM = 800;

	/// <summary>
	/// 移動を折り返すx(中心を0とした絶対値)
	/// </summary>
	constexpr float MOVE_X_END = 800;

}

StandardBoss::StandardBoss(SceneGame* parent, const CharacterModelData& modelData) :CharacterBase(modelData)
{
	attackUpdate_ = &StandardBoss::AttackUpdate_Shot;
	sceneGame_ = parent;
	counter_ = 0;
	changeTime_ = 0;
	state_ = ATTACK_STATE::AROUND;
	nextState_ = ATTACK_STATE::AROUND;
	moveState_ = MOVE_STATE::R;
	stateChangeCount_ = 0;
	lastCount_ = 0;
	dashVec_ = {0,0,1};
	dashCountEnd_ = 0;
	dashCount_ = 0;
	dashChangeVecFlag_ = false;
	lastPos_ = {};
	returnPos_ = {};
	chargeEffect_ = -1;
	died_ = false;
	lowHpAttackFlag_ = true;
}
StandardBoss::~StandardBoss(void)
{
}

void StandardBoss::ChangeState(ATTACK_STATE state)
{
	constexpr int DASH_NUM = 2;
	constexpr int STATE_CHANGE_TIME_STANDARD = 3;
	constexpr int STATE_CHANGE_TIME_LONG = 8;
	state_ = state;
	lastCount_ =counter_;
	lastPos_ = transform_.pos;
	changeTime_ = STATE_CHANGE_TIME_STANDARD;

	const VECTOR& pPos = sceneGame_->GetOManager()->GetPlayer()->GetPos();
	//yを考慮しない差を求める
	VECTOR diff = VSub(transform_.pos, pPos);
	diff.y = 0;

	switch (state)
	{
	case StandardBoss::ATTACK_STATE::POWER_CHARGING:
		ChangeState_PowerCharging(diff);
		break;
	case StandardBoss::ATTACK_STATE::DASH:
		dashCountEnd_ = DASH_NUM;
		dashCount_ = 0;//プレイヤー座標を越えたら1として扱う。その後は方向転換の度に+1
		dashChangeVecFlag_ = true;
		//ダッシュ効果音
		SoundManager::GetInstance().PlaySE(SoundManager::SOUND_ID::ENEMY_DASH);
		//updateに反映
		attackUpdate_ = &StandardBoss::AttackUpdate_Dash;
		break;
	case StandardBoss::ATTACK_STATE::RETURN:
		ChangeState_Return();
		break;
	case StandardBoss::ATTACK_STATE::RANDOM:
	case StandardBoss::ATTACK_STATE::RANDOM_MOVING:
		changeTime_ = STATE_CHANGE_TIME_LONG;
		attackUpdate_ = &StandardBoss::AttackUpdate_Shot;
		break;
	case StandardBoss::ATTACK_STATE::AROUND:
	case StandardBoss::ATTACK_STATE::AROUND_MOVING:
		attackUpdate_ = &StandardBoss::AttackUpdate_Shot;
		break;
	default:
		attackUpdate_ = nullptr;
		break;
	}
}

void StandardBoss::Update()
{
	if (IsActive() == false)
	{
		return;
	}
	if (attackUpdate_ != nullptr&& died_ == false)
	{
		(this->*attackUpdate_)();
	}
	constexpr float LINE_LENGTH = 500;
	transform_.pos.y = sceneGame_->GetOManager()->GetOnGroundY(GetPos(), LINE_LENGTH, LINE_LENGTH);

	//モデルの位置を更新
	transform_.Update();

	Animation();
}

void StandardBoss::Died()
{
	if (sceneGame_->IsTutorial())
	{
		//チュートリアル中は死なない
		return;
	}
	died_ = true;
	//SetActive(false);
}

VECTOR StandardBoss::GetLocalDiff(void)
{
	const VECTOR& pPos = sceneGame_->GetOManager()->GetPlayer()->GetPos();
	//yを考慮しない差を求める
	VECTOR diffXZ = VSub(pPos, transform_.pos);
	return diffXZ;
}

void StandardBoss::ResetStatus()
{
	CharacterBase::ResetStatus();
	ChangeState(ATTACK_STATE::AROUND);
	stateChangeCount_ = 0;
}

bool StandardBoss::Damage(int damage)
{
	bool ret = CharacterBase::Damage(damage);
	if (ret)
	{
		//ヒット時の演出
		SoundManager& sManager = SoundManager::GetInstance();
		sManager.PlaySE_CT(SoundManager::SOUND_ID::HIT);
		sceneGame_->EnemyBarAnim();
	}
	return ret;
}

void StandardBoss::SetParam()
{
	transform_.quaRotLocal = Quaternion::Euler({ 0, AsoUtility::DegToRadF(180), 0 });
	colliderSize_ = DEFAULT_COLLIDERSIZE;
	colliderRadiusShot_ = 24;
	hpMax_ = 200;
	hp_ = hpMax_;
}

void StandardBoss::Update_Move()
{
	if(IsMovingShot())
	{
		moveVec_.x = 4;
		if (moveState_ != MOVE_STATE::R)
		{
			moveVec_.x *= -1;
		}

		transform_.pos = VAdd(transform_.pos, moveVec_);
		//移動制限とループ
		if (moveState_ == MOVE_STATE::R && transform_.pos.x> MOVE_X_END)
		{
			moveState_ = MOVE_STATE::L;
			NextState();
		}
		else if(transform_.pos.x <-1*MOVE_X_END)
		{
			moveState_ = MOVE_STATE::R;
			NextState();
		}
	}
}


void StandardBoss::Update_Shot()
{
	counter_++;
	//fps60基準のcount_
	int shotCount = GetShotCount(counter_);
	if (state_ == ATTACK_STATE::RANDOM && chargeEffect_ == -1 &&Application::FPS * changeTime_- SceneGame::CHARGE_EFFECT_F <= (counter_ - lastCount_))
	{
		//突進の予兆エフェクト
		chargeEffect_ = CreateChargeEffect();
	}
	if (Application::FPS * changeTime_ <= (counter_- lastCount_) && IsMovingShot() == false)
	{
		//時間が経過したのでステート切り替え
		if (state_ != ATTACK_STATE::RANDOM)
		{
			NextState();
		}
		else
		{
			//ランダム攻撃の場合は続けて突進行動
			ChangeState(ATTACK_STATE::DASH);
			StopEffekseer3DEffect(chargeEffect_);
			chargeEffect_ = -1;
		}
	}
	constexpr int AROUND_TIME = 45;
	constexpr int AROUND_LOOP = 18;
	switch (state_)
	{
	case StandardBoss::ATTACK_STATE::RANDOM_MOVING:
	case StandardBoss::ATTACK_STATE::RANDOM:
		if (shotCount % (2) == 0)
		{
			//ショットの生成
			sceneGame_->GetOManager()->CreateShot(ObjectManager::ShotType::SHOT_LOW, GetColPos(),
				Quaternion::Euler(0, AsoUtility::Deg2RadF(GetRand(360)), 0),
				1);
		}
		break;
	case ATTACK_STATE::AROUND_MOVING:
	case ATTACK_STATE::AROUND:
		if (shotCount % (AROUND_TIME) == 0)
		{
			//1つ進むごとに加算される角度(deg)
			static constexpr float AROUND_ADD = 6;
			float addRad = 0 ;
			if (IsMovingShot() == false)
			{
				//回転
				addRad = AsoUtility::Deg2RadF(AROUND_ADD) * (shotCount / AROUND_TIME);
			}
			//ショットの生成
			ShotAround(ObjectManager::ShotType::SHOT, GetColPos(), AROUND_LOOP, addRad);
		}
		break;
	default:
		break;
	}
}

void StandardBoss::AttackUpdate_Shot()
{
	Update_Move();
	Update_Shot();
}

void StandardBoss::AttackUpdate_PowerCharging()
{
	//終了までの時間に待機時間を足したもの
	constexpr int DASH_START_TOTAL_F = DASH_START_END_F + DASH_START_WAIT_F;

	counter_++;
	if (counter_ - lastCount_ <= DASH_START_WAIT_F)
	{
		//待機
		return;
	}
	if (counter_ - lastCount_ <= DASH_START_WAIT_F+ DASH_START_ANIM_F)
	{
		float dashAnim = sinf((static_cast<float>((counter_ - lastCount_)- DASH_START_WAIT_F) / DASH_START_ANIM_F) * (DX_PI_F * 0.5f));
		transform_.pos = VAdd(lastPos_,VScale(moveVec_, dashAnim));
	}
	if ((counter_ - lastCount_) >= DASH_START_TOTAL_F -SceneGame::CHARGE_EFFECT_F)
	{
		VECTOR colPos = GetColPos();
		if (chargeEffect_ == -1)
		{
			chargeEffect_ =CreateChargeEffect();
		}
		else
		{
			SetPosPlayingEffekseer3DEffect(chargeEffect_, colPos.x, colPos.y, colPos.z);
		}
	}
	if (counter_ - lastCount_ >= DASH_START_TOTAL_F)
	{
		//待機が終わったら攻撃開始
		int rand = GetRand(3-1);
		if (IsLowHp() && (lowHpAttackFlag_ || rand == 0))
		{
			lowHpAttackFlag_ =false;
			ChangeState(ATTACK_STATE::RANDOM);
		}
		else
		{
			ChangeState(ATTACK_STATE::DASH);
		}
		StopEffekseer3DEffect(chargeEffect_);
		chargeEffect_ = -1;
	}
}

void StandardBoss::AttackUpdate_Dash()
{
	constexpr int DASH_TIME_LONG_F = DASH_TIME_F *3;
	counter_++;
	if (counter_- lastCount_ <= DASH_WAIT_F && !(dashCount_ <= 1))
	{
		//待機時間
		return;
	}
	if (dashCount_ > dashCountEnd_)
	{
		//突進終わり
		ChangeState(ATTACK_STATE::RETURN);
		return;
	}
	const VECTOR& pPos = sceneGame_->GetOManager()->GetPlayer()->GetPos();
	//yを考慮しない差を求める
	VECTOR diffXZ = VSub(pPos, transform_.pos);
	diffXZ.y = 0;
	if (dashChangeVecFlag_)
	{
		//方向転換可能なら転換
		DashTurn(diffXZ);
		dashChangeVecFlag_ = false;

	}
	transform_.pos = VAdd(transform_.pos, dashVec_);
	if (dashCount_ == 0)
	{
		//最初の突進なら
		if (VDot(diffXZ,dashVec_)< 0 || (counter_ - lastCount_ > DASH_TIME_LONG_F))
		{
			//内積がマイナス(ベクトルが反対を向いている状態)になったらダッシュを止める
			dashCount_++;
			lastCount_ = counter_;
		}
	}
	else if (dashCount_ == 1)
	{
		//プレイヤーを越えた後
		if (counter_ - lastCount_ > DASH_SHORT_F)
		{
			dashCount_++;
			dashChangeVecFlag_ = true;
			lastCount_ = counter_;
		}
	}
	else
	{
		if (counter_-lastCount_ > DASH_TIME_F+ DASH_WAIT_F)
		{
			dashCount_++;
			dashChangeVecFlag_ = true;
			lastCount_ = counter_;
		}
	}
}

void StandardBoss::AttackUpdate_Return()
{
	counter_++;
	if (counter_ - lastCount_>= DASH_START_ANIM_F)
	{
		transform_.pos = returnPos_;
		if (counter_ - lastCount_ >= DASH_START_ANIM_F * 2)
		{
			if (sceneGame_->IsJumpTutorial())
			{
				sceneGame_->CheckJumpTutorial();
				//シーンゲーム側でプレイヤーのを見てジャンプを一回以上、HPが減っていないをどちらも満たすなら次のチュートリアルへ向かう
			}
			NextState();
		}
		return;
	}
	float rate = static_cast<float>((counter_ - lastCount_)) / DASH_START_ANIM_F;
	//線形補完
	transform_.pos = VAdd(VScale(returnPos_, rate) , VScale(lastPos_, 1 - rate));
}

int StandardBoss::CreateChargeEffect()
{
	int ret = sceneGame_->CreateEffect(SceneGame::EFFECT_TYPE::CHARGE, GetColPos(), 1.0f);
	SoundManager::GetInstance().PlaySE(SoundManager::SOUND_ID::CHARGE);
	return ret;
}

void StandardBoss::DashTurn(const VECTOR& diffXZ)
{
	//ベクトル変更
	dashVec_ = VScale(VNorm(diffXZ), DASH_SPEED);
	//効果音
	SoundManager::GetInstance().PlaySE(SoundManager::SOUND_ID::ENEMY_DASH);
}

void StandardBoss::ChangeState_PowerCharging(const VECTOR& diff)
{
	//向き設定
	moveVec_ = VScale(VNorm(diff), DASH_START_MOVE);
	attackUpdate_ = &StandardBoss::AttackUpdate_PowerCharging;
}
void StandardBoss::ChangeState_Return()
{
	float randomX = GetRand(DASH_RETURN_RANDOM) - (DASH_RETURN_RANDOM / 2);
	float randomZ = GetRand(DASH_RETURN_RANDOM) - (DASH_RETURN_RANDOM / 2);
	returnPos_ = { randomX,0, randomZ };
	attackUpdate_ = &StandardBoss::AttackUpdate_Return;
}


int StandardBoss::GetShotCount(int count)
{
	return static_cast<int>(Application::DEFAULT_FPS / static_cast<float>(Application::FPS) * count);
}

void StandardBoss::NextState()
{

	if (sceneGame_->IsTutorial())
	{
		//チュートリアルは行動が変わらない
		if (sceneGame_->IsGrazeTutorial())
		{
			//行動終了時にCheckGrazeTutorial(クリア判定)
			if(sceneGame_->CheckGrazeTutorial())
			{
				ChangeState(ATTACK_STATE::NONE);
				return;
			}
			ChangeState(ATTACK_STATE::AROUND);
			return;
		}
		if (sceneGame_->IsJumpTutorial())
		{
			ChangeState(ATTACK_STATE::POWER_CHARGING);
			return;
		}
		//行動しない場合はNONE
		ChangeState(ATTACK_STATE::NONE);
		return;
	}
	stateChangeCount_++;
	ChangeState(nextState_);
	if (stateChangeCount_% ATTACK_STATE_NUM != 0)
	{
		//nextState_ = ATTACK_STATE::AROUND_MOVING;
		nextState_ = ATTACK_STATE::AROUND;
		return;
	}
	//ATTACK_STATE_NUMの倍数でパワーチャージ
	nextState_ = ATTACK_STATE::POWER_CHARGING;
	//int rand = GetRand(1);
	//if (rand == 1 && IsLowHp())
	//{
	//	nextState_ = ATTACK_STATE::RANDOM;
	//}
	//else
	//{
	//	//nextState_ = ATTACK_STATE::AROUND;
	//	nextState_ = ATTACK_STATE::POWER_CHARGING;
	//}
}

bool StandardBoss::IsLowHp()const
{
	constexpr float LOW_HP = 0.5f;
	return static_cast<float>(hp_) / hpMax_ <= LOW_HP;
}

bool StandardBoss::IsMovingShot()
{
	return (state_ == ATTACK_STATE::AROUND_MOVING||
		state_ == ATTACK_STATE::RANDOM_MOVING||
		state_ == ATTACK_STATE::DASH);
}

void StandardBoss::ShotAround(ObjectManager::ShotType type, VECTOR pos, int loop, float addRot)
{
	for (int i = 0; i < loop; i++)
	{
		sceneGame_->GetOManager()->CreateShot(type, pos,
			Quaternion::Euler(0, AsoUtility::Deg2RadF(360.0f / loop) * i + addRot, 0),
			1);
	}
}

void StandardBoss::Mv1Scale()
{
}

bool StandardBoss::IsActiveCollision() const
{
	
	return died_ == false;
}
