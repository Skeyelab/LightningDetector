#include "miniunity/unity.h"
#include "radio_state.h"

void test_radio_state_format() {
  RadioState s{915.0f, 125.0f, 9, 5, 17};
  TEST_ASSERT_EQUAL_STRING("SF9 BW125", s.toSettingsLine().c_str());
  TEST_ASSERT_EQUAL_STRING("TX 915.0MHz", s.toModeLine(true).c_str());
  TEST_ASSERT_EQUAL_STRING("RX 915.0MHz", s.toModeLine(false).c_str());
}