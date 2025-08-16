#include <unity.h>
#include "../src/sensors/lightning_sensor.h"
#include "../src/hardware/hardware_abstraction.h"

using namespace Sensors;
using namespace SensorSystem;

// Test fixture
static LightningSensor* sensor = nullptr;
static bool callbackCalled = false;
static Reading lastReading;
static uint32_t lastErrorCode = 0;
static State lastState = State::UNINITIALIZED;

// Test callbacks
void testReadingCallback(const Reading& reading) {
    callbackCalled = true;
    lastReading = reading;
}

void testErrorCallback(const char* sensorId, uint32_t errorCode) {
    lastErrorCode = errorCode;
}

void testStateChangeCallback(const char* sensorId, State state) {
    lastState = state;
}

void setUp(void) {
    // Initialize hardware abstraction
    HardwareAbstraction::initialize();
    
    // Create sensor instance
    sensor = new LightningSensor();
    
    // Reset test state
    callbackCalled = false;
    lastErrorCode = 0;
    lastState = State::UNINITIALIZED;
    memset(&lastReading, 0, sizeof(lastReading));
}

void tearDown(void) {
    if (sensor) {
        sensor->deinitialize();
        delete sensor;
        sensor = nullptr;
    }
}

// Test basic sensor properties
void test_sensor_properties(void) {
    TEST_ASSERT_EQUAL_STRING("AS3935", sensor->getId());
    TEST_ASSERT_EQUAL_STRING("Lightning Sensor", sensor->getName());
    TEST_ASSERT_EQUAL(State::UNINITIALIZED, sensor->getState());
    
    uint16_t capabilities = sensor->getCapabilities();
    TEST_ASSERT_TRUE(capabilities & static_cast<uint16_t>(Capability::INTERRUPT_CAPABLE));
    TEST_ASSERT_TRUE(capabilities & static_cast<uint16_t>(Capability::CONFIGURABLE));
    TEST_ASSERT_TRUE(capabilities & static_cast<uint16_t>(Capability::SELF_TEST));
    TEST_ASSERT_TRUE(capabilities & static_cast<uint16_t>(Capability::CALIBRATION));
    TEST_ASSERT_TRUE(capabilities & static_cast<uint16_t>(Capability::POWER_MANAGEMENT));
    TEST_ASSERT_TRUE(capabilities & static_cast<uint16_t>(Capability::THRESHOLD_DETECTION));
}

// Test sensor initialization
void test_sensor_initialization(void) {
    // Test initialization
    bool result = sensor->initialize();
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(State::READY, sensor->getState());
    
    // Test double initialization (should succeed)
    result = sensor->initialize();
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(State::READY, sensor->getState());
}

// Test sensor deinitialization
void test_sensor_deinitialization(void) {
    // Initialize first
    sensor->initialize();
    TEST_ASSERT_EQUAL(State::READY, sensor->getState());
    
    // Test deinitialization
    bool result = sensor->deinitialize();
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(State::UNINITIALIZED, sensor->getState());
}

// Test parameter configuration
void test_parameter_configuration(void) {
    sensor->initialize();
    
    // Test noise floor configuration
    uint8_t noiseFloor = 3;
    bool result = sensor->setParameter("noise_floor", &noiseFloor, sizeof(noiseFloor));
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(3, sensor->getNoiseFloor());
    
    // Test invalid noise floor
    noiseFloor = 10;  // Invalid (> 7)
    result = sensor->setParameter("noise_floor", &noiseFloor, sizeof(noiseFloor));
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL(3, sensor->getNoiseFloor());  // Should remain unchanged
    
    // Test watchdog threshold
    uint8_t threshold = 5;
    result = sensor->setParameter("watchdog_threshold", &threshold, sizeof(threshold));
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(5, sensor->getWatchdogThreshold());
    
    // Test spike rejection
    uint8_t rejection = 7;
    result = sensor->setParameter("spike_rejection", &rejection, sizeof(rejection));
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(7, sensor->getSpikeRejection());
    
    // Test minimum strikes
    uint8_t strikes = 9;
    result = sensor->setParameter("minimum_strikes", &strikes, sizeof(strikes));
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(9, sensor->getMinimumStrikes());
    
    // Test invalid minimum strikes
    strikes = 7;  // Invalid value
    result = sensor->setParameter("minimum_strikes", &strikes, sizeof(strikes));
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_EQUAL(9, sensor->getMinimumStrikes());  // Should remain unchanged
    
    // Test indoor mode
    bool indoor = false;
    result = sensor->setParameter("indoor_mode", &indoor, sizeof(indoor));
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_FALSE(sensor->isIndoorMode());
    
    // Test disturber masking
    bool mask = true;
    result = sensor->setParameter("mask_disturbers", &mask, sizeof(mask));
    TEST_ASSERT_TRUE(result);
}

