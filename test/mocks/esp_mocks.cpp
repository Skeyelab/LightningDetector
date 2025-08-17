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
#include <cstdlib>

#ifdef ARDUINO_MOCK

// Mock NVS implementation
esp_err_t nvs_open(const char* name, nvs_open_mode_t open_mode, nvs_handle_t* out_handle) {
    (void)name;
    (void)open_mode;
    *out_handle = 1; // Mock handle
    return ESP_OK;
}

esp_err_t nvs_close(nvs_handle_t handle) {
    (void)handle;
    return ESP_OK;
}

esp_err_t nvs_get_u8(nvs_handle_t handle, const char* key, uint8_t* out_value) {
    (void)handle;
    (void)key;
    *out_value = 0; // Mock value
    return ESP_OK;
}

esp_err_t nvs_set_u8(nvs_handle_t handle, const char* key, uint8_t value) {
    (void)handle;
    (void)key;
    (void)value;
    return ESP_OK;
}

esp_err_t nvs_get_u16(nvs_handle_t handle, const char* key, uint16_t* out_value) {
    (void)handle;
    (void)key;
    *out_value = 0; // Mock value
    return ESP_OK;
}

esp_err_t nvs_set_u16(nvs_handle_t handle, const char* key, uint16_t value) {
    (void)handle;
    (void)key;
    (void)value;
    return ESP_OK;
}

esp_err_t nvs_get_u32(nvs_handle_t handle, const char* key, uint32_t* out_value) {
    (void)handle;
    (void)key;
    *out_value = 0; // Mock value
    return ESP_OK;
}

esp_err_t nvs_set_u32(nvs_handle_t handle, const char* key, uint32_t value) {
    (void)handle;
    (void)key;
    (void)value;
    return ESP_OK;
}

esp_err_t nvs_get_str(nvs_handle_t handle, const char* key, char* out_value, size_t* required_size) {
    (void)handle;
    (void)key;
    (void)out_value;
    *required_size = 0;
    return ESP_OK;
}

esp_err_t nvs_set_str(nvs_handle_t handle, const char* key, const char* value) {
    (void)handle;
    (void)key;
    (void)value;
    return ESP_OK;
}

esp_err_t nvs_commit(nvs_handle_t handle) {
    (void)handle;
    return ESP_OK;
}

esp_err_t nvs_erase_key(nvs_handle_t handle, const char* key) {
    (void)handle;
    (void)key;
    return ESP_OK;
}

// Mock NVS Flash implementation
esp_err_t nvs_flash_init() {
    return ESP_OK;
}

esp_err_t nvs_flash_erase() {
    return ESP_OK;
}

esp_err_t nvs_flash_deinit() {
    return ESP_OK;
}

// Mock FreeRTOS implementation
void vTaskDelay(TickType_t xTicksToDelay) {
    (void)xTicksToDelay;
}

void vTaskDelayUntil(TickType_t* pxPreviousWakeTime, TickType_t xTimeIncrement) {
    (void)pxPreviousWakeTime;
    (void)xTimeIncrement;
}

void vTaskDelete(void* pvTaskToDelete) {
    (void)pvTaskToDelete;
}

UBaseType_t uxTaskPriorityGet(void* xTask) {
    (void)xTask;
    return 1;
}

// Mock ESP Task WDT implementation
esp_err_t esp_task_wdt_init() {
    return ESP_OK;
}

esp_err_t esp_task_wdt_add(void* task_handle) {
    (void)task_handle;
    return ESP_OK;
}

esp_err_t esp_task_wdt_delete(void* task_handle) {
    (void)task_handle;
    return ESP_OK;
}

esp_err_t esp_task_wdt_reset() {
    return ESP_OK;
}

// Mock LEDC implementation
esp_err_t ledc_timer_config(const void* timer_conf) {
    (void)timer_conf;
    return ESP_OK;
}

esp_err_t ledc_channel_config(const void* channel_conf) {
    (void)channel_conf;
    return ESP_OK;
}

esp_err_t ledc_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty) {
    (void)speed_mode;
    (void)channel;
    (void)duty;
    return ESP_OK;
}

// Mock ADC implementation
esp_err_t adc1_config_width(int width) {
    (void)width;
    return ESP_OK;
}

esp_err_t adc1_config_channel_atten(int channel, adc_atten_t atten) {
    (void)channel;
    (void)atten;
    return ESP_OK;
}

int adc1_get_raw(int channel) {
    (void)channel;
    return rand() % 4096; // Mock 12-bit ADC value
}

// Mock ADC Calibration implementation
esp_err_t esp_adc_cal_characterize(uint32_t adc_num, uint32_t atten, uint32_t bit_width, uint32_t default_vref, esp_adc_cal_characteristics_t* chars) {
    (void)adc_num;
    (void)atten;
    (void)bit_width;
    (void)default_vref;
    (void)chars;
    return ESP_OK;
}

uint32_t esp_adc_cal_raw_to_voltage(uint32_t adc_reading, const esp_adc_cal_characteristics_t* chars) {
    (void)chars;
    // Mock voltage conversion (rough approximation)
    return adc_reading * 3300 / 4096; // Assuming 3.3V reference
}

// Mock ESP Sleep implementation
void esp_sleep_enable_ext0_wakeup(int gpio_num, int level) {
    (void)gpio_num;
    (void)level;
}

void esp_sleep_enable_ext1_wakeup(uint64_t mask, esp_sleep_source_t level) {
    (void)mask;
    (void)level;
}

void esp_sleep_enable_timer_wakeup(uint64_t time_in_us) {
    (void)time_in_us;
}

void esp_deep_sleep_start() {
}

void esp_deep_sleep(uint64_t time_in_us) {
    (void)time_in_us;
}

#endif // ARDUINO_MOCK
