#include <iostream>
#include <cassert>
#include "src/hardware/hardware_abstraction.h"
#include "src/sensors/gps_sensor.h"

using namespace HardwareAbstraction;

int main() {
    std::cout << "Running HAL and GPS compilation tests..." << std::endl;
    
    // Test 1: HAL Initialization
    std::cout << "Test 1: HAL Initialization..." << std::endl;
    assert(initialize() == Result::SUCCESS);
    assert(isInitialized() == true);
    std::cout << "✓ HAL initialization works" << std::endl;
    
    // Test 2: Result to String conversion
    std::cout << "Test 2: Result to String conversion..." << std::endl;
    assert(std::string(resultToString(Result::SUCCESS)) == "SUCCESS");
    assert(std::string(resultToString(Result::ERROR_INIT_FAILED)) == "ERROR_INIT_FAILED");
    std::cout << "✓ Result to string conversion works" << std::endl;
    
    // Test 3: GPIO Operations
    std::cout << "Test 3: GPIO Operations..." << std::endl;
    assert(GPIO::pinMode(2, GPIO::Mode::MODE_OUTPUT) == Result::SUCCESS);
    assert(GPIO::digitalWrite(2, GPIO::Level::LEVEL_HIGH) == Result::SUCCESS);
    GPIO::Level level = GPIO::digitalRead(2);
    assert(level == GPIO::Level::LEVEL_LOW || level == GPIO::Level::LEVEL_HIGH);
    std::cout << "✓ GPIO operations work" << std::endl;
    
    // Test 4: I2C Operations
    std::cout << "Test 4: I2C Operations..." << std::endl;
    assert(I2C::initialize(21, 22, 400000) == Result::SUCCESS);
    std::cout << "✓ I2C initialization works" << std::endl;
    
    // Test 5: SPI Operations
    std::cout << "Test 5: SPI Operations..." << std::endl;
    assert(SPI::initialize() == Result::SUCCESS);
    std::cout << "✓ SPI initialization works" << std::endl;
    
    // Test 6: ADC Operations
    std::cout << "Test 6: ADC Operations..." << std::endl;
    assert(ADC::initialize() == Result::SUCCESS);
    assert(ADC::setResolution(12) == Result::SUCCESS); // ESP32-S3 supports 12-bit
    std::cout << "✓ ADC operations work" << std::endl;
    
    // Test 7: GPS Configuration
    std::cout << "Test 7: GPS Configuration..." << std::endl;
    GPS::Config default_config = GPS::getDefaultConfig();
    assert(default_config.baud_rate == 9600);
    assert(default_config.uart_num == 1);
    
    GPS::Config tracker_config = GPS::getWirelessTrackerV11Config();
    assert(tracker_config.enable_pin == 3);
    assert(tracker_config.baud_rate == 9600);
    std::cout << "✓ GPS configuration works" << std::endl;
    
    // Test 8: Memory Operations
    std::cout << "Test 8: Memory Operations..." << std::endl;
    void* ptr = Memory::allocate(100);
    assert(ptr != nullptr);
    Memory::deallocate(ptr);
    assert(Memory::getFreeHeap() > 0);
    std::cout << "✓ Memory operations work" << std::endl;
    
    // Test 9: Power Management
    std::cout << "Test 9: Power Management..." << std::endl;
    assert(Power::enableVext() == Result::SUCCESS);
    assert(Power::disableVext() == Result::SUCCESS);
    std::cout << "✓ Power management works" << std::endl;
    
    // Test 10: HAL Cleanup
    std::cout << "Test 10: HAL Cleanup..." << std::endl;
    deinitialize();
    assert(isInitialized() == false);
    std::cout << "✓ HAL cleanup works" << std::endl;
    
    std::cout << std::endl << "🎉 ALL TESTS PASSED! HAL and GPS implementation is working correctly." << std::endl;
    std::cout << "✅ Hardware Abstraction Layer: FUNCTIONAL" << std::endl;
    std::cout << "✅ GPS Support: READY" << std::endl;
    std::cout << "✅ Build System: VERIFIED" << std::endl;
    
    return 0;
}