#!/bin/bash

# Flash unified Lightning Detector firmware and SPIFFS data
# Unified firmware supports both Sender & Receiver roles at runtime

echo "==========================================="
echo "Flashing Unified Lightning Detector Firmware"
echo "==========================================="

# Check if device port is provided as argument
if [ $# -eq 0 ]; then
    echo "❌ Error: Please provide the device port (e.g., /dev/cu.usbserial-XXXX)"
    echo "Usage: $0 <device_port>"
    echo ""
    echo "Example: $0 /dev/cu.usbserial-0001"
    echo ""
    echo "Available ports (if any):"
    ls /dev/cu.usbserial* 2>/dev/null || echo "No USB serial devices found"
    exit 1
fi

DEVICE_PORT="$1"

echo ""
echo "🔧 Flashing UNIFIED firmware to $DEVICE_PORT..."
echo "-----------------------------------------------"
echo "This firmware supports both Sender & Receiver roles at runtime"
echo ""

pio run -e unified --target upload --upload-port "$DEVICE_PORT"

if [ $? -eq 0 ]; then
    echo "✅ Unified firmware flash completed successfully!"
else
    echo "❌ Unified firmware flash failed!"
    exit 1
fi

echo ""
echo "📁 Uploading web interface files to SPIFFS..."
echo "---------------------------------------------"
echo "Uploading: index.html, CSS, JavaScript, and test files"
echo ""

pio run -e unified --target uploadfs --upload-port "$DEVICE_PORT"

if [ $? -eq 0 ]; then
    echo "✅ SPIFFS upload completed successfully!"
else
    echo "❌ SPIFFS upload failed!"
    exit 1
fi

echo ""
echo "🎉 Unified firmware and SPIFFS data flashed successfully!"
echo "==========================================="
echo ""
echo "Device is now running unified firmware with:"
echo "- Runtime role switching (Sender/Receiver)"
echo "- WiFi and OTA update support"
echo "- Web interface accessible via WiFi"
echo "- LoRa communication capabilities"
echo ""
echo "To configure device:"
echo "- Short/Medium button press (100ms-6s): Cycle through LoRa presets"
echo "- Long button press (>6s): Enter deep sleep mode"
echo "- Role switching (Sender/Receiver): Use web interface at device IP"
echo ""
echo "Web interface will be available at the device's IP address"
echo "once connected to WiFi (check serial monitor for IP)"
echo ""
