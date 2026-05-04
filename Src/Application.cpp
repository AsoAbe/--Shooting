#include <DxLib.h>
#include <Windows.h>
#include <EffekseerForDXLib.h>
#include "./Manager/SceneManager.h"
#include "./Manager/InputManager.h"
#include "./Manager/MessageManager.h"
#include "./Manager/SoundManager.h"
#include "./Manager/ScoreManager.h"
#include "./Fader.h"
#include"Application.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

Application* Application::instance_ = nullptr;
const std::string Application::PATH_MODEL = "Data/Model/";
const std::string Application::PATH_IMAGE = "Data/Image/";
const std::string Application::PATH_EFFECT = "Data/Effect/";
const std::string Application::PATH_SOUND = "Data/Sound/";
const std::string Application::PATH_STAGE = "Data/Stage/";
const std::string Application::PATH_FONT = "Data/Font/";
const std::string Application::PATH_SHADER = "Data/Shader/";
const std::string Application::PATH_BGM = "Data/Bgm/";

//コンストラクタ
Application::Application(void)
{
	time_ = 0;
	lastFrameTime_ = 0;
	frameCount_ = 0;
	fpsUpdateTime_ = 0;
	fps_ = 0;
}
//デストラクタ
Application::~Application(void)
{

}

//インスタンスの生成(シングルトン)
void Application::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new Application();
	}
	instance_->Init();
}
//インスタンスを返す
//参照型にしているがポインタ型でもよい
Application& Application::GetInstance(void)
{
	return *instance_;
}

//インスタンスを破棄
void Application::Destroy(void)
{
	//インスタンスを削除
	delete instance_;
	//領域を初期化
	instance_ = nullptr;
}

const Size& Application::GetWindowSize() const
{
	return windoSize_;
}
//初期化
bool Application::Init(void)
{
	//システム処理
	//-----
	windoSize_ = { SCREEN_SIZE_X, SCREEN_SIZE_Y };
	SetWindowText("東方シューティング");//ウインドウのタイトル
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 32);//サイズと色
	ChangeWindowMode(true);//表示方法

	//effekseer用
	SetUseDirect3DVersion(DX_DIRECT3D_11);


	SetZBufferBitDepth(256);
	SetCreateDrawValidGraphZBufferBitDepth(256);
	SetFullSceneAntiAliasingMode(4, 2);
	SetDrawValidMultiSample(4, 2);
	if (DxLib_Init() == -1)//初期化
	{
		OutputDebugString("DxLibの初期化失敗");
		return false;
	}

	if (Effekseer_Init(2000) == -1)
	{
		OutputDebugString("Effekseerの初期化失敗");
		return false;
	}

	//effekseer用
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	//インスタンス
	//-----

	//サウンドマネージャー
	SoundManager::CreateInstance();

	//シーンマネージャ
	SceneManager::CreateInstance();

	// キー制御初期化
	SetUseDirectInputFlag(true);
	//インプットマネージャー
	InputManager::CreateInstance();

	//メッセージマネージャー
	MessageManager::CreateInstance();

	//スコアマネージャー
	ScoreManager::CreateInstance();


	return true;
}

//ゲームループ処理
void Application::Run(void)
{
	
	while (ProcessMessage() == 0)
	{
		Sleep(1);//システムに処理を返す

		//現在の時間を取得

		time_ = GetNowCount();

		//fpsの秒数が経過していたら処理を実行する
		if (time_ - lastFrameTime_ >= FRAME_RATE)
		{
			if (CheckHitKey(KEY_INPUT_ESCAPE) == 1)
			{
				if (MessageBox(NULL,TEXT("ゲームを終了しますか?"),TEXT("Esc"), 0x00000041L) == 1)
				{
					//while文を抜ける
					return;
				}
			}

			lastFrameTime_ = time_;
			frameCount_++;

			InputManager::GetInstance().Update();

			SceneManager::GetInstance().Update();

			MessageManager::GetInstance().Update();

			SoundManager::GetInstance().Update();
			
			//描画
			SetDrawScreen(DX_SCREEN_BACK);
			ClearDrawScreen();

			SceneManager::GetInstance().Draw();
			MessageManager::GetInstance().Draw();
			SoundManager::GetInstance().Draw();

			//フレームレート
			int diffTime = time_ - fpsUpdateTime_;
			//一秒経過ごとにfps値を更新
			if (diffTime > 1000)
			{
				float frameCountF = static_cast<float>(frameCount_ * 1000);

				fps_ = frameCountF / diffTime;

				frameCount_ = 0;
				fpsUpdateTime_ = time_;
			}

#ifdef _DEBUG
			//fpsを表示
			DrawFormatString(SCREEN_SIZE_X - GetDrawFormatStringWidth("FPS : %.1f", fps_)
				, 0
				, 0xFFFFFF, "FPS : %.1f", fps_);
#endif

			ScreenFlip();
		}
	}
}

//解放
bool Application::Release(void)
{
	//シーンマネージャ解放処理
	SceneManager::GetInstance().Release();
	//InputManager解放処理
	InputManager::GetInstance().Release();
	//MessageManager解放処理
	MessageManager::GetInstance().Release();

	//SoundManager解放処理
	SoundManager::GetInstance().Release();

	//ScoreManager解放処理
	ScoreManager::GetInstance().Release();

	Destroy();

	//Effkseer終了処理
	Effkseer_End();
	//システム終了処理
	DxLib_End();
	
	return true;
}