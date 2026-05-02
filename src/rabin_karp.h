#pragma once
#include "common.h"

//  RABIN-KARP
//
//  Design paradigm : Hashing (rolling hash)
//  Time complexity : O(n + m) average case
//                   O(n * m) worst case (all hash collisions)
//  Space complexity: O(1)

const int RK_BASE  = 256;   // number of characters in alphabet
const int RK_MOD   = 101;   // a prime number (reduce spurious hits)

MatchResult rabin_karp_search(const std::string& text, const std::string& pattern) {
    MatchResult result;
    auto start = std::chrono::high_resolution_clock::now();

    int n = text.size(), m = pattern.size();
    if (m == 0 || m > n) { result.time_ms = measure_ms(start); return result; }

    long long h = 1;
    for (int i = 0; i < m - 1; i++) h = (h * RK_BASE) % RK_MOD;

    long long pat_hash = 0, win_hash = 0;
    for (int i = 0; i < m; i++) {
        pat_hash = (RK_BASE * pat_hash + pattern[i]) % RK_MOD;
        win_hash = (RK_BASE * win_hash + text[i])    % RK_MOD;
    }

    for (int i = 0; i <= n - m; i++) {
        if (pat_hash == win_hash) {
            if (text.substr(i, m) == pattern)
                result.positions.push_back(i);
        }
        if (i < n - m) {
            win_hash = (RK_BASE * (win_hash - text[i] * h) + text[i + m]) % RK_MOD;
            if (win_hash < 0) win_hash += RK_MOD;
        }
    }

    result.time_ms = measure_ms(start);
    return result;
}