#include <unity.h>
#include "../src/config/system_config.h"
#include "../src/system/error_handler.h"
#include "../src/system/logger.h"
#include "../src/sensors/sensor_interface.h"
#include "../src/actuators/actuator_interface.h"
#include "../src/communication/communication_interface.h"
#include "../src/system/state_machine.h"

// Test configuration constants and pin definitions
void test_system_config_pins() {
    // Test hardware pin definitions
    TEST_ASSERT_EQUAL_UINT8(36, Config::Pins::VEXT);
    TEST_ASSERT_EQUAL_UINT8(21, Config::Pins::OLED_RST);
    TEST_ASSERT_EQUAL_UINT8(0, Config::Pins::BUTTON);
    TEST_ASSERT_EQUAL_UINT8(17, Config::Pins::I2C_SDA);
    TEST_ASSERT_EQUAL_UINT8(18, Config::Pins::I2C_SCL);

    // Test LoRa pin definitions
    TEST_ASSERT_EQUAL_UINT8(8, Config::Pins::LORA_NSS);
    TEST_ASSERT_EQUAL_UINT8(14, Config::Pins::LORA_DIO1);
    TEST_ASSERT_EQUAL_UINT8(12, Config::Pins::LORA_RST);
    TEST_ASSERT_EQUAL_UINT8(13, Config::Pins::LORA_BUSY);

    // Test future expansion pins
    TEST_ASSERT_EQUAL_UINT8(4, Config::Pins::LIGHTNING_IRQ);
    TEST_ASSERT_EQUAL_UINT8(5, Config::Pins::LIGHTNING_CS);
    TEST_ASSERT_EQUAL_UINT8(2, Config::Pins::LED_DATA);
    TEST_ASSERT_EQUAL_UINT8(3, Config::Pins::BUZZER);
}

void test_system_config_lora() {
    // Test LoRa configuration defaults
    TEST_ASSERT_EQUAL_FLOAT(915.0f, Config::LoRa::DEFAULT_FREQ_MHZ);
    TEST_ASSERT_EQUAL_FLOAT(125.0f, Config::LoRa::DEFAULT_BW_KHZ);
    TEST_ASSERT_EQUAL_UINT8(9, Config::LoRa::DEFAULT_SF);
    TEST_ASSERT_EQUAL_UINT8(5, Config::LoRa::DEFAULT_CR);
    TEST_ASSERT_EQUAL_INT8(17, Config::LoRa::DEFAULT_TX_DBM);

    // Test LoRa ranges
    TEST_ASSERT_EQUAL_UINT8(7, Config::LoRa::SF_MIN);
    TEST_ASSERT_EQUAL_UINT8(12, Config::LoRa::SF_MAX);
    TEST_ASSERT_EQUAL_UINT32(4, Config::LoRa::BW_OPTIONS_COUNT);

    // Test bandwidth options
    TEST_ASSERT_EQUAL_FLOAT(62.5f, Config::LoRa::BW_OPTIONS[0]);
    TEST_ASSERT_EQUAL_FLOAT(125.0f, Config::LoRa::BW_OPTIONS[1]);
    TEST_ASSERT_EQUAL_FLOAT(250.0f, Config::LoRa::BW_OPTIONS[2]);
    TEST_ASSERT_EQUAL_FLOAT(500.0f, Config::LoRa::BW_OPTIONS[3]);
}

void test_system_config_timing() {
    // Test button timing
    TEST_ASSERT_EQUAL_UINT32(50, Config::Button::DEBOUNCE_MS);
    TEST_ASSERT_EQUAL_UINT32(100, Config::Button::SHORT_PRESS_MS);
    TEST_ASSERT_EQUAL_UINT32(1000, Config::Button::MEDIUM_PRESS_MS);
    TEST_ASSERT_EQUAL_UINT32(3000, Config::Button::LONG_PRESS_MS);

    // Test system timing
    TEST_ASSERT_EQUAL_UINT32(10, Config::Timing::MAIN_LOOP_INTERVAL_MS);
    TEST_ASSERT_EQUAL_UINT32(1000, Config::Timing::STATUS_UPDATE_INTERVAL_MS);
    TEST_ASSERT_EQUAL_UINT32(2000, Config::Timing::PING_INTERVAL_MS);
}

