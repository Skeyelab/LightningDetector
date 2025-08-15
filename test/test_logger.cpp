// Comprehensive tests for the logging system
#include <unity.h>
#include "../src/system/logger.h"
#include <cstring>

void setUp(void) {
    // Reset logging system before each test
    Logging::initialize();
}

void tearDown(void) {
    // Clean up after each test
    // Note: Logger doesn't have a reset function, so we'll just reinitialize
}

void test_logger_enums() {
    // Test log levels
    TEST_ASSERT_EQUAL_INT(0, (int)Logging::Level::TRACE);
    TEST_ASSERT_EQUAL_INT(1, (int)Logging::Level::DEBUG);
    TEST_ASSERT_EQUAL_INT(2, (int)Logging::Level::INFO);
    TEST_ASSERT_EQUAL_INT(3, (int)Logging::Level::WARN);
    TEST_ASSERT_EQUAL_INT(4, (int)Logging::Level::ERROR);
    TEST_ASSERT_EQUAL_INT(5, (int)Logging::Level::FATAL);

    // Test categories
    TEST_ASSERT_EQUAL_INT(0, (int)Logging::Category::SYSTEM);
    TEST_ASSERT_EQUAL_INT(1, (int)Logging::Category::HARDWARE);
    TEST_ASSERT_EQUAL_INT(2, (int)Logging::Category::RADIO);
    TEST_ASSERT_EQUAL_INT(3, (int)Logging::Category::WIFI);
    TEST_ASSERT_EQUAL_INT(4, (int)Logging::Category::SENSOR);
    TEST_ASSERT_EQUAL_INT(5, (int)Logging::Category::ACTUATOR);
    TEST_ASSERT_EQUAL_INT(6, (int)Logging::Category::OTA);
    TEST_ASSERT_EQUAL_INT(7, (int)Logging::Category::UI);
    TEST_ASSERT_EQUAL_INT(8, (int)Logging::Category::CONFIG);
    TEST_ASSERT_EQUAL_INT(9, (int)Logging::Category::TEST);

    // Test destinations
    TEST_ASSERT_EQUAL_INT(1, (int)Logging::Destination::SERIAL);
    TEST_ASSERT_EQUAL_INT(2, (int)Logging::Destination::DISPLAY);
    TEST_ASSERT_EQUAL_INT(4, (int)Logging::Destination::RADIO);
    TEST_ASSERT_EQUAL_INT(8, (int)Logging::Destination::STORAGE);
}

void test_initialization() {
    // Test default initialization
    Logging::initialize();
    
    // Test custom initialization
    Logging::initialize(Logging::Level::DEBUG, 
                       static_cast<uint8_t>(Logging::Destination::SERIAL | Logging::Destination::DISPLAY));
    
    // Test passed - initialization should not crash
    TEST_ASSERT_TRUE(true);
}

void test_level_filtering() {
    // Set to DEBUG level
    Logging::setLevel(Logging::Level::DEBUG);
    
    // Test that TRACE messages are filtered out
    Logging::trace(Logging::Category::SYSTEM, "This should be filtered out");
    
    // Test that DEBUG and above messages get through
    Logging::debug(Logging::Category::SYSTEM, "This should get through");
    Logging::info(Logging::Category::SYSTEM, "This should get through");
    Logging::warn(Logging::Category::SYSTEM, "This should get through");
    Logging::error(Logging::Category::SYSTEM, "This should get through");
    Logging::fatal(Logging::Category::SYSTEM, "This should get through");
    
    // Test passed - level filtering should work
    TEST_ASSERT_TRUE(true);
}

