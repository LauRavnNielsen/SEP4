#pragma once

#include "ATMEGA_FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"

#include <stdint.h>

typedef struct CO2* CO2_t;

CO2_t co2_create(EventGroupHandle_t measureEventGroup, EventGroupHandle_t dataReadyEventGroup, SemaphoreHandle_t semaphore);
uint16_t co2_getData(CO2_t self);
void _co2_task(void* pvParameters);