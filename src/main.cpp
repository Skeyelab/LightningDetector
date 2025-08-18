#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <RadioLib.h>
#include <U8g2lib.h>
#include <Preferences.h>
#include <esp_sleep.h>
#include <driver/rtc_io.h>
#include "app_logic.h"
#include "config/device_config.h"
#include "config/system_config.h"

#ifdef ENABLE_WIFI_OTA
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <Update.h>
#endif
// Allow sender to flash firmware received over LoRa
#if defined(ROLE_SENDER) && !defined(ENABLE_WIFI_OTA)
#include <Update.h>
#endif

// Vext power control and OLED reset (dynamic based on device)
#define VEXT_PIN SystemConfig::Pins::VEXT
#define OLED_RST_PIN SystemConfig::Pins::OLED_RST
#define BUTTON_PIN SystemConfig::Pins::BUTTON
#define ALT_BUTTON_PIN 38  // Alternative button pin for testing (device-specific)

// SSD1306 128x64 OLED over HW I2C; pins set dynamically
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

#ifndef PIN_LORA_NSS
  #define PIN_LORA_NSS   SystemConfig::Pins::LORA_NSS
#endif
#ifndef PIN_LORA_DIO1
  #define PIN_LORA_DIO1  SystemConfig::Pins::LORA_DIO1
#endif
#ifndef PIN_LORA_RST
  #define PIN_LORA_RST   SystemConfig::Pins::LORA_RST
#endif
#ifndef PIN_LORA_BUSY
  #define PIN_LORA_BUSY  SystemConfig::Pins::LORA_BUSY
#endif

#ifndef LORA_FREQ_MHZ
  #define LORA_FREQ_MHZ  915.0
#endif
#ifndef LORA_BW_KHZ
  #define LORA_BW_KHZ    125.0
#endif
#ifndef LORA_SF
  #define LORA_SF        9
#endif
#ifndef LORA_CR
  #define LORA_CR        5
#endif
#ifndef LORA_TX_DBM
  #define LORA_TX_DBM    17
#endif

// Control channel used for discovery/sync at boot
#ifndef CTRL_FREQ_MHZ
  #define CTRL_FREQ_MHZ  LORA_FREQ_MHZ
#endif
#ifndef CTRL_BW_KHZ
  #define CTRL_BW_KHZ    125.0
#endif
#ifndef CTRL_SF
  #define CTRL_SF        9
#endif
#ifndef CTRL_CR
  #define CTRL_CR        5
#endif

// WiFi and OTA Configuration (Receiver only)
#ifdef ENABLE_WIFI_OTA
#include "wifi_manager.h"
#include "web_interface/web_server.h"

// Firmware storage for LoRa OTA cascade updates
static uint8_t storedFirmware[64 * 1024]; // 64KB buffer for firmware storage (reduced for DRAM)
static size_t storedFirmwareSize = 0;
static bool hasStoredFirmware = false;
static uint32_t firmwareVersion = 0x010000; // Version 1.0.0
#endif

SX1262 radio = new Module(PIN_LORA_NSS, PIN_LORA_DIO1, PIN_LORA_RST, PIN_LORA_BUSY);
static Preferences prefs;

static bool isSender = true;
static uint32_t seq = 0;
static uint32_t lastButtonMs = 0;
static int lastButtonState = HIGH;
static uint32_t buttonPressMs = 0;
static bool buttonPressed = false;



// Display mode cycling for receiver
static uint8_t displayMode = 0;
static const uint8_t MAX_DISPLAY_MODES = 3; // Signal info, network status, settings

// LoRa parameters that can be changed at runtime
static float currentFreq = LORA_FREQ_MHZ;
static float currentBW = LORA_BW_KHZ;
static int currentSF = LORA_SF;
static int currentCR = LORA_CR;
static int currentTxPower = LORA_TX_DBM;

// LoRa parameter arrays for cycling through values
static const int sfValues[] = {7, 8, 9, 10, 11, 12};
static const float bwValues[] = {62.5f, 125.0f, 250.0f, 500.0f};
static const int txPowerValues[] = {2, 3, 5, 8, 10, 12, 15, 17, 20, 22};

// Current indices for parameter cycling
static size_t currentSfIndex = 2;  // Default to SF9
static size_t currentBwIndex = 1;  // Default to 125kHz
static size_t currentTxIndex = 7;  // Default to 17dBm

// Signal quality tracking
static float lastRSSI = -999.0;
static float lastSNR = -999.0;
static uint32_t lastPacketTime = 0;
static uint32_t packetCount = 0;
static uint32_t errorCount = 0;

// Blinking dot state for ping indication
static uint32_t dotBlinkStartMs = 0;
static bool dotBlinkActive = false;
static const uint32_t DOT_FLASH_DURATION_MS = 1000; // 1 second on (half of 2-second ping interval)

// Available values for cycling
// Old arrays removed - using new arrays defined above

// Config broadcast state (sender)
static bool pendingConfigBroadcast = false;
static float pendingFreq = 0;
static float pendingBW = 0;
static int pendingSF = 0;
static int pendingCR = 0;
static int pendingTxPower = 0;
static uint32_t cfgLastTxMs = 0;
static int cfgRemaining = 0;

// OTA Update state
#ifdef ENABLE_WIFI_OTA
static bool wifiConnected = false;
static bool otaActive = false;
static uint32_t lastOtaCheck = 0;
#endif

// LoRa OTA state (both sender and receiver)
static bool loraOtaActive = false;
static uint32_t loraOtaStartTime = 0;
static uint32_t loraOtaTimeout = 30000; // 30 seconds timeout
static uint8_t loraOtaBuffer[1024]; // Buffer for OTA data
static size_t loraOtaBufferSize = 0;
static uint32_t loraOtaExpectedSize = 0;
static uint32_t loraOtaReceivedSize = 0;

// Persistence helpers
static void savePersistedSettings();
static void loadPersistedSettings();
static void computeIndicesFromCurrent();
static void broadcastConfigOnControlChannel(uint8_t times = 8, uint32_t intervalMs = 300);
static void tryReceiveConfigOnControlChannel(uint32_t durationMs = 4000);

// Deep sleep functions
static void enterDeepSleep();
static void configureWakeupSources();
static void restoreStateAfterWakeup();

// Full-screen ping flash function
static void drawFullScreenPingFlash();

// IP address scrolling state
static String currentIP = "";
static int ipScrollOffset = 0;
static uint32_t lastScrollUpdate = 0;
static const uint32_t SCROLL_INTERVAL_MS = 300; // Scroll every 300ms
static const int MAX_DISPLAY_WIDTH = 12; // Maximum characters that fit on screen

// Full-screen ping flash mode state
static const uint32_t IDLE_TIMEOUT_MS = 10000; // 10 seconds to enter idle mode
static const uint32_t INTERACTIVE_TIMEOUT_MS = 30000; // 30 seconds to return to idle mode
static uint32_t lastButtonPressTime = 0;
static bool isInIdleMode = false;
static uint32_t idleModeStartTime = 0;

// RTC memory for deep sleep state preservation
RTC_DATA_ATTR uint32_t sleepCount = 0;
RTC_DATA_ATTR uint32_t lastSleepTime = 0;
RTC_DATA_ATTR bool wasInSleepMode = false;

