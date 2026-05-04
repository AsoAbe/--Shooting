#include <cmath>
#include<DxLib.h>
#include<EffekseerForDXLib.h>
#include "../Application.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Manager/ObjectManager.h"
#include"../Manager/SoundManager.h"
#include"../Manager/ScoreManager.h"
#include "../Common/ScoreData.h"
#include "../Common/AsoUtility.h"
#include "../Common/DrawUtility.h"
#include "../UI/ScorePanel.h"
#include "../UI/PanelBase.h"
#include "../UI/ContinuePanel.h"
#include "../UI/GameoverPanel.h"
#include "../UI/GameclearPanel.h"
#include "../Grid.h"
#include "../Object/Map.h"
#include "../Object/Player.h"

#include "../Common/PixelShaderMaterial.h"
#include "../Common/PixelShaderRenderer.h"

#include"SceneGame.h"

using SOUND_ID = SoundManager::SOUND_ID;

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//コンストラクタ
SceneGame::SceneGame(void)
{
	mainGameScreen_ = -1;
	explosionYEffect_ = -1;
	chargingEffect_ = -1;
	explosionShotEffect_ = -1;
	smokeEffect_ = -1;
	scorePanel_ = nullptr;
	panel_ = nullptr;
	grid_ = nullptr;
	oManager_ = nullptr;
	timer_ = 0;
	tutorialSkipTimer_ = 0;
	gameState_ = GAME_STATE::GAME;
	mainPSMaterial_ = nullptr;
	mainPSRenderer_ = nullptr;
	
	bgm_ = -1;
}

SceneGame::~SceneGame(void)
{
	DeleteSoundMem(bgm_);
}

