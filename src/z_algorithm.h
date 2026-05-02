#pragma once
#include "common.h"
#include <vector>


//  Z-ALGORITHM
//
//  Design paradigm : Z-function / prefix matching
//  Time complexity : O(n + m) — all cases
//  Space complexity: O(n + m) for the concatenated Z-array


// Build Z-array for a string s
// z[i] = length of longest substring starting at s[i]
//        that matches a prefix of s
std::vector<int> build_z_array(const std::string& s) {
    int n = s.size();
    std::vector<int> z(n, 0);
    z[0] = n;
    int l = 0, r = 0;

    for (int i = 1; i < n; i++) {
        if (i < r) z[i] = std::min(r - i, z[i - l]);
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) z[i]++;
        if (i + z[i] - 1 > r) { l = i; r = i + z[i] - 1; }
    }
    return z;
}

MatchResult z_search(const std::string& text, const std::string& pattern) {
    MatchResult result;
    auto start = std::chrono::high_resolution_clock::now();

    int m = pattern.size(), n = text.size();
    if (m == 0 || m > n) { result.time_ms = measure_ms(start); return result; }

    std::string combined = pattern + "$" + text;
    auto z = build_z_array(combined);

    for (int i = m + 1; i < (int)combined.size(); i++) {
        if (z[i] == m)
            result.positions.push_back(i - m - 1);
    }

    result.time_ms = measure_ms(start);
    return result;
}