// Draw status bar at the bottom of the screen
static void drawStatusBar() {
  // Check if device has OLED display
  if (!DeviceConfig::DeviceManager::getCurrentCapabilities().hasOLED) {
    return;
  }

  u8g2.setFont(u8g2_font_5x7_tr); // Smaller font for status bar

  const int yPos = 120; // Bottom of screen
  int xPos = 2;

#ifdef ENABLE_WIFI_OTA
  if (!isSender) {
    // WiFi status
    if (wifiConnected) {
      // Get IP address and handle scrolling if needed
      String ipAddress = WiFi.localIP().toString();

      // Reset scroll if IP changed
      if (currentIP != ipAddress) {
        currentIP = ipAddress;
        ipScrollOffset = 0;
        lastScrollUpdate = millis();
      }

      // Display IP address with scrolling if too long
      const char* ipStr = ipAddress.c_str();
      int ipLength = strlen(ipStr);

      if (ipLength <= MAX_DISPLAY_WIDTH) {
        // IP fits, display normally
        u8g2.drawStr(xPos, yPos - 10, ipStr);
      } else {
        // IP is too long, implement scrolling
        uint32_t now = millis();
        if (now - lastScrollUpdate >= SCROLL_INTERVAL_MS) {
          ipScrollOffset++;
          // Reset scroll when we've shown the entire string
          if (ipScrollOffset > ipLength - MAX_DISPLAY_WIDTH) {
            ipScrollOffset = 0;
          }
          lastScrollUpdate = now;
        }

        // Create substring for scrolling display
        char scrolledIP[MAX_DISPLAY_WIDTH + 1];
        strncpy(scrolledIP, ipStr + ipScrollOffset, MAX_DISPLAY_WIDTH);
        scrolledIP[MAX_DISPLAY_WIDTH] = '\0';
        u8g2.drawStr(xPos, yPos - 10, scrolledIP);
      }

      // Display network location
      const char* location = getCurrentNetworkLocation();
      u8g2.drawStr(xPos, yPos, location);
      xPos += (strlen(location) * 6); // Approximate character width
    } else {
      u8g2.drawStr(xPos, yPos, "NoWiFi");
      xPos += 20;
    }

    // OTA status
    if (otaActive) {
      u8g2.drawStr(xPos, yPos, "OTA");
      xPos += 20;
    }

    // LoRa OTA status
    if (loraOtaActive) {
      u8g2.drawStr(xPos, yPos, "LoRaOTA");
    }
  }
#endif
}

// Trigger ping dot flash
static void triggerPingDotBlink() {
  if (!dotBlinkActive) {
    // Only start new flash if not already active (prevents overlapping flashes)
    dotBlinkStartMs = millis();
    dotBlinkActive = true;
    Serial.printf("[DEBUG] *** PING DOT FLASH STARTED at %lu ms ***\n", dotBlinkStartMs);
  } else {
    Serial.println("[DEBUG] Ping dot already active - not restarting");
  }
}

// Draw ping flash dot if active
static void drawPingDot() {
  if (!dotBlinkActive) return;

  uint32_t now = millis();
  uint32_t elapsed = now - dotBlinkStartMs;

  // Stop flashing after duration
  if (elapsed >= DOT_FLASH_DURATION_MS) {
    dotBlinkActive = false;
    Serial.println("[DEBUG] Ping dot flash finished");
    return;
  }

  if (isInIdleMode) {
    // Full-screen ping flash mode
    drawFullScreenPingFlash();
  } else {
    // Interactive mode - show small dot
    u8g2.drawDisc(55, 12, 4); // Main ping flash dot
    Serial.printf("[DEBUG] *** PING DOT VISIBLE *** elapsed=%lu ms\n", elapsed);
  }
}

static void oledMsg(const char* l1, const char* l2 = nullptr, const char* l3 = nullptr) {
  // Check if device has OLED display
  if (!DeviceConfig::DeviceManager::getCurrentCapabilities().hasOLED) {
    // Fallback to Serial output for non-OLED devices
    Serial.printf("[OLED] %s", l1);
    if (l2) Serial.printf(" - %s", l2);
    if (l3) Serial.printf(" - %s", l3);
    Serial.println();
    return;
  }

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);

  // Portrait layout: 64 pixels wide, 128 pixels tall

  // Top section - main status
  if (l1) u8g2.drawStr(2, 12, l1);

  // Second line - additional info (if provided) - moved down one line
  if (l2) u8g2.drawStr(2, 32, l2);

  // Middle section - signal quality for receiver mode
  if (!isSender && lastRSSI > -999.0) {
    char rssiStr[12], snrStr[12];
    snprintf(rssiStr, sizeof(rssiStr), "RSSI: %.0f", lastRSSI);
    snprintf(snrStr, sizeof(snrStr), "SNR: %.1f", lastSNR);

    u8g2.drawStr(2, 51, rssiStr);
    u8g2.drawStr(2, 65, snrStr);
  }

  // Bottom section - settings (moved up to make room for status bar)
  char settings[32];
  snprintf(settings, sizeof(settings), "SF%d BW%.0f", currentSF, currentBW);
  u8g2.drawStr(2, 81, settings);

  char modeStr[16];
  snprintf(modeStr, sizeof(modeStr), "%s %.1fMHz", isSender ? "TX" : "RX", currentFreq);
  u8g2.drawStr(2, 95, modeStr);

  // Status bar at the bottom - WiFi and OTA status
  drawStatusBar();

    // Draw blinking dot if ping activity
  drawPingDot();

  u8g2.sendBuffer();
}

// Role display removed - device role is now fixed at build time

static void oledSettings() {
  oledMsg("Settings", "Updated");
}

static void startConfigBroadcast(float newFreq, float newBW, int newSF, int newCR, int newTxPower) {
  pendingConfigBroadcast = true;
  pendingFreq = newFreq;
  pendingBW = newBW;
  pendingSF = newSF;
  pendingCR = newCR;
  pendingTxPower = newTxPower;
  cfgLastTxMs = 0;
  cfgRemaining = 8; // send several times for reliability
  oledMsg("Syncing...", "Sending config");
}

static void computeIndicesFromCurrent() {
      for (size_t i = 0; i < (sizeof(sfValues) / sizeof(sfValues[0])); i++) {
      if (sfValues[i] == currentSF) { currentSfIndex = i; break; }
    }
    for (size_t i = 0; i < (sizeof(bwValues) / sizeof(bwValues[0])); i++) {
      if (bwValues[i] == currentBW) { currentBwIndex = i; break; }
    }
    for (size_t i = 0; i < (sizeof(txPowerValues) / sizeof(txPowerValues[0])); i++) {
      if (txPowerValues[i] == currentTxPower) { currentTxIndex = i; break; }
    }
}

static void savePersistedSettings() {
  prefs.begin("LtngDet", false);
  prefs.putFloat("freq", currentFreq);
  prefs.putFloat("bw", currentBW);
  prefs.putInt("sf", currentSF);
  prefs.putInt("cr", currentCR);
  prefs.putInt("tx", currentTxPower);
  prefs.end();
}

// Role persistence removed - device role is now fixed at build time

