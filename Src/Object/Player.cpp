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

	/// <summary>
	/// チュートリアル完了と判定する進行度
	/// </summary>
	constexpr float TUTORIAL_COMPLETE_PROGRESS = 1.0f;

	/// <summary>
	/// 残機がなくなる判定値
	/// </summary>
	constexpr int NO_LIFE = 0;

	/// <summary>
	/// プレイヤーの初期向き
	/// </summary>
	constexpr float PLAYER_INITIAL_ROTATION_DEG = 180.0f;

	/// <summary>
	/// 画面外カウントの初期値
	/// </summary>
	constexpr int OUT_COUNT_INITIAL = 0;

	/// <summary>
	/// 移動方向のX軸最小値
	/// </summary>
	constexpr float MOVE_DIRECTION_LEFT = -1.0f;

	/// <summary>
	/// 移動方向のX軸最大値
	/// </summary>
	constexpr float MOVE_DIRECTION_RIGHT = 1.0f;

	/// <summary>
	/// 移動方向のZ軸前方
	/// </summary>
	constexpr float MOVE_DIRECTION_FORWARD = 1.0f;

	/// <summary>
	/// 移動方向のZ軸後方
	/// </summary>
	constexpr float MOVE_DIRECTION_BACK = -1.0f;

	/// <summary>
	/// 掠り演出の角度
	/// </summary>
	constexpr float GRAZE_ANIMATION_ANGLE = DX_PI_F / 2.0f;

	/// <summary>
	/// 掠り演出の最大アルファ値
	/// </summary>
	constexpr int GRAZE_ALPHA_MAX = 255;

	/// <summary>
	/// 掠り演出ポリゴンの頂点数
	/// </summary>
	constexpr int GRAZE_VERTEX_COUNT = 4;

	/// <summary>
	/// 掠り演出ポリゴンのインデックス数
	/// </summary>
	constexpr int GRAZE_INDEX_COUNT = 6;

	/// <summary>
	/// 掠り演出のポリゴン数
	/// </summary>
	constexpr int GRAZE_POLYGON_COUNT = 2;

	/// <summary>
	/// テクスチャ座標の開始値
	/// </summary>
	constexpr float TEXTURE_COORD_MIN = 0.0f;

	/// <summary>
	/// テクスチャ座標の終了値
	/// </summary>
	constexpr float TEXTURE_COORD_MAX = 1.0f;

	/// <summary>
	/// 頂点法線のZ方向
	/// </summary>
	constexpr float GRAZE_NORMAL_Z = -1.0f;

	/// <summary>
	/// マテリアルのアウトライン幅
	/// </summary>
	constexpr float OUTLINE_WIDTH_NONE = 0.0f;

	/// <summary>
	/// マテリアル色の最小値
	/// </summary>
	constexpr float MATERIAL_COLOR_MIN = 0.0f;

	/// <summary>
	/// マテリアルのアルファ値最大値
	/// </summary>
	constexpr float MATERIAL_ALPHA_MAX = 1.0f;

	/// <summary>
/// 画面外ダメージ後に再判定を開始するための加算値
/// </summary>
	constexpr int OUT_COUNT_RESTART_OFFSET = 1;
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
	recoveryTimer_ = INITIAL_TIMER;
	respawnPos_ = {};
	prePos_ = {};
	grazeAnimTimer_ = INITIAL_TIMER;
	preGrazed_ = false;
	lifeMax_ = INITIAL_LIFE;
	life_ = lifeMax_;
	grazeImg_ = INVALID_GRAPH_HANDLE;
	grazeBonusShot_ = INITIAL_TIMER;
	lastJumpPosY_ = 0;
	jumpCount_ = INITIAL_TIMER;
	grazeCombo_ = INITIAL_TIMER;
	grazeComboScore_ = INITIAL_TIMER;
	outCount_ = OUT_COUNT_INITIAL;
	sprintFlag_ = false;
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
}

