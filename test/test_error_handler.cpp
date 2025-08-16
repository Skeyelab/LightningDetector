// Comprehensive tests for the error handling system
#include <unity.h>
#include "../src/system/error_handler.h"
#include <cstring>

// Global variables for callback testing
static ErrorHandling::ErrorInfo lastCallbackError;
static int callbackCount = 0;

// Test callback function
void testErrorCallback(const ErrorHandling::ErrorInfo& error) {
    lastCallbackError = error;
    callbackCount++;
}

void setUp(void) {
    // Reset callback tracking
    callbackCount = 0;
    memset(&lastCallbackError, 0, sizeof(lastCallbackError));
}

void tearDown(void) {
    // Clean up after each test
    ErrorHandling::clearErrors();
}

void test_error_enums() {
    // Test severity levels
    TEST_ASSERT_EQUAL_INT(0, (int)ErrorHandling::Severity::INFO);
    TEST_ASSERT_EQUAL_INT(1, (int)ErrorHandling::Severity::WARNING);
    TEST_ASSERT_EQUAL_INT(2, (int)ErrorHandling::Severity::ERROR);
    TEST_ASSERT_EQUAL_INT(3, (int)ErrorHandling::Severity::CRITICAL);

    // Test categories
    TEST_ASSERT_EQUAL_INT(0, (int)ErrorHandling::Category::HARDWARE);
    TEST_ASSERT_EQUAL_INT(1, (int)ErrorHandling::Category::RADIO);
    TEST_ASSERT_EQUAL_INT(2, (int)ErrorHandling::Category::WIFI);
    TEST_ASSERT_EQUAL_INT(3, (int)ErrorHandling::Category::SENSOR);
    TEST_ASSERT_EQUAL_INT(4, (int)ErrorHandling::Category::ACTUATOR);
    TEST_ASSERT_EQUAL_INT(5, (int)ErrorHandling::Category::OTA);
    TEST_ASSERT_EQUAL_INT(6, (int)ErrorHandling::Category::SYSTEM);
    TEST_ASSERT_EQUAL_INT(7, (int)ErrorHandling::Category::CONFIG);

    // Test error codes
    TEST_ASSERT_EQUAL_INT(100, (int)ErrorHandling::Code::OLED_INIT_FAILED);
    TEST_ASSERT_EQUAL_INT(200, (int)ErrorHandling::Code::RADIO_INIT_FAILED);
    TEST_ASSERT_EQUAL_INT(300, (int)ErrorHandling::Code::WIFI_CONNECT_FAILED);
    TEST_ASSERT_EQUAL_INT(400, (int)ErrorHandling::Code::SENSOR_INIT_FAILED);
    TEST_ASSERT_EQUAL_INT(500, (int)ErrorHandling::Code::LED_INIT_FAILED);
    TEST_ASSERT_EQUAL_INT(600, (int)ErrorHandling::Code::OTA_INIT_FAILED);
    TEST_ASSERT_EQUAL_INT(700, (int)ErrorHandling::Code::MEMORY_ALLOCATION_FAILED);
    TEST_ASSERT_EQUAL_INT(800, (int)ErrorHandling::Code::CONFIG_LOAD_FAILED);
}

void test_error_info_structure() {
    // Test ErrorInfo structure
    ErrorHandling::ErrorInfo errorInfo = {};

    // Test initialization
    errorInfo.code = ErrorHandling::Code::OLED_INIT_FAILED;
    errorInfo.category = ErrorHandling::Category::HARDWARE;
    errorInfo.severity = ErrorHandling::Severity::ERROR;
    errorInfo.timestamp = 123456789;
    errorInfo.message = "OLED display failed to initialize";
    errorInfo.module = "display_manager";
    errorInfo.data = 0x42;

    // Verify fields
    TEST_ASSERT_EQUAL_INT((int)ErrorHandling::Code::OLED_INIT_FAILED, (int)errorInfo.code);
    TEST_ASSERT_EQUAL_INT((int)ErrorHandling::Category::HARDWARE, (int)errorInfo.category);
    TEST_ASSERT_EQUAL_INT((int)ErrorHandling::Severity::ERROR, (int)errorInfo.severity);
    TEST_ASSERT_EQUAL_UINT32(123456789, errorInfo.timestamp);
    TEST_ASSERT_EQUAL_STRING("OLED display failed to initialize", errorInfo.message);
    TEST_ASSERT_EQUAL_STRING("display_manager", errorInfo.module);
    TEST_ASSERT_EQUAL_UINT32(0x42, errorInfo.data);
}

void test_initialization() {
    // Test that initialization works
    ErrorHandling::initialize();
    
    // Test that error count starts at 0
    TEST_ASSERT_EQUAL_UINT32(0, ErrorHandling::getErrorCount());
    TEST_ASSERT_FALSE(ErrorHandling::hasErrors());
}

