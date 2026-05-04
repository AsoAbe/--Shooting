#pragma once
#include <chrono>

class TimeOut
{
public:
    TimeOut();

    // 計測を開始する
    void start();
    void stop();                // 計測終了（ストップ）
    void reset();               // 計測リセット

    // 現在の経過秒数を返す
    int elapsedSeconds() const;

private:
    std::chrono::steady_clock::time_point start_time;
    int total_seconds;
    bool running;
};