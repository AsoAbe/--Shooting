#pragma once
#include"../Common/DrawUtility.h"

class SceneGame;
class Player;
class CharacterBase;
class Trans;

class PixelShaderMaterial;
class PixelShaderRenderer;

/// <summary>
/// プレイヤーHPとスコアを描画
/// </summary>
class ScorePanel
{
public:
	// 初期値
	static constexpr int INITIAL_COUNTER = 0;
	static constexpr int INVALID_HANDLE = -1;

	// ライフ画像
	static constexpr int LIFE_IMG_SIZE = 16;
	static constexpr int LIFE_IMG_HALF = LIFE_IMG_SIZE / 2;

	// 描画
	static constexpr int DRAW_POS_START = 0;
	static constexpr int FULL_ALPHA = 255;
	static constexpr int BLEND_ALPHA_BACKGROUND = 153;
	static constexpr int DRAW_BLEND_RESET = 0;

	// 描画色
	static constexpr int DRAW_COLOR_BLACK = 0x000000;
	static constexpr int DRAW_COLOR_WHITE = 0xFFFFFF;
	static constexpr int DRAW_COLOR_GREEN = 0x00FF00;
	static constexpr int DRAW_COLOR_YELLOW = 0xFFFF00;

	// HPバー
	static constexpr int BAR_SCREEN_SIZE = 256;
	static constexpr int BAR_FLASH_TIME = 30;
	static constexpr int BAR_ANIM_TIME = 12;
	static constexpr int BAR_SIZE = 100;
	static constexpr int BAR_WIDTH = 16;
	static constexpr int HP_ANIM_RAND = 3;
	static constexpr int SHOTBAR_WIDTH = 4;

	// 座標
	static constexpr int ENEMY_BAR_Y = 88;
	static constexpr int PLAYER_BAR_Y = 420;
	static constexpr int PANEL_POS_X_OFFSET = 8;
	static constexpr int BOSS_HP_Y_OFFSET = 64;

	// ライフ表示
	static constexpr int LIFE_LOW = 1;

	// バー色
	static constexpr VECTOR STAMINA_BAR_COLOR = { 255, 255, 0 };
	static constexpr VECTOR PLAYER_HP_BAR_COLOR = { 0, 255, 0 };
	static constexpr VECTOR SHOT_BAR_COLOR = { 255, 255, 0 };
	static constexpr VECTOR BOSS_HP_BAR_COLOR = { 210, 32, 0 };

	// HPバー演出
	static constexpr int HP_ANIM_DIVISOR = 2;
	static constexpr int HP_ANIM_RANDOM_OFFSET = 1;

	// バー間隔
	static constexpr int BAR_VERTICAL_GAP = 8;
	static constexpr int LIFE_BAR_GAP = 4;

	// ピクセルシェーダー
	static constexpr int PIXEL_SHADER_VALUE_INDEX = 2;
	static constexpr int PIXEL_SHADER_ENABLED = 1;
	static constexpr float FLASH_POWER = 0.8f;


	class BarStruct
	{
	public:
		int x;
		int y;
		int size;
		int width;
		int flashTimer;
		int animTimer;
		BarStruct();
		BarStruct(int x,int y);
		void Flash();
		void Anim();
		void Update();
	};

	ScorePanel(SceneGame& sceneGame);
	~ScorePanel();

	void Update();
	void Draw();

	/// <summary>
	/// 特定チュートリアルまたはボスキャラクターのHPの進行度をバーで描画する
	/// </summary>
	/// <param name="bar">バーの座標などを格納する構造体</param>
	/// <param name="useScreen">trueでピクセルシェーダーを反映するためのスクリーンを使用する</param>
	/// <param name="outScreen">描画先スクリーン(useScreenがtrueの場合のみ使用)</param>
	void DrawProgressBar(const BarStruct& bar,bool useScreen,int outScreen = -1);

	void DrawCharacterHpBar(const CharacterBase* target, int posX, int posY, int barSize, int barWidth, DrawUtility::IntRGB color);
	
	/// <summary>
	/// HPバー描画
	/// </summary>
	/// <param name="target">対象キャラ</param>
	/// <param name="bar">バーの座標などを格納する構造体</param>
	/// <param name="color"></param>
	/// <param name="useScreen">trueでピクセルシェーダーを反映するためのスクリーンを使用する</param>
	/// <param name="outScreen">描画先スクリーン(useScreenがtrueの場合のみ使用)</param>
	void DrawCharacterHpBar(const CharacterBase* target, const BarStruct& bar, DrawUtility::IntRGB color, bool useScreen, int outScreen = -1);

	/// <summary>
	/// プレイヤーのHPとボーナス弾のゲージを描画する
	/// </summary>
	/// <param name="target"></param>
	/// <param name="bar"></param>
	void DrawPlayerHpBar(const Player& target, const BarStruct& bar, bool useScreen, int outScreen = -1);


	/// <summary>
	/// プレイヤーのHPバーを光らせる
	/// </summary>
	void PlayerBarFlash();
	/// <summary>
	/// プレイヤーのHPバーを震えさせる
	/// </summary>
	void PlayerBarAnim();

	/// <summary>
	/// 敵のHPバーを光らせる
	/// </summary>
	void EnemyBarFlash();
	/// <summary>
	/// 敵のHPバーを震えさせる
	/// </summary>
	void EnemyBarAnim();
private:

	SceneGame& sceneGame_;

	Trans* trans_;

	//フレーム数
	int counter_;

	int lifeImg_;

	/// <summary>
	/// 背景
	/// </summary>
	int bgImg_;

	int barScreen_;

	PixelShaderMaterial* barPSMaterial_;
	PixelShaderRenderer* barPSRenderer_;

	BarStruct bossHpBar_;
	BarStruct playerHpBar_;

	/// <summary>
	/// デストラクタによって呼ばれる
	/// </summary>
	void Release();
};

