#include <iostream>
#include <cassert>
#include <cstring>
#include "../../src/app_logic.h"
#include "../../src/hardware/hardware_abstraction.h"

using namespace HardwareAbstraction;

void test_button_classification() {
    std::cout << "Testing button classification..." << std::endl;

    // Test button press classification
    assert(classifyPress(0) == ButtonAction::Ignore);
    assert(classifyPress(100) == ButtonAction::CycleSF);
    assert(classifyPress(1000) == ButtonAction::CycleBW);
    assert(classifyPress(3000) == ButtonAction::SleepMode);

    std::cout << "  ✓ Button classification passed" << std::endl;
}

void test_cycle_index_function() {
    std::cout << "Testing cycle index function..." << std::endl;

    // Test cycle index functionality
    assert(cycleIndex(0, 3) == 1);
    assert(cycleIndex(1, 3) == 2);
    assert(cycleIndex(2, 3) == 0);
    assert(cycleIndex(0, 1) == 0);

    std::cout << "  ✓ Cycle index function passed" << std::endl;
}

void test_format_tx_message() {
    std::cout << "Testing TX message formatting..." << std::endl;

    // Test message formatting
    std::string msg1 = formatTxMessage(123);
    assert(msg1 == "PING seq=123");

    std::string msg2 = formatTxMessage(456);
    assert(msg2 == "PING seq=456");

    std::cout << "  ✓ TX message formatting passed" << std::endl;
}

void test_hardware_integration() {
    std::cout << "Testing hardware integration..." << std::endl;

    // Test hardware abstraction initialization
    assert(initialize() == Result::SUCCESS);
    assert(isInitialized());

    // Test basic GPIO functionality
    assert(GPIO::pinMode(2, GPIO::Mode::MODE_OUTPUT) == Result::SUCCESS);
    assert(GPIO::digitalWrite(2, GPIO::Level::LEVEL_HIGH) == Result::SUCCESS);

    // Cleanup
    deinitialize();

    std::cout << "  ✓ Hardware integration passed" << std::endl;
}

int main() {
    std::cout << "Running integration tests..." << std::endl;

    try {
        test_button_classification();
        test_cycle_index_function();
        test_format_tx_message();
        test_hardware_integration();

        std::cout << "\n✅ All integration tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}
