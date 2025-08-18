#pragma once

#include <stdint.h>
#include <cstddef>
#include "device_config.h"

// System-wide configuration management
// Centralized configuration for all modules

namespace SystemConfig {
    // Hardware pin definitions - now dynamically configured based on device
    namespace Pins {
        // These will be set dynamically based on detected device
        extern uint8_t VEXT;        // Vext control: LOW = ON
        extern uint8_t OLED_RST;    // OLED reset pin
        extern uint8_t BUTTON;      // BOOT button on GPIO0 (active LOW)

        // Display pins
        extern uint8_t I2C_SDA;    // OLED I2C SDA
        extern uint8_t I2C_SCL;    // OLED I2C SCL
        extern uint8_t TFT_SDIN;   // TFT-LCD SDIN
        extern uint8_t TFT_SCLK;   // TFT-LCD SCLK
        extern uint8_t TFT_RS;     // TFT-LCD RS
        extern uint8_t TFT_RES;    // TFT-LCD RESET
        extern uint8_t TFT_CS;     // TFT-LCD CS
        extern uint8_t TFT_LED_K;  // TFT-LCD LED Control

        // LoRa radio pins
        extern uint8_t LORA_NSS;
        extern uint8_t LORA_DIO1;
        extern uint8_t LORA_RST;
        extern uint8_t LORA_BUSY;
        extern uint8_t LORA_MISO;
        extern uint8_t LORA_MOSI;
        extern uint8_t LORA_SCK;

        // Future sensor pins
        extern uint8_t LIGHTNING_IRQ;    // AS3935 interrupt pin
        extern uint8_t LIGHTNING_CS;     // AS3935 chip select

        // Future actuator pins
        extern uint8_t LED_DATA;         // WS2812 LED strip data
        extern uint8_t BUZZER;           // Buzzer/speaker output

        // GPS pins (if available)
        extern uint8_t GPS_TX;
        extern uint8_t GPS_RX;
        extern uint8_t GPS_RST;
        extern uint8_t GPS_PPS;

        // System pins
        extern uint8_t VBAT_READ;   // Battery voltage reading

        // Initialize pins based on detected device
        void initializePins();
    }

    // LoRa configuration
    namespace LoRa {
        constexpr float DEFAULT_FREQ_MHZ = 915.0f;
        constexpr float DEFAULT_BW_KHZ = 125.0f;
        constexpr uint8_t DEFAULT_SF = 9;
        constexpr uint8_t DEFAULT_CR = 5;
        constexpr int8_t DEFAULT_TX_DBM = 17;

        // Control channel for discovery/sync
        constexpr float CTRL_FREQ_MHZ = DEFAULT_FREQ_MHZ;
        constexpr float CTRL_BW_KHZ = 125.0f;
        constexpr uint8_t CTRL_SF = 9;
        constexpr uint8_t CTRL_CR = 5;

        // Valid ranges
        constexpr uint8_t SF_MIN = 7;
        constexpr uint8_t SF_MAX = 12;
        constexpr float BW_OPTIONS[] = {62.5f, 125.0f, 250.0f, 500.0f};
        constexpr size_t BW_OPTIONS_COUNT = sizeof(BW_OPTIONS) / sizeof(BW_OPTIONS[0]);
    }

    // Button timing configuration
    namespace Button {
        constexpr uint32_t DEBOUNCE_MS = 50;
        constexpr uint32_t SHORT_PRESS_MS = 100;
        constexpr uint32_t MEDIUM_PRESS_MS = 1000;
        constexpr uint32_t LONG_PRESS_MS = 3000;
    }

    // Display configuration
    namespace Display {
        constexpr uint8_t WIDTH = 128;
        constexpr uint8_t HEIGHT = 64;
        constexpr uint32_t UPDATE_INTERVAL_MS = 100;
    }

    // System timing
    namespace Timing {
        constexpr uint32_t MAIN_LOOP_INTERVAL_MS = 10;
        constexpr uint32_t STATUS_UPDATE_INTERVAL_MS = 1000;
        constexpr uint32_t PING_INTERVAL_MS = 2000;
    }

    // WiFi configuration
    namespace WiFi {
        constexpr uint32_t CONNECT_TIMEOUT_MS = 10000;
        constexpr uint32_t RETRY_DELAY_MS = 1000;
        constexpr uint8_t MAX_RETRIES = 3;
    }

    // OTA configuration
    namespace OTA {
        constexpr char HOSTNAME[] = "LtngDet-Receiver";
        constexpr char PASSWORD[] = "123456";
        constexpr size_t FIRMWARE_BUFFER_SIZE = 64 * 1024; // 64KB
        constexpr uint32_t CURRENT_VERSION = 0x010000; // Version 1.0.0
    }

    // Lightning sensor configuration
    namespace Lightning {
        constexpr uint8_t NOISE_FLOOR = 2;
        constexpr uint8_t WATCHDOG_THRESHOLD = 2;
        constexpr uint8_t SPIKE_REJECTION = 2;
        constexpr uint8_t MIN_STRIKES = 5;
    }

    // LED strip configuration
    namespace LEDs {
        constexpr uint16_t COUNT = 16;          // Number of LEDs in ring
        constexpr uint8_t BRIGHTNESS = 128;     // Default brightness (0-255)
        constexpr uint32_t ANIMATION_SPEED_MS = 50;
    }
}
