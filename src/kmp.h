#pragma once
#include "common.h"
#include <vector>

// KMP (KNUTH-MORRIS-PRATT)

// Design paradigm: Preprocessing / Failure function
// Time complexity: O(n + m) in all cases (best, average, worst)
// Space complexity: O(m) for the LPS array

//1: Build the LPS (Longest Proper Prefix which is also a Suffix) array

// Before searching, we study the pattern itself.
// For each position i, we ask: does any prefix of the pattern
// match a suffix ending at position i?
// We store the length of the longest such match in lps[i].

//eg: pattern= "AABAAB"
//          lps= [0, 1, 0, 1, 2, 3]

// This tells us: if we fail at position i during search,
// we can jump to lps[i-1] instead of going back to zero.
// We reuse progress we already made.

std::vector<int> build_lps(const std::string& pattern) {
    int m= pattern.size();
    std::vector<int> lps(m, 0); // start with all zeros

    int len= 0; // length of the current matching prefix
    int i= 1;   // lps[0] is always 0 so we start from index 1

    while (i< m){
        if(pattern[i]== pattern[len]){
            // characters match so we extend the prefix
            lps[i++]= ++len;

        }
        else if (len!= 0){
            // mismatch but we have some prefix matched
            // fall back using lps instead of resetting to zero
            // we do not move i forward here
            len= lps[len - 1];

        } 
        else{
            // mismatch and no prefix matched at all
            lps[i++]= 0;
        }
    }
    return lps;
}


//Search for the pattern in the text using the LPS array

// We use two pointers: i for the text, j for the pattern.
// When characters match, both move forward.
// When they mismatch, j jumps back using lps instead of resetting to 0.
//key point: i never moves backward.
// We scan the entire text exactly once which gives us O(n).

MatchResult kmp_search(const std::string& text, const std::string& pattern){
    MatchResult result;
    auto start= std::chrono::high_resolution_clock::now();

    int n= text.size(), m= pattern.size();

    //edge case: if pattern is empty or longer than text, no match is possible
    if(m== 0 || m> n) {
        result.time_ms= measure_ms(start);
        return result;
    }

    // Step 1: preprocess the pattern to get the LPS array
    auto lps= build_lps(pattern);

    int i= 0; // pointer for text
    int j= 0; // pointer for pattern

    while (i< n) {
        if(text[i]== pattern[j]){
            // characters match, move both pointers forward
            i++; j++;
        }

        if (j== m) {
            // j reached the end of pattern, full match found
            // match starts at position (i- j) in the text
            result.positions.push_back(i- j);

            // use lps to keep searching for more matches
            j= lps[j- 1];

        }
        else if(i< n && text[i]!= pattern[j]) {
            if (j!= 0) {
                // mismatch but some characters matched before
                // jump back using LPS, do not reset j to zero
                j= lps[j - 1];
            }
            else{
                // j is already zero, just move text pointer forward
                i++;
            }
        }
    }

    result.time_ms= measure_ms(start);
    return result;
}