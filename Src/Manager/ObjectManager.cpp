#include "ObjectManager.h"
#include "../Application.h"
#include "./SceneManager.h"
#include "./MessageManager.h"
#include "./Camera.h"
#include "../Scene/SceneGame.h"
#include "../Object/Map.h"
#include "../Object/Player.h"
#include "../Object/StandardBoss.h"
#include "../Object/EnemyGolem.h"
#include "../Object/TestCharacter.h"
#include "../Object/TestObject.h"
#include "../Object/Shot/PlayerShot.h"
#include "../Object/Shot/StandardShot.h"
#include "../Object/Shot/LowDamageShot.h"
#include "../Object/Shot/TestShot.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

ObjectManager::ObjectManager(void)
{
	shadowGraph_ = -1;
	sceneGame_ = nullptr;
	player_ = nullptr;
	map_ = nullptr;
	characters_.clear();
	objects_.clear();
	shotObj_.clear();
}

bool ObjectManager::Init(SceneGame* sceneGame)
{
	bool ret = true;
	sceneGame_ = sceneGame;

	shadowGraph_ = LoadGraph((Application::PATH_IMAGE + "Shadow.png").c_str());

	LoadPlayerModel();
	LoadTestModel();
	LoadGolemModel();

	CreatePlayer();
	
	CharacterBase* enemygolem = new EnemyGolem(sceneGame_, golemModel_);
	enemygolem->Init();

	/*CharacterBase* testC = new StandardBoss (sceneGame_, golemModel_);
	testC->Init();*/
	AddCharacter(enemygolem, {0,0,200});

	//ObjectBase* testO = new TestObject(sceneGame_);
	//testO->Init();
	//AddObject(testO, { 200,0,200 });

	map_ = new Map(this);
	map_->Init();

	//shotModelId_ = MV1LoadModel((Application::PATH_MODEL + "Coin.mv1").c_str());
	//shotModelId_ = MV1LoadModel((Application::PATH_MODEL + "Crystal3.mv1").c_str());
	//rockModelId_ = MV1LoadModel((Application::PATH_MODEL + "Golem/rock.mv1").c_str());
	shotModel_.modelFileName_ = ("御札/ofuda.mv1");
	shotModel_.modelScale_ = 0.4f;
	shotModel_.modelRot_ = VGet(AsoUtility::Deg2RadF(0), AsoUtility::Deg2RadF(0), AsoUtility::Deg2RadF(0));
	shotModel_.LoadModel();

	rockModel_.modelFileName_ = ("Golem/rock.mv1");
	rockModel_.modelScale_ = 3;
	rockModel_.LoadModel();


	return ret;
}

void ObjectManager::Update(void)
{
	//if (map_ != nullptr)
	//{
	//	//必要なら関数を作成する
	//	map_->Update();
	//}
	for (ObjectBase* o : objects_)
	{
		o->Update();
	}
	if (player_ != nullptr)
	{
		player_->Update();
	}
	for (CharacterBase* c : characters_)
	{
		c->Update();
	}

	for (ShotBase* o : shotObj_)
	{
		o->Update();
	}

	//衝突判定
	Collision();
}

void ObjectManager::Draw(void)
{
	if (map_ != nullptr)
	{
		map_->DrawMap();
	}
	/*if (enemygolem_ != nullptr)
	{
		enemygolem_->Draw();
	}*/
	for (ObjectBase* o : objects_)
	{
		o->Draw();
	}

	for (ShotBase* o : shotObj_)
	{
		o->Draw();
#ifdef _DEBUG
		o->DrawCol();
#endif
	}

	if (player_ != nullptr)
	{
		//画面外判定
		player_->SetIsOut(CheckCameraViewClip(player_->GetPos()) == TRUE);
		player_->Draw();
		DrawShadow(player_->GetPos());
#ifdef _DEBUG
		DrawSphere3D(player_->GetColPos(), player_->GetColRadiusShot(), 8, 0xff0000, 0xffffff, false);
		DrawSphere3D(player_->GetColPos(), player_->GetGrazeRadius(), 8, 0xff8800, 0xffffff, false);
#endif
	}
	for (CharacterBase* c : characters_)
	{
		c->Draw();
		DrawShadow(c->GetPos());
#ifdef _DEBUG
		DrawSphere3D(c->GetColPos(), c->GetColSize(), 8, 0xff0000, 0xffffff, false);
#endif
	}

#ifdef _DEBUG
	//テスト用
	DrawFormatString(0, 16*4+8, 0xffffff, "shotObj_ = (%d)", shotObj_.size());
#endif
}

