// Keep this test minimal and host-friendly
#include <unity.h>
#include "app_logic.h"

void test_classifyPress() {
  TEST_ASSERT_EQUAL_INT((int)ButtonAction::Ignore, (int)classifyPress(50));
  TEST_ASSERT_EQUAL_INT((int)ButtonAction::ToggleMode, (int)classifyPress(150));
  TEST_ASSERT_EQUAL_INT((int)ButtonAction::ToggleMode, (int)classifyPress(999));
  TEST_ASSERT_EQUAL_INT((int)ButtonAction::CycleSF, (int)classifyPress(1500));
  TEST_ASSERT_EQUAL_INT((int)ButtonAction::CycleSF, (int)classifyPress(2999));
  TEST_ASSERT_EQUAL_INT((int)ButtonAction::CycleBW, (int)classifyPress(3000));
  TEST_ASSERT_EQUAL_INT((int)ButtonAction::CycleBW, (int)classifyPress(10000));
}

void test_cycleIndex() {
  TEST_ASSERT_EQUAL_INT(1, cycleIndex(0, 3));
  TEST_ASSERT_EQUAL_INT(2, cycleIndex(1, 3));
  TEST_ASSERT_EQUAL_INT(0, cycleIndex(2, 3));
  TEST_ASSERT_EQUAL_INT(0, cycleIndex(5, 0));
}

void test_formatTxMessage() {
  TEST_ASSERT_EQUAL_STRING("PING seq=0", formatTxMessage(0).c_str());
  TEST_ASSERT_EQUAL_STRING("PING seq=42", formatTxMessage(42).c_str());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_classifyPress);
  RUN_TEST(test_cycleIndex);
  RUN_TEST(test_formatTxMessage);
  return UNITY_END();
}
