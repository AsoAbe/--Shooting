#include <DxLib.h>
#include"../Manager/InputManager.h"
#include"../Common/AsoUtility.h"
#include"../Object/Player.h"
#include "../Scene/SceneGame.h"
#include "Camera.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

Camera::Camera(void)
{
	pos_ = { 0,0,0 };
	angles_ = { 0,0,0 };
	cameraShake_ = { 0,0,0 };
	oldMousePos_ = { 0,0 };
	targetCharacterPos_ = AsoUtility::VECTOR_ZERO;
}

Camera::~Camera(void)
{
}

void Camera::Init(void)
{
	Reset();
}

void Camera::Update(void)
{
	//後で状態ごとに関数を分ける
	if (mode_ == MODE::LOOK_DOWN_F && follow_ != nullptr)
	{
		//デグリ回転
		constexpr float cameraRotSpdDeg = 8;
		//座標の差がこの距離より小さいなら動きを止める
		constexpr float cameraMoveMinDiff = 400;
		//相対座標
		VECTOR diff = VSub(targetCharacterPos_, follow_->GetPos());
		//Yの影響をなくしたベクトル
		VECTOR diffXZ = { diff.x,0,diff.z };
		if (VSquareSize(diffXZ) < (cameraMoveMinDiff * cameraMoveMinDiff))
		{
			return;
		}
		//目標Y回転
		float targetAngle = atan2f(diff.x, diff.z);

		float diffDegAngle = AsoUtility::DegIn360(AsoUtility::Rad2DegF(targetAngle) - AsoUtility::Rad2DegF(angles_.y));
		if (diffDegAngle > 180)
		{
			//-180から180の範囲に入れる
			diffDegAngle -= 360;
		}
		if (fabsf(diffDegAngle) < cameraRotSpdDeg)
		{
			//追い越し防止
			angles_.y = targetAngle;
		}
		else
		{
			if (diffDegAngle>0)
			{
				angles_.y = AsoUtility::RadIn2PI(angles_.y + AsoUtility::Deg2RadF(cameraRotSpdDeg));
			}
			else
			{
				angles_.y = AsoUtility::RadIn2PI(angles_.y- AsoUtility::Deg2RadF(cameraRotSpdDeg));
			}
		}

	}
}

void Camera::SetBeforeDraw(void)
{
	//SetScreenでリセットされる
	
	//クリップ範囲を設定する
	SetCameraNearFar(10.0f, 30000.0f);
	switch (mode_)
	{
	case MODE::FIXED_POINT:
		SetBeforeDrawFixedPoint();
		break;
	case MODE::FREE:
		SetBeforeDrawFree();
		break;
	case Camera::MODE::FOLLOW:
		//SetBeforeDrawFollow(false);
		SetBeforeDrawFollow(true);
		break;
	case Camera::MODE::LOOK_DOWN:
		SetBeforeDrawLookDown();
		break;
	case Camera::MODE::LOOK_DOWN_F:
		SetBeforeDrawFollowLookDown();
		break;
	default:
		SetCameraPositionAndAngle(pos_, angles_.x, angles_.y, angles_.z);
		break;
	}
}

void Camera::Draw(void)
{
	if (mouseLock_)
	{
		DrawFormatString(0, Application::SCREEN_SIZE_Y - 16, 0xFFFF00, "マウスカーソルを表示するにはZキーを押す");
	}
	else
	{
		if (mode_ == MODE::FOLLOW)
		{
			//案内表示(有効なカメラモードのみ)
			DrawFormatString(0, Application::SCREEN_SIZE_Y - 16, 0xFFFFFF, "マウス操作を有効にするにはZキーを押す");
		}
	}
	/*
	//カメラ情報の描画
	DrawFormatString(0, 0, 0xFFFFFF, "camera座標 %.2f,%.2f,%.2f",
		pos_.x, pos_.y, pos_.z);
	DrawFormatString(0, 16, 0xFFFFFF, "cameraRotation %.2f,%.2f,%.2f"
		, AsoUtility::Rad2DegF(angles_.x)
		, AsoUtility::Rad2DegF(angles_.y)
		, AsoUtility::Rad2DegF(angles_.z));
	DrawFormatString(0, 64, 0xFFFFFF, "注視点 %.2f,%.2f,%.2f",
		GetTargetPos().x
		, GetTargetPos().y
		, GetTargetPos().z);
	*/
	//注視点の目印
	//DrawSphere3D(GetTargetPos(), 16, 16, 0xFF0000, 0x000000, true);
	
}