static void loadPersistedSettings() {
  prefs.begin("LtngDet", true);
  bool haveFreq = prefs.isKey("freq");
  bool haveBW = prefs.isKey("bw");
  bool haveSF = prefs.isKey("sf");
  bool haveCR = prefs.isKey("cr");
  bool haveTX = prefs.isKey("tx");

  if (haveFreq) currentFreq = prefs.getFloat("freq", currentFreq);
  if (haveBW) currentBW = prefs.getFloat("bw", currentBW);
  if (haveSF) currentSF = prefs.getInt("sf", currentSF);
  if (haveCR) currentCR = prefs.getInt("cr", currentCR);
  if (haveTX) currentTxPower = prefs.getInt("tx", currentTxPower);
  // Role is no longer loaded from preferences - fixed at build time
  prefs.end();
}

static void initDisplay() {
  // Check if device has OLED display
  if (!DeviceConfig::DeviceManager::getCurrentCapabilities().hasOLED) {
    Serial.println("Device does not have OLED display - skipping OLED init");
    return;
  }

  // Power OLED via Vext and reset it
  pinMode(VEXT_PIN, OUTPUT);
  digitalWrite(VEXT_PIN, LOW);   // enable Vext
  delay(50);

  pinMode(OLED_RST_PIN, OUTPUT);
  digitalWrite(OLED_RST_PIN, LOW);
  delay(20);
  digitalWrite(OLED_RST_PIN, HIGH);
  delay(50);

  // I2C - use dynamic pins based on device
  Wire.begin(SystemConfig::Pins::I2C_SDA, SystemConfig::Pins::I2C_SCL);
  Wire.setTimeOut(1000);
  Wire.setClock(100000);
  delay(100);

  u8g2.setI2CAddress(0x3C << 1);
  if (!u8g2.begin()) {
    u8g2.setI2CAddress(0x3D << 1);
    if (!u8g2.begin()) {
      // give up
      while (true) {
        Serial.println("OLED init failed");
        delay(1000);
      }
    }
  }
  u8g2.setPowerSave(0);
  u8g2.setContrast(255);

  // Rotate display 90 degrees for portrait orientation
  u8g2.setDisplayRotation(U8G2_R1);
}

static void updateRadioSettings() {
  int st = radio.setFrequency(currentFreq);
  if (st == RADIOLIB_ERR_NONE) {
    st = radio.setBandwidth(currentBW);
  }
  if (st == RADIOLIB_ERR_NONE) {
    st = radio.setSpreadingFactor(currentSF);
  }
  if (st == RADIOLIB_ERR_NONE) {
    st = radio.setCodingRate(currentCR);
  }
  if (st == RADIOLIB_ERR_NONE) {
    st = radio.setOutputPower(currentTxPower);
  }

  if (st != RADIOLIB_ERR_NONE) {
    Serial.printf("Failed to update radio settings: %d\n", st);
    char errBuf[16]; snprintf(errBuf, sizeof(errBuf), "Settings fail %d", st);
    oledMsg("Settings fail", errBuf);
  } else {
    Serial.printf("Radio updated: SF%d BW%.0f Tx%ddBm\n", currentSF, currentBW, currentTxPower);
    oledSettings();
  }
}

static void initRadioOrHalt() {
  Serial.println("Initializing LoRa radio...");
  int st = radio.begin(currentFreq, currentBW, currentSF, currentCR, 0x34, currentTxPower);
  if (st != RADIOLIB_ERR_NONE) {
    char buf[48]; snprintf(buf, sizeof(buf), "Radio fail %d", st);
    oledMsg("Radio init", buf);
    while (true) { Serial.println(buf); delay(1000); }
  }
  radio.setDio2AsRfSwitch(true);
  radio.setCRC(true);
  oledSettings();
}

static void broadcastConfigOnControlChannel(uint8_t times, uint32_t intervalMs) {
  // Switch to control channel
  int st = radio.begin(CTRL_FREQ_MHZ, CTRL_BW_KHZ, CTRL_SF, CTRL_CR, 0x34, currentTxPower);
  if (st != RADIOLIB_ERR_NONE) {
    Serial.printf("[CTRL] begin fail %d\n", st);
    return;
  }
  radio.setDio2AsRfSwitch(true);
  radio.setCRC(true);

  char msg[64];
  snprintf(msg, sizeof(msg), "CFG F=%.1f BW=%.0f SF=%d CR=%d TX=%d",
           currentFreq, currentBW, currentSF, currentCR, currentTxPower);

  for (uint8_t i = 0; i < times; i++) {
    int tx = radio.transmit(msg);
    Serial.printf("[CTRL][TX] %s %s\n", msg, tx == RADIOLIB_ERR_NONE ? "OK" : "FAIL");
    delay(intervalMs);
  }

  // Restore operational settings
  st = radio.begin(currentFreq, currentBW, currentSF, currentCR, 0x34, currentTxPower);
  if (st != RADIOLIB_ERR_NONE) {
    Serial.printf("[CTRL] restore begin fail %d\n", st);
  } else {
    radio.setDio2AsRfSwitch(true);
    radio.setCRC(true);
  }
}

static void tryReceiveConfigOnControlChannel(uint32_t durationMs) {
  // Switch to control channel
  int st = radio.begin(CTRL_FREQ_MHZ, CTRL_BW_KHZ, CTRL_SF, CTRL_CR, 0x34, currentTxPower);
  if (st != RADIOLIB_ERR_NONE) {
    Serial.printf("[CTRL] begin fail %d\n", st);
    return;
  }
  radio.setDio2AsRfSwitch(true);
  radio.setCRC(true);

  uint32_t start = millis();
  while (millis() - start < durationMs) {
    String rx;
    int r = radio.receive(rx);
    if (r == RADIOLIB_ERR_NONE && rx.startsWith("CFG ")) {
      float nf = currentFreq;
      float nb = currentBW;
      int nsf = currentSF;
      int ncr = currentCR;
      int ntx = currentTxPower;
      int parsed = sscanf(rx.c_str(), "CFG F=%f BW=%f SF=%d CR=%d TX=%d", &nf, &nb, &nsf, &ncr, &ntx);
      if (parsed == 5) {
        currentFreq = nf;
        currentBW = nb;
        currentSF = nsf;
        currentCR = ncr;
        currentTxPower = ntx;
        computeIndicesFromCurrent();
        savePersistedSettings();
        Serial.printf("[CTRL][RX] applied %s\n", rx.c_str());
        break;
      }
    }
    delay(50);
  }

  // Restore operational settings (applied ones if updated)
  st = radio.begin(currentFreq, currentBW, currentSF, currentCR, 0x34, currentTxPower);
  if (st != RADIOLIB_ERR_NONE) {
    Serial.printf("[CTRL] restore begin fail %d\n", st);
  } else {
    radio.setDio2AsRfSwitch(true);
    radio.setCRC(true);
  }
}