// Test parameter retrieval
void test_parameter_retrieval(void) {
    sensor->initialize();
    
    // Set known values
    uint8_t noiseFloor = 4;
    sensor->setParameter("noise_floor", &noiseFloor, sizeof(noiseFloor));
    
    // Test retrieval
    uint8_t retrievedValue;
    size_t size = sizeof(retrievedValue);
    bool result = sensor->getParameter("noise_floor", &retrievedValue, size);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL(4, retrievedValue);
    TEST_ASSERT_EQUAL(sizeof(uint8_t), size);
    
    // Test invalid parameter name
    result = sensor->getParameter("invalid_param", &retrievedValue, size);
    TEST_ASSERT_FALSE(result);
    
    // Test null pointer
    result = sensor->getParameter("noise_floor", nullptr, size);
    TEST_ASSERT_FALSE(result);
}

// Test sensor reading
void test_sensor_reading(void) {
    sensor->initialize();
    
    Reading reading;
    bool result = sensor->readSensor(reading);
    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_TRUE(reading.isValid);
    TEST_ASSERT_EQUAL(DataType::BINARY, reading.type);
    TEST_ASSERT_EQUAL_STRING("Lightning", reading.name);
    TEST_ASSERT_EQUAL_STRING("event", reading.unit);
    TEST_ASSERT_EQUAL(sizeof(LightningData), reading.value.binaryValue.length);
    TEST_ASSERT_NOT_NULL(reading.value.binaryValue.data);
    
    // Test reading when not initialized
    sensor->deinitialize();
    result = sensor->readSensor(reading);
    TEST_ASSERT_FALSE(result);
    TEST_ASSERT_FALSE(reading.isValid);
}

// Test calibration
void test_calibration(void) {
    sensor->initialize();
    
    // Test calibration
    bool result = sensor->calibrate();
    TEST_ASSERT_TRUE(result);
    
    // Test calibration when not ready
    sensor->deinitialize();
    result = sensor->calibrate();
    TEST_ASSERT_FALSE(result);
}

// Test self-test
void test_self_test(void) {
    sensor->initialize();
    
    // Test self-test
    bool result = sensor->selfTest();
    TEST_ASSERT_TRUE(result);
    
    // Test self-test when not ready
    sensor->deinitialize();
    result = sensor->selfTest();
    TEST_ASSERT_FALSE(result);
}

// Test power management
void test_power_management(void) {
    sensor->initialize();
    
    // Test sleep
    bool result = sensor->sleep();
    TEST_ASSERT_TRUE(result);
    
    // Test wakeup
    result = sensor->wakeup();
    TEST_ASSERT_TRUE(result);
}

// Test reset functionality
void test_reset(void) {
    sensor->initialize();
    
    // Test reset
    bool result = sensor->reset();
    TEST_ASSERT_TRUE(result);
}

// Test callbacks
void test_callbacks(void) {
    sensor->initialize();
    
    // Set callbacks
    sensor->setReadingCallback(testReadingCallback);
    sensor->setErrorCallback(testErrorCallback);
    sensor->setStateChangeCallback(testStateChangeCallback);
    
    // Test state change callback
    sensor->deinitialize();
    TEST_ASSERT_EQUAL(State::UNINITIALIZED, lastState);
    
    sensor->initialize();
    TEST_ASSERT_EQUAL(State::READY, lastState);
}