//初期化
bool SceneGame::Init(void)
{
	//スコアロード
	ScoreData scoreData;
	scoreData.LoadScoreBoard();
	//サウンドロード
	LoadSceneGameSound();

	//カメラセット
	SceneManager::GetInstance().GetCamera()->ChangeMode(Camera::MODE::LOOK_DOWN_F);
	//仮ポストエフェクト
	mainGameScreen_ = MakeScreen(Application::MAINGAME_SIZE_X, Application::SCREEN_SIZE_Y);

	mainPSMaterial_ = new PixelShaderMaterial("PixelShader.cso", 1);
	mainPSMaterial_->SetValue(0);
	mainPSRenderer_ = new PixelShaderRenderer(*mainPSMaterial_);
	mainPSRenderer_->MakeSquereVertex(Application::MAINGAME_POS_X, 0,
		Application::MAINGAME_POS_X + Application::MAINGAME_SIZE_X, Application::SCREEN_SIZE_Y);

	oManager_ = new ObjectManager();
	oManager_->Init(this);

	scorePanel_ = new ScorePanel(*this);

	grid_ = new Grid();
	grid_->Init();

	explosionYEffect_ = LoadEffekseerEffect((Application::PATH_EFFECT+"explosionY.efkefc").c_str(), 50.0f);
	chargingEffect_ = LoadEffekseerEffect((Application::PATH_EFFECT+"chargingOrn.efkefc").c_str(), 50.0f);
	explosionShotEffect_ = LoadEffekseerEffect((Application::PATH_EFFECT+"explosionShotImg.efkefc").c_str(), 50.0f);
	smokeEffect_ = LoadEffekseerEffect((Application::PATH_EFFECT + "smoke.efkefc").c_str(), 50.0f);

	if (SoundManager::GetInstance().IsActiveBGM())
	{
		bgm_ = LoadSoundMem((SceneManager::GetInstance().GetBgmChangType()).c_str());
		PlaySoundMem(bgm_, DX_PLAYTYPE_LOOP);
		//ChangeVolumeSoundMem(255 * 55 / 100, bgm_);
		ChangeVolumeSoundMem(SoundManager::GetInstance().GetBgmVolume(), bgm_);
	}

	timer_ = 0;

	return true;
}
//更新
void SceneGame::Update(void)
{
	//フレームが進むたびにタイマー加算(一秒に60)
	timer_++;

	auto& ins = InputManager::GetInstance();

	if (CheckHitKey(KEY_INPUT_RETURN) == 1)
	{
		GameOver();
	}
	//パネル
	scorePanel_->Update();
	//メインゲーム
	grid_->Update();
	oManager_->Update();

	//ボスキャラ(nullptrの可能性あり)
	const CharacterBase* targetC = oManager_->GetTargetCharacter();
	if (targetC != nullptr)
	{
		SceneManager::GetInstance().GetCamera()->SetTargetCharacterPos(targetC->GetPos());
		if (!(IsTutorial()) &&targetC->GetHp() <= 0)
		{
			//ゲームクリア判定
			GameClear();
		}
	}

	//テスト用
	if (ins.IsTrgDown(KEY_INPUT_LSHIFT))
	{
		//oManager_->GetPlayer()->Graze();
		//testPlayingEffect_ = PlayEffekseer3DEffect(testEffect_);
	}
	//エフェクトの座標
	//SetPosPlayingEffekseer3DEffect(testPlayingEffect_, 0, 20, 0);

	//if (timer_ % (2) == 0)
	//{
	// //スタンダードショットの生成
	//	oManager_->CreateShot(ObjectManager::ShotType::SHOT, { 0,100,400 },
	//		Quaternion::Euler(0, AsoUtility::Deg2RadF(GetRand(360)), 0),
	//		1);
	//}
	//if (timer_ % (Application::FPS / 4) == 0)
	//{
	//	//テストショットの生成
	//	oManager_->CreateShot(ObjectManager::ShotType::TEST, { 0,100,400 },
	//		Quaternion::Euler(0, AsoUtility::Deg2RadF(GetRand(360)), 0),
	//		1);
	//}

	if (panel_ != nullptr)
	{
		if (panel_->IsActive())
		{
			panel_->Update();
		}
	}

	if (IsTutorial())
	{
		if (ins.IsNew(KEY_INPUT_R))
		{
			tutorialSkipTimer_++;
		}
		else
		{
			tutorialSkipTimer_ = 0;
		}
		int TUTORIALSKIP_F = static_cast<int>(Application::LONG_KEY_DOWN_TIME_S * Application::FPS);
		if (tutorialSkipTimer_ >= TUTORIALSKIP_F || 
			(ins.IsTrgDown(KEY_INPUT_R) && gameState_ == GAME_STATE::TUTORIAL_END))
		{
			tutorialSkipTimer_ = 0;
			NextTutorial();
			if (IsGrazeTutorial())
			{
				//スキップの場合はサウンドがならないのでここでならす
				SoundManager::GetInstance().PlaySE(SoundManager::SOUND_ID::SUCCESS);
			}
		}
	}
}
//描画
void SceneGame::Draw(void)
{
	SceneManager& sceneManager = SceneManager::GetInstance();

	// メインスクリーンに切り替え
	SetDrawScreen(mainGameScreen_);
	ClearDrawScreen();

	// パネル描画
	Draw_ScorePanel();

	// hpBar描画後にこの処理を入れないとエフェクトに不具合
	sceneManager.GetCamera()->UpdateCameraPos();
	Effekseer_Sync3DSetting();

	// メイン描画
	Draw_MainGame();

	if (panel_ != nullptr)
	{
		if (panel_->IsActive())
		{
			panel_->Draw();
		}
	}

	// メインスクリーンを描画
	int mainScreen = SceneManager::GetInstance().GetMainScreen();
	SetDrawScreen(mainScreen);
	mainPSRenderer_->Draw(mainGameScreen_, mainScreen);
}
//解放
bool SceneGame::Release(void)
{
	delete mainPSMaterial_;
	mainPSMaterial_ = nullptr;
	delete mainPSRenderer_;
	mainPSRenderer_ = nullptr;

	delete scorePanel_;
	scorePanel_ = nullptr;

	delete panel_;
	panel_ = nullptr;

	grid_->Release();
	delete grid_;
	grid_ = nullptr;

	oManager_->Release();
	delete oManager_;
	oManager_ = nullptr;

	// エフェクトリソース
	int i =DeleteEffekseerEffect(explosionYEffect_);
	DeleteEffekseerEffect(chargingEffect_);
	DeleteEffekseerEffect(explosionShotEffect_); 
	DeleteEffekseerEffect(smokeEffect_);
	
	DeleteGraph(mainGameScreen_);


	return true;
}

int SceneGame::CreateEffect(EFFECT_TYPE eType, VECTOR pos, float scale)
{
	int playingE = -1; 
	switch (eType)
	{
	case SceneGame::EFFECT_TYPE::EXPLOSION_SHOT:
		playingE = PlayEffekseer3DEffect(explosionShotEffect_);
		//エフェクトの座標
		SetPosPlayingEffekseer3DEffect(playingE, pos.x, pos.y, pos.z);
		break;

	case SceneGame::EFFECT_TYPE::CHARGE:
		playingE = PlayEffekseer3DEffect(chargingEffect_);
		//エフェクトの座標
		SetPosPlayingEffekseer3DEffect(playingE, pos.x, pos.y, pos.z);
		break;

	case SceneGame::EFFECT_TYPE::EXPLOSION_Y:
		playingE = PlayEffekseer3DEffect(explosionYEffect_);
		//エフェクトの座標
		SetPosPlayingEffekseer3DEffect(playingE, pos.x, pos.y, pos.z);
		break;
	case SceneGame::EFFECT_TYPE::SMOKE:
		playingE = PlayEffekseer3DEffect(smokeEffect_);
		//エフェクトの座標
		SetPosPlayingEffekseer3DEffect(playingE, pos.x, pos.y, pos.z);
		break;
	default:

		break;
	}
	SetScalePlayingEffekseer3DEffect(playingE, scale, scale, scale);
	return playingE;
}