void test_error_handler_enums() {
    // Test error severity levels
    TEST_ASSERT_EQUAL_INT(0, (int)ErrorHandler::Severity::INFO);
    TEST_ASSERT_EQUAL_INT(1, (int)ErrorHandler::Severity::WARNING);
    TEST_ASSERT_EQUAL_INT(2, (int)ErrorHandler::Severity::ERROR);
    TEST_ASSERT_EQUAL_INT(3, (int)ErrorHandler::Severity::CRITICAL);

    // Test error categories
    TEST_ASSERT_EQUAL_INT(0, (int)ErrorHandler::Category::HARDWARE);
    TEST_ASSERT_EQUAL_INT(1, (int)ErrorHandler::Category::RADIO);
    TEST_ASSERT_EQUAL_INT(2, (int)ErrorHandler::Category::WIFI);
    TEST_ASSERT_EQUAL_INT(3, (int)ErrorHandler::Category::SENSOR);
    TEST_ASSERT_EQUAL_INT(4, (int)ErrorHandler::Category::ACTUATOR);

    // Test some error codes
    TEST_ASSERT_EQUAL_INT(100, (int)ErrorHandler::Code::OLED_INIT_FAILED);
    TEST_ASSERT_EQUAL_INT(200, (int)ErrorHandler::Code::RADIO_INIT_FAILED);
    TEST_ASSERT_EQUAL_INT(300, (int)ErrorHandler::Code::WIFI_CONNECT_FAILED);
    TEST_ASSERT_EQUAL_INT(400, (int)ErrorHandler::Code::SENSOR_INIT_FAILED);
    TEST_ASSERT_EQUAL_INT(500, (int)ErrorHandler::Code::LED_INIT_FAILED);
}

void test_logger_enums() {
    // Test log levels
    TEST_ASSERT_EQUAL_INT(0, (int)Logger::Level::TRACE);
    TEST_ASSERT_EQUAL_INT(1, (int)Logger::Level::DEBUG);
    TEST_ASSERT_EQUAL_INT(2, (int)Logger::Level::INFO);
    TEST_ASSERT_EQUAL_INT(3, (int)Logger::Level::WARN);
    TEST_ASSERT_EQUAL_INT(4, (int)Logger::Level::ERROR);
    TEST_ASSERT_EQUAL_INT(5, (int)Logger::Level::FATAL);

    // Test log categories
    TEST_ASSERT_EQUAL_INT(0, (int)Logger::Category::SYSTEM);
    TEST_ASSERT_EQUAL_INT(1, (int)Logger::Category::HARDWARE);
    TEST_ASSERT_EQUAL_INT(2, (int)Logger::Category::RADIO);

    // Test log destinations as bitmask
    TEST_ASSERT_EQUAL_INT(1, (int)Logger::Destination::SERIAL);
    TEST_ASSERT_EQUAL_INT(2, (int)Logger::Destination::DISPLAY);
    TEST_ASSERT_EQUAL_INT(4, (int)Logger::Destination::RADIO);
    TEST_ASSERT_EQUAL_INT(8, (int)Logger::Destination::STORAGE);
}

void test_sensor_interface_enums() {
    // Test sensor states
    TEST_ASSERT_EQUAL_INT(0, (int)Sensors::State::UNINITIALIZED);
    TEST_ASSERT_EQUAL_INT(1, (int)Sensors::State::INITIALIZING);
    TEST_ASSERT_EQUAL_INT(2, (int)Sensors::State::READY);
    TEST_ASSERT_EQUAL_INT(3, (int)Sensors::State::READING);
    TEST_ASSERT_EQUAL_INT(4, (int)Sensors::State::ERROR);
    TEST_ASSERT_EQUAL_INT(5, (int)Sensors::State::DISABLED);

    // Test data types
    TEST_ASSERT_EQUAL_INT(0, (int)Sensors::DataType::BOOLEAN);
    TEST_ASSERT_EQUAL_INT(1, (int)Sensors::DataType::INTEGER);
    TEST_ASSERT_EQUAL_INT(2, (int)Sensors::DataType::FLOAT);
    TEST_ASSERT_EQUAL_INT(3, (int)Sensors::DataType::STRING);
    TEST_ASSERT_EQUAL_INT(4, (int)Sensors::DataType::BINARY);

    // Test capabilities as bitmask
    TEST_ASSERT_EQUAL_INT(0, (int)Sensors::Capability::NONE);
    TEST_ASSERT_EQUAL_INT(1, (int)Sensors::Capability::INTERRUPT_CAPABLE);
    TEST_ASSERT_EQUAL_INT(2, (int)Sensors::Capability::CONFIGURABLE);
    TEST_ASSERT_EQUAL_INT(4, (int)Sensors::Capability::SELF_TEST);
    TEST_ASSERT_EQUAL_INT(8, (int)Sensors::Capability::CALIBRATION);
}

