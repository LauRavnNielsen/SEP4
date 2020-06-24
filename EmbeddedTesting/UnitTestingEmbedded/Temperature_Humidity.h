#pragma once

#include <stdint.h>

typedef struct Temperature_Humidity* Temperature_Humidity_t;

void th_initDriver();
Temperature_Humidity_t th_create();
uint16_t th_getHum(Temperature_Humidity_t self);
uint16_t th_getTemp(Temperature_Humidity_t self);
