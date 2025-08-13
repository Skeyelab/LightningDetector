#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <RadioLib.h>
#include <Preferences.h>

// Vext power control and OLED reset (Heltec V3)
#define VEXT_PIN 36        // Vext control: LOW = ON
#define OLED_RST_PIN 21    // OLED reset pin
#define BUTTON_PIN 0       // BOOT button on GPIO0 (active LOW)

// SSD1306 128x64 OLED over HW I2C; pins set via Wire.begin(17,18)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

#ifndef PIN_LORA_NSS
  #define PIN_LORA_NSS   8
#endif
#ifndef PIN_LORA_DIO1
  #define PIN_LORA_DIO1  14
#endif
#ifndef PIN_LORA_RST
  #define PIN_LORA_RST   12
#endif
#ifndef PIN_LORA_BUSY
  #define PIN_LORA_BUSY  13
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

SX1262 radio = new Module(PIN_LORA_NSS, PIN_LORA_DIO1, PIN_LORA_RST, PIN_LORA_BUSY);
static Preferences prefs;

static bool isSender = true;
static uint32_t seq = 0;
static uint32_t lastButtonMs = 0;
static int lastButtonState = HIGH;
static uint32_t buttonPressMs = 0;
static bool buttonPressed = false;

// LoRa parameters that can be changed at runtime
static float currentFreq = LORA_FREQ_MHZ;
static float currentBW = LORA_BW_KHZ;
static int currentSF = LORA_SF;
static int currentCR = LORA_CR;
static int currentTxPower = LORA_TX_DBM;

// Signal quality tracking
static float lastRSSI = -999.0;
static float lastSNR = -999.0;
static uint32_t lastPacketTime = 0;
static uint32_t packetCount = 0;
static uint32_t errorCount = 0;

// Available values for cycling
static const float bwValues[] = {125.0, 250.0, 500.0};
static const int sfValues[] = {7, 8, 9, 10, 11, 12};
static const int txPowerValues[] = {10, 14, 17, 20, 22};
static int currentBwIndex = 0;
static int currentSfIndex = 2; // Start with SF9
static int currentTxIndex = 2; // Start with 17 dBm

// Config broadcast state (sender)
static bool pendingConfigBroadcast = false;
static float pendingFreq = 0;
static float pendingBW = 0;
static int pendingSF = 0;
static int pendingCR = 0;
static int pendingTxPower = 0;
static uint32_t cfgLastTxMs = 0;
static int cfgRemaining = 0;

// Persistence helpers
static void savePersistedSettings();
static void savePersistedRole();
static void loadPersistedSettingsAndRole();
static void computeIndicesFromCurrent();
static void broadcastConfigOnControlChannel(uint8_t times = 8, uint32_t intervalMs = 300);
static void tryReceiveConfigOnControlChannel(uint32_t durationMs = 4000);

static void oledMsg(const char* l1, const char* l2 = nullptr, const char* l3 = nullptr) {
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

  // Bottom section - settings
  char settings[32];
  snprintf(settings, sizeof(settings), "SF%d BW%.0f", currentSF, currentBW);
  u8g2.drawStr(2, 91, settings);

  char modeStr[16];
  snprintf(modeStr, sizeof(modeStr), "%s %.1fMHz", isSender ? "TX" : "RX", currentFreq);
  u8g2.drawStr(2, 105, modeStr);

  u8g2.sendBuffer();
}

static void oledRole() {
  oledMsg("Mode", isSender ? "Sender" : "Receiver");
}

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
  prefs.begin("talos", false);
  prefs.putFloat("freq", currentFreq);
  prefs.putFloat("bw", currentBW);
  prefs.putInt("sf", currentSF);
  prefs.putInt("cr", currentCR);
  prefs.putInt("tx", currentTxPower);
  prefs.end();
}

static void savePersistedRole() {
  prefs.begin("talos", false);
  prefs.putBool("sender", isSender);
  prefs.end();
}

static void loadPersistedSettingsAndRole() {
  prefs.begin("talos", true);
  bool haveFreq = prefs.isKey("freq");
  bool haveBW = prefs.isKey("bw");
  bool haveSF = prefs.isKey("sf");
  bool haveCR = prefs.isKey("cr");
  bool haveTX = prefs.isKey("tx");
  bool haveRole = prefs.isKey("sender");

  if (haveFreq) currentFreq = prefs.getFloat("freq", currentFreq);
  if (haveBW) currentBW = prefs.getFloat("bw", currentBW);
  if (haveSF) currentSF = prefs.getInt("sf", currentSF);
  if (haveCR) currentCR = prefs.getInt("cr", currentCR);
  if (haveTX) currentTxPower = prefs.getInt("tx", currentTxPower);
  if (haveRole) isSender = prefs.getBool("sender", isSender);
  prefs.end();
}

