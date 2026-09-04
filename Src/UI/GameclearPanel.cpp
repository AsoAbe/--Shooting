#include "../Application.h"
#include "../Common/DrawUtility.h"
#include "../Manager/InputManager.h"
#include "../Scene/SceneGame.h"
#include "../Manager/ScoreManager.h"
#include "../Common/ScoreData.h"
#include "GameclearPanel.h"

GameclearPanel::GameclearPanel(SceneGame& sceneGame, const ScoreData& score):PanelBase(sceneGame), score_(score), counter_(INITIAL_COUNTER)
{
	
}

GameclearPanel::~GameclearPanel()
{
	Release();
}

void GameclearPanel::Update()
{
	counter_++;

	
	
	if (counter_ < GAMECLEAR_PANEL_MIN_TIME)
	{
		
		return;
	}
	
	InputManager& inputM = InputManager::GetInstance();
	if (inputM.IsTrgDown(KEY_INPUT_SPACE))
	{
		
		sceneGame_.BackToTitle();
	}
}

void GameclearPanel::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, FULL_ALPHA / HALF_DIVISOR);
	DrawBox(DRAW_POS_START, DRAW_POS_START, Application::MAINGAME_SIZE_X, Application::SCREEN_SIZE_Y, DRAW_COLOR_BLACK, true);
	SetDrawBlendMode(DX_BLENDGRAPHTYPE_NORMAL, DRAW_POS_START);
	DrawUtility::DrawStringCenterScreen("GameClear", DRAW_COLOR_WHITE, TEXT_Y, Application::MAINGAME_SIZE_X);

	std::string text = "今回のタイム : %d";

	//スコアのテキスト部分を描画
#pragma region SCORE_TEXT
	int drawX = SCORE_TEXT_X;
	int drawY = PanelBase::SCORE_TEXT_Y;
	int drawColor = DRAW_COLOR_WHITE;
	//今回のタイム
	DrawFormatString(drawX, drawY, DRAW_COLOR_WHITE, "今回のタイム         ");
	drawY += SCORE_TIME_ROW_OFFSET;

	//クリアボーナス
	DrawFormatString(drawX, drawY, DRAW_COLOR_WHITE, "クリアボーナス       ");
	drawY += DrawUtility::DEFAULT_TEXT_SIZE;

	//タイムボーナス
	DrawFormatString(drawX, drawY, DRAW_COLOR_WHITE, "タイムボーナス       ");
	drawY += DrawUtility::DEFAULT_TEXT_SIZE;

	//ライフボーナス
	DrawFormatString(drawX, drawY, DRAW_COLOR_WHITE, "ライフボーナス       ");
	drawY += DrawUtility::DEFAULT_TEXT_SIZE;
	if (score_.perfectScore > DRAW_POS_START)
	{
		//ノーダメージボーナス 
		//有効な場合のみ描画
		DrawFormatString(drawX, drawY, DRAW_COLOR_WHITE, "ノーダメージボーナス  ");
	}  

	drawY = SCORE_RESULT_TEXT_Y;

	int highScore = ScoreManager::GetInstance().GetHighScore();
	//ハイスコアを更新した場合、色を変更して描画
	if (score_.resultScore >= highScore)
	{
		drawColor = DRAW_COLOR_YELLOW;
	}
	//ハイスコア
	DrawFormatString(drawX, drawY, drawColor, "ハイスコア           ");
	drawY += DrawUtility::DEFAULT_TEXT_SIZE;
	//今回のスコア
	DrawFormatString(drawX, drawY, drawColor, "今回のスコア         ");
#pragma endregion

	//XをずらしてYを上に戻し、スコアテキストの数字部分を描画
#pragma region SCORE_NUM

	drawX = SCORE_NUM_TEXT_X;;
	drawY = PanelBase::SCORE_TEXT_Y;
	drawColor = DRAW_COLOR_WHITE;
	//今回のタイム
	DrawFormatString(drawX, drawY, DRAW_COLOR_WHITE, ": %.0f", score_.timeS);
	drawY += SCORE_TIME_ROW_OFFSET;

	//クリアボーナス
	DrawFormatString(drawX, drawY, DRAW_COLOR_WHITE, ": %d", score_.winScore);
	drawY += DrawUtility::DEFAULT_TEXT_SIZE;

	//タイムボーナス
	DrawFormatString(drawX, drawY, DRAW_COLOR_WHITE, ": %d", score_.timeScore);
	drawY += DrawUtility::DEFAULT_TEXT_SIZE;

	//ライフボーナス
	DrawFormatString(drawX, drawY, DRAW_COLOR_WHITE, ": %d × %d", score_.lifeScore, score_.lifeScoreNum);
	drawY += DrawUtility::DEFAULT_TEXT_SIZE;
	if (score_.perfectScore > DRAW_POS_START)
	{
		//ノーダメージボーナス
		//有効な場合のみ描画
		DrawFormatString(drawX, drawY, DRAW_COLOR_WHITE, ": %d", score_.perfectScore);
	}

	//リザルト
	drawY = SCORE_RESULT_TEXT_Y;
	//ハイスコアを更新した場合、色を変更して描画
	if (score_.resultScore >= highScore)
	{
		drawColor = DRAW_COLOR_YELLOW;
	}
	//ハイスコア           
	DrawFormatString(drawX, drawY, drawColor, ": %d", highScore);
	drawY += DrawUtility::DEFAULT_TEXT_SIZE;
	//今回のスコア
	DrawFormatString(drawX, drawY, drawColor, ": %d", score_.resultScore);

#pragma endregion

	if (counter_ < GAMECLEAR_PANEL_MIN_TIME)
	{
		return;
	}
	if (DrawUtility::Blink(counter_))
	{
		DrawUtility::DrawStringCenterScreen("スペースキーでタイトルに戻る", DRAW_COLOR_WHITE, PanelBase::TITLESCENE_TEXT_Y, Application::MAINGAME_SIZE_X);
	}
}

void GameclearPanel::SetActive(bool b)
{
	PanelBase::SetActive(b);
}

void GameclearPanel::Release()
{
	//解放処理
}
