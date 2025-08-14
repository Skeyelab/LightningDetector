#include "radio_state.h"

#include <cstdio>

std::string RadioState::toSettingsLine() const {
  char buf[32];
  std::snprintf(buf, sizeof(buf), "SF%d BW%.0f", spreadingFactor, bandwidthKHz);
  return std::string(buf);
}

std::string RadioState::toModeLine(bool isSender) const {
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%s %.1fMHz", isSender ? "TX" : "RX", frequencyMHz);
  return std::string(buf);
}