#pragma once

#include "device_config.h"

// Device-specific feature flags and conditional compilation
// This header provides macros and constants for different device capabilities

namespace DeviceFeatures {

    // WiFi support
    #ifdef ENABLE_WIFI_OTA
        #define HAS_WIFI_SUPPORT 1
    #else
        #define HAS_WIFI_SUPPORT 0
    #endif

    // GPS support
    #define HAS_GPS_SUPPORT (DeviceConfig::DeviceManager::getCurrentCapabilities().hasGPS)

    // Display support
    #define HAS_OLED_SUPPORT (DeviceConfig::DeviceManager::getCurrentCapabilities().hasOLED)
    #define HAS_TFT_SUPPORT (DeviceConfig::DeviceManager::getCurrentCapabilities().hasTFT)

    // LoRa support
    #define HAS_LORA_SUPPORT (DeviceConfig::DeviceManager::getCurrentCapabilities().hasLoRa)

    // Battery support
    #define HAS_BATTERY_SUPPORT (DeviceConfig::DeviceManager::getCurrentCapabilities().hasBattery)

    // Vext power rail support
    #define HAS_VEXT_SUPPORT (DeviceConfig::DeviceManager::getCurrentCapabilities().hasVext)

    // Device-specific feature macros
    #ifdef HELTEC_V3_OLED
        #define DEVICE_HELTEC_V3 1
        #define DEVICE_WIRELESS_TRACKER 0
    #elif defined(WIRELESS_TRACKER)
        #define DEVICE_HELTEC_V3 0
        #define DEVICE_WIRELESS_TRACKER 1
    #else
        #define DEVICE_HELTEC_V3 0
        #define DEVICE_WIRELESS_TRACKER 0
    #endif

    // Conditional compilation helpers
    #define IF_HELTEC_V3(code) if (DEVICE_HELTEC_V3) { code }
    #define IF_WIRELESS_TRACKER(code) if (DEVICE_WIRELESS_TRACKER) { code }
    #define IF_HAS_WIFI(code) if (HAS_WIFI_SUPPORT) { code }
    #define IF_HAS_GPS(code) if (HAS_GPS_SUPPORT) { code }
    #define IF_HAS_OLED(code) if (HAS_OLED_SUPPORT) { code }
    #define IF_HAS_TFT(code) if (HAS_TFT_SUPPORT) { code }
    #define IF_HAS_LORA(code) if (HAS_LORA_SUPPORT) { code }
    #define IF_HAS_BATTERY(code) if (HAS_BATTERY_SUPPORT) { code }
    #define IF_HAS_VEXT(code) if (HAS_VEXT_SUPPORT) { code }

    // Device-specific constants
    namespace Constants {
        #if DEVICE_HELTEC_V3
            constexpr uint32_t FLASH_SIZE = 16 * 1024 * 1024;  // 16MB
            constexpr uint32_t SRAM_SIZE = 512 * 1024;         // 512KB
            constexpr uint8_t OLED_WIDTH = 128;
            constexpr uint8_t OLED_HEIGHT = 64;
        #elif DEVICE_WIRELESS_TRACKER
            constexpr uint32_t FLASH_SIZE = 8 * 1024 * 1024;   // 8MB
            constexpr uint32_t SRAM_SIZE = 256 * 1024;         // 256KB
            constexpr uint8_t OLED_WIDTH = 128;
            constexpr uint8_t OLED_HEIGHT = 64;
        #else
            constexpr uint32_t FLASH_SIZE = 16 * 1024 * 1024;  // Default to V3
            constexpr uint32_t SRAM_SIZE = 512 * 1024;         // Default to V3
            constexpr uint8_t OLED_WIDTH = 128;
            constexpr uint8_t OLED_HEIGHT = 64;
        #endif
    }

    // Feature availability checks
    inline bool hasWiFi() { return HAS_WIFI_SUPPORT; }
    inline bool hasGPS() { return HAS_GPS_SUPPORT; }
    inline bool hasOLED() { return HAS_OLED_SUPPORT; }
    inline bool hasTFT() { return HAS_TFT_SUPPORT; }
    inline bool hasLoRa() { return HAS_LORA_SUPPORT; }
    inline bool hasBattery() { return HAS_BATTERY_SUPPORT; }
    inline bool hasVext() { return HAS_VEXT_SUPPORT; }
    inline bool isHeltecV3() { return DEVICE_HELTEC_V3; }
    inline bool isWirelessTracker() { return DEVICE_WIRELESS_TRACKER; }

    // Device-specific initialization functions
    bool initializeDeviceFeatures();
    void printDeviceCapabilities();
}
