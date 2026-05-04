#include<DxLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include"SceneGameOver.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//コンストラクタ
//SceneTitle::SceneTitle(void)
//{
//	sceneManager_ = nullptr;
//	titleImage = 0;
//}
////デストラクタ
//SceneTitle::~SceneTitle(void)
//{
//
//}

//初期化
bool SceneGameOver::Init(void)
{
	/*
	//タイトル画像
	titleImage = LoadGraph("****.png");
	if (titleImage == -1)
	{
		//画像読み込み失敗
		OutputDebugString("タイトル画像読み込み失敗");
		return false;
	}
	*/

	//タイトル画像
	return true;
}
//更新
void SceneGameOver::Update(void)
{
	if (CheckHitKey(KEY_INPUT_SPACE) == 1)
	{
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE, true);
	}
}
//描画
void SceneGameOver::Draw(void)
{
	DrawBox(0, 0, Application::SCREEN_SIZE_X - 100, Application::SCREEN_SIZE_Y - 100, 0xFFFF00, true);
}
//解放
bool SceneGameOver::Release(void)
{
	//DeleteGraph(titleImage);
	return true;
}