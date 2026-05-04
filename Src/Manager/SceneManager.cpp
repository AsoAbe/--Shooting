#include<DxLib.h>
#include<EffekseerForDXLib.h>
#include"../Fader.h"
#include"../Scene/SceneTitle.h"
#include "../Manager/InputManager.h"
#include"../Manager/SceneManager.h"
#include"../Manager/SoundManager.h"
#include"../Manager/Camera.h"
#include"../Scene/SceneGame.h"
#include"../Scene/SceneGameOver.h"
#include"../Scene/SceneTutorial.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
SceneManager* SceneManager::instance_ = nullptr;
//コンストラクタ
SceneManager::SceneManager(void)
{
	mainScreen_ = -1;
	fader_ = nullptr;
	scene_ = nullptr;
	sceneID_ = SCENE_ID::NONE;
	waitSceneID_ = SCENE_ID::NONE;
	isSceneChanging_ = false;
	backgroundColor_ = 0xFFFFFF;
	testImg_ = -1;
	bgmChang_ = -1;
	//testPlayingEffect_ = -1;
	//testEffect_ = -1;
}
//デストラクタ
SceneManager::~SceneManager(void)
{
	DeleteGraph(mainScreen_);
	mainScreen_ = -1;
}
//インスタンスの生成(シングルトン)
void SceneManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new SceneManager();
	}
	instance_->Init();
}
//インスタンスを返す
//参照型にしているがポインタ型でもよい
SceneManager& SceneManager::GetInstance(void)
{
	return *instance_;
}

//インスタンスを破棄
void SceneManager::Destroy(void)
{
	//インスタンスを削除
	delete instance_;
	//領域を初期化
	instance_ = nullptr;
}

void SceneManager::Init3D()
{
	backgroundColor_ = GetColor(138, 190, 222);
	SetBackgroundColor(138, 190, 222);

	//zバッファ有効化
	SetUseZBuffer3D(true);
	//zバッファへの書き込み有効化
	SetWriteZBuffer3D(true);

	//バックカリング有効化　裏側を表示しない
	SetUseBackCulling(true);

	//ライトを有効にする
	SetUseLighting(true);
	//グローバルアンビエントライトカラー
	SetGlobalAmbientLight(GetColorF(0.2f, 0.2f, 0.2f,1));
	//ディレクショナルライトの方向の設定(正規化されていなくても良い)
	//正面から斜め下に向かったライト
	ChangeLightTypeDir({ 0,-1,1 });
	//----------
	/*
	//スポットライト
	ChangeLightTypeSpot(
		{ 0.0f, 50.0f, 0.0f },
		{ -45.0f, 0.0f, 0.f },
		10.0f, 0.0f,
		200.0f, 0.000f, 0.001f, 0.000f);*/

		//ポイントライト
		/*ChangeLightTypePoint({ -2000.0f,500.0f,0.0f }, 600.0f, 0.000f, 0.001f, 0.000f);

		pointLight1_ = CreatePointLightHandle({ 0.0f,60.0f,0.0f },
			600.0f, 0.000f, 0.001f, 0.000f);*/


	SetFogEnable(true);
	SetFogColor(110, 152, 178);
	SetFogStartEnd(8000.0f, 10000.0f);
}

void SceneManager::BgmChang(std::string name)
{
	bgmChang_ = name;
}

//初期化
bool SceneManager::Init(void)
{
	testImg_ = LoadGraph((Application::PATH_IMAGE+"Background.png").c_str());
	//フェーダーをインスタンス
	fader_ = new Fader();
	fader_->Init();

	//3dの設定
	Init3D();

	//カメラ作成
	camera_ = new Camera();
	camera_->Init();

	//シーンの生成処理
	sceneID_ = SCENE_ID::NONE;
	waitSceneID_ = SCENE_ID::TITLE;
	DoChangeScene();

	//フェードインを表示する
	fader_->SetFade(Fader::STATE::FADE_IN);
	isSceneChanging_ = true;

	mainScreen_ = MakeScreen(Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y);

	//エフェクト
	//testEffect_ = LoadEffekseerEffect("Effect/explosionY.efkefc", 80.0f);
	//testPlayingEffect_ = -1;
	return true;
}
//更新
void SceneManager::Update(void)
{
	//フェード処理
	fader_->Update();

	//フェード状態確認
	if(fader_->IsEnd())
	if (isSceneChanging_)
	{
		//フェード中
		Fade();
	}
	else //シーン遷移終了後
	{
		//シーン更新
		scene_->Update();

		//Effekseerの再生中エフェクト更新
		UpdateEffekseer3D();

		//カメラ更新
		camera_->Update();
	}
	//dxlibのカメラとEffekseerのカメラ同期
	Effekseer_Sync3DSetting();


	auto& ins = InputManager::GetInstance();

	//if (ins.IsTrgDown(KEY_INPUT_LSHIFT))
	//{
	//	//testPlayingEffect_ = PlayEffekseer3DEffect(testEffect_);
	//}
	////エフェクトの座標
	//SetPosPlayingEffekseer3DEffect(testPlayingEffect_, 0, 0, 0);
	
	// Effekseerにより再生中のエフェクトを更新する。
	UpdateEffekseer3D();
}
//描画
void SceneManager::Draw(void)
{
	//メインスクリーン
	int mainScreen = SceneManager::GetInstance().GetMainScreen();
	SetDrawScreen(mainScreen);
	ClearDrawScreen();

	//Effekseerを使うときは背景描画時になにかしら一つは画像を描画
	DrawGraph(0, 0, testImg_, false);
	//背景描画
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, backgroundColor_,true);
	
	camera_->SetBeforeDraw();



	//シーン描画
	scene_->Draw();

	camera_->Draw();
	
	//フェードの描画
	fader_->Draw(mainScreen_);

	//スクリーンを戻す
	SetDrawScreen(DX_SCREEN_BACK);
	DrawGraph(0,0,mainScreen_,false);
}
//解放
bool SceneManager::Release(void)
{
	//シーン解放処理
	ReleaseScene();

	//画像削除
	DeleteGraph(testImg_);
	
	//フェーダー解放処理
	delete fader_;
	fader_ = nullptr;

	//カメラ解放処理
	delete camera_;
	camera_ = nullptr;

	//インスタンスを破棄
	Destroy();
	
	return true;
}

