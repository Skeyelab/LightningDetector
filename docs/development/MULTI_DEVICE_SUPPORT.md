# Multi-Device Support

This project now supports multiple Heltec devices with automatic hardware detection and configuration.

## Supported Devices

### 1. Heltec WiFi LoRa 32 V3 (Current Device)
- **Board**: `heltec_wifi_lora_32_V3`
- **Features**: WiFi, LoRa, OLED, Battery, Vext power rail
- **Flash**: 16MB
- **SRAM**: 512KB
- **Environment**: `heltec-v3`, `sender`, `receiver`

### 2. Heltec Wireless Tracker V1.1 (New Device)
- **Board**: `heltec_wifi_lora_32_V3` (same as V3)
- **Features**: WiFi, LoRa, TFT-LCD, GPS, Battery, Vext power rail
- **Display**: 0.96-inch 80x160 RGB TFT-LCD (not OLED)
- **GPS**: Built-in UC6580 multi-system GNSS (GPS, GLONASS, BDS, Galileo, NAVIC, QZSS)
- **Flash**: 8MB
- **SRAM**: 256KB
- **Environment**: `wireless-tracker`, `wireless-tracker-sender`, `wireless-tracker-receiver`

## Hardware Differences

| Feature | Heltec V3 | Wireless Tracker |
|---------|-----------|------------------|
| WiFi | ✅ Yes | ✅ Yes |
| LoRa | ✅ Yes | ✅ Yes |
| Display | ✅ OLED (128x64) | ✅ TFT-LCD (80x160) |
| GPS | ❌ No | ✅ Yes (UC6580 multi-system) |
| Battery | ✅ Yes | ✅ Yes |
| Vext | ✅ Yes | ✅ Yes |
| Flash | 16MB | 8MB |
| SRAM | 512KB | 256KB |

## Pin Configurations

### Heltec V3
```cpp
OLED_SDA = 17, OLED_SCL = 18, OLED_RST = 21
LORA_NSS = 8, LORA_DIO1 = 14, LORA_RST = 12, LORA_BUSY = 13
BUTTON = 0, VEXT = 36
```

### Wireless Tracker V1.1
```cpp
// TFT-LCD Display (0.96-inch 80x160 RGB)
TFT_SDIN = 42, TFT_SCLK = 41, TFT_RS = 40, TFT_RES = 39, TFT_CS = 38, TFT_LED_K = 21

// LoRa Radio (SX1262)
LORA_NSS = 8, LORA_DIO1 = 14, LORA_RST = 12, LORA_BUSY = 13
LORA_MISO = 11, LORA_MOSI = 10, LORA_SCK = 9

// System
BUTTON = 0, VEXT_CTRL = 3, VBAT_READ = 1

// GPS (UC6580 multi-system GNSS)
GPS_TX = 33, GPS_RX = 34, GPS_RST = 35, GPS_PPS = 36
```

## Building for Different Devices

### Heltec V3
```bash
# Sender firmware
pio run --environment sender

# Receiver firmware
pio run --environment receiver
```

### Wireless Tracker
```bash
# Sender firmware
pio run --environment wireless-tracker-sender

# Receiver firmware
pio run --environment wireless-tracker-receiver
```

## Device Detection

The system automatically detects the device type at runtime using:

1. **Hardware Detection**: Flash size, chip capabilities
2. **Compile-time Flags**: `HELTEC_V3_OLED` or `WIRELESS_TRACKER`
3. **Fallback**: Default to Heltec V3 configuration

```cpp
#include "config/device_config.h"

// Detect device automatically
DeviceConfig::DeviceType device = DeviceConfig::DeviceManager::detectDevice();
const char* deviceName = DeviceConfig::DeviceManager::getDeviceName(device);

// Check capabilities
if (DeviceConfig::DeviceManager::getCurrentCapabilities().hasWiFi) {
    // Initialize WiFi
}

if (DeviceConfig::DeviceManager::getCurrentCapabilities().hasGPS) {
    // Initialize GPS
}
```

## Dynamic Pin Configuration

Pins are configured dynamically based on the detected device:

```cpp
#include "config/system_config.h"

// Initialize pins for detected device
SystemConfig::Pins::initializePins();

// Use dynamic pin definitions
uint8_t oledSda = SystemConfig::Pins::I2C_SDA;
uint8_t oledScl = SystemConfig::Pins::I2C_SCL;
uint8_t loraNss = SystemConfig::Pins::LORA_NSS;
```

## Conditional Compilation

Use feature flags for device-specific code:

```cpp
#include "config/device_features.h"

// Device-specific code
IF_HAS_WIFI({
    // WiFi initialization code
});

IF_HAS_GPS({
    // GPS initialization code
});

// Or check at runtime
if (DeviceFeatures::hasWiFi()) {
    // WiFi code
}
```

## Adding New Devices

To add support for a new device:

1. **Add device type** in `src/config/device_config.h`:
```cpp
enum class DeviceType {
    HELTEC_WIFI_LORA_32_V3,
    HELTEC_WIRELESS_TRACKER_V1_1,
    NEW_DEVICE_TYPE,  // Add here
    UNKNOWN
};
```

2. **Add device configuration**:
```cpp
namespace NewDevice {
    constexpr DeviceCapabilities CAPABILITIES = {
        .hasOLED = false,  // Set based on actual hardware
        .hasTFT = true,    // Set based on actual hardware
        .hasLoRa = true,
        .hasWiFi = true,   // Set based on actual hardware
        .hasGPS = true,
        // ... other capabilities
    };

    constexpr PinConfig PINS = {
        .oledSda = 255,    // Not available
        .oledScl = 255,    // Not available
        .tftSdin = 42,     // TFT-LCD SDIN
        .tftSclk = 41,     // TFT-LCD SCLK
        // ... other pins
    };
}
```

3. **Add detection logic** in `detectByHardware()`:
```cpp
if (/* new device detection criteria */) {
    return DeviceType::NEW_DEVICE_TYPE;
}
```

4. **Add PlatformIO environment**:
```ini
[env:new-device]
board = new_device_board
extends = env
build_flags =
    ${env.build_flags}
    -D NEW_DEVICE=1
```

## Testing Multi-Device Support

### Local Testing
```bash
# Test Heltec V3 builds
pio run --environment sender
pio run --environment receiver

# Test Wireless Tracker builds
pio run --environment wireless-tracker-sender
pio run --environment wireless-tracker-receiver
```

### Unit Tests
```bash
# Run comprehensive tests
./scripts/ci/run_comprehensive_tests.sh

# Test specific components
pio test --environment native-hardware-comprehensive
```

## Troubleshooting

### Common Issues

1. **Pin Mismatch**: Ensure pins are correctly configured for your device
2. **Feature Not Available**: Check device capabilities before using features
3. **Build Errors**: Verify the correct environment is selected

### Debug Information

Enable debug output to see device detection:
```cpp
Serial.printf("[SETUP] Detected device: %s\n", deviceName);
Serial.printf("[SETUP] Has WiFi: %s\n", hasWiFi ? "YES" : "NO");
Serial.printf("[SETUP] Has GPS: %s\n", hasGPS ? "YES" : "NO");
```

## Future Enhancements

- **Auto-detection**: Improve hardware detection accuracy
- **More devices**: Add support for additional Heltec boards
- **Dynamic features**: Runtime feature enable/disable based on hardware
- **Configuration UI**: Web interface for device-specific settings
