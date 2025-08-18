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
        bool hasOLED;         // SSD1306 OLED display (Heltec V3)
        bool hasTFT;          // TFT-LCD display (Wireless Tracker)
        bool hasLoRa;         // SX1262 LoRa radio
        bool hasWiFi;         // ESP32-S3 WiFi
        bool hasGPS;          // GNSS module (UC6580 for Wireless Tracker)
        bool hasBattery;      // Battery management
        bool hasVext;         // Vext power rail
        uint8_t oledWidth;    // OLED width (if hasOLED)
        uint8_t oledHeight;   // OLED height (if hasOLED)
        uint8_t tftWidth;     // TFT width (if hasTFT)
        uint8_t tftHeight;    // TFT height (if hasTFT)
        uint32_t flashSize;   // Flash memory size
        uint32_t sramSize;    // SRAM size
    };

    // Pin configuration for each device
    struct PinConfig {
        // Display pins
        uint8_t oledSda;      // OLED I2C SDA (for Heltec V3)
        uint8_t oledScl;      // OLED I2C SCL (for Heltec V3)
        uint8_t oledRst;      // OLED Reset (for Heltec V3)
        uint8_t tftSdin;      // TFT-LCD SDIN (for Wireless Tracker)
        uint8_t tftSclk;      // TFT-LCD SCLK (for Wireless Tracker)
        uint8_t tftRs;        // TFT-LCD RS (for Wireless Tracker)
        uint8_t tftRes;       // TFT-LCD RESET (for Wireless Tracker)
        uint8_t tftCs;        // TFT-LCD CS (for Wireless Tracker)
        uint8_t tftLedK;      // TFT-LCD LED Control (for Wireless Tracker)

        // LoRa pins
        uint8_t loraNss;      // LoRa NSS/CS
        uint8_t loraDio1;     // LoRa DIO1
        uint8_t loraRst;      // LoRa Reset
        uint8_t loraBusy;     // LoRa Busy
        uint8_t loraMiso;     // LoRa MISO (SPI)
        uint8_t loraMosi;     // LoRa MOSI (SPI)
        uint8_t loraSck;      // LoRa SCK (SPI)

        // System pins
        uint8_t button;       // User button
        uint8_t vext;         // Vext power control
        uint8_t vbatRead;     // Battery voltage reading

        // Sensor pins
        uint8_t lightningIrq; // Lightning sensor interrupt
        uint8_t lightningCs;  // Lightning sensor chip select

        // Actuator pins
        uint8_t ledData;      // LED strip data
        uint8_t buzzer;       // Buzzer output

        // GPS pins (if available)
        uint8_t gpsTx;        // GPS UART TX
        uint8_t gpsRx;        // GPS UART RX
        uint8_t gpsRst;       // GPS Reset
        uint8_t gpsPps;       // GPS PPS signal
    };

    // Device-specific configurations
    namespace Devices {

        // Heltec WiFi LoRa 32 V3 (current device)
        namespace HeltecV3 {
            constexpr DeviceType TYPE = DeviceType::HELTEC_WIFI_LORA_32_V3;

            constexpr DeviceCapabilities CAPABILITIES = {
                .hasOLED = true,
                .hasTFT = false,
                .hasLoRa = true,
                .hasWiFi = true,
                .hasGPS = false,
                .hasBattery = true,
                .hasVext = true,
                .oledWidth = 128,
                .oledHeight = 64,
                .tftWidth = 0,
                .tftHeight = 0,
                .flashSize = 16 * 1024 * 1024,  // 16MB
                .sramSize = 512 * 1024          // 512KB
            };

                        constexpr PinConfig PINS = {
                // Display pins (order must match struct declaration)
                .oledSda = 4,       // OLED_SDA (GPIO4) - corrected from datasheet
                .oledScl = 15,      // OLED_SCL (GPIO15) - corrected from datasheet
                .oledRst = 21,      // OLED_RST (GPIO21)
                .tftSdin = 255,     // Not available
                .tftSclk = 255,     // Not available
                .tftRs = 255,       // Not available
                .tftRes = 255,      // Not available
                .tftCs = 255,       // Not available
                .tftLedK = 255,     // Not available

                // LoRa pins (SX1262)
                .loraNss = 8,       // LoRa_NSS (GPIO8)
                .loraDio1 = 14,     // LoRa_DIO1 (GPIO14)
                .loraRst = 12,      // LoRa_RST (GPIO12)
                .loraBusy = 13,     // LoRa_Busy (GPIO13)
                .loraMiso = 11,     // LoRa_MISO (GPIO11) - corrected from datasheet
                .loraMosi = 10,     // LoRa_MOSI (GPIO10) - corrected from datasheet
                .loraSck = 9,       // LoRa_SCK (GPIO9) - corrected from datasheet

                // System pins
                .button = 0,        // USER_SW (GPIO0)
                .vext = 36,         // Vext Ctrl (GPIO36)
                .vbatRead = 1,      // Vbat_Read (GPIO1) - corrected from datasheet

                // Sensor pins
                .lightningIrq = 4,  // Lightning IRQ (GPIO4)
                .lightningCs = 5,   // Lightning CS (GPIO5)
                .ledData = 2,       // LED Data (GPIO2)
                .buzzer = 3,        // Buzzer (GPIO3)

                // GPS pins (not available)
                .gpsTx = 255,       // Not available
                .gpsRx = 255,       // Not available
                .gpsRst = 255,      // Not available
                .gpsPps = 255       // Not available
            };
        };

        // Heltec Wireless Tracker V1.1 (new device)
        namespace WirelessTrackerV1_1 {
            constexpr DeviceType TYPE = DeviceType::HELTEC_WIRELESS_TRACKER_V1_1;

            constexpr DeviceCapabilities CAPABILITIES = {
                .hasOLED = false,  // Has TFT-LCD, not OLED
                .hasTFT = true,    // 0.96-inch 80x160 RGB TFT-LCD
                .hasLoRa = true,
                .hasWiFi = true,   // Wireless Tracker has WiFi (same board as V3)
                .hasGPS = true,     // Has built-in GPS (UC6580)
                .hasBattery = true,
                .hasVext = true,
                .oledWidth = 0,    // No OLED
                .oledHeight = 0,   // No OLED
                .tftWidth = 80,    // TFT display dimensions
                .tftHeight = 160,
                .flashSize = 8 * 1024 * 1024,   // 8MB
                .sramSize = 256 * 1024          // 256KB
            };

                                    constexpr PinConfig PINS = {
                // Display pins (order must match struct declaration)
                .oledSda = 255,    // Not available
                .oledScl = 255,    // Not available
                .oledRst = 255,    // Not available
                .tftSdin = 42,     // TFT_SDIN (GPIO42)
                .tftSclk = 41,     // TFT_SCLK (GPIO41)
                .tftRs = 40,       // TFT_RS (GPIO40)
                .tftRes = 39,      // TFT_RES (GPIO39)
                .tftCs = 38,       // TFT_CS (GPIO38)
                .tftLedK = 21,     // TFT_LED_K (GPIO21)

                // LoRa pins (SX1262)
                .loraNss = 8,      // LoRa_NSS (GPIO8)
                .loraDio1 = 14,    // LoRa_DIO1 (GPIO14)
                .loraRst = 12,     // LoRa_RST (GPIO12)
                .loraBusy = 13,    // LoRa_Busy (GPIO13)
                .loraMiso = 11,    // LoRa_MISO (GPIO11)
                .loraMosi = 10,    // LoRa_MOSI (GPIO10)
                .loraSck = 9,      // LoRa_SCK (GPIO9)

                // System pins
                .button = 0,       // USER_SW (GPIO0)
                .vext = 3,         // Vext Ctrl (GPIO3) - corrected from datasheet
                .vbatRead = 1,     // Vbat_Read (GPIO1)

                // Sensor pins
                .lightningIrq = 255, // Not available
                .lightningCs = 255,  // Not available
                .ledData = 255,      // Not available
                .buzzer = 255,       // Not available

                // GPS pins (UC6580)
                .gpsTx = 33,       // GNSS_TX (GPIO33)
                .gpsRx = 34,       // GNSS_RX (GPIO34)
                .gpsRst = 35,      // GNSS_RST (GPIO35)
                .gpsPps = 36       // GNSS_PPS (GPIO36)
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
