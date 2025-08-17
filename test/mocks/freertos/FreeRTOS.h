#ifndef FREERTOS_H
#define FREERTOS_H

#ifdef ARDUINO_MOCK

#include <cstdint>

// Mock FreeRTOS types and constants
typedef uint32_t TickType_t;
typedef uint32_t UBaseType_t;
typedef int32_t BaseType_t;

#define pdTRUE 1
#define pdFALSE 0
#define portMAX_DELAY 0xFFFFFFFF

// Mock task functions
void vTaskDelay(TickType_t xTicksToDelay);
void vTaskDelayUntil(TickType_t* pxPreviousWakeTime, TickType_t xTimeIncrement);

#endif // ARDUINO_MOCK

#endif // FREERTOS_H
