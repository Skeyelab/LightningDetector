#pragma once

#include <stdint.h>

// Device Configuration System
// Supports multiple Heltec devices with different pin configurations and capabilities

namespace DeviceConfig {

    // Supported device types
    enum class DeviceType {
        HELTEC_WIFI_LORA_32_V3,    // Current device
        HELTEC_WIRELESS_TRACKER_V1_1, // New Wireless Tracker
        UNKNOWN
    };

    // Device capabilities
    struct DeviceCapabilities {
        bool hasOLED;
        bool hasLoRa;
        bool hasWiFi;
        bool hasGPS;
        bool hasBattery;
        bool hasVext;
        uint8_t oledWidth;
        uint8_t oledHeight;
        uint32_t flashSize;
        uint32_t sramSize;
    };

    // Pin configuration for each device
    struct PinConfig {
        // OLED pins
        uint8_t oledSda;
        uint8_t oledScl;
        uint8_t oledRst;

        // LoRa pins
        uint8_t loraNss;
        uint8_t loraDio1;
        uint8_t loraRst;
        uint8_t loraBusy;

        // System pins
        uint8_t button;
        uint8_t vext;

        // Sensor pins
        uint8_t lightningIrq;
        uint8_t lightningCs;

        // Actuator pins
        uint8_t ledData;
        uint8_t buzzer;

        // GPS pins (if available)
        uint8_t gpsTx;
        uint8_t gpsRx;
        uint8_t gpsPps;
    };

    // Device-specific configurations
    namespace Devices {

        // Heltec WiFi LoRa 32 V3 (current device)
        namespace HeltecV3 {
            constexpr DeviceType TYPE = DeviceType::HELTEC_WIFI_LORA_32_V3;

            constexpr DeviceCapabilities CAPABILITIES = {
                .hasOLED = true,
                .hasLoRa = true,
                .hasWiFi = true,
                .hasGPS = false,
                .hasBattery = true,
                .hasVext = true,
                .oledWidth = 128,
                .oledHeight = 64,
                .flashSize = 16 * 1024 * 1024,  // 16MB
                .sramSize = 512 * 1024          // 512KB
            };

            constexpr PinConfig PINS = {
                .oledSda = 17,
                .oledScl = 18,
                .oledRst = 21,
                .loraNss = 8,
                .loraDio1 = 14,
                .loraRst = 12,
                .loraBusy = 13,
                .button = 0,
                .vext = 36,
                .lightningIrq = 4,
                .lightningCs = 5,
                .ledData = 2,
                .buzzer = 3,
                .gpsTx = 255,  // Not available
                .gpsRx = 255,  // Not available
                .gpsPps = 255   // Not available
            };
        };

        // Heltec Wireless Tracker V1.1 (new device)
        namespace WirelessTrackerV1_1 {
            constexpr DeviceType TYPE = DeviceType::HELTEC_WIRELESS_TRACKER_V1_1;

            constexpr DeviceCapabilities CAPABILITIES = {
                .hasOLED = true,
                .hasLoRa = true,
                .hasWiFi = true,   // Wireless Tracker has WiFi (same board as V3)
                .hasGPS = true,     // Has built-in GPS
                .hasBattery = true,
                .hasVext = true,
                .oledWidth = 128,
                .oledHeight = 64,
                .flashSize = 8 * 1024 * 1024,   // 8MB
                .sramSize = 256 * 1024          // 256KB
            };

            constexpr PinConfig PINS = {
                .oledSda = 21,     // Different I2C pins
                .oledScl = 22,
                .oledRst = 23,
                .loraNss = 8,      // Same LoRa pins
                .loraDio1 = 14,
                .loraRst = 12,
                .loraBusy = 13,
                .button = 0,
                .vext = 36,
                .lightningIrq = 4,
                .lightningCs = 5,
                .ledData = 2,
                .buzzer = 3,
                .gpsTx = 17,       // GPS UART TX
                .gpsRx = 18,       // GPS UART RX
                .gpsPps = 19       // GPS PPS signal
            };
        };
    };

    // Device detection and configuration
    class DeviceManager {
    public:
        static DeviceType detectDevice();
        static const DeviceCapabilities& getCapabilities(DeviceType device);
        static const PinConfig& getPinConfig(DeviceType device);
        static const char* getDeviceName(DeviceType device);

        // Get current device configuration
        static DeviceType getCurrentDevice();
        static const DeviceCapabilities& getCurrentCapabilities();
        static const PinConfig& getCurrentPinConfig();

    private:
        static DeviceType currentDevice;
        static bool deviceDetected;

        static DeviceType detectByHardware();
    };

    // Global device configuration access
    extern DeviceType CURRENT_DEVICE;
    extern const DeviceCapabilities* CURRENT_CAPABILITIES;
    extern const PinConfig* CURRENT_PINS;
}
