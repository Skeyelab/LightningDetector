# Device Port Mapping

This document lists the USB port mappings for the connected devices on this development machine.

## Device Ports

| Device | Port | Environment | Description |
|--------|------|-------------|-------------|
| **Heltec V3** | `/dev/cu.usbserial-0001` | `heltec-v3`, `sender`, `receiver` | Main development device with OLED display |
| **Wireless Tracker** | `/dev/cu.usbmodem401101` | `wireless-tracker`, `wireless-tracker-sender`, `wireless-tracker-receiver` | New device with TFT-LCD and GPS |

## Quick Commands

### Build and Upload to Heltec V3
```bash
# Build and upload sender firmware
pio run --environment sender --target upload

# Build and upload receiver firmware
pio run --environment receiver --target upload

# Build only (check size)
pio run --environment heltec-v3 --target size
```

### Build and Upload to Wireless Tracker
```bash
# Build and upload sender firmware
pio run --environment wireless-tracker-sender --target upload

# Build and upload receiver firmware
pio run --environment wireless-tracker-receiver --target upload

# Build only (check size)
pio run --environment wireless-tracker --target size
```

### Monitor Serial Output
```bash
# Monitor Heltec V3
pio device monitor --port /dev/cu.usbserial-0001

# Monitor Wireless Tracker
pio device monitor --port /dev/cu.usbmodem401101
```

## Port Configuration

The upload ports are automatically configured in `platformio.ini`:

- **Heltec V3 environments**: `upload_port = /dev/cu.usbserial-0001`
- **Wireless Tracker environments**: `upload_port = /dev/cu.usbmodem401101`

## Notes

- Both devices use the same board type (`heltec_wifi_lora_32_V3`) but have different pin configurations
- The Wireless Tracker has TFT-LCD display and built-in GPS (UC6580)
- The Heltec V3 has OLED display and no GPS
- Both devices support WiFi and LoRa
- Upload speeds are set to 921600 baud for optimal performance
