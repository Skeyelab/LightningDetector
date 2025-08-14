#pragma once

#include <string>

struct RadioState {
  float frequencyMHz;
  float bandwidthKHz;
  int spreadingFactor;
  int codingRate;
  int txPowerDbm;

  std::string toSettingsLine() const;    // e.g., "SF9 BW125"
  std::string toModeLine(bool isSender) const; // e.g., "TX 915.0MHz" or "RX 915.0MHz"
};