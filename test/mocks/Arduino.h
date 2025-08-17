#ifndef ARDUINO_H
#define ARDUINO_H

#ifdef ARDUINO_MOCK

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <functional>

// Mock Arduino types and constants
typedef uint8_t byte;
typedef uint16_t word;

// Mock pin modes
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define INPUT_PULLDOWN 3

// Mock pin levels
#define LOW 0
#define HIGH 1

// Mock time functions
unsigned long millis();
unsigned long micros();
void delay(unsigned long ms);
void delayMicroseconds(unsigned int us);

// Mock digital I/O
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);

// Mock analog I/O
int analogRead(uint8_t pin);
void analogWrite(uint8_t pin, int val);

// Mock interrupts
void attachInterrupt(uint8_t pin, void (*callback)(), int mode);
void detachInterrupt(uint8_t pin);

// Mock Serial
class SerialClass {
public:
    void begin(unsigned long baud);
    void print(const char* str);
    void println(const char* str);
    void println();
    int available();
    int read();
    void write(uint8_t val);
};

extern SerialClass Serial;

// Mock Wire (I2C)
class TwoWire {
public:
    void begin();
    void begin(uint8_t sda, uint8_t scl);
    void beginTransmission(uint8_t address);
    uint8_t endTransmission(bool sendStop = true);
    uint8_t requestFrom(uint8_t address, uint8_t quantity);
    size_t write(uint8_t data);
    size_t write(const uint8_t* data, size_t length);
    int available();
    int read();
};

extern TwoWire Wire;

// Mock SPI
class SPIClass {
public:
    void begin();
    void beginTransaction(void* settings);
    void endTransaction();
    uint8_t transfer(uint8_t data);
    void transfer(uint8_t* data, size_t length);
};

extern SPIClass SPI;

// Mock ESP32 specific includes
#include "esp_system.h"
#include "esp_sleep.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "driver/ledc.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "soc/soc_caps.h"

#endif // ARDUINO_MOCK

#endif // ARDUINO_H
