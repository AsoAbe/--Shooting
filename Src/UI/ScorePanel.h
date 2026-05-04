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
	static constexpr int LIFE_IMG_SIZE = 16;
	static constexpr int LIFE_IMG_HALF = LIFE_IMG_SIZE / 2;

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

