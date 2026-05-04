#pragma once

#include"SceneBase.h"
#include"../Common/Vector2.h"

class ObjectManager;
class ScorePanel;
class Grid;
class Camera;
class PanelBase;

class PixelShaderMaterial;
class PixelShaderRenderer;

struct ScoreData;

class SceneGame : public SceneBase
{
public:
	//プレイヤーの基準座標
	static constexpr  float PLAYER_START_Z = -2000;

	//カウンターチュートリアルの目標値
	static constexpr  int TUTORIAL_SHOT_NUM = 8;

	//チャージエフェクトのエフェクト時間(60FPS基準のフレーム数)
	static constexpr  int CHARGE_EFFECT_F = 66;

	/// <summary>
	/// ゲームオーバー状態など
	/// </summary>
	enum class GAME_STATE
	{
		GAME,			//メインゲーム
		TUTORIAL_JUMP,	//ジャンプのチュートリアル
		TUTORIAL_SHOT,	//カウンターのチュートリアル
		TUTORIAL_END,	//チュートリアル終了
		GAMEOVER,		//ゲームオーバー
		GAMECLEAR		//ゲームクリア
	};

	enum class EFFECT_TYPE
	{
		EXPLOSION_Y,
		CHARGE,
		EXPLOSION_SHOT,
		SMOKE
	};

	//コンストラクタ
	SceneGame(void);
	//デストラクタ
	~SceneGame(void);

	//初期化
	bool virtual Init(void) override;
	//更新
	void virtual Update(void) override;
	//描画
	void virtual Draw(void) override;
	//解放
	bool Release(void) override;

	//エフェクト発生
	int CreateEffect(EFFECT_TYPE eType,VECTOR pos,float scale);

	//時間の取得
	int GetTimer(void);

	ObjectManager* GetOManager();

	/// <summary>
	/// すべてのゲームオーバー判定(ゲームクリアなども含む)
	/// </summary>
	/// <returns></returns>
	bool IsGameOver()const;

	/// <summary>
	/// チュートリアル中ならtrue
	/// </summary>
	/// <returns></returns>
	bool virtual IsTutorial()const;

	/// <summary>
	/// カウンターのチュートリアル中ならtrue
	/// </summary>
	/// <returns></returns>
	bool IsGrazeTutorial() const;

	/// <summary>
	/// カウンターのチュートリアルの進行度を返す
	/// </summary>
	/// <returns></returns>
	float virtual GetGrazeTutorialProgress()const;

	/// <summary>
	/// ジャンプのチュートリアル中ならtrue
	/// </summary>
	/// <returns></returns>
	bool IsJumpTutorial() const;
	
	/// <summary>
	/// 条件をチェックして満たしたらステートを進める
	/// </summary>
	/// <returns>ステートが切り替わったらtrue</returns>
	bool CheckJumpTutorial();

	/// <summary>
	/// 条件をチェックして満たしたらステートを進める
	/// </summary>
	/// <returns>ステートが切り替わったらtrue</returns>
	bool virtual CheckGrazeTutorial();

	/// <summary>
	/// isGameover_をtrueにする
	/// </summary>
	void GameOver();

	void GameClear();

	/// <summary>
	/// コンテニューパネルを表示する
	/// </summary>
	void PlayerDied();

	/// <summary>
	/// プレイヤーを復活させ、コンテニューパネルを非表示に
	/// </summary>
	void Continue();

	/// <summary>
	/// (スコアを記録して)タイトルに戻る
	/// </summary>
	void BackToTitle();

	/// <summary>
	/// プレイヤーのHPバーを光らせる
	/// </summary>
	void PlayerBarFlash();

	/// <summary>
	/// 敵のHPバーを光らせる
	/// </summary>
	void EnemyBarFlash();

	/// <summary>
	/// プレイヤーのHPバーのアニメーション開始
	/// </summary>
	void PlayerBarAnim();

	/// <summary>
	/// 敵のHPバーのアニメーション開始
	/// </summary>
	void EnemyBarAnim();

protected:
	void Draw_MainGame();
	void Draw_ScorePanel();

	void NextTutorial();

	//使うサウンドをまとめてロード
	void LoadSceneGameSound();

	/// <summary>
	/// 実行したタイミングでスコアを計算してスコアマネージャに反映し、計算したスコアを返す。
	/// </summary>
	ScoreData CalcAndSetScore();

	//メイン画面スクリーン
	int mainGameScreen_;

	int explosionYEffect_;
	int chargingEffect_;
	int explosionShotEffect_;
	int smokeEffect_;
	
	//インスタンス
	ScorePanel* scorePanel_;
	Grid* grid_;
	//キャラなどのオブジェクト管理
	ObjectManager* oManager_;
	//プレイヤー死亡とゲームオーバー(ポーズも作るなら配列にする)プレイヤー死亡なら復活後も非アクティブとして保持したままにする
	PanelBase* panel_;

	/// <summary>
	/// 経過時間(フレーム)
	/// </summary>
	int timer_;

	GAME_STATE gameState_;

	/// <summary>
	/// スキップキーを長押ししたカウント(フレーム)
	/// </summary>
	int tutorialSkipTimer_;

	PixelShaderMaterial* mainPSMaterial_;
	PixelShaderRenderer* mainPSRenderer_;
	//BGM関数
	int bgm_ = 0;
};