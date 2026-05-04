#pragma once
#include <vector>

struct ScoreData
{
	/// <summary>
	/// 計算結果。マイナスの場合はスコアを一括で非表示とする。
	/// </summary>
	int resultScore;
	/// <summary>
	/// かかった時間(秒)
	/// </summary>
	float timeS;

	/// <summary>
	/// タイムボーナス(クリアボーナスが無効なら非表示)
	/// </summary>
	int timeScore;

	/// <summary>
	/// ライフボーナス(クリアボーナスが無効なら非表示)
	/// </summary>
	int lifeScore;

	/// <summary>
	/// ライフボーナスの数(クリアボーナスが無効なら非表示)
	/// </summary>
	int lifeScoreNum;

	/// <summary>
	/// クリアボーナス(0以下なら非表示)
	/// </summary>
	int winScore;
	/// <summary>
	/// パーフェクト(ノーダメージ)ボーナス(0以下なら非表示)
	/// </summary>
	int perfectScore;
	/// <summary>
	/// 目標達成率。1以上で完全
	/// </summary>
	float targetProgression;

	/// <summary>
	/// スコア順位
	/// </summary>
	/// <param name="score"></param>
	void AddScore(int score);

	/// <summary>
	/// スコアセーブ
	/// </summary>
	void SaveScoreBoard();

	/// <summary>
	/// スコアロード
	/// </summary>
	void LoadScoreBoard();


	void DrawScoreBoard(int screenW, int startY, int fontHandle, const std::vector<int>& scores);

	std::vector<int> scoreBoard_;  // 最大5件

	ScoreData();
};

