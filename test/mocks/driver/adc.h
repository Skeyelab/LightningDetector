#ifndef ADC_H
#define ADC_H

#ifdef ARDUINO_MOCK

#include "esp_system.h"

// Mock ADC types and constants
typedef enum {
    ADC_ATTEN_0db = 0,
    ADC_ATTEN_2_5db = 1,
    ADC_ATTEN_6db = 2,
    ADC_ATTEN_11db = 3
} adc_atten_t;

esp_err_t adc1_config_width(int width);
esp_err_t adc1_config_channel_atten(int channel, adc_atten_t atten);
int adc1_get_raw(int channel);

#endif // ARDUINO_MOCK

#endif // ADC_H
