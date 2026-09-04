#include<DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include"SceneGameOver.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//初期化
bool SceneGameOver::Init(void)
{
	//タイトル画像
	return true;
}
//更新
void SceneGameOver::Update(void)
{
	if (CheckHitKey(KEY_INPUT_SPACE) == KEY_PRESSED)
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE, true);
	}
}
//描画
void SceneGameOver::Draw(void)
{
	DrawBox(0, 0, Application::SCREEN_SIZE_X - GAME_OVER_DRAW_MARGIN, Application::SCREEN_SIZE_Y - GAME_OVER_DRAW_MARGIN, GAME_OVER_COLOR, true);
}
//解放
bool SceneGameOver::Release(void)
{
	return true;
}