//void SceneGame::CreateEffect(VECTOR pos,float scale)
//{
//	int playingE = PlayEffekseer3DEffect(explosionYEffect_);
//	//エフェクトの座標
//	SetPosPlayingEffekseer3DEffect(playingE, pos.x, pos.y+20, pos.z);
//	SetScalePlayingEffekseer3DEffect(playingE, scale, scale, scale);
//}

int SceneGame::GetTimer(void)
{
	return timer_;
}

ObjectManager* SceneGame::GetOManager()
{
	return oManager_;
}

bool SceneGame::IsGameOver() const
{
	return gameState_ == GAME_STATE::GAMEOVER|| gameState_ == GAME_STATE::GAMECLEAR;
}

bool SceneGame::IsTutorial() const
{
	return false;
	//return gameState_ == GAME_STATE::TUTORIAL_END || gameState_ == GAME_STATE::TUTORIAL_SHOT || gameState_ == GAME_STATE::TUTORIAL_JUMP;
}

bool SceneGame::IsGrazeTutorial() const
{
	return gameState_ == GAME_STATE::TUTORIAL_SHOT;
}
float SceneGame::GetGrazeTutorialProgress() const
{
	//SceneTutorial側で処理する
	return 1.0f;
}
bool SceneGame::IsJumpTutorial() const
{
	return gameState_ == GAME_STATE::TUTORIAL_JUMP;
}

bool SceneGame::CheckJumpTutorial()
{
	if (!(IsJumpTutorial()))
	{
		//対応のステート以外
		return false;
	}
	Player* player = oManager_->GetPlayer();
	bool ret = (player->GetHp() >= player->GetHpMax()) &&
		(player->GetJumpCount() > 0);
	if (ret)
	{
		NextTutorial();
	}
	else
	{
		//条件から外れたためHPを元に戻す
		player->ResetStatus();
	}
	return ret;
}

bool SceneGame::CheckGrazeTutorial()
{
	if (!(IsGrazeTutorial()))
	{
		//対応のステート以外
		return false;
	}
	Player* player = oManager_->GetPlayer();
	CharacterBase* enemy = oManager_->GetTargetCharacter();
	bool ret = (player->GetGrazeComboScore() >= TUTORIAL_SHOT_NUM) &&
		(enemy->GetHpMax() -enemy->GetHp() > 0);
	if (ret)
	{
		NextTutorial();
	}
	return ret;
}

void SceneGame::GameOver()
{
	if (IsGameOver())
	{
		//すでにゲームオーバー
		return;
	}
	ScoreData scoreData;
	if (!(IsTutorial()))
	{
		scoreData = CalcAndSetScore();
	}
	else
	{
		//チュートリアル中はスコア無効
		scoreData.resultScore = -1;
	}

	gameState_ = GAME_STATE::GAMEOVER;
	if (panel_ != nullptr)
	{
		//古いパネルをデリート
		delete panel_;
		panel_ = nullptr;
	}
	panel_ = new GameoverPanel(*this, scoreData);
	panel_->SetActive(true);
}

void SceneGame::GameClear()
{
	if (IsGameOver())
	{
		//すでにゲームオーバー
		return;
	}
	ScoreData scoreData = CalcAndSetScore();
	
	//弾をクリア(クリア後に死亡すると見栄えが悪いので予防する)
	oManager_->DeactivateAllShot();

	SoundManager::GetInstance().PlaySE(SoundManager::SOUND_ID::WIN);

	gameState_ = GAME_STATE::GAMECLEAR;

	if (panel_ != nullptr)
	{
		//古いパネルをデリート
		delete panel_;
		panel_ = nullptr;
	}
	panel_ = new GameclearPanel(*this, scoreData);
	panel_->SetActive(true);
}

