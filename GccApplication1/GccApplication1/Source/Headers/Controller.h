#pragma once

#include "CO2.h"
#include "Sound.h"
#include "Temperature_Humidity.h"
#include "Servo.h"

#include "ATMEGA_FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

typedef struct Controller* Controller_t;

Controller_t controller_create(QueueHandle_t queue, EventGroupHandle_t measureEventGroup, EventGroupHandle_t dataReadyEventGroup, EventGroupHandle_t loraReadyEventGroup, SemaphoreHandle_t sensorSemaphore, CO2_t CO2_sensor, Temperature_Humidity_t Temp_sensor, Sound_t Sound_sensor, Servo_t servo);
void _controller_task(void *pvParameters);