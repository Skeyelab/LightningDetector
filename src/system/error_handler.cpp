#include "error_handler.h"
#include <cstring>
#include <cstdio>

namespace ErrorHandling {

// Internal state
namespace {
    static constexpr size_t MAX_ERRORS = 50;
    static constexpr size_t MAX_CALLBACKS = 10;
    
    struct ErrorEntry {
        ErrorInfo info;
        bool active;
    };
    
    static ErrorEntry errorHistory[MAX_ERRORS];
    static size_t errorCount = 0;
    static size_t nextErrorIndex = 0;
    
    static ErrorCallback callbacks[MAX_CALLBACKS];
    static size_t callbackCount = 0;
    
    static bool systemInitialized = false;
    static uint32_t totalErrorsReported = 0;
    static uint32_t errorsByCategory[8] = {0}; // 8 categories
    static uint32_t errorsBySeverity[4] = {0}; // 4 severity levels
    
    static uint32_t getCurrentTimestamp() {
        // In a real implementation, this would get the actual system time
        // For now, we'll use a simple counter
        static uint32_t timestamp = 0;
        return ++timestamp;
    }
}

void initialize() {
    if (systemInitialized) {
        return;
    }
    
    // Clear error history
    memset(errorHistory, 0, sizeof(errorHistory));
    errorCount = 0;
    nextErrorIndex = 0;
    
    // Clear callbacks
    memset(callbacks, 0, sizeof(callbacks));
    callbackCount = 0;
    
    // Clear statistics
    totalErrorsReported = 0;
    memset(errorsByCategory, 0, sizeof(errorsByCategory));
    memset(errorsBySeverity, 0, sizeof(errorsBySeverity));
    
    systemInitialized = true;
}

void reportError(Code code, Category category, Severity severity,
                const char* module, const char* message, uint32_t data) {
    if (!systemInitialized) {
        initialize();
    }
    
    // Create error info
    ErrorInfo errorInfo;
    errorInfo.code = code;
    errorInfo.category = category;
    errorInfo.severity = severity;
    errorInfo.timestamp = getCurrentTimestamp();
    errorInfo.message = message ? message : "";
    errorInfo.module = module ? module : "unknown";
    errorInfo.data = data;
    
    // Add to history (circular buffer)
    errorHistory[nextErrorIndex].info = errorInfo;
    errorHistory[nextErrorIndex].active = true;
    
    if (errorCount < MAX_ERRORS) {
        errorCount++;
    }
    
    nextErrorIndex = (nextErrorIndex + 1) % MAX_ERRORS;
    
    // Update statistics
    totalErrorsReported++;
    if (static_cast<size_t>(category) < sizeof(errorsByCategory) / sizeof(errorsByCategory[0])) {
        errorsByCategory[static_cast<size_t>(category)]++;
    }
    if (static_cast<size_t>(severity) < sizeof(errorsBySeverity) / sizeof(errorsBySeverity[0])) {
        errorsBySeverity[static_cast<size_t>(severity)]++;
    }
    
    // Notify callbacks
    for (size_t i = 0; i < callbackCount; i++) {
        if (callbacks[i]) {
            callbacks[i](errorInfo);
        }
    }
}

void registerCallback(ErrorCallback callback) {
    if (!systemInitialized) {
        initialize();
    }
    
    if (callbackCount < MAX_CALLBACKS && callback) {
        callbacks[callbackCount++] = callback;
    }
}

bool getLastError(ErrorInfo& error) {
    if (!systemInitialized || errorCount == 0) {
        return false;
    }
    
    // Find the most recent active error
    for (int i = 0; i < static_cast<int>(MAX_ERRORS); i++) {
        size_t index = (nextErrorIndex - 1 - i + MAX_ERRORS) % MAX_ERRORS;
        if (errorHistory[index].active) {
            error = errorHistory[index].info;
            return true;
        }
    }
    
    return false;
}

void clearErrors() {
    if (!systemInitialized) {
        return;
    }
    
    memset(errorHistory, 0, sizeof(errorHistory));
    errorCount = 0;
    nextErrorIndex = 0;
    
    // Clear statistics
    totalErrorsReported = 0;
    memset(errorsByCategory, 0, sizeof(errorsByCategory));
    memset(errorsBySeverity, 0, sizeof(errorsBySeverity));
}

uint32_t getErrorCount(Category category) {
    if (!systemInitialized) {
        return 0;
    }
    
    if (category == Category::SYSTEM) {
        return totalErrorsReported;
    }
    
    size_t catIndex = static_cast<size_t>(category);
    if (catIndex < sizeof(errorsByCategory) / sizeof(errorsByCategory[0])) {
        return errorsByCategory[catIndex];
    }
    
    return 0;
}

bool hasErrors(Severity minSeverity) {
    if (!systemInitialized) {
        return false;
    }
    
    for (size_t i = static_cast<size_t>(minSeverity); i < 4; i++) {
        if (errorsBySeverity[i] > 0) {
            return true;
        }
    }
    
    return false;
}

bool attemptRecovery(Code code) {
    if (!systemInitialized) {
        return false;
    }
    
    // Simple recovery strategies based on error code
    switch (code) {
        case Code::OLED_INIT_FAILED:
        case Code::I2C_COMMUNICATION_FAILED:
            // Hardware errors - could retry initialization
            return true;
            
        case Code::WIFI_CONNECT_FAILED:
        case Code::WIFI_TIMEOUT:
            // WiFi errors - could retry connection
            return true;
            
        case Code::SENSOR_READ_FAILED:
            // Sensor errors - could retry reading
            return true;
            
        default:
            // For other errors, recovery may not be possible
            return false;
    }
}

bool performHealthCheck() {
    if (!systemInitialized) {
        return false;
    }
    
    // Check if we have critical errors
    if (errorsBySeverity[static_cast<size_t>(Severity::CRITICAL)] > 0) {
        return false;
    }
    
    // Check if error count is reasonable
    if (totalErrorsReported > 1000) {
        return false;
    }
    
    return true;
}

const char* errorCodeToString(Code code) {
    switch (code) {
        // Hardware errors
        case Code::OLED_INIT_FAILED: return "OLED_INIT_FAILED";
        case Code::I2C_COMMUNICATION_FAILED: return "I2C_COMMUNICATION_FAILED";
        case Code::POWER_MANAGEMENT_FAILED: return "POWER_MANAGEMENT_FAILED";
        
        // Radio errors
        case Code::RADIO_INIT_FAILED: return "RADIO_INIT_FAILED";
        case Code::RADIO_TX_FAILED: return "RADIO_TX_FAILED";
        case Code::RADIO_RX_FAILED: return "RADIO_RX_FAILED";
        case Code::RADIO_CONFIG_FAILED: return "RADIO_CONFIG_FAILED";
        
        // WiFi errors
        case Code::WIFI_CONNECT_FAILED: return "WIFI_CONNECT_FAILED";
        case Code::WIFI_TIMEOUT: return "WIFI_TIMEOUT";
        case Code::WIFI_AUTH_FAILED: return "WIFI_AUTH_FAILED";
        case Code::WIFI_CONFIG_INVALID: return "WIFI_CONFIG_INVALID";
        
        // Sensor errors
        case Code::SENSOR_INIT_FAILED: return "SENSOR_INIT_FAILED";
        case Code::SENSOR_READ_FAILED: return "SENSOR_READ_FAILED";
        case Code::SENSOR_CALIBRATION_FAILED: return "SENSOR_CALIBRATION_FAILED";
        
        // Actuator errors
        case Code::LED_INIT_FAILED: return "LED_INIT_FAILED";
        case Code::LED_UPDATE_FAILED: return "LED_UPDATE_FAILED";
        case Code::BUZZER_INIT_FAILED: return "BUZZER_INIT_FAILED";
        
        // OTA errors
        case Code::OTA_INIT_FAILED: return "OTA_INIT_FAILED";
        case Code::OTA_DOWNLOAD_FAILED: return "OTA_DOWNLOAD_FAILED";
        case Code::OTA_VERIFICATION_FAILED: return "OTA_VERIFICATION_FAILED";
        case Code::OTA_STORAGE_FAILED: return "OTA_STORAGE_FAILED";
        
        // System errors
        case Code::MEMORY_ALLOCATION_FAILED: return "MEMORY_ALLOCATION_FAILED";
        case Code::TASK_CREATION_FAILED: return "TASK_CREATION_FAILED";
        case Code::WATCHDOG_TIMEOUT: return "WATCHDOG_TIMEOUT";
        
        // Config errors
        case Code::CONFIG_LOAD_FAILED: return "CONFIG_LOAD_FAILED";
        case Code::CONFIG_SAVE_FAILED: return "CONFIG_SAVE_FAILED";
        case Code::CONFIG_VALIDATION_FAILED: return "CONFIG_VALIDATION_FAILED";
        
        default: return "UNKNOWN_ERROR";
    }
}

const char* categoryToString(Category category) {
    switch (category) {
        case Category::HARDWARE: return "HARDWARE";
        case Category::RADIO: return "RADIO";
        case Category::WIFI: return "WIFI";
        case Category::SENSOR: return "SENSOR";
        case Category::ACTUATOR: return "ACTUATOR";
        case Category::OTA: return "OTA";
        case Category::SYSTEM: return "SYSTEM";
        case Category::CONFIG: return "CONFIG";
        default: return "UNKNOWN";
    }
}

const char* severityToString(Severity severity) {
    switch (severity) {
        case Severity::INFO: return "INFO";
        case Severity::WARNING: return "WARNING";
        case Severity::ERROR: return "ERROR";
        case Severity::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

} // namespace ErrorHandling