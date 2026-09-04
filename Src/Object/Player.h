#pragma once
#include "CharacterBase.h"

class InputManager;

class Player :
    public CharacterBase
{
public:
	// 無効な画像ハンドル
	static constexpr int INVALID_GRAPH_HANDLE = -1;

	// 初期タイマー値
	static constexpr int INITIAL_TIMER = 0;

	// 初期ライフ
	static constexpr int INITIAL_LIFE = 1;

	// 復活後の無敵時間
	static constexpr float RECOVERY_TIME_S = 4.0f;

	// プレイヤーのHP最大値
	static constexpr int PLAYER_MAX_HP = 8;

	// プレイヤーの最大残機
	static constexpr int PLAYER_MAX_LIFE = 3;

	// 射撃時のコライダー半径
	static constexpr float SHOT_COLLIDER_RADIUS = 24.0f;

	// コライダーを浮かせる高さ
	static constexpr float COLLIDER_Y_ADD = 10.0f;

	// 画面外ダメージ
	static constexpr int DAMAGE_OUT = 1;

	// 画面外判定の開始値
	static constexpr int INITIAL_OUT_COUNT = 0;

	// 掠り演出のアルファ値
	static constexpr int GRAZE_ALPHA_MAX = 255;

	// 掠り演出の透明度計算に使用する値
	static constexpr float GRAZE_ANIM_HALF = 2.0f;

	// 頂点数
	static constexpr int GRAZE_VERTEX_COUNT = 4;

	// インデックス数
	static constexpr int GRAZE_INDEX_COUNT = 6;

	// ポリゴン数
	static constexpr int GRAZE_POLYGON_COUNT = 2;

	// テクスチャ座標の最大値
	static constexpr float TEXTURE_COORD_MAX = 1.0f;

	// 頂点法線のZ方向
	static constexpr float VERTEX_NORMAL_Z = -1.0f;

	// 頂点の黒色成分
	static constexpr int VERTEX_COLOR_BLACK = 0;

	// 頂点の透明度
	static constexpr int VERTEX_ALPHA_NONE = 0;

	// マテリアルのアウトライン幅
	static constexpr float OUTLINE_WIDTH_NONE = 0.0f;

	// マテリアルの色
	static constexpr float MATERIAL_COLOR_NONE = 0.0f;

	// マテリアルのアルファ値
	static constexpr float MATERIAL_ALPHA_MAX = 1.0f;

	//走行スピード
	static constexpr float SPEED_SPRINT = SPEED_WALK * 2;

	//頭の高さ
	static constexpr float HEAD_POS = 120;

	
	// 敵弾が近くを通った判定(TP判定)の半径
	static constexpr float GRAZE_RADIUS = DEFAULT_COLLIDERSIZE * 4.0f;
	
	// 掠り判定成功時のボーナス弾
	static constexpr int GRAZE_BONUS_SHOT = 10;

	
	// 警告までの時間(秒)
	static constexpr float OUT_WARNING = 1.5f;
	
	// 画面外に出た時間がこの秒数を超えたらライフを減らす(秒)
	static constexpr float OUT_LIMIT = 2.0f;

	//ジャンプ力
	static constexpr float JUMP_POW = 18.0f;

	// スタミナ(フレーム = 1秒 = 60fps)
	static constexpr float MAX_STAMINA = 120.0f;

	Player(SceneGame* parent, const CharacterModelData& modelData);
	~Player(void) override;

	void Draw() override;

	void Release() override;
	
	// 画面外の情報を渡す
	void SetIsOut(bool b);

	bool Damage(int damage) override;
	
	// 弾がキャラの近くを通った(TP判定)
	bool Graze();

	void Died() override;

	// 残り残機
	int GetLife()const;
	
	// 最大残機数
	int GetLifeMax()const;

	
	// 再アクティブ化と復活処理
	void Respawn_Player();

	float GetGrazeRadius()const;
	
	// 手持ちのボーナス弾の数を返す
	int GetBonusShot()const;
	
	// ジャンプ回数
	int GetJumpCount()const;
	
	// HPやジャンプカウントなどをリセットする
	void ResetStatus() override;
	
	// 最高記録を返す。この時、現時点のコンボが最高記録を上回るなら、GetGrazeComboを返す
	int GetGrazeComboScore()const;
	
	// 現時点のコンボを返す
	int GetGrazeCombo()const;

	bool StaminaCount(void);

private:
	
	// 輪の画像ハンドル
	int grazeImg_;

	bool preIsGrounded_;
	
	// 1フレーム前の座標
	VECTOR prePos_;
	
	// リスポーン座標
	VECTOR respawnPos_;
	
	// 画面外に出た時間をカウントする(フレーム数)
	int outCount_;

	bool sprintFlag_;

	ANIM animState_;
	//1フレーム前
	ANIM oldAnimState_;

	//oManagerからSetIsOutを通して設定する
	bool isOut_;
	//押しっぱなしで自動滑空のためのフラグ
	bool autoGliderFlag_;

	// 復活後の無敵時間。1以上で無敵扱い
	int recoveryTimer_;
	
	// 残機
	int life_;
	
	// 残機の最大値
	int lifeMax_;

	// リングのアニメーション
	int grazeAnimTimer_;

	// 射撃速度上昇。preDamagedがtrueの場合は0に戻るまでボーナスが発生しない。
	int grazeBonusShot_;
	
	// 1フレーム前の状態
	bool preGrazed_;
	
	// 最後のジャンプ高度
	float lastJumpPosY_;
	
	// ゲーム中にジャンプした回数
	int jumpCount_;
	
	// 連続でカウンター成立した数。ダメージでリセット
	int grazeCombo_;
	
	// grazeComboの最高記録
	int grazeComboScore_;

	// xキーとスペースキーを検知
	bool IsTrgDowm_Jump(const InputManager& ins)const;
	
	// xキーとスペースキーを検知
	bool IsNew_Jump(const InputManager& ins)const;
	
	// 射撃キーを検知
	bool IsNew_Shot(const InputManager& ins)const;

	void SetParam() override;
	void Update_Move() override;

	void Update_Shot() override;
	void Update_Count() override;

	// TP判定時のリング表示
	void Draw_Graze();

	ANIM PlayAnim(ANIM curState, ANIM type, bool isLoop = true,
		float startStep = 0.0f, float endStep = -1.0f, bool isStop = false, bool isForce = false)override;
	
	// trueの場合無敵扱い
	bool IsRecoveryTime()const;

	void SetMaterialSetting() override;
	
	// 掠り判定が発生中か調べる
	bool IsGrazed();

	// 現在の最高記録と比較してコンボ数を反映する。
	void RecordCombo();

	std::chrono::steady_clock::time_point expireTime_;
};

