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

    if (it != scoreBoard_.end() || scoreBoard_.size() < 5) {
        scoreBoard_.insert(it, score);
        if (scoreBoard_.size() > 5) {
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

    /*FILE* fp = nullptr;
    if (fopen_s(&fp, "scoreboard.dat", "wb") != 0 || fp == nullptr) {
        return;
    }

    uint32_t count = static_cast<uint32_t>(scoreBoard_.size());
    fwrite(&count, sizeof(count), 1, fp);
    fwrite(scoreBoard_.data(), sizeof(int), count, fp);

    fclose(fp);*/
}

void ScoreData::LoadScoreBoard()
{
    FILE* fp = nullptr;
    if (fopen_s(&fp, "scoreboard.dat", "r") != 0 || fp == nullptr) {
        return;
    }

    scoreBoard_.clear();
    int score = 0;
    while (fscanf_s(fp, "%5d", &score) == 1) {
        scoreBoard_.push_back(score);
        if (scoreBoard_.size() >= 5) break; // 最大5件まで
    }

    fclose(fp);

    /*int handle = FileRead_open("scoreboard.txt");
    if (handle == 0) {
        return;
    }

    uint32_t count = 0;
    FileRead_read(&count, sizeof(count), handle);

    scoreBoard_.resize(count);
    FileRead_read(scoreBoard_.data(), sizeof(int) * count, handle);

    FileRead_close(handle);*/
}

void ScoreData::DrawScoreBoard(int screenW, int startY, int fontHandle, const std::vector<int>& scores)
{
    LoadScoreBoard();
    const int lineHeight = 40;
    const int spacing = 10;
    //const int lineHeight = 30;

    // 最大文字幅を計算（全行で同じ幅にするため）
    int maxLabelWidth = 0;
    int maxScoreWidth = 0;

    for (int i = 0; i < scores.size(); ++i) {
        char labelBuf[32];
        sprintf_s(labelBuf, "%2d位 :", i + 1);

        char scoreBuf[32];
        sprintf_s(scoreBuf, "%5d 点", scores[i]);

        int labelWidth = GetDrawStringWidthToHandle(labelBuf, strlen(labelBuf), fontHandle);
        int scoreWidth = GetDrawStringWidthToHandle(scoreBuf, strlen(scoreBuf), fontHandle);

        if (labelWidth > maxLabelWidth) maxLabelWidth = labelWidth;
        if (scoreWidth > maxScoreWidth) maxScoreWidth = scoreWidth;
    }

    int totalLineWidth = maxLabelWidth + spacing + maxScoreWidth;
    int baseX = screenW / 2 - totalLineWidth / 2;

    // 描画ループ
    for (int i = 0; i < scores.size(); ++i) {
        int y = startY + lineHeight * i;

        // ラベル（順位）
        char labelBuf[32];
        sprintf_s(labelBuf, "%2d位 :", i + 1);

        // スコア
        char scoreBuf[32];
        sprintf_s(scoreBuf, "%5d 点", scores[i]);

        DrawStringToHandle(baseX, y , labelBuf, GetColor(200, 200, 200), fontHandle);
        DrawStringToHandle(baseX + maxLabelWidth + spacing, y, scoreBuf, GetColor(255, 255, 255), fontHandle);
    }
}

ScoreData::ScoreData()
{
	this->resultScore = 0;
	this->timeS = 0;
	this->targetProgression = 0.0f;
	this->timeScore = 0;
	this->winScore = 0;
	this->perfectScore = 0;
	this->lifeScore = 0;
	this->lifeScoreNum = 0;
   
}