void ObjectManager::Release(void)
{

	//インスタンス解放
	if (map_ != nullptr)
	{
		map_->Release();
		delete map_;
		map_ = nullptr;
	}
	//if (enemygolem_ != nullptr)
	//{
	//	enemygolem_->Release();
	//	delete enemygolem_;
	//	enemygolem_ = nullptr;
	//}
	if (player_ != nullptr)
	{
		player_->Release();
		delete player_;
		player_ = nullptr;
	}
	for (CharacterBase* c : characters_)
	{
		c->Release();
		delete c;
	}
	characters_.clear();
	for (ObjectBase* o : objects_)
	{
		o->Release();
		delete o;
	}
	objects_.clear();
	for (ShotBase* o : shotObj_)
	{
		o->Release();
		delete o;
	}
	shotObj_.clear();

	//ベースモデル解放
	//DeleteGraph(shadowGraph_);
	//shadowGraph_ = -1;
	//MV1DeleteModel(shotModelId_);
	//shotModelId_ = -1;

	playerModel_.ReleaseModel();
	testModel_.ReleaseModel();
	golemModel_.ReleaseModel();
}

void ObjectManager::LoadPlayerModel()
{
	static constexpr int JUMP_ANIMSPD = CharacterModelData::DEFAULT_ANIMSPD * 2;
	playerModel_.modelScale_ = 10;
	//ファイル名を指定
	playerModel_.modelFileName_ = "博麗霊夢/reimu.mv1";
	playerModel_.SetAnimPath(ANIM::IDLE, "博麗霊夢/reimu.mv1",1);
	playerModel_.SetAnimPath(ANIM::RUN, "博麗霊夢/reimu_run.mv1",1);
	playerModel_.SetAnimPath(ANIM::JUMP, "博麗霊夢/reimu_junp.mv1", 1, JUMP_ANIMSPD);
	playerModel_.SetAnimPath(ANIM::FALLING, "博麗霊夢/reimu_junp.mv1", 1);
	playerModel_.SetAnimPath(ANIM::ONGROUND, "博麗霊夢/reimu_junp.mv1", 1, JUMP_ANIMSPD);
	playerModel_.SetJumpOption(24, 26, 50);
	//ロードしてハンドルに
	playerModel_.LoadModel();
}

void ObjectManager::LoadTestModel()
{
	static constexpr int JUMP_ANIMSPD = CharacterModelData::DEFAULT_ANIMSPD * 2;
	testModel_.modelScale_ = 1;
	//ファイル名を指定
	testModel_.modelFileName_ = "HumanModel/Human.mv1";
	testModel_.SetAnimPath(ANIM::IDLE, "HumanModel/Human.mv1",1);
	testModel_.SetAnimPath(ANIM::RUN, "HumanModel/FastRun.mv1",1);
	testModel_.SetAnimPath(ANIM::JUMP, "HumanModel/Jumping.mv1",0, JUMP_ANIMSPD);
	testModel_.SetAnimPath(ANIM::FALLING, "HumanModel/Jumping.mv1", 0);
	testModel_.SetAnimPath(ANIM::ONGROUND, "HumanModel/Jumping.mv1", 0, JUMP_ANIMSPD);
	testModel_.SetJumpOption(24, 26, 34);
	//ロードしてハンドルに
	testModel_.LoadModel();
}

void ObjectManager::LoadGolemModel()
{
	golemModel_.modelScale_ = 1.2f;
	//ファイル名を指定
	golemModel_.modelFileName_ = "Golem/Golem.mv1";
	golemModel_.SetAnimPath(ANIM::IDLE, "Golem/ANIM_Neutral.mv1", 0, CharacterModelData::DEFAULT_ANIMSPD * 2);
	golemModel_.SetAnimPath(ANIM::RUN, "Golem/Dash.mv1", 1, CharacterModelData::DEFAULT_ANIMSPD * 2);
	golemModel_.SetAnimPath(ANIM::ATTACK1, "Golem/Anim_RightAttack.mv1", 0, CharacterModelData::DEFAULT_ANIMSPD * 2);
	golemModel_.SetAnimPath(ANIM::ATTACK2, "Golem/Anim_LeftAttack.mv1", 0, CharacterModelData::DEFAULT_ANIMSPD * 2);
	golemModel_.SetAnimPath(ANIM::ATTACK3, "Golem/Anim_RamdomAttack.mv1", 0, CharacterModelData::DEFAULT_ANIMSPD * 2);
	golemModel_.SetAnimPath(ANIM::DOWN, "Golem/Down.mv1", 0, CharacterModelData::DEFAULT_ANIMSPD * 2);
	golemModel_.SetJumpOption(24, 26, 34);
	//ロードしてハンドルに
	golemModel_.LoadModel();

}

