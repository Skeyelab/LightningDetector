#include "app_logic.h"
#include <cstdio>
#include "sensors/gps_sensor.h"

ButtonAction classifyPress(uint32_t pressDurationMs) {
  if (pressDurationMs < 100) return ButtonAction::Ignore;
  if (pressDurationMs < 1000) return ButtonAction::CycleSF;      // Short press: cycle SF
  if (pressDurationMs < 3000) return ButtonAction::CycleBW;      // Medium press: cycle BW
  return ButtonAction::SleepMode;                                // Long press: sleep mode
}

int cycleIndex(int currentIndex, int size) {
  if (size <= 0) return 0;
  int next = currentIndex + 1;
  if (next >= size) next = 0;
  return next;
}

std::string formatTxMessage(uint32_t sequenceNumber) {
  char buffer[128];
  if (GPS::hasGPSFix()) {
    const auto& gpsData = GPS::getGPSData();
    snprintf(buffer, sizeof(buffer),
             "PING seq=%lu lat=%.6f lon=%.6f alt=%.1f",
             (unsigned long)sequenceNumber,
             gpsData.latitude, gpsData.longitude, gpsData.altitude);
  } else {
    snprintf(buffer, sizeof(buffer),
             "PING seq=%lu gps=NO_FIX",
             (unsigned long)sequenceNumber);
  }
  return std::string(buffer);
}
