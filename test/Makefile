CXX ?= g++
CXXFLAGS += -std=c++17 -I../src -I. -I../lib/RadioLib/src -Wall -Wextra -O2
CXXFLAGS += -D UNIT_TEST
LDFLAGS +=

SRC := ../src/app_logic.cpp ../src/config.cpp ../src/radio_state.cpp ../src/radio_controller.cpp
TESTS := test_app_logic.cpp test_config.cpp test_radio_state.cpp test_radio_controller.cpp test_main.cpp

BIN := run_tests

all: $(BIN)

$(BIN): $(SRC) $(TESTS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC) $(TESTS) $(LDFLAGS)

clean:
	rm -f $(BIN)

.PHONY: all clean