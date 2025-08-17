#!/bin/bash

# Build script for PlatformIO Heltec ESP32 development container

set -e

echo "Building PlatformIO Heltec ESP32 development container..."

# Build the Docker image
docker build -t platformio-heltec-esp32 .

echo "Container built successfully!"

# Test the container
echo "Testing container..."
docker run --rm platformio-heltec-esp32 platformio --version

echo "Testing PlatformIO platforms..."
docker run --rm platformio-heltec-esp32 platformio platform list

echo "Testing PlatformIO libraries..."
docker run --rm platformio-heltec-esp32 platformio lib list

echo "Container test completed successfully!"
echo ""
echo "To use with dev container:"
echo "1. Open project in VS Code/Cursor"
echo "2. Press Ctrl+Shift+P and select 'Dev Containers: Reopen in Container'"
echo "3. Wait for container to build and start"
echo ""
echo "Background agent commands available:"
echo "- platformio run -e sender"
echo "- platformio run -e receiver"
echo "- platformio check -e sender-check -e receiver-check"
echo "- platformio check -e sender-tidy -e receiver-tidy"
echo "- platformio test -e native -e native-hardware-test -e native-hardware-isolated"
