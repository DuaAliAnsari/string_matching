# CSE 317 — String Matching Project
**Spring 2026 | Group: Dua, Haniya, Raviha, Zehra, Tahera**

---

## Project structure

```
string_matching/
├── src/
│   ├── common.h          ← shared types & timer (DO NOT edit)
│   ├── main.cpp          ← runs all algorithms, exports results (DO NOT edit)
│   ├── naive.h          
│   ├── kmp.h             
│   ├── rabin_karp.h    
│   ├── boyer_moore.h     
│   └── z_algorithm.h     
├── results/
│   └── results.csv       ← auto-generated after running
├── Makefile
└── README.md
```

---

## Each person's job

1. Open **only your own `.h` file**
2. Fill in the `TODO` sections
3. Test your function 
4. Do NOT change `common.h` or `main.cpp`

---

## How to build and run

```bash
# compile
make

# run all tests (saves results.csv automatically)
make run
```

Requires g++ with C++17 support. Works on Linux, macOS, and Windows (WSL).

---

## Function signatures (must not change)

```cpp
MatchResult naive_search       (const std::string& text, const std::string& pattern);
MatchResult kmp_search         (const std::string& text, const std::string& pattern);
MatchResult rabin_karp_search  (const std::string& text, const std::string& pattern);
MatchResult boyer_moore_search (const std::string& text, const std::string& pattern);
MatchResult z_search           (const std::string& text, const std::string& pattern);
```

Each returns a `MatchResult` with:
- `positions` — vector of 0-based indices where pattern was found
- `time_ms`   — how long your function took in milliseconds

---

## results.csv format

```
test_case, text_size, pattern_size, algorithm, time_ms, matches
```

Import this into Excel or Python (matplotlib/pandas) to make the graphs for the report.

---

## Sharing this code (GitHub instructions)

1. One person creates a **private GitHub repo** (github.com → New repository)
2. Go to Settings → Collaborators → Add each teammate's GitHub username
3. Everyone else runs:
   ```bash
   git clone https://github.com/<username>/<repo-name>.git
   ```
4. Each person works on their own `.h` file only
5. To push your changes:
   ```bash
   git add src/your_file.h
   git commit -m "implement kmp"
   git push
   ```
6. To get others' updates:
   ```bash
   git pull
   ```

---

## Report writing tips

Each person writes 2 sections for the report:
- **Algorithm Description** — explain how your algorithm works in plain English + pseudocode
- **Theoretical Analysis** — best/avg/worst complexity with justification

The results.csv will be used for the **Results & Discussion** section (graphs + table).