static void handleSenderButtonAction(ButtonAction action) {
  // Record button press time and exit idle mode
  lastButtonPressTime = millis();
  if (isInIdleMode) {
    isInIdleMode = false;
    Serial.println("[IDLE] Exiting idle mode due to button press");
    oledMsg("Interactive", "Mode");
    delay(1000);
  }

  switch (action) {
    case ButtonAction::CycleSF:
      // Cycle through Spreading Factor values
      {
        const int nextIndex = (currentSfIndex + 1) % (sizeof(sfValues) / sizeof(sfValues[0]));
        const int nextSF = sfValues[nextIndex];
        startConfigBroadcast(currentFreq, currentBW, nextSF, currentCR, currentTxPower);
        Serial.printf("SF change requested -> %d (broadcasting to receiver)\n", nextSF);
        oledMsg("SF Changed", String(nextSF).c_str());
      }
      break;
    case ButtonAction::CycleBW:
      // Cycle through Bandwidth values
      {
        const int nextIndex = (currentBwIndex + 1) % (sizeof(bwValues) / sizeof(bwValues[0]));
        const float nextBW = bwValues[nextIndex];
        startConfigBroadcast(currentFreq, nextBW, currentSF, currentCR, currentTxPower);
        Serial.printf("BW change requested -> %.0f kHz (broadcasting to receiver)\n", nextBW);
        oledMsg("BW Changed", String(nextBW).c_str());
      }
      break;
    case ButtonAction::SleepMode:
      Serial.println("Sleep mode requested");
      enterDeepSleep();
      break;
    default:
      break;
  }
}

static void handleReceiverButtonAction(ButtonAction action) {
  // Record button press time and exit idle mode
  lastButtonPressTime = millis();
  if (isInIdleMode) {
    isInIdleMode = false;
    Serial.println("[IDLE] Exiting idle mode due to button press");
    oledMsg("Interactive", "Mode");
    delay(1000);
  }

  Serial.printf("[RX_BTN] Handling action: %d\n", (int)action);
  switch (action) {
    case ButtonAction::CycleSF:
      // Cycle through Spreading Factor values (same as sender)
      {
        const int nextIndex = (currentSfIndex + 1) % (sizeof(sfValues) / sizeof(sfValues[0]));
        const int nextSF = sfValues[nextIndex];
        Serial.printf("[RX_BTN] SF change requested -> %d\n", nextSF);
        oledMsg("SF Changed", String(nextSF).c_str());
      }
      break;
    case ButtonAction::CycleBW:
      // Cycle through Bandwidth values (same as sender)
      {
        const int nextIndex = (currentBwIndex + 1) % (sizeof(bwValues) / sizeof(bwValues[0]));
        const float nextBW = bwValues[nextIndex];
        Serial.printf("[RX_BTN] BW change requested -> %.0f kHz\n", nextBW);
        oledMsg("BW Changed", String(nextBW).c_str());
      }
      break;
    case ButtonAction::SleepMode:
      Serial.println("[RX_BTN] Sleep mode triggered");
      Serial.println("Sleep mode requested");
      enterDeepSleep();
      break;
    default:
      Serial.printf("[RX_BTN] Unknown action: %d\n", (int)action);
      break;
  }
}

static void updateButton() {
  static uint32_t callCount = 0;
  callCount++;

  int s = digitalRead(BUTTON_PIN);
  uint32_t now = millis();

  // Debug button state every 5 seconds
  static uint32_t lastDebugMs = 0;
  if (now - lastDebugMs >= 5000) {
    int altState = digitalRead(ALT_BUTTON_PIN);
    Serial.printf("[BTN_DEBUG] Main pin: %d, Alt pin: %d, lastButtonState: %d, buttonPressed: %s, role: %s, calls: %lu\n",
                  s, altState, lastButtonState, buttonPressed ? "true" : "false", isSender ? "Sender" : "Receiver", callCount);
    lastDebugMs = now;
  }

  // Button press detection
  if (lastButtonState == HIGH && s == LOW) {
    buttonPressed = true;
    buttonPressMs = now;
    Serial.printf("[BTN] Press detected (role: %s)\n", isSender ? "Sender" : "Receiver");
  }

  // Button release detection
  if (lastButtonState == LOW && s == HIGH && buttonPressed) {
    buttonPressed = false;
    uint32_t pressDuration = now - buttonPressMs;
    Serial.printf("[BTN] Release detected, duration: %lu ms (role: %s)\n", pressDuration, isSender ? "Sender" : "Receiver");

    // Handle single press actions
    ButtonAction action = classifyPress(pressDuration);
    Serial.printf("[BTN] Single press action: %d\n", (int)action);
    if (action != ButtonAction::Ignore) {
      if (isSender) {
        Serial.println("[BTN] Handling sender action");
        handleSenderButtonAction(action);
      } else {
        Serial.println("[BTN] Handling receiver action");
        handleReceiverButtonAction(action);
      }
    }

    lastButtonMs = now;
  }

  lastButtonState = s;
}

// OTA Function Declarations
#ifdef ENABLE_WIFI_OTA
static void initWiFi();
static void initOTA();
static void triggerLoraFirmwareUpdates();
static bool storeCurrentFirmware();
#endif

// LoRa OTA Functions (both sender and receiver)
static void handleLoraOtaPacket(const String& packet);
static void checkLoraOtaTimeout();

#ifdef ENABLE_WIFI_OTA
// Only receivers send firmware out
static void sendLoraOtaUpdate(const uint8_t* firmware, size_t firmwareSize);
#endif

// OLED Display Functions
static void drawStatusBar();

// Simple button test function
static void testButton() {
  Serial.println("[BTN_TEST] Starting button test...");

  // Test both button pins
  int mainState = digitalRead(BUTTON_PIN);
  int altState = digitalRead(ALT_BUTTON_PIN);

  Serial.printf("[BTN_TEST] Main button (GPIO%d): %d, Alt button (GPIO%d): %d\n",
                BUTTON_PIN, mainState, ALT_BUTTON_PIN, altState);

  // Test button press detection
  Serial.println("[BTN_TEST] Press and hold the button for 2 seconds...");
  uint32_t startTime = millis();
  bool buttonPressed = false;

  while (millis() - startTime < 2000) {
    int currentState = digitalRead(BUTTON_PIN);
    if (currentState == LOW && !buttonPressed) {
      buttonPressed = true;
      Serial.println("[BTN_TEST] Button press detected!");
    }
    delay(100);
  }

  if (!buttonPressed) {
    Serial.println("[BTN_TEST] No button press detected during test");
  }

  Serial.println("[BTN_TEST] Button test complete");
}

// Deep sleep implementation
static void configureWakeupSources() {
  // Configure button as wake-up source
  esp_sleep_enable_ext0_wakeup((gpio_num_t)BUTTON_PIN, LOW);

  // Also enable timer wake-up as backup (30 seconds)
  esp_sleep_enable_timer_wakeup(30 * 1000000); // 30 seconds in microseconds

  Serial.println("[SLEEP] Wake-up sources configured: button (LOW) + 30s timer");
}

// Idle mode management
static void checkIdleMode() {
  uint32_t currentTime = millis();

  // Check if we should enter idle mode (no button press for IDLE_TIMEOUT_MS)
  if (!isInIdleMode && (currentTime - lastButtonPressTime) > IDLE_TIMEOUT_MS) {
    isInIdleMode = true;
    idleModeStartTime = currentTime;
    Serial.println("[IDLE] Entering full-screen ping flash mode");
    oledMsg("Idle Mode", "Full Screen");
    delay(1000);
  }

  // Check if we should exit idle mode (been in interactive mode for INTERACTIVE_TIMEOUT_MS)
  if (isInIdleMode && (currentTime - idleModeStartTime) > INTERACTIVE_TIMEOUT_MS) {
    isInIdleMode = false;
    Serial.println("[IDLE] Returning to full-screen ping flash mode");
    oledMsg("Idle Mode", "Full Screen");
    delay(1000);
  }
}

