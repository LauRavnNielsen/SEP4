#pragma once

#include "ATMEGA_FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"

typedef struct LoraUpLink* LoraUpLink_t;

LoraUpLink_t lora_createUpLink(EventGroupHandle_t loraReadyEventGroup, QueueHandle_t payloadQueue);
void _lora_uplink_task(void* pvParameters);
