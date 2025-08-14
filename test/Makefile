CXX ?= g++
CXXFLAGS += -std=c++17 -I../src -I. -Wall -Wextra -O2
LDFLAGS +=

SRC := ../src/app_logic.cpp ../src/config.cpp
TESTS := test_app_logic.cpp test_config.cpp test_main.cpp

BIN := run_tests

all: $(BIN)

$(BIN): $(SRC) $(TESTS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC) $(TESTS) $(LDFLAGS)

clean:
	rm -f $(BIN)

.PHONY: all clean