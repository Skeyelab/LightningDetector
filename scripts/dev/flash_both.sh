#!/bin/bash

# Flash both Lightning Detector devices with unified firmware
# Unified firmware supports both Sender & Receiver roles at runtime
# Device 1 on /dev/cu.usbserial-6 (or first available)
# Device 2 on /dev/cu.usbserial-0001 (or second available)

echo "==========================================="
echo "Flashing Lightning Detector Devices with Unified Firmware"
echo "==========================================="

# Function to find available USB serial devices
find_available_ports() {
    local ports=($(ls /dev/cu.usbserial* 2>/dev/null | sort))
    if [ ${#ports[@]} -eq 0 ]; then
        echo "❌ No USB serial devices found"
        exit 1
    elif [ ${#ports[@]} -eq 1 ]; then
        echo "⚠️  Only one device found: ${ports[0]}"
        echo "   Consider using flash_unified.sh for single device"
        echo ""
    fi
    echo "Found ${#ports[@]} device(s):"
    for i in "${!ports[@]}"; do
        echo "  Device $((i+1)): ${ports[$i]}"
    done
    echo ""
}

# Find available ports
find_available_ports

# Use first two available ports or specified ports
DEVICE1_PORT="${1:-/dev/cu.usbserial-7}"
DEVICE2_PORT="${2:-/dev/cu.usbserial-0001}"

echo "📡 Flashing DEVICE 1 (unified firmware) to $DEVICE1_PORT..."
echo "--------------------------------------------------------"
echo "This device will run unified firmware with Sender/Receiver role switching"
echo ""

pio run -e unified --target upload --upload-port "$DEVICE1_PORT"

if [ $? -eq 0 ]; then
    echo "✅ Device 1 unified firmware flash completed successfully!"
else
    echo "❌ Device 1 unified firmware flash failed!"
    exit 1
fi

echo ""
echo "📱 Flashing DEVICE 2 (unified firmware) to $DEVICE2_PORT..."
echo "--------------------------------------------------------"
echo "This device will also run unified firmware with Sender/Receiver role switching"
echo ""

pio run -e unified --target upload --upload-port "$DEVICE2_PORT"

if [ $? -eq 0 ]; then
    echo "✅ Device 2 unified firmware flash completed successfully!"
else
    echo "❌ Device 2 unified firmware flash failed!"
    exit 1
fi

echo ""
echo "📁 Uploading web interface files to SPIFFS on Device 1..."
echo "--------------------------------------------------------"
pio run -e unified --target uploadfs --upload-port "$DEVICE1_PORT"

if [ $? -eq 0 ]; then
    echo "✅ Device 1 SPIFFS upload completed successfully!"
else
    echo "❌ Device 1 SPIFFS upload failed!"
    exit 1
fi

echo ""
echo "📁 Uploading web interface files to SPIFFS on Device 2..."
echo "--------------------------------------------------------"
pio run -e unified --target uploadfs --upload-port "$DEVICE2_PORT"

if [ $? -eq 0 ]; then
    echo "✅ Device 2 SPIFFS upload completed successfully!"
else
    echo "❌ Device 2 SPIFFS upload failed!"
    exit 1
fi

echo ""
echo "🎉 Both devices flashed with unified firmware successfully!"
echo "==========================================="
echo ""
echo "Both devices now run unified firmware with:"
echo "- Runtime role switching (Sender/Receiver)"
echo "- WiFi and OTA update support"
echo "- Web interface accessible via WiFi"
echo "- LoRa communication capabilities"
echo ""
echo "To configure devices:"
echo "- Short/Medium button press (100ms-6s): Cycle through LoRa presets"
echo "- Long button press (>6s): Enter deep sleep mode"
echo "- Role switching (Sender/Receiver): Use web interface at device IP"
echo ""
echo "Recommended setup:"
echo "- Device 1: Set to Sender role (transmit lightning data)"
echo "- Device 2: Set to Receiver role (receive and display data)"
echo ""
echo "Web interfaces will be available at each device's IP address"
echo "once connected to WiFi (check serial monitor for IPs)"
echo ""
echo "Usage: $0 [device1_port] [device2_port]"
echo "Example: $0 /dev/cu.usbserial-6 /dev/cu.usbserial-0001"
echo ""
