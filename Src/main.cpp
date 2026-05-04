//-----
//ゲーム制作:
//氏名　　　:2316020田中峻介
//-----
#include <DxLib.h>//ライブラリ
#include"Application.h"


//メモリリーク検出

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//WinMain関数
//-----
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	AddFontResourceExA((Application::PATH_FONT + "GenEiPOPle_v1.0/GenEiPOPle-Bk.ttf").c_str(), FR_PRIVATE, NULL);
	//メモリリーク検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//インスタンス作成
	Application::CreateInstance();

	//ゲームループ
	Application::GetInstance().Run();

	//終了
	Application::GetInstance().Release();

	return 0;//システム終了
}