#ifndef ESP_ADC_CAL_H
#define ESP_ADC_CAL_H

#ifdef ARDUINO_MOCK

#include "esp_system.h"

// Mock ADC calibration types and constants
typedef struct {
    uint32_t adc_num;
    uint32_t atten;
    uint32_t vref;
    uint32_t low_curve;
    uint32_t high_curve;
} esp_adc_cal_characteristics_t;

esp_err_t esp_adc_cal_characterize(uint32_t adc_num, uint32_t atten, uint32_t bit_width, uint32_t default_vref, esp_adc_cal_characteristics_t* chars);
uint32_t esp_adc_cal_raw_to_voltage(uint32_t adc_reading, const esp_adc_cal_characteristics_t* chars);

#endif // ARDUINO_MOCK

#endif // ESP_ADC_CAL_H
