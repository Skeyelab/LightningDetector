# OLED Troubleshooting Guide - Heltec V3

## Current Issue: OLED Not Working

### Symptoms
- Code compiles successfully
- Serial output shows "Starting up..." then hangs
- No OLED display output
- Code stops at `Heltec.begin()` call

### Root Cause Analysis
The issue appears to be with the Heltec library compatibility with the current board/firmware version.

## Solutions to Try (In Order)

### Solution 1: Downgrade Heltec Library
```ini
# In platformio.ini, change:
lib_deps =
  jgromes/RadioLib @ ^6.5.0
  HelTecAutomation/Heltec_ESP32 @ ^2.0.0  # Try older version
```

### Solution 2: Manual U8G2 Setup (Fallback)
If Heltec library continues to cause issues, use this manual approach:

```cpp
// Remove: #include "heltec.h"
// Add: U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE, 18, 17);

bool oledInit() {
  Wire.begin(17, 18); // SDA=17, SCL=18
  Wire.setClock(400000);

  if (!oled.begin()) {
    Serial.println("OLED init failed!");
    return false;
  }

  Serial.println("OLED initialized successfully");
  return true;
}

void oledMsg(const String &l1, const String &l2="", const String &l3="") {
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tf);
  oled.drawStr(0, 12, l1.c_str());
  oled.drawStr(0, 28, l2.c_str());
  oled.drawStr(0, 44, l3.c_str());
  oled.sendBuffer();
}
```

### Solution 3: Check I2C Address
The OLED might be using a different I2C address:

```cpp
// Try different addresses
U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE, 18, 17); // Default 0x3C
// OR
U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE, 18, 17, U8X8_PIN_NONE, U8X8_PIN_NONE, U8X8_PIN_NONE, U8X8_PIN_NONE, U8X8_PIN_NONE, U8X8_PIN_NONE, 0x3D); // Try 0x3D
```

### Solution 4: I2C Scanner
Add this code to check what I2C devices are detected:

```cpp
void i2cScanner() {
  Serial.println("Scanning I2C devices...");
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.printf("I2C device found at address 0x%02X\n", addr);
    }
  }
}

// Call in setup() before OLED init
i2cScanner();
```

## PlatformIO Configuration Issues

### Check Board Definition
Ensure the correct board is selected:
```ini
[env:sender]
board = heltec_wifi_lora_32_V3  # Must match exactly
```

### Check Framework Version
```ini
[env]
platform = espressif32
framework = arduino
# Try specific version if issues persist:
# platform = espressif32 @ ^6.3.0
```

## Hardware Checks

### USB Cable
- Use high-quality data cable (not power-only)
- Try different USB ports
- Check if board shows up in device manager

### Power Supply
- Ensure stable 3.3V power
- Check for voltage drops during operation
- Verify USB power is sufficient

### Pin Connections
- Verify SDA=17, SCL=18 are correct for your board
- Check for pin conflicts with other peripherals
- Ensure no short circuits

## Debug Steps

### Step 1: Minimal Test
Create a minimal sketch to test just the OLED:

```cpp
#include <Wire.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE, 18, 17);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("OLED Test Starting...");

  Wire.begin(17, 18);
  if (oled.begin()) {
    Serial.println("OLED OK");
    oled.clearBuffer();
    oled.drawStr(0, 12, "OLED Working!");
    oled.sendBuffer();
  } else {
    Serial.println("OLED Failed");
  }
}

void loop() {
  delay(1000);
}
```

### Step 2: Check Serial Output
Monitor serial at 115200 baud to see where it fails.

### Step 3: Verify Board
Ensure you have the correct Heltec V3 board (not V2 or other variant).

## Common Error Codes

### RadioLib Errors
- `-1`: Invalid parameter
- `-2`: Invalid state
- `-3`: Timeout
- `-4`: Hardware error

### U8G2 Errors
- `false` from `oled.begin()`: I2C communication failed
- No display output: Check I2C address and pins

## Next Actions

1. **Try Solution 1** (downgrade Heltec library)
2. **If that fails**, implement Solution 2 (manual U8G2)
3. **Add I2C scanner** to debug I2C communication
4. **Test minimal OLED sketch** to isolate the issue

## Resources
- [Heltec V3 Documentation](https://docs.heltec.org/en/wifi_lora_32_v3/)
- [U8G2 Troubleshooting](https://github.com/olikraus/U8g2_Arduino/wiki/troubleshooting)
- [ESP32 I2C Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2c.html)