void Player::Release()
{
	if (grazeImg_ != INVALID_GRAPH_HANDLE)
	{
		DeleteGraph(grazeImg_);
		grazeImg_ = INVALID_GRAPH_HANDLE;
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
		grazeBonusShot_ = INITIAL_TIMER;
		//コンボを減少
		grazeCombo_--;
		//減少後にコンボを記録
		RecordCombo();
		//コンボをリセット
		grazeCombo_ = INITIAL_TIMER;
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
	if (sceneGame_->IsGrazeTutorial() && sceneGame_->GetGrazeTutorialProgress() < TUTORIAL_COMPLETE_PROGRESS)
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
	if (GetLife() <= NO_LIFE)
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
	recoveryTimer_ = static_cast<int>(Application::FPS * RECOVERY_TIME_S);
	//リセット
	grazeBonusShot_ = INITIAL_TIMER;
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
	if (grazeImg_ == INVALID_GRAPH_HANDLE)
	{
		grazeImg_ = LoadGraph((Application::PATH_IMAGE + "Circle.png").c_str());
	}

	hpMax_ = PLAYER_MAX_HP;
	hp_ = hpMax_;

	lifeMax_ = PLAYER_MAX_LIFE;
	life_ = lifeMax_;

	stamina_ = MAX_STAMINA;
	staminamax = MAX_STAMINA;

	transform_.quaRotLocal = Quaternion::Euler(
		{ 0, AsoUtility::DegToRadF(PLAYER_INITIAL_ROTATION_DEG), 0 }
	);

	respawnPos_ = { 0, 0, SceneGame::PLAYER_START_Z };
	transform_.pos = respawnPos_;
	prePos_ = transform_.pos;

	colliderSize_ = DEFAULT_COLLIDERSIZE;
	colliderRadiusShot_ = SHOT_COLLIDER_RADIUS;

	sprintFlag_ = true;

	transform_.quaRotLocal = Quaternion::AngleAxis(
		AsoUtility::Deg2RadF(PLAYER_INITIAL_ROTATION_DEG),
		AsoUtility::AXIS_Y
	);

	// コライダーを少し浮かせる
	capsule_->SetLocalPosDown(
		{ 0, DEFAULT_COLLIDERSIZE + COLLIDER_Y_ADD, 0 }
	);
	capsule_->SetLocalPosTop({ 0, HEAD_POS, 0 });
	capsule_->SetRadius(DEFAULT_COLLIDERSIZE);
}

void Player::Update_Move()
{
	//リセット
	animState_ = ANIM::IDLE;

	auto& ins = InputManager::GetInstance();
	
#pragma region MoveKey
	//移動方向
	VECTOR dir = AsoUtility::VECTOR_ZERO;

	if (ins.IsNew(KEY_INPUT_UP))
	{
		dir = VAdd(dir, { 0, 0, MOVE_DIRECTION_FORWARD });
	}
	if (ins.IsNew(KEY_INPUT_DOWN))
	{
		dir = VAdd(dir, { 0, 0, MOVE_DIRECTION_BACK });
	}
	if (ins.IsNew(KEY_INPUT_LEFT))
	{
		dir = VAdd(dir, { MOVE_DIRECTION_LEFT, 0, 0 });
	}
	if (ins.IsNew(KEY_INPUT_RIGHT))
	{
		dir = VAdd(dir, { MOVE_DIRECTION_RIGHT, 0, 0 });
	}
	if (ins.IsTrgDown(KEY_INPUT_LSHIFT))
	{
	}

	if (sceneGame_->IsJumpTutorial())
	{
		//移動禁止中の場合無効化
		dir.x = OUT_COUNT_INITIAL;
		dir.z = OUT_COUNT_INITIAL;
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
			if ((transform_.pos.y - lastJumpPosY_ >= GLIDER_MIN_Y || jumpPow_.y < 0) && activeGlider_ == false)
			{
				//上昇を消す
				if (jumpPow_.y > 0)
				{
					jumpPow_.y = OUT_COUNT_INITIAL;
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
		mat = MMult(mat, MGetRotY(cameraAngles.y));

		//カメラの回転を適用
		VECTOR moveDir = VTransform(dir, mat);

		float moveAngle = atan2f(moveDir.x, moveDir.z);

		//プレイヤー移動方向に回転を設定

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
	}
	else
	{
		moveVec_.x = OUT_COUNT_INITIAL;
		moveVec_.z = OUT_COUNT_INITIAL;
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
		outCount_ = OUT_COUNT_INITIAL;
	}
	else
	{
		outCount_++;
		int outWarningF = static_cast<int>(OUT_WARNING * Application::FPS);
		int outLimitF = static_cast<int>(OUT_LIMIT * Application::FPS);
		constexpr int DAMAGE_OUT = 1;
		if (outCount_ == outWarningF)
		{
		}
		if (outCount_ == outWarningF + outLimitF)
		{
			MessageManager::GetInstance().ShowNewMessage("画面外ダメージ", MessageManager::DEFAULT_MESSAGE_TIME);
			Damage(DAMAGE_OUT);
			outCount_ = outWarningF + OUT_COUNT_RESTART_OFFSET;
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
		if (timer % shotRate == INITIAL_TIMER)
		{
			if (shotRate == SHOT_RATE_HIGH)
			{
				if (grazeBonusShot_ == GRAZE_BONUS_SHOT)
				{
					//最初の一発はエフェクトを生成
					sManager.PlaySE(SoundManager::SOUND_ID::SHOT_HIGH);
				}
				//ボーナスを消費
				grazeBonusShot_--;

			}
			sManager.PlaySE(SoundManager::SOUND_ID::SHOT);
			//生成
			sceneGame_->GetOManager()->CreateShot(ObjectManager::ShotType::SHOT_PLAYER, GetColPos(),
				Quaternion::AngleAxis(sceneManager_->GetCamera()->GetAngles().y,AsoUtility::AXIS_Y), 1);
		}
	}
}

void Player::Update_Count()
{
	//カウントダウン
	if (recoveryTimer_ > INITIAL_TIMER)
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
	if (grazeAnimTimer_ <= OUT_COUNT_INITIAL)
	{
		//描画なし
		return;
	}
	float rate = static_cast<float>(grazeAnimTimer_) / GRAZE_ANIM_TIME;
	//三角関数でアニメーション
	rate = sinf(GRAZE_ANIMATION_ANGLE * rate);
	if (rate < OUT_COUNT_INITIAL)
	{
		//描画なし
		return;
	}
	//ポリゴン設定
	VECTOR circlePos = GetColPos();
	VERTEX3D vertex[GRAZE_VERTEX_COUNT]{};
	unsigned short index[GRAZE_INDEX_COUNT]{};
	for (auto& v : vertex)
	{
		v.pos = circlePos;
		v.u = TEXTURE_COORD_MIN;
		v.v = TEXTURE_COORD_MIN;
		v.norm = VGet(0.0f, 0.0f, GRAZE_NORMAL_Z);
		v.dif = GetColorU8(
			GRAZE_ALPHA_MAX,
			GRAZE_ALPHA_MAX,
			GRAZE_ALPHA_MAX,
			static_cast<int>(GRAZE_ALPHA_MAX * rate)
		);
		v.spc = GetColorU8(
			OUT_COUNT_INITIAL,
			OUT_COUNT_INITIAL,
			OUT_COUNT_INITIAL,
			OUT_COUNT_INITIAL
		);
		v.su = TEXTURE_COORD_MIN;
		v.sv = TEXTURE_COORD_MIN;
	}
	//01
	//23
	vertex[0].pos = VAdd(
		circlePos,
		{ -1 * GRAZE_RADIUS, 0, GRAZE_RADIUS }
	);
	vertex[0].u = TEXTURE_COORD_MIN;
	vertex[0].v = TEXTURE_COORD_MIN;

	vertex[1].pos = VAdd(
		circlePos,
		{ GRAZE_RADIUS, 0, GRAZE_RADIUS }
	);
	vertex[1].u = TEXTURE_COORD_MAX;
	vertex[1].v = TEXTURE_COORD_MIN;

	vertex[2].pos = VAdd(
		circlePos,
		{ -1 * GRAZE_RADIUS, 0, -1 * GRAZE_RADIUS }
	);
	vertex[2].u = TEXTURE_COORD_MIN;
	vertex[2].v = TEXTURE_COORD_MAX;

	vertex[3].pos = VAdd(
		circlePos,
		{ GRAZE_RADIUS, 0, -1 * GRAZE_RADIUS }
	);
	vertex[3].u = TEXTURE_COORD_MAX;
	vertex[3].v = TEXTURE_COORD_MAX;

	//ポリゴン設定
	index[0] = 0;
	index[1] = 1;
	index[2] = 2;
	index[3] = 1;
	index[4] = 3;
	index[5] = 2;
	//描画
	DrawPolygonIndexed3D(vertex, GRAZE_VERTEX_COUNT, index, GRAZE_POLYGON_COUNT, grazeImg_, true);
}

ANIM Player::PlayAnim(ANIM curState, ANIM type, bool isLoop,
	float startStep, float endStep, bool isStop, bool isForce)
{
	ANIM ret = CharacterBase::PlayAnim(curState, type, isLoop, startStep, endStep, isStop, isForce);
	animState_ = ret;
	if (curState == ANIM::MAX)
	{
		oldAnimState_ = animState_;
	}
	return ret;
}

bool Player::IsRecoveryTime() const
{
	return recoveryTimer_ > INITIAL_TIMER;
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

	for (int i = 0; i < MaterialNum; i++)
	{
		float DotWidth =
			MV1GetMaterialOutLineDotWidth(transform_.modelId, i);

		MV1SetMaterialOutLineWidth(
			transform_.modelId,
			i,
			OUTLINE_WIDTH_NONE
		);

		MV1SetMaterialOutLineDotWidth(
			transform_.modelId,
			i,
			OUTLINE_WIDTH_NONE
		);

		MV1SetMaterialOutLineColor(
			transform_.modelId,
			i,
			GetColorF(
				MATERIAL_COLOR_MIN,
				MATERIAL_COLOR_MIN,
				MATERIAL_COLOR_MIN,
				MATERIAL_ALPHA_MAX
			)
		);
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
	if(stamina_ < OUT_COUNT_INITIAL)
	{ 
		stamina_ = OUT_COUNT_INITIAL;
		activeGlider_ = false;
	}

	return stamina_;
}
