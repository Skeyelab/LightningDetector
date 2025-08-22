#ifndef NVS_FLASH_H
#define NVS_FLASH_H

#ifdef ARDUINO_MOCK

#include "esp_system.h"

esp_err_t nvs_flash_init();
esp_err_t nvs_flash_erase();
esp_err_t nvs_flash_deinit();

#endif // ARDUINO_MOCK

#endif // NVS_FLASH_H
