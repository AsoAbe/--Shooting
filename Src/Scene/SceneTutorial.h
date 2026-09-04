#pragma once
#include "SceneGame.h"
#include "../UI/ScorePanel.h"
class SceneTutorial :
    public SceneGame
{
public:
    // チュートリアル表示
    static constexpr int TUTORIAL_BOX_Y_OFFSET = 2;
    static constexpr int PROGRESS_BAR_SIZE = 160;

    // 描画
    static constexpr int DRAW_POS_START = 0;
    static constexpr int TUTORIAL_BLEND_ALPHA = 96;
    static constexpr int FULL_BLEND_ALPHA = 255;

    // 描画色
    static constexpr int TUTORIAL_TEXT_COLOR = 0xFFFF00;
    static constexpr int TUTORIAL_BACKGROUND_COLOR = 0x000000;

    // 進行度
    static constexpr int TUTORIAL_PROGRESS_COMPLETE = 1;

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