void test_actuator_interface_enums() {
    // Test actuator states
    TEST_ASSERT_EQUAL_INT(0, (int)Actuators::State::UNINITIALIZED);
    TEST_ASSERT_EQUAL_INT(1, (int)Actuators::State::INITIALIZING);
    TEST_ASSERT_EQUAL_INT(2, (int)Actuators::State::READY);
    TEST_ASSERT_EQUAL_INT(3, (int)Actuators::State::ACTIVE);
    TEST_ASSERT_EQUAL_INT(4, (int)Actuators::State::ERROR);
    TEST_ASSERT_EQUAL_INT(5, (int)Actuators::State::DISABLED);

    // Test actuator types
    TEST_ASSERT_EQUAL_INT(0, (int)Actuators::Type::LED_STRIP);
    TEST_ASSERT_EQUAL_INT(1, (int)Actuators::Type::LED_SINGLE);
    TEST_ASSERT_EQUAL_INT(2, (int)Actuators::Type::BUZZER);
    TEST_ASSERT_EQUAL_INT(3, (int)Actuators::Type::DISPLAY);

    // Test animations
    TEST_ASSERT_EQUAL_INT(0, (int)Actuators::Animation::NONE);
    TEST_ASSERT_EQUAL_INT(1, (int)Actuators::Animation::FADE);
    TEST_ASSERT_EQUAL_INT(2, (int)Actuators::Animation::BLINK);
    TEST_ASSERT_EQUAL_INT(3, (int)Actuators::Animation::PULSE);

    // Test sound patterns
    TEST_ASSERT_EQUAL_INT(0, (int)Actuators::SoundPattern::NONE);
    TEST_ASSERT_EQUAL_INT(1, (int)Actuators::SoundPattern::BEEP);
    TEST_ASSERT_EQUAL_INT(2, (int)Actuators::SoundPattern::DOUBLE_BEEP);
}

void test_color_structure() {
    // Test Color constructor
    Actuators::Color black;
    TEST_ASSERT_EQUAL_UINT8(0, black.red);
    TEST_ASSERT_EQUAL_UINT8(0, black.green);
    TEST_ASSERT_EQUAL_UINT8(0, black.blue);
    TEST_ASSERT_EQUAL_UINT8(0, black.white);

    // Test Color with values
    Actuators::Color red(255, 0, 0);
    TEST_ASSERT_EQUAL_UINT8(255, red.red);
    TEST_ASSERT_EQUAL_UINT8(0, red.green);
    TEST_ASSERT_EQUAL_UINT8(0, red.blue);

    // Test predefined colors
    TEST_ASSERT_EQUAL_UINT8(255, Actuators::Colors::WHITE.red);
    TEST_ASSERT_EQUAL_UINT8(255, Actuators::Colors::WHITE.green);
    TEST_ASSERT_EQUAL_UINT8(255, Actuators::Colors::WHITE.blue);

    TEST_ASSERT_EQUAL_UINT8(255, Actuators::Colors::RED.red);
    TEST_ASSERT_EQUAL_UINT8(0, Actuators::Colors::RED.green);
    TEST_ASSERT_EQUAL_UINT8(0, Actuators::Colors::RED.blue);
}

void test_communication_interface_enums() {
    // Test communication channels
    TEST_ASSERT_EQUAL_INT(0, (int)Communication::Channel::LORA_DATA);
    TEST_ASSERT_EQUAL_INT(1, (int)Communication::Channel::LORA_CONTROL);
    TEST_ASSERT_EQUAL_INT(2, (int)Communication::Channel::WIFI);
    TEST_ASSERT_EQUAL_INT(3, (int)Communication::Channel::SERIAL);

    // Test message types
    TEST_ASSERT_EQUAL_INT(0, (int)Communication::MessageType::PING);
    TEST_ASSERT_EQUAL_INT(1, (int)Communication::MessageType::PONG);
    TEST_ASSERT_EQUAL_INT(2, (int)Communication::MessageType::CONFIG);
    TEST_ASSERT_EQUAL_INT(3, (int)Communication::MessageType::SENSOR_DATA);

    // Test priority levels
    TEST_ASSERT_EQUAL_INT(0, (int)Communication::Priority::LOW);
    TEST_ASSERT_EQUAL_INT(1, (int)Communication::Priority::NORMAL);
    TEST_ASSERT_EQUAL_INT(2, (int)Communication::Priority::HIGH);
    TEST_ASSERT_EQUAL_INT(3, (int)Communication::Priority::CRITICAL);
}

