# Talos LoRa Lightning Detector Prototype - Project Status

## Project Overview
**Goal**: Build a prototype for a LoRa-connected lightning detection system with two Heltec WiFi LoRa 32 V3 boards (ESP32-S3 + SX1262 + OLED)

## Current Implementation Status

### ✅ Working Components
- **PlatformIO Environment**: Properly configured with sender/receiver environments
- **LoRa Communication**: SX1262 radio working with RadioLib library
- **Serial Communication**: ESP32-S3 booting and serial output functional
- **Build System**: Compiles successfully for both roles

### ⚠️ Current Issues
- **OLED Display**: Not working - code hangs at `Heltec.begin()` call
- **Heltec Library**: May be incompatible with current board/firmware version
- **I2C Communication**: OLED initialization failing

### 🔧 Attempted Solutions
1. **Manual U8G2 Setup**: Initial approach with custom I2C configuration
2. **Heltec Library Integration**: Attempted to use official Heltec library for OLED
3. **Hybrid Approach**: Heltec for OLED only, RadioLib for LoRa

## Current Code State

### Main.cpp Status
- Uses Heltec library for OLED (`#include "heltec.h"`)
- RadioLib for SX1262 LoRa control
- OLED functions using `Heltec.display->` API
- Serial output working up to "Starting up..." then hangs

### PlatformIO Configuration
- **Dependencies**: RadioLib 6.5.0, Heltec_ESP32 2.1.4
- **Board**: heltec_wifi_lora_32_V3
- **Framework**: Arduino
- **Environments**: sender (default), receiver

## Hardware Configuration
- **Board**: Heltec WiFi LoRa 32 V3 (ESP32-S3 + SX1262)
- **OLED**: Built-in SSD1306 (I2C: SDA=17, SCL=18, addr=0x3C)
- **LoRa**: SX1262 with custom pin configuration
- **USB**: Working (serial communication functional)

## Next Steps Priority

### 1. **Immediate - Fix OLED Display** 🔴
- **Option A**: Debug Heltec library compatibility
- **Option B**: Fall back to manual U8G2 setup
- **Option C**: Try different Heltec library version

### 2. **Short Term - Complete Basic Functionality** 🟡
- Verify both sender and receiver roles work
- Test LoRa communication between boards
- Implement proper error handling and status display

### 3. **Medium Term - Core Features** 🟢
- Integrate AS3935 lightning sensor
- Add addressable LED ring control
- Implement lightning detection logic

### 4. **Long Term - Advanced Features** 🔵
- AES-CTR encryption
- Enhanced message protocol
- PCB design for production

## Technical Details

### LoRa Configuration (Working)
- Frequency: 915.0 MHz
- Bandwidth: 125 kHz
- Spreading Factor: 9
- Coding Rate: 4/5
- TX Power: 17 dBm

### OLED Configuration (Not Working)
- Library: Heltec_ESP32 2.1.4
- Driver: SSD1306
- Interface: I2C
- Pins: SDA=17, SCL=18

### Build Flags
```ini
-D LORA_FREQ_MHZ=915.0
-D LORA_BW_KHZ=125.0
-D LORA_SF=9
-D LORA_CR=5
-D LORA_TX_DBM=17
-D ROLE_SENDER=1  # or ROLE_RECEIVER=1
```

## Known Issues & Solutions

### Issue: OLED Not Working
**Symptoms**: Code hangs at `Heltec.begin()` call
**Possible Causes**:
- Heltec library version incompatibility
- I2C pin conflicts
- Board firmware version mismatch

**Solutions to Try**:
1. Downgrade Heltec library to version 2.0.x
2. Use manual U8G2 setup instead
3. Check board-specific I2C pin assignments

### Issue: Serial Output Limited
**Symptoms**: Only see boot messages, no runtime output
**Cause**: OLED initialization hanging
**Solution**: Fix OLED issue first

## Development Environment
- **IDE**: VS Code with PlatformIO extension
- **Platform**: PlatformIO
- **Framework**: Arduino
- **Board Package**: espressif32
- **Monitor Speed**: 115200 baud

## File Structure
```
talos-pio-heltec-v3-oled/
├── platformio.ini          # Build configuration
├── src/
│   └── main.cpp            # Main application code
├── .vscode/                # VS Code configuration
└── CURSOR_PROJECT_STATUS.md # This file
```

## Session Notes
**Last Session**: OLED initialization debugging
**Current Blocking Issue**: Heltec library compatibility
**Next Action**: Test alternative OLED initialization methods

## Useful Commands
```bash
# Build sender
pio run -e sender

# Build receiver
pio run -e receiver

# Monitor serial
pio device monitor --environment sender

# Clean build
pio run -t clean
```

## References
- [Heltec ESP32 Library](https://github.com/HelTecAutomation/Heltec_ESP32)
- [RadioLib Documentation](https://jgromes.github.io/RadioLib/)
- [U8G2 Library](https://github.com/olikraus/U8g2_Arduino)
- [Heltec V3 Board Specs](https://docs.heltec.org/en/wifi_lora_32_v3/)

---
*Last Updated: Current Session*
*Status: OLED Display Issue - Needs Resolution*
