#pragma once

#include "ATMEGA_FreeRTOS.h"
#include "event_groups.h"
#include "semphr.h"

typedef struct Sound* Sound_t;

Sound_t sound_create(EventGroupHandle_t measureEventGroup, EventGroupHandle_t dataReadyEventGroup, SemaphoreHandle_t semaphore);
uint16_t sound_getData(Sound_t self);
void _sound_task(void* pvParameters);