#pragma once

#include <stdint.h>

typedef struct CO2* CO2_t;

CO2_t co2_create();
uint16_t co2_getData(CO2_t self);
void co2_setData(CO2_t self);
void co2_measureData();