#pragma once
#include <vector>

struct ScoreData
{
	// スコアランキングの最大保存数
	static constexpr size_t MAX_SCORE_COUNT = 5;

	// スコア表示の1行あたりの高さ
	static constexpr int SCORE_LINE_HEIGHT = 40;

	// スコア表示の順位とスコアの間隔
	static constexpr int SCORE_SPACING = 10;

	// スコア表示の順位
	static constexpr int FIRST_RANK = 1;

	// スコア表示の文字色
	static constexpr int LABEL_COLOR = 200;
	static constexpr int SCORE_COLOR = 255;

	// スコア表示の色成分
	static constexpr int COLOR_COMPONENT_COUNT = 3;

	// 初期スコア
	static constexpr int INITIAL_SCORE = 0;

	// 初期進行率
	static constexpr float INITIAL_PROGRESSION = 0.0f;

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

