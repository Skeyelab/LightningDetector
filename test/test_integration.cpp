// Integration tests for cross-module functionality
// Tests interactions between app_logic and other components
#include <unity.h>
#include <vector>
#include <string>
#include "../src/app_logic.h"

struct ButtonTest {
  std::string name;
  uint32_t duration;
  ButtonAction expectedAction;
  std::string description;
};

void test_button_classification() {
  std::vector<ButtonTest> tests = {
    {"Quick tap", 50, ButtonAction::Ignore, "Should ignore accidental touches"},
    {"Short press", 200, ButtonAction::CycleSF, "Should cycle Spreading Factor"},
    {"Medium press", 1500, ButtonAction::CycleBW, "Should cycle Bandwidth"},
    {"Long press", 5000, ButtonAction::SleepMode, "Should enter sleep mode"},
    {"Boundary short", 99, ButtonAction::Ignore, "Edge case for ignore threshold"},
    {"Boundary primary", 100, ButtonAction::CycleSF, "Edge case for SF function threshold"},
    {"Boundary secondary", 1000, ButtonAction::CycleBW, "Edge case for BW function threshold"},
    {"Boundary config", 3000, ButtonAction::SleepMode, "Edge case for sleep mode threshold"}
  };

  for (const auto& test : tests) {
    ButtonAction result = classifyPress(test.duration);
    TEST_ASSERT_EQUAL_MESSAGE(
      (int)test.expectedAction,
      (int)result,
      (test.name + ": " + test.description).c_str()
    );
  }
}

void test_button_action_handling() {
  // Test all button actions are handled
  ButtonAction actions[] = {
    ButtonAction::Ignore,
    ButtonAction::CycleSF,
    ButtonAction::CycleBW,
    ButtonAction::SleepMode
  };

  for (ButtonAction action : actions) {
    // This test verifies that all button actions are recognized
    // The actual handling is done in main.cpp
    TEST_ASSERT_TRUE((int)action >= 0 && (int)action <= 3);
  }
}

void test_cycle_index_function() {
  // Test normal cycling
  TEST_ASSERT_EQUAL(1, cycleIndex(0, 3));
  TEST_ASSERT_EQUAL(2, cycleIndex(1, 3));
  TEST_ASSERT_EQUAL(0, cycleIndex(2, 3));

  // Test edge cases
  TEST_ASSERT_EQUAL(0, cycleIndex(0, 1));  // size 1
  TEST_ASSERT_EQUAL(0, cycleIndex(1, 1));  // size 1, index 1
  TEST_ASSERT_EQUAL(0, cycleIndex(5, 1));  // index > size, size 1
}

void test_format_tx_message() {
  // Test normal cases
  TEST_ASSERT_EQUAL_STRING("PING seq=0", formatTxMessage(0).c_str());
  TEST_ASSERT_EQUAL_STRING("PING seq=42", formatTxMessage(42).c_str());
  TEST_ASSERT_EQUAL_STRING("PING seq=1", formatTxMessage(1).c_str());

  // Test large sequence numbers
  TEST_ASSERT_EQUAL_STRING("PING seq=1000000", formatTxMessage(1000000).c_str());
}

void test_sleep_mode_trigger() {
  // Test that long press triggers sleep mode
  ButtonAction action = classifyPress(5000);
  TEST_ASSERT_EQUAL((int)ButtonAction::SleepMode, (int)action);
}

void test_ignore_short_presses() {
  // Test that very short presses are ignored
  ButtonAction action = classifyPress(50);
  TEST_ASSERT_EQUAL((int)ButtonAction::Ignore, (int)action);
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_button_classification);
  RUN_TEST(test_button_action_handling);
  RUN_TEST(test_cycle_index_function);
  RUN_TEST(test_format_tx_message);
  RUN_TEST(test_sleep_mode_trigger);
  RUN_TEST(test_ignore_short_presses);
  return UNITY_END();
}
