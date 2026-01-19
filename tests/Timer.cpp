//
// Created by jakub on 21.03.2025.
//

#include "Timer.h"
#include <chrono>

Timer::Timer() {
    isRunning = false;
    duration = 0;
}

void Timer::start() {
    if ( !isRunning ) {
        startTime = std::chrono::high_resolution_clock::now();
        isRunning = true;
    }
}

void Timer::reset() {
    isRunning = false;
    duration = 0;
}

int Timer::stop() {
    if (isRunning) {
        endTime = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime-startTime).count();
        isRunning = false;
        return 0;
    }
    return -1;
}

int Timer::result() const {
    return static_cast<int>(duration);
}

