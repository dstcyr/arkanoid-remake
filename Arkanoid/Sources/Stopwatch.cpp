#include "Stopwatch.h"

CStopwatch::CStopwatch() : start_time(std::chrono::high_resolution_clock::now()) 
{
}

void CStopwatch::Start()
{
    start_time = std::chrono::high_resolution_clock::now();
}

void CStopwatch::Stop()
{
    end_time = std::chrono::high_resolution_clock::now();
}

double CStopwatch::ElapsedMilliseconds()
{
    std::chrono::duration<double, std::milli> elapsed = end_time - start_time;
    return elapsed.count();
}