Player* ObjectManager::GetPlayer(void)
{
	return player_;
}

Map* ObjectManager::GetMap(void)
{
	return map_;
}


void ObjectManager::AddCharacter(CharacterBase* c,VECTOR pos)
{
	for (CharacterBase* listC : characters_)
	{
		if (c == listC)
		{
			//既に同じものが含まれている
			return;
		}
	}
	c->SetPos(pos);
	//配列に追加
	characters_.push_back(c);
}
void ObjectManager::DestroyCharacter(CharacterBase* c)
{
	int target = -1;
	for (int i = 0; i < characters_.size(); i++)
	{
		if (characters_[i] == c)
		{
			target = i;
			break;
		}
	}
	//対象を見つけた
	if (target != -1)
	{
		characters_[target]->Release();
		delete characters_[target];
		characters_.erase(characters_.begin() + target);
	}
}

void ObjectManager::AddObject(ObjectBase* o, VECTOR pos)
{
	for (ObjectBase* listO : objects_)
	{
		if (o == listO)
		{
			//既に同じものが含まれている
			return;
		}
	}
	o->SetPos(pos);
	//配列に追加
	objects_.push_back(o);
}

void ObjectManager::DestroyObject(ObjectBase* o)
{
	int target = -1;
	for (int i = 0; i < objects_.size(); i++)
	{
		if (objects_[i] == o)
		{
			target = i;
			break;
		}
	}
	//対象を見つけた
	if (target != -1)
	{
		objects_[target]->Release();
		delete objects_[target];
		objects_.erase(objects_.begin() + target);
	}
}

void ObjectManager::AddShotObj(ShotBase* o, VECTOR pos,Quaternion quaRot,float size)
{
	for (ObjectBase* listO : objects_)
	{
		if (o == listO)
		{
			//既に同じものが含まれている
			return;
		}
	}
	o->Activation(pos, quaRot, size);
	//配列に追加
	shotObj_.push_back(o);
}

bool ObjectManager::ReplaceShotObj(ShotBase* o, VECTOR pos, Quaternion quaRot, float size)
{
	//探す
	int index = FindNotActiveShot();
	if (index == -1)
	{
		return false;
	}
	//非アクティブな場所から消して置き換える
	shotObj_[index]->Release();
	delete shotObj_[index];
	shotObj_[index] = o;
	//アクティブ化
	ActivateShot(index, pos, quaRot, size);
	return true;
}

int ObjectManager::FindNotActiveShot(const type_info& type)
{
	for (int i = 0; i < shotObj_.size(); i++)
	{
		if (shotObj_[i]->GetShotType() == type && !(shotObj_[i]->IsActive()))
		{
			//見つけた
			return i;
		}
	}
	return -1;
}

int ObjectManager::FindNotActiveShot()
{
	for (int i = 0; i < shotObj_.size(); i++)
	{
		if (!(shotObj_[i]->IsActive()))
		{
			//見つけた
			return i;
		}
	}
	return -1;
}

