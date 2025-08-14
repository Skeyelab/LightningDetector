#include "radio_controller.h"

RadioController::RadioController(SX1262 &radio) : _radio(radio) {}

int RadioController::beginOperational(const RadioState &state, uint8_t syncWord) {
  int st = _radio.begin(state.frequencyMHz, state.bandwidthKHz, state.spreadingFactor, state.codingRate, syncWord, state.txPowerDbm);
  if (st == RADIOLIB_ERR_NONE) {
    configureCommon();
  }
  return st;
}

int RadioController::applySettings(const RadioState &state) {
  int st = _radio.setFrequency(state.frequencyMHz);
  if (st == RADIOLIB_ERR_NONE) {
    st = _radio.setBandwidth(state.bandwidthKHz);
  }
  if (st == RADIOLIB_ERR_NONE) {
    st = _radio.setSpreadingFactor(state.spreadingFactor);
  }
  if (st == RADIOLIB_ERR_NONE) {
    st = _radio.setCodingRate(state.codingRate);
  }
  if (st == RADIOLIB_ERR_NONE) {
    st = _radio.setOutputPower(state.txPowerDbm);
  }
  return st;
}

void RadioController::configureCommon() {
  _radio.setDio2AsRfSwitch(true);
  _radio.setCRC(true);
}