ScoreData SceneGame::CalcAndSetScore()
{
	CharacterBase* targetEnemy = oManager_->GetTargetCharacter();
	float targetProgression = 0;
	if (targetEnemy != nullptr)
	{
		if (targetEnemy->GetHp() <= 0)
		{
			//完全なので1.0fを返す。
			targetProgression = 1;
		}
		else
		{
			//割合を計算
			targetProgression = static_cast<float>(targetEnemy->GetHpMax() - targetEnemy->GetHp()) / targetEnemy->GetHpMax();
		}
	}
	ScoreData scoreData =
		ScoreManager::CalcScore(targetProgression,
			oManager_->GetPlayer(),
			(timer_ / Application::FPS));

	ScoreManager::GetInstance().SetScore(scoreData.resultScore);
	return scoreData;
}

void SceneGame::PlayerDied()
{
	//ゲームオーバー時は無効
	//現在の仕様だとパネルは同時に一つまで
	if (panel_ == nullptr && !(IsGameOver()))
	{
		panel_ = new ContinuePanel(*this);
	}
	panel_->SetActive(true);

}

void SceneGame::Continue()
{
	if (panel_ != nullptr)
	{
		panel_->SetActive(false);
	}
	//プレイヤーを再度アクティブに
	oManager_->GetPlayer()->Respawn_Player();
}

void SceneGame::BackToTitle()
{
	ScoreData scoreData;
	scoreData.AddScore(ScoreManager::GetInstance().GetScore());
	//scoreData.SaveScoreBoard();
	SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::TITLE, true);
}

void SceneGame::PlayerBarFlash()
{
	scorePanel_->PlayerBarFlash();
}

void SceneGame::EnemyBarFlash()
{
	scorePanel_->EnemyBarFlash();
}

void SceneGame::PlayerBarAnim()
{
	scorePanel_->PlayerBarAnim();
}

void SceneGame::EnemyBarAnim()
{
	scorePanel_->EnemyBarAnim();
}

void SceneGame::Draw_MainGame()
{
	oManager_->Draw();
	grid_->Draw();

	//エフェクト描画
	DrawEffekseer3D();

}

void SceneGame::Draw_ScorePanel()
{
	DrawBox(0, 0, Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y, 0x202020, true);

	scorePanel_->Draw();

	//DrawString(Application::MAINGAME_SIZE_X + Application::MAINGAME_POS_X, 0, "TEXT", 0xffffff);
}

void SceneGame::NextTutorial()
{
	Player* player = oManager_->GetPlayer();
	CharacterBase* enemy = oManager_->GetTargetCharacter();
	//ステートを次に
	switch (gameState_)
	{
	case SceneGame::GAME_STATE::TUTORIAL_JUMP:
		gameState_ = GAME_STATE::TUTORIAL_SHOT;
		//サウンド再生
		SoundManager::GetInstance().PlaySE(SoundManager::SOUND_ID::SUCCESS);
		break;
	case SceneGame::GAME_STATE::TUTORIAL_SHOT:
		gameState_ = GAME_STATE::TUTORIAL_END;
		break;
	case SceneGame::GAME_STATE::TUTORIAL_END:
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME, true);
		//player->ResetStatus();
		//enemy->ResetStatus();
		//timer_ = 0;
		//gameState_ = GAME_STATE::GAME;
		break;
	default:
		break;
	}
}

void SceneGame::LoadSceneGameSound()
{
	SoundManager& sManager_ = SoundManager::GetInstance();
	sManager_.LoadSound(SOUND_ID::CURSOR, SoundManager::VOLUME_MAX);
	sManager_.LoadSound(SOUND_ID::GRAZE, SoundManager::VOLUME_MAX);
	sManager_.LoadSound(SOUND_ID::DEATH_PLAYER, SoundManager::VOLUME_HIGH);
	sManager_.LoadSound(SOUND_ID::HIT, SoundManager::VOLUME_STANDARD);
	sManager_.LoadSound(SOUND_ID::HIT_HIGH, SoundManager::VOLUME_HIGH);
	sManager_.LoadSound(SOUND_ID::HIT_PLAYER, SoundManager::VOLUME_STANDARD);
	sManager_.LoadSound(SOUND_ID::SHOT, SoundManager::VOLUME_LOW);
	sManager_.LoadSound(SOUND_ID::SHOT_HIGH, SoundManager::VOLUME_STANDARD);
	sManager_.LoadSound(SOUND_ID::SUCCESS, SoundManager::VOLUME_STANDARD);
	sManager_.LoadSound(SOUND_ID::JUMP, SoundManager::VOLUME_HIGH);
	sManager_.LoadSound(SOUND_ID::CHARGE, SoundManager::VOLUME_LOW);
	sManager_.LoadSound(SOUND_ID::WIN, SoundManager::VOLUME_HIGH);
	sManager_.LoadSound(SOUND_ID::ENEMY_DASH, SoundManager::VOLUME_STANDARD);
}

