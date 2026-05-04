#include <math.h>
#include <iostream>
#include <thread>
#include "../Application.h"
#include "../Common/AsoUtility.h"
#include "../Common/DrawUtility.h"
#include "../Common/TimeOut.h"
#include "../Scene/SceneGame.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/ObjectManager.h"
#include "../Manager/SoundManager.h"
#include "../Common/Capsule.h"
#include "../Common/Collider.h"
#include "../Manager/Camera.h"
#include "../Manager/MessageManager.h"
#include "../Manager/ObjectManager.h"
#include "Player.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

namespace
{
	/// <summary>
	/// 60で1秒を基準として何フレームごとに弾を撃つか
	/// </summary>
	constexpr int SHOT_RATE_HIGH = 8;

	/// <summary>
	/// 60で1秒を基準として何フレームごとに弾を撃つか
	/// </summary>
	constexpr int SHOT_RATE = SHOT_RATE_HIGH * 4;
	/// <summary>
	/// 掠り判定の演出時間
	/// </summary>
	constexpr int GRAZE_ANIM_TIME = 30;

	/// <summary>
	/// 射撃時のプレイヤー回転速度倍率
	/// </summary>
	constexpr float PLAYER_ROT_SPD_SHOT = 2;
}

Player::Player(SceneGame* parent, const CharacterModelData& modelData):CharacterBase(modelData)
{
	sceneGame_ = parent;
	animState_ = ANIM::IDLE;
	oldAnimState_ = animState_;
	isOut_ = false;
	autoGliderFlag_ = false;
	preIsGrounded_ = true;
	isGrounded_ = true;
	recoveryTimer_ = 0;
	respawnPos_ = {};
	prePos_ = {};
	grazeAnimTimer_ = 0;
	preGrazed_ = false;
	lifeMax_ = 1;
	life_ = lifeMax_;
	grazeImg_ = -1;
	grazeBonusShot_ = 0;
	lastJumpPosY_ = 0;
	jumpCount_ = 0;
	grazeCombo_ = 0;
	grazeComboScore_ = 0;
	expireTime_ = std::chrono::steady_clock::now();
}

Player::~Player(void)
{
}

void Player::Draw()
{
	if (isActive_)
	{
		Draw_Graze();
	}
	if (IsRecoveryTime() && DrawUtility::Blink(recoveryTimer_, DrawUtility::BLINK_HIGH))
	{
		//点滅処理
		return;
	}
	CharacterBase::Draw();
	//DrawFormatString(0, 200, 0xff0000, "stamina = %f", stamina_);
}

void Player::Release()
{
	if (grazeImg_ != -1)
	{
		DeleteGraph(grazeImg_);
		grazeImg_ = -1;
	}
	CharacterBase::Release();
}

void Player::SetIsOut(bool b)
{
	isOut_ = b;
}

bool Player::Damage(int damage)
{
	if (IsRecoveryTime())
	{
		//復活後無敵
		return false;
	}
	bool ret = CharacterBase::Damage(damage);
	if (ret)
	{
		SoundManager& sManager = SoundManager::GetInstance();
		//ボーナス消滅
		grazeBonusShot_ = 0;
		//コンボを減少
		grazeCombo_--;
		//減少後にコンボを記録
		RecordCombo();
		//コンボをリセット
		grazeCombo_ = 0;
		if ((sceneGame_->IsGrazeTutorial()&& sceneGame_->GetGrazeTutorialProgress() < 1.0f) || sceneGame_->IsJumpTutorial())
		{
			MessageManager::GetInstance().ShowNewMessage("MISS",MessageManager::DEFAULT_MESSAGE_TIME);
			//成功サウンドを取り消し
			sManager.StopSound(SoundManager::SOUND_ID::SUCCESS);
		}
		//カウンターサウンドを取り消し
		sManager.StopSound(SoundManager::SOUND_ID::SHOT_HIGH);
		//ここにサウンドなど
		if (isActive_)
		{
			sManager.PlaySE_CT(SoundManager::SOUND_ID::HIT_PLAYER);
		}
		else
		{
			//死亡サウンド
			sManager.PlaySE_CT(SoundManager::SOUND_ID::DEATH_PLAYER);
		}
		//HPバーの演出開始
		sceneGame_->PlayerBarAnim();
	}
	return ret;
}

