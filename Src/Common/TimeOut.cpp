#include "TimeOut.h"

TimeOut::TimeOut()
    : total_seconds(0), running(false) {}

void TimeOut::start() {
    if (!running) {
        start_time = std::chrono::steady_clock::now();
        running = true;
    }
}

void TimeOut::stop() {
    if (running) {
        auto now = std::chrono::steady_clock::now();
        total_seconds += std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
        running = false;
    }
}

void TimeOut::reset() {
    total_seconds = 0;
    running = false;
}

int TimeOut::elapsedSeconds() const {
    if (running) {
        auto now = std::chrono::steady_clock::now();
        return total_seconds + std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
    }
    else {
        return total_seconds;
    }
}