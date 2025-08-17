#ifndef ESP_SYSTEM_H
#define ESP_SYSTEM_H

#ifdef ARDUINO_MOCK

#include <cstdint>
#include <cstddef>

// Mock ESP system types and constants
typedef int esp_err_t;

#define ESP_OK 0
#define ESP_ERR_NVS_NO_FREE_PAGES -1
#define ESP_ERR_NVS_NEW_VERSION_FOUND -2

// Mock error checking macro
#define ESP_ERROR_CHECK(x) do { \
    esp_err_t __; \
    if ((__ = x) != ESP_OK) { \
        /* Handle error */ \
    } \
} while(0)

#endif // ARDUINO_MOCK

#endif // ESP_SYSTEM_H
