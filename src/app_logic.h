#pragma once

#include <stdint.h>
#include <string>

enum class ButtonAction {
  Ignore,
  CyclePrimary,      // Primary function: SF for sender, display mode for receiver
  SecondaryFunction, // Secondary function: config broadcast/request
  ConfigMode,        // Configuration/advanced settings
  SleepMode         // Low power sleep mode (5 rapid clicks)
};

ButtonAction classifyPress(uint32_t pressDurationMs);

// New function to detect rapid multiple clicks for sleep mode
ButtonAction classifyMultipleClicks(uint8_t clickCount, uint32_t totalTimeMs);

int cycleIndex(int currentIndex, int size);

std::string formatTxMessage(uint32_t sequenceNumber);
