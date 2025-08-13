#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <RadioLib.h>

// Vext power control and OLED reset (Heltec V3)
#define VEXT_PIN 36        // Vext control: LOW = ON
#define OLED_RST_PIN 21    // OLED reset pin

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

SX1262 radio = new Module(PIN_LORA_NSS, PIN_LORA_DIO1, PIN_LORA_RST, PIN_LORA_BUSY);

static void oledMsg(const char* l1, const char* l2 = nullptr, const char* l3 = nullptr) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  if (l1) u8g2.drawStr(0, 12, l1);
  if (l2) u8g2.drawStr(0, 28, l2);
  if (l3) u8g2.drawStr(0, 44, l3);
  u8g2.sendBuffer();
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
}

static void initRadioOrHalt() {
  Serial.println("Initializing LoRa radio...");
  int st = radio.begin(LORA_FREQ_MHZ, LORA_BW_KHZ, LORA_SF, LORA_CR, 0x34, LORA_TX_DBM);
  if (st != RADIOLIB_ERR_NONE) {
    char buf[48]; snprintf(buf, sizeof(buf), "Radio fail %d", st);
    oledMsg("Radio init", buf);
    while (true) { Serial.println(buf); delay(1000); }
  }
  radio.setDio2AsRfSwitch(true);
  radio.setCRC(true);
  char fbuf[32]; snprintf(fbuf, sizeof(fbuf), "%.1f MHz", (double)LORA_FREQ_MHZ);
  oledMsg("LoRa ready", fbuf);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== Talos LoRa + OLED (Heltec V3) ===");

  initDisplay();
  oledMsg("Booting...", "Heltec V3");

#ifdef ROLE_SENDER
  oledMsg("Booting...", "Role: Sender");
  Serial.println("Role: Sender");
#elif defined(ROLE_RECEIVER)
  oledMsg("Booting...", "Role: Receiver");
  Serial.println("Role: Receiver");
#else
  oledMsg("Booting...", "Role: (unset)");
  Serial.println("ERROR: Role not defined");
#endif

  initRadioOrHalt();
}

void loop() {
#ifdef ROLE_SENDER
  static uint32_t seq = 0;
  char msg[48];
  snprintf(msg, sizeof(msg), "PING seq=%lu", (unsigned long)seq++);
  int st = radio.transmit(msg);
  if (st == RADIOLIB_ERR_NONE) {
    Serial.printf("[TX] %s OK\n", msg);
    oledMsg("TX OK", msg);
  } else {
    char e[24]; snprintf(e, sizeof(e), "err %d", st);
    Serial.printf("[TX] %s FAIL %s\n", msg, e);
    oledMsg("TX FAIL", msg, e);
  }
  delay(2000);
#elif defined(ROLE_RECEIVER)
  String rx;
  int st = radio.receive(rx);
  if (st == RADIOLIB_ERR_NONE) {
    float rssi = radio.getRSSI();
    float snr  = radio.getSNR();
    char l2[20]; snprintf(l2, sizeof(l2), "RSSI %.1f", rssi);
    char l3[20]; snprintf(l3, sizeof(l3), "SNR  %.1f", snr);
    Serial.printf("[RX] %s | %s | %s\n", rx.c_str(), l2, l3);
    oledMsg("RX", rx.c_str(), l2);
  } else if (st != RADIOLIB_ERR_RX_TIMEOUT) {
    char e[24]; snprintf(e, sizeof(e), "err %d", st);
    Serial.printf("[RX] FAIL %s\n", e);
    oledMsg("RX FAIL", e);
  }
  delay(50);
#else
  #error "Define ROLE_SENDER or ROLE_RECEIVER"
#endif
}
