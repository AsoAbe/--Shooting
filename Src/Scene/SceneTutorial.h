#pragma once
#include "SceneGame.h"
#include "../UI/ScorePanel.h"
class SceneTutorial :
    public SceneGame
{
public:
    SceneTutorial();


    bool virtual Init(void) override;
    //更新
    void virtual Update(void) override;
    //描画
    void virtual Draw() override;

    bool virtual CheckGrazeTutorial() override;

    float GetGrazeTutorialProgress()const override;

    bool virtual IsTutorial()const override;
protected:
    //回避回数側のクリア条件を満たしたらtrueにしてテキストを切り替え
    bool shotTutorialTextFlag_;

    ScorePanel::BarStruct progressBar_;
    //値監視用
    int preProgressScore_;

    //バーの値変化を監視して更新する
    void Update_ProgressBar();
};

