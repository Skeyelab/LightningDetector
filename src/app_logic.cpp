#include "app_logic.h"
#include <cstdio>

ButtonAction classifyPress(uint32_t pressDurationMs) {
  if (pressDurationMs < 100) return ButtonAction::Ignore;
  if (pressDurationMs < 1000) return ButtonAction::CyclePrimary;
  if (pressDurationMs < 3000) return ButtonAction::SecondaryFunction;
  return ButtonAction::ConfigMode;
}

ButtonAction classifyMultipleClicks(uint8_t clickCount, uint32_t totalTimeMs) {
  // Sleep mode requires 5 rapid clicks within 2 seconds
  if (clickCount == 5 && totalTimeMs < 2000) {
    return ButtonAction::SleepMode;
  }
  return ButtonAction::Ignore;
}

int cycleIndex(int currentIndex, int size) {
  if (size <= 0) return 0;
  int next = currentIndex + 1;
  if (next >= size) next = 0;
  return next;
}

std::string formatTxMessage(uint32_t sequenceNumber) {
  char buffer[48];
  snprintf(buffer, sizeof(buffer), "PING seq=%lu", (unsigned long)sequenceNumber);
  return std::string(buffer);
}
