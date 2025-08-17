#ifndef LEDC_H
#define LEDC_H

#ifdef ARDUINO_MOCK

#include "esp_system.h"

// Mock LEDC types and constants
typedef enum {
    LEDC_LOW_SPEED_MODE = 0,
    LEDC_HIGH_SPEED_MODE = 1
} ledc_mode_t;

typedef enum {
    LEDC_CHANNEL_0 = 0,
    LEDC_CHANNEL_1 = 1,
    LEDC_CHANNEL_2 = 2,
    LEDC_CHANNEL_3 = 3,
    LEDC_CHANNEL_4 = 4,
    LEDC_CHANNEL_5 = 5,
    LEDC_CHANNEL_6 = 6,
    LEDC_CHANNEL_7 = 7
} ledc_channel_t;

esp_err_t ledc_timer_config(const void* timer_conf);
esp_err_t ledc_channel_config(const void* channel_conf);
esp_err_t ledc_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty);

#endif // ARDUINO_MOCK

#endif // LEDC_H
