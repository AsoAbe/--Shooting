#include "../Application.h"
#include "../Common/DrawUtility.h"
#include "../Manager/InputManager.h"
#include "../Scene/SceneGame.h"
#include "../Manager/ScoreManager.h"
#include "../Common/ScoreData.h"
#include "GameclearPanel.h"

namespace
{
	constexpr int GAMECLEAR_PANEL_MIN_TIME = PanelBase::PANEL_MIN_TIME * 2;
}
GameclearPanel::GameclearPanel(SceneGame& sceneGame, const ScoreData& score):PanelBase(sceneGame), score_(score), counter_(0)
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
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 / 2);
	DrawBox(0, 0, Application::MAINGAME_SIZE_X, Application::SCREEN_SIZE_Y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDGRAPHTYPE_NORMAL, 0);
	DrawUtility::DrawStringCenterScreen("GameClear", 0xffffff, TEXT_Y, Application::MAINGAME_SIZE_X);

	std::string text = "今回のタイム : %d";
	//int textWidth = GetDrawFormatStringWidth(text.c_str(), score_);//SCORE_TEXT_Xの値を求めた手順
	//int drawX = (Application::MAINGAME_SIZE_X - textWidth) / 2;

	//スコアのテキスト部分を描画
#pragma region SCORE_TEXT
	int drawX = SCORE_TEXT_X;
	int drawY = PanelBase::SCORE_TEXT_Y;
	int drawColor = 0xffffff;
	//今回のタイム
	DrawFormatString(drawX, drawY, 0xffffff, "今回のタイム         ");
	drawY += 20;

	//クリアボーナス
	DrawFormatString(drawX, drawY, 0xffffff, "クリアボーナス       ");
	drawY += DrawUtility::DEFAULT_TEXT_SIZE;

	//タイムボーナス
	DrawFormatString(drawX, drawY, 0xffffff, "タイムボーナス       ");
	drawY += DrawUtility::DEFAULT_TEXT_SIZE;

	//ライフボーナス
	DrawFormatString(drawX, drawY, 0xffffff, "ライフボーナス       ");
	drawY += DrawUtility::DEFAULT_TEXT_SIZE;
	if (score_.perfectScore > 0)
	{
		//ノーダメージボーナス 
		//有効な場合のみ描画
		DrawFormatString(drawX, drawY, 0xffffff, "ノーダメージボーナス  ");
	}  
	//drawY += DrawUtility::DEFAULT_TEXT_SIZE*2;//ここまでの結果を定数化

	//リザルト

	drawY = SCORE_RESULT_TEXT_Y;

	int highScore = ScoreManager::GetInstance().GetHighScore();
	//ハイスコアを更新した場合、色を変更して描画
	if (score_.resultScore >= highScore)
	{
		drawColor = 0xffff00;
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
	drawColor = 0xffffff;
	//今回のタイム
	DrawFormatString(drawX, drawY, 0xffffff, ": %.0f", score_.timeS);
	drawY += 20;

	//クリアボーナス
	DrawFormatString(drawX, drawY, 0xffffff, ": %d", score_.winScore);
	drawY += DrawUtility::DEFAULT_TEXT_SIZE;

	//タイムボーナス
	DrawFormatString(drawX, drawY, 0xffffff, ": %d", score_.timeScore);
	drawY += DrawUtility::DEFAULT_TEXT_SIZE;

	//ライフボーナス
	DrawFormatString(drawX, drawY, 0xffffff, ": %d × %d", score_.lifeScore, score_.lifeScoreNum);
	drawY += DrawUtility::DEFAULT_TEXT_SIZE;
	if (score_.perfectScore > 0)
	{
		//ノーダメージボーナス
		//有効な場合のみ描画
		DrawFormatString(drawX, drawY, 0xffffff, ": %d", score_.perfectScore);
	}

	//リザルト

	drawY = SCORE_RESULT_TEXT_Y;
	//ハイスコアを更新した場合、色を変更して描画
	if (score_.resultScore >= highScore)
	{
		drawColor = 0xffff00;
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
		DrawUtility::DrawStringCenterScreen("スペースキーでタイトルに戻る", 0xffffff, PanelBase::TITLESCENE_TEXT_Y, Application::MAINGAME_SIZE_X);
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
