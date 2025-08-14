#include "unity_compat.h"
#include "radio_controller.h"

void test_radio_controller_header_compiles() {
  // Create a stub radio and controller
  SX1262 radio;
  RadioController ctrl(radio);
  RadioState s{915.0f, 125.0f, 9, 5, 17};
  TEST_ASSERT_EQUAL_INT(0, ctrl.applySettings(s));
}