// Full-screen ping flash function
static void drawFullScreenPingFlash() {
  if (!isInIdleMode) return;

  // Clear the entire screen
  u8g2.clearBuffer();

  // Draw a large, centered ping indicator
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.setDrawColor(1);

  // Calculate center position
  int textWidth = u8g2.getStrWidth("PING");
  int x = (u8g2.getDisplayWidth() - textWidth) / 2;
  int y = (u8g2.getDisplayHeight() + 14) / 2; // +14 for font height

  // Draw "PING" text
  u8g2.drawStr(x, y, "PING");

  // Draw a large circle around it
  int centerX = u8g2.getDisplayWidth() / 2;
  int centerY = u8g2.getDisplayHeight() / 2;
  int radius = 25;
  u8g2.drawCircle(centerX, centerY, radius);

  // Send to display
  u8g2.sendBuffer();
}

static void enterDeepSleep() {
  Serial.println("[SLEEP] Entering deep sleep mode...");

  // Save current state to RTC memory
  sleepCount++;
  lastSleepTime = millis();
  wasInSleepMode = true;

  // Save important settings to flash before sleep
  savePersistedSettings();

  // Show sleep message on OLED
  oledMsg("Sleep Mode", "Entering...");
  delay(1000);

  // Turn off OLED to save power
  u8g2.setPowerSave(1);

  // Configure wake-up sources
  configureWakeupSources();

  Serial.println("[SLEEP] Going to sleep now. Press button to wake up.");
  Serial.flush(); // Ensure all serial output is sent

  // Enter deep sleep
  esp_deep_sleep_start();
}

