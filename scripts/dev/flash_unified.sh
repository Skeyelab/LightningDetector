#!/bin/bash

# Flash Unified Firmware Script
# This script builds and uploads the unified firmware to the Heltec V3 device

set -e  # Exit on any error

echo "🔧 Building unified firmware..."
pio run -e unified

echo "📱 Uploading firmware to device..."
pio run -e unified --target upload

echo "📊 Starting serial monitor..."
echo "Press Ctrl+C to stop monitoring"
echo ""
echo "🔋 Look for battery pin detection messages:"
echo "   [BATTERY] Starting battery pin detection..."
echo "   [BATTERY] Testing GPIO1 for battery voltage..."
echo "   [BATTERY] SUCCESS: Found battery voltage on GPIOX: X.XXXV"
echo ""

pio device monitor
