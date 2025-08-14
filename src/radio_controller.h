#pragma once

#include <RadioLib.h>
#include "radio_state.h"

class RadioController {
 public:
  explicit RadioController(SX1262 &radio);

  // Begin radio with provided state and common configuration
  int beginOperational(const RadioState &state, uint8_t syncWord = 0x34);

  // Apply settings non-destructively (without full begin)
  int applySettings(const RadioState &state);

  // Common configuration used after begin
  void configureCommon();

 private:
  SX1262 &_radio;
};