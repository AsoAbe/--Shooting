#include <DxLib.h>
#include "../Application.h"
#include "../Common/DrawUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/ScoreManager.h"
#include "../Scene/SceneGame.h"
#include "../Common/ScoreData.h"
#include "GameoverPanel.h"

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
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, FULL_ALPHA / HALF_DIVISOR);
	DrawBox(0, 0, Application::MAINGAME_SIZE_X, Application::SCREEN_SIZE_Y, DRAW_COLOR_BLACK, true);
	SetDrawBlendMode(DX_BLENDGRAPHTYPE_NORMAL, DRAW_POS_START);

	DrawUtility::DrawStringCenterScreen("GameOver", DRAW_COLOR_WHITE, TEXT_Y, Application::MAINGAME_SIZE_X);

	int drawX = SCORE_TEXT_X;
	int drawY = SCORE_RESULT_TEXT_Y;
	
	int drawColor = DRAW_COLOR_WHITE;
	if (score_.resultScore >= SCORE_DISPLAY_INVALID)
	{
		//スコア表示が有効な場合
		int highScore = ScoreManager::GetInstance().GetHighScore();
		//ハイスコアを更新した場合、色を変更して描画
		if (score_.resultScore >= highScore)
		{
			drawColor = DRAW_COLOR_YELLOW;
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
