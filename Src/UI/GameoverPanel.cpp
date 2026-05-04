#include <DxLib.h>
#include "../Application.h"
#include "../Common/DrawUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/ScoreManager.h"
#include "../Scene/SceneGame.h"
#include "../Common/ScoreData.h"
#include "GameoverPanel.h"

namespace
{
	constexpr int GAMEOVER_PANEL_MIN_TIME = PanelBase::PANEL_MIN_TIME * 2;
}
GameoverPanel::GameoverPanel(SceneGame& sceneGame, const ScoreData& score) :PanelBase(sceneGame), counter_(0), score_(score)
{
}

GameoverPanel::~GameoverPanel()
{
	Release();
}

void GameoverPanel::Update()
{
	counter_++;
	if (counter_ < GAMEOVER_PANEL_MIN_TIME)
	{
		return;
	}
	InputManager& inputM = InputManager::GetInstance();
	if (inputM.IsTrgDown(KEY_INPUT_SPACE))
	{
		sceneGame_.BackToTitle();
	}
}

void GameoverPanel::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 / 2);
	DrawBox(0, 0, Application::MAINGAME_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDGRAPHTYPE_NORMAL, 0);

	DrawUtility::DrawStringCenterScreen("GameOver", 0xffffff, TEXT_Y, Application::MAINGAME_SIZE_X);

	int drawX = SCORE_TEXT_X;
	int drawY = SCORE_RESULT_TEXT_Y;
	
	int drawColor = 0xffffff;
	if (score_.resultScore >= 0)
	{
		//スコア表示が有効な場合

		////パーセントでボスの残りHPを描画
		//DrawFormatString(drawX, SCORE_TEXT_Y, 0xffffff, "ボスの残りHP : %.0f%%", (1-score_.targetProgression) * 100.0f);

		int highScore = ScoreManager::GetInstance().GetHighScore();
		//ハイスコアを更新した場合、色を変更して描画
		if (score_.resultScore >= highScore)
		{
			drawColor = 0xffff00;
		}

		//スコアテキスト描画
#pragma region SCORE_TEXT
		//ハイスコア
		DrawFormatString(drawX, drawY, drawColor, "ハイスコア", highScore);
		drawY += DrawUtility::DEFAULT_TEXT_SIZE;
		//今回のスコア
		DrawFormatString(drawX, drawY, drawColor, "今回のスコア", score_.resultScore);
#pragma endregion

		//XをずらしてYを上に戻し、スコアテキストの数字部分を描画
#pragma region SCORE_NUM_TEXT
		drawX = SCORE_NUM_TEXT_X;
		drawY = SCORE_RESULT_TEXT_Y;
		//ハイスコア
		DrawFormatString(drawX, drawY, drawColor, ": %d", highScore);
		drawY += DrawUtility::DEFAULT_TEXT_SIZE;
		//今回のスコア 
		DrawFormatString(drawX, drawY, drawColor, ": %d", score_.resultScore);
#pragma endregion
	}

	if (counter_ < GAMEOVER_PANEL_MIN_TIME)
	{
		return;
	}
	if (DrawUtility::Blink(counter_))
	{
		DrawUtility::DrawStringCenterScreen("スペースキーでタイトルに戻る", 0xffffff, PanelBase::TITLESCENE_TEXT_Y, Application::MAINGAME_SIZE_X);
	}
}

void GameoverPanel::Release()
{
	//解放処理
}