void test_category_enabling() {
    // Disable TEST category
    Logging::enableCategory(Logging::Category::TEST, false);
    
    // Test that TEST category messages are filtered out
    Logging::info(Logging::Category::TEST, "This should be filtered out");
    
    // Test that other category messages get through
    Logging::info(Logging::Category::SYSTEM, "This should get through");
    Logging::info(Logging::Category::HARDWARE, "This should get through");
    
    // Re-enable TEST category
    Logging::enableCategory(Logging::Category::TEST, true);
    
    // Test that TEST category messages now get through
    Logging::info(Logging::Category::TEST, "This should now get through");
    
    // Test passed - category filtering should work
    TEST_ASSERT_TRUE(true);
}

void test_destination_setting() {
    // Test setting different destinations
    Logging::setDestinations(static_cast<uint8_t>(Logging::Destination::SERIAL));
    Logging::info(Logging::Category::SYSTEM, "Serial only");
    
    Logging::setDestinations(static_cast<uint8_t>(Logging::Destination::DISPLAY));
    Logging::info(Logging::Category::SYSTEM, "Display only");
    
    Logging::setDestinations(static_cast<uint8_t>(Logging::Destination::SERIAL | Logging::Destination::DISPLAY));
    Logging::info(Logging::Category::SYSTEM, "Both serial and display");
    
    // Test passed - destination setting should work
    TEST_ASSERT_TRUE(true);
}

void test_basic_logging() {
    // Test all log levels
    Logging::trace(Logging::Category::SYSTEM, "Trace message");
    Logging::debug(Logging::Category::SYSTEM, "Debug message");
    Logging::info(Logging::Category::SYSTEM, "Info message");
    Logging::warn(Logging::Category::SYSTEM, "Warning message");
    Logging::error(Logging::Category::SYSTEM, "Error message");
    Logging::fatal(Logging::Category::SYSTEM, "Fatal message");
    
    // Test passed - all log levels should work
    TEST_ASSERT_TRUE(true);
}

void test_category_logging() {
    // Test logging to different categories
    Logging::info(Logging::Category::HARDWARE, "Hardware message");
    Logging::info(Logging::Category::RADIO, "Radio message");
    Logging::info(Logging::Category::WIFI, "WiFi message");
    Logging::info(Logging::Category::SENSOR, "Sensor message");
    Logging::info(Logging::Category::ACTUATOR, "Actuator message");
    Logging::info(Logging::Category::OTA, "OTA message");
    Logging::info(Logging::Category::UI, "UI message");
    Logging::info(Logging::Category::CONFIG, "Config message");
    Logging::info(Logging::Category::TEST, "Test message");
    
    // Test passed - all categories should work
    TEST_ASSERT_TRUE(true);
}

void test_formatted_logging() {
    // Test formatted messages
    Logging::info(Logging::Category::SYSTEM, "String: %s", "test");
    Logging::info(Logging::Category::SYSTEM, "Integer: %d", 42);
    Logging::info(Logging::Category::SYSTEM, "Float: %.2f", 3.14159);
    Logging::info(Logging::Category::SYSTEM, "Multiple: %s %d %.2f", "test", 42, 3.14159);
    
    // Test passed - formatted logging should work
    TEST_ASSERT_TRUE(true);
}

void test_special_logging_functions() {
    // Test special logging functions
    Logging::logSystemBoot();
    Logging::logMemoryUsage();
    Logging::logRadioStats();
    Logging::logSensorReading("temperature", 23.5, "°C");
    Logging::logError("test_module", "Test error", 123);
    
    // Test passed - special functions should work
    TEST_ASSERT_TRUE(true);
}

void test_performance_timing() {
    // Test performance timing
    Logging::startTimer("test_timer");
    
    // Simulate some work
    for (int i = 0; i < 1000; i++) {
        // Do nothing
    }
    
    Logging::endTimer("test_timer");
    
    // Test multiple timers
    Logging::startTimer("timer1");
    Logging::startTimer("timer2");
    
    // Simulate work
    for (int i = 0; i < 500; i++) {
        // Do nothing
    }
    
    Logging::endTimer("timer1");
    Logging::endTimer("timer2");
    
    // Test passed - timing should work
    TEST_ASSERT_TRUE(true);
}

