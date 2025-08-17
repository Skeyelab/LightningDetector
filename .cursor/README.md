# PlatformIO Development Container for Heltec ESP32

This directory contains the Docker configuration for building and testing PlatformIO projects for Heltec ESP32 (WiFi LoRa 32 V3, ESP32-S3) boards inside Cursor IDE's background agents.

## Features

- **PlatformIO Core** installed via pipx
- **ESP32 Support** with espressif32 platform and Arduino framework
- **Pre-installed Libraries**: U8g2, RadioLib, ArduinoJson, WiFi, ArduinoOTA, Update
- **Static Analysis Tools**: clang-tidy, cppcheck
- **Caching Support** for toolchains and platforms across runs
- **Non-root User** with serial device access
- **Optimized for Cursor Background Agents** (non-interactive, slim)

## Quick Start

### 1. Build the Docker Image

```bash
cd .cursor
docker build -t platformio-heltec-esp32 .
```

### 2. Use with Dev Container

The `.devcontainer/devcontainer.json` is configured to:
- Mount persistent volumes for PlatformIO cache
- Configure VS Code extensions for PlatformIO development
- Set up proper user permissions

## Background Agent Commands

Cursor IDE can use these commands for automated building and testing:

### Build Commands
```bash
# Build sender firmware
platformio run -e sender

# Build receiver firmware
platformio run -e receiver
```

### Static Analysis Commands
```bash
# Run cppcheck analysis
platformio check -e sender-check -e receiver-check

# Run clang-tidy analysis
platformio check -e sender-tidy -e receiver-tidy
```

### Test Commands
```bash
# Run all native tests
platformio test -e native

# Run hardware-specific tests
platformio test -e native-hardware-test

# Run isolated hardware tests
platformio test -e native-hardware-isolated
```

### Combined Commands
```bash
# Full build and test pipeline
platformio run -e sender -e receiver && \
platformio check -e sender-check -e receiver-check -e sender-tidy -e receiver-tidy && \
platformio test -e native -e native-hardware-test -e native-hardware-isolated
```

## Environment Support

This container supports all your `platformio.ini` environments:

### ESP32 Environments
- **sender**: Heltec WiFi LoRa 32 V3 with sender role
- **receiver**: Heltec WiFi LoRa 32 V3 with receiver role and WiFi OTA

### Check Environments
- **sender-check**: cppcheck analysis for sender code
- **receiver-check**: cppcheck analysis for receiver code
- **sender-tidy**: clang-tidy analysis for sender code
- **receiver-tidy**: clang-tidy analysis for receiver code

### Native Test Environments
- **native**: C++17 unit tests with Arduino mocks
- **native-hardware-test**: Hardware abstraction tests
- **native-hardware-isolated**: Isolated hardware tests

## Caching Strategy

The container uses Docker volumes to persist:
- PlatformIO toolchains and platforms
- Installed libraries and dependencies
- Build cache and compiled objects

This ensures fast subsequent builds in Cursor's background agents.

## Serial Device Access

The container user has access to:
- `/dev/tty*` devices for ESP32 flashing
- USB devices for development boards
- Proper permissions for PlatformIO operations

## Troubleshooting

### Common Issues

1. **Permission Denied**: Ensure the container user has access to mounted volumes
2. **Cache Not Persisting**: Verify Docker volumes are properly mounted
3. **Library Installation Fails**: Check network connectivity and PlatformIO registry access

### Debug Commands

```bash
# Check PlatformIO installation
platformio --version

# List installed platforms
platformio platform list

# List installed libraries
platformio lib list

# Check environment configuration
platformio run --environment sender --dry-run
```

## Performance Optimization

- **Pre-warmed Cache**: ESP32 platform and Arduino framework pre-installed
- **Library Pre-installation**: Core libraries cached at build time
- **Volume Mounting**: Persistent cache across container restarts
- **Non-interactive Mode**: Optimized for automated CI/CD pipelines

## Security Notes

- Container runs as non-root user (`platformio`)
- Serial device access limited to necessary groups
- No unnecessary network services exposed
- Minimal attack surface for development environment
