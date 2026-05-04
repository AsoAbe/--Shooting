#pragma once
#include "CharacterBase.h"

class InputManager;

class Player :
    public CharacterBase
{
public:
	//走行スピード
	static constexpr float SPEED_SPRINT = SPEED_WALK * 2;
	//頭の高さ
	static constexpr float HEAD_POS = 120;
	/// <summary>
	/// 敵弾が近くを通った判定(TP判定)の半径
	/// </summary>
	static constexpr float GRAZE_RADIUS = DEFAULT_COLLIDERSIZE * 4.0f;
	/// <summary>
	/// 掠り判定成功時のボーナス弾
	/// </summary>
	static constexpr int GRAZE_BONUS_SHOT = 10;

	/// <summary>
	/// 警告までの時間(秒)
	/// </summary>
	static constexpr float OUT_WARNING = 1.5f;
	/// <summary>
	/// 画面外に出た時間がこの秒数を超えたらライフを減らす(秒)
	/// </summary>
	static constexpr float OUT_LIMIT = 2.0f;//4.0f;

	//ジャンプ力
	static constexpr float JUMP_POW = 18.0f;

	// スタミナ(フレーム = 1秒 = 60fps)
	static constexpr float MAX_STAMINA = 120.0f;

	Player(SceneGame* parent, const CharacterModelData& modelData);
	~Player(void) override;

	void Draw() override;

	void Release() override;
	/// <summary>
	/// 画面外の情報を渡す
	/// </summary>
	void SetIsOut(bool b);

	bool Damage(int damage) override;

	/// <summary>
	/// 弾がキャラの近くを通った(TP判定)
	/// </summary>
	/// <returns></returns>
	bool Graze();

	void Died() override;

	/// <summary>
	/// 残り残機
	/// </summary>
	/// <returns></returns>
	int GetLife()const;
	/// <summary>
	/// 最大残機数
	/// </summary>
	/// <returns></returns>
	int GetLifeMax()const;

	/// <summary>
	/// 再アクティブ化と復活処理
	/// </summary>
	void Respawn_Player();

	float GetGrazeRadius()const;
	/// <summary>
	/// 手持ちのボーナス弾の数を返す
	/// </summary>
	/// <returns></returns>
	int GetBonusShot()const;

	/// <summary>
	/// ジャンプ回数
	/// </summary>
	/// <returns></returns>
	int GetJumpCount()const;

	/// <summary>
	/// HPやジャンプカウントなどをリセットする
	/// </summary>
	void ResetStatus() override;
	
	/// <summary>
	/// 最高記録を返す。この時、現時点のコンボが最高記録を上回るなら、GetGrazeComboを返す
	/// </summary>
	int GetGrazeComboScore()const;
	/// <summary>
	/// 現時点のコンボを返す
	/// </summary>
	int GetGrazeCombo()const;

	bool StaminaCount(void);

private:

	/// <summary>
	/// 輪の画像ハンドル
	/// </summary>
	int grazeImg_;

	bool preIsGrounded_;

	/// <summary>
	/// 1フレーム前の座標
	/// </summary>
	VECTOR prePos_;
	/// <summary>
	/// リスポーン座標
	/// </summary>
	VECTOR respawnPos_;
	/// <summary>
	/// 画面外に出た時間をカウントする(フレーム数)
	/// </summary>
	int outCount_;

	bool sprintFlag_;

	

	ANIM animState_;
	//1フレーム前
	ANIM oldAnimState_;

	//oManagerからSetIsOutを通して設定する
	bool isOut_;
	//押しっぱなしで自動滑空のためのフラグ
	bool autoGliderFlag_;

	/// <summary>
	/// 復活後の無敵時間。1以上で無敵扱い
	/// </summary>
	int recoveryTimer_;
	/// <summary>
	/// 残機
	/// </summary>
	int life_;
	/// <summary>
	/// 残機の最大値
	/// </summary>
	int lifeMax_;

	/// <summary>
	/// リングのアニメーション
	/// </summary>
	int grazeAnimTimer_;

	/// <summary>
	/// 射撃速度上昇。preDamagedがtrueの場合は0に戻るまでボーナスが発生しない。
	/// </summary>
	float grazeBonusShot_;
	/// <summary>
	/// 1フレーム前の状態
	/// </summary>
	bool preGrazed_;

	/// <summary>
	/// 最後のジャンプ高度
	/// </summary>
	float lastJumpPosY_;

	/// <summary>
	/// ゲーム中にジャンプした回数
	/// </summary>
	int jumpCount_;

	/// <summary>
	/// 連続でカウンター成立した数。ダメージでリセット
	/// </summary>
	int grazeCombo_;
	/// <summary>
	/// grazeComboの最高記録
	/// </summary>
	int grazeComboScore_;

	/// <summary>
	/// xキーとスペースキーを検知
	/// </summary>
	bool IsTrgDowm_Jump(const InputManager& ins)const;
	/// <summary>
	/// xキーとスペースキーを検知
	/// </summary>
	bool IsNew_Jump(const InputManager& ins)const;
	/// <summary>
	/// 射撃キーを検知
	/// </summary>
	bool IsNew_Shot(const InputManager& ins)const;

	void SetParam() override;
	void Update_Move() override;


	void Update_Shot() override;
	void Update_Count() override;

	/// <summary>
	/// TP判定時のリング表示
	/// </summary>
	void Draw_Graze();

	ANIM PlayAnim(ANIM curState, ANIM type, bool isLoop = true,
		float startStep = 0.0f, float endStep = -1.0f, bool isStop = false, bool isForce = false)override;

	/// <summary>
	/// trueの場合無敵扱い
	/// </summary>
	bool IsRecoveryTime()const;

	void SetMaterialSetting() override;
	/// <summary>
	/// 掠り判定が発生中か調べる
	/// </summary>
	bool IsGrazed();

	/// <summary>
	/// 現在の最高記録と比較してコンボ数を反映する。
	/// </summary>
	void RecordCombo();

	std::chrono::steady_clock::time_point expireTime_;

};