void ObjectManager::CreateShot(ShotType type, VECTOR pos, Quaternion quaRot, float size)
{
	int target = -1;
	ShotBase* ins = nullptr;
	switch (type)
	{
	case ObjectManager::ShotType::TEST:
		target = FindNotActiveShot(typeid(TestShot));
		if (target == -1)
		{
			ins = new TestShot(sceneGame_, rockModel_);
			ins->Init();
		}
		break;

	case ObjectManager::ShotType::SHOT_PLAYER:
		target = FindNotActiveShot(typeid(PlayerShot));
		if (target == -1)
		{
			ins = new PlayerShot(sceneGame_, shotModel_);
			ins->Init();
		}
		break;
	case ObjectManager::ShotType::SHOT_LOW:
		target = FindNotActiveShot(typeid(LowDamageShot));
		if (target == -1)
		{
			ins = new LowDamageShot(sceneGame_, rockModel_);
			ins->Init();
		}
		break;

	case ObjectManager::ShotType::SHOT:
	default:
		target = FindNotActiveShot(typeid(StandardShot));
		if (target == -1)
		{
			ins = new StandardShot(sceneGame_, rockModel_);
			ins->Init();
			//MV1SetScale(rockModelId_, VGet(1.5f, 1.5f, 1.5f));
		}
		break;
	}
	if (target != -1)
	{
		//再アクティブ化
		ActivateShot(target, pos, quaRot, size);
		return;
	}
	if (shotObj_.size()< INSTANCE_MAX)
	{
		//上限まで配列に追加
		AddShotObj(ins, pos, quaRot, size);
	}
	else
	{
		//タイプ関係なしに非アクティブを探して置き換える
		if (ReplaceShotObj(ins,pos,quaRot,size) == false)
		{
			//失敗したらinsをデリート
			ins->Release();
			delete ins;
			ins = nullptr;
		}
	}
}

void ObjectManager::ActivateShot(int index, VECTOR pos, Quaternion quaRot, float size)
{
	if (shotObj_.size() <= index || index <0)
	{
		return;
	}
	shotObj_[index]->Activation(pos, quaRot, size);
}

void ObjectManager::DeactivateAllShot()
{
	for (auto& shot : shotObj_)
	{
		//すべて非アクティブ化
		shot->DeactivateShot();
	}
}

void ObjectManager::DrawShadow(const VECTOR& pos,float radius)
{
	constexpr float LINE_LENGTH_DOWN = 1000;
	constexpr float LINE_LENGTH_UP = 40;
	constexpr int SHADOW_ALPHA = 114; //52
	//三角形を表す
	constexpr int POLYGON = 3;
	//上に上げる
	constexpr float SHADOW_Y = 4;
	//地面判定の分割
	constexpr int LOOP = 16;
	//地面判定の1回あたりの角度
	constexpr float LOOP_ANGLE = 360.0f / LOOP;
	//影を映す地面の存在判定
	bool isHit = false;
	//足元座標を探す
	float shadowY = -1 * LINE_LENGTH_DOWN;

	//posのyを地面に合わせる
	VECTOR centerPos = pos;
	centerPos.y = GetOnGroundY(pos, LINE_LENGTH_UP, LINE_LENGTH_DOWN);
	isHit = centerPos.y > (pos.y- LINE_LENGTH_DOWN);
	if(!(isHit))
	{
		//当たらなかった
		return;
	}
	VERTEX3D vertex[1+ LOOP]{};
	unsigned short index[LOOP* POLYGON]{};
	for (auto& v : vertex)
	{
		//v.pos = pos;
		//v.u = 0;
		//v.v = 0;
		v.norm =VGet(0.0f, 0.0f, -1.0f);
		v.dif = GetColorU8(255, 255, 255, SHADOW_ALPHA);
		v.spc = GetColorU8(0, 0, 0, 0);
		v.su = 0.0f;
		v.sv = 0.0f;
	}
	////01
	////23
	//vertex[0].pos = VAdd(pos, { -1 * radius ,0,radius });
	//vertex[0].u = 0;
	//vertex[0].v = 0;
	//vertex[1].pos = VAdd(pos, { radius ,0,radius });
	//vertex[1].u = 1;
	//vertex[1].v = 0;
	//vertex[2].pos = VAdd(pos, { -1 * radius ,0,-1*radius });
	//vertex[2].u = 0;
	//vertex[2].v = 1;
	//vertex[3].pos = VAdd(pos, { radius ,0,-1*radius });
	//vertex[3].u = 1;
	//vertex[3].v = 1;
	

	vertex[0].pos = centerPos;
	vertex[0].u = 0.5f;
	vertex[0].v = 0.5f;
	//丸影の外周の部分
	float maxY = centerPos.y;
	for (int i = 1; i < LOOP+1; i++)
	{
		float xSin = sinf(AsoUtility::Deg2RadF(LOOP_ANGLE * i));
		float yCos = cosf(AsoUtility::Deg2RadF(LOOP_ANGLE * i));
		VECTOR vPos = { pos.x+xSin * radius,
			0,
			pos.z+yCos * radius };
		vPos.y = GetOnGroundY(VGet(vPos.x, pos.y, vPos.z), LINE_LENGTH_UP, LINE_LENGTH_DOWN);
		if (vPos.y > (pos.y - LINE_LENGTH_DOWN) && vPos.y> maxY)
		{
			//条件を満たしたらmaxYを更新する
			maxY = vPos.y;
		}
		vertex[i].pos =vPos;
		vertex[i].u = 0.5f+xSin*0.5f;
		vertex[i].v = 0.5f+yCos * 0.5f;
	}

	//最終的なyを決定
	vertex[0].pos.y += SHADOW_Y;
	for (int i = 1; i < LOOP + 1; i++)
	{
		//地形から外れた影をmaxYに合わせる
		if (vertex[i].pos.y <= (pos.y - LINE_LENGTH_DOWN))
		{
			vertex[i].pos.y = maxY;
		}
		vertex[i].pos.y += SHADOW_Y;
	}

	////ポリゴン設定
	//index[0] = 0;
	//index[1] = 1;
	//index[2] = 2;
	//index[3] = 1;
	//index[4] = 3;
	//index[5] = 2;
	
	for (int i = 0; i < LOOP; i++)
	{
		index[i * POLYGON] = 0;//中心点
		index[i * POLYGON + 1] = 1 + i;
		index[i * POLYGON + 2] = 1 + ((i + 1) % LOOP);
	}
	//DrawSphere3D(vertex[1].pos, 16, 8, 0xffffff, 0xffffff, false);
	//描画
	DrawPolygonIndexed3D(vertex, 1 + LOOP, index, LOOP, shadowGraph_, true);
}

