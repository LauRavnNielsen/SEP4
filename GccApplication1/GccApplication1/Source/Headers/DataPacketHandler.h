#pragma once

#include <lora_driver.h>

typedef struct DataPacketHandler* DataPacketHandler_t;

DataPacketHandler_t dph_create();
void dph_setCO2 (uint16_t CO2ppm, DataPacketHandler_t self);
void dph_setTemperature (uint16_t temperature, DataPacketHandler_t self);
void dph_setHumidity (uint16_t humidity, DataPacketHandler_t self);
void dph_setDb (uint16_t dB, DataPacketHandler_t self);
lora_payload_t dph_assemble(DataPacketHandler_t self);