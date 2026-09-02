#pragma once
#include <DxLib.h>
#include"../Application.h"
#include"../Common/Vector2.h"

class Player;
class Camera
{

public:
	// カメラの初期位置
	static constexpr VECTOR DEFAULT_CAMERA_POS = { 0.0f, 10.0f, -150.0f };

	// カメラの初期角度
	static constexpr VECTOR DEFAULT_CAMERA_ANGLES = { 0.0f, 0.0f, 0.0f };

	// カメラの初期シェイク量
	static constexpr VECTOR DEFAULT_CAMERA_SHAKE = { 0.0f, 0.0f, 0.0f };

	// フリーカメラの移動速度
	static constexpr float FREE_CAMERA_MOVE_SPEED = 16.0f;

	// フリーカメラの回転速度
	static constexpr float FREE_CAMERA_ROT_SPEED_DEG = 1.0f;

	// 追従カメラの回転速度
	static constexpr float FOLLOW_CAMERA_ROT_SPEED_DEG = 2.0f;

	// マウスによるカメラ回転の倍率
	static constexpr float MOUSE_ROT_SCALE = 0.05f;

	// カメラのY軸反転角度
	static constexpr float REVERSE_ANGLE_Y_DEG = 180.0f;

	// 見下ろしカメラの追従位置Z方向オフセット
	static constexpr float LOOKDOWN_FOLLOW_OFFSET_Z = 200;

	// 見下ろしカメラのローカル位置
	static constexpr VECTOR LOOKDOWN_LOCAL_POS = { 0.0f, 0.0f, 800.0f };

	// カメラシェイクの最大値
	static constexpr int CAMERA_SHAKE_MAX = 8;

	// カメラシェイクの中心補正
	static constexpr int CAMERA_SHAKE_OFFSET = 4;

	static constexpr Vector2 SCREEN_CENTER = { Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2 };
	// カメラの高さ
	static constexpr float HEIGHT = 170.0f;
	static constexpr float CAMERAPOS_X = -70.0f;
	
	// 追従対象とカメラの距離
	static constexpr float DIS_FOLLOW2CAMERA = 150.0f;
	
	// 追従対象から注視点までの距離
	static constexpr float DIS_FOLLOW2TARGET = 350.0f;

	// カメラのクリップ範囲（UpdateCameraPos用）
	static constexpr float CAMERA_NEAR = 10.0f;
	static constexpr float CAMERA_FAR = 30000.0f;

	// LOOK_DOWN_Fのカメラ回転速度（度）
	static constexpr float FOLLOW_LOOKDOWN_ROT_SPEED_DEG = 8.0f;

	// LOOK_DOWN_Fでカメラ移動を停止する最小距離
	static constexpr float FOLLOW_LOOKDOWN_MOVE_MIN_DIFF = 400.0f;

	// 角度を-180～180度に収めるための基準角度
	static constexpr float HALF_CIRCLE_DEG = 180.0f;

	// 角度の1周分
	static constexpr float FULL_CIRCLE_DEG = 360.0f;

	// カメラ案内表示のY方向オフセット
	static constexpr int CAMERA_MESSAGE_OFFSET_Y = 16;

	// マウスカーソル表示案内の文字色
	static constexpr unsigned int MOUSE_LOCK_MESSAGE_COLOR = 0xFFFF00;

	// マウス操作案内の文字色
	static constexpr unsigned int MOUSE_UNLOCK_MESSAGE_COLOR = 0xFFFFFF;

	// マウス移動量がない状態
	static constexpr int NO_MOUSE_MOVEMENT = 0;

	// フリーカメラ上方向
	static constexpr VECTOR FREE_CAMERA_MOVE_UP = { 0, 1, 0 };

	// フリーカメラ下方向
	static constexpr VECTOR FREE_CAMERA_MOVE_DOWN = { 0, -1, 0 };

	// フリーカメラ前方向
	static constexpr VECTOR FREE_CAMERA_MOVE_FORWARD = { 0, 0, 1 };

	// フリーカメラ後方向
	static constexpr VECTOR FREE_CAMERA_MOVE_BACK = { 0, 0, -1 };

	// フリーカメラ左方向
	static constexpr VECTOR FREE_CAMERA_MOVE_LEFT = { -1, 0, 0 };

	// フリーカメラ右方向
	static constexpr VECTOR FREE_CAMERA_MOVE_RIGHT = { 1, 0, 0 };

	// LOOK_DOWNの角度初期値
	static constexpr float DEFAULT_LOOKDOWN_ANGLE = 0.0f;

	// LOOK_DOWNのY方向オフセット
	static constexpr float LOOKDOWN_OFFSET_Y = 0.0f;

	// LOOK_DOWNのローカルX方向オフセット
	static constexpr float LOOKDOWN_LOCAL_OFFSET_X = 0.0f;

	// LOOK_DOWNのローカルY方向オフセット
	static constexpr float LOOKDOWN_LOCAL_OFFSET_Y = 0.0f;

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



