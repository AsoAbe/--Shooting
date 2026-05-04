#include <EffekseerForDXLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/ObjectManager.h"
#include"../Manager/SoundManager.h"
#include "../UI/PanelBase.h"
#include "../UI/ScorePanel.h"
#include "../Object/Player.h"
#include "../Common/DrawUtility.h"

#include "../Common/PixelShaderMaterial.h"
#include "../Common/PixelShaderRenderer.h"

#include "SceneTutorial.h"

namespace
{
	constexpr int TUTORIALBOX_Y = 128;
	constexpr int BAR_WIDTH = 8;
	constexpr int TUTORIAL_BAR_Y = TUTORIALBOX_Y - 2 - DrawUtility::DEFAULT_TEXT_SIZE - BAR_WIDTH;
}

SceneTutorial::SceneTutorial():SceneGame()
{
	gameState_ = GAME_STATE::TUTORIAL_JUMP;
	shotTutorialTextFlag_ = false;
	progressBar_.y =TUTORIALBOX_Y - BAR_WIDTH;
	progressBar_.size = 160;
	progressBar_.width = BAR_WIDTH;
	preProgressScore_ = 0;
}

bool SceneTutorial::Init(void)
{
	bool ret =SceneGame::Init();
	//今のところ未使用
	//SoundManager::GetInstance().ChangeBGM(SoundManager::SOUND_ID::CURSOR,true);
	return ret;
}

void SceneTutorial::Update(void)
{
	SceneGame::Update();
	if (!(shotTutorialTextFlag_) &&IsGrazeTutorial())
	{
		if (oManager_->GetPlayer()->GetGrazeComboScore() >= TUTORIAL_SHOT_NUM)
		{
			//回避回数を満たしたらフラグをtrueに
			if (shotTutorialTextFlag_ == false)
			{
				SoundManager::GetInstance().PlaySE(SoundManager::SOUND_ID::SUCCESS);
				shotTutorialTextFlag_ = true;
			}
		}
	}

	Update_ProgressBar();
}

void SceneTutorial::Update_ProgressBar()
{
	//バーの値変化を監視して更新する
	if (IsGrazeTutorial() && !(shotTutorialTextFlag_))
	{
		int score = oManager_->GetPlayer()->GetGrazeCombo();
		if (preProgressScore_ < score)
		{
			progressBar_.Flash();
		}
		preProgressScore_ = oManager_->GetPlayer()->GetGrazeCombo();
	}
	progressBar_.Update();
}

void SceneTutorial::Draw()
{
	SceneManager& sceneManager = SceneManager::GetInstance();

	//パネル描画
	Draw_ScorePanel();

	//メインスクリーンに切り替え
	SetDrawScreen(mainGameScreen_);
	ClearDrawScreen();
	//hpBar描画後にこの処理を入れないとエフェクトに不具合
	sceneManager.GetCamera()->UpdateCameraPos();
	Effekseer_Sync3DSetting();
	//メイン描画
	Draw_MainGame();
	if (IsTutorial() && sceneManager.IsEndFade())
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 96);
		DrawBox(0, 0, Application::MAINGAME_SIZE_X, TUTORIALBOX_Y, 0x000000, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		switch (gameState_)
		{
		case SceneGame::GAME_STATE::TUTORIAL_JUMP:
			DrawString(0, 0, "現在のチュートリアル:滑空で敵の攻撃を避ける\nXキーでジャンプ。長押しするか空中でもう一度押すと滑空に切り替え。\n(Rキー長押しでスキップ)", 0xffff00);
			break;
		case SceneGame::GAME_STATE::TUTORIAL_SHOT:
			if (shotTutorialTextFlag_)
			{
				DrawFormatString(0, 0, 0xffff00,
					"現在のチュートリアル:カウンターショットで敵を倒す\n矢印キーで移動。\n当たらない程度にギリギリを狙って敵の攻撃を避けるとリングが表示されカウンター成立。\n成功するとZキーの射撃が強化される。(長押しで連射)\n(Rキー長押しでスキップ)");
				DrawFormatString(0, TUTORIAL_BAR_Y, 0xffff00,
					"(Zキーを押して射撃)");
			}
			else 
			{
				DrawFormatString(0, 0, 0xffff00,
					"現在のチュートリアル:カウンターショットで敵を倒す\n矢印キーで移動。\n当たらない程度にギリギリを狙って敵の攻撃を避けるとリングが表示されカウンター成立。\n成功するとZキーの射撃が強化される。(長押しで連射)\n(Rキー長押しでスキップ)");
				DrawFormatString(0, TUTORIAL_BAR_Y, 0xffff00,
					"(あと%d回)",
					(TUTORIAL_SHOT_NUM - oManager_->GetPlayer()->GetGrazeCombo()));
			}
			scorePanel_->DrawProgressBar(progressBar_,true,mainGameScreen_);

			break;
		case SceneGame::GAME_STATE::TUTORIAL_END:
			DrawString(0, 0, "チュートリアル達成!\nRキーで本番開始。", 0xffff00);
			break;
		default:
			break;
		}
	}
	if (panel_ != nullptr)
	{
		if (panel_->IsActive())
		{
			panel_->Draw();
		}
	}
	//メインスクリーンを描画
	int mainScreen = SceneManager::GetInstance().GetMainScreen();
	SetDrawScreen(mainScreen);
	mainPSRenderer_->Draw(mainGameScreen_, mainScreen);
}

bool SceneTutorial::CheckGrazeTutorial()
{
	if (!(IsGrazeTutorial()))
	{
		//対応のステート以外
		return false;
	}
	Player* player = oManager_->GetPlayer();
	CharacterBase* enemy = oManager_->GetTargetCharacter();
	bool ret = (shotTutorialTextFlag_ &&
		enemy->GetHpMax() - enemy->GetHp() > 0);
	if (ret)
	{
		NextTutorial();
	}
	return ret;
}

float SceneTutorial::GetGrazeTutorialProgress()const
{
	if (shotTutorialTextFlag_)
	{
		//完了済み
		return 1;
	}
	return static_cast<float>(oManager_->GetPlayer()->GetGrazeCombo())/TUTORIAL_SHOT_NUM;
}

bool SceneTutorial::IsTutorial() const
{
	return true;
}
