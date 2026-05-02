#pragma once
#include "common.h"
#include <vector>

// ─────────────────────────────────────────────
//  KMP (KNUTH-MORRIS-PRATT)
//
//  Design paradigm : Preprocessing / Failure function
//  Time complexity : O(n + m) — ALL cases (best, avg, worst)
//  Space complexity: O(m)  for the LPS array
// ─────────────────────────────────────────────

// Step 1 — build LPS (Longest Prefix Suffix) array
// lps[i] = length of longest proper prefix of pattern[0..i]
//          that is also a suffix
std::vector<int> build_lps(const std::string& pattern) {
    int m = pattern.size();
    std::vector<int> lps(m, 0);
    int len = 0, i = 1;

    while (i < m) {
        if (pattern[i] == pattern[len]) {
            lps[i++] = ++len;
        } else if (len != 0) {
            len = lps[len - 1];   // don't increment i
        } else {
            lps[i++] = 0;
        }
    }
    return lps;
}

MatchResult kmp_search(const std::string& text, const std::string& pattern) {
    MatchResult result;
    auto start = std::chrono::high_resolution_clock::now();

    int n = text.size(), m = pattern.size();
    if (m == 0 || m > n) { result.time_ms = measure_ms(start); return result; }

    auto lps = build_lps(pattern);
    int i = 0, j = 0;

    while (i < n) {
        if (text[i] == pattern[j]) { i++; j++; }

        if (j == m) {
            result.positions.push_back(i - j);
            j = lps[j - 1];          // keep searching
        } else if (i < n && text[i] != pattern[j]) {
            if (j != 0) j = lps[j - 1];
            else        i++;
        }
    }

    result.time_ms = measure_ms(start);
    return result;
}