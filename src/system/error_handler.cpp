#include "error_handler.h"
#include <vector>
#include <cstring>
#include <algorithm>
#include <chrono>

namespace {
    using namespace ErrorHandling;
    std::vector<ErrorInfo> g_history;
    std::vector<ErrorCallback> g_callbacks;
    constexpr size_t kMaxHistory = 32;
    uint32_t getTimestamp() {
        using namespace std::chrono;
#ifdef ARDUINO
        return millis();
#else
        return static_cast<uint32_t>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
#endif
    }

    const char* copyString(const char* str) {
        return str; // assume static literals for now
    }
}

namespace ErrorHandling {

void initialize() {
    g_history.clear();
    g_callbacks.clear();
}

void reportError(Code code, Category category, Severity severity,
                 const char* module, const char* message, uint32_t data) {
    ErrorInfo info{};
    info.code = code;
    info.category = category;
    info.severity = severity;
    info.timestamp = getTimestamp();
    info.message = copyString(message);
    info.module = copyString(module);
    info.data = data;

    if (g_history.size() >= kMaxHistory) {
        g_history.erase(g_history.begin());
    }
    g_history.push_back(info);

    for (auto cb : g_callbacks) {
        if (cb) cb(info);
    }
}

void registerCallback(ErrorCallback callback) {
    if (callback) g_callbacks.push_back(callback);
}

bool getLastError(ErrorInfo& error) {
    if (g_history.empty()) return false;
    error = g_history.back();
    return true;
}

void clearErrors() {
    g_history.clear();
}

uint32_t getErrorCount(Category category) {
    return std::count_if(g_history.begin(), g_history.end(), [&](const ErrorInfo& e){ return e.category == category; });
}

bool hasErrors(Severity minSeverity) {
    return std::any_of(g_history.begin(), g_history.end(), [&](const ErrorInfo& e){ return static_cast<int>(e.severity) >= static_cast<int>(minSeverity); });
}

bool attemptRecovery(Code /*code*/) {
    // Placeholder: implement specific recovery per code.
    return false;
}

bool performHealthCheck() {
    // Simple implementation: healthy if no critical errors.
    return !hasErrors(Severity::CRITICAL);
}

static const char* codeStrings[] = {
    "OLED_INIT_FAILED","I2C_COMMUNICATION_FAILED","POWER_MANAGEMENT_FAILED",
    "RADIO_INIT_FAILED","RADIO_TX_FAILED","RADIO_RX_FAILED","RADIO_CONFIG_FAILED",
    "WIFI_CONNECT_FAILED","WIFI_TIMEOUT","WIFI_AUTH_FAILED","WIFI_CONFIG_INVALID",
    "SENSOR_INIT_FAILED","SENSOR_READ_FAILED","SENSOR_CALIBRATION_FAILED",
    "LED_INIT_FAILED","LED_UPDATE_FAILED","BUZZER_INIT_FAILED",
    "OTA_INIT_FAILED","OTA_DOWNLOAD_FAILED","OTA_VERIFICATION_FAILED","OTA_STORAGE_FAILED",
    "MEMORY_ALLOCATION_FAILED","TASK_CREATION_FAILED","WATCHDOG_TIMEOUT",
    "CONFIG_LOAD_FAILED","CONFIG_SAVE_FAILED","CONFIG_VALIDATION_FAILED"};

const char* errorCodeToString(Code code) {
    int idx = static_cast<int>(code);
    // map ranges roughly; not exhaustive
    switch(code) {
        case Code::OLED_INIT_FAILED: return codeStrings[0];
        case Code::I2C_COMMUNICATION_FAILED: return codeStrings[1];
        case Code::POWER_MANAGEMENT_FAILED: return codeStrings[2];
        case Code::RADIO_INIT_FAILED: return codeStrings[3];
        case Code::RADIO_TX_FAILED: return codeStrings[4];
        case Code::RADIO_RX_FAILED: return codeStrings[5];
        case Code::RADIO_CONFIG_FAILED: return codeStrings[6];
        case Code::WIFI_CONNECT_FAILED: return codeStrings[7];
        case Code::WIFI_TIMEOUT: return codeStrings[8];
        case Code::WIFI_AUTH_FAILED: return codeStrings[9];
        case Code::WIFI_CONFIG_INVALID: return codeStrings[10];
        case Code::SENSOR_INIT_FAILED: return codeStrings[11];
        case Code::SENSOR_READ_FAILED: return codeStrings[12];
        case Code::SENSOR_CALIBRATION_FAILED: return codeStrings[13];
        case Code::LED_INIT_FAILED: return codeStrings[14];
        case Code::LED_UPDATE_FAILED: return codeStrings[15];
        case Code::BUZZER_INIT_FAILED: return codeStrings[16];
        case Code::OTA_INIT_FAILED: return codeStrings[17];
        case Code::OTA_DOWNLOAD_FAILED: return codeStrings[18];
        case Code::OTA_VERIFICATION_FAILED: return codeStrings[19];
        case Code::OTA_STORAGE_FAILED: return codeStrings[20];
        case Code::MEMORY_ALLOCATION_FAILED: return codeStrings[21];
        case Code::TASK_CREATION_FAILED: return codeStrings[22];
        case Code::WATCHDOG_TIMEOUT: return codeStrings[23];
        case Code::CONFIG_LOAD_FAILED: return codeStrings[24];
        case Code::CONFIG_SAVE_FAILED: return codeStrings[25];
        case Code::CONFIG_VALIDATION_FAILED: return codeStrings[26];
        default: return "UNKNOWN_ERROR";
    }
}

const char* categoryToString(Category category) {
    switch(category) {
        case Category::HARDWARE: return "HARDWARE";
        case Category::RADIO: return "RADIO";
        case Category::WIFI: return "WIFI";
        case Category::SENSOR: return "SENSOR";
        case Category::ACTUATOR: return "ACTUATOR";
        case Category::OTA: return "OTA";
        case Category::SYSTEM: return "SYSTEM";
        case Category::CONFIG: return "CONFIG";
        default: return "UNKNOWN_CATEGORY";
    }
}

const char* severityToString(Severity severity) {
    switch(severity) {
        case Severity::INFO: return "INFO";
        case Severity::WARNING: return "WARNING";
        case Severity::ERROR: return "ERROR";
        case Severity::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN_SEVERITY";
    }
}

} // namespace ErrorHandling