void test_error_reporting() {
    ErrorHandling::initialize();
    
    // Report a test error
    ErrorHandling::reportError(
        ErrorHandling::Code::OLED_INIT_FAILED,
        ErrorHandling::Category::HARDWARE,
        ErrorHandling::Severity::ERROR,
        "test_module",
        "Test error message",
        0x1234
    );
    
    // Verify error count increased
    TEST_ASSERT_EQUAL_UINT32(1, ErrorHandling::getErrorCount());
    TEST_ASSERT_EQUAL_UINT32(1, ErrorHandling::getErrorCount(ErrorHandling::Category::HARDWARE));
    TEST_ASSERT_TRUE(ErrorHandling::hasErrors());
    TEST_ASSERT_TRUE(ErrorHandling::hasErrors(ErrorHandling::Severity::ERROR));
}

void test_error_callback() {
    ErrorHandling::initialize();
    
    // Register callback
    ErrorHandling::registerCallback(testErrorCallback);
    
    // Report an error
    ErrorHandling::reportError(
        ErrorHandling::Code::WIFI_CONNECT_FAILED,
        ErrorHandling::Category::WIFI,
        ErrorHandling::Severity::WARNING,
        "wifi_manager",
        "Connection timeout"
    );
    
    // Verify callback was called
    TEST_ASSERT_EQUAL_INT(1, callbackCount);
    TEST_ASSERT_EQUAL_INT((int)ErrorHandling::Code::WIFI_CONNECT_FAILED, (int)lastCallbackError.code);
    TEST_ASSERT_EQUAL_INT((int)ErrorHandling::Category::WIFI, (int)lastCallbackError.category);
    TEST_ASSERT_EQUAL_INT((int)ErrorHandling::Severity::WARNING, (int)lastCallbackError.severity);
    TEST_ASSERT_EQUAL_STRING("wifi_manager", lastCallbackError.module);
    TEST_ASSERT_EQUAL_STRING("Connection timeout", lastCallbackError.message);
}

void test_get_last_error() {
    ErrorHandling::initialize();
    
    // Report multiple errors
    ErrorHandling::reportError(
        ErrorHandling::Code::SENSOR_READ_FAILED,
        ErrorHandling::Category::SENSOR,
        ErrorHandling::Severity::WARNING,
        "sensor1",
        "First error"
    );
    
    ErrorHandling::reportError(
        ErrorHandling::Code::LED_UPDATE_FAILED,
        ErrorHandling::Category::ACTUATOR,
        ErrorHandling::Severity::ERROR,
        "led_manager",
        "Second error"
    );
    
    // Get last error
    ErrorHandling::ErrorInfo lastError;
    TEST_ASSERT_TRUE(ErrorHandling::getLastError(lastError));
    
    // Should be the second error
    TEST_ASSERT_EQUAL_INT((int)ErrorHandling::Code::LED_UPDATE_FAILED, (int)lastError.code);
    TEST_ASSERT_EQUAL_INT((int)ErrorHandling::Category::ACTUATOR, (int)lastError.category);
    TEST_ASSERT_EQUAL_STRING("led_manager", lastError.module);
}

void test_error_statistics() {
    ErrorHandling::initialize();
    
    // Report errors in different categories
    ErrorHandling::reportError(
        ErrorHandling::Code::OLED_INIT_FAILED,
        ErrorHandling::Category::HARDWARE,
        ErrorHandling::Severity::ERROR,
        "display",
        "Hardware error"
    );
    
    ErrorHandling::reportError(
        ErrorHandling::Code::WIFI_CONNECT_FAILED,
        ErrorHandling::Category::WIFI,
        ErrorHandling::Severity::WARNING,
        "wifi",
        "WiFi error"
    );
    
    ErrorHandling::reportError(
        ErrorHandling::Code::SENSOR_READ_FAILED,
        ErrorHandling::Category::SENSOR,
        ErrorHandling::Severity::INFO,
        "sensor",
        "Sensor info"
    );
    
    // Check statistics
    TEST_ASSERT_EQUAL_UINT32(3, ErrorHandling::getErrorCount());
    TEST_ASSERT_EQUAL_UINT32(1, ErrorHandling::getErrorCount(ErrorHandling::Category::HARDWARE));
    TEST_ASSERT_EQUAL_UINT32(1, ErrorHandling::getErrorCount(ErrorHandling::Category::WIFI));
    TEST_ASSERT_EQUAL_UINT32(1, ErrorHandling::getErrorCount(ErrorHandling::Category::SENSOR));
}

