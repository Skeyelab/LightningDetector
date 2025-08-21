#!/bin/bash

# Comprehensive test runner for LtngDet PIO Heltec V3 OLED project
# Compiles and runs each test individually to avoid PlatformIO symbol conflicts
# This preserves ALL test coverage while ensuring tests actually work

echo "=========================================="
echo "Running Comprehensive Tests for LtngDet PIO Project"
echo "=========================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to run a comprehensive test
run_comprehensive_test() {
    local test_name=$1
    local test_file=$2
    local dependencies=$3
    local include_paths=$4

    echo -e "\n${YELLOW}Running ${test_name} tests...${NC}"
    echo "----------------------------------------"

    # Create temporary test directory
    local temp_test_dir="temp_test_${test_name// /_}"
    mkdir -p "$temp_test_dir"

    # Copy test file to temp directory
    cp "$test_file" "$temp_test_dir/"

    # Compile the test
    echo "Compiling..."
    local compile_cmd="g++ -std=c++17 -D UNIT_TEST -D ARDUINO_MOCK $include_paths $dependencies -o $temp_test_dir/test_runner $temp_test_dir/$(basename $test_file)"
    echo "Compile command: $compile_cmd"
    echo "Checking Unity library..."
    ls -la .pio/libdeps/native/Unity/src/ || echo "Unity not found!"

    if eval "$compile_cmd"; then
        echo "Running..."
        if ./$temp_test_dir/test_runner; then
            echo -e "${GREEN}✅ ${test_name} tests PASSED${NC}"
            # Clean up
            rm -rf "$temp_test_dir"
            return 0
        else
            echo -e "${RED}❌ ${test_name} tests FAILED${NC}"
            # Clean up
            rm -rf "$temp_test_dir"
            return 1
        fi
    else
        echo -e "${RED}❌ ${test_name} tests FAILED to compile${NC}"
        echo "Compile command: $compile_cmd"
        # Clean up
        rm -rf "$temp_test_dir"
        return 1
    fi
}

# Initialize test results
total_tests=0
passed_tests=0
failed_tests=0

# Common include paths and dependencies
COMMON_INCLUDES="-I src -I test/mocks -I .pio/libdeps/native/Unity/src"
COMMON_DEPS="test/mocks/Arduino.cpp test/mocks/esp_mocks.cpp test/mocks/wifi_mocks.cpp test/mocks/preferences_mocks.cpp .pio/libdeps/native/Unity/src/unity.c"

echo -e "\n${YELLOW}Running Comprehensive Tests (Individual Compilation)${NC}"
echo "=========================================="

# Hardware Abstraction comprehensive test
total_tests=$((total_tests + 1))
if run_comprehensive_test "Hardware Abstraction" "test/test_hardware_abstraction.cpp" "src/hardware/hardware_abstraction.cpp $COMMON_DEPS" "$COMMON_INCLUDES"; then
    passed_tests=$((passed_tests + 1))
else
    failed_tests=$((failed_tests + 1))
fi

# App Logic test
total_tests=$((total_tests + 1))
if run_comprehensive_test "App Logic" "test/test_app_logic.cpp" "src/app_logic.cpp" "$COMMON_INCLUDES"; then
    passed_tests=$((passed_tests + 1))
else
    failed_tests=$((failed_tests + 1))
fi

# WiFi Manager test
total_tests=$((total_tests + 1))
if run_comprehensive_test "WiFi Manager" "test/test_wifi_manager.cpp" "$COMMON_DEPS" "$COMMON_INCLUDES"; then
    passed_tests=$((passed_tests + 1))
else
    failed_tests=$((failed_tests + 1))
fi

# WiFi Logic test
total_tests=$((total_tests + 1))
if run_comprehensive_test "WiFi Logic" "test/test_wifi_logic.cpp" "$COMMON_DEPS" "$COMMON_INCLUDES"; then
    passed_tests=$((passed_tests + 1))
else
    failed_tests=$((failed_tests + 1))
fi

# Sensor Framework test
total_tests=$((total_tests + 1))
if run_comprehensive_test "Sensor Framework" "test/test_sensor_framework.cpp" "$COMMON_DEPS" "$COMMON_INCLUDES"; then
    passed_tests=$((passed_tests + 1))
else
    failed_tests=$((failed_tests + 1))
fi

# State Machine test
total_tests=$((total_tests + 1))
if run_comprehensive_test "State Machine" "test/test_state_machine.cpp" "$COMMON_DEPS" "$COMMON_INCLUDES"; then
    passed_tests=$((passed_tests + 1))
else
    failed_tests=$((failed_tests + 1))
fi

# Error Handler test
total_tests=$((total_tests + 1))
if run_comprehensive_test "Error Handler" "test/test_error_handler.cpp" "$COMMON_DEPS" "$COMMON_INCLUDES"; then
    passed_tests=$((passed_tests + 1))
else
    failed_tests=$((failed_tests + 1))
fi

# Modular Architecture test
total_tests=$((total_tests + 1))
if run_comprehensive_test "Modular Architecture" "test/test_modular_architecture.cpp" "$COMMON_DEPS" "$COMMON_INCLUDES"; then
    passed_tests=$((passed_tests + 1))
else
    failed_tests=$((failed_tests + 1))
fi

# Integration test
total_tests=$((total_tests + 1))
if run_comprehensive_test "Integration" "test/test_integration.cpp" "src/app_logic.cpp src/hardware/hardware_abstraction.cpp $COMMON_DEPS" "$COMMON_INCLUDES"; then
    passed_tests=$((passed_tests + 1))
else
    failed_tests=$((failed_tests + 1))
fi

# LoRa Presets test - Unity compatible
total_tests=$((total_tests + 1))
if run_comprehensive_test "LoRa Presets" "test/test_lora_presets_unity.cpp" "$COMMON_DEPS" "$COMMON_INCLUDES"; then
    passed_tests=$((passed_tests + 1))
else
    failed_tests=$((failed_tests + 1))
fi

# Web Preset Integration test - Unity compatible
total_tests=$((total_tests + 1))
if run_comprehensive_test "Web Integration" "test/test_web_integration_unity.cpp" "$COMMON_DEPS" "$COMMON_INCLUDES"; then
    passed_tests=$((passed_tests + 1))
else
    failed_tests=$((failed_tests + 1))
fi

# Summary
echo -e "\n=========================================="
echo "Comprehensive Test Summary"
echo "=========================================="
echo "Total test suites: $total_tests"
echo -e "${GREEN}Passed: ${passed_tests}${NC}"
echo -e "${RED}Failed: ${failed_tests}${NC}"

if [ $failed_tests -eq 0 ]; then
    echo -e "\n${GREEN}🎉 All comprehensive tests passed!${NC}"
    echo -e "${GREEN}Full test coverage achieved with working tests!${NC}"
    exit 0
else
    echo -e "\n${RED}💥 Some comprehensive tests failed.${NC}"
    echo -e "${YELLOW}This preserves test coverage while identifying issues.${NC}"
    exit 1
fi
