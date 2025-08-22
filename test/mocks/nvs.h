#ifndef NVS_H
#define NVS_H

#ifdef ARDUINO_MOCK

#include "esp_system.h"

// Mock NVS types and constants
typedef uint32_t nvs_handle_t;

typedef enum {
    NVS_OPEN_READONLY = 0,
    NVS_OPEN_READWRITE = 1
} nvs_open_mode_t;

#define NVS_READONLY 0
#define NVS_READWRITE 1

esp_err_t nvs_open(const char* name, nvs_open_mode_t open_mode, nvs_handle_t* out_handle);
esp_err_t nvs_close(nvs_handle_t handle);
esp_err_t nvs_get_u8(nvs_handle_t handle, const char* key, uint8_t* out_value);
esp_err_t nvs_set_u8(nvs_handle_t handle, const char* key, uint8_t value);
esp_err_t nvs_get_u16(nvs_handle_t handle, const char* key, uint16_t* out_value);
esp_err_t nvs_set_u16(nvs_handle_t handle, const char* key, uint16_t value);
esp_err_t nvs_get_u32(nvs_handle_t handle, const char* key, uint32_t* out_value);
esp_err_t nvs_set_u32(nvs_handle_t handle, const char* key, uint32_t value);
esp_err_t nvs_get_str(nvs_handle_t handle, const char* key, char* out_value, size_t* required_size);
esp_err_t nvs_set_str(nvs_handle_t handle, const char* key, const char* value);
esp_err_t nvs_commit(nvs_handle_t handle);
esp_err_t nvs_erase_key(nvs_handle_t handle, const char* key);

#endif // ARDUINO_MOCK

#endif // NVS_H
