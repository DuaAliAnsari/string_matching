#pragma once
#include <string>
#include <vector>
#include <chrono>

//  Shared result struct — every algorithm returns this

struct MatchResult {
    std::vector<int> positions;   // 0-based indices where pattern starts
    double time_ms;               // wall-clock time in milliseconds
};

//  Timer helper

inline double measure_ms(std::chrono::high_resolution_clock::time_point start) {
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}
