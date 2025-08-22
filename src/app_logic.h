#pragma once

#include <stdint.h>
#include <string>

enum class ButtonAction {
  Ignore,
  CyclePreset,  // Short press: cycle through LoRa presets
  SleepMode     // Long press: enter sleep mode
};

ButtonAction classifyPress(uint32_t pressDurationMs);

int cycleIndex(int currentIndex, int size);

std::string formatTxMessage(uint32_t sequenceNumber);