bool Player::Graze()
{
	if (sceneGame_->IsJumpTutorial())
	{
		//無効化されるチュートリアルの場合、無効化する
		return false;
	}
	if (damaged_)
	{
		//ダメージ直後は発生しない
		return false;
	}
	if (preGrazed_)
	{
		//同じ判定で連続ヒット防止
		grazeAnimTimer_ = GRAZE_ANIM_TIME;
		return false;
	}
	preGrazed_ = true;
	//ヒットした
	grazeAnimTimer_ = GRAZE_ANIM_TIME;
	grazeBonusShot_ = GRAZE_BONUS_SHOT;
	grazeCombo_++;
	if (sceneGame_->IsGrazeTutorial() && sceneGame_->GetGrazeTutorialProgress() < 1.0f)
	{
		//チュートリアル効果音
		MessageManager::GetInstance().ShowNewMessage("GOOD", MessageManager::DEFAULT_MESSAGE_TIME);
		SoundManager::GetInstance().PlaySE(SoundManager::SOUND_ID::SUCCESS);
	}
	else
	{
		//通常の効果音
		SoundManager::GetInstance().PlaySE(SoundManager::SOUND_ID::GRAZE);
	}
	//HPバーを光らせる
	sceneGame_->PlayerBarFlash();
	return true;
}

void Player::Died()
{
	if (sceneGame_->IsTutorial())
	{
		//チュートリアル中は死なない
		return;
	}
	SetActive(false);
	//残機消費
	life_--;
	//判定
	if (GetLife() <= 0)
	{
		sceneGame_->GameOver();
	}
	else
	{
		//復活可能
		sceneGame_->PlayerDied();
	}
}

int Player::GetLife() const
{
	return life_;
}

int Player::GetLifeMax() const
{
	return lifeMax_;
}

void Player::Respawn_Player()
{
	if (IsActive())
	{
		//すでにアクティブ
		return;
	}
	//復活後無敵時間(秒)
	constexpr float RECOVERY_TIME_S = 4.0f;
	recoveryTimer_ = static_cast<int>(Application::FPS * RECOVERY_TIME_S);
	//リセット
	grazeBonusShot_ = 0;
	hp_ = hpMax_;
	SetPos(respawnPos_);
	sceneManager_->GetCamera()->ResetCameraPos();
	SetActive(true);
}

float Player::GetGrazeRadius() const
{
	return GRAZE_RADIUS;
}

void Player::SetParam()
{
	if (grazeImg_ == -1)
	{
		grazeImg_ = LoadGraph((Application::PATH_IMAGE + "Circle.png").c_str());
	}
	hpMax_ = 8;
	hp_ = hpMax_;
	//stamina = ;
	lifeMax_ = 3;
	life_ = lifeMax_;
	stamina_ = MAX_STAMINA;
	staminamax = MAX_STAMINA;
	transform_.quaRotLocal = Quaternion::Euler({ 0, AsoUtility::DegToRadF(180), 0 });
	respawnPos_ = { 0,0,SceneGame::PLAYER_START_Z };
	transform_.pos = respawnPos_;
	prePos_ = transform_.pos;
	colliderSize_ = DEFAULT_COLLIDERSIZE;
	colliderRadiusShot_ = 24;

	sprintFlag_ = true;

	transform_.quaRotLocal = Quaternion::AngleAxis(AsoUtility::Deg2RadF(180), AsoUtility::AXIS_Y);
	//コライダーを少し浮かせる
	constexpr float COL_Y_ADD = 10;
	capsule_->SetLocalPosDown({ 0,DEFAULT_COLLIDERSIZE + COL_Y_ADD,0 });
	capsule_->SetLocalPosTop({ 0,HEAD_POS,0 });
	capsule_->SetRadius(DEFAULT_COLLIDERSIZE);
}