float ObjectManager::GetOnGroundY(VECTOR pos, float lengthUp, float lengthDown) const
{
	float ret = pos .y - lengthDown;
	//足元座標を探す
	MV1_COLL_RESULT_POLY hit;

	hit = MV1CollCheck_Line(map_->GetModelId(), -1,
		VAdd(VScale(AsoUtility::DIR_U, lengthUp), pos),
		VAdd(VScale(AsoUtility::DIR_D, lengthDown), pos));
	if (hit.HitFlag)
	{
		//ヒットした
		ret = hit.HitPosition.y;
	}
	return ret;
}

CharacterBase* ObjectManager::GetTargetCharacter()
{
	if (characters_.size() <= 0)
	{
		//まだ生成されていない
		return nullptr;
	}
	return characters_[0];
}

void ObjectManager::CreatePlayer()
{
	if (player_ != nullptr || sceneGame_ == nullptr)
	{
		//無効
		return;
	}
	player_ = new Player(sceneGame_, playerModel_);
	//player_ = new Player(sceneGame_, testModel_);
	player_->Init();

	SceneManager::GetInstance().GetCamera()->SetFollow(player_);
}

void ObjectManager::Collision()
{
	//接触ダメージ
	constexpr int DAMAGE_HIT = 2;
	//プレイヤーと敵弾
	if (player_ != nullptr && player_->IsActive())
	{
		for (ShotBase* o : shotObj_)
		{
			if (o->IsPlayerTag())
			{
				//チームが違うのでスキップ
				continue;
			}
			if (Collision_GrazeAndEShot(*player_, *o))
			{
				//掠り判定
				if (player_->Graze())
				{

				}
			}
			//掠り判定後に当たり判定
			if (Collision_PlayerAndEShot(*player_, *o))
			{
				if (player_->Damage(o->GetDamage()))
				{
					sceneGame_->CreateEffect(SceneGame::EFFECT_TYPE::EXPLOSION_Y,player_->GetColPos(), 0.8f);
					o->SetActive(false);
				}
			}
		}
		for (CharacterBase* c : characters_)
		{
			if (Collision_GrazeAndCharacter(*player_, *c))
			{
				//掠り判定
				if (player_->Graze())
				{

				}
			}
			//掠り判定後に接触判定
			if (Collision_CharacterAndCharacter(*player_, *c))
			{
				if (player_->Damage(DAMAGE_HIT))
				{
					sceneGame_->CreateEffect(SceneGame::EFFECT_TYPE::EXPLOSION_Y, player_->GetColPos(), 0.8f);
				}
			}
			
		}
	}
	//敵とプレイヤー弾
	for (CharacterBase* e : characters_)
	{
		for (ShotBase* o : shotObj_)
		{
			if (o->IsPlayerTag() == false)
			{
				//チームが違うのでスキップ
				continue;
			}
			//当たり判定
			if (Collision_EnemyAndPShot(*e, *o))
			{
				if (e->Damage(o->GetDamage()))
				{
					sceneGame_->CreateEffect(SceneGame::EFFECT_TYPE::EXPLOSION_Y, e->GetColPos(), 0.4f);
					o->SetActive(false);
				}
			}
		}
	}
}

