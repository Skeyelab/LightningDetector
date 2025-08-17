#include <unity.h>
#include "../../src/hardware/hardware_abstraction.h"

using namespace HardwareAbstraction;

void setUp(void) {
    // Initialize before each test
    initialize();
}

void tearDown(void) {
    // Clean up after each test
    deinitialize();
}

// Test basic hardware initialization
void test_hardware_initialization() {
    deinitialize(); // Start clean
    
    TEST_ASSERT_FALSE(isInitialized());
    TEST_ASSERT_EQUAL(Result::SUCCESS, initialize());
    TEST_ASSERT_TRUE(isInitialized());
    
    // Test double initialization
    TEST_ASSERT_EQUAL(Result::SUCCESS, initialize());
    TEST_ASSERT_TRUE(isInitialized());
}

void test_result_to_string() {
    TEST_ASSERT_EQUAL_STRING("SUCCESS", resultToString(Result::SUCCESS));
    TEST_ASSERT_EQUAL_STRING("ERROR_INIT_FAILED", resultToString(Result::ERROR_INIT_FAILED));
    TEST_ASSERT_EQUAL_STRING("ERROR_NOT_INITIALIZED", resultToString(Result::ERROR_NOT_INITIALIZED));
    TEST_ASSERT_EQUAL_STRING("ERROR_INVALID_PARAMETER", resultToString(Result::ERROR_INVALID_PARAMETER));
    TEST_ASSERT_EQUAL_STRING("ERROR_TIMEOUT", resultToString(Result::ERROR_TIMEOUT));
    TEST_ASSERT_EQUAL_STRING("ERROR_COMMUNICATION_FAILED", resultToString(Result::ERROR_COMMUNICATION_FAILED));
    TEST_ASSERT_EQUAL_STRING("ERROR_HARDWARE_FAULT", resultToString(Result::ERROR_HARDWARE_FAULT));
}

// Test GPIO functionality
void test_gpio_basic() {
    // Test pin mode setting
    TEST_ASSERT_EQUAL(Result::SUCCESS, GPIO::pinMode(2, GPIO::Mode::MODE_OUTPUT));
    TEST_ASSERT_EQUAL(Result::SUCCESS, GPIO::pinMode(4, GPIO::Mode::MODE_INPUT));
    
    // Test digital write/read
    TEST_ASSERT_EQUAL(Result::SUCCESS, GPIO::digitalWrite(2, GPIO::Level::LEVEL_HIGH));
    GPIO::Level level = GPIO::digitalRead(2);
    TEST_ASSERT_TRUE(level == GPIO::Level::LEVEL_LOW || level == GPIO::Level::LEVEL_HIGH);
}

// Test timer functionality
void test_timer_basic() {
    // Test timer initialization
    TEST_ASSERT_EQUAL(Result::SUCCESS, Timer::initialize());
    
    // Test basic time functions
    uint32_t time1 = Timer::millis();
    uint32_t time2 = Timer::millis();
    TEST_ASSERT_GREATER_OR_EQUAL(time1, time2 - 1); // Allow for small timing differences
}

int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_hardware_initialization);
    RUN_TEST(test_result_to_string);
    RUN_TEST(test_gpio_basic);
    RUN_TEST(test_timer_basic);
    
    return UNITY_END();
}
