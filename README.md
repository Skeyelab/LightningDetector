# Lightning Detection System - PlatformIO Heltec V3

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/Skeyelab/LightningDetector)
[![Tests](https://img.shields.io/badge/tests-112%20passing-brightgreen)](./scripts/ci/run_tests.sh)
[![Platform](https://img.shields.io/badge/platform-ESP32--S3-blue)](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/)
[![Framework](https://img.shields.io/badge/framework-Arduino-red)](https://www.arduino.cc/)

A distributed lightning detection system built for Heltec WiFi LoRa 32 V3 boards with ESP32-S3, featuring modular architecture, comprehensive hardware abstraction, and GPS tracking capabilities.

## 🚀 Features

### Core Functionality
- **Lightning Detection** - AS3935 sensor integration with noise filtering
- **LoRa Communication** - Long-range data transmission using SX1262 radio
- **GPS Tracking** - UC6580 GNSS support for precise location data
- **OLED Display** - Real-time status and data visualization
- **WiFi Connectivity** - Multi-network support with automatic fallback
- **OTA Updates** - Over-the-air firmware updates via WiFi

### Hardware Abstraction Layer (HAL)
- **Modular Design** - Clean hardware abstraction for easy portability
- **Comprehensive API** - GPIO, I2C, SPI, PWM, ADC, Timer, Power, Memory management
- **Test Coverage** - 51 unit tests ensuring reliability
- **ESP32-S3 Optimized** - Tailored for Heltec WiFi LoRa 32 V3 hardware

### Advanced Features
- **Multi-Board Support** - Sender/receiver architecture
- **Power Management** - Sleep modes and battery monitoring
- **Error Handling** - Robust error detection and recovery
- **Debugging Tools** - Serial logging and diagnostic information

## 📋 Hardware Requirements

### Supported Boards
- **[Heltec WiFi LoRa 32 V3](https://heltec.org/project/wifi-lora-32-v3/)** - Main development board
- **[Heltec Wireless Tracker](https://heltec.org/project/wireless-tracker/)** - GPS-enabled transmitter (upcoming)

### Key Specifications
- **MCU**: ESP32-S3 (240MHz, dual-core, 8MB Flash, 512KB SRAM)
- **Radio**: SX1262 LoRa transceiver (915MHz)
- **Display**: 128x64 OLED (SSD1306)
- **GPS**: UC6580 GNSS (L1+L5/L2 dual frequency)
- **Connectivity**: WiFi 802.11 b/g/n

### Sensors & Peripherals
- AS3935 Lightning sensor
- Battery voltage monitoring
- External GPIO expansion
- I2C/SPI device support

## 🛠️ Quick Start

### Prerequisites
```bash
# Install PlatformIO Core
pip install platformio

# Clone the repository
git clone https://github.com/Skeyelab/LightningDetector.git
cd LightningDetector
```

### Build Options

#### Unified Firmware  (single binary for Sender **or** Receiver)
```bash
# Build
pio run -e unified

# Upload (USB serial bootloader)
pio run -e unified -t upload

# Monitor serial output
pio device monitor
```

After flashing, the board boots as **Sender** by default.  Change the role at runtime:

*Web UI*
1. Connect to the device’s IP or `esp32.local`
2. Scroll to **Device Role** → click **Toggle Role** (sender ↔ receiver)
3. Reboot to apply

*HTTP API*
```bash
curl -X POST http://<ip>/api/v1/config \
     -H 'Content-Type: application/json' \
     -d '{"role":"receiver"}'
```

The choice is stored in NVS, so future boots keep the selected role.

## 🌐 Web Flasher

### Browser-Based Firmware Flashing
The project includes a web-based ESP32 firmware flasher that allows you to flash devices directly from your browser without installing additional software.

#### Features
- **🌐 Web Interface**: Flash ESP32 devices from any modern browser
- **🌟 Unified Firmware**: Single firmware supports both TX and RX modes
- **📱 Legacy Support**: Transmitter and receiver firmware flashing (backward compatibility)
- **📁 File Upload**: Support for custom firmware files
- **🔌 Serial Port**: Direct USB connection to ESP32 devices
- **📊 Progress Tracking**: Real-time flashing progress and status updates

#### Quick Start
1. **Visit**: [Web Flasher](https://skeyelab.github.io/LightningDetector/)
2. **Connect**: Plug your ESP32 device via USB
3. **Select**: Choose **Unified Firmware** (recommended) or legacy transmitter/receiver
4. **Flash**: Click "Connect & Flash" and follow the prompts
5. **Configure**: After flashing unified firmware, use web interface to set device role

#### Requirements
- **Browser**: Chrome or Edge (Web Serial API support)
- **Hardware**: ESP32 device in download mode
- **USB Cable**: Data cable (not just charging cable)

#### Manual Firmware Upload
- Upload custom `.bin` files for unified, transmitter, or receiver firmware
- Support for both pre-built and custom firmware
- Automatic file validation and size checking
- **Recommended**: Use unified firmware for maximum flexibility

### Configuration

#### WiFi Networks
```cpp
// Copy and modify wifi_networks_example.h
cp wifi_networks_example.h src/wifi_networks.h
// Edit src/wifi_networks.h with your network credentials
```

#### Hardware Pins
```cpp
// Heltec V3 Pin Configuration (src/config/system_config.h)
#define VEXT_PIN 36        // Power control
#define OLED_RST_PIN 21    // OLED reset
#define BUTTON_PIN 0       // User button (GPIO0)
#define LORA_NSS 8         // LoRa chip select
#define LORA_DIO1 14       // LoRa interrupt
```

#### Button Controls
**Button Actions (GPIO0):**
- **Short/Medium Press (100ms-6s)**: Cycle through LoRa presets
  - Cycles through 8 predefined LoRa configurations (bandwidth, spreading factor)
  - Both Sender and Receiver modes support preset cycling
  - Presets are automatically synchronized between devices
- **Long Press (>6s)**: Enter deep sleep mode
  - Device enters low-power sleep state
  - Wake up via button press or timer

**Role Configuration:**
- **NOT via button** - Device role (Sender/Receiver) is configured via web interface
- Access web interface at device IP or `esp32.local`
- Navigate to **Device Mode** section to change role
- Role changes require reboot and are stored persistently

## 📁 Repository Structure

The repository is organized for easy navigation and maintenance:

### Scripts (`scripts/`)
- **`ci/`** - Continuous Integration scripts
  - `run_tests.sh` - Comprehensive test suite
  - `run_static_analysis.sh` - Code analysis tools
  - `run_tidy.sh` - Clang-tidy formatting
- **`dev/`** - Development and deployment scripts
  - `flash_unified.sh` - Flash unified firmware to single device
  - `flash_both.sh` - Flash unified firmware to both devices
  - `create_release.sh` - Create release packages
- **`optimize/`** - Performance optimization tools
  - `optimize_clang_tidy_cache.sh` - Build cache optimization

### Documentation (`docs/`)
- **`development/`** - Development guides
  - `QUICK_START_RELEASE.md` - Release quick start
  - `RELEASE_README.md` - Release procedures
  - `TROUBLESHOOTING.md` - Common issues and solutions
- **`deployment/`** - Deployment documentation
  - `OTA_README.md` - Over-the-air update guide
  - `WEB_FLASHER_DEPLOYMENT.md` - Web flasher setup
  - `WIFI_MULTI_NETWORK_README.md` - Multi-network WiFi config
- **`testing/`** - Testing documentation
  - `NEW_TESTS_NEEDED.md` - Test coverage gaps
  - `STATIC_ANALYSIS.md` - Static analysis setup
- **`project/`** - Project management
  - `CURSOR_PROJECT_STATUS.md` - Current project status
  - `CI_CACHING_OPTIMIZATION.md` - CI performance notes

## 🧪 Testing

### Run All Tests
```bash
# Execute comprehensive test suite
./scripts/ci/run_tests.sh
```

### Individual Test Suites
```bash
# Hardware Abstraction Layer tests
pio test -e native -f test_hardware_abstraction

# Integration tests
pio test -e native -f test_integration

# WiFi functionality tests
pio test -e native -f test_wifi_manager
```

### Static Analysis
```bash
# Code quality checks
./run_static_analysis.sh

# Clang-tidy analysis
./run_tidy.sh
```

## 📁 Project Structure

```
├── src/
│   ├── hardware/              # Hardware Abstraction Layer
│   │   ├── hardware_abstraction.h
│   │   └── hardware_abstraction.cpp
│   ├── sensors/               # Sensor implementations
│   │   ├── gps_sensor.h/.cpp
│   │   ├── lightning_sensor.h
│   │   └── sensor_interface.h
│   ├── communication/         # Communication protocols
│   ├── system/               # System management
│   ├── config/               # Configuration files
│   └── main.cpp              # Application entry point
├── test/                     # Unit tests
├── docs/                     # Additional documentation
└── platformio.ini           # PlatformIO configuration
```

## 🔧 Hardware Abstraction Layer

### GPIO Operations
```cpp
#include "hardware/hardware_abstraction.h"
using namespace HardwareAbstraction;

// Initialize HAL
initialize();

// Configure GPIO
GPIO::pinMode(2, GPIO::Mode::MODE_OUTPUT);
GPIO::digitalWrite(2, GPIO::Level::LEVEL_HIGH);
```

### I2C Communication
```cpp
// Initialize I2C for OLED
I2C::initialize(21, 22, 400000); // SDA, SCL, frequency

// Write to device
uint8_t data[] = {0x00, 0xFF};
I2C::beginTransmission(0x3C);
I2C::write(data, sizeof(data));
I2C::endTransmission();
```

### GPS Integration
```cpp
#include "sensors/gps_sensor.h"

// Initialize GPS for Wireless Tracker
GPS::Config config = GPS::getWirelessTrackerV11Config();
GPS::initializeGPS(config);

// Read GPS data
if (GPS::hasGPSFix()) {
    const GPS::Data& data = GPS::getGPSData();
    printf("Position: %.6f, %.6f\n", data.latitude, data.longitude);
}
```

## 📡 Communication Protocol

### LoRa Configuration & Presets
The system includes 8 predefined LoRa configurations that can be cycled via button press:

**Available Presets:**
- **Long Range - Fast**: 125kHz, SF10 (best range, moderate speed)
- **Long Range - Slow**: 125kHz, SF12 (maximum range, slowest speed)  
- **Long Range - Moderate**: 125kHz, SF11 (good range, balanced speed)
- **Medium Range - Slow**: 125kHz, SF10 (moderate range, good speed)
- **Medium Range - Fast**: 250kHz, SF9 (shorter range, faster speed)
- **Short Range - Slow**: 125kHz, SF8 (short range, fast speed)
- **Short Range - Fast**: 250kHz, SF7 (very short range, very fast)
- **Short Range - Turbo**: 500kHz, SF7 (shortest range, maximum speed)

**Preset Synchronization:**
- Receivers automatically broadcast preset changes to all devices
- All devices on the network automatically sync to the same preset
- Presets are stored persistently and survive reboots

### LoRa Messaging
```cpp
// Message format
struct LightningMessage {
    uint32_t timestamp;
    float latitude;
    float longitude;
    uint16_t strike_count;
    uint8_t noise_level;
    uint16_t battery_mv;
};
```

### WiFi Multi-Network
- Automatic network selection
- Connection priority management
- Fallback mechanisms
- OTA update support

## 🔋 Power Management

### Sleep Modes
```cpp
// Deep sleep for battery conservation
Power::sleep(Power::Mode::DEEP_SLEEP, 300000); // 5 minutes

// Wake on button press or timer
```

### Battery Monitoring
```cpp
// Check battery status
float voltage = Power::getBatteryVoltage();
uint8_t percent = Power::getBatteryPercent();
```

## 📊 Monitoring & Debugging

### Serial Logging
```cpp
// Debug output levels
#define LOG_DEBUG   0
#define LOG_INFO    1
#define LOG_WARN    2
#define LOG_ERROR   3
```

### Performance Metrics
- Memory usage tracking
- LoRa transmission statistics
- GPS fix quality monitoring
- Power consumption analysis

## 🤝 Contributing

### Development Workflow
1. **Fork** the repository
2. **Create** feature branch (`git checkout -b feature/new-sensor`)
3. **Implement** changes with tests
4. **Run** test suite (`./scripts/ci/run_tests.sh`)
5. **Submit** pull request

### Code Standards
- Follow existing code style
- Add unit tests for new features
- Update documentation
- Pass static analysis checks

### Issue Tracking
- [Hardware Abstraction Layer](https://github.com/Skeyelab/LightningDetector/issues/7) ✅ **COMPLETE**
- [Modular Architecture](https://github.com/Skeyelab/LightningDetector/issues/14) 🚧 **IN PROGRESS**

## 📚 Documentation

- [WiFi Multi-Network Setup](./WIFI_MULTI_NETWORK_README.md)
- [OTA Update Guide](./OTA_README.md)
- [Troubleshooting Guide](./TROUBLESHOOTING.md)
- [Static Analysis Results](./STATIC_ANALYSIS.md)
- [CI Caching Optimization](./CI_CACHING_OPTIMIZATION.md)
- [Project Status](./CURSOR_PROJECT_STATUS.md)

## 🎯 Roadmap

### Phase 1: Foundation ✅
- [x] Hardware Abstraction Layer
- [x] GPS Integration (UC6580)
- [x] Comprehensive Testing
- [x] Build System Optimization
- [x] Unified Firmware Architecture
- [x] Web Interface & Role Management
- [x] Web Server Performance Optimization

### Phase 2: Lightning Detection 🚧
- [ ] AS3935 Sensor Integration
- [ ] Noise Filtering Algorithms
- [ ] Calibration Procedures
- [ ] Real-time Processing

### Phase 3: Communication 🔮
- [ ] LoRa Network Protocol
- [ ] Message Encryption
- [ ] Multi-hop Routing
- [ ] Data Aggregation

### Phase 4: Analytics 🔮
- [ ] Storm Tracking
- [ ] Prediction Models
- [ ] Web Dashboard
- [ ] Mobile App

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **Heltec Automation** - Hardware platform and documentation
- **Espressif Systems** - ESP32-S3 microcontroller and ESP-IDF
- **RadioLib** - LoRa communication library
- **U8g2** - OLED display library
- **PlatformIO** - Development platform and toolchain

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/Skeyelab/LightningDetector/issues)
- **Discussions**: [GitHub Discussions](https://github.com/Skeyelab/LightningDetector/discussions)
- **Documentation**: [Project Wiki](https://github.com/Skeyelab/LightningDetector/wiki)

---

**Built with ❤️ for the lightning detection community**
