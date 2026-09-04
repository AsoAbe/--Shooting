#include <DxLib.h>
#include "../Application.h"
#include "../Common/DrawUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/ObjectManager.h"
#include "../Object/Player.h"
#include "../Scene/SceneGame.h"
#include "ContinuePanel.h"

ContinuePanel::ContinuePanel(SceneGame& sceneGame) :PanelBase(sceneGame), counter_(0)
{
}

ContinuePanel::~ContinuePanel()
{
	Release();
}

void ContinuePanel::Update()
{
	counter_++;
	if (counter_ < PANEL_MIN_TIME)
	{
		return;
	}
	InputManager& inputM = InputManager::GetInstance();
	if (inputM.IsTrgDown(KEY_INPUT_SPACE))
	{
		SetActive(false);
		//プレイヤー復活を呼び出す
		sceneGame_.Continue();
	}
}

void ContinuePanel::Draw()
{
	const Player& player = *(sceneGame_.GetOManager()->GetPlayer());
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, FULL_ALPHA / HALF_DIVISOR);
	DrawBox(DRAW_POS_START, DRAW_POS_START, Application::MAINGAME_SIZE_X, Application::SCREEN_SIZE_Y, DRAW_COLOR_BLACK, true);
	SetDrawBlendMode(DX_BLENDGRAPHTYPE_NORMAL, BLEND_ALPHA_RESET);
	DrawUtility::DrawStringCenterScreen("YOU DIED", DRAW_COLOR_WHITE, DEATH_TEXT_POS_Y, Application::MAINGAME_SIZE_X);
	std::string text = "残りライフ : %d";
	int textWidth = GetDrawFormatStringWidth(text.c_str(), player.GetLife());
	DrawFormatString((Application::MAINGAME_SIZE_X - textWidth) / HALF_DIVISOR, LIFE_TEXT_POS_Y, DRAW_COLOR_YELLOW, text.c_str(), player.GetLife());
	if (counter_ < PANEL_MIN_TIME)
	{
		return;
	}
	if (DrawUtility::Blink(counter_))
	{
		DrawUtility::DrawStringCenterScreen("スペースキーで復活", DRAW_COLOR_WHITE,CONTINUE_TEXT_POS_Y, Application::MAINGAME_SIZE_X);
	}
}

void ContinuePanel::SetActive(bool b)
{
	if (isActive_ == false)
	{
		//カウンターリセット
		counter_ = 0;
	}
	PanelBase::SetActive(b);
}

void ContinuePanel::Release()
{
	//解放処理
}
