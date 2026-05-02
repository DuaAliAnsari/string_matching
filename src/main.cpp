#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <iomanip>

#include "naive.h"
#include "kmp.h"
#include "rabin_karp.h"
#include "boyer_moore.h"
#include "z_algorithm.h"

//  HELPER GENERATORS

// Random string from a given alphabet (seeded for reproducibility)
std::string rand_str(int len, const std::string& alpha = "abcd", int seed = 42) {
    std::mt19937 rng(seed);
    std::uniform_int_distribution<> d(0, (int)alpha.size() - 1);
    std::string s(len, ' ');
    for (char& c : s) c = alpha[d(rng)];
    return s;
}

// Random pattern guaranteed to exist at a known position in text
std::string planted_pattern(const std::string& text, int pos, int len) {
    return text.substr(pos, len);
}

// Load a real-world text file (place any .txt in test_cases/)
std::string load_file(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return "";
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}


//  TEST CASE STRUCT

struct TestCase {
    std::string category;   // for grouping in CSV
    std::string name;
    std::string text;
    std::string pattern;
};


//  ALL TEST CASES

std::vector<TestCase> generate_test_cases() {

    // ── 1. EDGE CASES — shortest possible inputs
    // Tests correctness on tiny inputs where algorithms must not crash
    std::string t_edge = rand_str(10);
    std::vector<TestCase> cases = {
        { "edge", "single_char_match",    "a",           "a"    },
        { "edge", "single_char_no_match", "a",           "b"    },
        { "edge", "pattern_eq_text",      "hello",       "hello"},
        { "edge", "pattern_longer",       "hi",          "hello"},   // m > n — no match possible
        { "edge", "empty_pattern_guard",  "abcdef",      "z"    },   // pattern not in text at all
        { "edge", "one_char_text",        "a",           "ab"   },   // pattern longer than text
    };

    // ── 2. SHORT INPUT — small scale, simple alphabet 
    // Low n, low m. All algorithms should be fast. Tests basic correctness.
    cases.push_back({ "short", "short_n100_m5",   rand_str(100),  rand_str(5)  });
    cases.push_back({ "short", "short_n500_m10",  rand_str(500),  rand_str(10) });
    cases.push_back({ "short", "short_planted",   rand_str(500),  planted_pattern(rand_str(500), 200, 8) });

    // ── 3. MEDIUM INPUT — scaling up 
    cases.push_back({ "medium", "med_n1k_m10",    rand_str(1000),   rand_str(10)  });
    cases.push_back({ "medium", "med_n10k_m20",   rand_str(10000),  rand_str(20)  });
    cases.push_back({ "medium", "med_n50k_m50",   rand_str(50000),  rand_str(50)  });
    cases.push_back({ "medium", "med_n100k_m100", rand_str(100000), rand_str(100) });

    // ── 4. LONG INPUT — stress test, large scale
    cases.push_back({ "long", "long_n500k_m50",   rand_str(500000),   rand_str(50)  });
    cases.push_back({ "long", "long_n1m_m100",    rand_str(1000000),  rand_str(100) });
    cases.push_back({ "long", "long_n1m_m500",    rand_str(1000000),  rand_str(500) });

    // ── 5. SHORT PATTERN vs LONG TEXT 
    // Boyer-Moore shines with longer patterns (more skip opportunity).
    // Very short patterns give it less advantage — tests that nuance.
    cases.push_back({ "pattern_len", "long_text_pat1",  rand_str(500000), rand_str(1)   });
    cases.push_back({ "pattern_len", "long_text_pat3",  rand_str(500000), rand_str(3)   });
    cases.push_back({ "pattern_len", "long_text_pat10", rand_str(500000), rand_str(10)  });
    cases.push_back({ "pattern_len", "long_text_pat50", rand_str(500000), rand_str(50)  });
    cases.push_back({ "pattern_len", "long_text_pat200",rand_str(500000), rand_str(200) });

    // ── 6. SIMPLE ALPHABET (binary)
    // Only 2 chars — high repetition, many partial matches.
    // Stresses KMP's LPS jumps and Naive's worst behaviour.
    cases.push_back({ "alphabet", "binary_n10k_m10",   rand_str(10000,  "ab"), rand_str(10,  "ab") });
    cases.push_back({ "alphabet", "binary_n100k_m50",  rand_str(100000, "ab"), rand_str(50,  "ab") });
    cases.push_back({ "alphabet", "binary_n500k_m100", rand_str(500000, "ab"), rand_str(100, "ab") });

    // ── 7. COMPLEX ALPHABET (full ASCII printable) 
    // 95 distinct chars — very low chance of accidental match.
    // Boyer-Moore gets large skips; Rabin-Karp fewer hash collisions.
    std::string ASCII95 = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    cases.push_back({ "alphabet", "ascii_n10k_m10",   rand_str(10000,  ASCII95, 43), rand_str(10,  ASCII95, 77) });
    cases.push_back({ "alphabet", "ascii_n100k_m50",  rand_str(100000, ASCII95, 43), rand_str(50,  ASCII95, 77) });
    cases.push_back({ "alphabet", "ascii_n500k_m100", rand_str(500000, ASCII95, 43), rand_str(100, ASCII95, 77) });

    // ── 8. WORST CASE — Naive killer
    // Text = aaaa...a, Pattern = aaa...ab (never matches until the very end / never at all).
    // Naive: O(n*m). KMP/Z: O(n+m). The most dramatic graph difference.
    cases.push_back({ "worst", "worst_n10k_m10",   std::string(10000,  'a'), std::string(9,  'a') + "b" });
    cases.push_back({ "worst", "worst_n50k_m50",   std::string(50000,  'a'), std::string(49, 'a') + "b" });
    cases.push_back({ "worst", "worst_n100k_m100", std::string(100000, 'a'), std::string(99, 'a') + "b" });
    cases.push_back({ "worst", "worst_n500k_m100", std::string(500000, 'a'), std::string(99, 'a') + "b" });

    // ── 9. BEST CASE — pattern never partially matches 
    // Text = aaa...a, Pattern = "b" — mismatch on first char every time.
    // Naive is actually fast here. Tests best-case behaviour.
    cases.push_back({ "best", "best_n100k", std::string(100000, 'a'), "b"  });
    cases.push_back({ "best", "best_n500k", std::string(500000, 'a'), "b"  });
    cases.push_back({ "best", "best_n1m",   std::string(1000000,'a'), "bc" });

    // ── 10. MANY MATCHES — pattern occurs very frequently 
    // Tests how algorithms handle reporting large numbers of matches.
    cases.push_back({ "many_matches", "freq_n100k", std::string(100000, 'a'), "a"   });
    cases.push_back({ "many_matches", "freq_n100k_m2", std::string(100000, 'a'), "aa"  });
    cases.push_back({ "many_matches", "freq_n500k_m3", std::string(500000, 'a'), "aaa" });

    // ── 11. NO MATCH — pattern never appears 
    // Algorithms must scan entire text without false positives.
    cases.push_back({ "no_match", "nomatch_n100k",  rand_str(100000), "ZZZZZZZZZZZ" });
    cases.push_back({ "no_match", "nomatch_n500k",  rand_str(500000), "ZZZZZZZZZZZ" });
    cases.push_back({ "no_match", "nomatch_binary", std::string(500000,'a'), std::string(100,'b') });

    // ── 12. PATTERN AT EXTREMES — start and end of text 
    // Tests boundary handling in each implementation.
    std::string base = rand_str(100000);
    cases.push_back({ "position", "match_at_start", "FOUND" + base,        "FOUND"  });
    cases.push_back({ "position", "match_at_end",   base + "FOUND",        "FOUND"  });
    cases.push_back({ "position", "match_both_ends","FOUND" + base + "FOUND", "FOUND" });

    // ── 13. REAL-WORLD DATASET 
    // Download any book from Project Gutenberg as book.txt and place in test_cases/
    // e.g. https://www.gutenberg.org/files/1342/1342-0.txt  (Pride and Prejudice)
std::string book = load_file("book.txt");    if (!book.empty()) {
        cases.push_back({ "realworld", "rw_common_word",   book, "the"        });
        cases.push_back({ "realworld", "rw_rare_word",     book, "melancholy"  });
        cases.push_back({ "realworld", "rw_phrase",        book, "it is a truth" });
        cases.push_back({ "realworld", "rw_nonexistent",   book, "xyzxyzxyz"   });
        cases.push_back({ "realworld", "rw_long_pattern",  book, book.substr(5000, 100) }); // 100-char phrase from middle
    } else {
        std::cout << "[INFO] No real-world file found at test_cases/book.txt — skipping real-world tests.\n"
                  << "       Download any .txt book from gutenberg.org and place it there.\n";
    }

    return cases;
}


