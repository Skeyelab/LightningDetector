#include "display_status.h"
#include <cstdio>

DisplayStatus::DisplayStatus(U8G2 &u8g2) : _u8g2(u8g2) {}

void DisplayStatus::render(const char *l1,
                           const char *l2,
                           const char *l3,
                           const RadioState &state,
                           bool isSender,
                           float lastRSSI,
                           float lastSNR) {
  _u8g2.clearBuffer();
  _u8g2.setFont(u8g2_font_6x10_tr);

  if (l1) _u8g2.drawStr(2, 12, l1);
  if (l2) _u8g2.drawStr(2, 32, l2);
  (void)l3; // intentionally unused to preserve existing layout

  if (!isSender && lastRSSI > -999.0f) {
    char rssiStr[12];
    char snrStr[12];
    std::snprintf(rssiStr, sizeof(rssiStr), "RSSI: %.0f", lastRSSI);
    std::snprintf(snrStr, sizeof(snrStr), "SNR: %.1f", lastSNR);
    _u8g2.drawStr(2, 51, rssiStr);
    _u8g2.drawStr(2, 65, snrStr);
  }

  const std::string settings = state.toSettingsLine();
  _u8g2.drawStr(2, 91, settings.c_str());

  const std::string mode = state.toModeLine(isSender);
  _u8g2.drawStr(2, 105, mode.c_str());
}