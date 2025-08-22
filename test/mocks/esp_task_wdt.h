#ifndef ESP_TASK_WDT_H
#define ESP_TASK_WDT_H

#ifdef ARDUINO_MOCK

#include "esp_system.h"

esp_err_t esp_task_wdt_init();
esp_err_t esp_task_wdt_add(void* task_handle);
esp_err_t esp_task_wdt_delete(void* task_handle);
esp_err_t esp_task_wdt_reset();

#endif // ARDUINO_MOCK

#endif // ESP_TASK_WDT_H