void Camera::Release(void)
{
}
void Camera::UpdateCameraPos(void)
{
	SetCameraNearFar(10.0f, 30000.0f);
	SetCameraPositionAndAngle(pos_, angles_.x, angles_.y, angles_.z);
}
VECTOR Camera::GetPos(void) const
{
	return pos_;
}
VECTOR Camera::GetAngles(void) const
{
	return angles_;
}

void Camera::ChangeMode(MODE mode)
{
	//モード切替
	mode_ = mode;
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		break;
	case Camera::MODE::FREE:
		break;
	case Camera::MODE::FOLLOW:
		break;
	default:
		break;
	}
}

void Camera::SetFollow(Player* follow)
{
	follow_ = follow;
}

VECTOR Camera::GetTargetPos(void)
{
	return targetPos_;
}

void Camera::Reset()
{
	//リセット
	SetMouseLock(false);
	//カメラの初期モード
	ChangeMode(MODE::FIXED_POINT);
	//位置
	pos_ = { 0.0f,10.0f,-150 };
	//角度(ラジアン)
	angles_ = { 0.0f,0.0f,0.0f };

	cameraShake_ = { 0,0,0 };
}

void Camera::SetMouseLock(bool b)
{
	if (b)
	{
		//マウスを画面中央に固定する
		mouseLock_ = true;
		SetMouseDispFlag(false);

		SetMousePoint(SCREEN_CENTER.x, SCREEN_CENTER.y);
		oldMousePos_ = SCREEN_CENTER;
	}
	else
	{
		//マウス固定解除
		mouseLock_ = false;
		SetMouseDispFlag(true);
	}
}

void Camera::ResetCameraPos()
{
	if (mode_ == MODE::LOOK_DOWN_F && follow_ != nullptr)
	{
		//相対座標
		VECTOR diff = VSub(targetCharacterPos_, follow_->GetPos());
		//目標Y回転
		angles_.y = atan2f(diff.x, diff.z);
	}
}

void Camera::SetTargetCharacterPos(const VECTOR& pos)
{
	targetCharacterPos_ = pos;
}

