#include "../Application.h"
#include "../Common/DrawUtility.h"
#include "../Scene/SceneGame.h"
#include "../Manager/ObjectManager.h"
#include "../Object/Player.h"
#include"../Common/Vector2.h"
#include"../Common/Trans.h"
#include "../Common/PixelShaderMaterial.h"
#include "../Common/PixelShaderRenderer.h"
#include "../Manager/SceneManager.h"

#include "ScorePanel.h"

using IntRGB = DrawUtility::IntRGB;

namespace
{
	constexpr int FRAME_WIDTH_BAR = DrawUtility::FRAME_WIDTH_BAR;

	/// <summary>
	/// 一時スクリーンのサイズ
	/// </summary>
	constexpr int BAR_SCREEN_SIZE = 256;

	/// <summary>
	/// hpバー演出の時間(60FPSのフレーム数)
	/// </summary>
	constexpr int BAR_FLASH_TIME = 30;
	/// <summary>
	/// hpバー演出の時間(60FPSのフレーム数)
	/// </summary>
	constexpr int BAR_ANIM_TIME = 12;
	/// <summary>
	/// デフォルトの長さ
	/// </summary>
	static constexpr int BAR_SIZE = 100;
	/// <summary>
	/// デフォルトの幅
	/// </summary>
	static constexpr int BAR_WIDTH = 16;

	constexpr int HP_ANIM_RAND = 3;
}

ScorePanel::BarStruct::BarStruct()
{
	this->x = DRAW_POS_START;
	this->y = DRAW_POS_START;
	this->size = BAR_SIZE;
	this->width = BAR_WIDTH;
	this->animTimer = INITIAL_COUNTER;
	this->flashTimer = INITIAL_COUNTER;
}

ScorePanel::BarStruct::BarStruct(int x, int y)
{
	this->x = x;
	this->y = y;
	this->size = BAR_SIZE;
	this->width = BAR_WIDTH;
	this->animTimer = INITIAL_COUNTER;
	this->flashTimer = INITIAL_COUNTER;
}

void ScorePanel::BarStruct::Flash()
{
	flashTimer = BAR_FLASH_TIME;
}

void ScorePanel::BarStruct::Anim()
{
	//タイマーに値をセット
	animTimer = BAR_ANIM_TIME;
	//光る演出をキャンセル
	flashTimer = INITIAL_COUNTER;
}

void ScorePanel::BarStruct::Update()
{
	if (animTimer > INITIAL_COUNTER)
	{
		animTimer--;
	}
	else
	{
		animTimer = INITIAL_COUNTER;
	}
	if (flashTimer > INITIAL_COUNTER)
	{
		flashTimer--;
	}
	else
	{
		flashTimer = INITIAL_COUNTER;
	}
}

ScorePanel::ScorePanel(SceneGame& sceneGame) :sceneGame_(sceneGame)
{
	lifeImg_ = LoadGraph((Application::PATH_IMAGE + "Life.png").c_str());
	bgImg_ = LoadGraph((Application::PATH_IMAGE + "Image_fx.jpg").c_str());
	counter_ = INITIAL_COUNTER;
	barPSMaterial_ = new PixelShaderMaterial("psFlash.cso", 2);
	constexpr float FLASH_POW = 0.8f;
	barPSMaterial_->SetValue(FLASH_POW, 1);
	barPSRenderer_ = new PixelShaderRenderer(*barPSMaterial_);

	barPSRenderer_->MakeSquereVertex(DRAW_POS_START, DRAW_POS_START, BAR_SCREEN_SIZE, BAR_SCREEN_SIZE);

	barScreen_ = MakeScreen(BAR_SCREEN_SIZE, BAR_SCREEN_SIZE, TRUE);
	trans_ = nullptr;
}

ScorePanel::~ScorePanel()
{
	//解放処理
	Release();
}

void ScorePanel::Update()
{
	counter_++;

	bossHpBar_.Update();
	playerHpBar_.Update();
}

void ScorePanel::Draw()
{
	//メインスクリーン
	int mainScreen = SceneManager::GetInstance().GetMainScreen();

	DrawExtendGraph(DRAW_POS_START, DRAW_POS_START, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, bgImg_, false);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, BLEND_ALPHA_BACKGROUND);
	DrawBox(DRAW_POS_START, DRAW_POS_START, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, DRAW_COLOR_BLACK, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, DRAW_BLEND_RESET);

	//バーのy始点
	int ENbarY = ENEMY_BAR_Y;  //エネミー用スコアバーの高さ
	int barY = PLAYER_BAR_Y;    //プレイヤー用スコアバーの高さ

	const Player& player = *(sceneGame_.GetOManager()->GetPlayer());
	//右側パネルの始点のx
	constexpr int PANEL_R_POS_X = Application::MAINGAME_POS_X + Application::MAINGAME_SIZE_X+ PANEL_POS_X_OFFSET;