bool ObjectManager::Collision_SphereAndSphere(const VECTOR& oPos, float oRadius, const VECTOR& tPos, float tRadius)const
{
	//差を求めて球判定をする
	VECTOR diff = VSub(oPos, tPos);
	float inRange = oRadius + tRadius;
	//SquareSizeは2乗された値なので注意
	if (VSquareSize(diff) <= (inRange * inRange))
	{
		//当たり判定検知
		return true;
	}
	return false;
}

bool ObjectManager::Collision_GrazeSphereAndSphere(const VECTOR& oPos, float oGRadius, float oCRadius, const VECTOR& tPos, float tRadius) const
{
	//差を求めて球判定をする
	VECTOR diff = VSub(oPos, tPos);
	//掠り判定を含めないキャラの半径の合計
	float inRange = oCRadius + tRadius;
	//半径の合計
	float inRangeG = oGRadius + tRadius;
	if (fabsf(diff.y) > inRange)
	{
		//y座標の判定で範囲外なので無効か
		return false;
	}
	//SquareSizeは2乗された値なので注意
	if (VSquareSize(diff) <= (inRangeG * inRangeG))
	{
		//当たり判定検知
		return true;
	}
	return false;
}

bool ObjectManager::Collision_PlayerAndEShot(const CharacterBase& player, const ShotBase& eShot)const
{
	if (!(eShot.IsActive())|| !(player.IsActive()))
	{
		//アクティブではない
		return false;
	}
	//処理が同じなのでそちらを呼び出し
	return Collision_EnemyAndPShot(player, eShot);
}

bool ObjectManager::Collision_EnemyAndPShot(const CharacterBase& enemy, const ShotBase& pShot) const
{
	if (!(pShot.IsActive()) || !(enemy.IsActive())|| !(enemy.IsActiveCollision()))
	{
		//アクティブではない
		return false;
	}
	const VECTOR enemyPos = enemy.GetColPos();
	const float enemyColRadius = enemy.GetColRadiusShot();

	const VECTOR pShotPos = pShot.GetPos();
	const float pShotRadius = pShot.GetColRadius();

	return Collision_SphereAndSphere(enemyPos, enemyColRadius, pShotPos, pShotRadius);
}

bool ObjectManager::Collision_CharacterAndCharacter(const CharacterBase& o, const CharacterBase& t) const
{
	if (!(o.IsActive())|| !(t.IsActive())|| !(o.IsActiveCollision())|| !(t.IsActiveCollision()))
	{
		//アクティブではない
		return false;
	}
	const VECTOR oPos = o.GetColPos();
	const float oColRadius = o.GetColRadiusShot();

	const VECTOR tPos = t.GetColPos();
	const float tColRadius = t.GetColSize();

	return Collision_SphereAndSphere(oPos, oColRadius, tPos, tColRadius);
}

bool ObjectManager::Collision_GrazeAndCharacter(const Player& p, const CharacterBase& e) const
{
	if (!(p.IsActive()) || !(e.IsActive()) || !(p.IsActiveCollision()) || !(e.IsActiveCollision()))
	{
		//アクティブではない
		return false;
	}
	const VECTOR pPos = p.GetColPos();
	const float pGRadius = p.GetGrazeRadius();
	const float pColRadius = p.GetColSize();

	const VECTOR ePos = e.GetColPos();
	const float eColRadius = e.GetColSize();
	return Collision_GrazeSphereAndSphere(pPos, pGRadius,pColRadius,ePos,eColRadius);
}

bool ObjectManager::Collision_GrazeAndEShot(const Player& p, const ShotBase& eShot) const
{
	if (!(eShot.IsActive()) || !(p.IsActive()))
	{
		//アクティブではない
		return false;
	}
	const VECTOR pPos = p.GetColPos();
	const float pGRadius = p.GetGrazeRadius();
	const float pColRadius = p.GetColSize();

	const VECTOR eShotPos = eShot.GetPos();
	const float eShotRadius = eShot.GetColRadius();

	return Collision_GrazeSphereAndSphere(pPos, pGRadius, pColRadius, eShotPos, eShotRadius);
}