void test_state_machine_enums() {
    // Test system states
    TEST_ASSERT_EQUAL_INT(0, (int)StateMachine::SystemState::BOOT);
    TEST_ASSERT_EQUAL_INT(1, (int)StateMachine::SystemState::INIT);
    TEST_ASSERT_EQUAL_INT(2, (int)StateMachine::SystemState::IDLE);
    TEST_ASSERT_EQUAL_INT(3, (int)StateMachine::SystemState::SENDER);
    TEST_ASSERT_EQUAL_INT(4, (int)StateMachine::SystemState::RECEIVER);
    TEST_ASSERT_EQUAL_INT(5, (int)StateMachine::SystemState::CONFIG);

    // Test events
    TEST_ASSERT_EQUAL_INT(0, (int)StateMachine::Event::POWER_ON);
    TEST_ASSERT_EQUAL_INT(1, (int)StateMachine::Event::INIT_COMPLETE);
    TEST_ASSERT_EQUAL_INT(2, (int)StateMachine::Event::BUTTON_SHORT);
    TEST_ASSERT_EQUAL_INT(3, (int)StateMachine::Event::BUTTON_MEDIUM);
    TEST_ASSERT_EQUAL_INT(4, (int)StateMachine::Event::BUTTON_LONG);

    // Test transition results
    TEST_ASSERT_EQUAL_INT(0, (int)StateMachine::TransitionResult::SUCCESS);
    TEST_ASSERT_EQUAL_INT(1, (int)StateMachine::TransitionResult::INVALID_EVENT);
    TEST_ASSERT_EQUAL_INT(2, (int)StateMachine::TransitionResult::TRANSITION_BLOCKED);
}

void test_message_header_structure() {
    // Test MessageHeader size and alignment
    Communication::MessageHeader header = {};

    // Initialize header fields
    header.messageId = 0x12345678;
    header.type = Communication::MessageType::PING;
    header.priority = Communication::Priority::NORMAL;
    header.sourceId = 0x01;
    header.destinationId = 0xFF;
    header.sequenceNumber = 100;
    header.payloadSize = 0;
    header.timestamp = 123456789;
    header.checksum = 0xABCD;

    // Verify fields are set correctly
    TEST_ASSERT_EQUAL_UINT32(0x12345678, header.messageId);
    TEST_ASSERT_EQUAL_INT((int)Communication::MessageType::PING, (int)header.type);
    TEST_ASSERT_EQUAL_INT((int)Communication::Priority::NORMAL, (int)header.priority);
    TEST_ASSERT_EQUAL_UINT8(0x01, header.sourceId);
    TEST_ASSERT_EQUAL_UINT8(0xFF, header.destinationId);
    TEST_ASSERT_EQUAL_UINT16(100, header.sequenceNumber);
    TEST_ASSERT_EQUAL_UINT16(0, header.payloadSize);
    TEST_ASSERT_EQUAL_UINT32(123456789, header.timestamp);
    TEST_ASSERT_EQUAL_UINT16(0xABCD, header.checksum);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Configuration tests
    RUN_TEST(test_system_config_pins);
    RUN_TEST(test_system_config_lora);
    RUN_TEST(test_system_config_timing);

    // Error handling tests
    RUN_TEST(test_error_handler_enums);

    // Logging tests
    RUN_TEST(test_logger_enums);

    // Sensor framework tests
    RUN_TEST(test_sensor_interface_enums);

    // Actuator framework tests
    RUN_TEST(test_actuator_interface_enums);
    RUN_TEST(test_color_structure);

    // Communication framework tests
    RUN_TEST(test_communication_interface_enums);
    RUN_TEST(test_message_header_structure);

    // State machine tests
    RUN_TEST(test_state_machine_enums);

    return UNITY_END();
}
