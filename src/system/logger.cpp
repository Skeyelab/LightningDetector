#include "logger.h"
#include <cstring>
#include <cstdio>
#include <cstdarg>

namespace Logging {

// Internal state
namespace {
    static Level currentLevel = Level::INFO;
    static uint8_t currentDestinations = static_cast<uint8_t>(Destination::SERIAL);
    static bool categoryEnabled[10] = {true}; // All categories enabled by default
    
    static uint32_t totalMessages = 0;
    static uint32_t messagesByLevel[6] = {0}; // Count for each level
    static uint32_t droppedMessages = 0;
    static uint32_t startTime = 0;
    
    static const char* levelStrings[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
    static const char* categoryStrings[] = {"SYSTEM", "HARDWARE", "RADIO", "WIFI", "SENSOR", "ACTUATOR", "OTA", "UI", "CONFIG", "TEST"};
    
    // Performance timing
    static constexpr size_t MAX_TIMERS = 20;
    static struct {
        const char* name;
        uint32_t startTime;
        bool active;
    } timers[MAX_TIMERS];
    static size_t timerCount = 0;
    
    static uint32_t getCurrentTime() {
        // In a real implementation, this would get the actual system time
        // For now, we'll use a simple counter
        static uint32_t time = 0;
        return ++time;
    }
    
    static void outputToSerial(const char* message) {
        // In Arduino environment, this would use Serial.print
        // For now, we'll use printf for testing
        printf("%s", message);
    }
    
    static void outputToDisplay(const char* message) {
        // In real implementation, this would output to OLED display
        // For now, we'll just use printf with a prefix
        printf("[DISPLAY] %s", message);
    }
    
    static void outputToRadio(const char* message) {
        // In real implementation, this would send via LoRa
        // For now, we'll just use printf with a prefix
        printf("[RADIO] %s", message);
    }
    
    static void outputToStorage(const char* message) {
        // In real implementation, this would store in flash
        // For now, we'll just use printf with a prefix
        printf("[STORAGE] %s", message);
    }
    
    // Helper function for internal use
    static void logInternal(Level level, Category category, const char* format, va_list args) {
        // Check if this level and category should be logged
        if (static_cast<int>(level) < static_cast<int>(currentLevel)) {
            droppedMessages++;
            return;
        }
        
        size_t catIndex = static_cast<size_t>(category);
        if (catIndex >= sizeof(categoryEnabled) / sizeof(categoryEnabled[0]) || !categoryEnabled[catIndex]) {
            droppedMessages++;
            return;
        }
        
        // Format the message
        char buffer[256];
        vsnprintf(buffer, sizeof(buffer), format, args);
        
        // Create formatted log entry
        char formattedMessage[512];
        snprintf(formattedMessage, sizeof(formattedMessage), "[%s][%s] %s\n",
                 levelStrings[static_cast<size_t>(level)],
                 categoryStrings[catIndex],
                 buffer);
        
        // Output to enabled destinations
        if (currentDestinations & static_cast<uint8_t>(Destination::SERIAL)) {
            outputToSerial(formattedMessage);
        }
        
        if (currentDestinations & static_cast<uint8_t>(Destination::DISPLAY)) {
            outputToDisplay(formattedMessage);
        }
        
        if (currentDestinations & static_cast<uint8_t>(Destination::RADIO)) {
            // Only send critical messages via radio
            if (level >= Level::ERROR) {
                outputToRadio(formattedMessage);
            }
        }
        
        if (currentDestinations & static_cast<uint8_t>(Destination::STORAGE)) {
            outputToStorage(formattedMessage);
        }
        
        // Update statistics (only count once per message, not per destination)
        totalMessages++;
        size_t levelIndex = static_cast<size_t>(level);
        if (levelIndex < sizeof(messagesByLevel) / sizeof(messagesByLevel[0])) {
            messagesByLevel[levelIndex]++;
        }
    }
}

void initialize(Level minLevel, uint8_t destinations) {
    currentLevel = minLevel;
    currentDestinations = destinations;
    
    // Enable all categories by default
    for (int i = 0; i < 10; i++) {
        categoryEnabled[i] = true;
    }
    
    // Initialize timing system
    memset(timers, 0, sizeof(timers));
    timerCount = 0;
    
    // Set start time
    startTime = getCurrentTime();
    
    // Log system boot
    logSystemBoot();
}

void setLevel(Level level) {
    currentLevel = level;
}

void enableCategory(Category category, bool enabled) {
    size_t catIndex = static_cast<size_t>(category);
    if (catIndex < sizeof(categoryEnabled) / sizeof(categoryEnabled[0])) {
        categoryEnabled[catIndex] = enabled;
    }
}

void setDestinations(uint8_t destinations) {
    currentDestinations = destinations;
}

void log(Level level, Category category, const char* format, ...) {
    va_list args;
    va_start(args, format);
    logInternal(level, category, format, args);
    va_end(args);
}

void trace(Category category, const char* format, ...) {
    va_list args;
    va_start(args, format);
    logInternal(Level::TRACE, category, format, args);
    va_end(args);
}

void debug(Category category, const char* format, ...) {
    va_list args;
    va_start(args, format);
    logInternal(Level::DEBUG, category, format, args);
    va_end(args);
}

void info(Category category, const char* format, ...) {
    va_list args;
    va_start(args, format);
    logInternal(Level::INFO, category, format, args);
    va_end(args);
}

void warn(Category category, const char* format, ...) {
    va_list args;
    va_start(args, format);
    logInternal(Level::WARN, category, format, args);
    va_end(args);
}

void error(Category category, const char* format, ...) {
    va_list args;
    va_start(args, format);
    logInternal(Level::ERROR, category, format, args);
    va_end(args);
}

void fatal(Category category, const char* format, ...) {
    va_list args;
    va_start(args, format);
    logInternal(Level::FATAL, category, format, args);
    va_end(args);
}

void logSystemBoot() {
    info(Category::SYSTEM, "System booting up");
    logMemoryUsage();
}

void logMemoryUsage() {
    // In real implementation, this would get actual memory usage
    // For now, we'll log a placeholder
    info(Category::SYSTEM, "Memory usage: %d%% (placeholder)", 75);
}

void logRadioStats() {
    // In real implementation, this would get actual radio statistics
    // For now, we'll log placeholders
    info(Category::RADIO, "Radio stats: RSSI=%d, SNR=%d, Packets=%d", -45, 12, 156);
}

void logSensorReading(const char* sensorName, float value, const char* unit) {
    info(Category::SENSOR, "Sensor %s: %.2f %s", sensorName, value, unit);
}

void logError(const char* module, const char* errorMsg, uint32_t errorCode) {
    if (errorCode > 0) {
        error(Category::SYSTEM, "Error in %s: %s (code: %lu)", module, errorMsg, errorCode);
    } else {
        error(Category::SYSTEM, "Error in %s: %s", module, errorMsg);
    }
}

void startTimer(const char* name) {
    if (timerCount >= MAX_TIMERS) {
        warn(Category::SYSTEM, "Too many timers, cannot start %s", name);
        return;
    }
    
    // Find existing timer or create new one
    size_t index = 0;
    for (; index < timerCount; index++) {
        if (timers[index].name == name) {
            break;
        }
    }
    
    if (index == timerCount) {
        timers[index].name = name;
        timerCount++;
    }
    
    timers[index].startTime = getCurrentTime();
    timers[index].active = true;
}

void endTimer(const char* name) {
    for (size_t i = 0; i < timerCount; i++) {
        if (timers[i].name == name && timers[i].active) {
            uint32_t duration = getCurrentTime() - timers[i].startTime;
            timers[i].active = false;
            debug(Category::SYSTEM, "Timer %s: %lu ms", name, duration);
            return;
        }
    }
    
    warn(Category::SYSTEM, "Timer %s not found or not active", name);
}

const char* levelToString(Level level) {
    size_t index = static_cast<size_t>(level);
    if (index < sizeof(levelStrings) / sizeof(levelStrings[0])) {
        return levelStrings[index];
    }
    return "UNKNOWN";
}

const char* categoryToString(Category category) {
    size_t index = static_cast<size_t>(category);
    if (index < sizeof(categoryStrings) / sizeof(categoryStrings[0])) {
        return categoryStrings[index];
    }
    return "UNKNOWN";
}

void getStats(LogStats& stats) {
    stats.totalMessages = totalMessages;
    memcpy(stats.messagesByLevel, messagesByLevel, sizeof(messagesByLevel));
    stats.droppedMessages = droppedMessages;
    stats.uptime = getCurrentTime() - startTime;
}

} // namespace Logging