void Player::Update_Move()
{
	//リセット
	animState_ = ANIM::IDLE;

	auto& ins = InputManager::GetInstance();
	
#pragma region MoveKey
	//移動方向
	//moveVec_ = {0,moveVec_.y,0};//yはそのまま
	VECTOR dir = AsoUtility::VECTOR_ZERO;

	if (ins.IsNew(KEY_INPUT_UP))
	{
		dir = VAdd(dir, { 0,0,1 });
	}
	if (ins.IsNew(KEY_INPUT_DOWN))
	{
		dir = VAdd(dir, { 0,0,-1 });
	}
	if (ins.IsNew(KEY_INPUT_LEFT))
	{
		dir = VAdd(dir, { -1,0,0 });
	}
	if (ins.IsNew(KEY_INPUT_RIGHT))
	{
		dir = VAdd(dir, { 1,0,0 });
	}
	if (ins.IsTrgDown(KEY_INPUT_LSHIFT))
	{
		//sprintFlag_ = !sprintFlag_;
	}
	if (sceneGame_->IsJumpTutorial())
	{
		//移動禁止中の場合無効化
		dir.x = 0;
		dir.z = 0;
	}
#pragma endregion
#pragma region Jump
	if (IsTrgDowm_Jump(ins) && isGrounded_)
	{
		//ジャンプ
		PlayAnim(animState_, ANIM::JUMP);
		SoundManager::GetInstance().PlaySE(SoundManager::SOUND_ID::JUMP);
		oldAnimState_ = ANIM::JUMP;
		jumpPow_.y = JUMP_POW;
		lastJumpPosY_ = transform_.pos.y;
		activeGlider_ = false;
		autoGliderFlag_ = true;
		isGrounded_ = false;
		jumpCount_++;
	}	

	if (autoGliderFlag_ && IsNew_Jump(ins) && jumpPow_.y < 0)
	{
		//ｎ秒間だけ滑空を有効に
		activeGlider_ = true;
		//キー長押しで最高地点で自動滑空
		//activeGlider_ = true;

		autoGliderFlag_ = false;
	}
	else if (IsTrgDowm_Jump(ins))
	{
		if (activeGlider_)
		{
			activeGlider_ = false;
		}
		else
		{
			//activeGlider_ = false;
			if ((transform_.pos.y - lastJumpPosY_ >= GLIDER_MIN_Y || jumpPow_.y < 0) && activeGlider_ == false)
			{
				//上昇を消す
				if (jumpPow_.y > 0)
				{
					jumpPow_.y = 0;
				}
				activeGlider_ = true;
				autoGliderFlag_ = false;
			}
		}
	}
	if (activeGlider_ == true)
	{
		StaminaCount();
	}
#pragma endregion
#pragma region Move
	//移動が入力された
	if (AsoUtility::EqualsVZero(dir) == false)
	{
		
		float speed = SPEED_WALK;
		if (ins.IsNew(KEY_INPUT_C))
		{
			//低速移動
			speed = SPEED_WALK;
			animState_ = ANIM::RUN;
		}
		else
		{
			speed = SPEED_SPRINT;
			animState_ = ANIM::RUN;
		}
		//カメラのアングルを取得
		VECTOR cameraAngles = sceneManager_->GetCamera()->GetAngles();

		//カメラ回転マトリクス
		MATRIX mat = MGetIdent();

		//平面移動の場合はXZを無視
		//mat = MMult(mat, MGetRotY(angles_.x));
		mat = MMult(mat, MGetRotY(cameraAngles.y));
		//mat = MMult(mat, MGetRotY(angles_.z));

		//カメラの回転を適用
		VECTOR moveDir = VTransform(dir, mat);

		float moveAngle = atan2f(moveDir.x, moveDir.z);

		//プレイヤー移動方向に回転を設定
		//rot_ = { 0,angle,0 };

		if (!(IsNew_Shot(ins)))
		{
			LazyRotation(moveAngle);
		}
		else
		{
			//キー入力中は敵の方向(カメラ方向)を向く
			LazyRotation(sceneManager_->GetCamera()->GetAngles().y, PLAYER_ROT_SPD_SHOT);
		}

		//長さを1に
		moveDir = VNorm(moveDir);
		//移動
		moveVec_.x = moveDir.x* speed;
		moveVec_.z = moveDir.z * speed;

		//pos_ = VAdd(pos_, moveVec_);
	}
	else
	{
		moveVec_.x = 0;
		moveVec_.z = 0;
		if (IsNew_Shot(ins))
		{
			//キー入力中は敵の方向(カメラ方向)を向く
			LazyRotation(sceneManager_->GetCamera()->GetAngles().y, PLAYER_ROT_SPD_SHOT);
		}
	}

#pragma endregion Move
	//画面外判定
	if (isOut_ == false)
	{
		outCount_ = 0;
	}
	else
	{
		outCount_++;
		float outWarningF = static_cast<int>(OUT_WARNING * Application::FPS);
		float outLimitF = static_cast<int>(OUT_LIMIT * Application::FPS);
		constexpr int DAMAGE_OUT = 1;
		if (outCount_ == outWarningF)
		{
			//MessageManager::GetInstance().ShowNewMessage("警告演出開始", MessageManager::DEFAULT_MESSAGE_TIME);
		}
		if (outCount_ == outWarningF + outLimitF)
		{
			MessageManager::GetInstance().ShowNewMessage("画面外ダメージ", MessageManager::DEFAULT_MESSAGE_TIME);
			Damage(DAMAGE_OUT);
			outCount_ = outWarningF+1;
		}
	}

	//移動
	Move(moveVec_);
	if (activeGlider_&& !(isGrounded_))
	{
		animState_ = ANIM::FALLING;
	}
	if (!(preIsGrounded_) && isGrounded_)
	{
		//着地アニメーション
		PlayAnim(oldAnimState_, ANIM::ONGROUND);
	}

	if (animState_ != oldAnimState_)
	{
		PlayAnim(oldAnimState_,animState_);
	}


	//更新
	oldAnimState_ = animState_;
	prePos_ = transform_.pos;
	preIsGrounded_ = isGrounded_;
}

