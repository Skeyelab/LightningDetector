#pragma once

#include <stdint.h>
#include <string>

enum class ButtonAction {
  Ignore,
  CycleSF,      // Short press: cycle through Spreading Factor values
  CycleBW,      // Medium press: cycle through Bandwidth values
  SleepMode     // Long press: enter sleep mode
};

ButtonAction classifyPress(uint32_t pressDurationMs);

int cycleIndex(int currentIndex, int size);

std::string formatTxMessage(uint32_t sequenceNumber);