void test_error_recovery() {
    ErrorHandling::initialize();
    
    // Test recovery for different error types
    TEST_ASSERT_TRUE(ErrorHandling::attemptRecovery(ErrorHandling::Code::OLED_INIT_FAILED));
    TEST_ASSERT_TRUE(ErrorHandling::attemptRecovery(ErrorHandling::Code::WIFI_CONNECT_FAILED));
    TEST_ASSERT_TRUE(ErrorHandling::attemptRecovery(ErrorHandling::Code::SENSOR_READ_FAILED));
    
    // Test recovery for non-recoverable errors
    TEST_ASSERT_FALSE(ErrorHandling::attemptRecovery(ErrorHandling::Code::MEMORY_ALLOCATION_FAILED));
    TEST_ASSERT_FALSE(ErrorHandling::attemptRecovery(ErrorHandling::Code::TASK_CREATION_FAILED));
}

void test_health_check() {
    ErrorHandling::initialize();
    
    // System should be healthy initially
    TEST_ASSERT_TRUE(ErrorHandling::performHealthCheck());
    
    // Add some non-critical errors
    for (int i = 0; i < 5; i++) {
        ErrorHandling::reportError(
            ErrorHandling::Code::SENSOR_READ_FAILED,
            ErrorHandling::Category::SENSOR,
            ErrorHandling::Severity::WARNING,
            "sensor",
            "Minor issue"
        );
    }
    
    // System should still be healthy
    TEST_ASSERT_TRUE(ErrorHandling::performHealthCheck());
    
    // Add a critical error
    ErrorHandling::reportError(
        ErrorHandling::Code::MEMORY_ALLOCATION_FAILED,
        ErrorHandling::Category::SYSTEM,
        ErrorHandling::Severity::CRITICAL,
        "system",
        "Critical memory issue"
    );
    
    // System should now be unhealthy
    TEST_ASSERT_FALSE(ErrorHandling::performHealthCheck());
}

void test_error_strings() {
    // Test error code to string conversion
    TEST_ASSERT_EQUAL_STRING("OLED_INIT_FAILED", 
        ErrorHandling::errorCodeToString(ErrorHandling::Code::OLED_INIT_FAILED));
    TEST_ASSERT_EQUAL_STRING("WIFI_CONNECT_FAILED", 
        ErrorHandling::errorCodeToString(ErrorHandling::Code::WIFI_CONNECT_FAILED));
    TEST_ASSERT_EQUAL_STRING("SENSOR_READ_FAILED", 
        ErrorHandling::errorCodeToString(ErrorHandling::Code::SENSOR_READ_FAILED));
    
    // Test category to string conversion
    TEST_ASSERT_EQUAL_STRING("HARDWARE", 
        ErrorHandling::categoryToString(ErrorHandling::Category::HARDWARE));
    TEST_ASSERT_EQUAL_STRING("WIFI", 
        ErrorHandling::categoryToString(ErrorHandling::Category::WIFI));
    TEST_ASSERT_EQUAL_STRING("SENSOR", 
        ErrorHandling::categoryToString(ErrorHandling::Category::SENSOR));
    
    // Test severity to string conversion
    TEST_ASSERT_EQUAL_STRING("INFO", 
        ErrorHandling::severityToString(ErrorHandling::Severity::INFO));
    TEST_ASSERT_EQUAL_STRING("WARNING", 
        ErrorHandling::severityToString(ErrorHandling::Severity::WARNING));
    TEST_ASSERT_EQUAL_STRING("ERROR", 
        ErrorHandling::severityToString(ErrorHandling::Severity::ERROR));
    TEST_ASSERT_EQUAL_STRING("CRITICAL", 
        ErrorHandling::severityToString(ErrorHandling::Severity::CRITICAL));
}

void test_clear_errors() {
    ErrorHandling::initialize();
    
    // Add some errors
    ErrorHandling::reportError(
        ErrorHandling::Code::OLED_INIT_FAILED,
        ErrorHandling::Category::HARDWARE,
        ErrorHandling::Severity::ERROR,
        "display",
        "Test error"
    );
    
    TEST_ASSERT_EQUAL_UINT32(1, ErrorHandling::getErrorCount());
    
    // Clear errors
    ErrorHandling::clearErrors();
    
    // Verify errors are cleared
    TEST_ASSERT_EQUAL_UINT32(0, ErrorHandling::getErrorCount());
    TEST_ASSERT_FALSE(ErrorHandling::hasErrors());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Basic functionality tests
    RUN_TEST(test_error_enums);
    RUN_TEST(test_error_info_structure);
    
    // Core functionality tests
    RUN_TEST(test_initialization);
    RUN_TEST(test_error_reporting);
    RUN_TEST(test_error_callback);
    RUN_TEST(test_get_last_error);
    RUN_TEST(test_error_statistics);
    RUN_TEST(test_error_recovery);
    RUN_TEST(test_health_check);
    RUN_TEST(test_error_strings);
    RUN_TEST(test_clear_errors);

    return UNITY_END();
}
