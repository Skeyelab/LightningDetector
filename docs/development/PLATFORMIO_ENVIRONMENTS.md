# PlatformIO Environments Reference

This document provides a comprehensive overview of all available PlatformIO environments for building, testing, and deploying firmware.

## 🎯 **Quick Start Environments**

### **Default Environment**
- **`sender`** - Default environment for Heltec V3 sender firmware

### **Production Firmware**
```bash
# Heltec V3
pio run --environment sender      # Sender firmware
pio run --environment receiver    # Receiver firmware (with WiFi/OTA)

# Wireless Tracker
pio run --environment wireless-tracker-sender    # Sender firmware
pio run --environment wireless-tracker-receiver  # Receiver firmware (with WiFi/OTA/SPIFFS)
```

## 📱 **Device-Specific Base Environments**

### **Heltec WiFi LoRa 32 V3**
- **Board**: `heltec_wifi_lora_32_V3`
- **Display**: SSD1306 OLED (128x64)
- **Features**: WiFi, LoRa, Battery, Vext
- **Upload Port**: `/dev/cu.usbserial-0001`
- **Build Flags**: `HELTEC_V3_OLED=1`, `DEVICE_TYPE=HELTEC_V3`

### **Heltec Wireless Tracker V1.1**
- **Board**: `esp32-s3-devkitc-1`
- **Display**: TFT-LCD (80x160 RGB)
- **Features**: WiFi, LoRa, GPS (UC6580), Battery, Vext
- **Upload Port**: `/dev/cu.usbmodem401101`
- **Build Flags**: `WIRELESS_TRACKER=1`, `DEVICE_TYPE=WIRELESS_TRACKER`, `ARDUINO_USB_MODE=1`

## 🔧 **Firmware Role Environments**

### **Sender Firmware**
| Environment | Device | Description | WiFi | OTA |
|-------------|--------|-------------|------|-----|
| `sender` | Heltec V3 | Basic sender firmware | ❌ | ❌ |
| `wireless-tracker-sender` | Wireless Tracker | Sender with WiFi support | ✅ | ✅ |

### **Receiver Firmware**
| Environment | Device | Description | WiFi | OTA | SPIFFS |
|-------------|--------|-------------|------|-----|---------|
| `receiver` | Heltec V3 | Receiver with WiFi/OTA | ✅ | ✅ | ✅ |
| `wireless-tracker-receiver` | Wireless Tracker | Receiver with WiFi/OTA/SPIFFS | ✅ | ✅ | ✅ |

## 📊 **Code Quality Environments**

### **Static Analysis (Cppcheck)**
```bash
# Run static analysis
pio run --environment sender-check    # Heltec V3 sender
pio run --environment receiver-check  # Heltec V3 receiver
```

### **Code Quality (Clang-tidy)**
```bash
# Run code quality checks
pio run --environment sender-tidy     # Heltec V3 sender
pio run --environment receiver-tidy   # Heltec V3 receiver
```

## 🧪 **Testing Environments (Native)**

### **General Testing**
```bash
# Run all tests
pio test --environment native
```

### **Specific Test Categories**
```bash
# Hardware abstraction tests
pio test --environment native-hardware

# WiFi functionality tests
pio test --environment native-wifi

# Integration tests
pio test --environment native-integration
```

## 🚀 **Common Development Commands**

### **Build and Upload**
```bash
# Build for specific device
pio run --environment sender --target size
pio run --environment wireless-tracker-sender --target size

# Upload to device
pio run --environment sender --target upload
pio run --environment wireless-tracker-sender --target upload

# Monitor serial output
pio device monitor --port /dev/cu.usbserial-0001      # Heltec V3
pio device monitor --port /dev/cu.usbmodem401101      # Wireless Tracker
```

### **Clean and Rebuild**
```bash
# Clean build artifacts
pio run --environment sender --target clean

# Clean all environments
pio run --target clean
```

## 📋 **Environment Hierarchy**

```
env (base)
├── heltec-v3 (Heltec V3 base)
│   ├── sender (Heltec V3 sender)
│   │   ├── sender-check (static analysis)
│   │   └── sender-tidy (code quality)
│   └── receiver (Heltec V3 receiver)
│       ├── receiver-check (static analysis)
│       └── receiver-tidy (code quality)
└── wireless-tracker (Wireless Tracker base)
    ├── wireless-tracker-sender
    └── wireless-tracker-receiver
```

## ⚙️ **Build Flags Reference**

### **Device Type Flags**
- `HELTEC_V3_OLED=1` - Enable Heltec V3 OLED support
- `WIRELESS_TRACKER=1` - Enable Wireless Tracker support
- `DEVICE_TYPE=HELTEC_V3` - Set device type for Heltec V3
- `DEVICE_TYPE=WIRELESS_TRACKER` - Set device type for Wireless Tracker

### **Role Flags**
- `ROLE_SENDER=1` - Build sender firmware
- `ROLE_RECEIVER=1` - Build receiver firmware
- `ENABLE_WIFI_OTA=1` - Enable WiFi and OTA support

### **Wireless Tracker Specific**
- `ARDUINO_USB_MODE=1` - Enable USB mode
- `ARDUINO_USB_CDC_ON_BOOT=1` - Enable CDC on boot

## 🔍 **Troubleshooting**

### **Common Issues**
1. **Wrong Board**: Ensure you're using the correct board for each device
2. **Port Issues**: Check that the correct upload port is configured
3. **Build Failures**: Clean and rebuild if you encounter build issues

### **Environment Selection**
- **For Heltec V3**: Use `sender` or `receiver`
- **For Wireless Tracker**: Use `wireless-tracker-sender` or `wireless-tracker-receiver`
- **For Testing**: Use `native-*` environments
- **For Code Quality**: Use `*-check` or `*-tidy` environments

## 📚 **Related Documentation**

- [Device Port Mapping](DEVICE_PORTS.md) - USB port assignments
- [Heltec V3 Pin Mapping](HELTEC_V3_PIN_MAPPING.md) - Complete pin reference
- [Wireless Tracker Pin Mapping](WIRELESS_TRACKER_PIN_MAPPING.md) - Complete pin reference
- [Multi-Device Support](MULTI_DEVICE_SUPPORT.md) - Device differences and configuration
