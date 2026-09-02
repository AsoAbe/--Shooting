#include "ScoreData.h"
#include <string>
#include <algorithm>
#include <DxLib.h>

void ScoreData::AddScore(int score)
{
    LoadScoreBoard();

    auto it = std::find_if(scoreBoard_.begin(), scoreBoard_.end(),
        [&](int s) {
            return score > s;
        });

    if (it != scoreBoard_.end() || scoreBoard_.size() < MAX_SCORE_COUNT)
    {
        scoreBoard_.insert(it, score);

        if (scoreBoard_.size() > MAX_SCORE_COUNT)
        {
            scoreBoard_.pop_back(); // 最下位を削除
        }
    }

    SaveScoreBoard();
}

void ScoreData::SaveScoreBoard()
{
    FILE* fp = nullptr;
    if (fopen_s(&fp, "scoreboard.dat", "w") != 0 || fp == nullptr) {
        return;
    }

  
    for (int score : scoreBoard_) {
        fprintf(fp, "%5d\n", score);
    }

    fclose(fp);

}

void ScoreData::LoadScoreBoard()
{
    FILE* fp = nullptr;
    if (fopen_s(&fp, "scoreboard.dat", "r") != 0 || fp == nullptr) {
        return;
    }

    scoreBoard_.clear();
    int score = INITIAL_SCORE;
    while (fscanf_s(fp, "%5d", &score) == 1) {
        scoreBoard_.push_back(score);
        if (scoreBoard_.size() >= MAX_SCORE_COUNT) break; // 最大5件まで
    }

    fclose(fp);

}

void ScoreData::DrawScoreBoard(int screenW, int startY, int fontHandle, const std::vector<int>& scores)
{
	LoadScoreBoard();

	// 最大文字幅を計算（全行で同じ幅にするため）
	int maxLabelWidth = INITIAL_SCORE;
	int maxScoreWidth = INITIAL_SCORE;

	for (size_t i = INITIAL_SCORE; i < scores.size(); ++i)
	{
		char labelBuf[32];
		sprintf_s(labelBuf, "%2d位 :", static_cast<int>(i) + FIRST_RANK);

		char scoreBuf[32];
		sprintf_s(scoreBuf, "%5d 点", scores[i]);

		int labelWidth =
			GetDrawStringWidthToHandle(
				labelBuf,
				strlen(labelBuf),
				fontHandle);

		int scoreWidth =
			GetDrawStringWidthToHandle(
				scoreBuf,
				strlen(scoreBuf),
				fontHandle);

		if (labelWidth > maxLabelWidth)
		{
			maxLabelWidth = labelWidth;
		}

		if (scoreWidth > maxScoreWidth)
		{
			maxScoreWidth = scoreWidth;
		}
	}

	int totalLineWidth =
		maxLabelWidth +
		SCORE_SPACING +
		maxScoreWidth;

	int baseX =
		screenW / 2 -
		totalLineWidth / 2;

	// 描画ループ
	for (size_t i = INITIAL_SCORE; i < scores.size(); ++i)
	{
		int y =
			startY +
			SCORE_LINE_HEIGHT * static_cast<int>(i);

		// ラベル（順位）
		char labelBuf[32];
		sprintf_s(
			labelBuf,
			"%2d位 :",
			static_cast<int>(i) + FIRST_RANK);

		// スコア
		char scoreBuf[32];
		sprintf_s(scoreBuf, "%5d 点", scores[i]);

		DrawStringToHandle(
			baseX,
			y,
			labelBuf,
			GetColor(
				LABEL_COLOR,
				LABEL_COLOR,
				LABEL_COLOR),
			fontHandle);

		DrawStringToHandle(
			baseX + maxLabelWidth + SCORE_SPACING,
			y,
			scoreBuf,
			GetColor(
				SCORE_COLOR,
				SCORE_COLOR,
				SCORE_COLOR),
			fontHandle);
	}
}

ScoreData::ScoreData()
{
	this->resultScore = INITIAL_SCORE;
	this->timeS = INITIAL_PROGRESSION;
	this->targetProgression = INITIAL_PROGRESSION;
	this->timeScore = INITIAL_SCORE;
	this->winScore = INITIAL_SCORE;
	this->perfectScore = INITIAL_SCORE;
	this->lifeScore = INITIAL_SCORE;
	this->lifeScoreNum = INITIAL_SCORE;
}