#pragma region PlayerHp
	
	//テキスト描画
	DrawString(PANEL_R_POS_X, barY - DrawUtility::DEFAULT_TEXT_SIZE, "PLAYER", DRAW_COLOR_WHITE);

	//バー描画
	playerHpBar_.x = PANEL_R_POS_X;
	playerHpBar_.y = barY;
	DrawPlayerHpBar(player, playerHpBar_,true,mainScreen);
	barY += BAR_WIDTH+ PANEL_POS_X_OFFSET;

	//滑空スタミナ
	DrawUtility::DrawBarGlossy({ PANEL_R_POS_X ,barY},
		PANEL_R_POS_X + BAR_SIZE, BAR_WIDTH,
		{255,255,0}, player.GetStamina(), player.GetStaminaMax());
	barY += BAR_WIDTH+4+ LIFE_IMG_HALF;

	//ライフ
	if (player.GetLife() > LIFE_LOW || DrawUtility::Blink(counter_))
	{
		//life一定以下で点滅として使う
		DrawUtility::DrawBarImg(
			PANEL_R_POS_X + LIFE_IMG_HALF, barY,
			player.GetLife(), player.GetLifeMax(), lifeImg_, INVALID_HANDLE, LIFE_IMG_SIZE, INVALID_HANDLE);
	}
#pragma endregion

	//ボスhp描画
	const CharacterBase* boss = sceneGame_.GetOManager()->GetTargetCharacter();
	constexpr int BOSS_HP_Y = BOSS_HP_Y_OFFSET;
	ENbarY += BOSS_HP_Y;
	DrawString(PANEL_R_POS_X, ENbarY, "ENEMY", DRAW_COLOR_WHITE);
	ENbarY += DrawUtility::DEFAULT_TEXT_SIZE;
	bossHpBar_.x = PANEL_R_POS_X;
	bossHpBar_.y = ENbarY;
	DrawCharacterHpBar(boss, bossHpBar_, { 210,32,0 }, true, mainScreen);
}

void ScorePanel::DrawProgressBar(const BarStruct& bar, bool useScreen, int outScreen)
{
	if (!(sceneGame_.IsTutorial()))
	{
		const CharacterBase* boss = sceneGame_.GetOManager()->GetTargetCharacter();
		DrawCharacterHpBar(boss, bar, {210,32,0}, useScreen, outScreen);//0xd22000
	}
	int drawX = bar.x;
	int drawY = bar.y;
	if (useScreen)
	{
		//スクリーン設定
		SetDrawScreen(barScreen_);
		ClearDrawScreen();
		//描画座標を左上に
		drawX = DRAW_POS_START;
		drawY = DRAW_POS_START;
		//マテリアルに値セット
		barPSMaterial_->SetValue(static_cast<float>(bar.flashTimer) / BAR_FLASH_TIME);
	}
	else
	{
		if (bar.animTimer > INITIAL_COUNTER)
		{
			//演出中は上下にランダムに動かす(useScreen == false)
			drawY += HP_ANIM_RAND / 2 - GetRand(HP_ANIM_RAND - 1);
		}
	}
	//バーを描画
	if (sceneGame_.IsGrazeTutorial())
	{
		DrawUtility::DrawBar({ drawX ,drawY },
			drawX + bar.size, bar.width,
			{255,255,0}, sceneGame_.GetGrazeTutorialProgress(),1);
	}
	if (useScreen)
	{
		drawX = bar.x;
		drawY = bar.y;
		if (bar.animTimer > INITIAL_COUNTER)
		{
			//演出中は上下にランダムに動かす(useScreen == true)
			drawY += HP_ANIM_RAND / 2 - GetRand(HP_ANIM_RAND - 1);
		}
		//barスクリーンを描画
		//アルファ値を有効化
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, FULL_ALPHA);
		barPSRenderer_->SetPosAndDraw(barScreen_, outScreen, drawX, drawY);
		//ブレンドモードを戻す
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, FULL_ALPHA);
	}

}

void ScorePanel::DrawCharacterHpBar(const CharacterBase* target, int posX, int posY, int barSize, int barWidth, DrawUtility::IntRGB color)
{
	if (target != nullptr)
	{
		int hpBarValue = target->GetHp();
		if (sceneGame_.IsTutorial())
		{
			//チュートリアル中はHP表示を満タンで固定
			hpBarValue = target->GetHpMax();
		}
		
		DrawUtility::DrawBarGlossy({ posX ,posY },
				 posX + barSize, barWidth,
				color, hpBarValue, target->GetHpMax());
	}
}

