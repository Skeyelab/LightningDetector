#include "config.h"

#include <cstdio>

std::string RadioConfig::toControlMessage() const {
  char buffer[64];
  snprintf(buffer, sizeof(buffer), "CFG F=%.1f BW=%.0f SF=%d CR=%d TX=%d",
          frequencyMHz, bandwidthKHz, spreadingFactor, codingRate, txPowerDbm);
  return std::string(buffer);
}

bool RadioConfig::parseControlMessage(const std::string &message, RadioConfig &outConfig) {
  if (message.rfind("CFG ", 0) != 0) {
    return false;
  }
  float f = 0.0f;
  float bw = 0.0f;
  int sf = 0;
  int cr = 0;
  int tx = 0;
  int parsed = std::sscanf(message.c_str(), "CFG F=%f BW=%f SF=%d CR=%d TX=%d", &f, &bw, &sf, &cr, &tx);
  if (parsed != 5) {
    return false;
  }
  outConfig.frequencyMHz = f;
  outConfig.bandwidthKHz = bw;
  outConfig.spreadingFactor = sf;
  outConfig.codingRate = cr;
  outConfig.txPowerDbm = tx;
  return true;
}