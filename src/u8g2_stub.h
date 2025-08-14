#pragma once

#include <string>
#include <vector>

// Placeholder font identifiers to satisfy code using U8G2
static const void* u8g2_font_6x10_tr = reinterpret_cast<const void*>(0x1);
static const void* u8g2_font_5x7_tr  = reinterpret_cast<const void*>(0x2);

struct U8G2_DrawOp {
  int x;
  int y;
  std::string text;
};

class U8G2 {
 public:
  void clearBuffer() {}
  void setFont(const void*) {}
  void drawStr(int x, int y, const char* s) { ops.push_back({x, y, s ? std::string(s) : std::string()}); }
  void setFontRefHeightExtendedText() {}
  void sendBuffer() {}

  std::vector<U8G2_DrawOp> ops;
};