void ScorePanel::DrawCharacterHpBar(const CharacterBase* target, const BarStruct& bar, DrawUtility::IntRGB color, bool useScreen, int outScreen)
{
	if (target == nullptr)
	{
		return;
	}
	int drawX = bar.x;
	int drawY = bar.y;
	if (useScreen)
	{
		//スクリーン設定
		SetDrawScreen(barScreen_);
		ClearDrawScreen();
		//描画座標を左上に
		drawX = DRAW_POS_START;
		drawY = DRAW_POS_START;
		//マテリアルに値セット
		barPSMaterial_->SetValue(static_cast<float>(bar.flashTimer) / BAR_FLASH_TIME);
	}
	else
	{
		if (bar.animTimer > INITIAL_COUNTER)
		{
			//演出中は上下にランダムに動かす(useScreen == false)
			drawY += HP_ANIM_RAND / 2 - GetRand(HP_ANIM_RAND - 1);
		}
	}
	//バーを描画
	int hpBarValue = target->GetHp();
	if (sceneGame_.IsTutorial())
	{
		//チュートリアル中はHP表示を満タンで固定
		hpBarValue = target->GetHpMax();
	}
	DrawUtility::DrawBarGlossy({ drawX ,drawY },
		drawX + bar.size, bar.width,
		color, hpBarValue, target->GetHpMax());
	if (useScreen)
	{
		drawX = bar.x;
		drawY = bar.y;
		if (bar.animTimer > INITIAL_COUNTER)
		{
			//演出中は上下にランダムに動かす(useScreen == true)
			drawY += HP_ANIM_RAND / 2 - GetRand(HP_ANIM_RAND - 1);
		}
		//barスクリーンを描画
		//アルファ値を有効化
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, FULL_ALPHA);
		barPSRenderer_->SetPosAndDraw(barScreen_, outScreen, drawX, drawY);
		//ブレンドモードを戻す
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, FULL_ALPHA);
	}
}

void ScorePanel::DrawPlayerHpBar(const Player& target, const BarStruct& bar, bool useScreen, int outScreen)
{
	int drawX = bar.x;
	int drawY = bar.y;
	if (useScreen)
	{
		//一時スクリーンの左上に合わせる
		drawX = DRAW_POS_START;
		drawY = DRAW_POS_START;
		//マテリアルにタイマー反映
		barPSMaterial_->SetValue(static_cast<float>(bar.flashTimer) / BAR_FLASH_TIME);
		//スクリーン
		SetDrawScreen(barScreen_);
		ClearDrawScreen();
	}
	else
	{
		if (bar.animTimer > INITIAL_COUNTER)
		{
			//演出中は上下にランダムに動かす(useScreen == false)
			drawY += HP_ANIM_RAND / 2 - GetRand(HP_ANIM_RAND - 1);
		}
	}
	//プレイヤーhp
	DrawCharacterHpBar(&target, drawX, drawY, bar.size, bar.width, {0,255,0});
	//プレイヤー弾
	drawX += FRAME_WIDTH_BAR;
	drawY += FRAME_WIDTH_BAR- SHOTBAR_WIDTH;
	DrawUtility::DrawBar({ drawX,drawY +bar.width },
		drawX+  bar.size, SHOTBAR_WIDTH,
		{255,255,0}, target.GetBonusShot(), Player::GRAZE_BONUS_SHOT);
	if (useScreen)
	{
		drawX = bar.x;
		drawY = bar.y;
		if (bar.animTimer > INITIAL_COUNTER)
		{
			//演出中は上下にランダムに動かす(useScreen == true)
			drawY += HP_ANIM_RAND / 2 - GetRand(HP_ANIM_RAND - 1);
		}
		//barスクリーンを描画
		//アルファ値を有効化
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, FULL_ALPHA);
		barPSRenderer_->SetPosAndDraw(barScreen_, outScreen, drawX, drawY);
		//ブレンドモードを戻す
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, FULL_ALPHA);
	}
}

void ScorePanel::PlayerBarFlash()
{
	playerHpBar_.Flash();
}

void ScorePanel::PlayerBarAnim()
{
	playerHpBar_.Anim();
}

void ScorePanel::EnemyBarFlash()
{
	bossHpBar_.Flash();
}

void ScorePanel::EnemyBarAnim()
{
	bossHpBar_.Anim();
}

void ScorePanel::Release()
{
	delete barPSMaterial_;
	delete barPSRenderer_;
	barPSMaterial_ = nullptr;
	barPSRenderer_ = nullptr;

	DeleteGraph(barScreen_);
	barScreen_ = INVALID_HANDLE;

	DeleteGraph(lifeImg_);
	lifeImg_ = INVALID_HANDLE;

	DeleteGraph(bgImg_);
	bgImg_ = INVALID_HANDLE;
}
