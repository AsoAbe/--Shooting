#pragma once
#include <DxLib.h>
#include"../Application.h"
#include"../Common/Vector2.h"

class Player;
class Camera
{

public:
	static constexpr Vector2 SCREEN_CENTER = { Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 };
	// カメラの高さ
	//static constexpr float HEIGHT = 200.0f;
	static constexpr float HEIGHT = 170.0f;
	static constexpr float CAMERAPOS_X = -70.0f;
	// 追従対象とカメラの距離
	//static constexpr float DIS_FOLLOW2CAMERA = 500.0f;
	static constexpr float DIS_FOLLOW2CAMERA = 150.0f;
	// 追従対象から注視点までの距離
	static constexpr float DIS_FOLLOW2TARGET = 350.0f;

	static constexpr float ANGLE_X_MIN = DX_PI_F/180*(-45);//カメラ回転の下限
	static constexpr float ANGLE_X_MAX = DX_PI_F / 180*(85);//カメラ回転の上限
	
	static constexpr float CAMERAOFFSET_LOOKDOWN_Z = 200;

	static constexpr float CAMERADIS_LOOKDOWN_Z = 800;
	static constexpr float CAMERAANGLE_LOOKDOWN_X = 30;


	enum class MODE
	{
		NONE
		, FIXED_POINT	//定点
		, FREE			//フリーカメラ
		, FOLLOW		//追従
		, LOOK_DOWN		//見下ろし
		, LOOK_DOWN_F	//見下ろし追従
	};


	Camera(void);
	~Camera(void);

	void Init(void);
	void Update(void);
	void SetBeforeDraw(void);
	void Draw(void);
	void Release(void);

	//SetBeforeDrawの座標設定だけを行う
	void UpdateCameraPos(void);

	VECTOR GetPos(void) const;
	VECTOR GetAngles(void) const;

	void ChangeMode(MODE mode);

	void SetCameraShake(void);

	//追従対象の設定
	void SetFollow(Player* follow);

	VECTOR GetTargetPos(void);

	//初期状態に戻す(シーン遷移後用)
	void Reset();

	void SetMouseLock(bool b);

	/// <summary>
	/// Updateによる滑らかな挙動を飛ばす
	/// </summary>
	void ResetCameraPos();

	void SetTargetCharacterPos(const VECTOR& pos);
private:
	//trueでマウスを固定しマウスの移動量でカメラを操作するようにする zキーで切り替え
	bool mouseLock_;

	//一フレーム前のマウス座標
	Vector2 oldMousePos_;


	//追従対象
	Player* follow_;
	//対象を画面中央に収める
	VECTOR targetCharacterPos_;
	

	// カメラの位置
	VECTOR pos_;

	// カメラの角度(rad)
	VECTOR angles_;

	//注視点
	VECTOR targetPos_;


	VECTOR cameraShake_;
	MODE mode_;
	void SetBeforeDrawFixedPoint(void);
	void SetBeforeDrawFree(void);
	void SetBeforeDrawFollow(bool forward);
	void SetBeforeDrawLookDown();
	void SetBeforeDrawFollowLookDown();
};



