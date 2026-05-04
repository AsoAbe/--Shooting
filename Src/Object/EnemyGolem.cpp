#include <math.h>
#include <iostream>
#include <thread>
#include <chrono>
#include "../Application.h"
#include "../Common/AsoUtility.h"
#include "../Common/DrawUtility.h"
#include "../Scene/SceneGame.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/ObjectManager.h"
#include "../Common/IntVector3.h"
#include "../Common/Capsule.h"
#include "../Common/Collider.h"
#include "../Manager/Camera.h"
#include "../Manager/MessageManager.h"
#include "../Manager/ObjectManager.h"
#include "../Object/Player.h"
#include "../Common/AnimationController.h"
#include "EnemyGolem.h"

//NONE,
//DASH_START,		//突進予備動作
//DASH,			//突進
//RETURN,			//初期位置に戻る
//RANDOM,			//周囲ランダム
//RANDOM_MOVING,	//動きながら全方向
//AROUND,			//時計回りにずらしながら全方向
//AROUND_MOVING	//動きながら全方向

//攻撃に合わせたアニメーション

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


EnemyGolem::EnemyGolem(SceneGame* parent, const CharacterModelData& modelData) :StandardBoss(parent,modelData)
{

	attackUpdate_ = &EnemyGolem::AttackUpdate_Shot;
	sceneGame_ = parent;
	counter_ = 0;
	changeTime_ = 0;
	state_ = ATTACK_STATE::AROUND;
	nextState_ = ATTACK_STATE::AROUND;
	moveState_ = MOVE_STATE::R;
	stateChangeCount_ = 0;
	lastCount_ = 0;
	dashVec_ = { 0,0,1 };
	dashCountEnd_ = 0;
	dashCount_ = 0;
	dashChangeVecFlag_ = false;
	lastPos_ = {};
	returnPos_ = {};
}

EnemyGolem::~EnemyGolem(void)
{
}

void EnemyGolem::Draw()
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
	
#ifdef _DEBUG
	capsule_->Draw();
#endif
}

void EnemyGolem::Update_Move()
{
	VECTOR targetPos = sceneGame_->GetOManager()->GetPlayer()->GetPos();
	
	VECTOR Dir_ = VSub(targetPos , transform_.pos);
	transform_.quaRot =Quaternion::Euler(0, atan2(Dir_.x, Dir_.z),0); 

	switch (state_)
	{
	case StandardBoss::ATTACK_STATE::POWER_CHARGING:
	case StandardBoss::ATTACK_STATE::DASH:
		//animationController_->Play(ANIM::RUN);
	break;
	case StandardBoss::ATTACK_STATE::RANDOM_MOVING:
	case StandardBoss::ATTACK_STATE::RANDOM:
	case ATTACK_STATE::AROUND_MOVING:
	case ATTACK_STATE::AROUND:
		
		//std::this_thread::sleep_for(std::chrono::milliseconds(500));
		animationController_->Play(ANIM::ATTACK3);

		break;
	default:
		break;
	}
}

void EnemyGolem::AttackUpdate_Dash()
{
	animationController_->Play(ANIM::RUN);
	StandardBoss::AttackUpdate_Dash();
}
void EnemyGolem::AttackUpdate_Return()
{
	//プレイヤーの方向を向きつつ直立する
	animationController_->Play(ANIM::IDLE);

	VECTOR targetPos = sceneGame_->GetOManager()->GetPlayer()->GetPos();

	VECTOR Dir_ = VSub(targetPos, transform_.pos);
	transform_.quaRot = Quaternion::Euler(0, atan2(Dir_.x, Dir_.z), 0);
	StandardBoss::AttackUpdate_Return();
}
void EnemyGolem::AttackUpdate_PowerCharging()
{
	//プレイヤー方向を向く
	VECTOR targetPos = sceneGame_->GetOManager()->GetPlayer()->GetPos();

	VECTOR Dir_ = VSub(targetPos, transform_.pos);
	transform_.quaRot = Quaternion::Euler(0, atan2(Dir_.x, Dir_.z), 0);
	StandardBoss::AttackUpdate_PowerCharging();
}
void EnemyGolem::DashTurn(const VECTOR& diffXZ)
{
	transform_.quaRot = Quaternion::Euler(0, atan2(diffXZ.x, diffXZ.z), 0);
	StandardBoss::DashTurn(diffXZ);
}
void EnemyGolem::ChangeState_PowerCharging(const VECTOR& diff)
{
	animationController_->Play(ANIM::IDLE);
	StandardBoss::ChangeState_PowerCharging(diff);

}

void EnemyGolem::Died()
{
	if (sceneGame_->IsTutorial())
	{
		//チュートリアル中は死なない
		return;
	}
	animationController_->Play(ANIM::DOWN,false);
	died_ = true;
	//SetActive(false);
}
//Vector2 Ap_ = AngPos_.ToVector2();
//
//Vector2 Mp = sceneGame_->GetMousePos();
//
//Position2 targetPos(Mp.x, Mp.y);
//Vector2 lookAt = targetPos - Ap_;
//Angle = atan2(lookAt.y, lookAt.x);   //マウスの方向に回転

//
//void EnemyGolem::Mv1Scale()
//{
//	// モデルを2倍の大きさにする
//	MV1SetScale(modelId_, VGet(2.0f, 2.0f, 2.0f));
//}