static void restoreStateAfterWakeup() {
  if (wasInSleepMode) {
    Serial.println("[SLEEP] Waking up from deep sleep...");
    Serial.printf("[SLEEP] Sleep count: %lu, Last sleep time: %lu ms ago\n",
                  sleepCount, millis() - lastSleepTime);

    // Reset sleep flag
    wasInSleepMode = false;

    // Restore OLED power
    u8g2.setPowerSave(0);

    // Show wake-up message
    oledMsg("Wake Up", "Resuming...");
    delay(1000);

    Serial.println("[SLEEP] State restored, resuming normal operation");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // Initialize NVS first
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  if (ret != ESP_OK) {
    Serial.printf("[SETUP] NVS init failed: %s\n", esp_err_to_name(ret));
    // Continue anyway, but some features may not work
  } else {
    Serial.println("[SETUP] NVS initialized successfully");
  }

  // Initialize device configuration first
  SystemConfig::Pins::initializePins();
  DeviceConfig::DeviceType device = DeviceConfig::DeviceManager::detectDevice();
  const char* deviceName = DeviceConfig::DeviceManager::getDeviceName(device);

  Serial.printf("\n=== LtngDet LoRa + OLED (%s) ===\n", deviceName);
  Serial.printf("[SETUP] Detected device: %s\n", deviceName);
  Serial.printf("[SETUP] Has WiFi: %s\n", DeviceConfig::DeviceManager::getCurrentCapabilities().hasWiFi ? "YES" : "NO");
  Serial.printf("[SETUP] Has GPS: %s\n", DeviceConfig::DeviceManager::getCurrentCapabilities().hasGPS ? "YES" : "NO");

  // Check if we're waking up from deep sleep
  restoreStateAfterWakeup();

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(ALT_BUTTON_PIN, INPUT_PULLUP);
  Serial.printf("[SETUP] Button pin %d configured as INPUT_PULLUP\n", BUTTON_PIN);
  Serial.printf("[SETUP] Alt button pin %d configured as INPUT_PULLUP\n", ALT_BUTTON_PIN);

  // Test button pin states
  int initialButtonState = digitalRead(BUTTON_PIN);
  int initialAltButtonState = digitalRead(ALT_BUTTON_PIN);
  Serial.printf("[SETUP] Main button state: %d, Alt button state: %d (HIGH=%d, LOW=%d)\n",
                initialButtonState, initialAltButtonState, HIGH, LOW);

  // Explicitly initialize button state variables
  lastButtonState = HIGH;
  buttonPressed = false;
  Serial.println("[SETUP] Button state variables initialized");

  // Initialize idle mode variables
  lastButtonPressTime = millis();
  isInIdleMode = false;
  idleModeStartTime = 0;
  Serial.println("[SETUP] Idle mode variables initialized");

  // Test button functionality
  testButton();

  // Initialize current values
  currentFreq = LORA_FREQ_MHZ;
  currentBW = LORA_BW_KHZ;
  currentSF = LORA_SF;
  currentCR = LORA_CR;
  currentTxPower = LORA_TX_DBM;

  // initial role from build flags
#ifdef ROLE_SENDER
  isSender = true;
  Serial.println("[SETUP] Role: SENDER (from ROLE_SENDER flag)");
#elif defined(ROLE_RECEIVER)
  isSender = false;
  Serial.println("[SETUP] Role: RECEIVER (from ROLE_RECEIVER flag)");
#else
  isSender = true;
  Serial.println("[SETUP] Role: SENDER (default)");
#endif

  // Load persisted settings (role is now fixed at build time)
  loadPersistedSettings();
  computeIndicesFromCurrent();

  initDisplay();
  oledMsg("Booting...", "Heltec V3");
  oledMsg("Role", isSender ? "Sender" : "Receiver");

  initRadioOrHalt();

  // Initialize WiFi and OTA for receivers (only if device supports WiFi)
#ifdef ENABLE_WIFI_OTA
  if (!isSender && DeviceConfig::DeviceManager::getCurrentCapabilities().hasWiFi) {
    Serial.println("[MAIN] Initializing WiFi for receiver...");
    initWiFi();
    Serial.printf("[MAIN] WiFi connection status: %s\n", WiFi.status() == WL_CONNECTED ? "CONNECTED" : "DISCONNECTED");
    Serial.printf("[MAIN] wifiConnected variable: %s\n", wifiConnected ? "TRUE" : "FALSE");
    if (wifiConnected) {
      initOTA();
      oledMsg("WiFi + OTA", "Ready");
      // Start web interface server only when WiFi is connected
      Serial.println("[MAIN] Starting web server...");
      webServerManager.begin();
    } else {
      Serial.println("[MAIN] WiFi not connected, skipping web server start");
    }
  } else if (!isSender && !DeviceConfig::DeviceManager::getCurrentCapabilities().hasWiFi) {
    Serial.println("[MAIN] Device does not support WiFi, skipping WiFi initialization");
    oledMsg("No WiFi", "Device");
  }
#endif

  // Broadcast current settings at boot if sender
  if (isSender) {
    // Give receivers time to enter control-channel listen
    delay(750);
    // Also use the control channel to reach mismatched receivers
    broadcastConfigOnControlChannel(6, 250);
    startConfigBroadcast(currentFreq, currentBW, currentSF, currentCR, currentTxPower);
  }
  // Try to catch a control-channel config at boot if receiver
  if (!isSender) {
    tryReceiveConfigOnControlChannel(6000);
  }
}

void loop() {
  static uint32_t lastTxMs = 0;
  static uint32_t lastRxMs = 0;
  uint32_t now = millis();

  // Handle web server requests if receiver role and WiFi connected
#ifdef ENABLE_WIFI_OTA
  if (!isSender && WiFi.status() == WL_CONNECTED) {
    static uint32_t lastWebDebug = 0;
    if (millis() - lastWebDebug > 10000) { // Debug every 10 seconds
      Serial.printf("[MAIN] Calling webServerManager.loop(), WiFi status: %s\n", WiFi.status() == WL_CONNECTED ? "CONNECTED" : "DISCONNECTED");
      lastWebDebug = millis();
    }
    webServerManager.loop();
  }
#endif

  // Check button more frequently
  updateButton();

  // Check and manage idle mode transitions
  checkIdleMode();

  if (isSender) {
    if (pendingConfigBroadcast) {
      if (now - lastTxMs >= 50 && now - cfgLastTxMs >= 300) {
        char msg[64];
        snprintf(msg, sizeof(msg), "CFG F=%.1f BW=%.0f SF=%d CR=%d TX=%d",
                 pendingFreq, pendingBW, pendingSF, pendingCR, pendingTxPower);
        int st = radio.transmit(msg);
        if (st == RADIOLIB_ERR_NONE) {
          Serial.printf("[TX] %s OK\n", msg);
        } else {
          Serial.printf("[TX] %s FAIL %d\n", msg, st);
        }
        cfgLastTxMs = now;
        cfgRemaining--;
        if (cfgRemaining <= 0) {
          // Apply the new settings on the transmitter after broadcasting
          currentFreq = pendingFreq;
          currentBW = pendingBW;
          currentSF = pendingSF;
          currentCR = pendingCR;
          currentTxPower = pendingTxPower;

          // Update index trackers to reflect applied settings
          for (size_t i = 0; i < (sizeof(sfValues) / sizeof(sfValues[0])); i++) {
            if (sfValues[i] == currentSF) { currentSfIndex = i; break; }
          }
          for (size_t i = 0; i < (sizeof(bwValues) / sizeof(bwValues[0])); i++) {
            if (bwValues[i] == currentBW) { currentBwIndex = i; break; }
          }
          for (size_t i = 0; i < (sizeof(txPowerValues) / sizeof(txPowerValues[0])); i++) {
            if (txPowerValues[i] == currentTxPower) { currentTxIndex = i; break; }
          }

          updateRadioSettings();
          savePersistedSettings();
          oledMsg("Sync complete", "TX switched");
          pendingConfigBroadcast = false;
          lastTxMs = now; // reset TX timer
        }
      }
    } else {
      // Non-blocking TX every 2 seconds
      if (now - lastTxMs >= 2000) {
        char msg[48];
        snprintf(msg, sizeof(msg), "PING seq=%lu", (unsigned long)seq++);
        int st = radio.transmit(msg);
        if (st == RADIOLIB_ERR_NONE) {
          Serial.printf("[TX] %s OK\n", msg);
          Serial.printf("[DEBUG] TX seq counter at: %lu\n", (unsigned long)(seq-1));
          // Trigger blinking dot instead of showing PING text
          triggerPingDotBlink();
        } else {
          char e[24]; snprintf(e, sizeof(e), "err %d", st);
          Serial.printf("[TX] %s FAIL %s\n", msg, e);
          oledMsg("TX FAIL", msg, e);
        }
        lastTxMs = now;
      }
    }
  } else {
    // Non-blocking RX every 50ms
    if (now - lastRxMs >= 50) {
      String rx;
      int st = radio.receive(rx);
      if (st == RADIOLIB_ERR_NONE) {
        float rssi = radio.getRSSI();
        float snr  = radio.getSNR();

        // Update signal quality tracking
        lastRSSI = rssi;
        lastSNR = snr;
        lastPacketTime = now;
        packetCount++;

        if (rx.startsWith("CFG ")) {
          float nf = currentFreq;
          float nb = currentBW;
          int nsf = currentSF;
          int ncr = currentCR;
          int ntx = currentTxPower;
          int parsed = sscanf(rx.c_str(), "CFG F=%f BW=%f SF=%d CR=%d TX=%d", &nf, &nb, &nsf, &ncr, &ntx);
          if (parsed == 5) {
            currentFreq = nf;
            currentBW = nb;
            currentSF = nsf;
            currentCR = ncr;
            currentTxPower = ntx;

            // Update index trackers to reflect applied settings
            for (size_t i = 0; i < (sizeof(sfValues) / sizeof(sfValues[0])); i++) {
              if (sfValues[i] == currentSF) { currentSfIndex = i; break; }
            }
            for (size_t i = 0; i < (sizeof(bwValues) / sizeof(bwValues[0])); i++) {
              if (bwValues[i] == currentBW) { currentBwIndex = i; break; }
            }
            for (size_t i = 0; i < (sizeof(txPowerValues) / sizeof(txPowerValues[0])); i++) {
              if (txPowerValues[i] == currentTxPower) { currentTxIndex = i; break; }
            }

            updateRadioSettings();
            savePersistedSettings();
            char l2[20]; snprintf(l2, sizeof(l2), "RSSI %.1f", rssi);
            Serial.printf("[RX] APPLIED %s | SNR %.1f | PKT:%lu\n", rx.c_str(), snr, packetCount);
            oledMsg("SYNC", rx.c_str(), l2);
          } else {
            char l2[20]; snprintf(l2, sizeof(l2), "RSSI %.1f", rssi);
            Serial.printf("[RX] CFG PARSE FAIL | %s | SNR %.1f | PKT:%lu\n", rx.c_str(), snr, packetCount);
            oledMsg("RX", rx.c_str(), l2);
          }
        } else if (rx.startsWith("OTA_")) {
          // Handle OTA packets (both roles)
          handleLoraOtaPacket(rx);
        } else if (rx.startsWith("FW_UPDATE_AVAILABLE") || rx.startsWith("UPDATE_NOW")) {
          // Sender: request update when notified
          if (isSender) {
            Serial.println("FW update notice received; requesting update...");
            radio.transmit("REQUEST_UPDATE");
          }
        } else if (rx.startsWith("REQUEST_UPDATE")) {
          // Receiver only: handle update request from transmitter
          if (!isSender) {
            Serial.println("Transmitter requested firmware update!");
            oledMsg("Update Req", "Received");

            // Acknowledge the request
            radio.transmit("UPDATE_ACK");
            delay(100);

            // Send the actual firmware if we have it stored
            #ifdef ENABLE_WIFI_OTA
            if (hasStoredFirmware && storedFirmwareSize > 0) {
              Serial.printf("Sending stored firmware (%zu bytes) to transmitter\n", storedFirmwareSize);
              oledMsg("Sending FW", "To TX");
              sendLoraOtaUpdate(storedFirmware, storedFirmwareSize);
            } else {
              Serial.println("No firmware stored to send!");
              oledMsg("No FW", "Stored");
              radio.transmit("NO_FIRMWARE");
            }
            #else
            radio.transmit("NO_FIRMWARE");
            #endif
          }
        } else {
          char l2[20]; snprintf(l2, sizeof(l2), "RSSI %.1f", rssi);
          if (rx.startsWith("PING ")) {
            // Extract and log the sequence number for verification
            const char* seqPtr = strstr(rx.c_str(), "seq=");
            if (seqPtr) {
              unsigned long rxSeq = strtoul(seqPtr + 4, nullptr, 10);
              Serial.printf("[DEBUG] RX parsed seq: %lu\n", rxSeq);
            }
            // Log ping reception to serial console
            Serial.printf("[RX] %s | %s | SNR %.1f | PKT:%lu\n", rx.c_str(), l2, snr, packetCount);
            // Trigger blinking dot instead of showing PING text
            triggerPingDotBlink();
          } else {
            Serial.printf("[RX] %s | %s | SNR %.1f | PKT:%lu\n", rx.c_str(), l2, snr, packetCount);
            oledMsg("RX", rx.c_str(), l2);
          }
        }
      } else if (st != RADIOLIB_ERR_RX_TIMEOUT) {
        errorCount++;
        char e[24]; snprintf(e, sizeof(e), "err %d", st);
        Serial.printf("[RX] FAIL %s | ERR:%lu\n", e, errorCount);
        oledMsg("RX FAIL", e);
      }
      lastRxMs = now;
    }
  }

  // Handle OTA updates (WiFi OTA only on receiver)
  #ifdef ENABLE_WIFI_OTA
  if (!isSender && wifiConnected) {
    ArduinoOTA.handle();

    // Periodically check WiFi connection and reconnect if needed
    static uint32_t lastWiFiCheck = 0;
    if (now - lastWiFiCheck >= 30000) { // Check every 30 seconds
      if (!checkWiFiConnection()) {
        wifiConnected = false;
        oledMsg("WiFi", "Reconnecting...");
      } else if (!wifiConnected) {
        wifiConnected = true;
        oledMsg("WiFi", "Reconnected");
      }
      lastWiFiCheck = now;
    }
  }
  #endif

  // Check LoRa OTA timeout (both roles)
  checkLoraOtaTimeout();

        // Refresh display when dot state changes
  static bool lastDotState = false;
  static uint32_t lastDotRefresh = 0;

        if (dotBlinkActive && (now - lastDotRefresh >= 200)) {
    // Refresh display periodically while dot is active (keep original display content)
    // The dot will be added automatically by drawPingDot() in oledMsg()
    oledMsg("Role", isSender ? "Sender" : "Receiver"); // Show normal "Mode" / "Sender" or "Receiver" display
    lastDotRefresh = now;
  } else if (lastDotState && !dotBlinkActive) {
    // Dot just finished - refresh display to clear it (normal content, no dot)
    oledMsg("Role", isSender ? "Sender" : "Receiver"); // Show normal "Mode" / "Sender" or "Receiver" display
  }
  lastDotState = dotBlinkActive;

  // Small delay to prevent overwhelming the system, but keep button responsive
  delay(10);
}

// WiFi and OTA Functions (Receiver only)
#ifdef ENABLE_WIFI_OTA
static void initWiFi() {
  Serial.println("Initializing WiFi...");
  oledMsg("WiFi", "Connecting...");

  // Initialize WiFi preferences
  initWiFiPreferences();

  // Print configured networks
  printConfiguredNetworks();

  // Attempt to connect using the WiFi manager
  if (connectToWiFi()) {
    wifiConnected = true;
    const char* location = getCurrentNetworkLocation();
    Serial.printf("\nWiFi connected to %s! IP: %s\n",
                  location, WiFi.localIP().toString().c_str());
    oledMsg("WiFi", location);
  } else {
    Serial.println("\nWiFi connection failed!");
    oledMsg("WiFi", "Failed!");
  }
}

static void initOTA() {
  if (!wifiConnected) return;

  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);

  ArduinoOTA.onStart([]() {
    otaActive = true;
    Serial.println("OTA Update starting...");
    oledMsg("OTA", "Starting...");
  });

    ArduinoOTA.onEnd([]() {
    otaActive = false;
    Serial.println("OTA Update complete!");
    oledMsg("OTA", "Complete!");
    delay(1000);

    // NEW: Store the firmware for LoRa OTA cascade updates
    #ifdef ENABLE_WIFI_OTA
    if (!isSender) {
      // Store the current firmware for LoRa OTA distribution
      if (storeCurrentFirmware()) {
        Serial.println("Firmware stored for LoRa OTA distribution");
        oledMsg("Firmware", "Stored");
        delay(1000);

        // Now trigger LoRa firmware updates
        Serial.println("Triggering LoRa firmware updates...");
        oledMsg("LoRa Update", "Triggering...");
        triggerLoraFirmwareUpdates();
      } else {
        Serial.println("Failed to store firmware for LoRa OTA");
        oledMsg("Firmware", "Store failed");
        delay(1000);
      }
    }
    #endif
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int percent = (progress * 100) / total;
    char progressStr[20];
    snprintf(progressStr, sizeof(progressStr), "%d%%", percent);
    oledMsg("OTA Update", progressStr);
  });

  ArduinoOTA.onError([](ota_error_t error) {
    otaActive = false;
    Serial.printf("OTA Error: %u\n", error);
    char errorStr[20];
    snprintf(errorStr, sizeof(errorStr), "Error: %u", error);
    oledMsg("OTA Error", errorStr);
  });

  ArduinoOTA.begin();
  Serial.println("OTA initialized");
}
#endif