//シーン変更を依頼
//isToFadeがtrueでフェードアウト
void SceneManager::ChangeScene(SCENE_ID nextID, bool IsToFade)
{
	//次シーンを設定(フェードアウト終了後に遷移する場合があるため)
	waitSceneID_ = nextID;
	if (IsToFade)
	{
		//フェードアウト開始でカーソルの固定を外す
		camera_->SetMouseLock(false);
		//フェードアウト シーン遷移
		//fader_->SetFade(Fader::STATE::FADE_OUT);
		fader_->SetFade(Fader::STATE::FADEOUT_PS);
		isSceneChanging_ = true;
	}
	else
	{
		//フェードを実行せずにシーン遷移
		DoChangeScene();
	}
}
Camera* SceneManager::GetCamera()
{
	return camera_;
}
int SceneManager::GetMainScreen() const
{
	return mainScreen_;
}
std::string SceneManager::GetBgmChangType(void)
{
	return bgmChang_;
}
//シーンを切り替える
void SceneManager::DoChangeScene(void)
{
	SoundManager::GetInstance().ReleaseAllSound();

	//現在のシーン解放
	ReleaseScene();

	//シーン切り替え
	sceneID_ = waitSceneID_;

	//切り替えたシーンのインスタンスを生成
	switch (sceneID_)
	{
	case SCENE_ID::TITLE:
		//タイトルシーンをインスタンス
		scene_ = new SceneTitle();
		break;

	case SCENE_ID::GAME:
		//ゲームシーン
		scene_ = new SceneGame();
		break;

	case SCENE_ID::GAMEOVER:
		//ゲームオーバーシーン
		scene_ = new SceneGameOver();
		break;

	case SCENE_ID::TUTORIAL:
		//ゲームオーバーシーン
		scene_ = new SceneTutorial();
		break;
	default:
		break;
	}

	//カメラを初期状態に戻す(シーンよりも先)
	camera_->Reset();
	//初期化
	scene_->Init();

	//シーンの遷移が終了したので次のシーンをクリア
	waitSceneID_ = SCENE_ID::NONE;
}
//フェード実施用
void SceneManager::Fade(void)
{
	//現在のフェード設定を取得
	Fader::STATE fState = fader_->GetState();

	switch (fState)
	{
	case Fader::STATE::FADE_OUT:
		if (fader_->IsEnd() == true)
		{
			//シーン切り替え
			DoChangeScene();
			//フェードで明るくしていく
			fader_->SetFade(Fader::STATE::FADE_IN);
		}
		break;
	case Fader::STATE::FADE_IN:
		if (fader_->IsEnd() == true)
		{
			//フェードを終了する
			fader_->SetFade(Fader::STATE::NONE);
			//シーン遷移の終了
			isSceneChanging_ = false;
		}
		break;
	case Fader::STATE::FADEOUT_PS:
		if (fader_->IsEnd() == true)
		{
			//シーン切り替え
			DoChangeScene();
			//フェードで明るくしていく
			fader_->SetFade(Fader::STATE::FADEIN_PS);
		}
		break;
	case Fader::STATE::FADEIN_PS:
		if (fader_->IsEnd() == true)
		{
			//フェードを終了する
			fader_->SetFade(Fader::STATE::NONE);
			//シーン遷移の終了
			isSceneChanging_ = false;
		}
		break;
	}
}
bool SceneManager::IsEndFade(void) const
{
	return fader_->IsEnd();
}
//シーンの解放処理
void SceneManager::ReleaseScene(void)
{
	if (scene_ != nullptr)
	{
		scene_->Release();
		delete scene_;
		scene_ = nullptr;//ヌルポインター
	}
}