void Camera::SetBeforeDrawFixedPoint(void)
{
	//SetCameraPositionAndAngle({ 0.0f,200.0f,-500.0f }, 15.0f * DX_PI_F / 180.0f, 0, 0);

	SetCameraPositionAndAngle({ pos_.x + cameraShake_.x,pos_.y,pos_.z }
	, angles_.x, angles_.y, angles_.z);

}
void Camera::SetBeforeDrawFree(void)
{
	auto& ins = InputManager::GetInstance();

	float movePow = 16.0f;
	float rotPaw = 1.0f * DX_PI_F / 180.0f;

	VECTOR dir = AsoUtility::VECTOR_ZERO;

	if (ins.IsNew(KEY_INPUT_E))
	{
		dir = VAdd(dir, { 0,1,0 });
	}
	if (ins.IsNew(KEY_INPUT_Q))
	{
		dir = VAdd(dir, { 0,-1,0 });
	}

	if (ins.IsNew(KEY_INPUT_W))
	{
		dir = VAdd(dir, { 0,0,1 });
	}
	if (ins.IsNew(KEY_INPUT_S))
	{
		dir = VAdd(dir, { 0,0,-1 });
	}
	if (ins.IsNew(KEY_INPUT_A))
	{
		dir = VAdd(dir, { -1,0,0 });
	}
	if (ins.IsNew(KEY_INPUT_D))
	{
		dir = VAdd(dir, { 1,0,0 });
	}

	if (AsoUtility::EqualsVZero(dir) == false)
	{
		MATRIX mat = MGetIdent();

		//平面移動の場合はXZを無視

		//mat = MMult(mat, MGetRotX(angles_.x));
		mat = MMult(mat, MGetRotY(angles_.y));
		//mat = MMult(mat, MGetRotZ(angles_.z));

		VECTOR moveDir = VTransform(dir, mat);
		pos_ = VAdd(pos_, VScale(moveDir, movePow));
	}

	//カメラ回転
	if (CheckHitKey(KEY_INPUT_UP))
	{
		angles_.x -= rotPaw;
	}
	if (ins.IsNew(KEY_INPUT_DOWN))
	{
		angles_.x += rotPaw;
	}
	if (ins.IsNew(KEY_INPUT_RIGHT))
	{
		angles_.y += rotPaw;
	}
	if (ins.IsNew(KEY_INPUT_LEFT))
	{
		angles_.y -= rotPaw;
	}
	SetCameraPositionAndAngle(pos_, angles_.x, angles_.y, angles_.z);
}

void Camera::SetBeforeDrawFollow(bool forward)
{
	auto& ins = InputManager::GetInstance();
	
	
	int mouseMoveX = 0;
	int mouseMoveY = 0;

	//トグル
	if (ins.IsTrgDown(KEY_INPUT_Z))
	{
		SetMouseLock(!mouseLock_);
	}

	if (mouseLock_)
	{
		//マウスの移動量を入力
		int mousePosX;
		int mousePosY;
		GetMousePoint(&mousePosX, &mousePosY);
		mouseMoveX = mousePosX - oldMousePos_.x;
		mouseMoveY = mousePosY - oldMousePos_.y;
		SetMousePoint(SCREEN_CENTER.x, SCREEN_CENTER.y);
		oldMousePos_ = SCREEN_CENTER;
	}
	else
	{
		//入力しない
		mouseMoveX = 0;
		mouseMoveY = 0;
	}

	float rotPaw = 2.0f * DX_PI_F / 180.0f;
	float rotPawMouse = 0.05f;

	VECTOR dir = AsoUtility::VECTOR_ZERO;

	//カメラ回転
	if (mouseMoveY != 0)
	{
		angles_.x += mouseMoveY * rotPaw* rotPawMouse;
	}
	else
	{
		if (CheckHitKey(KEY_INPUT_UP))
		{
			angles_.x -= rotPaw;
		}
		if (ins.IsNew(KEY_INPUT_DOWN))
		{
			angles_.x += rotPaw;
		}
	}
	//x軸の回転制限
	if (angles_.x < ANGLE_X_MIN)
	{
		angles_.x = ANGLE_X_MIN;
	}
	if (angles_.x > ANGLE_X_MAX)
	{
		angles_.x = ANGLE_X_MAX;
	}
	//y軸回転
	if (mouseMoveX != 0)
	{
		angles_.y += mouseMoveX * rotPaw * rotPawMouse;
	}
	else
	{
		if (ins.IsNew(KEY_INPUT_RIGHT))
		{
			angles_.y += rotPaw;
		}
		if (ins.IsNew(KEY_INPUT_LEFT))
		{
			angles_.y -= rotPaw;
		}
	}
	VECTOR followPos = AsoUtility::VECTOR_ZERO;
	if (follow_ != nullptr)
	{
		followPos = VAdd(follow_->GetPos(), { 0,HEIGHT,0 });
	}
	targetPos_ = followPos;

	//マトリクス作成
	MATRIX mat = MGetIdent();

	//yを180度反対に
	float revAngleY = angles_.y + AsoUtility::DegToRadF(180.0f);

	//XとYを適用
	if (forward == false)
	{
		mat = MMult(mat, MGetRotX(angles_.x));
	}
	else
	{
		//注視点を考慮する場合はxを反転
		mat = MMult(mat, MGetRotX(angles_.x * -1));
	}
	mat = MMult(mat, MGetRotY(revAngleY));

	VECTOR localPos = { CAMERAPOS_X,0.0f,DIS_FOLLOW2CAMERA };

	//ローカル座標を回転させる
	localPos = VTransform(localPos, mat);
	
	////高さはここで加算
	//localPos.y += HEIGHT;
	//最終的なカメラの位置を設定
	pos_ = VAdd(followPos, localPos);


	VECTOR targetPos = targetPos_;
	if (forward)
	{

		//注視点あり

		//新しくマトリクスを作成
		mat = MGetIdent();

		//XとYを適用
		mat = MMult(mat, MGetRotX(angles_.x));
		mat = MMult(mat, MGetRotY(angles_.y));
		//注視点の座標を設定
		targetPos = VAdd(targetPos, VTransform({ 0,0,DIS_FOLLOW2TARGET }, mat));
		//targetPos_を更新
		targetPos_ = targetPos;
	}

	// カメラの設定(位置と回転による制御)
	SetCameraPositionAndAngle(pos_, angles_.x, angles_.y, angles_.z);
	//// カメラの設定(位置と注視点による制御)
	//SetCameraPositionAndTargetAndUpVec(pos_,
	//	targetPos_,
	//	{ 0.0f, 1.0f, 0.0f });

}

