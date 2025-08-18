#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== SIMPLE TEST FIRMWARE ===");
  Serial.println("If you see this, the device is working!");
  Serial.flush();
}

void loop() {
  static unsigned long lastPrint = 0;
  unsigned long now = millis();

  if (now - lastPrint >= 1000) {
    Serial.printf("Alive at %lu ms\n", now);
    Serial.flush();
    lastPrint = now;
  }

  delay(10);
}
