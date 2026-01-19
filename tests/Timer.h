//
// Created by jakub on 21.03.2025.
//

#ifndef TIMER_H
#define TIMER_H
#include <chrono>


class Timer {
    public:
    Timer();
    void reset();
    void start();
    int stop();
    [[nodiscard]] int result() const;

    private:
    std::chrono::high_resolution_clock::time_point startTime,endTime;
    long long duration;
    bool isRunning;
};



#endif //TIMER_H
