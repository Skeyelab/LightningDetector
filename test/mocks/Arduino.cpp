#include "Arduino.h"
#include <chrono>
#include <cstdlib>
#include <cstring>

#ifdef ARDUINO_MOCK

// Mock time tracking
static std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

unsigned long millis() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
    return static_cast<unsigned long>(duration.count());
}

unsigned long micros() {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start_time);
    return static_cast<unsigned long>(duration.count());
}

void delay(unsigned long ms) {
    // Mock delay - do nothing in test environment
    (void)ms;
}

void delayMicroseconds(unsigned int us) {
    // Mock delay - do nothing in test environment
    (void)us;
}

// Mock GPIO state storage
static uint8_t pin_modes[100] = {0};
static uint8_t pin_states[100] = {0};

void pinMode(uint8_t pin, uint8_t mode) {
    if (pin < 100) {
        pin_modes[pin] = mode;
    }
}

void digitalWrite(uint8_t pin, uint8_t val) {
    if (pin < 100 && pin_modes[pin] == OUTPUT) {
        pin_states[pin] = val;
    }
}

int digitalRead(uint8_t pin) {
    if (pin < 100) {
        return pin_states[pin];
    }
    return LOW;
}

// Mock analog functions
int analogRead(uint8_t pin) {
    // Return mock analog value
    (void)pin;
    return rand() % 4096; // 12-bit ADC mock
}

void analogWrite(uint8_t pin, int val) {
    // Mock analog write
    (void)pin;
    (void)val;
}

// Mock interrupt functions
void attachInterrupt(uint8_t pin, void (*callback)(), int mode) {
    // Mock interrupt attachment
    (void)pin;
    (void)callback;
    (void)mode;
}

void detachInterrupt(uint8_t pin) {
    // Mock interrupt detachment
    (void)pin;
}

// Mock Serial
SerialClass Serial;

void SerialClass::begin(unsigned long baud) {
    (void)baud;
}

void SerialClass::print(const char* str) {
    (void)str;
}

void SerialClass::println(const char* str) {
    (void)str;
}

void SerialClass::println() {
}

int SerialClass::available() {
    return 0;
}

int SerialClass::read() {
    return -1;
}

void SerialClass::write(uint8_t val) {
    (void)val;
}

// Mock Wire (I2C)
TwoWire Wire;

void TwoWire::begin() {
}

void TwoWire::begin(uint8_t sda, uint8_t scl) {
    (void)sda;
    (void)scl;
}

void TwoWire::beginTransmission(uint8_t address) {
    (void)address;
}

uint8_t TwoWire::endTransmission(bool sendStop) {
    (void)sendStop;
    return 0;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity) {
    (void)address;
    (void)quantity;
    return 0;
}

size_t TwoWire::write(uint8_t data) {
    (void)data;
    return 1;
}

size_t TwoWire::write(const uint8_t* data, size_t length) {
    (void)data;
    (void)length;
    return length;
}

int TwoWire::available() {
    return 0;
}

int TwoWire::read() {
    return -1;
}

// Mock SPI
SPIClass SPI;

void SPIClass::begin() {
}

void SPIClass::beginTransaction(void* settings) {
    (void)settings;
}

void SPIClass::endTransaction() {
}

uint8_t SPIClass::transfer(uint8_t data) {
    (void)data;
    return 0;
}

void SPIClass::transfer(uint8_t* data, size_t length) {
    (void)data;
    (void)length;
}

#endif // ARDUINO_MOCK