bool Player::IsTrgDowm_Jump(const InputManager& ins)const
{
	return ins.IsTrgDown(KEY_INPUT_X) || ins.IsTrgDown(KEY_INPUT_SPACE);
}

bool Player::IsNew_Jump(const InputManager& ins)const
{
	return ins.IsNew(KEY_INPUT_X) || ins.IsNew(KEY_INPUT_SPACE);
}

bool Player::IsNew_Shot(const InputManager& ins) const
{
	return ins.IsNew(KEY_INPUT_Z);
}

void Player::Update_Shot()
{
	if (sceneGame_->IsJumpTutorial())
	{
		//無効化されるチュートリアルの場合、無効化する
		return;
	}
	int timer = sceneGame_->GetTimer();
	InputManager& iManager = InputManager::GetInstance();
	const SoundManager& sManager = SoundManager::GetInstance();
	if (IsNew_Shot(iManager))
	{
		int shotRate = SHOT_RATE;
		if (grazeBonusShot_ > 0 && isGrounded_)
		{
			//ボーナスが有効
			shotRate = SHOT_RATE_HIGH;
		}
		if (timer % shotRate == 0)
		{
			if (shotRate == SHOT_RATE_HIGH)
			{
				if (grazeBonusShot_ == GRAZE_BONUS_SHOT)
				{
					//最初の一発はエフェクトを生成
					//sceneGame_->CreateEffect(SceneGame::EFFECT_TYPE::EXPLOSION_SHOT, GetColPos(), 0.8f);
					sManager.PlaySE(SoundManager::SOUND_ID::SHOT_HIGH);
				}
				//ボーナスを消費
				grazeBonusShot_--;

			}
			sManager.PlaySE(SoundManager::SOUND_ID::SHOT);
			//生成
			sceneGame_->GetOManager()->CreateShot(ObjectManager::ShotType::SHOT_PLAYER, GetColPos(),
				Quaternion::AngleAxis(sceneManager_->GetCamera()->GetAngles().y,AsoUtility::AXIS_Y), 1);

			////3way
			//Quaternion lq = Quaternion::Mult(Quaternion::LookRotation(AsoUtility::DIR_F), Quaternion::AngleAxis(AsoUtility::Deg2RadF(-4), AsoUtility::AXIS_Y));
			//Quaternion rq = Quaternion::Mult(Quaternion::LookRotation(AsoUtility::DIR_F), Quaternion::AngleAxis(AsoUtility::Deg2RadF(4), AsoUtility::AXIS_Y));
			//VECTOR lpos = VAdd(GetColPos(), VScale(AsoUtility::DIR_L,10));
			//VECTOR rpos = VAdd(GetColPos(), VScale(AsoUtility::DIR_R,10));
			//sceneGame_->GetOManager()->CreateShot(ObjectManager::ShotType::SHOT_PLAYER, rpos, rq, 1);
			//sceneGame_->GetOManager()->CreateShot(ObjectManager::ShotType::SHOT_PLAYER, lpos, lq, 1);
		}
	}
}

void Player::Update_Count()
{
	//カウントダウン
	if (recoveryTimer_ > 0)
	{
		recoveryTimer_--;
	}
	//判定更新
	if(IsGrazed())
	{
		preGrazed_ = true;
	}
	else
	{
		preGrazed_ = false;
	}
	if (isGrounded_)
	{
		activeGlider_ = false;
		autoGliderFlag_ = false;
		stamina_ = MAX_STAMINA;
	}
	//preGrazed_の後にカウントを減らす
	grazeAnimTimer_--;

	CharacterBase::Update_Count();
}

