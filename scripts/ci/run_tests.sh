#!/bin/bash

# Test runner script for LtngDet PIO Heltec V3 OLED project
# Uses isolated test compilation to avoid PlatformIO symbol conflicts

echo "=========================================="
echo "Running Unit Tests for LtngDet PIO Project"
echo "=========================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to run an isolated test
run_isolated_test() {
    local test_name=$1
    local test_dir=$2
    local test_file=$3
    local dependencies=$4
    
    echo -e "\n${YELLOW}Running ${test_name} tests...${NC}"
    echo "----------------------------------------"
    
    # Compile the test
    echo "Compiling..."
    if g++ -std=c++17 -D UNIT_TEST -D ARDUINO_MOCK -I src -I test/mocks -I .pio/libdeps/native/Unity/src $dependencies -o test_isolated/${test_dir}/${test_file} test_isolated/${test_dir}/${test_file}.cpp 2>/dev/null; then
        echo "Running..."
        if ./test_isolated/${test_dir}/${test_file}; then
            echo -e "${GREEN}✅ ${test_name} tests PASSED${NC}"
            return 0
        else
            echo -e "${RED}❌ ${test_name} tests FAILED${NC}"
            return 1
        fi
    else
        echo -e "${RED}❌ ${test_name} tests FAILED to compile${NC}"
        return 1
    fi
}

# Initialize test results
total_tests=0
passed_tests=0
failed_tests=0

# Run isolated tests (most reliable)
echo -e "\n${YELLOW}Running Isolated Tests (Manual Compilation)${NC}"
echo "=========================================="

# Hardware abstraction test
total_tests=$((total_tests + 1))
if run_isolated_test "Hardware Abstraction" "hardware" "test_hardware" "src/hardware/hardware_abstraction.cpp test/mocks/Arduino.cpp test/mocks/esp_mocks.cpp .pio/libdeps/native/Unity/src/unity.c"; then
    passed_tests=$((passed_tests + 1))
else
    failed_tests=$((failed_tests + 1))
fi

# App logic test
total_tests=$((total_tests + 1))
if run_isolated_test "App Logic" "app_logic" "test_app_logic" "src/app_logic.cpp"; then
    passed_tests=$((passed_tests + 1))
else
    failed_tests=$((failed_tests + 1))
fi

# Integration test
total_tests=$((total_tests + 1))
if run_isolated_test "Integration" "integration" "test_integration" "src/app_logic.cpp src/hardware/hardware_abstraction.cpp test/mocks/Arduino.cpp test/mocks/esp_mocks.cpp"; then
    passed_tests=$((passed_tests + 1))
else
    failed_tests=$((failed_tests + 1))
fi

# Note: PlatformIO tests are temporarily disabled due to symbol conflicts
# They will be re-enabled once the isolated test approach is fully validated
echo -e "\n${YELLOW}Note: PlatformIO tests temporarily disabled${NC}"
echo "PlatformIO tests are experiencing symbol conflicts when compiling all test files together."
echo "The isolated test approach provides a more reliable testing solution."

# Summary
echo -e "\n=========================================="
echo "Test Summary"
echo "=========================================="
echo "Total test suites: $total_tests"
echo -e "${GREEN}Passed: ${passed_tests}${NC}"
echo -e "${RED}Failed: ${failed_tests}${NC}"

if [ $failed_tests -eq 0 ]; then
    echo -e "\n${GREEN}🎉 All tests passed! Ready for refactoring.${NC}"
    echo -e "${GREEN}The isolated test approach successfully resolves the Arduino.h dependency issues.${NC}"
    exit 0
else
    echo -e "\n${RED}💥 Some tests failed. Please fix before refactoring.${NC}"
    exit 1
fi