//  RUN ONE TEST

void run_test(const TestCase& tc, std::ofstream& csv) {
    std::cout << "\n[" << tc.category << " | " << tc.name << "]"
              << "  text=" << tc.text.size()
              << "  pattern=" << tc.pattern.size() << "\n";

    auto print = [&](const std::string& algo, MatchResult& r) {
        std::cout << "  " << std::left << std::setw(15) << algo
                  << std::fixed << std::setprecision(3)
                  << r.time_ms << " ms   matches=" << r.positions.size() << "\n";
        csv << tc.category << "," << tc.name << ","
            << tc.text.size() << "," << tc.pattern.size() << ","
            << algo << "," << r.time_ms << "," << r.positions.size() << "\n";
    };

    auto r1 = naive_search(tc.text, tc.pattern);        print("Naive",        r1);
    auto r2 = kmp_search(tc.text, tc.pattern);          print("KMP",          r2);
    auto r3 = rabin_karp_search(tc.text, tc.pattern);   print("Rabin-Karp",   r3);
    auto r4 = boyer_moore_search(tc.text, tc.pattern);  print("Boyer-Moore",  r4);
    auto r5 = z_search(tc.text, tc.pattern);            print("Z-Algorithm",  r5);
}


//  ENTRY POINT

int main() {
    std::ofstream csv("../results/results.csv");
    csv << "category,test_case,text_size,pattern_size,algorithm,time_ms,matches\n";

    auto tests = generate_test_cases();
    std::cout << "Running " << tests.size() << " test cases across 5 algorithms...\n";

    for (auto& tc : tests)
        run_test(tc, csv);

    csv.close();
    std::cout << "\nDone. Results saved to results/results.csv\n";
    return 0;
}