void Player::Draw_Graze()
{
	if (grazeAnimTimer_ <= 0)
	{
		//描画なし
		return;
	}
	float rate = static_cast<float>(grazeAnimTimer_) / GRAZE_ANIM_TIME;
	//三角関数でアニメーション
	rate = sinf((DX_PI_F /2) * rate);
	if (rate < 0)
	{
		//描画なし
		return;
	}
	//ポリゴン設定
	VECTOR circlePos = GetColPos();
	VERTEX3D vertex[4]{};
	unsigned short index[6]{};
	for (auto& v : vertex)
	{
		v.pos = circlePos;
		v.u = 0;
		v.v = 0;
		v.norm = VGet(0.0f, 0.0f, -1.0f);
		v.dif = GetColorU8(255, 255, 255, static_cast<int>(255* rate));
		v.spc = GetColorU8(0, 0, 0, 0);
		v.su = 0.0f;
		v.sv = 0.0f;
	}
	//01
	//23
	vertex[0].pos = VAdd(circlePos, { -1 * GRAZE_RADIUS ,0,GRAZE_RADIUS });
	vertex[0].u = 0;
	vertex[0].v = 0;
	vertex[1].pos = VAdd(circlePos, { GRAZE_RADIUS ,0,GRAZE_RADIUS });
	vertex[1].u = 1;
	vertex[1].v = 0;
	vertex[2].pos = VAdd(circlePos, { -1 * GRAZE_RADIUS ,0,-1 * GRAZE_RADIUS });
	vertex[2].u = 0;
	vertex[2].v = 1;
	vertex[3].pos = VAdd(circlePos, { GRAZE_RADIUS ,0,-1 * GRAZE_RADIUS });
	vertex[3].u = 1;
	vertex[3].v = 1;

	//ポリゴン設定
	index[0] = 0;
	index[1] = 1;
	index[2] = 2;
	index[3] = 1;
	index[4] = 3;
	index[5] = 2;
	//描画
	DrawPolygonIndexed3D(vertex, 4, index, 2, grazeImg_, true);
}

ANIM Player::PlayAnim(ANIM curState, ANIM type, bool isLoop,
	float startStep, float endStep, bool isStop, bool isForce)
{
	ANIM ret = CharacterBase::PlayAnim(curState, type, isLoop, startStep, endStep, isStop, isForce);
	animState_ = ret;
	//curState == ANIM::MAXによって同じフレームで使用するPlayAnimにも影響を与える
	if (curState == ANIM::MAX)
	{
		oldAnimState_ = animState_;
	}
	return ret;
}

bool Player::IsRecoveryTime() const
{
	return recoveryTimer_ > 0;
}

int Player::GetBonusShot() const
{
	return grazeBonusShot_;
}

int Player::GetJumpCount() const
{
	return jumpCount_;
}

void Player::ResetStatus()
{
	CharacterBase::ResetStatus();
	grazeCombo_ = 0;
	grazeComboScore_ = 0;
	grazeBonusShot_ = 0;
	jumpCount_ = 0;
}

int Player::GetGrazeComboScore() const
{
	if (grazeComboScore_ < GetGrazeCombo())
	{
		return GetGrazeCombo();
	}
	return grazeComboScore_;
}

int Player::GetGrazeCombo() const
{
	return grazeCombo_;
}


void Player::SetMaterialSetting()
{
	int MaterialNum = MV1GetMaterialNum(transform_.modelId);
	for (int i = 0; i < MaterialNum; i++) {
		float DotWidth = MV1GetMaterialOutLineDotWidth(transform_.modelId, i);
		MV1SetMaterialOutLineWidth(transform_.modelId, i, 0.0f);
		MV1SetMaterialOutLineDotWidth(transform_.modelId, i, 0.0f);
		MV1SetMaterialOutLineColor(transform_.modelId, i, GetColorF(0.0f, 0.0f, 0.0f, 1.0f));
	}
}

bool Player::IsGrazed()
{
	//ヒットし続けている状態を検知。タイマーが1でも進むと範囲外になる。
	return grazeAnimTimer_ >= GRAZE_ANIM_TIME;
}

void Player::RecordCombo()
{
	if (grazeCombo_ > grazeComboScore_)
	{
		//記録更新
		grazeComboScore_ = grazeCombo_;
	}
}

bool Player::StaminaCount(void)
{
	stamina_--;
	if(stamina_ < 0)
	{ 
		stamina_ = 0;
		activeGlider_ = false;
	}

	return stamina_;
}
