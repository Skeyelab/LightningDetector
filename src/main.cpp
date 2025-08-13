#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>

// Vext power control and OLED reset (Heltec V3)
#define VEXT_PIN 36        // Vext control: LOW = ON
#define OLED_RST_PIN 21    // OLED reset pin

// SSD1306 128x64 OLED over HW I2C; pins set via Wire.begin(17,18)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=== Heltec V3 OLED Test ===");
  Serial.println("Starting up...");

  // Power OLED via Vext and reset it
  pinMode(VEXT_PIN, OUTPUT);
  digitalWrite(VEXT_PIN, LOW);   // enable Vext
  delay(50);

  pinMode(OLED_RST_PIN, OUTPUT);
  digitalWrite(OLED_RST_PIN, LOW);
  delay(20);
  digitalWrite(OLED_RST_PIN, HIGH);
  delay(50);

  // Initialize I2C with timeout
  Serial.println("Initializing I2C...");
  Wire.begin(17, 18); // SDA=17, SCL=18 - correct Heltec V3 pins
  Wire.setTimeOut(1000); // 1 second timeout
  Wire.setClock(100000); // 100 kHz for stability
  delay(100);

  // Force SSD1306 default address 0x3C
  u8g2.setI2CAddress(0x3C << 1);

  Serial.println("I2C initialized, trying OLED...");
  Serial.println("Initializing OLED display (SSD1306)...");

  if (!u8g2.begin()) {
    Serial.println("SSD1306 init failed at 0x3C, trying 0x3D...");
    u8g2.setI2CAddress(0x3D << 1);
    if (!u8g2.begin()) {
      Serial.println("SSD1306 init failed at both addresses!");
      while (true) {
        Serial.println("OLED failed - halting");
        delay(1000);
      }
    }
  }

  Serial.println("SSD1306 init OK");
  u8g2.setPowerSave(0);
  u8g2.setContrast(255);

  // Test: full white box
  u8g2.clearBuffer();
  u8g2.drawBox(0, 0, 128, 64);
  u8g2.sendBuffer();
  delay(500);

  // Test: normal text
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(0, 12, "Heltec V3 Test");
  u8g2.drawStr(0, 28, "OLED Working?!");
  u8g2.drawStr(0, 44, "R0 orientation");
  u8g2.sendBuffer();
  Serial.println("Display test complete (R0)");

  // Test: flip mode toggle (try different scan direction)
  u8g2.setFlipMode(1);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(0, 12, "Flip ON");
  u8g2.sendBuffer();
  delay(300);
  u8g2.setFlipMode(0);
  Serial.println("Flip mode toggled");
}

void loop() {
  static bool state = false;
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(0, 12, "Heltec V3 Test");
  u8g2.drawStr(0, 28, state ? "ON" : "OFF");
  u8g2.sendBuffer();
  state = !state;
  delay(1000);
}
