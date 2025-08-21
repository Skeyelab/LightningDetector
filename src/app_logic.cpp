#include "app_logic.h"
#include <cstdio>

ButtonAction classifyPress(uint32_t pressDurationMs) {
  if (pressDurationMs < 100) return ButtonAction::Ignore;
  if (pressDurationMs < 6000) return ButtonAction::CyclePreset;  // Up to 6s: cycle presets
  return ButtonAction::SleepMode;                                // Long press: sleep mode
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