// LoRa OTA Functions (both sender and receiver)
static void handleLoraOtaPacket(const String& packet) {
  if (packet.startsWith("OTA_START:")) {
    // Format: OTA_START:size:chunks
    int parsed = sscanf(packet.c_str(), "OTA_START:%u:%u", &loraOtaExpectedSize, &loraOtaTimeout);
    if (parsed == 2) {
      loraOtaActive = true;
      loraOtaStartTime = millis();
      loraOtaReceivedSize = 0;
      loraOtaBufferSize = 0;

      Serial.printf("LoRa OTA starting: %lu bytes\n", loraOtaExpectedSize);
      oledMsg("LoRa OTA", "Starting...");
    }
  } else if (packet.startsWith("OTA_DATA:")) {
    if (!loraOtaActive) return;

    // Format: OTA_DATA:chunk:data
    int chunk;
    char data[256];
    int parsed = sscanf(packet.c_str(), "OTA_DATA:%d:%255s", &chunk, data);
    if (parsed == 2) {
      // Decode base64 data and add to buffer
      // For simplicity, we'll use a basic approach
      size_t dataLen = strlen(data);
      if (loraOtaBufferSize + dataLen < sizeof(loraOtaBuffer)) {
        memcpy(loraOtaBuffer + loraOtaBufferSize, data, dataLen);
        loraOtaBufferSize += dataLen;
        loraOtaReceivedSize += dataLen;

        int percent = (loraOtaReceivedSize * 100) / loraOtaExpectedSize;
        char progressStr[20];
        snprintf(progressStr, sizeof(progressStr), "%d%%", percent);
        oledMsg("LoRa OTA", progressStr);
      }
    }
  } else if (packet.startsWith("OTA_END:")) {
    if (!loraOtaActive) return;

    // Verify we have all data
    if (loraOtaReceivedSize >= loraOtaExpectedSize) {
      Serial.println("LoRa OTA complete, flashing...");
      oledMsg("LoRa OTA", "Flashing...");

      // Flash the firmware
#ifdef ENABLE_WIFI_OTA
      if (Update.begin(loraOtaExpectedSize)) {
        (void)Update.write(loraOtaBuffer, loraOtaBufferSize); // Suppress unused variable warning
        if (Update.end()) {
          Serial.println("Firmware flashed successfully!");
          oledMsg("OTA Complete", "Rebooting...");
          delay(2000);
          ESP.restart();
        } else {
          Serial.println("Firmware flash failed!");
          oledMsg("OTA Error", "Flash failed!");
        }
      } else {
        Serial.println("OTA begin failed!");
        oledMsg("OTA Error", "Begin failed!");
      }
#else
      Serial.println("Update library not available - cannot flash firmware");
      oledMsg("OTA Error", "Update not available");
#endif
    }

    loraOtaActive = false;
  }
}

