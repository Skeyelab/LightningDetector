#include "device_config.h"
#include <Arduino.h>

namespace DeviceConfig {

    // Static member initialization
    DeviceType DeviceManager::currentDevice = DeviceType::UNKNOWN;
    bool DeviceManager::deviceDetected = false;

    // Global device configuration access
    DeviceType CURRENT_DEVICE = DeviceType::UNKNOWN;
    const DeviceCapabilities* CURRENT_CAPABILITIES = nullptr;
    const PinConfig* CURRENT_PINS = nullptr;

    // Device detection by hardware characteristics
    DeviceType DeviceManager::detectByHardware() {
        // Try to detect by checking hardware characteristics
        // This is a simplified detection - in practice, you might use chip ID, flash size, etc.

        // Prioritize compile-time detection flags
        #ifdef HELTEC_V3_OLED
            return DeviceType::HELTEC_WIFI_LORA_32_V3;
        #elif defined(WIRELESS_TRACKER)
            return DeviceType::HELTEC_WIRELESS_TRACKER_V1_1;
        #endif

        // Fallback to runtime detection based on flash size
        #ifdef ESP32
            uint32_t flashSize = ESP.getFlashChipSize();

            if (flashSize >= 16 * 1024 * 1024) {  // 16MB or larger
                return DeviceType::HELTEC_WIFI_LORA_32_V3;
            } else if (flashSize >= 8 * 1024 * 1024) {  // 8MB
                return DeviceType::HELTEC_WIRELESS_TRACKER_V1_1;
            }
        #endif

        return DeviceType::UNKNOWN;
    }

    DeviceType DeviceManager::detectDevice() {
        if (!deviceDetected) {
            currentDevice = detectByHardware();
            deviceDetected = true;

            // Set global configuration
            CURRENT_DEVICE = currentDevice;
            CURRENT_CAPABILITIES = &getCapabilities(currentDevice);
            CURRENT_PINS = &getPinConfig(currentDevice);
        }
        return currentDevice;
    }

    const DeviceCapabilities& DeviceManager::getCapabilities(DeviceType device) {
        switch (device) {
            case DeviceType::HELTEC_WIFI_LORA_32_V3:
                return Devices::HeltecV3::CAPABILITIES;
            case DeviceType::HELTEC_WIRELESS_TRACKER_V1_1:
                return Devices::WirelessTrackerV1_1::CAPABILITIES;
            default:
                // Return V3 capabilities as default
                return Devices::HeltecV3::CAPABILITIES;
        }
    }

    const PinConfig& DeviceManager::getPinConfig(DeviceType device) {
        switch (device) {
            case DeviceType::HELTEC_WIFI_LORA_32_V3:
                return Devices::HeltecV3::PINS;
            case DeviceType::HELTEC_WIRELESS_TRACKER_V1_1:
                return Devices::WirelessTrackerV1_1::PINS;
            default:
                // Return V3 pin config as default
                return Devices::HeltecV3::PINS;
        }
    }

    const char* DeviceManager::getDeviceName(DeviceType device) {
        switch (device) {
            case DeviceType::HELTEC_WIFI_LORA_32_V3:
                return "Heltec WiFi LoRa 32 V3";
            case DeviceType::HELTEC_WIRELESS_TRACKER_V1_1:
                return "Heltec Wireless Tracker V1.1";
            default:
                return "Unknown Device";
        }
    }

    DeviceType DeviceManager::getCurrentDevice() {
        return detectDevice();
    }

    const DeviceCapabilities& DeviceManager::getCurrentCapabilities() {
        detectDevice();
        return *CURRENT_CAPABILITIES;
    }

    const PinConfig& DeviceManager::getCurrentPinConfig() {
        detectDevice();
        return *CURRENT_PINS;
    }
}
