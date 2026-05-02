#pragma once
#include "common.h"
#include <unordered_map>
#include <algorithm>

//  BOYER-MOORE
//  Design paradigm : Heuristics (bad character + good suffix)
//  Time complexity : O(n / m) best case  (sub-linear!)
//                   O(n + m) average
//                   O(n * m) worst case (rare with good suffix rule)
//  Space complexity: O(m + alphabet_size)

// Bad character table: for each character, store its last position in pattern
std::unordered_map<char, int> build_bad_char(const std::string& pattern) {
    std::unordered_map<char, int> table;

    // For each character c in pattern, table[c] = last index of c in pattern
    for (int i = 0; i < (int)pattern.size(); i++) {
        table[pattern[i]] = i;  // overwrites with later index each time
    }

    return table;
}

MatchResult boyer_moore_search(const std::string& text, const std::string& pattern) {
    MatchResult result;
    auto start = std::chrono::high_resolution_clock::now();

    int n = (int)text.size();
    int m = (int)pattern.size();

    if (m == 0 || m > n) {
        result.time_ms = measure_ms(start);
        return result;
    }

    auto bad_char = build_bad_char(pattern);

    int i = 0;  // shift of pattern over text
    while (i <= n - m) {
        int j = m - 1;  // start from rightmost character of pattern

        // Move left while characters match
        while (j >= 0 && pattern[j] == text[i + j])
            j--;

        if (j < 0) {
            // Pattern found at index i
            result.positions.push_back(i);

            i++;  // shift by 1 to find next match
        } else {
            // Bad character heuristic:
            // shift = max(1, j - last occurrence of text[i+j] in pattern)
            char bad = text[i + j];
            int last = (bad_char.count(bad)) ? bad_char[bad] : -1;
            int shift = std::max(1, j - last);
            i += shift;
        }
    }

    result.time_ms = measure_ms(start);
    return result;
}