#include "DataPacketHandler.h"
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct DataPacketHandler {

	uint16_t temperature;
	uint16_t ppm;
	uint16_t humidity;
	uint16_t dB;
}DataPacketHandler;

DataPacketHandler_t dph_create() {
	DataPacketHandler_t self = malloc(sizeof(DataPacketHandler));

	if (self == NULL) {
		return NULL;
	}

	//self->dummy = 0;
	self->temperature = 0;
	self->ppm = 0;
	self->humidity = 0;
	self->dB = 0;

	return self;
}

void dph_setCO2(uint16_t CO2ppm, DataPacketHandler_t self)
{
	self->ppm = CO2ppm;

}

void dph_setTemperature(uint16_t temperature, DataPacketHandler_t self)
{
	self->temperature = temperature;

}

void dph_setHumidity(uint16_t humidity, DataPacketHandler_t self)
{
	self->humidity = humidity;

}

void dph_setDb(uint16_t dB, DataPacketHandler_t self)
{
	self->dB = dB;

}


