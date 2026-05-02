#pragma once
#include "common.h"

//  NAIVE STRING MATCHING
//
//  Design paradigm : Brute force
//  Time complexity : O(n*m) worst case
//                   O(n)   best case (no match, first char always differs)
//  Space complexity: O(1)

MatchResult naive_search(const std::string& text, const std::string& pattern) {
    MatchResult result;
    auto start = std::chrono::high_resolution_clock::now();

    int n = text.size(), m = pattern.size();
    if (m == 0 || m > n) { result.time_ms = measure_ms(start); return result; }

    for (int i = 0; i <= n - m; i++) {
        int j = 0;
        while (j < m && text[i + j] == pattern[j]) j++;
        if (j == m) result.positions.push_back(i);
    }

    result.time_ms = measure_ms(start);
    return result;
}