static void checkLoraOtaTimeout() {
  if (loraOtaActive && (millis() - loraOtaStartTime > loraOtaTimeout)) {
    Serial.println("LoRa OTA timeout!");
    oledMsg("LoRa OTA", "Timeout!");
    loraOtaActive = false;
  }
}

// Function to send OTA update to transmitters (receiver only)
#ifdef ENABLE_WIFI_OTA
static void sendLoraOtaUpdate(const uint8_t* firmware, size_t firmwareSize) {
  if (isSender) return; // Only receivers can send OTA updates

  Serial.printf("Sending LoRa OTA update: %zu bytes\n", firmwareSize);
  oledMsg("LoRa OTA", "Sending...");

  // Send OTA start packet
  char startMsg[64];
  snprintf(startMsg, sizeof(startMsg), "OTA_START:%zu:%lu", firmwareSize, loraOtaTimeout);
  radio.transmit(startMsg);
  delay(100);

  // Break firmware into chunks and send
  const size_t chunkSize = 200; // Max LoRa packet size
  size_t sentBytes = 0;
  int chunkNum = 0;

  while (sentBytes < firmwareSize) {
    size_t currentChunkSize = min(chunkSize, firmwareSize - sentBytes);

    // Create chunk packet
    char chunkMsg[256];
    snprintf(chunkMsg, sizeof(chunkMsg), "OTA_DATA:%d:", chunkNum);

    // Copy chunk data (simplified - in practice you'd want base64 encoding)
    memcpy(chunkMsg + strlen(chunkMsg), firmware + sentBytes, currentChunkSize);
    chunkMsg[strlen(chunkMsg) + currentChunkSize] = '\0';

    // Send chunk
    radio.transmit(chunkMsg);
    delay(50);

    sentBytes += currentChunkSize;
    chunkNum++;

    // Update progress
    int percent = (sentBytes * 100) / firmwareSize;
    char progressStr[20];
    snprintf(progressStr, sizeof(progressStr), "Sending %d%%", percent);
    oledMsg("LoRa OTA", progressStr);
  }

  // Send OTA end packet
  radio.transmit("OTA_END:");
  delay(100);

  Serial.println("LoRa OTA update sent!");
  oledMsg("LoRa OTA", "Sent!");
}

// NEW: Function to automatically trigger LoRa firmware updates after WiFi OTA
static void triggerLoraFirmwareUpdates() {
  if (isSender) return; // Only receivers can trigger updates

  Serial.println("Broadcasting firmware update notification...");
  oledMsg("LoRa Update", "Broadcasting...");

  // Proactively resync receivers to our current settings over control channel
  // to maximize the chance they can hear the update notifications
  broadcastConfigOnControlChannel(8, 250);

  // Send multiple notifications to ensure transmitters receive them
  for (int i = 0; i < 10; i++) {
    // Send firmware update available notification
    radio.transmit("FW_UPDATE_AVAILABLE");
    delay(200);

          // Send version info from stored firmware
      char versionMsg[64];
      if (hasStoredFirmware) {
        snprintf(versionMsg, sizeof(versionMsg), "FW_VERSION:%lu", firmwareVersion);
      } else {
        snprintf(versionMsg, sizeof(versionMsg), "FW_VERSION:0.0.0");
      }
      radio.transmit(versionMsg);
      delay(200);

    // Send update trigger command
    radio.transmit("UPDATE_NOW");
    delay(200);
  }

  Serial.println("Firmware update notifications sent!");
  oledMsg("LoRa Update", "Notifications sent!");

  // Wait a bit for transmitters to process
  delay(2000);

  // Check if any transmitters requested updates
  Serial.println("Checking for update requests...");
  oledMsg("LoRa Update", "Checking...");

  // Listen for update requests for longer to catch remote nodes
  uint32_t startTime = millis();
  while (millis() - startTime < 15000) { // Listen for 15 seconds
    String rx;
    int r = radio.receive(rx);
    if (r == RADIOLIB_ERR_NONE) {
      if (rx.startsWith("REQUEST_UPDATE")) {
        Serial.println("Transmitter requested update!");
        oledMsg("LoRa Update", "Request received!");

        // Here you could implement logic to send the actual firmware
        // For now, we'll just acknowledge the request
        radio.transmit("UPDATE_ACK");
        delay(100);

        // You could call sendLoraOtaUpdate() here with the firmware data
        // sendLoraOtaUpdate(firmwareData, firmwareSize);
      }
    }
    delay(100);
  }

  Serial.println("LoRa firmware update trigger complete!");
  oledMsg("LoRa Update", "Complete!");
}

// NEW: Function to store the current firmware for LoRa OTA distribution
static bool storeCurrentFirmware() {
  if (isSender) return false; // Only receivers can store firmware

  // For now, we'll create a simple firmware representation
  // In a real implementation, you'd read the actual firmware from flash
  // or store it during the WiFi OTA process

  // Create a simple firmware header
  const char* firmwareHeader = "LtngDet_FW_v1.0.0";
  size_t headerLen = strlen(firmwareHeader);

  // Copy header to firmware buffer
  memcpy(storedFirmware, firmwareHeader, headerLen);

  // Add some dummy firmware data (in practice, this would be the actual firmware)
  // For demonstration, we'll create a simple firmware representation
  uint8_t dummyFirmware[] = {
    0xAA, 0x55, 0xAA, 0x55, // Magic bytes
    0x01, 0x00, 0x00,       // Version 1.0.0
    0x00, 0x00, 0x00, 0x00, // Checksum placeholder
    0x00, 0x00, 0x00, 0x00  // Size placeholder
  };

  size_t dummyLen = sizeof(dummyFirmware);
  memcpy(storedFirmware + headerLen, dummyFirmware, dummyLen);

  // Set the total firmware size
  storedFirmwareSize = headerLen + dummyLen;
  hasStoredFirmware = true;

  Serial.printf("Firmware stored: %zu bytes (header: %zu, data: %zu)\n",
                storedFirmwareSize, headerLen, dummyLen);

  // TODO: In a real implementation, you would:
  // 1. Read the actual firmware from flash memory
  // 2. Calculate proper checksums
  // 3. Store the complete firmware binary
  // 4. Handle firmware versioning properly

  return true;
}
#endif
