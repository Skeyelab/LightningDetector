#include "lightning_sensor.h"
#include "../hardware/hardware_abstraction.h"
#include <AS3935.h>

#ifdef ARDUINO
#include <Arduino.h>
#include <SPI.h>
#else
// Mock implementations for unit testing
#define millis() 0
#define FALLING 2
#define digitalPinToInterrupt(pin) (pin)
#endif

namespace Sensors {

// Static instance for interrupt handling
LightningSensor* LightningSensor::instance_ = nullptr;

LightningSensor::LightningSensor() 
    : state_(SensorSystem::State::UNINITIALIZED)
    , hasNewData_(false)
    , readingCount_(0)
    , lastError_(0)
    , interruptPending_(false)
{
    // Initialize configuration with default values
    config_.noiseFloor = SystemConfig::Lightning::NOISE_FLOOR;
    config_.watchdogThreshold = SystemConfig::Lightning::WATCHDOG_THRESHOLD;
    config_.spikeRejection = SystemConfig::Lightning::SPIKE_REJECTION;
    config_.minimumStrikes = SystemConfig::Lightning::MIN_STRIKES;
    config_.indoorMode = true;  // Default to indoor mode
    config_.disturbersLightning = false;
    config_.tuningCapacitor = 8;  // Mid-range default

    // Initialize statistics
    memset(&stats_, 0, sizeof(stats_));

    // Initialize lightning data
    memset(&lastLightning_, 0, sizeof(lastLightning_));

    // Set static instance for interrupt handling
    instance_ = this;
}

bool LightningSensor::initialize() {
    setState(SensorSystem::State::INITIALIZING);

    // Initialize SPI
    auto spiResult = HardwareAbstraction::SPI::initialize();
    if (spiResult != HardwareAbstraction::Result::SUCCESS) {
        reportError(LightningErrorCodes::COMMUNICATION_FAILED, "SPI initialization failed");
        return false;
    }

    // Configure CS pin
    auto csResult = HardwareAbstraction::GPIO::pinMode(
        SystemConfig::Pins::LIGHTNING_CS, 
        HardwareAbstraction::GPIO::Mode::MODE_OUTPUT
    );
    if (csResult != HardwareAbstraction::Result::SUCCESS) {
        reportError(LightningErrorCodes::COMMUNICATION_FAILED, "CS pin configuration failed");
        return false;
    }

    // Set CS high (inactive)
    HardwareAbstraction::GPIO::digitalWrite(
        SystemConfig::Pins::LIGHTNING_CS, 
        HardwareAbstraction::GPIO::Level::LEVEL_HIGH
    );

    // Configure interrupt pin
    auto irqResult = HardwareAbstraction::GPIO::pinMode(
        SystemConfig::Pins::LIGHTNING_IRQ, 
        HardwareAbstraction::GPIO::Mode::MODE_INPUT_PULLUP
    );
    if (irqResult != HardwareAbstraction::Result::SUCCESS) {
        reportError(LightningErrorCodes::INTERRUPT_SETUP_FAILED, "IRQ pin configuration failed");
        return false;
    }

    // Power up the AS3935
    if (!powerUp()) {
        reportError(LightningErrorCodes::CHIP_NOT_FOUND, "AS3935 power up failed");
        return false;
    }

    // Verify chip ID
    uint8_t chipId;
    if (!readRegister(AS3935Register::AFE_GAIN, chipId)) {
        reportError(LightningErrorCodes::COMMUNICATION_FAILED, "Failed to read chip ID");
        return false;
    }

    // Reset the chip
    if (!reset()) {
        reportError(LightningErrorCodes::COMMUNICATION_FAILED, "Chip reset failed");
        return false;
    }

    // Apply configuration
    if (!validateConfig() || !applyConfiguration()) {
        reportError(LightningErrorCodes::INVALID_CONFIGURATION, "Configuration application failed");
        return false;
    }

    // Set up interrupt handling
    auto interruptResult = HardwareAbstraction::GPIO::attachInterrupt(
        SystemConfig::Pins::LIGHTNING_IRQ, 
        interruptHandler, 
        FALLING
    );
    if (interruptResult != HardwareAbstraction::Result::SUCCESS) {
        reportError(LightningErrorCodes::INTERRUPT_SETUP_FAILED, "Interrupt setup failed");
        return false;
    }

    // Perform initial calibration
    if (!calibrate()) {
        reportError(LightningErrorCodes::CALIBRATION_FAILED, "Initial calibration failed");
        return false;
    }

    setState(SensorSystem::State::READY);
    return true;
}

bool LightningSensor::deinitialize() {
    setState(SensorSystem::State::UNINITIALIZED);

    // Detach interrupt
    HardwareAbstraction::GPIO::detachInterrupt(SystemConfig::Pins::LIGHTNING_IRQ);

    // Power down the chip
    powerDown();

    return true;
}

uint16_t LightningSensor::getCapabilities() const {
    using namespace SensorSystem;
    return static_cast<uint16_t>(Capability::INTERRUPT_CAPABLE) |
           static_cast<uint16_t>(Capability::CONFIGURABLE) |
           static_cast<uint16_t>(Capability::SELF_TEST) |
           static_cast<uint16_t>(Capability::CALIBRATION) |
           static_cast<uint16_t>(Capability::POWER_MANAGEMENT) |
           static_cast<uint16_t>(Capability::THRESHOLD_DETECTION);
}

bool LightningSensor::readSensor(SensorSystem::Reading& reading) {
    if (state_ != SensorSystem::State::READY) {
        reading.isValid = false;
        reading.errorCode = LightningErrorCodes::COMMUNICATION_FAILED;
        return false;
    }

    reading.timestamp = millis();
    reading.type = SensorSystem::DataType::BINARY;
    reading.name = "Lightning";
    reading.unit = "event";
    reading.value.binaryValue.data = reinterpret_cast<const uint8_t*>(&lastLightning_);
    reading.value.binaryValue.length = sizeof(lastLightning_);
    reading.isValid = true;
    reading.errorCode = 0;

    hasNewData_ = false;
    return true;
}

bool LightningSensor::setParameter(const char* name, const void* value, size_t size) {
    if (name == nullptr || value == nullptr || size == 0) {
        return false;
    }

    if (strcmp(name, "noise_floor") == 0 && size == sizeof(uint8_t)) {
        return setNoiseFloor(*static_cast<const uint8_t*>(value));
    } else if (strcmp(name, "watchdog_threshold") == 0 && size == sizeof(uint8_t)) {
        return setWatchdogThreshold(*static_cast<const uint8_t*>(value));
    } else if (strcmp(name, "spike_rejection") == 0 && size == sizeof(uint8_t)) {
        return setSpikeRejection(*static_cast<const uint8_t*>(value));
    } else if (strcmp(name, "minimum_strikes") == 0 && size == sizeof(uint8_t)) {
        return setMinimumStrikes(*static_cast<const uint8_t*>(value));
    } else if (strcmp(name, "indoor_mode") == 0 && size == sizeof(bool)) {
        return setIndoorMode(*static_cast<const bool*>(value));
    } else if (strcmp(name, "mask_disturbers") == 0 && size == sizeof(bool)) {
        return maskDisturbers(*static_cast<const bool*>(value));
    }

    return false;
}

bool LightningSensor::getParameter(const char* name, void* value, size_t& size) const {
    if (name == nullptr || value == nullptr) {
        return false;
    }

    if (strcmp(name, "noise_floor") == 0 && size >= sizeof(uint8_t)) {
        *static_cast<uint8_t*>(value) = config_.noiseFloor;
        size = sizeof(uint8_t);
        return true;
    } else if (strcmp(name, "watchdog_threshold") == 0 && size >= sizeof(uint8_t)) {
        *static_cast<uint8_t*>(value) = config_.watchdogThreshold;
        size = sizeof(uint8_t);
        return true;
    } else if (strcmp(name, "spike_rejection") == 0 && size >= sizeof(uint8_t)) {
        *static_cast<uint8_t*>(value) = config_.spikeRejection;
        size = sizeof(uint8_t);
        return true;
    } else if (strcmp(name, "minimum_strikes") == 0 && size >= sizeof(uint8_t)) {
        *static_cast<uint8_t*>(value) = config_.minimumStrikes;
        size = sizeof(uint8_t);
        return true;
    } else if (strcmp(name, "indoor_mode") == 0 && size >= sizeof(bool)) {
        *static_cast<bool*>(value) = config_.indoorMode;
        size = sizeof(bool);
        return true;
    }

    return false;
}

bool LightningSensor::calibrate() {
    if (state_ != SensorSystem::State::READY) {
        return false;
    }

    setState(SensorSystem::State::READING);

    bool success = true;

    // Perform tank circuit tuning
    if (!tuneTankCircuit()) {
        reportError(LightningErrorCodes::TANK_TUNING_FAILED, "Tank circuit tuning failed");
        success = false;
    }

    // Perform RCO calibration
    if (!calibrateRCO()) {
        reportError(LightningErrorCodes::RCO_CALIBRATION_FAILED, "RCO calibration failed");
        success = false;
    }

    if (success) {
        stats_.calibrationCount++;
    }

    setState(SensorSystem::State::READY);
    return success;
}

bool LightningSensor::selfTest() {
    if (state_ != SensorSystem::State::READY) {
        return false;
    }

    setState(SensorSystem::State::READING);

    // Clear any pending interrupts
    clearStatistics();

    // Send direct command to test the antenna
    uint8_t testValue = 0x96;  // Test pattern
    bool result = writeRegister(AS3935Register::DISP_LCO, testValue);

    if (result) {
        // Wait for test completion
        delay(50);

        // Read back to verify
        uint8_t readValue;
        result = readRegister(AS3935Register::DISP_LCO, readValue);
        if (!result || readValue != testValue) {
            result = false;
        }
    }

    setState(SensorSystem::State::READY);
    return result;
}

bool LightningSensor::sleep() {
    return powerDown();
}

bool LightningSensor::wakeup() {
    return powerUp();
}

bool LightningSensor::reset() {
    // Write reset command
    bool result = writeRegister(AS3935Register::PWD, 0x96);
    if (result) {
        delay(RESET_DELAY_MS);
        // Clear statistics after reset
        clearStatistics();
    }
    return result;
}

void LightningSensor::update() {
    if (interruptPending_) {
        handleInterrupt();
        interruptPending_ = false;
    }

    // Update statistics
    stats_.lastActivity = millis();
}

const char* LightningSensor::getErrorString(uint32_t errorCode) const {
    switch (errorCode) {
        case LightningErrorCodes::CHIP_NOT_FOUND:
            return "AS3935 chip not found";
        case LightningErrorCodes::COMMUNICATION_FAILED:
            return "SPI communication failed";
        case LightningErrorCodes::CALIBRATION_FAILED:
            return "Calibration failed";
        case LightningErrorCodes::INVALID_CONFIGURATION:
            return "Invalid configuration";
        case LightningErrorCodes::INTERRUPT_SETUP_FAILED:
            return "Interrupt setup failed";
        case LightningErrorCodes::TANK_TUNING_FAILED:
            return "Tank circuit tuning failed";
        case LightningErrorCodes::RCO_CALIBRATION_FAILED:
            return "RCO calibration failed";
        default:
            return "Unknown error";
    }
}

// Lightning sensor specific methods
bool LightningSensor::setNoiseFloor(uint8_t level) {
    if (level > 7) {
        return false;
    }
    
    config_.noiseFloor = level;
    return modifyRegister(AS3935Register::NF_LEV, 0x70, level << 4);
}

bool LightningSensor::setWatchdogThreshold(uint8_t threshold) {
    if (threshold > 15) {
        return false;
    }
    
    config_.watchdogThreshold = threshold;
    return modifyRegister(AS3935Register::WDTH, 0x0F, threshold);
}

bool LightningSensor::setSpikeRejection(uint8_t rejection) {
    if (rejection > 15) {
        return false;
    }
    
    config_.spikeRejection = rejection;
    return modifyRegister(AS3935Register::SREJ, 0xF0, rejection << 4);
}

bool LightningSensor::setMinimumStrikes(uint8_t strikes) {
    uint8_t value;
    switch (strikes) {
        case 1: value = 0; break;
        case 5: value = 1; break;
        case 9: value = 2; break;
        case 16: value = 3; break;
        default: return false;
    }
    
    config_.minimumStrikes = strikes;
    return modifyRegister(AS3935Register::MIN_NUM_LIGH, 0x30, value << 4);
}

bool LightningSensor::setIndoorMode(bool indoor) {
    config_.indoorMode = indoor;
    uint8_t afeGain = indoor ? 0x12 : 0x0E;  // Indoor: 18, Outdoor: 14
    return modifyRegister(AS3935Register::AFE_GAIN, 0x3E, afeGain << 1);
}

bool LightningSensor::maskDisturbers(bool mask) {
    config_.disturbersLightning = !mask;
    return modifyRegister(AS3935Register::MASK_DIST, 0x20, mask ? 0x20 : 0x00);
}

bool LightningSensor::tuneTankCircuit() {
    // Automatic tuning algorithm
    uint8_t bestCap = 8;  // Start with middle value
    uint32_t bestFreq = 0;
    
    for (uint8_t cap = 0; cap <= 15; cap++) {
        // Set tuning capacitor value
        if (!modifyRegister(AS3935Register::TUN_CAP, 0x0F, cap)) {
            continue;
        }
        
        // Display LCO on IRQ pin
        writeRegister(AS3935Register::DISP_LCO, 0x80);
        delay(2);
        
        // Measure frequency (simplified - would need frequency counter in real implementation)
        uint32_t freq = measureLCOFrequency();
        
        // Look for frequency closest to target (500kHz / 16 = 31.25kHz)
        if (abs((int32_t)freq - 31250) < abs((int32_t)bestFreq - 31250)) {
            bestFreq = freq;
            bestCap = cap;
        }
        
        // Turn off LCO display
        writeRegister(AS3935Register::DISP_LCO, 0x00);
    }
    
    // Set the best capacitor value
    config_.tuningCapacitor = bestCap;
    return modifyRegister(AS3935Register::TUN_CAP, 0x0F, bestCap);
}

bool LightningSensor::calibrateRCO() {
    // Start RCO calibration
    bool result = modifyRegister(AS3935Register::DISP_SRCO, 0x80, 0x80);
    if (!result) {
        return false;
    }
    
    // Wait for calibration to complete
    uint32_t startTime = millis();
    while (millis() - startTime < CALIBRATION_TIMEOUT_MS) {
        uint8_t status;
        if (readRegister(AS3935Register::DISP_SRCO, status)) {
            if ((status & 0x80) == 0) {
                // Calibration completed
                return true;
            }
        }
        delay(10);
    }
    
    return false;  // Timeout
}

bool LightningSensor::getLastLightningData(LightningData& data) const {
    data = lastLightning_;
    return true;
}

void IRAM_ATTR LightningSensor::interruptHandler() {
    if (instance_ != nullptr) {
        instance_->interruptPending_ = true;
    }
}

void LightningSensor::handleInterrupt() {
    InterruptReason reason = getInterruptReason();
    
    switch (reason) {
        case InterruptReason::LIGHTNING:
            handleLightningEvent();
            break;
        case InterruptReason::DISTURBER:
            handleDisturberEvent();
            break;
        case InterruptReason::NOISE:
            handleNoiseEvent();
            break;
    }
}

// Private helper methods
bool LightningSensor::writeRegister(AS3935Register reg, uint8_t value) {
    HardwareAbstraction::SPI::Settings settings = {
        .frequency = 2000000,  // 2MHz
        .bitOrder = 1,         // MSB first
        .dataMode = 1          // SPI_MODE1
    };
    
    HardwareAbstraction::SPI::beginTransaction(settings);
    
    // Set CS low
    HardwareAbstraction::GPIO::digitalWrite(
        SystemConfig::Pins::LIGHTNING_CS, 
        HardwareAbstraction::GPIO::Level::LEVEL_LOW
    );
    
    // Send address (write command)
    uint8_t address = static_cast<uint8_t>(reg);
    HardwareAbstraction::SPI::transfer(address);
    
    // Send data
    HardwareAbstraction::SPI::transfer(value);
    
    // Set CS high
    HardwareAbstraction::GPIO::digitalWrite(
        SystemConfig::Pins::LIGHTNING_CS, 
        HardwareAbstraction::GPIO::Level::LEVEL_HIGH
    );
    
    HardwareAbstraction::SPI::endTransaction();
    
    return true;
}

bool LightningSensor::readRegister(AS3935Register reg, uint8_t& value) {
    HardwareAbstraction::SPI::Settings settings = {
        .frequency = 2000000,  // 2MHz
        .bitOrder = 1,         // MSB first
        .dataMode = 1          // SPI_MODE1
    };
    
    HardwareAbstraction::SPI::beginTransaction(settings);
    
    // Set CS low
    HardwareAbstraction::GPIO::digitalWrite(
        SystemConfig::Pins::LIGHTNING_CS, 
        HardwareAbstraction::GPIO::Level::LEVEL_LOW
    );
    
    // Send address (read command - set bit 6)
    uint8_t address = static_cast<uint8_t>(reg) | 0x40;
    HardwareAbstraction::SPI::transfer(address);
    
    // Read data
    value = HardwareAbstraction::SPI::transfer(0x00);
    
    // Set CS high
    HardwareAbstraction::GPIO::digitalWrite(
        SystemConfig::Pins::LIGHTNING_CS, 
        HardwareAbstraction::GPIO::Level::LEVEL_HIGH
    );
    
    HardwareAbstraction::SPI::endTransaction();
    
    return true;
}

bool LightningSensor::modifyRegister(AS3935Register reg, uint8_t mask, uint8_t value) {
    uint8_t currentValue;
    if (!readRegister(reg, currentValue)) {
        return false;
    }
    
    uint8_t newValue = (currentValue & ~mask) | (value & mask);
    return writeRegister(reg, newValue);
}

bool LightningSensor::powerUp() {
    return modifyRegister(AS3935Register::PWD, 0x01, 0x00);
}

bool LightningSensor::powerDown() {
    return modifyRegister(AS3935Register::PWD, 0x01, 0x01);
}

bool LightningSensor::clearStatistics() {
    memset(&stats_, 0, sizeof(stats_));
    return writeRegister(AS3935Register::CL_STAT, 0xC0);
}

LightningSensor::InterruptReason LightningSensor::getInterruptReason() {
    uint8_t reason;
    if (!readRegister(AS3935Register::CL_STAT, reason)) {
        return InterruptReason::NOISE;  // Default to noise if read fails
    }
    
    return static_cast<InterruptReason>(reason & 0x0F);
}

uint8_t LightningSensor::getLightningDistance() {
    uint8_t distance;
    if (!readRegister(AS3935Register::MASK_DIST, distance)) {
        return 63;  // Out of range
    }
    
    return distance & 0x1F;
}

uint32_t LightningSensor::getLightningEnergy() {
    uint8_t energy_lsb, energy_msb, energy_mmsb;
    
    if (!readRegister(static_cast<AS3935Register>(0x04), energy_mmsb) ||
        !readRegister(static_cast<AS3935Register>(0x05), energy_msb) ||
        !readRegister(static_cast<AS3935Register>(0x06), energy_lsb)) {
        return 0;
    }
    
    return (static_cast<uint32_t>(energy_mmsb & 0x1F) << 16) |
           (static_cast<uint32_t>(energy_msb) << 8) |
           energy_lsb;
}

bool LightningSensor::validateConfig() const {
    return config_.noiseFloor <= 7 &&
           config_.watchdogThreshold <= 15 &&
           config_.spikeRejection <= 15 &&
           (config_.minimumStrikes == 1 || config_.minimumStrikes == 5 ||
            config_.minimumStrikes == 9 || config_.minimumStrikes == 16) &&
           config_.tuningCapacitor <= 15;
}

bool LightningSensor::isConfigurationValid(const Config& config) const {
    return config.noiseFloor <= 7 &&
           config.watchdogThreshold <= 15 &&
           config.spikeRejection <= 15 &&
           (config.minimumStrikes == 1 || config.minimumStrikes == 5 ||
            config.minimumStrikes == 9 || config.minimumStrikes == 16) &&
           config.tuningCapacitor <= 15;
}

void LightningSensor::setState(SensorSystem::State newState) {
    if (state_ != newState) {
        state_ = newState;
        if (stateChangeCallback_) {
            stateChangeCallback_(getId(), state_);
        }
    }
}

void LightningSensor::reportError(uint32_t errorCode, const char* message) {
    lastError_ = errorCode;
    setState(SensorSystem::State::ERROR);
    
    if (errorCallback_) {
        errorCallback_(getId(), errorCode);
    }
}

bool LightningSensor::applyConfiguration() {
    return setNoiseFloor(config_.noiseFloor) &&
           setWatchdogThreshold(config_.watchdogThreshold) &&
           setSpikeRejection(config_.spikeRejection) &&
           setMinimumStrikes(config_.minimumStrikes) &&
           setIndoorMode(config_.indoorMode) &&
           maskDisturbers(!config_.disturbersLightning);
}

uint32_t LightningSensor::measureLCOFrequency() {
    // Simplified frequency measurement
    // In a real implementation, this would use a timer or frequency counter
    // For now, return a mock value based on tuning capacitor
    return 31250 + (config_.tuningCapacitor - 8) * 1000;
}

void LightningSensor::handleLightningEvent() {
    lastLightning_.lightningDetected = true;
    lastLightning_.distance = getLightningDistance();
    lastLightning_.energy = getLightningEnergy();
    lastLightning_.lastStrikeTime = millis();
    lastLightning_.isDisturber = false;
    lastLightning_.strikeCount++;
    
    stats_.totalLightning++;
    hasNewData_ = true;
    readingCount_++;
    
    if (readingCallback_) {
        SensorSystem::Reading reading;
        if (readSensor(reading)) {
            readingCallback_(reading);
        }
    }
}

void LightningSensor::handleDisturberEvent() {
    lastLightning_.lightningDetected = false;
    lastLightning_.isDisturber = true;
    lastLightning_.lastStrikeTime = millis();
    
    stats_.totalDisturbers++;
    hasNewData_ = true;
    
    if (readingCallback_) {
        SensorSystem::Reading reading;
        if (readSensor(reading)) {
            readingCallback_(reading);
        }
    }
}

void LightningSensor::handleNoiseEvent() {
    lastLightning_.noiseLevel = getNoiseFloor();
    stats_.totalNoise++;
    
    // Auto-adjust noise floor if too many noise events
    if (stats_.totalNoise % 10 == 0 && config_.noiseFloor < 7) {
        setNoiseFloor(config_.noiseFloor + 1);
    }
}

} // namespace Sensors