CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall
SRC      = src/main.cpp
OUT      = string_matching

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

run: all
	./$(OUT)

clean:
	rm -f $(OUT)
