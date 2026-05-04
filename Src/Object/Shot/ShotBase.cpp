#include "../../Common/AsoUtility.h"
#include "../../Application.h"
#include "../../Scene/SceneGame.h"
#include "../../Manager/ObjectManager.h"
#include "../../Common/ObjectModelData.h"
#include "ShotBase.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

ShotBase::ShotBase(SceneGame* parent, const ObjectModelData& modelData):ObjectBase(modelData)
{
	shotTimer_ = 0;
	isPlayerTag_ = false;
	sceneGame_ = parent;
	speed_ = 1;
	colRadius_ = 1;
	damage_ = DEFAULT_DAMAGE;
	isActive_ = false;
}

ShotBase::~ShotBase(void)
{
}

void ShotBase::Init()
{
	SetParam();
	modelId_ = MV1DuplicateModel(baseModel_.model_);
	//マトリクスでモデルを制御
	SetMatrixModel();
}

void ShotBase::Update()
{
	if (!isActive_)
	{
		return;
	}
	Move();
	if (shotTimer_ > 0)
	{
		shotTimer_ -= 1.0f/Application::FPS;
	}
	else
	{
		//自然消滅
		DeactivateShot();
	}
	//地形に沿う
	SetPosOnGround();
	//親クラスの処理
	ObjectBase::Update();
}

void ShotBase::Activation(VECTOR startPos, Quaternion quaRot, float size)
{
	//タイマーリセットを兼ねて
	SetParam();

	pos_ = startPos; 
	quaRot_ = quaRot;
	scale_ = VScale(AsoUtility::VECTOR_ONE,size);
	//modelScale_ = 3;
	//マトリクスでモデルを制御
	SetMatrixModel();
	//アクティブ化
	isActive_ = true;
}

void ShotBase::DrawCol()
{
	if (IsPlayerTag())
	{
		DrawSphere3D(pos_, GetColRadius(), 8, 0x0000FF, 0xFFFFFF, false);
	}
	else
	{
		DrawSphere3D(pos_, GetColRadius(), 8, 0xFF0000, 0xFFFFFF, false);
	}
}

void ShotBase::Move()
{
	//直進
	pos_ = VAdd(pos_, VScale(quaRot_.GetForward(), speed_));
}

void ShotBase::SetPosOnGround()
{
	//地形に沿う
	constexpr float LINE_LENGTH = 500;
	constexpr float POS_Y = 100;
	float hitY = sceneGame_->GetOManager()->GetOnGroundY(GetPos(), LINE_LENGTH, LINE_LENGTH);
	if (hitY > GetPos().y - LINE_LENGTH)
	{
		//地面が見つかったらyを地形に沿わせる
		pos_.y = hitY + POS_Y;
	}
}

const type_info& ShotBase::GetShotType(void)const
{
	return typeid(*this);
}

void ShotBase::Hit()
{
	if (!isActive_)
	{
		return;
	}
	isActive_ = false;
}

float ShotBase::GetColRadius()const
{
	return colRadius_* scale_.y;
}

bool ShotBase::IsPlayerTag()const
{
	return isPlayerTag_;
}

void ShotBase::DeactivateShot()
{
	if (IsActive())
	{
		sceneGame_->CreateEffect(SceneGame::EFFECT_TYPE::EXPLOSION_Y , pos_, 0.5f);
		SetActive(false);
	}
}

int ShotBase::GetDamage() const
{
	return damage_;
}
