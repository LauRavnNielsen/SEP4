#pragma once

#include "ATMEGA_FreeRTOS.h"
#include "event_groups.h"
#include "semphr.h"

#include <stdint.h>

typedef struct Temperature_Humidity* Temperature_Humidity_t;

Temperature_Humidity_t th_create(EventGroupHandle_t measureEventGroup, EventGroupHandle_t dataReadyEventGroup, SemaphoreHandle_t semaphore);
uint16_t th_getHum(Temperature_Humidity_t self);
uint16_t th_getTemp(Temperature_Humidity_t self);
void _th_task(void* pvParameters);