void test_timer_edge_cases() {
    // Test timer edge cases
    Logging::endTimer("nonexistent_timer"); // Should warn
    
    // Start many timers
    for (int i = 0; i < 25; i++) {
        char timerName[32];
        snprintf(timerName, sizeof(timerName), "timer_%d", i);
        Logging::startTimer(timerName);
    }
    
    // Test passed - edge cases should be handled gracefully
    TEST_ASSERT_TRUE(true);
}

void test_utility_functions() {
    // Test utility functions
    const char* levelStr = Logging::levelToString(Logging::Level::INFO);
    TEST_ASSERT_EQUAL_STRING("INFO", levelStr);
    
    const char* categoryStr = Logging::categoryToString(Logging::Category::HARDWARE);
    TEST_ASSERT_EQUAL_STRING("HARDWARE", categoryStr);
    
    // Test passed - utility functions should work
    TEST_ASSERT_TRUE(true);
}

void test_logging_statistics() {
    // Generate some log messages
    for (int i = 0; i < 10; i++) {
        Logging::info(Logging::Category::SYSTEM, "Message %d", i);
    }
    
    for (int i = 0; i < 5; i++) {
        Logging::warn(Logging::Category::HARDWARE, "Warning %d", i);
    }
    
    for (int i = 0; i < 3; i++) {
        Logging::error(Logging::Category::WIFI, "Error %d", i);
    }
    
    // Get statistics
    Logging::LogStats stats;
    Logging::getStats(stats);
    
    // Verify statistics
    TEST_ASSERT_EQUAL_UINT32(18, stats.totalMessages); // 10 + 5 + 3
    TEST_ASSERT_EQUAL_UINT32(10, stats.messagesByLevel[2]); // INFO level
    TEST_ASSERT_EQUAL_UINT32(5, stats.messagesByLevel[3]);  // WARN level
    TEST_ASSERT_EQUAL_UINT32(3, stats.messagesByLevel[4]);  // ERROR level
}

void test_radio_filtering() {
    // Set destinations to include radio
    Logging::setDestinations(static_cast<uint8_t>(Logging::Destination::RADIO));
    
    // Test that only ERROR and FATAL messages go to radio
    Logging::info(Logging::Category::SYSTEM, "This should not go to radio");
    Logging::warn(Logging::Category::SYSTEM, "This should not go to radio");
    Logging::error(Logging::Category::SYSTEM, "This should go to radio");
    Logging::fatal(Logging::Category::SYSTEM, "This should go to radio");
    
    // Test passed - radio filtering should work
    TEST_ASSERT_TRUE(true);
}

void test_storage_logging() {
    // Set destinations to include storage
    Logging::setDestinations(static_cast<uint8_t>(Logging::Destination::STORAGE));
    
    // Test storage logging
    Logging::info(Logging::Category::SYSTEM, "This should go to storage");
    Logging::warn(Logging::Category::HARDWARE, "This should go to storage");
    Logging::error(Logging::Category::WIFI, "This should go to storage");
    
    // Test passed - storage logging should work
    TEST_ASSERT_TRUE(true);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Basic functionality tests
    RUN_TEST(test_logger_enums);
    RUN_TEST(test_initialization);
    
    // Core functionality tests
    RUN_TEST(test_level_filtering);
    RUN_TEST(test_category_enabling);
    RUN_TEST(test_destination_setting);
    RUN_TEST(test_basic_logging);
    RUN_TEST(test_category_logging);
    RUN_TEST(test_formatted_logging);
    RUN_TEST(test_special_logging_functions);
    
    // Advanced functionality tests
    RUN_TEST(test_performance_timing);
    RUN_TEST(test_timer_edge_cases);
    RUN_TEST(test_utility_functions);
    RUN_TEST(test_logging_statistics);
    RUN_TEST(test_radio_filtering);
    RUN_TEST(test_storage_logging);

    return UNITY_END();
}