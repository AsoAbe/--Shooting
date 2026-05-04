#include <DxLib.h>
#include "../Application.h"
#include "../Common/ScoreData.h"
#include "ScoreManager.h"
#include "../Object/Player.h"

//minとmax関数を使用可能にするため
#undef min
#undef max

ScoreManager* ScoreManager::instance_ = nullptr;

namespace
{
	/// <summary>
	/// ハイスコア初期値
	/// </summary>
	constexpr int START_HIGHSCORE = 500;
}


ScoreManager::ScoreManager(void)
{
	ClearScore();
}

ScoreManager::~ScoreManager(void)
{
}

void ScoreManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ScoreManager();
	}
}

ScoreManager& ScoreManager::GetInstance()
{
	return *instance_;
}

void ScoreManager::Release(void)
{
	delete instance_;
	instance_ = nullptr;
}

int ScoreManager::GetScore() const
{
	return score_;
}

int ScoreManager::GetHighScore() const
{
	return highScore_;
}

bool ScoreManager::SetScore(int score)
{
	score_ = score;
	if (score_ >= highScore_)
	{
		highScore_ = score_;
		return true;
	}
	return false;
}

void ScoreManager::ClearScore()
{
	highScore_ = START_HIGHSCORE;
	score_ = 0;
}

ScoreData ScoreManager::CalcScore(float targetProgression, const Player* player, float timerS)
{
	ScoreData ret;
	//基本スコア。ゲームオーバーの場合は進行度に合わせて減少。
	constexpr int BONUS_CLEAR = 2000;
	//タイムボーナスの最高値
	constexpr int BONUS_TIME_MAX = 1000;
	//これより下には減少しない
	constexpr int BONUS_TIME_MIN = BONUS_TIME_MAX/2;
	//1秒あたりの減少値
	constexpr int BONUS_TIME_DOWN = 2;
	//ライフ一つ当たりのボーナス。今のところは残りHPは関係なし
	constexpr int BONUS_LIFE = 100;
	//ノーダメージボーナス
	constexpr int BONUS_PERFECT = 200;

	ret.targetProgression = targetProgression;
	ret.timeS = timerS;
	if (targetProgression >= 1)
	{
		//クリアボーナス付与
		ret.winScore = BONUS_CLEAR;
		ret.resultScore += ret.winScore;
	}
	else
	{
		//減少したスコアを判定
		ret.resultScore += static_cast<int>(BONUS_CLEAR * targetProgression);
		//これ以降は処理しない
		return ret;
	}
	if (player != nullptr)
	{
		if (player->GetLife() > 0)
		{
			//ライフボーナス付与
			ret.lifeScore = BONUS_LIFE;
			ret.lifeScoreNum = player->GetLife();
			ret.resultScore += ret.lifeScore* ret.lifeScoreNum;
		}
		if (player->GetLife() >= player->GetLifeMax() && player->GetHp() >= player->GetHpMax())
		{
			//ノーダメージボーナス付与
			ret.perfectScore = BONUS_PERFECT;
			ret.resultScore += ret.perfectScore;
		}
	}
	int timeBonus = BONUS_TIME_MAX;
	//タイムボーナスを時間分減少
	timeBonus -= static_cast<int>(ret.timeS * BONUS_TIME_DOWN);
	//clamp
	timeBonus = std::min(std::max(timeBonus, BONUS_TIME_MIN), BONUS_TIME_MAX);
	//タイムボーナス付与
	ret.timeScore = timeBonus;
	ret.resultScore += ret.timeScore;
	return ret;
}