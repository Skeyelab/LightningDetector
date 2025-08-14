#include "miniunity/unity.h"
#include "display_status.h"

void test_display_status_renders_lines_and_footer() {
  U8G2 u;
  DisplayStatus d(u);
  RadioState s{915.0f, 125.0f, 9, 5, 17};
  d.render("Booting...", "Heltec V3", nullptr, s, true, -999.0f, -999.0f);
  bool hasBoot = false, hasHeltec = false, hasSettings = false, hasMode = false;
  for (const auto &op : u.ops) {
    if (op.y == 12 && op.text == "Booting...") hasBoot = true;
    if (op.y == 32 && op.text == "Heltec V3") hasHeltec = true;
    if (op.y == 91 && op.text == "SF9 BW125") hasSettings = true;
    if (op.y == 105 && op.text == "TX 915.0MHz") hasMode = true;
  }
  TEST_ASSERT_TRUE(hasBoot && hasHeltec && hasSettings && hasMode);
}