void Camera::SetBeforeDrawLookDown()
{
	angles_.x = AsoUtility::Deg2RadF(30);
	angles_.y = 0;
	angles_.z = 0;

	//マトリクス作成
	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetRotX(angles_.x + DX_PI_F));

	VECTOR followPos = AsoUtility::VECTOR_ZERO;
	VECTOR offsetPos = { 0, HEIGHT, CAMERAOFFSET_LOOKDOWN_Z };
	offsetPos = VTransform(offsetPos, MGetRotY(angles_.y));
	if (follow_ != nullptr)
	{
		followPos = VAdd(follow_->GetPos(), { 0,HEIGHT,200 });
	}
	VECTOR localPos = { 0,0,800.0f };
	localPos = VTransform(localPos, mat);
	//最終的なカメラの位置を設定
	pos_ = VAdd(followPos, localPos);

	// カメラの設定(位置と回転による制御)
	SetCameraPositionAndAngle(pos_, angles_.x, angles_.y, angles_.z);
	
}

void Camera::SetBeforeDrawFollowLookDown()
{
	angles_.x = AsoUtility::Deg2RadF(CAMERAANGLE_LOOKDOWN_X);
	//相対座標
	/*VECTOR diff = VSub(targetCharacterPos_,follow_->GetPos());
	angles_.y = atan2f(diff.x, diff.z);*/
	angles_.z = 0;


	//マトリクス作成
	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetRotX(angles_.x + DX_PI_F));
	mat = MMult(mat, MGetRotY(angles_.y));

	VECTOR followPos = follow_->GetPos();

	VECTOR offsetPos = { 0, HEIGHT, CAMERAOFFSET_LOOKDOWN_Z };
	offsetPos = VTransform(offsetPos, MGetRotY(angles_.y));
	if (follow_ != nullptr)
	{
		followPos = VAdd(follow_->GetPos(), offsetPos);
	}
	VECTOR localPos = { 0,0,CAMERADIS_LOOKDOWN_Z };
	localPos = VTransform(localPos, mat);
	//最終的なカメラの位置を設定
	pos_ = VAdd(followPos, localPos);

	// カメラの設定(位置と回転による制御)
	SetCameraPositionAndAngle(pos_, angles_.x, angles_.y, angles_.z);

}



void Camera::SetCameraShake(void)
{
	cameraShake_ = { static_cast<float>(GetRand(8) - 4),0,0 };
}
