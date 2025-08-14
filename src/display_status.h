#pragma once

#ifndef UNIT_TEST
#include <U8g2lib.h>
#else
#include "u8g2_stub.h"
#endif
#include <string>
#include "radio_state.h"

class DisplayStatus {
 public:
  explicit DisplayStatus(U8G2 &u8g2);
  void render(const char *l1,
              const char *l2,
              const char *l3,
              const RadioState &state,
              bool isSender,
              float lastRSSI,
              float lastSNR);
 private:
  U8G2 &_u8g2;
};