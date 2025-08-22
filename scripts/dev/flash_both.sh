#!/bin/bash

# Flash both Lightning Detector devices
# Sender on /dev/cu.usbserial-6
# Receiver on /dev/cu.usbserial-0001

echo "==========================================="
echo "Flashing Lightning Detector Devices"
echo "==========================================="

echo ""
echo "📡 Flashing SENDER to /dev/cu.usbserial-6..."
echo "-------------------------------------------"
pio run -e sender --target upload --upload-port /dev/cu.usbserial-7

if [ $? -eq 0 ]; then
    echo "✅ Sender flash completed successfully!"
else
    echo "❌ Sender flash failed!"
    exit 1
fi

echo ""
echo "📱 Flashing RECEIVER to /dev/cu.usbserial-0001..."
echo "-----------------------------------------------"
pio run -e receiver --target upload --upload-port /dev/cu.usbserial-0001

if [ $? -eq 0 ]; then
    echo "✅ Receiver flash completed successfully!"
else
    echo "❌ Receiver flash failed!"
    exit 1
fi

echo ""
echo "📁 Uploading web interface files to SPIFFS..."
echo "---------------------------------------------"
pio run -e receiver --target uploadfs --upload-port /dev/cu.usbserial-0001

if [ $? -eq 0 ]; then
    echo "✅ SPIFFS upload completed successfully!"
else
    echo "❌ SPIFFS upload failed!"
    exit 1
fi

echo ""
echo "🎉 Both devices flashed successfully!"
echo "==========================================="
echo ""
echo "You should now see:"
echo "- Sender: Blinking dot every 2 seconds (1s on, 1s off)"
echo "- Receiver: Blinking dot when pings received (1s duration)"
echo "- Serial console: Full ping logging preserved"
echo ""
