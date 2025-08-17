#!/bin/bash

# Test runner for isolated tests
# This script compiles and runs all tests without PlatformIO's test system

set -e  # Exit on any error

echo "=========================================="
echo "Running Isolated Tests"
echo "=========================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to run a test
run_test() {
    local test_name=$1
    local test_dir=$2
    local test_file=$3
    local dependencies=$4

    echo -e "\n${YELLOW}Testing: ${test_name}${NC}"
    echo "------------------------------------------"

    # Compile the test
    echo "Compiling..."
    g++ -std=c++17 -D UNIT_TEST -D ARDUINO_MOCK -I ../src -I ../test/mocks $dependencies -o ${test_dir}/${test_file} ${test_dir}/${test_file}.cpp

    # Run the test
    echo "Running..."
    if ./${test_dir}/${test_file}; then
        echo -e "${GREEN}✅ ${test_name} PASSED${NC}"
        return 0
    else
        echo -e "${RED}❌ ${test_name} FAILED${NC}"
        return 1
    fi
}

# Track test results
passed=0
failed=0

# Hardware abstraction test
if run_test "Hardware Abstraction" "hardware" "test_hardware" "../src/hardware/hardware_abstraction.cpp ../test/mocks/Arduino.cpp ../test/mocks/esp_mocks.cpp ../.pio/libdeps/native-hardware-isolated/Unity/src/unity.c"; then
    ((passed++))
else
    ((failed++))
fi

# App logic test
if run_test "App Logic" "app_logic" "test_app_logic" "../src/app_logic.cpp"; then
    ((passed++))
else
    ((failed++))
fi

# Integration test
if run_test "Integration" "integration" "test_integration" "../src/app_logic.cpp ../src/hardware/hardware_abstraction.cpp ../test/mocks/Arduino.cpp ../test/mocks/esp_mocks.cpp"; then
    ((passed++))
else
    ((failed++))
fi

# Summary
echo -e "\n=========================================="
echo "Test Summary"
echo "=========================================="
echo -e "${GREEN}Passed: ${passed}${NC}"
echo -e "${RED}Failed: ${failed}${NC}"
echo -e "Total: $((passed + failed))"

if [ $failed -eq 0 ]; then
    echo -e "\n${GREEN}🎉 All tests passed!${NC}"
    exit 0
else
    echo -e "\n${RED}💥 Some tests failed!${NC}"
    exit 1
fi
