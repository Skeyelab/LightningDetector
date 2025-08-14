#pragma once

#include <string>

struct RadioConfig {
  float frequencyMHz;
  float bandwidthKHz;
  int spreadingFactor;
  int codingRate;
  int txPowerDbm;

  std::string toControlMessage() const;
  static bool parseControlMessage(const std::string &message, RadioConfig &outConfig);
};