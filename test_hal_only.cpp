#include <unity.h>
#include "src/hardware/hardware_abstraction.h"

using namespace HardwareAbstraction;

void setUp(void) {
    // Initialize before each test
}

void tearDown(void) {
    // Clean up after each test
}

// Basic compilation and functionality test
void test_hal_basic_functionality() {
    // Test that HAL can be initialized
    TEST_ASSERT_EQUAL(Result::SUCCESS, initialize());
    TEST_ASSERT_TRUE(isInitialized());
    
    // Test result to string conversion
    TEST_ASSERT_EQUAL_STRING("SUCCESS", resultToString(Result::SUCCESS));
    TEST_ASSERT_EQUAL_STRING("ERROR_INIT_FAILED", resultToString(Result::ERROR_INIT_FAILED));
    
    // Test GPIO pinMode (should work with mock implementation)
    TEST_ASSERT_EQUAL(Result::SUCCESS, GPIO::pinMode(2, GPIO::Mode::MODE_OUTPUT));
    
    // Test cleanup
    TEST_ASSERT_EQUAL(Result::SUCCESS, deinitialize());
    TEST_ASSERT_FALSE(isInitialized());
}

void test_gps_compilation() {
    // Test that GPS code compiles
    #include "src/sensors/gps_sensor.h"
    
    GPS::Config config = GPS::getDefaultConfig();
    TEST_ASSERT_EQUAL(9600, config.baud_rate);
    TEST_ASSERT_EQUAL(1, config.uart_num);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    RUN_TEST(test_hal_basic_functionality);
    RUN_TEST(test_gps_compilation);
    
    return UNITY_END();
}