#include "system_config.h"
#include "device_config.h"

namespace SystemConfig {
    namespace Pins {
        // Pin definitions - will be set dynamically
        uint8_t VEXT = 36;
        uint8_t OLED_RST = 21;
        uint8_t BUTTON = 0;
        uint8_t I2C_SDA = 17;
        uint8_t I2C_SCL = 18;
        uint8_t LORA_NSS = 8;
        uint8_t LORA_DIO1 = 14;
        uint8_t LORA_RST = 12;
        uint8_t LORA_BUSY = 13;
        uint8_t LIGHTNING_IRQ = 4;
        uint8_t LIGHTNING_CS = 5;
        uint8_t LED_DATA = 2;
        uint8_t BUZZER = 3;
        uint8_t GPS_TX = 255;  // Not available by default
        uint8_t GPS_RX = 255;  // Not available by default
        uint8_t GPS_PPS = 255; // Not available by default

        void initializePins() {
            // Detect device and set pins accordingly
            DeviceConfig::DeviceType device = DeviceConfig::DeviceManager::detectDevice();
            const DeviceConfig::PinConfig& pins = DeviceConfig::DeviceManager::getPinConfig(device);

            // Update all pin definitions based on detected device
            VEXT = pins.vext;
            OLED_RST = pins.oledRst;
            BUTTON = pins.button;
            I2C_SDA = pins.oledSda;
            I2C_SCL = pins.oledScl;
            LORA_NSS = pins.loraNss;
            LORA_DIO1 = pins.loraDio1;
            LORA_RST = pins.loraRst;
            LORA_BUSY = pins.loraBusy;
            LIGHTNING_IRQ = pins.lightningIrq;
            LIGHTNING_CS = pins.lightningCs;
            LED_DATA = pins.ledData;
            BUZZER = pins.buzzer;
            GPS_TX = pins.gpsTx;
            GPS_RX = pins.gpsRx;
            GPS_PPS = pins.gpsPps;
        }
    }
}
