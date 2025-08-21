#include <iostream>
#include <cassert>
#include "../src/app_logic.h"

void test_classifyPress() {
  std::cout << "Testing classifyPress..." << std::endl;

  // Test ignore cases
  assert(classifyPress(0) == ButtonAction::Ignore);
  assert(classifyPress(50) == ButtonAction::Ignore);
  assert(classifyPress(99) == ButtonAction::Ignore);
  std::cout << "  ✓ Ignore cases passed" << std::endl;

  // Test CyclePreset (up to 6 seconds)
  assert(classifyPress(100) == ButtonAction::CyclePreset);
  assert(classifyPress(1000) == ButtonAction::CyclePreset);
  assert(classifyPress(3000) == ButtonAction::CyclePreset);
  assert(classifyPress(5999) == ButtonAction::CyclePreset);
  std::cout << "  ✓ CyclePreset cases passed" << std::endl;

  // Test SleepMode (6+ seconds)
  assert(classifyPress(6000) == ButtonAction::SleepMode);
  assert(classifyPress(10000) == ButtonAction::SleepMode);
  std::cout << "  ✓ SleepMode cases passed" << std::endl;
}

void test_cycleIndex() {
  std::cout << "Testing cycleIndex..." << std::endl;

  assert(cycleIndex(0, 1) == 0);
  assert(cycleIndex(1, 1) == 0);
  assert(cycleIndex(0, 2) == 1);
  assert(cycleIndex(1, 2) == 0);
  assert(cycleIndex(2, 2) == 0);
  std::cout << "  ✓ cycleIndex cases passed" << std::endl;
}

void test_formatTxMessage() {
  std::cout << "Testing formatTxMessage..." << std::endl;

  std::string msg = formatTxMessage(123);
  assert(msg == "PING seq=123");
  std::cout << "  ✓ formatTxMessage case passed" << std::endl;
}

int main() {
  std::cout << "Running app_logic tests..." << std::endl;

  try {
    test_classifyPress();
    test_cycleIndex();
    test_formatTxMessage();

    std::cout << "\n✅ All tests passed!" << std::endl;
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "\n❌ Test failed with unknown exception" << std::endl;
    return 1;
  }
}