// Test lightning data structure
void test_lightning_data(void) {
    LightningData data;
    bool result = sensor->getLastLightningData(data);
    TEST_ASSERT_TRUE(result);
    
    // Initial values should be zero
    TEST_ASSERT_FALSE(data.lightningDetected);
    TEST_ASSERT_EQUAL(0, data.distance);
    TEST_ASSERT_EQUAL(0, data.energy);
    TEST_ASSERT_EQUAL(0, data.strikeCount);
    TEST_ASSERT_EQUAL(0, data.lastStrikeTime);
    TEST_ASSERT_FALSE(data.isDisturber);
    TEST_ASSERT_EQUAL(0, data.noiseLevel);
}

// Test statistics
void test_statistics(void) {
    sensor->initialize();
    
    // Initial statistics should be zero
    TEST_ASSERT_EQUAL(0, sensor->getTotalLightningCount());
    TEST_ASSERT_EQUAL(0, sensor->getTotalDisturberCount());
    TEST_ASSERT_EQUAL(0, sensor->getTotalNoiseEvents());
    
    // Activity time should be non-zero after update
    sensor->update();
    TEST_ASSERT_NOT_EQUAL(0, sensor->getLastActivityTime());
}

// Test error handling
void test_error_handling(void) {
    sensor->initialize();
    
    // Test error string retrieval
    const char* errorStr = sensor->getErrorString(LightningErrorCodes::CHIP_NOT_FOUND);
    TEST_ASSERT_EQUAL_STRING("AS3935 chip not found", errorStr);
    
    errorStr = sensor->getErrorString(LightningErrorCodes::COMMUNICATION_FAILED);
    TEST_ASSERT_EQUAL_STRING("SPI communication failed", errorStr);
    
    errorStr = sensor->getErrorString(999);  // Unknown error
    TEST_ASSERT_EQUAL_STRING("Unknown error", errorStr);
}

// Test configuration edge cases
void test_configuration_edge_cases(void) {
    sensor->initialize();
    
    // Test boundary values for noise floor
    TEST_ASSERT_TRUE(sensor->setNoiseFloor(0));   // Minimum
    TEST_ASSERT_TRUE(sensor->setNoiseFloor(7));   // Maximum
    TEST_ASSERT_FALSE(sensor->setNoiseFloor(8));  // Over maximum
    
    // Test boundary values for watchdog threshold
    TEST_ASSERT_TRUE(sensor->setWatchdogThreshold(0));   // Minimum
    TEST_ASSERT_TRUE(sensor->setWatchdogThreshold(15));  // Maximum
    TEST_ASSERT_FALSE(sensor->setWatchdogThreshold(16)); // Over maximum
    
    // Test boundary values for spike rejection
    TEST_ASSERT_TRUE(sensor->setSpikeRejection(0));   // Minimum
    TEST_ASSERT_TRUE(sensor->setSpikeRejection(15));  // Maximum
    TEST_ASSERT_FALSE(sensor->setSpikeRejection(16)); // Over maximum
    
    // Test valid minimum strikes values
    TEST_ASSERT_TRUE(sensor->setMinimumStrikes(1));
    TEST_ASSERT_TRUE(sensor->setMinimumStrikes(5));
    TEST_ASSERT_TRUE(sensor->setMinimumStrikes(9));
    TEST_ASSERT_TRUE(sensor->setMinimumStrikes(16));
    
    // Test invalid minimum strikes values
    TEST_ASSERT_FALSE(sensor->setMinimumStrikes(2));
    TEST_ASSERT_FALSE(sensor->setMinimumStrikes(10));
    TEST_ASSERT_FALSE(sensor->setMinimumStrikes(20));
}

