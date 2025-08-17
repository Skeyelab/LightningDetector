#ifndef TASK_H
#define TASK_H

#ifdef ARDUINO_MOCK

#include "FreeRTOS.h"

// Mock task functions
void vTaskDelete(void* pvTaskToDelete);
UBaseType_t uxTaskPriorityGet(void* xTask);

#endif // ARDUINO_MOCK

#endif // TASK_H
