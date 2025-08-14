#pragma once

#ifndef RADIOLIB_ERR_NONE
#define RADIOLIB_ERR_NONE (0)
#endif

class SX1262 {
 public:
  template<typename... Args>
  explicit SX1262(Args&&...) {}

  int begin(float, float, int, int, unsigned char, int) { return RADIOLIB_ERR_NONE; }
  int setFrequency(float) { return RADIOLIB_ERR_NONE; }
  int setBandwidth(float) { return RADIOLIB_ERR_NONE; }
  int setSpreadingFactor(int) { return RADIOLIB_ERR_NONE; }
  int setCodingRate(int) { return RADIOLIB_ERR_NONE; }
  int setOutputPower(int) { return RADIOLIB_ERR_NONE; }
  void setDio2AsRfSwitch(bool) {}
  void setCRC(bool) {}
};