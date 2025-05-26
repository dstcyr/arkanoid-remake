#pragma once
#include <iostream>
#include <chrono>

class CStopwatch
{
public:
    CStopwatch();
    void Start();
    void Stop();
    double ElapsedMilliseconds();

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
};