// Test data availability
void test_data_availability(void) {
    sensor->initialize();
    
    // Initially no new data
    TEST_ASSERT_FALSE(sensor->hasNewData());
    TEST_ASSERT_EQUAL(0, sensor->getReadingCount());
    
    // After reading, new data flag should be cleared
    Reading reading;
    sensor->readSensor(reading);
    TEST_ASSERT_FALSE(sensor->hasNewData());
}

// Test null pointer safety
void test_null_pointer_safety(void) {
    // Test setParameter with null pointers
    TEST_ASSERT_FALSE(sensor->setParameter(nullptr, nullptr, 0));
    TEST_ASSERT_FALSE(sensor->setParameter("test", nullptr, 1));
    TEST_ASSERT_FALSE(sensor->setParameter(nullptr, "test", 1));
    
    // Test getParameter with null pointers
    size_t size = 1;
    TEST_ASSERT_FALSE(sensor->getParameter(nullptr, nullptr, size));
    TEST_ASSERT_FALSE(sensor->getParameter("test", nullptr, size));
    TEST_ASSERT_FALSE(sensor->getParameter(nullptr, "test", size));
}

// Test interrupt handling safety
void test_interrupt_handling(void) {
    sensor->initialize();
    
    // Test interrupt handler with no instance
    LightningSensor::instance_ = nullptr;
    LightningSensor::interruptHandler();  // Should not crash
    
    // Restore instance
    LightningSensor::instance_ = sensor;
    
    // Test interrupt pending flag
    sensor->update();  // Should handle any pending interrupts safely
}

// Test tank circuit tuning
void test_tank_circuit_tuning(void) {
    sensor->initialize();
    
    bool result = sensor->tuneTankCircuit();
    TEST_ASSERT_TRUE(result);
}

// Test RCO calibration
void test_rco_calibration(void) {
    sensor->initialize();
    
    bool result = sensor->calibrateRCO();
    TEST_ASSERT_TRUE(result);
}

// Test indoor/outdoor mode switching
void test_indoor_outdoor_mode(void) {
    sensor->initialize();
    
    // Test switching to outdoor mode
    TEST_ASSERT_TRUE(sensor->setIndoorMode(false));
    TEST_ASSERT_FALSE(sensor->isIndoorMode());
    
    // Test switching back to indoor mode
    TEST_ASSERT_TRUE(sensor->setIndoorMode(true));
    TEST_ASSERT_TRUE(sensor->isIndoorMode());
}

// Test disturber masking
void test_disturber_masking(void) {
    sensor->initialize();
    
    // Test masking disturbers
    TEST_ASSERT_TRUE(sensor->maskDisturbers(true));
    
    // Test unmasking disturbers
    TEST_ASSERT_TRUE(sensor->maskDisturbers(false));
}

// Main test runner
void runLightningSensorTests(void) {
    RUN_TEST(test_sensor_properties);
    RUN_TEST(test_sensor_initialization);
    RUN_TEST(test_sensor_deinitialization);
    RUN_TEST(test_parameter_configuration);
    RUN_TEST(test_parameter_retrieval);
    RUN_TEST(test_sensor_reading);
    RUN_TEST(test_calibration);
    RUN_TEST(test_self_test);
    RUN_TEST(test_power_management);
    RUN_TEST(test_reset);
    RUN_TEST(test_callbacks);
    RUN_TEST(test_lightning_data);
    RUN_TEST(test_statistics);
    RUN_TEST(test_error_handling);
    RUN_TEST(test_configuration_edge_cases);
    RUN_TEST(test_data_availability);
    RUN_TEST(test_null_pointer_safety);
    RUN_TEST(test_interrupt_handling);
    RUN_TEST(test_tank_circuit_tuning);
    RUN_TEST(test_rco_calibration);
    RUN_TEST(test_indoor_outdoor_mode);
    RUN_TEST(test_disturber_masking);
}

#ifdef UNIT_TEST
int main(int argc, char **argv) {
    UNITY_BEGIN();
    runLightningSensorTests();
    return UNITY_END();
}
#endif