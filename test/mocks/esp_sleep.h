#ifndef ESP_SLEEP_H
#define ESP_SLEEP_H

#ifdef ARDUINO_MOCK

#include <cstdint>

// Mock ESP sleep types and constants
typedef uint64_t esp_sleep_source_t;

#define ESP_SLEEP_WAKEUP_EXT0 0
#define ESP_SLEEP_WAKEUP_EXT1 1
#define ESP_SLEEP_WAKEUP_TIMER 2
#define ESP_SLEEP_WAKEUP_TOUCHPAD 3
#define ESP_SLEEP_WAKEUP_ULP 4

void esp_sleep_enable_ext0_wakeup(int gpio_num, int level);
void esp_sleep_enable_ext1_wakeup(uint64_t mask, esp_sleep_source_t level);
void esp_sleep_enable_timer_wakeup(uint64_t time_in_us);
void esp_deep_sleep_start();
void esp_deep_sleep(uint64_t time_in_us);

#endif // ARDUINO_MOCK

#endif // ESP_SLEEP_H