static void initDisplay() {
  // Power OLED via Vext and reset it
  pinMode(VEXT_PIN, OUTPUT);
  digitalWrite(VEXT_PIN, LOW);   // enable Vext
  delay(50);

  pinMode(OLED_RST_PIN, OUTPUT);
  digitalWrite(OLED_RST_PIN, LOW);
  delay(20);
  digitalWrite(OLED_RST_PIN, HIGH);
  delay(50);

  // I2C
  Wire.begin(17, 18);
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

static void updateButton() {
  int s = digitalRead(BUTTON_PIN);
  uint32_t now = millis();

  // Button press detection
  if (lastButtonState == HIGH && s == LOW) {
    buttonPressed = true;
    buttonPressMs = now;
  }

  // Button release detection
  if (lastButtonState == LOW && s == HIGH && buttonPressed) {
    buttonPressed = false;
    uint32_t pressDuration = now - buttonPressMs;

    if (pressDuration < 100) {
      // Very short press - ignore (debounce)
    } else if (pressDuration < 1000) {
      // Short press - toggle mode
      isSender = !isSender;
      seq = 0;
      savePersistedRole();
      oledRole();
      Serial.printf("Switched mode -> %s\n", isSender ? "Sender" : "Receiver");
    } else if (pressDuration < 3000) {
      // Medium press - cycle SF
      if (isSender) {
        int nextIndex = (currentSfIndex + 1) % (sizeof(sfValues) / sizeof(sfValues[0]));
        int nextSF = sfValues[nextIndex];
        startConfigBroadcast(currentFreq, currentBW, nextSF, currentCR, currentTxPower);
        Serial.printf("SF change requested -> %d (broadcasting to receiver)\n", nextSF);
      } else {
        currentSfIndex = (currentSfIndex + 1) % (sizeof(sfValues) / sizeof(sfValues[0]));
        currentSF = sfValues[currentSfIndex];
        updateRadioSettings();
        savePersistedSettings();
        Serial.printf("SF changed to %d\n", currentSF);
      }
    } else {
      // Long press - cycle BW
      if (isSender) {
        int nextIndex = (currentBwIndex + 1) % (sizeof(bwValues) / sizeof(bwValues[0]));
        float nextBW = bwValues[nextIndex];
        startConfigBroadcast(currentFreq, nextBW, currentSF, currentCR, currentTxPower);
        Serial.printf("BW change requested -> %.0f kHz (broadcasting to receiver)\n", nextBW);
      } else {
        currentBwIndex = (currentBwIndex + 1) % (sizeof(bwValues) / sizeof(bwValues[0]));
        currentBW = bwValues[currentBwIndex];
        updateRadioSettings();
        savePersistedSettings();
        Serial.printf("BW changed to %.0f kHz\n", currentBW);
      }
    }

    lastButtonMs = now;
  }

  lastButtonState = s;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== Talos LoRa + OLED (Heltec V3) ===");

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize current values
  currentFreq = LORA_FREQ_MHZ;
  currentBW = LORA_BW_KHZ;
  currentSF = LORA_SF;
  currentCR = LORA_CR;
  currentTxPower = LORA_TX_DBM;

  // initial role from build flags
#ifdef ROLE_SENDER
  isSender = true;
#elif defined(ROLE_RECEIVER)
  isSender = false;
#else
  isSender = true;
#endif

  // Load persisted settings/role (overrides defaults when present)
  loadPersistedSettingsAndRole();
  computeIndicesFromCurrent();

  initDisplay();
  oledMsg("Booting...", "Heltec V3");
  oledRole();

  initRadioOrHalt();

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

  // Check button more frequently
  updateButton();

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
          // Show ping on two lines
          unsigned long usedSeq = (unsigned long)(seq - 1);
          char seqLine[20]; snprintf(seqLine, sizeof(seqLine), "seq=%lu", usedSeq);
          oledMsg("PING", seqLine);
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
        } else {
          char l2[20]; snprintf(l2, sizeof(l2), "RSSI %.1f", rssi);
          if (rx.startsWith("PING ")) {
            // Extract seq part from message "PING seq=NNN"
            const char* seqPtr = strstr(rx.c_str(), "seq=");
            const char* seqStr = seqPtr ? seqPtr : rx.c_str();
            oledMsg("PING", seqStr);
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

  // Small delay to prevent overwhelming the system, but keep button responsive
  delay(10);
}
